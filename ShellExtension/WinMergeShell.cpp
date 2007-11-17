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
// Look at http://www.codeproject.com/shell/ for excellent guide
// to Windows Shell programming by Michael Dunn.
// 
// This extension needs two registry values to be defined:
//  HKEY_CURRENT_USER\Software\Thingamahoochie\WinMerge\ContextMenuEnabled
//   defines if context menu is shown (extension enabled) and if
//   we show simple or advanced menu
//  HKEY_CURRENT_USER\Software\Thingamahoochie\WinMerge\Executable
//   contains path to program to run (can be batch file too)
//
// HKEY_CURRENT_USER\Software\Thingamahoochie\WinMerge\FirstSelection
//  is used to store path for first selection in advanced mode
//
//  HKEY_CURRENT_USER\Software\Thingamahoochie\WinMerge\PriExecutable
//   overwrites 'Executable' if defined. Useful to overwrite
//   option set from UI when debugging/testing.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  WinMergeShell.cpp
 *
 * @brief Implementation of the ShellExtension class
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "ShellExtension.h"
#include "WinMergeShell.h"
#include "UnicodeString.h"
#include "RegKey.h"
#include "coretools.h"
#include <sys/types.h>
#include <sys/stat.h>

/** 
 * @brief Flags for enabling and other settings of context menu.
 */
enum ExtensionFlags
{
	EXT_ENABLED = 0x01, /**< ShellExtension enabled/disabled. */
	EXT_ADVANCED = 0x02, /**< Advanced menuitems enabled/disabled. */
	EXT_SUBFOLDERS = 0x04, /**< Subfolders included by default? */
};

/// Max. filecount to select
static const int MaxFileCount = 2;
/// Registry path to WinMerge 
#define REGDIR _T("Software\\Thingamahoochie\\WinMerge")
static const TCHAR f_RegDir[] = REGDIR;
static const TCHAR f_RegLocaleDir[] = REGDIR _T("\\Locale");

/**
 * @name Registry valuenames.
 */
/*@{*/ 
/** Shell context menuitem enabled/disabled */
static const TCHAR f_RegValueEnabled[] = _T("ContextMenuEnabled");
/** 'Saved' path in advanced mode */
static const TCHAR f_FirstSelection[] = _T("FirstSelection");
/** Path to WinMerge[U].exe */
static const TCHAR f_RegValuePath[] = _T("Executable");
/** Path to WinMerge[U].exe, overwrites f_RegValuePath if present. */
static const TCHAR f_RegValuePriPath[] = _T("PriExecutable");
/** LanguageId */
static const TCHAR f_LanguageId[] = _T("LanguageId");
/*@}*/

/// Shown menustate
enum
{
	MENU_SIMPLE = 0,
	MENU_ONESEL_NOPREV,
	MENU_ONESEL_PREV,
	MENU_TWOSEL,
};

#define USES_WINMERGELOCALE CWinMergeTempLocale __wmtl__

static String GetResourceString(UINT resourceID);

class CWinMergeTempLocale
{
private:
	LCID m_lcidOld;
public:
	CWinMergeTempLocale() {
		CRegKeyEx reg;
		if (reg.Open(HKEY_CURRENT_USER, f_RegLocaleDir) != ERROR_SUCCESS)
			return;

		m_lcidOld = GetThreadLocale();

		int iLangId = reg.ReadDword(f_LanguageId, (DWORD)-1);
		if (iLangId != -1)
			SetThreadLocale(MAKELCID(iLangId, SORT_DEFAULT));
	}
	~CWinMergeTempLocale() {
		SetThreadLocale(m_lcidOld);
	}
};

/**
 * @brief Load a string from resource.
 * @param [in] Resource string ID.
 * @return String loaded from resource.
 */
static String GetResourceString(UINT resourceID)
{
	TCHAR resStr[1024] = {0};
	int res = LoadString(_Module.GetModuleInstance(), resourceID, resStr, 1024);
	ATLASSERT(res!= 0);
	String strResource = resStr;
	return strResource;
}

/////////////////////////////////////////////////////////////////////////////
// CWinMergeShell

/// Default constructor, loads icon bitmap
CWinMergeShell::CWinMergeShell()
{
	m_dwMenuState = 0;

	// compress or stretch icon bitmap according to menu item height
	HDC hdc = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	HDC hdcMemDst = CreateCompatibleDC(hdc);
	HDC hdcMemSrc = CreateCompatibleDC(hdc);

	HBITMAP hBitmapResource = LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDB_WINMERGE));		
	BITMAP bm;	
	GetObject(hBitmapResource, sizeof(bm), &bm);

	int nMenuBitmapWidth = GetSystemMetrics(SM_CXMENUCHECK);
	int nMenuBitmapHeight = GetSystemMetrics(SM_CYMENUCHECK);
	m_MergeBmp = CreateCompatibleBitmap(hdc, nMenuBitmapWidth, nMenuBitmapHeight);

	HBITMAP hOldBitmapSrc = (HBITMAP)SelectObject(hdcMemSrc, hBitmapResource);
	HBITMAP hOldBitmapDst = (HBITMAP)SelectObject(hdcMemDst, m_MergeBmp);

	SetBrushOrgEx(hdcMemDst, 0, 0, NULL);
	SetStretchBltMode(hdcMemDst, HALFTONE);
	StretchBlt(hdcMemDst, 0, 0, nMenuBitmapWidth, nMenuBitmapHeight, hdcMemSrc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	SelectObject(hdcMemSrc, hOldBitmapSrc);
	SelectObject(hdcMemDst, hOldBitmapDst);
	DeleteObject(hBitmapResource);
	DeleteDC(hdcMemSrc);
	DeleteDC(hdcMemDst);
	DeleteDC(hdc);
}

/// Default destructor, unloads bitmap
CWinMergeShell::~CWinMergeShell()
{
	DeleteObject(m_MergeBmp);
}

/// Reads selected paths
HRESULT CWinMergeShell::Initialize(LPCITEMIDLIST pidlFolder,
		LPDATAOBJECT pDataObj, HKEY hProgID)
{
	USES_WINMERGELOCALE;
	HRESULT hr = E_INVALIDARG;

	// Files/folders selected normally from the explorer
	if (pDataObj)
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

		hr = S_OK;

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

			if (x < MaxFileCount)
				m_strPaths[x] = npszFile;

			delete[] npszFile;
		}
		GlobalUnlock(stg.hGlobal);
		ReleaseStgMedium(&stg);
	}

	// No item selected - selection is the folder background
	if (pidlFolder)
	{
		TCHAR szPath[MAX_PATH] = {0};

		if (SHGetPathFromIDList(pidlFolder, szPath))
		{
			m_strPaths[0] = szPath;
			m_nSelectedItems = 1;
			hr = S_OK;
		}
		else
			hr = E_INVALIDARG;
	}
	return hr;
}

/// Adds context menu item
HRESULT CWinMergeShell::QueryContextMenu(HMENU hmenu, UINT uMenuIndex,
		UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
	int nItemsAdded = 0;
	USES_WINMERGELOCALE;

	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	// Check if user wants to use context menu
	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegDir) != ERROR_SUCCESS)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	m_dwContextMenuEnabled = reg.ReadDword(f_RegValueEnabled, 0);
	m_strPreviousPath = reg.ReadString(f_FirstSelection, _T("")).c_str();

	if (m_dwContextMenuEnabled & EXT_ENABLED) // Context menu enabled
	{
		// Check if advanced menuitems enabled
		if ((m_dwContextMenuEnabled & EXT_ADVANCED) == 0)
		{
			m_dwMenuState = MENU_SIMPLE;
			nItemsAdded = DrawSimpleMenu(hmenu, uMenuIndex, uidFirstCmd);
		}
		else
		{
			if (m_nSelectedItems == 1 && m_strPreviousPath.empty())
				m_dwMenuState = MENU_ONESEL_NOPREV;
			else if (m_nSelectedItems == 1 && !m_strPreviousPath.empty())
				m_dwMenuState = MENU_ONESEL_PREV;
			else if (m_nSelectedItems == 2)
				m_dwMenuState = MENU_TWOSEL;

			nItemsAdded = DrawAdvancedMenu(hmenu, uMenuIndex, uidFirstCmd);
		}

		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, nItemsAdded);
	}
	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
}

/// Gets string shown explorer's status bar when menuitem selected
HRESULT CWinMergeShell::GetCommandString(UINT_PTR idCmd, UINT uFlags,
		UINT* pwReserved, LPSTR pszName, UINT  cchMax)
{
	USES_CONVERSION;
	USES_WINMERGELOCALE;

	// Check idCmd, it must be 0 in simple mode and 0 or 1 in advanced mode.
	if ((m_dwMenuState & EXT_ADVANCED) == 0)
	{
		if (idCmd > 0)
			return E_INVALIDARG;
	}
	else
	{
		if (idCmd > 1)
			return E_INVALIDARG;
	}

	// If Explorer is asking for a help string, copy our string into the
	// supplied buffer.
	if (uFlags & GCS_HELPTEXT)
	{
		String strHelp;

		strHelp = GetHelpText(idCmd);

		if (uFlags & GCS_UNICODE)
			// We need to cast pszName to a Unicode string, and then use the
			// Unicode string copy API.
			lstrcpynW((LPWSTR) pszName, T2CW(strHelp.c_str()), cchMax);
		else
			// Use the ANSI string copy API to return the help string.
			lstrcpynA(pszName, T2CA(strHelp.c_str()), cchMax);

		return S_OK;
	}
	return E_INVALIDARG;
}

/// Runs WinMerge with given paths
HRESULT CWinMergeShell::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	CRegKeyEx reg;
	String strWinMergePath;
	BOOL bCompare = FALSE;
	BOOL bAlterSubFolders = FALSE;
	USES_WINMERGELOCALE;

	// If lpVerb really points to a string, ignore this function call and bail out.
	if (HIWORD(pCmdInfo->lpVerb) != 0)
		return E_INVALIDARG;

	// Read WinMerge location from registry
	if (!GetWinMergeDir(strWinMergePath))
		return S_FALSE;

	// Check that file we are trying to execute exists and is executable
	if (!CheckExecutable(strWinMergePath))
		return S_FALSE;

	if (LOWORD(pCmdInfo->lpVerb) == 0)
	{
		switch (m_dwMenuState)
		{
		case MENU_SIMPLE:
			bCompare = TRUE;
			break;

		case MENU_ONESEL_NOPREV:
			m_strPreviousPath = m_strPaths[0];
			if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
				reg.WriteString(f_FirstSelection, m_strPreviousPath.c_str());
			break;

		case MENU_ONESEL_PREV:
			m_strPaths[1] = m_strPaths[0];
			m_strPaths[0] = m_strPreviousPath;
			bCompare = TRUE;
			
			// Forget previous selection
			if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
				reg.WriteString(f_FirstSelection, _T(""));
			break;

		case MENU_TWOSEL:
			// "Compare" - compare paths
			bCompare = TRUE;
			m_strPreviousPath.erase();
			break;
		}
	}
	else if (LOWORD(pCmdInfo->lpVerb) == 1)
	{
		switch (m_dwMenuState)
		{
		case MENU_ONESEL_PREV:
			m_strPreviousPath = m_strPaths[0];
			if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
				reg.WriteString(f_FirstSelection, m_strPreviousPath.c_str());
			bCompare = FALSE;
			break;
		default:
			// "Compare..." - user wants to compare this single item and open WinMerge
			m_strPaths[1].erase();
			bCompare = TRUE;
			break;
		}
	}
	else
		return E_INVALIDARG;

	if (bCompare == FALSE)
		return S_FALSE;

	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)
		bAlterSubFolders = TRUE;

	String strCommandLine = FormatCmdLine(strWinMergePath, m_strPaths[0],
		m_strPaths[1], bAlterSubFolders);

	// Finally start a new WinMerge process
	BOOL retVal = FALSE;
	STARTUPINFO stInfo = {0};
	stInfo.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION processInfo = {0};
	
	retVal = CreateProcess(NULL, (LPTSTR)strCommandLine.c_str(),
		NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&stInfo, &processInfo);

	if (!retVal)
		return S_FALSE;

	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
	return S_OK;
}

/// Reads WinMerge path from registry
BOOL CWinMergeShell::GetWinMergeDir(String &strDir)
{
	CRegKeyEx reg;
	if (!reg.QueryRegUser(f_RegDir))
		return FALSE;
	
	// Try first reading debug/test value
	strDir = reg.ReadString(f_RegValuePriPath, _T(""));
	if (strDir.empty())
	{
		strDir = reg.ReadString(f_RegValuePath, _T(""));
		if (strDir.empty())
			return FALSE;
	}	

	return TRUE;
}

/// Checks if given file exists and is executable
BOOL CWinMergeShell::CheckExecutable(String path)
{
	String sExt;
	SplitFilename(path.c_str(), NULL, NULL, &sExt);

	// Check extension
	if (_tcsicmp(sExt.c_str(), _T("exe")) == 0 ||
		_tcsicmp(sExt.c_str(), _T("cmd")) == 0 ||
		_tcsicmp(sExt.c_str(), _T("bat")) == 0)
	{
		// Check if file exists
		struct _stati64 statBuffer;
		int nRetVal = _tstati64(path.c_str(), &statBuffer);
		if (nRetVal > -1)
			return TRUE;
	}
	return FALSE;
}

/// Create menu for simple mode
int CWinMergeShell::DrawSimpleMenu(HMENU hmenu, UINT uMenuIndex,
		UINT uidFirstCmd)
{
	String strMenu = GetResourceString(IDS_CONTEXT_MENU);
	InsertMenu(hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd, strMenu.c_str());
	
	// Add bitmap
	if (m_MergeBmp != NULL)
		SetMenuItemBitmaps(hmenu, uMenuIndex, MF_BYPOSITION, m_MergeBmp, NULL);
	
	// Show menu item as grayed if more than two items selected
	if (m_nSelectedItems > MaxFileCount)
		EnableMenuItem(hmenu, uMenuIndex, MF_BYPOSITION | MF_GRAYED);
	
	return 1;
}

/// Create menu for advanced mode
int CWinMergeShell::DrawAdvancedMenu(HMENU hmenu, UINT uMenuIndex,
		UINT uidFirstCmd)
{
	String strCompare = GetResourceString(IDS_COMPARE);
	String strCompareEllipsis = GetResourceString(IDS_COMPARE_ELLIPSIS);
	String strCompareTo = GetResourceString(IDS_COMPARE_TO);
	String strReselect = GetResourceString(IDS_RESELECT_FIRST);
	int nItemsAdded = 0;
	
	switch (m_dwMenuState)
	{
	// No items selected earlier
	// Select item as first item to compare
	case MENU_ONESEL_NOPREV:
		InsertMenu(hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd,
			strCompareTo.c_str());
		uMenuIndex++;
		uidFirstCmd++;
		InsertMenu(hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd,
			strCompareEllipsis.c_str());
		nItemsAdded = 2;
		break;

	// One item selected earlier:
	// Allow re-selecting first item or selecting second item
	case MENU_ONESEL_PREV:
		InsertMenu(hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd,
			strCompare.c_str());
		uMenuIndex++;
		uidFirstCmd++;
		InsertMenu(hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd,
			strReselect.c_str());
		nItemsAdded = 2;
		break;

	// Two items selected
	// Select both items for compare
	case MENU_TWOSEL:
		InsertMenu(hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd,
			strCompare.c_str());
		nItemsAdded = 1;
		break;

	default:
		InsertMenu(hmenu, uMenuIndex, MF_BYPOSITION, uidFirstCmd,
			strCompare.c_str());
		nItemsAdded = 1;
		break;
	}
	
	// Add bitmap
	if (m_MergeBmp != NULL)
	{
		if (nItemsAdded == 2)
			SetMenuItemBitmaps(hmenu, uMenuIndex - 1, MF_BYPOSITION, m_MergeBmp, NULL);
		SetMenuItemBitmaps(hmenu, uMenuIndex, MF_BYPOSITION, m_MergeBmp, NULL);
	}
	
	// Show menu item as grayed if more than two items selected
	if (m_nSelectedItems > MaxFileCount)
	{
		if (nItemsAdded == 2)
			EnableMenuItem(hmenu, uMenuIndex - 1, MF_BYPOSITION | MF_GRAYED);
		EnableMenuItem(hmenu, uMenuIndex, MF_BYPOSITION | MF_GRAYED);
	}

	return nItemsAdded;
}

/// Determine help text shown in explorer's statusbar
String CWinMergeShell::GetHelpText(UINT_PTR idCmd)
{
	String strHelp;

	// More than two items selected, advice user
	if (m_nSelectedItems > MaxFileCount)
	{
		strHelp = GetResourceString(IDS_CONTEXT_HELP_MANYITEMS);
		return strHelp;
	}

	if (idCmd == 0)
	{
		switch (m_dwMenuState)
		{
		case MENU_SIMPLE:
			strHelp = GetResourceString(IDS_CONTEXT_HELP);;
			break;

		case MENU_ONESEL_NOPREV:
			strHelp = GetResourceString(IDS_HELP_SAVETHIS);
			break;
		
		case MENU_ONESEL_PREV:
			strHelp = GetResourceString(IDS_HELP_COMPARESAVED);
			string_replace(strHelp, _T("%1"), m_strPreviousPath);
			break;
		
		case MENU_TWOSEL:
			strHelp = GetResourceString(IDS_CONTEXT_HELP);
			break;
		}
	}
	else if (idCmd == 1)
	{
		switch (m_dwMenuState)
		{
		case MENU_ONESEL_PREV:
			strHelp = GetResourceString(IDS_HELP_SAVETHIS);
			break;
		default:
			strHelp = GetResourceString(IDS_CONTEXT_HELP);
			break;
		}
	}
	return strHelp;
}

/// Format commandline used to start WinMerge
String CWinMergeShell::FormatCmdLine(const String &winmergePath,
	const String &path1, const String &path2, BOOL bAlterSubFolders)
{
	String strCommandline(winmergePath);

	// Check if user wants to use context menu
	BOOL bSubfoldersByDefault = FALSE;
	if (m_dwContextMenuEnabled & EXT_SUBFOLDERS) // User wants subfolders by def
		bSubfoldersByDefault = TRUE;

	if (bAlterSubFolders && !bSubfoldersByDefault)
		strCommandline += _T(" /r");
	else if (!bAlterSubFolders && bSubfoldersByDefault)
		strCommandline += _T(" /r");
	
	strCommandline += _T(" \"") + path1 + _T("\"");
	
	if (!m_strPaths[1].empty())
		strCommandline += _T(" \"") + path2 + _T("\"");

	return strCommandline;
}
