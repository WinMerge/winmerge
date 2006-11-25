/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  FileOrFolderSelect.cpp
 *
 * @brief Implementation of the file and folder selection routines.
 */

#include "stdafx.h"
#include "FileOrFolderSelect.h"
#include "coretools.h"
#include "paths.h"

/**
 * @brief Helper function for selecting dir/file
 * @param [in] parent Handle to parent window. Can be a NULL, but then
 *     CMainFrame is used which can cause modality problems.
 * @param [out] path Selected path is returned in this string
 * @param [in] initialPath Initial path (and file) shown when dialog is opened
 * @param [in] title Title for path selection dialog
 * @param [in] filterid 0 or STRING ID for filter string
 *     - 0 means "All files (*.*)"
 * @param [in] is_open Selects Open/Save -dialog
 */
BOOL SelectFile(HWND parent, CString& path, LPCTSTR initialPath /*=NULL*/,
		LPCTSTR title /*= _T("Open")*/, UINT filterid /*=0*/,
		BOOL is_open /*=TRUE*/)
{
	path.Empty(); // Clear output param

	// This will tell common file dialog what to show
	// and also this will hold its return value
	CString sSelectedFile;

	// check if specified path is a file
	if (initialPath && initialPath[0])
	{
		// If initial path info includes a file
		// we put the bare filename into sSelectedFile
		// so the common file dialog will start up with that file selected
		if (paths_DoesPathExist(initialPath) == IS_EXISTING_FILE)
		{
			SplitFilename(initialPath, 0, &sSelectedFile, 0);
		}
	}

	if (parent == NULL)
		parent = AfxGetMainWnd()->GetSafeHwnd();
	
	if (!filterid)
		filterid = IDS_ALLFILES;
	CString filters = LoadResString(filterid);

	// Convert extension mask from MFC style separators ('|')
	//  to Win32 style separators ('\0')
	LPTSTR filtersStr = filters.GetBuffer(0);
	ConvertFilter(filtersStr);
	filters.ReleaseBuffer();

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = parent;
	ofn.lpstrFilter = filtersStr;
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = sSelectedFile.GetBuffer(MAX_PATH);
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = initialPath;
	ofn.lpstrTitle = title;
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;

	BOOL bRetVal = FALSE;
	if (is_open)
		bRetVal = GetOpenFileName(&ofn);
	else
		bRetVal = GetSaveFileName(&ofn);
	// common file dialog populated sSelectedFile variable's buffer
	sSelectedFile.ReleaseBuffer();
	SetCurrentDirectory(paths_GetWindowsDirectory()); // Free handle held by GetOpenFileName

	if (bRetVal)
		path = sSelectedFile;
	
	return bRetVal;
}

/** 
 * @brief Helper function for selecting directory
 * @param [out] path Selected path is returned in this string
 * @param [in] root_path Initial path shown when dialog is opened
 * @param [in] title Title for path selection dialog
 * @param [in] hwndOwner Handle to owner window or NULL
 * @return TRUE if valid folder selected (not cancelled)
 */
BOOL SelectFolder(CString& path, LPCTSTR root_path /*=NULL*/, 
			LPCTSTR title /*=NULL*/, 
			HWND hwndOwner /*=NULL*/) 
{
	UNREFERENCED_PARAMETER(root_path);
	BROWSEINFO bi;
	LPMALLOC pMalloc;
	LPITEMIDLIST pidl;
	TCHAR szPath[MAX_PATH] = {0};
	BOOL bRet = FALSE;
	
	bi.hwndOwner = hwndOwner;
	bi.pidlRoot = NULL;  // Start from desktop folder
	bi.pszDisplayName = szPath;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	bi.lpfn = NULL;
	bi.lParam = NULL;

	pidl = SHBrowseForFolder(&bi);

	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szPath))
		{
			path = szPath;
			bRet = TRUE;
		}

		SHGetMalloc(&pMalloc);
		pMalloc->Free(pidl);
		pMalloc->Release();
	}
	return bRet;
}

/** 
 * @brief Helper function for converting filter format.
 *
 * MFC functions separate filter strings with | char which is also
 * good choice to safe into resource. But WinAPI32 functions we use
 * needs '\0' as separator. This function replaces '|'s with '\0's.
 *
 * @param [in,out] filterStr
 * - in Mask string to convert
 * - out Converted string
 */
void ConvertFilter(LPTSTR filterStr)
{
	TCHAR *ch = 0;
	TCHAR *strPtr = filterStr;
	do
	{
		ch = _tcschr(strPtr, '|');
		if (ch)
		{
			strPtr = ch + 1;
			*ch = '\0';
		}
	}
	while (ch != 0);
}
