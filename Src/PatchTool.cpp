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

/** 
 * @brief Adds files to list for patching
 */
void CPatchTool::AddFiles(CString file1, CString file2)
{
	PATCHFILES files;
	files.lfile = file1;
	files.rfile = file2;

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
	DIFFOPTIONS diffOptions = {0};
	PATCHOPTIONS patchOptions;
	DIFFSTATUS status = {0};
	CPatchDlg dlg;
	BOOL bResult = TRUE;
	BOOL bDiffSuccess;

	// If files already inserted, add them to dialog
	int count = m_fileList.GetCount();
	POSITION pos = m_fileList.GetHeadPosition();

	for (int i = 0; i < count; i++)
	{
		PATCHFILES files = m_fileList.GetNext(pos);
		dlg.AddItem(files);
	}

	if (dlg.DoModal() == IDOK)
	{
		patchOptions.outputStyle = dlg.m_outputStyle;
		patchOptions.context = dlg.m_contextLines;
		m_diffWrapper.SetPatchOptions(&patchOptions);

		diffOptions.nIgnoreWhitespace = dlg.m_whitespaceCompare;
		diffOptions.bIgnoreBlankLines = dlg.m_ignoreBlanks;
		diffOptions.bEolSensitive = TRUE;
		m_diffWrapper.SetAppendFiles(dlg.m_appendFile);

		m_diffWrapper.SetUseDiffList(FALSE);
		m_diffWrapper.SetCreatePatchFile(TRUE);
		m_diffWrapper.SetPatchFile(dlg.m_fileResult);

		diffOptions.bIgnoreCase = !dlg.m_caseSensitive;
		m_diffWrapper.SetOptions(&diffOptions);
		
		int fileCount = dlg.GetItemCount();
		POSITION pos = dlg.GetFirstItem();

		for (int i = 0; i < fileCount; i++)
		{
			PATCHFILES files = dlg.GetNextItem(pos);
			CString filename1 = files.lfile;
			CString filename2 = files.rfile;
			
			// Set up DiffWrapper
			m_diffWrapper.SetCompareFiles(filename1, filename2);
			bDiffSuccess = m_diffWrapper.RunFileDiff();
			m_diffWrapper.GetDiffStatus(&status);

			if (!bDiffSuccess)
			{
				bResult = FALSE;
				CString errMsg;
				AfxFormatString1(errMsg, IDS_FILEWRITE_ERROR, dlg.m_fileResult);
				AfxMessageBox(errMsg, MB_ICONSTOP);
				break;
			}

			// If patch file could not be written do not continue to other files
			if (status.bPatchFileFailed)
			{
				bResult = FALSE;
				break;
			}

			// Append next files...
			m_diffWrapper.SetAppendFiles(TRUE);
		}
		if (bResult)
			AfxMessageBox(IDS_DIFF_SUCCEEDED, MB_ICONINFORMATION);
	}
	dlg.ClearItems();
	return 1;
}
