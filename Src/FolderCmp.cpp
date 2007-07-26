/** 
 * @file  FolderCmp.cpp
 *
 * @brief Implementation file for FolderCmp
 */
// ID line follows -- this is updated by SVN
// $Id$


#include "stdafx.h"
#include "LogFile.h"
#include "Merge.h"
#include "paths.h"
#include "FilterList.h"
#include "DiffContext.h"
#include "DiffWrapper.h"
#include "FileTransform.h"
#include "diff.h"
#include "IAbortable.h"
#include "FolderCmp.h"
#include "ByteComparator.h"

static const int KILO = 1024; // Kilo(byte)

/** @brief Quick contents compare's file buffer size. */
static const int WMCMPBUFF = 32 * KILO;

static void CopyDiffutilTextStats(file_data *inf, DiffFileData * diffData);
static void CopyTextStats(const file_data * inf, FileTextStats * myTextStats);
static void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, CString & left, CString & right);
static bool Unpack(CString & filepathTransformed,
	const CString & filteredFilenames, PackingInfo * infoUnpacker);

FolderCmp::FolderCmp()
: m_ndiffs(DIFFS_UNKNOWN)
, m_ntrivialdiffs(DIFFS_UNKNOWN)
{

}

/**
 * @brief Prepare files (run plugins) & compare them, and return diffcode.
 * This is function to compare two files in folder compare. It is not used in
 * file compare.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in, out] di Compared files with associated data.
 * @return Compare result code.
 */
int FolderCmp::prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di)
{
	int nCompMethod = pCtxt->m_nCompMethod;
	CString filepath1;
	CString filepath2;
	GetComparePaths(pCtxt, di, filepath1, filepath2);
	m_pCtx = pCtxt;

	// Reset text stats
	m_diffFileData.m_textStats0.clear();
	m_diffFileData.m_textStats1.clear();

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
	m_diffFileData.SetDisplayFilepaths(filepath1, filepath2); // store true names for diff utils patch file
	if (!m_diffFileData.OpenFiles(filepathTransformed1, filepathTransformed2))
	{
		di.errorDesc = _T("OpenFiles Error (before tranform)");
		goto exitPrepAndCompare;
	}

	// Invoke prediff'ing plugins
	if (!m_diffFileData.Filepath_Transform(m_diffFileData.m_FileLocation[0], filepathUnpacked1,
			filepathTransformed1, filteredFilenames, infoPrediffer,
			m_diffFileData.m_inf[0].desc))
	{
		di.errorDesc = _T("Transform Error Side 1");
		goto exitPrepAndCompare;
	}

	// we use the same plugins for both files, so they must be defined before second file
	ASSERT(infoPrediffer->bToBeScanned == FALSE);

	if (!m_diffFileData.Filepath_Transform(m_diffFileData.m_FileLocation[1], filepathUnpacked2,
			filepathTransformed2, filteredFilenames, infoPrediffer,
			m_diffFileData.m_inf[1].desc))
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
		if (!m_diffFileData.OpenFiles(filepathTransformed1, filepathTransformed2))
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
		if (di.isSideRightOnly() || di.isSideLeftOnly())
		{
			m_ndiffs = DIFFS_UNKNOWN;
			m_ntrivialdiffs = DIFFS_UNKNOWN;
		}
		if (DIFFCODE::isResultError(code))
			di.errorDesc = _T("DiffUtils Error");

		if (!DIFFCODE::isResultError(code) && pCtxt->m_bGuessEncoding)
		{
			// entire file is in memory in the diffutils buffers
			// inside the diff context, so may as well use in-memory copy
			m_diffFileData.GuessEncoding_from_buffer_in_DiffContext(0, pCtxt);
			m_diffFileData.GuessEncoding_from_buffer_in_DiffContext(1, pCtxt);
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
		di.left.m_textStats = m_diffFileData.m_textStats0;
		di.right.m_textStats = m_diffFileData.m_textStats1;

		if (!DIFFCODE::isResultError(code) && pCtxt->m_bGuessEncoding)
		{
			m_diffFileData.GuessEncoding_from_FileLocation(m_diffFileData.m_FileLocation[0]);
			m_diffFileData.GuessEncoding_from_FileLocation(m_diffFileData.m_FileLocation[1]);
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
	m_diffFileData.Reset();
	// delete the temp files after comparison
	if (filepathTransformed1 != filepathUnpacked1)
		VERIFY(::DeleteFile(filepathTransformed1) || GetLog()->DeleteFileFailed(filepathTransformed1));
	if (filepathTransformed2 != filepathUnpacked2)
		VERIFY(::DeleteFile(filepathTransformed2) || GetLog()->DeleteFileFailed(filepathTransformed2));
	if (filepathUnpacked1 != filepath1)
		VERIFY(::DeleteFile(filepathUnpacked1) || GetLog()->DeleteFileFailed(filepathUnpacked1));
	if (filepathUnpacked2 != filepath2)
		VERIFY(::DeleteFile(filepathUnpacked2) || GetLog()->DeleteFileFailed(filepathUnpacked2));
	return code;
}


/**
 * @brief Compare two specified files.
 *
 * @param [in] depth Current directory depth.
 * @return Compare result as DIFFCODE.
 */
int FolderCmp::diffutils_compare_files(int depth)
{
	int bin_flag = 0;
	int bin_file = 0; // bitmap for binary files

	DiffutilsOptions *pOptions = 
		dynamic_cast<DiffutilsOptions *>(m_pCtx->GetCompareOptions(CMP_CONTENT));
	if (pOptions == NULL)
		return DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;
	
	pOptions->SetToDiffUtils();

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

	if (script && m_pCtx->m_pFilterList && m_pCtx->m_pFilterList->HasRegExps())
	{
		struct change *next = script;
		struct change *thisob=0, *end=0;
		
		while (next)
		{
			/* Find a set of changes that belong together.  */
			thisob = next;
			end = find_change(next);
			
			/* Disconnect them from the rest of the changes,
			making them a hunk, and remember the rest for next iteration.  */
			next = end->link;
			end->link = 0;
#ifdef DEBUG
			debug_script(thisob);
#endif

			{					
				/* Determine range of line numbers involved in each file.  */
				int first0=0, last0=0, first1=0, last1=0, deletes=0, inserts=0;
				analyze_hunk (thisob, &first0, &last0, &first1, &last1, &deletes, &inserts);
//				int op=0;
				if (deletes || inserts || thisob->trivial)
				{
/*					if (deletes && inserts)
						op = OP_DIFF;
					else if (deletes)
						op = OP_LEFTONLY;
					else if (inserts)
						op = OP_RIGHTONLY;
					else
						op = OP_TRIVIAL;
*/					
					/* Print the lines that the first file has.  */
					int trans_a0=0, trans_b0=0, trans_a1=0, trans_b1=0;
					translate_range(&m_diffFileData.m_inf[0], first0, last0, &trans_a0, &trans_b0);
					translate_range(&m_diffFileData.m_inf[1], first1, last1, &trans_a1, &trans_b1);

					//Determine quantity of lines in this block for both sides
					int QtyLinesLeft = (trans_b0 - trans_a0);
					int QtyLinesRight = (trans_b1 - trans_a1);
					
					// Match lines against regular expression filters
					// Our strategy is that every line in both sides must
					// match regexp before we mark difference as ignored.
					bool match2 = false;
					bool match1 = RegExpFilter(thisob->line0, thisob->line0 + QtyLinesLeft, 0);
					if (match1)
						match2 = RegExpFilter(thisob->line1, thisob->line1 + QtyLinesRight, 1);
					if (match1 && match2)
						//op = OP_TRIVIAL;
						thisob->trivial = 1;

				}
				/* Reconnect the script so it will all be freed properly.  */
				end->link = next;
			}
		}
	}


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
		m_ndiffs = DIFFS_UNKNOWN;
	}

	if (bin_flag < 0)
	{
		// Clear same-flag, set diff-flag
		code = code & ~DIFFCODE::SAME | DIFFCODE::DIFF;
	}

	return code;
}

/** 
 * @brief Compare two specified files, byte-by-byte
 * @param [in] bStopAfterFirstDiff Stop compare after we find first difference?
 * @param [in] piAbortable Interface allowing to abort compare
 * @return DIFFCODE
 */
int FolderCmp::byte_compare_files(BOOL bStopAfterFirstDiff, const IAbortable * piAbortable)
{
	QuickCompareOptions *pOptions = 
		dynamic_cast<QuickCompareOptions*>(m_pCtx->GetCompareOptions(CMP_QUICK_CONTENT));
	if (pOptions == NULL)
		return DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;

	// Close any descriptors open for diffutils
	m_diffFileData.Reset();

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
		fp[i] = _tfopen(m_diffFileData.m_FileLocation[i].filepath, _T("rb"));
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

	ByteComparator comparator(pOptions);

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
		if (!comparator.CompareBuffers(m_diffFileData.m_textStats0, m_diffFileData.m_textStats1, 
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

			BOOL bBin0 = (m_diffFileData.m_textStats0.nzeros>0);
			BOOL bBin1 = (m_diffFileData.m_textStats1.nzeros>0);

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
BOOL FolderCmp::Diff2Files(struct change ** diffs, int depth,
	int * bin_status, BOOL bMovedBlocks, int * bin_file)
{
	BOOL bRet = TRUE;
	__try
	{
		*diffs = diff_2_files (m_diffFileData.m_inf, depth, bin_status, bMovedBlocks, bin_file);
		CopyDiffutilTextStats(m_diffFileData.m_inf, &m_diffFileData);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*diffs = NULL;
		bRet = FALSE;
	}
	return bRet;
}

/**
 * @brief Match regular expression list against given difference.
 * This function matches the regular expression list against the difference
 * (given as start line and end line). Matching the diff requires that all
 * lines in difference match.
 * @param [in] StartPos First line of the difference.
 * @param [in] endPos Last line of the difference.
 * @param [in] FileNo File to match.
 * return true if any of the expressions matches.
 */
bool FolderCmp::RegExpFilter(int StartPos, int EndPos, int FileNo)
{
	if (m_pCtx->m_pFilterList == NULL)
	{	
		_RPTF0(_CRT_ERROR, "FolderCmp::RegExpFilter() called when "
			"filterlist doesn't exist (=NULL)");
		return false;
	}

	const char EolIndicators[] = "\r\n"; //List of characters used as EOL
	bool linesMatch = true; // set to false when non-matching line is found.
	int line = StartPos;

	while (line <= EndPos && linesMatch == true)
	{
		std::string LineData(files[FileNo].linbuf[line]);
		size_t EolPos = LineData.find_first_of(EolIndicators);
		if (EolPos != std::string::npos)
		{
			LineData.erase(EolPos);
		}

		if (!m_pCtx->m_pFilterList->Match(LineData.c_str()))
		{
			linesMatch = false;
		}
		++line;
	}
	return linesMatch;
}

/**
 * @brief Copy both left & right text stats results back into the DiffFileData text stats
 */
static void CopyDiffutilTextStats(file_data *inf, DiffFileData * diffData)
{
	CopyTextStats(&inf[0], &diffData->m_textStats0);
	CopyTextStats(&inf[1], &diffData->m_textStats1);
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
 * @brief Get actual compared paths from DIFFITEM.
 * @note If item is unique, same path is returned for both.
 */
void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, CString & left, CString & right)
{
	static const TCHAR backslash[] = _T("\\");

	if (!di.isSideRightOnly())
	{
		// Compare file to itself to detect encoding
		left = pCtxt->GetNormalizedLeft();
		if (!paths_EndsWithSlash(left))
			left += backslash;
		if (!di.sLeftSubdir.IsEmpty())
			left += di.sLeftSubdir + backslash;
		left += di.sLeftFilename;
		if (di.isSideLeftOnly())
			right = left;
	}
	if (!di.isSideLeftOnly())
	{
		// Compare file to itself to detect encoding
		right = pCtxt->GetNormalizedRight();
		if (!paths_EndsWithSlash(right))
			right += backslash;
		if (!di.sRightSubdir.IsEmpty())
			right += di.sRightSubdir + backslash;
		right += di.sRightFilename;
		if (di.isSideRightOnly())
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
