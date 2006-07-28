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
 * @file  files.cpp
 *
 * @brief Code file routines
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "DiffWrapper.h"
#include "patchDlg.h"
#include "patchtool.h"
#include "Coretools.h"
#include "paths.h"

/** 
 * @brief Adds files to list for patching
 */
void CPatchTool::AddFiles(const CString &file1, const CString &file2)
{
	PATCHFILES files;
	files.lfile = file1;
	files.rfile = file2;

	// TODO: Read and add file's timestamps
	m_fileList.AddTail(files);
}

void CPatchTool::AddFiles(const CString &file1, const CString &altPath1,
		const CString &file2, const CString &altPath2)
{
	PATCHFILES files;
	files.lfile = file1;
	files.rfile = file2;
	files.pathLeft = altPath1;
	files.pathRight = altPath2;

	// TODO: Read and add file's timestamps
	m_fileList.AddTail(files);
}

/** 
 * @brief Create patch from files given
 * @note Files can be given using AddFiles() or selecting using
 * CPatchDlg.
 */
int CPatchTool::CreatePatch()
{
	DIFFSTATUS status;
	BOOL bResult = TRUE;
	BOOL bDiffSuccess;
	int retVal = 0;

	// If files already inserted, add them to dialog
	int count = m_fileList.GetCount();
	POSITION pos = m_fileList.GetHeadPosition();

	for (int i = 0; i < count; i++)
	{
		PATCHFILES files = m_fileList.GetNext(pos);
		m_dlgPatch.AddItem(files);
	}

	if (ShowDialog())
	{
		CString path;
		SplitFilename(m_dlgPatch.m_fileResult, &path, NULL, NULL);
		if (!paths_CreateIfNeeded(path))
		{
			AfxMessageBox(IDS_FOLDER_NOTEXIST, MB_OK | MB_ICONSTOP);
			return 0;
		}

		// Select patch create -mode
		m_diffWrapper.SetCreatePatchFile(m_dlgPatch.m_fileResult);
		m_diffWrapper.SetAppendFiles(m_dlgPatch.m_appendFile);
		m_diffWrapper.SetPrediffer(NULL);

		int fileCount = m_dlgPatch.GetItemCount();
		POSITION pos = m_dlgPatch.GetFirstItem();

		for (int i = 0; i < fileCount; i++)
		{
			PATCHFILES files = m_dlgPatch.GetNextItem(pos);
			
			// Set up DiffWrapper
			m_diffWrapper.SetPaths(files.lfile, files.rfile, FALSE);
			m_diffWrapper.SetAlternativePaths(files.pathLeft, files.pathRight);
			m_diffWrapper.SetCompareFiles(files.lfile, files.rfile);
			bDiffSuccess = m_diffWrapper.RunFileDiff();
			m_diffWrapper.GetDiffStatus(&status);

			if (!bDiffSuccess)
			{
				AfxMessageBox(IDS_FILEERROR, MB_ICONSTOP);
				bResult = FALSE;
				break;
			}
			else if (status.bBinaries)
			{
				AfxMessageBox(IDS_CANNOT_CREATE_BINARYPATCH, MB_ICONSTOP);
				bResult = FALSE;
				break;
			}
			else if (status.bPatchFileFailed)
			{
				CString errMsg;
				AfxFormatString1(errMsg, IDS_FILEWRITE_ERROR, m_dlgPatch.m_fileResult);
				AfxMessageBox(errMsg, MB_ICONSTOP);
				bResult = FALSE;
				break;
			}

			// Append next files...
			m_diffWrapper.SetAppendFiles(TRUE);
		}
		
		if (bResult && fileCount > 0)
		{
			AfxMessageBox(IDS_DIFF_SUCCEEDED, MB_ICONINFORMATION|MB_DONT_DISPLAY_AGAIN,
				            IDS_DIFF_SUCCEEDED);
			
			m_sPatchFile = m_dlgPatch.m_fileResult;
			m_bOpenToEditor = m_dlgPatch.m_openToEditor;
			retVal = 1;
		}
	}
	m_dlgPatch.ClearItems();
	return retVal;
}

/** 
 * @brief Show patch options dialog and check options selected.
 * @return TRUE if user wants to create a patch (didn't cancel dialog).
 */
BOOL CPatchTool::ShowDialog()
{
	DIFFOPTIONS diffOptions = {0};
	PATCHOPTIONS patchOptions;
	BOOL bRetVal = TRUE;

	if (m_dlgPatch.DoModal() == IDOK)
	{
		// There must be one filepair
		if (m_dlgPatch.GetItemCount() < 1)
			bRetVal = FALSE;

		// These two are from dropdown list - can't be wrong
		patchOptions.outputStyle = m_dlgPatch.m_outputStyle;
		patchOptions.nContext = m_dlgPatch.m_contextLines;

		// Checkbox - can't be wrong
		patchOptions.bAddCommandline = m_dlgPatch.m_includeCmdLine;
		m_diffWrapper.SetPatchOptions(&patchOptions);

		// These are from checkboxes and radiobuttons - can't be wrong
		diffOptions.nIgnoreWhitespace = m_dlgPatch.m_whitespaceCompare;
		diffOptions.bIgnoreBlankLines = m_dlgPatch.m_ignoreBlanks;
		m_diffWrapper.SetAppendFiles(m_dlgPatch.m_appendFile);

		// Use this because non-sensitive setting can't write
		// patch file EOLs correctly
		diffOptions.bIgnoreEol = FALSE;
		
		diffOptions.bIgnoreCase = !m_dlgPatch.m_caseSensitive;
		m_diffWrapper.SetOptions(&diffOptions);
	}
	else
		return FALSE;

	return bRetVal;
}

/** 
 * @brief Returns filename and path for patch-file
 */
CString CPatchTool::GetPatchFile() const
{
	return m_sPatchFile;
}

/** 
 * @brief Returns TRUE if user wants to open patch file
 * to external editor (specified in WinMerge options).
 */
BOOL CPatchTool::GetOpenToEditor() const
{
	return m_bOpenToEditor;
}
