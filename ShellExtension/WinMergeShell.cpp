/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//    You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
// Look at http://www.codeproject.com/shell/ for excellent guide
// to Windows Shell programming by Michael Dunn.
// 
// This extension needs two registry values to be defined:
//  HKEY_CURRENT_USER\Software\Thingamahoochie\WinMerge\ContextMenuEnabled
//   defines if context menu is shown (extension enabled)
//  HKEY_CURRENT_USER\Software\Thingamahoochie\WinMerge\Executable
//   contains path to program to run (can be batch file too)
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  WinMergeShell.cpp
 *
 * @brief Implementation of the ShellExtension class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "ShellExtension.h"
#include "WinMergeShell.h"
#include "RegKey.h"
#include "coretools.h"

// Registry path to WinMerge 
static LPCTSTR f_RegDir = _T("Software\\Thingamahoochie\\WinMerge");

// Registry values
static LPCTSTR f_RegValueEnabled = _T("ContextMenuEnabled");
static LPCTSTR f_RegValuePath = _T("Executable");

/////////////////////////////////////////////////////////////////////////////
// CWinMergeShell

/// Default constructor, loads icon bitmap
CWinMergeShell::CWinMergeShell()
{
	m_nSelectedItems = 0;
	m_hMergeBmp = LoadBitmap(_Module.GetModuleInstance(),
			MAKEINTRESOURCE(IDB_WINMERGE));
}

/// Reads selected paths
HRESULT CWinMergeShell::Initialize(LPCITEMIDLIST pidlFolder,
		LPDATAOBJECT pDataObj, HKEY hProgID)
{
	FORMATETC fmt = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	STGMEDIUM stg = {TYMED_HGLOBAL};
	HDROP hDropInfo;

	// Look for CF_HDROP data in the data object.
	if (FAILED(pDataObj->GetData(&fmt, &stg)))
		// Nope! Return an "invalid argument" error back to Explorer.
		return E_INVALIDARG;

	// Get a pointer to the actual data.
	hDropInfo = (HDROP) GlobalLock(stg.hGlobal);

	// Make sure it worked.
	if (NULL == hDropInfo)
		return E_INVALIDARG;

	// Sanity check & make sure there is at least one filename.
	UINT uNumFilesDropped = DragQueryFile (hDropInfo, 0xFFFFFFFF, NULL, 0);
	m_nSelectedItems = uNumFilesDropped;

	if (uNumFilesDropped == 0)
	{
		GlobalUnlock(stg.hGlobal);
		ReleaseStgMedium(&stg);
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	// Get all file names.
	for (WORD x = 0 ; x < uNumFilesDropped; x++)
	{
		// Get the number of bytes required by the file's full pathname
		UINT wPathnameSize = DragQueryFile(hDropInfo, x, NULL, 0);

		// Allocate memory to contain full pathname & zero byte
		wPathnameSize += 1;
		LPTSTR npszFile = (TCHAR *) new TCHAR[wPathnameSize];

		// If not enough memory, skip this one
		if (npszFile == NULL)
			continue;

		// Copy the pathname into the buffer
		DragQueryFile(hDropInfo, x, npszFile, wPathnameSize);

		if (x < 2)
			m_strPaths[x] = npszFile;

		delete[] npszFile;
	}
	GlobalUnlock(stg.hGlobal);
	ReleaseStgMedium(&stg);

    return hr;
}

/// Adds context menu item
HRESULT CWinMergeShell::QueryContextMenu(HMENU hmenu, UINT uMenuIndex,
		UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	CString strMenu;
	if (strMenu.LoadString(IDS_CONTEXT_MENU))
	{
		// Check if user wants to use context menu
		CRegKeyEx reg;
		if (reg.Open(HKEY_CURRENT_USER, f_RegDir) != ERROR_SUCCESS)
			return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

		// This will be bit mask, although now there is only one bit defined
		DWORD dwContextEnabled = reg.ReadDword(f_RegValueEnabled, 0);

		if (dwContextEnabled & 0x1)
		{
			InsertMenu(hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd, strMenu);
			
			// Add bitmap
			if (m_hMergeBmp != NULL)
				SetMenuItemBitmaps(hmenu, uMenuIndex, MF_BYPOSITION, m_hMergeBmp, NULL);
			
			// Show menu item as grayed if more than two items selected
			if (m_nSelectedItems > 2)
				EnableMenuItem(hmenu, uMenuIndex, MF_BYPOSITION | MF_GRAYED);

			return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 1);
		}
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
	}
	else
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
}

/// Gets string shown explorer's status bar when menuitem selected
HRESULT CWinMergeShell::GetCommandString(UINT idCmd, UINT uFlags,
		UINT* pwReserved, LPSTR pszName, UINT  cchMax)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	USES_CONVERSION;

	// Check idCmd, it must be 0 since we have only one menu item.
	if (idCmd != 0)
		return E_INVALIDARG;

	// If Explorer is asking for a help string, copy our string into the
	// supplied buffer.
	if (uFlags & GCS_HELPTEXT)
	{
		CString strHelp;

		// Load help text
		if (m_nSelectedItems <= 2)
		{
			if (!strHelp.LoadString(IDS_CONTEXT_HELP))
				return S_FALSE;
		}
		else
		{
			if (!strHelp.LoadString(IDS_CONTEXT_HELP_MANYITEMS))
				return S_FALSE;
		}

		if (uFlags & GCS_UNICODE)
			// We need to cast pszName to a Unicode string, and then use the
			// Unicode string copy API.
			lstrcpynW((LPWSTR) pszName, T2CW(strHelp), cchMax);
		else
			// Use the ANSI string copy API to return the help string.
			lstrcpynA(pszName, T2CA(strHelp), cchMax);

		return S_OK;
	}

	return E_INVALIDARG;
}

/// Runs WinMerge with given paths
HRESULT CWinMergeShell::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
	CString strWinMergePath;

	// If lpVerb really points to a string, ignore this function call and bail out.
	if (HIWORD(pCmdInfo->lpVerb) != 0)
		return E_INVALIDARG;

	// Read WinMerge location from registry
	if (!GetWinMergeDir(strWinMergePath))
		return S_FALSE;

	// Check that file we are trying to execute exists and is executable
	if (!CheckExecutable(strWinMergePath))
		return S_FALSE;

	// Format command line
	CString strCommandLine = strWinMergePath + _T(" \"") +
		m_strPaths[0] + _T("\"");
	
	if (!m_strPaths[1].IsEmpty())
		strCommandLine += _T(" \"") + m_strPaths[1] + _T("\"");
	
	BOOL retVal = FALSE;
	STARTUPINFO stInfo = {0};
	stInfo.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION processInfo = {0};
	
	// Get the command index - the only valid one is 0.
	switch (LOWORD(pCmdInfo->lpVerb))
	{
	case 0:
		retVal = CreateProcess(NULL, (LPTSTR)(LPCTSTR) strCommandLine,
			NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
			&stInfo, &processInfo);

		if (retVal)
			return S_OK;
		else
			return S_FALSE;
		break;

	default:
		return E_INVALIDARG;
		break;
	}
}

/// Reads WinMerge path from registry
BOOL CWinMergeShell::GetWinMergeDir(CString &strDir)
{
	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegDir) != ERROR_SUCCESS)
		// TODO: Ask location from user?
		return FALSE;
	
	strDir = reg.ReadString(f_RegValuePath, _T(""));
	if (strDir.IsEmpty())
		// TODO: Ask location from user?
		return FALSE;

	return TRUE;
}

/// Checks if given file exists and is executable
BOOL CWinMergeShell::CheckExecutable(CString path)
{
	CString ext;
	SplitFilename(path, NULL, NULL, &ext);

	// Check extension
	ext.MakeLower();
	if (ext == _T("exe") || ext == _T("cmd") || ext == ("bat"))
	{
		// Check if file exists
		CFileStatus status;
		if (CFile::GetStatus(path, status))
			return TRUE;
	}
	return FALSE;
}
