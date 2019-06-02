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
 * @file  PatchTool.cpp
 *
 * @brief Code file routines
 */

#include "StdAfx.h"
#include "PatchTool.h"
#include "UnicodeString.h"
#include "DiffWrapper.h"
#include "PathContext.h"
#include "PatchDlg.h"
#include "paths.h"
#include "Merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Default constructor.
 */
CPatchTool::CPatchTool() : m_bOpenToEditor(false)
{
}

/**
 * @brief Default destructor.
 */
CPatchTool::~CPatchTool()
{
}

/** 
 * @brief Adds files to list for patching.
 * @param [in] file1 First file to add.
 * @param [in] file2 Second file to add.
 */
void CPatchTool::AddFiles(const String &file1, const String &file2)
{
	PATCHFILES tFiles;
	tFiles.lfile = file1;
	tFiles.rfile = file2;

	// TODO: Read and add file's timestamps
	m_fileList.push_back(tFiles);
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
	PATCHFILES tFiles;
	tFiles.lfile = file1;
	tFiles.rfile = file2;
	tFiles.pathLeft = altPath1;
	tFiles.pathRight = altPath2;

	// TODO: Read and add file's timestamps
	m_fileList.push_back(tFiles);
}

/** 
 * @brief Create a patch from files given.
 * @note Files can be given using AddFiles() or selecting using
 * CPatchDlg.
 */
int CPatchTool::CreatePatch()
{
	DIFFSTATUS status;
	int retVal = 0;

	CPatchDlg dlgPatch;

	// If files already inserted, add them to dialog
    for(std::vector<PATCHFILES>::iterator iter = m_fileList.begin(); iter != m_fileList.end(); ++iter)
    {
        dlgPatch.AddItem(*iter);
	}

	if (ShowDialog(&dlgPatch))
	{
		bool bResult = true;

		if (!paths::CreateIfNeeded(paths::GetPathOnly(dlgPatch.m_fileResult)))
		{
			LangMessageBox(IDS_FOLDER_NOTEXIST, MB_OK | MB_ICONSTOP);
			return 0;
		}

		// Select patch create -mode
		m_diffWrapper.SetCreatePatchFile(dlgPatch.m_fileResult);
		m_diffWrapper.SetAppendFiles(dlgPatch.m_appendFile);
		m_diffWrapper.SetPrediffer(nullptr);

		size_t fileCount = dlgPatch.GetItemCount();

		m_diffWrapper.WritePatchFileHeader(dlgPatch.m_outputStyle, dlgPatch.m_appendFile);
		m_diffWrapper.SetAppendFiles(true);

		for (size_t index = 0; index < fileCount; index++)
		{
			const PATCHFILES& tFiles = dlgPatch.GetItemAt(index);
			String filename1 = tFiles.lfile.length() == 0 ? _T("NUL") : tFiles.lfile;
			String filename2 = tFiles.rfile.length() == 0 ? _T("NUL") : tFiles.rfile;
			
			// Set up DiffWrapper
			m_diffWrapper.SetPaths(PathContext(filename1, filename2), false);
			m_diffWrapper.SetAlternativePaths(PathContext(tFiles.pathLeft, tFiles.pathRight));
			m_diffWrapper.SetCompareFiles(PathContext(tFiles.lfile, tFiles.rfile));
			bool bDiffSuccess = m_diffWrapper.RunFileDiff();
			m_diffWrapper.GetDiffStatus(&status);

			if (!bDiffSuccess)
			{
				LangMessageBox(IDS_FILEERROR, MB_ICONSTOP);
				bResult = false;
				break;
			}
			else if (status.bBinaries)
			{
				LangMessageBox(IDS_CANNOT_CREATE_BINARYPATCH, MB_ICONSTOP);
				bResult = false;
				break;
			}
			else if (status.bPatchFileFailed)
			{
				String errMsg = strutils::format_string1(_("Could not write to file %1."), dlgPatch.m_fileResult);
				AfxMessageBox(errMsg.c_str(), MB_ICONSTOP);
				bResult = false;
				break;
			}
		}
		
		m_diffWrapper.WritePatchFileTerminator(dlgPatch.m_outputStyle);

		if (bResult && fileCount > 0)
		{
			LangMessageBox(IDS_DIFF_SUCCEEDED, MB_ICONINFORMATION|MB_DONT_DISPLAY_AGAIN,
				            IDS_DIFF_SUCCEEDED);
			
			m_sPatchFile = dlgPatch.m_fileResult;
			m_bOpenToEditor = dlgPatch.m_openToEditor;
			retVal = 1;
		}
	}
	dlgPatch.ClearItems();
	if (retVal)
	{
		if (m_bOpenToEditor)
			theApp.OpenFileToExternalEditor(m_sPatchFile);
	}
	return retVal;
}

/** 
 * @brief Show patch options dialog and check options selected.
 * @return `true` if user wants to create a patch (didn't cancel dialog).
 */
bool CPatchTool::ShowDialog(CPatchDlg *pDlgPatch)
{
	DIFFOPTIONS diffOptions = {0};
	PATCHOPTIONS patchOptions;
	bool bRetVal = true;

	if (pDlgPatch->DoModal() == IDOK)
	{
		// There must be one filepair
		if (pDlgPatch->GetItemCount() < 1)
			bRetVal = false;

		// These two are from dropdown list - can't be wrong
		patchOptions.outputStyle = pDlgPatch->m_outputStyle;
		patchOptions.nContext = pDlgPatch->m_contextLines;

		// Checkbox - can't be wrong
		patchOptions.bAddCommandline = pDlgPatch->m_includeCmdLine;
		m_diffWrapper.SetPatchOptions(&patchOptions);

		// These are from checkboxes and radiobuttons - can't be wrong
		diffOptions.nIgnoreWhitespace = pDlgPatch->m_whitespaceCompare;
		diffOptions.bIgnoreBlankLines = pDlgPatch->m_ignoreBlanks;
		m_diffWrapper.SetAppendFiles(pDlgPatch->m_appendFile);

		// Use this because non-sensitive setting can't write
		// patch file EOLs correctly
		diffOptions.bIgnoreEol = pDlgPatch->m_ignoreEOLDifference;
		
		diffOptions.bIgnoreCase = !pDlgPatch->m_caseSensitive;
		diffOptions.nDiffAlgorithm = pDlgPatch->m_diffAlgorithm;
		diffOptions.bIndentHeuristic = pDlgPatch->m_indentHeuristic;
		m_diffWrapper.SetOptions(&diffOptions);
	}
	else
		return false;

	return bRetVal;
}
