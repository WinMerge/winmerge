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
#include "DiffList.h"
#include "diffwrapper.h"
#include "diff.h"
#include "FileTransform.h"
#include "LogFile.h"
#include "codepage.h"
#include <shlwapi.h>
#include "ByteComparator.h"
#include "codepage_detect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int recursive;
extern CLogFile gLog;
static int f_defcp = 0; // default codepage
static const int KILO = 1024; // Kilo(byte)
static const int MEG = 1024 * KILO; // Mega(byte)

/**
 * @brief Limit for compare by contents/quick contents.
 * If compare by contents is selected (normal) then files bigger than limit
 * are compared with compare by quick contents. That allows us to compare
 * big binary files and overall makes comparing bigger files faster.
 */
static const int CMP_SIZE_LIMIT = 2 * MEG;

/**
 * @brief Quick contents compare's file buffer size
 */
static const int WMCMPBUFF = 32 * KILO;

static void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, CString & left, CString & right);
static inline BOOL isBinaryBuf(char * bufBegin, char * bufEnd);

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
	m_infoPrediffer = NULL;
	m_pDiffList = NULL;

	m_settings.heuristic = 1;
	m_settings.outputStyle = OUTPUT_NORMAL;
	m_settings.context = -1;

	// character that ends a line.  Currently this is always `\n'
	line_end_char = '\n';
}

CDiffWrapper::~CDiffWrapper()
{
	if (m_infoPrediffer)
		delete m_infoPrediffer;
}

/**
 * @brief Sets files to compare
 */
void CDiffWrapper::SetCompareFiles(CString file1, CString file2, ARETEMPFILES areTempFiles)
{
	m_sFile1 = file1;
	m_sFile2 = file2;
	m_sFile1.Replace('/', '\\');
	m_sFile2.Replace('/', '\\');
	m_areTempFiles = areTempFiles;
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
void CDiffWrapper::SetDiffList(DiffList *diffList)
{
	ASSERT(diffList);
	m_pDiffList = diffList;
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
 * @brief Set text tested to find the prediffer automatically.
 * Most probably a concatenated string of both filenames.
 */
void CDiffWrapper::SetTextForAutomaticPrediff(CString text)
{
	m_sToFindPrediffer = text;
}
void CDiffWrapper::SetPrediffer(PrediffingInfo * prediffer /*=NULL*/)
{
	if (m_infoPrediffer)
		delete m_infoPrediffer;

	// all flags are set correctly during the construction
	m_infoPrediffer = new PrediffingInfo;

	if (prediffer)
		*m_infoPrediffer = *prediffer;
}
void CDiffWrapper::GetPrediffer(PrediffingInfo * prediffer)
{
	*prediffer = *m_infoPrediffer;
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
	BOOL bRet = TRUE;
	USES_CONVERSION;
	CString strFile1Temp = m_sFile1;
	CString strFile2Temp = m_sFile2;
	SwapToInternalSettings();

	if (m_bUseDiffList)
		m_nDiffs = m_pDiffList->GetSize();

	// perform rescan
	CString sdir0, sdir1, sname0, sname1, sext0, sext1;
	struct change *e, *p;
	struct change *script = NULL;

	// Are our working files overwritable (temp)?
	BOOL bMayOverwrite = (m_areTempFiles == YESTEMPFILES);

	// Do the preprocessing now, overwrite the temp files
	// NOTE: FileTransform_UCS2ToUTF8() may create new temp
	// files and return new names, those created temp files
	// are deleted in end of function.
	if (m_infoPrediffer->bToBeScanned)
	{
		// this can only fail if the data can not be saved back (no more place on disk ???)
		// what to do then ??
		FileTransform_Prediffing(strFile1Temp, m_sToFindPrediffer, m_infoPrediffer, bMayOverwrite);
	}
	else
	{
		// this can failed if the prediffer has a problem
		if (FileTransform_Prediffing(strFile1Temp, *m_infoPrediffer, bMayOverwrite) == FALSE)
		{
			// display a message box
			CString sError;
			AfxFormatString2(sError, IDS_PREDIFFER_ERROR, strFile1Temp, m_infoPrediffer->pluginName);
			AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
			// don't use any more this prediffer
			m_infoPrediffer->bToBeScanned = FALSE;
			m_infoPrediffer->pluginName = _T("");
		}
	}

	FileTransform_UCS2ToUTF8(strFile1Temp, bMayOverwrite);
	// we use the same plugin for both files, so it must be defined before second file
	ASSERT(m_infoPrediffer->bToBeScanned == FALSE);
	if (FileTransform_Prediffing(strFile2Temp, *m_infoPrediffer, bMayOverwrite) == FALSE)
	{
		// display a message box
		CString sError;
		AfxFormatString2(sError, IDS_PREDIFFER_ERROR, strFile2Temp, m_infoPrediffer->pluginName);
		AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
		// don't use any more this prediffer
		m_infoPrediffer->bToBeScanned = FALSE;
		m_infoPrediffer->pluginName = _T("");
	}
	FileTransform_UCS2ToUTF8(strFile2Temp, bMayOverwrite);

	DiffFileData diffdata;

	// This opens & fstats both files (if it succeeds)
	if (!diffdata.OpenFiles(strFile1Temp, strFile2Temp))
	{
		return FALSE;
	}

	file_data * inf = diffdata.m_inf;

	/* Compare the files, if no error was found.  */
	int bin_flag = 0;
	bRet = Diff2Files(&script, &diffdata, &bin_flag);

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

	// First determine what happened during comparison
	// If there were errors or files were binaries, don't bother
	// creating diff-lists or patches
	
	// diff_2_files set bin_flag to -1 if different binary
	// diff_2_files set bin_flag to +1 if same binary
	if (bin_flag != 0)
	{
		m_status.bBinaries = TRUE;
		if (bin_flag == -1)
			m_status.bIdentical = FALSE;
		else
			m_status.bIdentical = TRUE;
	}
	else
	{ // text files according to diffutils, so change script exists
		m_status.bIdentical = (script == 0);
		m_status.bBinaries = FALSE;
	}
	m_status.bLeftMissingNL = inf[0].missing_newline;
	m_status.bRightMissingNL = inf[1].missing_newline;


	// Create patch file
	if (!m_status.bBinaries && m_bCreatePatchFile)
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
	if (!m_status.bBinaries && m_bUseDiffList)
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

	SwapToGlobalSettings();
	return bRet;
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
		m_pDiffList->AddDiff(dr);
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
	const int count = m_pDiffList->GetSize();
	if (count > 0)
	{
		m_pDiffList->GetDiff(count - 1, dr);

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
		m_pDiffList->SetDiff(count - 1, dr);
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

/**
 * @brief clear the lists (left & right) of moved blocks before RunFileDiff
 */
void CDiffWrapper::ClearMovedLists() 
{ 
	m_moved0.RemoveAll(); 
	m_moved1.RemoveAll(); 
}

/**
 * @brief Get left->right info for a moved line (real line number)
 */
int CDiffWrapper::RightLineInMovedBlock(int leftLine)
{
	int rightLine;
	if (m_moved0.Lookup(leftLine, rightLine))
		return rightLine;
	else
		return -1;
}

/**
 * @brief Get right->left info for a moved line (real line number)
 */
int CDiffWrapper::LeftLineInMovedBlock(int rightLine)
{
	int leftLine;
	if (m_moved1.Lookup(rightLine, leftLine))
		return leftLine;
	else
		return -1;
}

/** @brief Allow caller to specify codepage to assume for all unknown files */
void // static
DiffFileData::SetDefaultCodepage(int defcp)
{
	f_defcp = defcp;
}

/** @brief Simple initialization of DiffFileData */
DiffFileData::DiffFileData()
{
	m_inf = new file_data[2];
	for (int i=0; i<2; ++i)
		memset(&m_inf[i], 0, sizeof(m_inf[i]));
	m_used = false;
	m_ndiffs = 0;
	m_ntrivialdiffs = 0;
	Reset();
	// Set default codepages
	for (i=0; i<sizeof(m_sFilepath)/sizeof(m_sFilepath[0]); ++i)
	{
		m_sFilepath[i].codepage = f_defcp;
	}
}

/** @brief deallocate member data */
DiffFileData::~DiffFileData()
{
	Reset();
	delete [] m_inf;
}

/** @brief Open file descriptors in the inf structure (return false if failure) */
bool DiffFileData::OpenFiles(LPCTSTR szFilepath1, LPCTSTR szFilepath2)
{
	m_sFilepath[0].AssignPath(szFilepath1);
	m_sFilepath[1].AssignPath(szFilepath2);
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
		if (m_inf[i].desc == 0)
		{
		m_inf[i].desc = _topen(m_sFilepath[i], O_RDONLY|O_BINARY, _S_IREAD);
		}
		if (m_inf[i].desc < 0)
			return false;

		// Get file stats (diffutils uses these)
		if (fstat(m_inf[i].desc, &m_inf[i].stat) != 0)
		{
			return false;
		}
		if (m_sFilepath[1] == m_sFilepath[0])
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
 * @brief Try to deduce encoding for this file
 */
void DiffFileData::FilepathWithEncoding::GuessEncoding(const char **data, int count)
{
	if (unicoding == 0)
	{
		const CString & filepath = *this;
		CString sExt = PathFindExtension(filepath);
		GuessEncoding_from_bytes(sExt, data, count, &codepage);
	}
}

/** @brief Guess encoding for one file */
void DiffFileData::GuessEncoding(int side, CDiffContext * pCtxt)
{
	if (!pCtxt->m_bGuessEncoding)
		return;

	m_sFilepath[side].GuessEncoding(m_inf[side].linbuf + m_inf[side].linbuf_base, 
	                                m_inf[side].valid_lines - m_inf[side].linbuf_base);
}

/** @brief Compare two specified files */
int DiffFileData::diffutils_compare_files(int depth)
{
	int bin_flag = 0;

	// Do the actual comparison (generating a change script)
	struct change *script = NULL;
	BOOL success = Diff2Files(&script, depth, &bin_flag, FALSE);
	if (!success)
	{
		return DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;
	}
	int code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::SAME;

	// Free change script (which we don't want)
	if (script != NULL)
	{
		struct change *p,*e;
		for (e = script; e; e = p)
		{
			++m_ndiffs;
			if (!e->trivial)
				code = code & ~DIFFCODE::SAME | DIFFCODE::DIFF;
			else
				++m_ntrivialdiffs;
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


/*
 WinMerge moved block code
 This is called by diffutils code, by diff_2_files routine
 read_files earlier computed the hash chains ("equivs" file variable) and freed them,
 but the equivs numerics are still available in each line
*/
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
 * @brief Get actual compared paths from DIFFITEM.
 * @note If item is unique, same path is returned for both.
 */
void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, CString & left, CString & right)
{
	static const TCHAR backslash[] = _T("\\");

	if (!di.isSideRight())
	{
		// Compare file to itself to detect encoding
		left = pCtxt->GetNormalizedLeft() + backslash;
		if (!di.sLeftSubdir.IsEmpty())
			left += di.sLeftSubdir + backslash;
		left += di.sLeftFilename;
		if (di.isSideLeft())
			right = left;
	}
	if (!di.isSideLeft())
	{
		// Compare file to itself to detect encoding
		right = pCtxt->GetNormalizedRight() + backslash;
		if (!di.sRightSubdir.IsEmpty())
			right += di.sRightSubdir + backslash;
		right += di.sRightFilename;
		if (di.isSideRight())
			left = right;
	}
}


/**
 * @brief Invoke appropriate plugins for prediffing
 * return false if anything fails
 * caller has to DeleteFile filepathTransformed, if it differs from filepath
 */
bool DiffFileData::FilepathWithEncoding::Transform(const CString & filepath, CString & filepathTransformed,
	const CString & filteredFilenames, PrediffingInfo * infoPrediffer, int fd)
{
	BOOL bMayOverwrite = FALSE; // temp variable set each time it is used

	UniFileBom bom = fd; // guess encoding
	unicoding = bom.unicoding;

	if (unicoding && unicoding != ucr::UCS2LE)
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

	if (unicoding)
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
 * @brief Prepare files (run plugins) & compare them, and return diffcode
 */
int DiffFileData::prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di)
{
	int nCompMethod = pCtxt->m_nCompMethod;
	CString filepath1;
	CString filepath2;
	GetComparePaths(pCtxt, di, filepath1, filepath2);

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
	/*diffdata.m_sFilepath[0] = */filepathTransformed1 = filepathUnpacked1;
	/*diffdata.m_sFilepath[1] = */filepathTransformed2 = filepathUnpacked2;
	if (!OpenFiles(filepathTransformed1, filepathTransformed2))
	{
		di.errorDesc = _T("OpenFiles Error (before tranform)");
		goto exitPrepAndCompare;
	}

	// Invoke prediff'ing plugins
	if (!m_sFilepath[0].Transform(filepathUnpacked1, filepathTransformed1, filteredFilenames, infoPrediffer, m_inf[0].desc))
	{
		di.errorDesc = _T("Transform Error Side 1");
		goto exitPrepAndCompare;
	}

	// we use the same plugins for both files, so they must be defined before second file
	ASSERT(infoPrediffer->bToBeScanned == FALSE);

	if (!m_sFilepath[1].Transform(filepathUnpacked2, filepathTransformed2, filteredFilenames, infoPrediffer, m_inf[1].desc))
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

	// If either file is larger than 2 Megs compare files by quick contents
	// This allows us to (faster) compare big binary files
	if (pCtxt->m_nCompMethod == CMP_CONTENT && 
		(di.left.size > CMP_SIZE_LIMIT || di.right.size > CMP_SIZE_LIMIT))
	{
		nCompMethod = CMP_QUICK_CONTENT;
	}

	if (nCompMethod == CMP_CONTENT)
	{
		// use diffutils
		code = diffutils_compare_files(0);
		if (code & DIFFCODE::CMPERR)
			di.errorDesc = _T("DiffUtils Error");
	}
	else if (nCompMethod == CMP_QUICK_CONTENT)
	{
		// use our own byte-by-byte compare
		code = byte_compare_files();
	}
	else
	{
		// Print error since we should have handled by date compare earlier
		_RPTF0(_CRT_ERROR, "Invalid compare type, DiffFileData can't handle it");
		di.errorDesc = _T("Bad compare type");
		goto exitPrepAndCompare;
	}

	if ((code & DIFFCODE::CMPERR) == 0)
	{
		GuessEncoding(0, pCtxt);
		GuessEncoding(1, pCtxt);
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

BOOL CDiffWrapper::Diff2Files(struct change ** diffs, DiffFileData *diffData,
	int * bin_status)
{
	BOOL bRet = TRUE;
	__try
	{
		// Diff files. depth is zero because we are not comparing dirs
		*diffs = diff_2_files (diffData->m_inf, 0, bin_status, m_bDetectMovedBlocks);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*diffs = NULL;
		bRet = FALSE;
	}
	return bRet;
}

BOOL DiffFileData::Diff2Files(struct change ** diffs, int depth,
	int * bin_status, BOOL bMovedBlocks)
{
	BOOL bRet = TRUE;
	__try
	{
		*diffs = diff_2_files (m_inf, depth, bin_status, bMovedBlocks);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*diffs = NULL;
		bRet = FALSE;
	}
	return bRet;
}

struct FileHandle
{
	FileHandle() : m_fp(0) { }
	void Assign(FILE * fp) { Close(); m_fp = fp; }
	void Close() { if (m_fp) { fclose(m_fp); m_fp = 0; } }
	~FileHandle() { Close(); }
	FILE * m_fp;
};

/** @brief Compare two specified files, byte-by-byte */
int DiffFileData::byte_compare_files()
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
		fp[i] = _tfopen(m_sFilepath[i], _T("r"));
		if (!fp[i])
			return DIFFCODE::CMPERR;
		fhd[i].Assign(fp[i]);
	}

	int bfstart[2], bfend[2]; // area of buffer currently holding data
	bool eof[2]; // if we've finished file

	// initialize our buffer pointers and end of file flags
	for (i=0; i<2; ++i)
	{
		bfstart[i] = bfend[i] = 0;
		eof[i] = false;
	}

	ByteComparator comparator(ignore_case_flag, ignore_space_change_flag
		, ignore_all_space_flag, ignore_eol_diff, ignore_blank_lines_flag);

	while (1)
	{
		// load or update buffers as appropriate
		for (i=0; i<2; ++i)
		{
			if (!eof[i] && bfstart[i]==countof(buff[i]))
			{
				bfstart[i]=bfend[i] = 0;
			}
			if (!eof[i] && bfend[i]<countof(buff[i])-1)
			{
				int rtn = fread(buff[i], 1, countof(buff[i]), fp[i]);
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

		BOOL bBin0 = isBinaryBuf(&buff[0][bfstart[0]], &buff[0][bfend[0]]);
		BOOL bBin1 = isBinaryBuf(&buff[1][bfstart[1]], &buff[1][bfend[1]]);

		// If either buffer is binary file, don't bother ignoring differences
		// for whitespaces or EOLs anymore.
		if (bBin0 || bBin1)
		{
			diffcode |= DIFFCODE::BIN;
			comparator.ResetIgnore();
		}

		// are these two buffers the same?
		if (!comparator.CompareBuffers(ptr0, ptr1, end0, end1, eof[0], eof[1]))
			return diffcode | DIFFCODE::DIFF;


		// did we finish both files?
		if (eof[0] && eof[1])
		{
			if (ptr0 == end0 && ptr1 == end1)
				return diffcode | DIFFCODE::SAME;
			else
				return diffcode | DIFFCODE::DIFF;
		}

		// move our current pointers over what we just compared
		ASSERT(ptr0 >= orig0);
		ASSERT(ptr1 >= orig1);
		bfstart[0] += ptr0-orig0;
		bfstart[1] += ptr1-orig1;
	}
}

/**
 * @brief Check if given buffer contains zero-bytes.
 * If buffer has zero-bytes we determine it contains binary data.
 * @param [in] bufBegin Start address of the buffer.
 * @param [in] bufEnd End address of the buffer.
 * @return TRUE if zero-bytes found.
 */
inline BOOL isBinaryBuf(char * bufBegin, char * bufEnd)
{
	for (char * pByte = bufBegin; pByte <= bufEnd; ++pByte)
	{
		if (*pByte == 0x0)
			return TRUE;
	}
	return FALSE;
}
