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
#include "diffwrapper.h"
#include "diff.h"

extern int recursive;

CDiffWrapper::CDiffWrapper()
{
	DIFFOPTIONS options = {0};
	ZeroMemory(&m_settings, sizeof(DIFFSETTINGS));
	ZeroMemory(&m_globalSettings, sizeof(DIFFSETTINGS));
	ZeroMemory(&m_status, sizeof(DIFFSTATUS));
	m_bCreatePatchFile = FALSE;
	m_bUseDiffList = FALSE;
	m_bAddCmdLine = TRUE;
	m_bAppendFiles = FALSE;
	m_nDiffs = 0;

	// Read options from registry and convert
	// to internal format
	ReadDiffOptions(&options);
	InternalSetOptions(&options);
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
 * @brief Get options used for patch creation
 */
void CDiffWrapper::GetPatchOptions(PATCHOPTIONS *options)
{
	ASSERT(options);
	options->context = m_settings.context;
	options->outputStyle = m_settings.outputStyle;
}

/**
 * @brief Set options used for patch creation
 */
void CDiffWrapper::SetPatchOptions(PATCHOPTIONS *options)
{
	ASSERT(options);
	m_settings.context = options->context;
	m_settings.outputStyle = options->outputStyle;
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

	BOOL bRetStatus = FALSE;
	SwapToInternalSettings();

	if (m_bUseDiffList)
		m_nDiffs = m_diffs->GetSize();

	// perform rescan
	struct file_data inf[2] = {0};
	char *free0 = NULL,*free1 = NULL;
	CString sdir0, sdir1, sname0, sname1, sext0, sext1;
	int val,failed=0, depth=0;
	bool same_files = FALSE;
	struct change *e, *p;
	struct change *script = NULL;

	SplitFilename(m_sFile1, &sdir0, &sname0, NULL);
	SplitFilename(m_sFile2, &sdir1, &sname1, NULL);
	ZeroMemory(&inf[0], sizeof(inf[0]));
	ZeroMemory(&inf[1], sizeof(inf[1]));

	/* Both exist and neither is a directory.  */
	int o_binary = always_text_flag ? 0:O_BINARY;

	/* Open the files and record their descriptors.  */
	if (sdir0.IsEmpty())
		inf[0].name = T2CA(sname0);
	else
		inf[0].name = free0 = dir_file_pathname (T2CA(sdir0), T2CA(sname0));
	inf[0].desc = -2;
	if (sdir1.IsEmpty())
		inf[1].name = T2CA(sname1);
	else
		inf[1].name = free1 = dir_file_pathname (T2CA(sdir1), T2CA(sname1));
	inf[1].desc = -2;
	if (inf[0].desc == -2)
	{
		if ((inf[0].desc = open (inf[0].name, O_RDONLY|o_binary, 0)) < 0)
		{
			perror_with_name (inf[0].name);
			failed = 1;
		}
		if (inf[1].desc == -2)
		{
			if (same_files)
				inf[1].desc = inf[0].desc;
			else if ((inf[1].desc = open (inf[1].name, O_RDONLY|o_binary, 0)) < 0)
			{
				perror_with_name (inf[1].name);
				failed = 1;
			}
			
			/* Compare the files, if no error was found.  */
			int diff_flag = 0;

			// Diff files. depth is zero because we are not comparind dirs
			script = diff_2_files (inf, 0, &diff_flag);

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
						fprintf(outfile, "diff%s %s %s\n", 
							switches, inf[0].name , inf[1].name);
					}

					// Output patchfile
					switch (output_style)
					{
					case OUTPUT_CONTEXT:
						print_context_header(files, 0);
						print_context_script(script, 0);
						break;
						
					case OUTPUT_UNIFIED:
						print_context_header(files, 1);
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

			cleanup_file_buffers(inf);
			
			/* Close the file descriptors.  */
			if (inf[0].desc >= 0 && close (inf[0].desc) != 0)
			{
				perror_with_name (inf[0].name);
				val = 2;
			}
			if (inf[1].desc >= 0 && inf[0].desc != inf[1].desc
				&& close (inf[1].desc) != 0)
			{
				perror_with_name (inf[1].name);
				val = 2;
			}

			m_status.bBinaries = diff_flag > 0;
			m_status.bLeftMissingNL = inf[0].missing_newline;
			m_status.bRightMissingNL = inf[1].missing_newline;
		}
	}
	
	if (free0)
		free (free0);
	if (free1)
		free (free1);

	SwapToGlobalSettings();

	bRetStatus = !failed;
	if (failed)
	{
		// Do not return error status when binary file is found, as we
		// return binary status in status struct
		if (m_status.bBinaries)
			bRetStatus = TRUE;
	}

	return bRetStatus;
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

/**
 * @brief Static function for reading diffoptions from registry
 */
void CDiffWrapper::ReadDiffOptions(DIFFOPTIONS *options)
{
	options->nIgnoreWhitespace = ::AfxGetApp()->GetProfileInt(_T("Settings"), _T("IgnoreSpace"), 1);
	options->bIgnoreBlankLines = ::AfxGetApp()->GetProfileInt(_T("Settings"), _T("IgnoreBlankLines"), FALSE)!=0;
	options->bEolSensitive = ::AfxGetApp()->GetProfileInt(_T("Settings"), _T("EolSensitive"), FALSE)!=0;
	options->bIgnoreCase = ::AfxGetApp()->GetProfileInt(_T("Settings"), _T("IgnoreCase"), FALSE)!=0;
}

/**
 * @brief Static function for writing diffoptions to registry
 */
void CDiffWrapper::WriteDiffOptions(DIFFOPTIONS *options)
{
	::AfxGetApp()->WriteProfileInt(_T("Settings"), _T("IgnoreSpace"), options->nIgnoreWhitespace);
	::AfxGetApp()->WriteProfileInt(_T("Settings"), _T("EolSensitive"), options->bEolSensitive);
	::AfxGetApp()->WriteProfileInt(_T("Settings"), _T("IgnoreBlankLines"), options->bIgnoreBlankLines);
	::AfxGetApp()->WriteProfileInt(_T("Settings"), _T("IgnoreCase"), options->bIgnoreCase);
}
