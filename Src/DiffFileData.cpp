/** 
 * @file DiffFileData.cpp
 *
 * @brief Code for DiffFileData class
 *
 * @date  Created: 2003-08-22
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "io.h"
#include "DiffItem.h"
#include "FileLocation.h"
#include "Diff.h"
#include "IAbortable.h"
#include "Paths.h"
#include "DiffContext.h"
#include "FileTransform.h"
#include "common/unicoder.h"
#include "DiffWrapper.h"
#include "ByteComparator.h"
#include "codepage_detect.h"
#include <shlwapi.h>
#include "DiffFileData.h"

static const int KILO = 1024; // Kilo(byte)

/** @brief Quick contents compare's file buffer size. */
static const int WMCMPBUFF = 32 * KILO;

static void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, CString & left, CString & right);
static bool Unpack(CString & filepathTransformed,
	const CString & filteredFilenames, PackingInfo * infoUnpacker);
static void CopyTextStats(const file_data * inf, FileTextStats * myTextStats);
static void CopyDiffutilTextStats(file_data *inf, DiffFileData * diffData);

static int f_defcp = 0; // default codepage


struct FileHandle
{
	FileHandle() : m_fp(0) { }
	void Assign(FILE * fp) { Close(); m_fp = fp; }
	void Close() { if (m_fp) { fclose(m_fp); m_fp = 0; } }
	~FileHandle() { Close(); }
	FILE * m_fp;
};

/**
 * @brief Simple initialization of DiffFileData
 * @note Diffcounts are initialized to invalid values, not zeros.
 */
DiffFileData::DiffFileData()
{
	m_inf = new file_data[2];
	int i=0;
	for (i=0; i<2; ++i)
		memset(&m_inf[i], 0, sizeof(m_inf[i]));
	m_used = false;
	m_ndiffs = DiffFileData::DIFFS_UNKNOWN;
	m_ntrivialdiffs = DiffFileData::DIFFS_UNKNOWN;
	Reset();
	// Set default codepages
	for (i=0; i<sizeof(m_FileLocation)/sizeof(m_FileLocation[0]); ++i)
	{
		m_FileLocation[i].encoding.SetCodepage(f_defcp);
	}
}

/** @brief deallocate member data */
DiffFileData::~DiffFileData()
{
	Reset();
	delete [] m_inf;
}

/** @brief Allow caller to specify codepage to assume for all unknown files */
void DiffFileData::SetDefaultCodepage(int defcp)
{
	f_defcp = defcp;
}

/** @brief Open file descriptors in the inf structure (return false if failure) */
bool DiffFileData::OpenFiles(LPCTSTR szFilepath1, LPCTSTR szFilepath2)
{
	m_FileLocation[0].setPath(szFilepath1);
	m_FileLocation[1].setPath(szFilepath2);
	bool b = DoOpenFiles();
	if (!b)
		Reset();
	return b;
}

/** @brief stash away true names for display, before opening files */
void DiffFileData::SetDisplayFilepaths(LPCTSTR szTrueFilepath1, LPCTSTR szTrueFilepath2)
{
	m_sDisplayFilepath[0] = szTrueFilepath1;
	m_sDisplayFilepath[1] = szTrueFilepath2;
}


/** @brief Open file descriptors in the inf structure (return false if failure) */
bool DiffFileData::DoOpenFiles()
{
	Reset();

	for (int i=0; i<2; ++i)
	{
		// Fill in 8-bit versions of names for diffutils (WinMerge doesn't use these)
		USES_CONVERSION;
		// Actual paths are m_FileLocation[i].filepath
		// but these are often temporary files
		// Displayable (original) paths are m_sDisplayFilepath[i]
		m_inf[i].name = strdup(T2CA(m_sDisplayFilepath[i]));
		if (m_inf[i].name == NULL)
			return false;

		// Open up file descriptors
		// Always use O_BINARY mode, to avoid terminating file read on ctrl-Z (DOS EOF)
		// Also, WinMerge-modified diffutils handles all three major eol styles
		if (m_inf[i].desc == 0)
		{
			m_inf[i].desc = _topen(m_FileLocation[i].filepath, O_RDONLY|O_BINARY, _S_IREAD);
		}
		if (m_inf[i].desc < 0)
			return false;

		// Get file stats (diffutils uses these)
		if (fstat(m_inf[i].desc, &m_inf[i].stat) != 0)
		{
			return false;
		}
		if (0 == m_FileLocation[1].filepath.CompareNoCase(m_FileLocation[0].filepath))
		{
			m_inf[1].desc = m_inf[0].desc;
		}
	}

	m_used = true;
	return true;
}

/** @brief Clear inf structure to pristine */
void DiffFileData::Reset()
{
	ASSERT(m_inf);
	// If diffutils put data in, have it cleanup
	if (m_used)
	{
		cleanup_file_buffers(m_inf);
		m_used = false;
	}
	// clean up any open file handles, and zero stuff out
	// open file handles might be leftover from a failure in DiffFileData::OpenFiles
	for (int i=0; i<2; ++i)
	{
		if (m_inf[1].desc == m_inf[0].desc)
		{
			m_inf[1].desc = 0;
		}
		free((void *)m_inf[i].name);
		m_inf[i].name = NULL;

		if (m_inf[i].desc > 0)
		{
			close(m_inf[i].desc);
		}
		m_inf[i].desc = 0;
		memset(&m_inf[i], 0, sizeof(m_inf[i]));
	}
}

/**
 * @brief Try to deduce encoding for this file (given copy in memory)
 */
void DiffFileData::GuessEncoding_from_buffer(FileLocation & fpenc, const char **data, int count)
{
	if (!fpenc.encoding.m_bom)
	{
		CString sExt = PathFindExtension(fpenc.filepath);
		GuessEncoding_from_bytes(sExt, data, count, &fpenc.encoding);
	}
}

/** @brief Guess encoding for one file (in DiffContext memory buffer) */
void DiffFileData::GuessEncoding_from_buffer_in_DiffContext(int side, CDiffContext * pCtxt)
{
	GuessEncoding_from_buffer(m_FileLocation[side], m_inf[side].linbuf + m_inf[side].linbuf_base, 
	                                m_inf[side].valid_lines - m_inf[side].linbuf_base);
}

/** @brief Guess encoding for one file (in DiffContext memory buffer) */
void DiffFileData::GuessEncoding_from_FileLocation(FileLocation & fpenc)
{
	if (!fpenc.encoding.m_bom)
	{
		BOOL bGuess = TRUE;
		GuessCodepageEncoding(fpenc.filepath, &fpenc.encoding, bGuess);
	}
}

/**
 * @brief Compare two specified files.
 *
 * @param [in] depth Current directory depth.
 * @return Compare result as DIFFCODE.
 */
int DiffFileData::diffutils_compare_files(int depth)
{
	int bin_flag = 0;
	int bin_file = 0; // bitmap for binary files

	// Do the actual comparison (generating a change script)
	struct change *script = NULL;
	BOOL success = Diff2Files(&script, depth, &bin_flag, FALSE, &bin_file);
	if (!success)
	{
		return DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;
	}
	int code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::SAME;

	// make sure to start counting diffs at 0
	// (usually it is -1 at this point, for unknown)
	m_ndiffs = 0;
	m_ntrivialdiffs = 0;

	// Free change script (which we don't want)
	if (script != NULL)
	{
		struct change *p,*e;
		for (e = script; e; e = p)
		{
			if (!e->trivial)
				++m_ndiffs;
			else
				++m_ntrivialdiffs;
			p = e->link;
			free (e);
		}
		if (m_ndiffs > 0)
			code = code & ~DIFFCODE::SAME | DIFFCODE::DIFF;
	}

	// diff_2_files set bin_flag to -1 if different binary
	// diff_2_files set bin_flag to +1 if same binary

	if (bin_flag != 0)
	{
		// Clear text-flag, set binary flag
		// We don't know diff counts for binary files
		code = code & ~DIFFCODE::TEXT;
		switch (bin_file)
		{
		case BINFILE_SIDE1: code |= DIFFCODE::BINSIDE1;
			break;
		case BINFILE_SIDE2: code |= DIFFCODE::BINSIDE2;
			break;
		case BINFILE_SIDE1 | BINFILE_SIDE2: code |= DIFFCODE::BIN;
			break;
		default:
			_RPTF1(_CRT_ERROR, "Invalid bin_file value: %d", bin_file);
			break;
		}
		m_ndiffs = DiffFileData::DIFFS_UNKNOWN;
	}

	if (bin_flag < 0)
	{
		// Clear same-flag, set diff-flag
		code = code & ~DIFFCODE::SAME | DIFFCODE::DIFF;
	}

	return code;
}

/** @brief detect unicode file and quess encoding */
DiffFileData::UniFileBom::UniFileBom(int fd)
{
	size = 0;
	unicoding = ucr::NONE;
	if (fd == -1) 
		return;
	long tmp = _lseek(fd, 0, SEEK_SET);
	switch (_read(fd, buffer, 3))
	{
		case 3:
			if (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF)
			{
				size = 3;
				unicoding = ucr::UTF8;
				break;
			}
			// fall through & try the 2-byte BOMs
		case 2:
			if (buffer[0] == 0xFF && buffer[1] == 0xFE)
			{
				size = 2;
				unicoding = ucr::UCS2LE;
				break;
			}
			if (buffer[0] == 0xFE && buffer[1] == 0xFF)
			{
				size = 2;
				unicoding = ucr::UCS2BE;
				break;
			}
		default:
			size = 0;
			unicoding = ucr::NONE;
	}
	_lseek(fd, tmp, SEEK_SET);
}

/**
 * @brief Invoke appropriate plugins for prediffing
 * return false if anything fails
 * caller has to DeleteFile filepathTransformed, if it differs from filepath
 */
bool DiffFileData::Filepath_Transform(FileLocation & fpenc, const CString & filepath, CString & filepathTransformed,
	const CString & filteredFilenames, PrediffingInfo * infoPrediffer, int fd)
{
	BOOL bMayOverwrite = FALSE; // temp variable set each time it is used

	// Read BOM to check for Unicode
	UniFileBom bom = fd;
	if (bom.unicoding)
		fpenc.encoding.SetUnicoding(bom.unicoding);

	if (fpenc.encoding.m_unicoding && fpenc.encoding.m_unicoding != ucr::UCS2LE)
	{
		// second step : normalize Unicode to OLECHAR (most of time, do nothing) (OLECHAR = UCS-2LE in Windows)
		bMayOverwrite = (filepathTransformed != filepath); // may overwrite if we've already copied to temp file
		if (!FileTransform_NormalizeUnicode(filepathTransformed, bMayOverwrite))
			return false;
	}

	// Note: filepathTransformed may be in UCS-2 (if toUtf8), or it may be raw encoding (if !Utf8)
	// prediff plugins must handle both

	// third step : prediff (plugins)
	bMayOverwrite = (filepathTransformed != filepath); // may overwrite if we've already copied to temp file
	if (infoPrediffer->bToBeScanned)
	{
		// FileTransform_Prediffing tries each prediffer for the pointed out filteredFilenames
		// if a prediffer fails, we consider it is not the good one, that's all
		// FileTransform_Prediffing returns FALSE only if the prediffer works, 
		// but the data can not be saved to disk (no more place ??)
		if (FileTransform_Prediffing(filepathTransformed, filteredFilenames, infoPrediffer, bMayOverwrite) 
				== FALSE)
			return false;
	}
	else
	{
		// this can failed if the pointed out prediffer has a problem
		if (FileTransform_Prediffing(filepathTransformed, *infoPrediffer, bMayOverwrite) 
				== FALSE)
			return false;
	}

	if (fpenc.encoding.m_unicoding)
	{
		// fourth step : prepare for diffing
		// may overwrite if we've already copied to temp file
		BOOL bMayOverwrite = (0 != filepathTransformed.CompareNoCase(filepath));
		if (!FileTransform_UCS2ToUTF8(filepathTransformed, bMayOverwrite))
			return false;
	}
	return true;
}

/**
 * @brief Prepare files (run plugins) & compare them, and return diffcode.
 * This is function to compare two files in folder compare. It is not used in
 * file compare.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in, out] di Compared files with associated data.
 * @return Compare result code.
 */
int DiffFileData::prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di)
{
	int nCompMethod = pCtxt->m_nCompMethod;
	CString filepath1;
	CString filepath2;
	GetComparePaths(pCtxt, di, filepath1, filepath2);

	// Reset text stats
	m_textStats0.clear();
	m_textStats1.clear();

	int code = DIFFCODE::FILE | DIFFCODE::CMPERR;
	// For user chosen plugins, define bAutomaticUnpacker as false and use the chosen infoHandler
	// but how can we receive the infoHandler ? DirScan actually only 
	// returns info, but can not use file dependent information.

	// Transformation happens here
	// text used for automatic mode : plugin filter must match it
	CString filteredFilenames = filepath1 + "|" + filepath2;

	PackingInfo * infoUnpacker=0;
	PrediffingInfo * infoPrediffer=0;

	// Get existing or new plugin infos
	pCtxt->FetchPluginInfos(filteredFilenames, &infoUnpacker, &infoPrediffer);

	// plugin may alter filepaths to temp copies (which we delete before returning in all cases)
	CString filepathUnpacked1 = filepath1;
	CString filepathUnpacked2 = filepath2;

	CString filepathTransformed1;
	CString filepathTransformed2;

	//DiffFileData diffdata; //(filepathTransformed1, filepathTransformed2);
	// Invoke unpacking plugins
	if (!Unpack(filepathUnpacked1, filteredFilenames, infoUnpacker))
	{
		di.errorDesc = _T("Unpack Error Side 1");
		goto exitPrepAndCompare;
	}

	// we use the same plugins for both files, so they must be defined before second file
	ASSERT(infoUnpacker->bToBeScanned == FALSE);

	if (!Unpack(filepathUnpacked2, filteredFilenames, infoUnpacker))
	{
		di.errorDesc = _T("Unpack Error Side 2");
		goto exitPrepAndCompare;
	}

	// As we keep handles open on unpacked files, Transform() may not delete them.
	// Unpacked files will be deleted at end of this function.
	filepathTransformed1 = filepathUnpacked1;
	filepathTransformed2 = filepathUnpacked2;
	SetDisplayFilepaths(filepath1, filepath2); // store true names for diff utils patch file
	if (!OpenFiles(filepathTransformed1, filepathTransformed2))
	{
		di.errorDesc = _T("OpenFiles Error (before tranform)");
		goto exitPrepAndCompare;
	}

	// Invoke prediff'ing plugins
	if (!Filepath_Transform(m_FileLocation[0], filepathUnpacked1, filepathTransformed1, filteredFilenames, infoPrediffer, m_inf[0].desc))
	{
		di.errorDesc = _T("Transform Error Side 1");
		goto exitPrepAndCompare;
	}

	// we use the same plugins for both files, so they must be defined before second file
	ASSERT(infoPrediffer->bToBeScanned == FALSE);

	if (!Filepath_Transform(m_FileLocation[1], filepathUnpacked2, filepathTransformed2, filteredFilenames, infoPrediffer, m_inf[1].desc))
	{
		di.errorDesc = _T("Transform Error Side 2");
		goto exitPrepAndCompare;
	}

	// If options are binary equivalent, we could check for filesize
	// difference here, and bail out if files are clearly different
	// But, then we don't know if file is ascii or binary, and this
	// affects behavior (also, we don't have an icon for unknown type)

	// Actually compare the files
	// diffutils_compare_files is a fairly thin front-end to diffutils
	if (filepathTransformed1 != filepathUnpacked1 || filepathTransformed2 != filepathUnpacked2)
	{
		//diffdata.m_sFilepath[0] = filepathTransformed1;
		//diffdata.m_sFilepath[1] = filepathTransformed2;
		if (!OpenFiles(filepathTransformed1, filepathTransformed2))
		{
			di.errorDesc = _T("OpenFiles Error (after tranform)");
			goto exitPrepAndCompare;
		}
	}

	// If either file is larger than limit compare files by quick contents
	// This allows us to (faster) compare big binary files
	if (pCtxt->m_nCompMethod == CMP_CONTENT && 
		(di.left.size > pCtxt->m_nQuickCompareLimit ||
		di.right.size > pCtxt->m_nQuickCompareLimit))
	{
		nCompMethod = CMP_QUICK_CONTENT;
	}

	if (nCompMethod == CMP_CONTENT)
	{
		// use diffutils
		code = diffutils_compare_files(0);
		// If unique item, it was being compared to itself to determine encoding
		// and the #diffs is invalid
		if (di.isSideRight() || di.isSideLeft())
		{
			m_ndiffs = DiffFileData::DIFFS_UNKNOWN;
			m_ntrivialdiffs = DiffFileData::DIFFS_UNKNOWN;
		}
		if (DIFFCODE::isResultError(code))
			di.errorDesc = _T("DiffUtils Error");

		if (!DIFFCODE::isResultError(code) && pCtxt->m_bGuessEncoding)
		{
			// entire file is in memory in the diffutils buffers
			// inside the diff context, so may as well use in-memory copy
			GuessEncoding_from_buffer_in_DiffContext(0, pCtxt);
			GuessEncoding_from_buffer_in_DiffContext(1, pCtxt);
		}
	}
	else if (nCompMethod == CMP_QUICK_CONTENT)
	{
		// use our own byte-by-byte compare
		code = byte_compare_files(pCtxt->m_bStopAfterFirstDiff, pCtxt->GetAbortable());
		// Quick contents doesn't know about diff counts
		// Set to special value to indicate invalid
		m_ndiffs = DIFFS_UNKNOWN_QUICKCOMPARE;
		m_ntrivialdiffs = DIFFS_UNKNOWN_QUICKCOMPARE;
		di.left.m_textStats = m_textStats0;
		di.right.m_textStats = m_textStats1;

		if (!DIFFCODE::isResultError(code) && pCtxt->m_bGuessEncoding)
		{
			GuessEncoding_from_FileLocation(m_FileLocation[0]);
			GuessEncoding_from_FileLocation(m_FileLocation[1]);
		}
	}
	else
	{
		// Print error since we should have handled by date compare earlier
		_RPTF0(_CRT_ERROR, "Invalid compare type, DiffFileData can't handle it");
		di.errorDesc = _T("Bad compare type");
		goto exitPrepAndCompare;
	}


exitPrepAndCompare:
	Reset();
	// delete the temp files after comparison
	if (filepathTransformed1 != filepathUnpacked1)
		VERIFY(::DeleteFile(filepathTransformed1) || gLog::DeleteFileFailed(filepathTransformed1));
	if (filepathTransformed2 != filepathUnpacked2)
		VERIFY(::DeleteFile(filepathTransformed2) || gLog::DeleteFileFailed(filepathTransformed2));
	if (filepathUnpacked1 != filepath1)
		VERIFY(::DeleteFile(filepathUnpacked1) || gLog::DeleteFileFailed(filepathUnpacked1));
	if (filepathUnpacked2 != filepath2)
		VERIFY(::DeleteFile(filepathUnpacked2) || gLog::DeleteFileFailed(filepathUnpacked2));
	return code;
}

/**
 * @brief Compare two files using diffutils.
 *
 * Compare two files (in DiffFileData param) using diffutils. Run diffutils
 * inside SEH so we can trap possible error and exceptions. If error or
 * execption is trapped, return compare failure.
 * @param [out] diffs Pointer to list of change structs where diffdata is stored.
 * @param [in] depth Depth in folder compare (we use 0).
 * @param [out] bin_status used to return binary status from compare.
 * @param [in] bMovedBlocks If TRUE moved blocks are analyzed.
 * @param [out] bin_file Returns which file was binary file as bitmap.
    So if first file is binary, first bit is set etc. Can be NULL if binary file
    info is not needed (faster compare since diffutils don't bother checking
    second file if first is binary).
 * @return TRUE when compare succeeds, FALSE if error happened during compare.
 */
BOOL DiffFileData::Diff2Files(struct change ** diffs, int depth,
	int * bin_status, BOOL bMovedBlocks, int * bin_file)
{
	BOOL bRet = TRUE;
	__try
	{
		*diffs = diff_2_files (m_inf, depth, bin_status, bMovedBlocks, bin_file);
		CopyDiffutilTextStats(m_inf, this);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*diffs = NULL;
		bRet = FALSE;
	}
	return bRet;
}

/** 
 * @brief Compare two specified files, byte-by-byte
 * @param [in] bStopAfterFirstDiff Stop compare after we find first difference?
 * @param [in] piAbortable Interface allowing to abort compare
 * @return DIFFCODE
 */
int DiffFileData::byte_compare_files(BOOL bStopAfterFirstDiff, const IAbortable * piAbortable)
{
	// Close any descriptors open for diffutils
	Reset();

	// TODO
	// Right now, we assume files are in 8-bit encoding
	// because transform code converted any UCS-2 files to UTF-8
	// We could compare directly in UCS-2LE here, as an optimization, in that case
	char buff[2][WMCMPBUFF]; // buffered access to files
	FILE * fp[2]; // for files to compare
	FileHandle fhd[2]; // to ensure file handles fp get closed
	int i;
	int diffcode = 0;

	// Open both files
	for (i=0; i<2; ++i)
	{
		fp[i] = _tfopen(m_FileLocation[i].filepath, _T("rb"));
		if (!fp[i])
			return DIFFCODE::CMPERR;
		fhd[i].Assign(fp[i]);
	}

	// area of buffer currently holding data
	__int64 bfstart[2]; // offset into buff[i] where current data resides
	__int64 bfend[2]; // past-the-end pointer into buff[i], giving end of current data
	// buff[0] has bytes to process from buff[0][bfstart[0]] to buff[0][bfend[0]-1]

	bool eof[2]; // if we've finished file

	// initialize our buffer pointers and end of file flags
	for (i=0; i<2; ++i)
	{
		bfstart[i] = bfend[i] = 0;
		eof[i] = false;
	}

	ByteComparator comparator(ignore_case_flag, ignore_space_change_flag
		, ignore_all_space_flag, ignore_eol_diff, ignore_blank_lines_flag);

	// Begin loop
	// we handle the files in WMCMPBUFF sized buffers (variable buff[][])
	// That is, we do one buffer full at a time
	// or even less, as we process until one side buffer is empty, then reload that one
	// and continue
	while (!eof[0] || !eof[1])
	{
		if (piAbortable && piAbortable->ShouldAbort())
			return DIFFCODE::CMPABORT;

		// load or update buffers as appropriate
		for (i=0; i<2; ++i)
		{
			if (!eof[i] && bfstart[i]==countof(buff[i]))
			{
				bfstart[i]=bfend[i] = 0;
			}
			if (!eof[i] && bfend[i]<countof(buff[i])-1)
			{
				// Assume our blocks are in range of unsigned int
				unsigned int space = countof(buff[i]) - bfend[i];
				size_t rtn = fread(&buff[i][bfend[i]], 1, space, fp[i]);
				if (ferror(fp[i]))
					return DIFFCODE::CMPERR;
				if (feof(fp[i]))
					eof[i] = true;
				bfend[i] += rtn;
			}
		}

		// where to start comparing right now
		LPCSTR ptr0 = &buff[0][bfstart[0]];
		LPCSTR ptr1 = &buff[1][bfstart[1]];

		// remember where we started
		LPCSTR orig0 = ptr0, orig1 = ptr1;

		// how far can we go right now?
		LPCSTR end0 = &buff[0][bfend[0]];
		LPCSTR end1 = &buff[1][bfend[1]];

		__int64 offset0 = (ptr0 - &buff[0][0]);
		__int64 offset1 = (ptr1 - &buff[1][0]);

		// are these two buffers the same?
		if (!comparator.CompareBuffers(m_textStats0, m_textStats1, 
			ptr0, ptr1, end0, end1, eof[0], eof[1], offset0, offset1))
		{
			if (bStopAfterFirstDiff)
			{
				// By bailing out here
				// we leave our text statistics incomplete
				return diffcode | DIFFCODE::DIFF;
			}
			else
			{
				diffcode |= DIFFCODE::DIFF;
				ptr0 = end0;
				ptr1 = end1;
			}
		}
		else
		{
			ptr0 = end0;
			ptr1 = end1;
		}


		// did we finish both files?
		if (eof[0] && eof[1])
		{

			BOOL bBin0 = (m_textStats0.nzeros>0);
			BOOL bBin1 = (m_textStats1.nzeros>0);

			if (bBin0 && bBin1)
				diffcode |= DIFFCODE::BIN;
			else if (bBin0)
				diffcode |= DIFFCODE::BINSIDE1;
			else if (bBin1)
				diffcode |= DIFFCODE::BINSIDE2;

			// If either unfinished, they differ
			if (ptr0 != end0 || ptr1 != end1)
				diffcode = (diffcode & DIFFCODE::DIFF);
			
			if (diffcode & DIFFCODE::DIFF)
				return diffcode | DIFFCODE::DIFF;
			else
				return diffcode | DIFFCODE::SAME;
		}

		// move our current pointers over what we just compared
		ASSERT(ptr0 >= orig0);
		ASSERT(ptr1 >= orig1);
		bfstart[0] += ptr0-orig0;
		bfstart[1] += ptr1-orig1;
	}
	return diffcode;
}

/**
 * @brief Get actual compared paths from DIFFITEM.
 * @note If item is unique, same path is returned for both.
 */
void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, CString & left, CString & right)
{
	static const TCHAR backslash[] = _T("\\");

	if (!di.isSideRight())
	{
		// Compare file to itself to detect encoding
		left = pCtxt->GetNormalizedLeft();
		if (!paths_EndsWithSlash(left))
			left += backslash;
		if (!di.sLeftSubdir.IsEmpty())
			left += di.sLeftSubdir + backslash;
		left += di.sLeftFilename;
		if (di.isSideLeft())
			right = left;
	}
	if (!di.isSideLeft())
	{
		// Compare file to itself to detect encoding
		right = pCtxt->GetNormalizedRight();
		if (!paths_EndsWithSlash(right))
			right += backslash;
		if (!di.sRightSubdir.IsEmpty())
			right += di.sRightSubdir + backslash;
		right += di.sRightFilename;
		if (di.isSideRight())
			left = right;
	}
}

/**
 * @brief Invoke appropriate plugins for unpacking
 * return false if anything fails
 * caller has to DeleteFile filepathTransformed, if it differs from filepath
 */
static bool Unpack(CString & filepathTransformed,
	const CString & filteredFilenames, PackingInfo * infoUnpacker)
{
	// first step : unpack (plugins)
	if (infoUnpacker->bToBeScanned)
	{
		if (!FileTransform_Unpacking(filepathTransformed, filteredFilenames, infoUnpacker, &infoUnpacker->subcode))
			return false;
	}
	else
	{
		if (!FileTransform_Unpacking(filepathTransformed, infoUnpacker, &infoUnpacker->subcode))
			return false;
	}
	return true;
}

/**
 * @brief Copy text stat results from diffutils back into the FileTextStats structure
 */
static void CopyTextStats(const file_data * inf, FileTextStats * myTextStats)
{
	myTextStats->ncrlfs = inf->count_crlfs;
	myTextStats->ncrs = inf->count_crs;
	myTextStats->nlfs = inf->count_lfs;
}

/**
 * @brief Copy both left & right text stats results back into the DiffFileData text stats
 */
static void CopyDiffutilTextStats(file_data *inf, DiffFileData * diffData)
{
	CopyTextStats(&inf[0], &diffData->m_textStats0);
	CopyTextStats(&inf[1], &diffData->m_textStats1);
}

