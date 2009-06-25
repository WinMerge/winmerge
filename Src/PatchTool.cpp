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
// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"
#include "UnicodeString.h"
#include "DiffWrapper.h"
#include "PatchTool.h"
#include "PatchDlg.h"
#include "coretools.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Default constructor.
 */
CPatchTool::CPatchTool()
: m_pDlgPatch(NULL)
{
}

/**
 * @brief Default destructor.
 */
CPatchTool::~CPatchTool()
{
	delete m_pDlgPatch;
}

/** 
 * @brief Adds files to list for patching.
 * @param [in] file1 First file to add.
 * @param [in] file2 Second file to add.
 */
void CPatchTool::AddFiles(const String &file1, const String &file2)
{
	PATCHFILES files;
	files.lfile = file1;
	files.rfile = file2;

	// TODO: Read and add file's timestamps
	m_fileList.push_back(files);
}

/**
 * @brief Add files with alternative paths.
 * This function adds files with alternative paths. Alternative path is the
 * one that is added to the patch file. So while @p file1 and @p file2 are
 * paths in disk (can be temp file names), @p altPath1 and @p altPath2 are
 * "visible " paths printed to the patch file.
 * @param [in] file1 First path in disk.
 * @param [in] altPath1 First path as printed to the patch file.
 * @param [in] file2 Second path in disk.
 * @param [in] altPath2 Second path as printed to the patch file.
 */
void CPatchTool::AddFiles(const String &file1, const String &altPath1,
		const String &file2, const String &altPath2)
{
	PATCHFILES files;
	files.lfile = file1;
	files.rfile = file2;
	files.pathLeft = altPath1;
	files.pathRight = altPath2;

	// TODO: Read and add file's timestamps
	m_fileList.push_back(files);
}

/** 
 * @brief Create a patch from files given.
 * @note Files can be given using AddFiles() or selecting using
 * CPatchDlg.
 */
int CPatchTool::CreatePatch()
{
	DIFFSTATUS status;
	BOOL bResult = TRUE;
	BOOL bDiffSuccess;
	int retVal = 0;

	if (m_pDlgPatch == NULL)
		m_pDlgPatch = new CPatchDlg();

	// If files already inserted, add them to dialog
    for(std::vector<PATCHFILES>::iterator iter = m_fileList.begin(); iter != m_fileList.end(); ++iter)
    {
        m_pDlgPatch->AddItem(*iter);
	}

	if (ShowDialog())
	{
		String path;
		SplitFilename(m_pDlgPatch->m_fileResult, &path, NULL, NULL);
		if (!paths_CreateIfNeeded(path.c_str()))
		{
			LangMessageBox(IDS_FOLDER_NOTEXIST, MB_OK | MB_ICONSTOP);
			return 0;
		}

		// Select patch create -mode
		m_diffWrapper.SetCreatePatchFile((LPCTSTR)m_pDlgPatch->m_fileResult);
		m_diffWrapper.SetAppendFiles(m_pDlgPatch->m_appendFile);
		m_diffWrapper.SetPrediffer(NULL);

		int fileCount = m_pDlgPatch->GetItemCount();
		for (int index = 0; index < fileCount; index++)
		{
			const PATCHFILES& files = m_pDlgPatch->GetItemAt(index);
			
			// Set up DiffWrapper
			m_diffWrapper.SetPaths(files.lfile, files.rfile, FALSE);
			m_diffWrapper.SetAlternativePaths(files.pathLeft, files.pathRight);
			m_diffWrapper.SetCompareFiles(files.lfile, files.rfile);
			bDiffSuccess = m_diffWrapper.RunFileDiff();
			m_diffWrapper.GetDiffStatus(&status);

			if (!bDiffSuccess)
			{
				LangMessageBox(IDS_FILEERROR, MB_ICONSTOP);
				bResult = FALSE;
				break;
			}
			else if (status.bBinaries)
			{
				LangMessageBox(IDS_CANNOT_CREATE_BINARYPATCH, MB_ICONSTOP);
				bResult = FALSE;
				break;
			}
			else if (status.bPatchFileFailed)
			{
				CString errMsg;
				LangFormatString1(errMsg, IDS_FILEWRITE_ERROR, m_pDlgPatch->m_fileResult);
				AfxMessageBox(errMsg, MB_ICONSTOP);
				bResult = FALSE;
				break;
			}

			// Append next files...
			m_diffWrapper.SetAppendFiles(TRUE);
		}
		
		if (bResult && fileCount > 0)
		{
			LangMessageBox(IDS_DIFF_SUCCEEDED, MB_ICONINFORMATION|MB_DONT_DISPLAY_AGAIN,
				            IDS_DIFF_SUCCEEDED);
			
			m_sPatchFile = (LPCTSTR)m_pDlgPatch->m_fileResult;
			m_bOpenToEditor = m_pDlgPatch->m_openToEditor;
			retVal = 1;
		}
	}
	m_pDlgPatch->ClearItems();
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

	if (m_pDlgPatch->DoModal() == IDOK)
	{
		// There must be one filepair
		if (m_pDlgPatch->GetItemCount() < 1)
			bRetVal = FALSE;

		// These two are from dropdown list - can't be wrong
		patchOptions.outputStyle = m_pDlgPatch->m_outputStyle;
		patchOptions.nContext = m_pDlgPatch->m_contextLines;

		// Checkbox - can't be wrong
		patchOptions.bAddCommandline = m_pDlgPatch->m_includeCmdLine;
		m_diffWrapper.SetPatchOptions(&patchOptions);

		// These are from checkboxes and radiobuttons - can't be wrong
		diffOptions.nIgnoreWhitespace = m_pDlgPatch->m_whitespaceCompare;
		diffOptions.bIgnoreBlankLines = !!m_pDlgPatch->m_ignoreBlanks;
		m_diffWrapper.SetAppendFiles(m_pDlgPatch->m_appendFile);

		// Use this because non-sensitive setting can't write
		// patch file EOLs correctly
		diffOptions.bIgnoreEol = false;
		
		diffOptions.bIgnoreCase = m_pDlgPatch->m_caseSensitive == FALSE;
		m_diffWrapper.SetOptions(&diffOptions);
	}
	else
		return FALSE;

	return bRetVal;
}

/** 
 * @brief Returns filename and path for patch-file
 */
String CPatchTool::GetPatchFile() const
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
