/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DiffWrapper.cpp
 *
 * @brief Code for DiffWrapper class
 *
 * @date  Created: 2003-08-22
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "coretools.h"
#include "common/unicoder.h"
#include "diffcontext.h"
#include "diffwrapper.h"
#include "diff.h"
#include "FileTransform.h"
#include "LogFile.h"

extern int recursive;
extern CLogFile gLog;

/**
 * @brief Default constructor
 */
CDiffWrapper::CDiffWrapper()
{
	ZeroMemory(&m_settings, sizeof(DIFFSETTINGS));
	ZeroMemory(&m_globalSettings, sizeof(DIFFSETTINGS));
	ZeroMemory(&m_status, sizeof(DIFFSTATUS));
	m_bCreatePatchFile = FALSE;
	m_bUseDiffList = FALSE;
	m_bDetectMovedBlocks = FALSE;
	m_bAddCmdLine = TRUE;
	m_bAppendFiles = FALSE;
	m_nDiffs = 0;

	m_settings.heuristic = 1;
	m_settings.outputStyle = OUTPUT_NORMAL;
	m_settings.context = -1;
    
	// character that ends a line.  Currently this is always `\n'
	line_end_char = '\n';
}

/**
 * @brief Sets files to compare
 */
void CDiffWrapper::SetCompareFiles(CString file1, CString file2)
{
	m_sFile1 = file1;
	m_sFile2 = file2;
	m_sFile1.Replace('/', '\\');
	m_sFile2.Replace('/', '\\');
}

/**
 * @brief Sets filename of produced patch-file
 */
void CDiffWrapper::SetPatchFile(CString file)
{
	m_sPatchFile = file;
	m_sPatchFile.Replace('/', '\\');
}

/**
 * @brief Sets pointer to external diff-list filled when analysing files
 */
void CDiffWrapper::SetDiffList(CArray<DIFFRANGE,DIFFRANGE> *diffs)
{
	ASSERT(diffs);
	m_diffs = diffs;
}

/**
 * @brief Returns current set of options used by diff-engine
 */
void CDiffWrapper::GetOptions(DIFFOPTIONS *options)
{
	ASSERT(options);
	InternalGetOptions(options);
}

/**
 * @brief Set options used by diff-engine
 */
void CDiffWrapper::SetOptions(DIFFOPTIONS *options)
{
	ASSERT(options);
	InternalSetOptions(options);
}

/**
 * @brief Set text tested to find the unpacker automatically.
 * Most probably a concatenated string of both filenames.
 */
void CDiffWrapper::SetTextForAutomaticUnpack(CString text)
{
	m_sToFindUnpacker = text;
}

/**
 * @brief Get options used for patch creation
 */
void CDiffWrapper::GetPatchOptions(PATCHOPTIONS *options)
{
	ASSERT(options);
	options->nContext = m_settings.context;
	options->outputStyle = m_settings.outputStyle;
	options->bAddCommandline = m_bAddCmdLine;
}

/**
 * @brief Set options used for patch creation
 */
void CDiffWrapper::SetPatchOptions(PATCHOPTIONS *options)
{
	ASSERT(options);
	m_settings.context = options->nContext;
	m_settings.outputStyle = options->outputStyle;
	m_bAddCmdLine = options->bAddCommandline;
}

/**
 * @brief Determines if external diff-list is used
 */
BOOL CDiffWrapper::GetUseDiffList() const
{
	return m_bUseDiffList;
}

/**
 * @brief Enables/disables external diff-list usage
 */
BOOL CDiffWrapper::SetUseDiffList(BOOL bUseDiffList)
{
	BOOL temp = m_bUseDiffList;
	m_bUseDiffList = bUseDiffList;
	return temp;
}

/**
 * @brief Determines if patch-file is created
 */
BOOL CDiffWrapper::GetCreatePatchFile() const 
{
	return m_bCreatePatchFile;
}

/**
 * @brief Enables/disables creation of patch-file
 */
BOOL CDiffWrapper::SetCreatePatchFile(BOOL bCreatePatchFile)
{
	BOOL temp = m_bCreatePatchFile;
	m_bCreatePatchFile = bCreatePatchFile;
	return temp;
}

/**
 * @brief Runs diff-engine
 */
BOOL CDiffWrapper::RunFileDiff()
{
	USES_CONVERSION;
	CString strFile1Temp = m_sFile1;
	CString strFile2Temp = m_sFile2;
	SwapToInternalSettings();

	if (m_bUseDiffList)
		m_nDiffs = m_diffs->GetSize();

	// perform rescan
	CString sdir0, sdir1, sname0, sname1, sext0, sext1;
	struct change *e, *p;
	struct change *script = NULL;

	// Do the preprocessing now, overwrite the temp files
	// TODO : do something with this prediffer info
	// NOTE: FileTransform_UCS2ToUTF8() may create new temp
	// files and return new names, those created temp files
	// are deleted in end of function.
	PrediffingInfo infoPrediffer;
	if (infoPrediffer.bToBeScanned)
	{
		FileTransform_Prediffing(strFile1Temp, m_sToFindUnpacker, &infoPrediffer, TRUE);
	}
	else
	{
		FileTransform_Prediffing(strFile1Temp, infoPrediffer, TRUE);
	}
	FileTransform_UCS2ToUTF8(strFile1Temp, TRUE);
	// we use the same plugin for both files, so it must be defined before second file
	ASSERT(infoPrediffer.bToBeScanned == FALSE);
	FileTransform_Prediffing(strFile2Temp, infoPrediffer, TRUE);
	FileTransform_UCS2ToUTF8(strFile2Temp, TRUE);

	DiffFileData diffdata(strFile1Temp, strFile2Temp);

	// This opens & fstats both files (if it succeeds)
	if (!diffdata.OpenFiles())
	{
		return FALSE;
	}

	file_data * inf = diffdata.m_inf;

	/* Compare the files, if no error was found.  */
	int bin_flag = 0;

	// Diff files. depth is zero because we are not comparing dirs
	script = diff_2_files (inf, 0, &bin_flag, m_bDetectMovedBlocks);

	// We don't anymore create diff-files for every rescan.
	// User can create patch-file whenever one wants to.
	// We don't need to waste time. But lets keep this as
	// debugging aid. Sometimes it is very useful to see
	// what differences diff-engine sees!
#ifdef _DEBUG
	// throw the diff into a temp file
	TCHAR lpBuffer[MAX_PATH] = {0};       // path buffer
	GetTempPath(MAX_PATH,lpBuffer);		// get path to Temp folder
	CString path = CString(lpBuffer) + _T("Diff.txt");

	outfile = _tfopen(path, _T("w+"));
	if (outfile != NULL)
	{
		print_normal_script(script);
		fclose(outfile);
		outfile = NULL;
	}
#endif
	// Create patch file
	if (m_bCreatePatchFile)
	{
		outfile = NULL;
		if (!m_sPatchFile.IsEmpty())
		{
			if (m_bAppendFiles)
				outfile = _tfopen(m_sPatchFile, _T("a+"));
			else
				outfile = _tfopen(m_sPatchFile, _T("w+"));
		}

		if (outfile != NULL)
		{
			// Print "command line"
			if (m_bAddCmdLine)
			{
				CString switches = FormatSwitchString();
				_ftprintf(outfile, _T("diff%s %s %s\n"),
					switches, m_sFile1, m_sFile2);
			}

			// Output patchfile
			switch (output_style)
			{
			case OUTPUT_CONTEXT:
				print_context_header(inf, 0);
				print_context_script(script, 0);
				break;
				
			case OUTPUT_UNIFIED:
				print_context_header(inf, 1);
				print_context_script(script, 1);
				break;
				
			case OUTPUT_ED:
				print_ed_script(script);
				break;
				
			case OUTPUT_FORWARD_ED:
				pr_forward_ed_script(script);
				break;
				
			case OUTPUT_RCS:
				print_rcs_script(script);
				break;
				
			case OUTPUT_NORMAL:
				print_normal_script(script);
				break;
				
			case OUTPUT_IFDEF:
				print_ifdef_script(script);
				break;
				
			case OUTPUT_SDIFF:
				print_sdiff_script(script);
			}
			
			fclose(outfile);
			outfile = NULL;
		}
		else
			m_status.bPatchFileFailed = TRUE;
	}
	
	// Go through diffs adding them to WinMerge's diff list
	// This is done on every WinMerge's doc rescan!
	if (m_bUseDiffList)
	{
		struct change *next = script;
		int trans_a0, trans_b0, trans_a1, trans_b1;
		int first0, last0, first1, last1, deletes, inserts, op;
		struct change *thisob, *end;
		
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

			/* Print thisob hunk.  */
			//(*printfun) (thisob);
			{					
				/* Determine range of line numbers involved in each file.  */
				analyze_hunk (thisob, &first0, &last0, &first1, &last1, &deletes, &inserts);
				if (deletes || inserts || thisob->trivial)
				{
					if (deletes && inserts)
						op = OP_DIFF;
					else if (deletes)
						op = OP_LEFTONLY;
					else if (inserts)
						op = OP_RIGHTONLY;
					else
						op = OP_TRIVIAL;
					
					/* Print the lines that the first file has.  */
					translate_range (&inf[0], first0, last0, &trans_a0, &trans_b0);
					translate_range (&inf[1], first1, last1, &trans_a1, &trans_b1);

					// Store information about these blocks in moved line info
					if (thisob->match0>=0)
					{
						ASSERT(thisob->inserted);
						for (int i=0; i<thisob->inserted; ++i)
						{
							int line0 = i+thisob->match0 + (trans_a0-first0-1);
							int line1 = i+thisob->line1 + (trans_a1-first1-1);
							m_moved1[line1]=line0;
						}
					}
					if (thisob->match1>=0)
					{
						ASSERT(thisob->deleted);
						for (int i=0; i<thisob->deleted; ++i)
						{
							int line0 = i+thisob->line0 + (trans_a0-first0-1);
							int line1 = i+thisob->match1 + (trans_a1-first1-1);
							m_moved0[line0]=line1;
						}
					}

					AddDiffRange(trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, (BYTE)op);
					TRACE(_T("left=%d,%d   right=%d,%d   op=%d\n"),
						trans_a0-1, trans_b0-1, trans_a1-1, trans_b1-1, op);
				}
			}
			
			/* Reconnect the script so it will all be freed properly.  */
			end->link = next;
		}
	}			

	// cleanup the script
	for (e = script; e; e = p)
	{
		p = e->link;
		free (e);
	}


	// Done with diffutils filedata
	diffdata.Close();

	// Delete temp files transformation functions possibly created
	if (m_sFile1.CompareNoCase(strFile1Temp) != 0)
	{
		if (!::DeleteFile(strFile1Temp))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
				strFile1Temp, GetSysError(GetLastError())));
		}
		strFile1Temp.Empty();
	}
	if (m_sFile2.CompareNoCase(strFile2Temp) != 0)
	{
		if (!::DeleteFile(strFile2Temp))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
				strFile2Temp, GetSysError(GetLastError())));
		}
		strFile2Temp.Empty();
	}

	m_status.bBinaries = bin_flag > 0;
	m_status.bLeftMissingNL = inf[0].missing_newline;
	m_status.bRightMissingNL = inf[1].missing_newline;
	

	SwapToGlobalSettings();

	return TRUE;
}

/**
 * @brief Return current diffutils options
 */
void CDiffWrapper::InternalGetOptions(DIFFOPTIONS *options)
{
	int nIgnoreWhitespace = 0;

	if (m_settings.ignoreAllSpace)
		nIgnoreWhitespace = WHITESPACE_IGNORE_ALL;
	else if (m_settings.ignoreSpaceChange)
		nIgnoreWhitespace = WHITESPACE_IGNORE_CHANGE;

	options->nIgnoreWhitespace = nIgnoreWhitespace;
	options->bIgnoreBlankLines = m_settings.ignoreBlankLines;
	options->bIgnoreCase = m_settings.ignoreCase;
	options->bEolSensitive = !m_settings.ignoreEOLDiff;

}

/**
 * @brief Set diffutils options
 */
void CDiffWrapper::InternalSetOptions(DIFFOPTIONS *options)
{
	m_settings.ignoreAllSpace = (options->nIgnoreWhitespace == WHITESPACE_IGNORE_ALL);
	m_settings.ignoreSpaceChange = (options->nIgnoreWhitespace == WHITESPACE_IGNORE_CHANGE);
	m_settings.ignoreBlankLines = options->bIgnoreBlankLines;
	m_settings.ignoreEOLDiff = !options->bEolSensitive;
	m_settings.ignoreCase = options->bIgnoreCase;
	m_settings.ignoreSomeChanges = (options->nIgnoreWhitespace != WHITESPACE_COMPARE_ALL) ||
		options->bIgnoreCase || options->bIgnoreBlankLines ||
		!options->bEolSensitive;
	m_settings.lengthVaries = (options->nIgnoreWhitespace != WHITESPACE_COMPARE_ALL);
}

/**
 * @brief Replaces global options used by diff-engine with options in diff-wrapper
 */
void CDiffWrapper::SwapToInternalSettings()
{
	// Save current settings to temp variables
	m_globalSettings.outputStyle = output_style;
	output_style = m_settings.outputStyle;
	
	m_globalSettings.context = context;
	context = m_settings.context;
	
	m_globalSettings.alwaysText = always_text_flag;
	always_text_flag = m_settings.alwaysText;

	m_globalSettings.horizLines = horizon_lines;
	horizon_lines = m_settings.horizLines;

	m_globalSettings.ignoreSpaceChange = ignore_space_change_flag;
	ignore_space_change_flag = m_settings.ignoreSpaceChange;

	m_globalSettings.ignoreAllSpace = ignore_all_space_flag;
	ignore_all_space_flag = m_settings.ignoreAllSpace;

	m_globalSettings.ignoreBlankLines = ignore_blank_lines_flag;
	ignore_blank_lines_flag = m_settings.ignoreBlankLines;

	m_globalSettings.ignoreCase = ignore_case_flag;
	ignore_case_flag = m_settings.ignoreCase;

	m_globalSettings.ignoreEOLDiff = ignore_eol_diff;
	ignore_eol_diff = m_settings.ignoreEOLDiff;

	m_globalSettings.ignoreSomeChanges = ignore_some_changes;
	ignore_some_changes = m_settings.ignoreSomeChanges;

	m_globalSettings.lengthVaries = length_varies;
	length_varies = m_settings.lengthVaries;

	m_globalSettings.heuristic = heuristic;
	heuristic = m_settings.heuristic;

	m_globalSettings.recursive = recursive;
	recursive = m_settings.recursive;
}

/**
 * @brief Resumes global options as they were before calling SwapToInternalOptions()
 */
void CDiffWrapper::SwapToGlobalSettings()
{
	// Resume values
	output_style = m_globalSettings.outputStyle;
	context = m_globalSettings.context;
	always_text_flag = m_globalSettings.alwaysText;
	horizon_lines = m_globalSettings.horizLines;
	ignore_space_change_flag = m_globalSettings.ignoreSpaceChange;
	ignore_all_space_flag = m_globalSettings.ignoreAllSpace;
	ignore_blank_lines_flag = m_globalSettings.ignoreBlankLines;
	ignore_case_flag = m_globalSettings.ignoreCase;
	ignore_eol_diff = m_globalSettings.ignoreEOLDiff;
	ignore_some_changes = m_globalSettings.ignoreSomeChanges;
	length_varies = m_globalSettings.lengthVaries;
	heuristic = m_globalSettings.heuristic;
	recursive = m_globalSettings.recursive;
}

/**
 * @brief Add diff to external diff-list
 */
void CDiffWrapper::AddDiffRange(UINT begin0, UINT end0, UINT begin1, UINT end1, BYTE op)
{
	TRY {
		DIFFRANGE dr = {0};
		dr.begin0 = begin0;
		dr.end0 = end0;
		dr.begin1 = begin1;
		dr.end1 = end1;
		dr.op = op;
		dr.blank0 = dr.blank1 = -1;
		m_diffs->SetAtGrow(m_nDiffs, dr);
		m_nDiffs++;
	}
	CATCH_ALL(e)
	{
		TCHAR msg[1024] = {0};
		e->GetErrorMessage(msg, 1024);
		AfxMessageBox(msg, MB_ICONSTOP);
	}
	END_CATCH_ALL;
}

/**
 * @brief Expand last DIFFRANGE of file by one line to contain last line after EOL.
 */
void CDiffWrapper::FixLastDiffRange(int leftBufferLines, int rightBufferLines, BOOL left)
{
	DIFFRANGE dr = {0};
	int count = m_diffs->GetSize();
	if (count > 0)
	{
		dr = m_diffs->GetAt(count - 1);

		if (left)
		{
			if (dr.op == OP_RIGHTONLY)
				dr.op = OP_DIFF;
			dr.end0++;
		}
		else
		{
			if (dr.op == OP_LEFTONLY)
				dr.op = OP_DIFF;
			dr.end1++;
		}

		m_diffs->SetAt(count - 1, dr); 
	}
	else 
	{
		// we have to create the DIFF
		dr.end0 = leftBufferLines - 1;
		dr.end1 = rightBufferLines - 1;
		if (left)
		{
			dr.begin0 = dr.end0;
			dr.begin1 = dr.end1 + 1;
			dr.op = OP_LEFTONLY;
		}
		else
		{
			dr.begin0 = dr.end0 + 1;
			dr.begin1 = dr.end1;
			dr.op = OP_RIGHTONLY;
		}
		ASSERT(dr.begin0 == dr.begin1);

		AddDiffRange(dr.begin0, dr.end0, dr.begin1, dr.end1, dr.op); 
	}
}

/**
 * @brief Returns status-data from diff-engine last run
 */
void CDiffWrapper::GetDiffStatus(DIFFSTATUS *status)
{
	CopyMemory(status, &m_status, sizeof(DIFFSTATUS));
}

/**
 * @brief Formats command-line for diff-engine last run (like it was called from command-line)
 */
CString CDiffWrapper::FormatSwitchString()
{
	CString switches;
	TCHAR tmpNum[5] = {0};
	
	switch (m_settings.outputStyle)
	{
	case OUTPUT_CONTEXT:
		switches = _T(" C");
		break;
	case OUTPUT_UNIFIED:
		switches = _T(" U");
		break;
	case OUTPUT_ED:
		switches = _T(" e");
		break;
	case OUTPUT_FORWARD_ED:
		switches = _T(" f");
		break;
	case OUTPUT_RCS:
		switches = _T(" n");
		break;
	case OUTPUT_NORMAL:
		switches = _T(" ");
		break;
	case OUTPUT_IFDEF:
		switches = _T(" D");
		break;
	case OUTPUT_SDIFF:
		switches = _T(" y");
		break;
	}

	if (m_settings.context > 0)
	{
		_itot(m_settings.context, tmpNum, 10);
		switches += tmpNum;
	}

	if (m_settings.ignoreAllSpace > 0)
		switches += _T("w");

	if (m_settings.ignoreBlankLines > 0)
		switches += _T("B");

	if (m_settings.ignoreCase > 0)
		switches += _T("i");

	if (m_settings.ignoreSpaceChange > 0)
		switches += _T("b");

	return switches;
}

/**
 * @brief Determines if patch-files are appended (not overwritten)
 */
BOOL CDiffWrapper::GetAppendFiles() const
{
	return m_bAppendFiles;
}

/**
 * @brief Enables/disables patch-file appending (files with same filename are appended)
 */
BOOL CDiffWrapper::SetAppendFiles(BOOL bAppendFiles)
{
	BOOL temp = m_bAppendFiles;
	m_bAppendFiles = bAppendFiles;
	return temp;
}

/**
 * @brief Sets options for directory compare
 */
void CDiffWrapper::StartDirectoryDiff()
{
	SwapToInternalSettings();
}

/**
 * @brief resumes options after directory compare
 */
void CDiffWrapper::EndDirectoryDiff()
{
	SwapToGlobalSettings();
}

/** @brief Simple initialization of DiffFileData */
DiffFileData::DiffFileData(LPCTSTR szFilepath1, LPCTSTR szFilepath2)
{
	m_inf = new file_data[2];
	for (int i=0; i<2; ++i)
		memset(&m_inf[i], 0, sizeof(m_inf[i]));
	m_used = false;
	m_sFilepath[0] = szFilepath1;
	m_sFilepath[1] = szFilepath2;
	Reset();
}

/** @brief deallocate member data */
DiffFileData::~DiffFileData()
{
	Reset();
	delete [] m_inf;
	m_inf = 0;
}

/** @brief Open file descriptors in the inf structure (return false if failure) */
bool DiffFileData::OpenFiles()
{
	bool b = DoOpenFiles();
	if (!b)
		Reset();
	return b;
}

/** @brief Open file descriptors in the inf structure (return false if failure) */
bool DiffFileData::DoOpenFiles()
{
	Reset();

	for (int i=0; i<2; ++i)
	{
		// Fill in 8-bit versions of names for diffutils (WinMerge doesn't use these)
		USES_CONVERSION;
		m_inf[i].name = strdup(T2CA(m_sFilepath[i]));
		if (m_inf[i].name == NULL)
			return false;

		// Open up file descriptors
		// Always use O_BINARY mode, to avoid terminating file read on ctrl-Z (DOS EOF)
		// Also, WinMerge-modified diffutils handles all three major eol styles
		m_inf[i].desc = _topen(m_sFilepath[i], O_RDONLY|O_BINARY, _S_IREAD);
		if (m_inf[i].desc < 0)
			return false;

		// Get file stats (diffutils uses these)
		if (fstat(m_inf[i].desc, &m_inf[i].stat) != 0)
		{
			return false;
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

/** @brief Compare two specified files */
int DiffFileData::just_compare_files(int depth, int *ndiffs, int *ntrivialdiffs)
{
	int bin_flag = 0;

	// Do the actual comparison (generating a change script)
	BOOL bDetectMovedBlocks = FALSE;
	struct change *script = diff_2_files (m_inf, depth, &bin_flag, bDetectMovedBlocks);

	int code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::SAME;

	// Free change script (which we don't want)
	if (script != NULL)
	{
		struct change *p,*e;
		for (e = script; e; e = p)
		{
			(*ndiffs)++;
			if (!e->trivial)
				code = code & ~DIFFCODE::SAME | DIFFCODE::DIFF;
			else
				(*ntrivialdiffs)++;
			p = e->link;
			free (e);
		}
	}

	// diff_2_files set bin_flag to -1 if different binary
	// diff_2_files set bin_flag to +1 if same binary

	if (bin_flag != 0)
		code = code & ~DIFFCODE::TEXT | DIFFCODE::BIN;

	if (bin_flag < 0)
		code = code & ~DIFFCODE::SAME | DIFFCODE::DIFF;

	return code;
}

/** @brief return 1st or 2nd file descriptor */
int DiffFileData::fd(int i)
{
	return m_inf[i].desc;
}

/** @brief detect unicode file and quess encoding */
DiffFileData::UniFileBom::UniFileBom(int fd)
{
	size = 0;
	unicoding = ucr::NONE;
	if (fd != -1)
	{
		long tmp = _lseek(fd, 0, SEEK_SET);
		switch (_read(fd, buffer, 3))
		{
			case 3:
				size = 3;
				unicoding = ucr::UTF8;
				if (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF)
					break;
			case 2:
				size = 2;
				unicoding = ucr::UCS2LE;
				if (buffer[0] == 0xFF && buffer[1] == 0xFE)
					break;
				unicoding = ucr::UCS2BE;
				if (buffer[0] == 0xFE && buffer[1] == 0xFF)
					break;
			default:
				size = 0;
				unicoding = ucr::NONE;
		}
		_lseek(fd, tmp, SEEK_SET);
	}
}

/** @brief Create int array with size elements, and set initial entries to initvalue */
#if 0
static int * NewIntArray(int size, int initvalue)
{
	int * arr = new int[size];
	for (int i=0; i<size; ++i)
		arr[i] = initvalue;
	return arr;
}
#endif

class IntSet
{
public:
	void Add(int val) { m_map.SetAt(val, 1); }
	void Remove(int val) { m_map.RemoveKey(val); }
	int count() const { return m_map.GetCount(); }
	bool isPresent(int val) const { int parm; return !!m_map.Lookup(val, parm); }
	int getSingle() const 
	{
		int val, parm;
		POSITION pos = m_map.GetStartPosition();
		m_map.GetNextAssoc(pos, val, parm); 
		return val; 
	}

private:
	CMap<int, int, int, int> m_map;
};

/** 
 * @brief  Set of equivalent lines
 * This uses diffutils line numbers, which are counted from the prefix
 */
struct EqGroup
{
	IntSet m_lines0; // equivalent lines on side#0
	IntSet m_lines1; // equivalent lines on side#1

	bool isPerfectMatch() const { return m_lines0.count()==1 && m_lines1.count()==1; }
};


/** @brief  Maps equivalency code to equivalency group */
class CodeToGroupMap : public CTypedPtrMap<CMapPtrToPtr, void*, EqGroup *>
{
public:
	/** @brief Add a line to the appropriate equivalency group */
	void Add(int lineno, int eqcode, int nside)
	{
		EqGroup * pgroup = 0;
		if (!Lookup((void *)eqcode, pgroup))
		{
			pgroup = new EqGroup;
			SetAt((void *)eqcode, pgroup);
		}
		if (nside)
			pgroup->m_lines1.Add(lineno);
		else
			pgroup->m_lines0.Add(lineno);
	}

	/** @brief Return the appropriate equivalency group */
	EqGroup * find(int eqcode)
	{
		EqGroup * pgroup=0;
		Lookup((void *)eqcode, pgroup);
		return pgroup;
	}

	~CodeToGroupMap()
	{
		for (POSITION pos = GetStartPosition(); pos; )
		{
			void * v=0;
			EqGroup * pgroup=0;
			GetNextAssoc(pos, v, pgroup);
			delete pgroup;
		}
	}
};


/* WinMerge moved block code */
extern "C" void moved_block_analysis(struct change ** pscript, struct file_data fd[])
{
	// Hash all altered lines
	CodeToGroupMap map;

	struct change * script = *pscript;
	struct change *p,*e;
	for (e = script; e; e = p)
	{
		p = e->link;
		int i;
		for (i=e->line0; i-(e->line0) < (e->deleted); ++i)
			map.Add(i, fd[0].equivs[i], 0);
		for (i=e->line1; i-(e->line1) < (e->inserted); ++i)
			map.Add(i, fd[1].equivs[i], 1);
	}


	// Scan through diff blocks, finding moved sections from left side
	// and splitting them out
	// That is, we actually fragment diff blocks as we find moved sections
	for (e = script; e; e = p)
	{
		// scan down block for a match
		p = e->link;
		EqGroup * pgroup = 0;
		for (int i=e->line0; i-(e->line0) < (e->deleted); ++i)
		{
			EqGroup * tempgroup = map.find(fd[0].equivs[i]);
			if (tempgroup->isPerfectMatch())
			{
				pgroup = tempgroup;
				break;
			}
		}

		// if no match, go to next diff block
		if (!pgroup)
			continue;

		// found a match
		int j = pgroup->m_lines1.getSingle();
		// Ok, now our moved block is the single line i,j

		// extend moved block upward as far as possible
		int i1 = i-1;
		int j1 = j-1;
		for ( ; i1>=e->line0; --i1, --j1)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i1]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j1]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i1);
			pgroup1->m_lines1.Remove(j1);
		}
		++i1;
		++j1;
		// Ok, now our moved block is i1->i, j1->j

		// extend moved block downward as far as possible
		int i2 = i+1;
		int j2 = j+1;
		for ( ; i2-(e->line0) < (e->deleted); ++i2,++j2)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i2]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j2]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i2);
			pgroup1->m_lines1.Remove(j2);
		}
		--i2;
		--j2;
		// Ok, now our moved block is i1->i2,j1->j2

		ASSERT(i2-i1 >= 0);
		ASSERT(i2-i1 == j2-j1);

		int prefix = i1 - (e->line0);
		if (prefix)
		{
			// break e (current change) into two pieces
			// first part is the prefix, before the moved part
			// that stays in e
			// second part is the moved part & anything after it
			// that goes in newob
			// leave the right side (e->inserted) on e
			// so no right side on newob
			// newob will be the moved part only, later after we split off any suffix from it
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = i1;
			newob->line1 = e->line1 + e->inserted;
			newob->inserted = 0;
			newob->deleted = e->deleted - prefix;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = -1;

			e->deleted = prefix;
			e->link = newob;

			// now make e point to the moved part (& any suffix)
			e = newob;
		}
		// now e points to a moved diff chunk with no prefix, but maybe a suffix

		e->match1 = j1;

		int suffix = (e->deleted) - (i2-(e->line0)) - 1;
		if (suffix)
		{
			// break off any suffix from e
			// newob will be the suffix, and will get all the right side
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = i2+1;
			newob->line1 = e->line1;
			newob->inserted = e->inserted;
			newob->deleted = suffix;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = -1;

			e->inserted = 0;
			e->deleted -= suffix;
			e->link = newob;

			p = newob; // next block to scan
		}
	}

	// Scan through diff blocks, finding moved sections from right side
	// and splitting them out
	// That is, we actually fragment diff blocks as we find moved sections
	for (e = script; e; e = p)
	{
		// scan down block for a match
		p = e->link;
		EqGroup * pgroup = 0;
		for (int j=e->line1; j-(e->line1) < (e->inserted); ++j)
		{
			EqGroup * tempgroup = map.find(fd[1].equivs[j]);
			if (tempgroup->isPerfectMatch())
			{
				pgroup = tempgroup;
				break;
			}
		}

		// if no match, go to next diff block
		if (!pgroup)
			continue;

		// found a match
		int i = pgroup->m_lines0.getSingle();
		// Ok, now our moved block is the single line i,j

		// extend moved block upward as far as possible
		int i1 = i-1;
		int j1 = j-1;
		for ( ; j1>=e->line1; --i1, --j1)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i1]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j1]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i1);
			pgroup1->m_lines1.Remove(j1);
		}
		++i1;
		++j1;
		// Ok, now our moved block is i1->i, j1->j

		// extend moved block downward as far as possible
		int i2 = i+1;
		int j2 = j+1;
		for ( ; j2-(e->line1) < (e->inserted); ++i2,++j2)
		{
			EqGroup * pgroup0 = map.find(fd[0].equivs[i2]);
			EqGroup * pgroup1 = map.find(fd[1].equivs[j2]);
			if (pgroup0 != pgroup1)
				break;
			pgroup0->m_lines0.Remove(i2);
			pgroup1->m_lines1.Remove(j2);
		}
		--i2;
		--j2;
		// Ok, now our moved block is i1->i2,j1->j2

		ASSERT(i2-i1 >= 0);
		ASSERT(i2-i1 == j2-j1);

		int prefix = j1 - (e->line1);
		if (prefix)
		{
			// break e (current change) into two pieces
			// first part is the prefix, before the moved part
			// that stays in e
			// second part is the moved part & anything after it
			// that goes in newob
			// leave the left side (e->deleted) on e
			// so no right side on newob
			// newob will be the moved part only, later after we split off any suffix from it
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = e->line0 + e->deleted;
			newob->line1 = j1;
			newob->inserted = e->inserted - prefix;
			newob->deleted = 0;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = -1;

			e->inserted = prefix;
			e->link = newob;

			// now make e point to the moved part (& any suffix)
			e = newob;
		}
		// now e points to a moved diff chunk with no prefix, but maybe a suffix

		e->match0 = i1;

		int suffix = (e->inserted) - (j2-(e->line1)) - 1;
		if (suffix)
		{
			// break off any suffix from e
			// newob will be the suffix, and will get all the left side
			struct change *newob = (struct change *) xmalloc (sizeof (struct change));
			memset(newob, 0, sizeof(*newob));

			newob->line0 = e->line0;
			newob->line1 = j2+1;
			newob->inserted = suffix;
			newob->deleted = e->deleted;
			newob->link = e->link;
			newob->match0 = -1;
			newob->match1 = -1;

			e->inserted -= suffix;
			e->deleted = 0;
			e->link = newob;

			p = newob; // next block to scan
		}
	}

}

/*
read_files
 creates and destroys the hash table of lines (equivs & buckets)

It is called from diff_2_files
 which subsequently builds the script chain (of blocks)

So we need to (optionally) keep the hash table around after
 read_files so diff_2_files can pass it to the new
 block moved routine




*/
void cleanup_script(struct change ** pscript)
{
	struct change * script = *pscript;
	struct change *p,*e;
	for (e = script; e; e = p)
	{
		p = e->link;
		free (e);
	}
	*pscript = 0;
}

