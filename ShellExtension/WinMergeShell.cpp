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
// $Id: WinMergeShell.cpp 6933 2009-07-26 14:07:03Z kimmov $

#include "stdafx.h"
#include "ShellExtension.h"
#include "WinMergeShell.h"
#include "UnicodeString.h"
#include "RegKey.h"
#include <sys/types.h>
#include <sys/stat.h>

OBJECT_ENTRY_AUTO(CLSID_WinMergeShell, CWinMergeShell)

/**
 * @brief Flags for enabling and other settings of context menu.
 */
enum ExtensionFlags
{
	EXT_ENABLED = 0x01, /**< ShellExtension enabled/disabled. */
	EXT_ADVANCED = 0x02, /**< Advanced menuitems enabled/disabled. */
};

enum
{
	CMD_COMPARE = 0,
	CMD_COMPARE_ELLIPSE,
	CMD_SELECT_LEFT,
	CMD_SELECT_MIDDLE,
	CMD_RESELECT_LEFT,
	CMD_LAST = CMD_RESELECT_LEFT,
};

/// Max. filecount to select
static const int MaxFileCount = 3;
/// Registry path to WinMerge
#define REGDIR _T("Software\\Thingamahoochie\\WinMerge")
static const TCHAR f_RegDir[] = REGDIR;
static const TCHAR f_RegLocaleDir[] = REGDIR _T("\\Locale");
static const TCHAR f_RegSettingsDir[] = REGDIR _T("\\Settings");

/**
 * @name Registry valuenames.
 */
/*@{*/
/** Shell context menuitem enabled/disabled */
static const TCHAR f_RegValueEnabled[] = _T("ContextMenuEnabled");
/** 'Saved' path in advanced mode */
static const TCHAR f_FirstSelection[] = _T("FirstSelection");
/** 'Saved' path in advanced mode */
static const TCHAR f_SecondSelection[] = _T("SecondSelection");
/** Path to WinMergeU.exe */
static const TCHAR f_RegValuePath[] = _T("Executable");
/** Path to WinMergeU.exe, overwrites f_RegValuePath if present. */
static const TCHAR f_RegValuePriPath[] = _T("PriExecutable");
/** LanguageId */
static const TCHAR f_LanguageId[] = _T("LanguageId");
/** Recurse */
static const TCHAR f_Recurse[] = _T("Recurse");
/*@}*/

/**
 * @brief The states in which the menu can be.
 * These states define what items are added to the menu and how those
 * items work.
 */
enum
{
	MENU_SIMPLE = 0,  /**< Simple menu, only "Compare item" is shown. */
	MENU_ONESEL_NOPREV,  /**< One item selected, no previous selections. */
	MENU_ONESEL_PREV,  /**< One item selected, previous selection exists. */
	MENU_ONESEL_TWO_PREV,  /**< One item selected, two previous selections exist. */
	MENU_TWOSEL,  /**< Two items are selected. */
	MENU_THREESEL
};

static String GetResourceString(UINT resourceID);

// GreyMerlin (03 Sept 2017) - The following Version Info checking code is a 
// short extract from the Microsoft <versionhelpers.h> file.  Unfortunatly, 
// that file is not available for WinXP-compatible Platform Toolsets (e.g. 
// v141_xp for VS2017).  Fortunatly, all the actual API interfaces do exist 
// in WinXP (actually, in all Windows products since Win2000).  Use of this 
// <versionhelpers.h> code avoids the unpleasant deprecation of the GetVersionEx()
// API begining with Win 8.1.  This Version Info checking code is also fully 
// compatible with all non-XP-compatible Toolsets as well (e.g. v141).

#ifndef _WIN32_WINNT_VISTA
#define _WIN32_WINNT_VISTA	0x0600
#endif
#ifndef _WIN32_WINNT_WIN8
#define _WIN32_WINNT_WIN8	0x0602
#endif

#ifndef VERSIONHELPERAPI
#define VERSIONHELPERAPI inline bool

VERSIONHELPERAPI
IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
    OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0 };
    DWORDLONG        const dwlConditionMask = VerSetConditionMask(
        VerSetConditionMask(
        VerSetConditionMask(
            0, VER_MAJORVERSION, VER_GREATER_EQUAL),
               VER_MINORVERSION, VER_GREATER_EQUAL),
               VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

    osvi.dwMajorVersion = wMajorVersion;
    osvi.dwMinorVersion = wMinorVersion;
    osvi.wServicePackMajor = wServicePackMajor;

    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}


VERSIONHELPERAPI
IsWindows8OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
}
#endif // VERSIONHELPERAPI

/**
 * @brief Load a string from resource.
 * @param [in] Resource string ID.
 * @return String loaded from resource.
 */
static String GetResourceString(UINT resourceID)
{
	TCHAR resStr[1024] = {0};
	int res = LoadString(_AtlComModule.m_hInstTypeLib, resourceID, resStr, 1024);
	ATLASSERT(res != 0);
	String strResource = resStr;
	return strResource;
}

static BOOL InsertMenuString(HMENU hMenu, UINT uPosition, UINT uIDNewItem, UINT uStringId)
{
	String str = GetResourceString(uStringId);
	MENUITEMINFO mii{sizeof mii};
	mii.fMask = MIIM_ID | MIIM_STRING | MIIM_DATA;
	mii.wID = uIDNewItem;
	mii.dwTypeData = const_cast<LPTSTR>(str.c_str());
	mii.dwItemData = uStringId;
	return InsertMenuItem(hMenu, uPosition, TRUE, &mii);
}

HBITMAP ConvertHICONtoHBITMAP(HICON hIcon, int cx, int cy)
{
	LPVOID lpBits;
	BITMAPINFO bmi = { { sizeof(BITMAPINFOHEADER), cx, cy, 1, IsWindows8OrGreater() ? 32u : 24u } };
	HBITMAP hbmp = CreateDIBSection(NULL, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, &lpBits, NULL, 0);
	HDC hdcMem = CreateCompatibleDC(NULL);
	HBITMAP hbmpPrev = (HBITMAP)SelectObject(hdcMem, hbmp);
	RECT rc = { 0, 0, cx, cy };
	if (bmi.bmiHeader.biBitCount <= 24)
	{
		SetBkColor(hdcMem, GetSysColor(COLOR_MENU));
		ExtTextOut(hdcMem, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}
	DrawIconEx(hdcMem, 0, 0, hIcon, cx, cy, 0, NULL, DI_NORMAL);
	SelectObject(hdcMem, hbmpPrev);
	DeleteDC(hdcMem);
	return hbmp;
}

/////////////////////////////////////////////////////////////////////////////
// CWinMergeShell

/// Default constructor, loads icon bitmap
CWinMergeShell::CWinMergeShell()
{
	m_dwMenuState = 0;
	int cx = GetSystemMetrics(SM_CXMENUCHECK);
	int cy = GetSystemMetrics(SM_CYMENUCHECK);

	// compress or stretch icon bitmap according to menu item height
	HICON hMergeIcon = (HICON)LoadImage(_AtlComModule.m_hInstTypeLib, MAKEINTRESOURCE(IDI_WINMERGE), IMAGE_ICON,
		cx, cy, LR_DEFAULTCOLOR);
	HICON hMergeDirIcon = (HICON)LoadImage(_AtlComModule.m_hInstTypeLib, MAKEINTRESOURCE(IDI_WINMERGEDIR), IMAGE_ICON,
		cx, cy, LR_DEFAULTCOLOR);

	m_MergeBmp = ConvertHICONtoHBITMAP(hMergeIcon, cx, cy);
	m_MergeDirBmp = ConvertHICONtoHBITMAP(hMergeDirIcon, cx, cy);

	DestroyIcon(hMergeIcon);
	DestroyIcon(hMergeDirIcon);

}

/// Default destructor, unloads bitmap
CWinMergeShell::~CWinMergeShell()
{
	DeleteObject(m_MergeDirBmp);
	DeleteObject(m_MergeBmp);
}

/// Reads selected paths
HRESULT CWinMergeShell::Initialize(LPCITEMIDLIST pidlFolder,
		LPDATAOBJECT pDataObj, HKEY hProgID)
{
	HRESULT hr = E_INVALIDARG;

	for (auto& path: m_strPaths)
		path.erase();

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
		UINT uNumFilesDropped = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
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
	else
	{
		m_nSelectedItems = 0;
	}

		// No item selected - selection is the folder background
	if (pidlFolder)
	{
		TCHAR szPath[MAX_PATH] = {0};

		if (SHGetPathFromIDList(pidlFolder, szPath))
		{
			if (m_nSelectedItems < MaxFileCount)
				m_strPaths[m_nSelectedItems++] = szPath;
			hr = S_OK;
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	return hr;
}

/// Adds context menu item
HRESULT CWinMergeShell::QueryContextMenu(HMENU hmenu, UINT uMenuIndex,
		UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
	// check whether menu items are already added
	if (hmenu == s_hMenuLastAdded)
	{
		MENUITEMINFO mii{ sizeof mii };
		mii.fMask = MIIM_DATA;
		if (GetMenuItemInfo(hmenu, s_uidCmdLastAdded, FALSE, &mii))
		{
			if (mii.dwItemData >= IDS_COMPARE && mii.dwItemData <= IDS_RESELECT_LEFT)
				return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
		}
	}

	s_hMenuLastAdded = hmenu;
	s_uidCmdLastAdded = uidFirstCmd;

	int uidUserLastCmd = 0;

	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	// Check if user wants to use context menu
	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegDir) != ERROR_SUCCESS)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	m_dwContextMenuEnabled = reg.ReadDword(f_RegValueEnabled, 0);
	m_strPreviousPath[0] = reg.ReadString(f_FirstSelection, _T("")).c_str();
	m_strPreviousPath[1] = reg.ReadString(f_SecondSelection, _T("")).c_str();

	if (m_dwContextMenuEnabled & EXT_ENABLED) // Context menu enabled
	{
		// Check if advanced menuitems enabled
		if ((m_dwContextMenuEnabled & EXT_ADVANCED) == 0)
		{
			m_dwMenuState = MENU_SIMPLE;
			uidUserLastCmd = DrawSimpleMenu(hmenu, uMenuIndex, uidFirstCmd);
		}
		else
		{
			if (m_nSelectedItems == 1 && m_strPreviousPath[0].empty())
				m_dwMenuState = MENU_ONESEL_NOPREV;
			else if (m_nSelectedItems == 1 && !m_strPreviousPath[0].empty() && m_strPreviousPath[1].empty())
				m_dwMenuState = MENU_ONESEL_PREV;
			else if (m_nSelectedItems == 1 && !m_strPreviousPath[0].empty() && !m_strPreviousPath[1].empty())
				m_dwMenuState = MENU_ONESEL_TWO_PREV;
			else if (m_nSelectedItems == 2)
				m_dwMenuState = MENU_TWOSEL;
			else if (m_nSelectedItems == 3)
				m_dwMenuState = MENU_THREESEL;

			uidUserLastCmd = DrawAdvancedMenu(hmenu, uMenuIndex, uidFirstCmd);
		}

		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, (uidUserLastCmd - uidFirstCmd) + 1);
	}
	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
}

/// Gets string shown explorer's status bar when menuitem selected
HRESULT CWinMergeShell::GetCommandString(UINT_PTR idCmd, UINT uFlags,
		UINT* pwReserved, LPSTR pszName, UINT  cchMax)
{
	USES_CONVERSION;

	// Check idCmd, it must be 0 in simple mode and 0 or 1 in advanced mode.
	if ((m_dwMenuState & EXT_ADVANCED) == 0)
	{
		if (idCmd > 0)
			return E_INVALIDARG;
	}
	else
	{
		if (idCmd > 2)
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

	// If lpVerb really points to a string, ignore this function call and bail out.
	if (HIWORD(pCmdInfo->lpVerb) != 0)
		return E_INVALIDARG;

	// Read WinMerge location from registry
	if (!GetWinMergeDir(strWinMergePath))
		return S_FALSE;

	// Check that file we are trying to execute exists
	if (!PathFileExists(strWinMergePath.c_str()))
		return S_FALSE;

	if (LOWORD(pCmdInfo->lpVerb) == CMD_COMPARE)
	{
		switch (m_dwMenuState)
		{
		case MENU_SIMPLE:
			bCompare = TRUE;
			break;

		case MENU_ONESEL_PREV:
			m_strPaths[1] = m_strPaths[0];
			m_strPaths[0] = m_strPreviousPath[0];
			bCompare = TRUE;

			// Forget previous selection
			if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			{
				reg.WriteString(f_FirstSelection, _T(""));
				reg.WriteString(f_SecondSelection, _T(""));
			}
			break;

		case MENU_ONESEL_TWO_PREV:
			m_strPaths[2] = m_strPaths[0];
			m_strPaths[0] = m_strPreviousPath[0];
			m_strPaths[1] = m_strPreviousPath[1];
			bCompare = TRUE;

			// Forget previous selection
			if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			{
				reg.WriteString(f_FirstSelection, _T(""));
				reg.WriteString(f_SecondSelection, _T(""));
			}
			break;

		case MENU_TWOSEL:
		case MENU_THREESEL:
			// "Compare" - compare paths
			bCompare = TRUE;
			m_strPreviousPath[0].erase();
			m_strPreviousPath[1].erase();
			break;
		}
	}
	else if (LOWORD(pCmdInfo->lpVerb) == CMD_COMPARE_ELLIPSE)
	{
		// "Compare..." - user wants to compare this single item and open WinMerge
		m_strPaths[1].erase();
		m_strPaths[2].erase();
		bCompare = TRUE;
	}
	else if (LOWORD(pCmdInfo->lpVerb) == CMD_SELECT_LEFT)
	{
		// Select Left
		m_strPreviousPath[0] = m_strPaths[0];
		if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			reg.WriteString(f_FirstSelection, m_strPreviousPath[0].c_str());
	}
	else if (LOWORD(pCmdInfo->lpVerb) == CMD_SELECT_MIDDLE)
	{
		// Select Middle
		m_strPreviousPath[1] = m_strPaths[0];
		if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
			reg.WriteString(f_SecondSelection, m_strPreviousPath[1].c_str());
	}
	else if (LOWORD(pCmdInfo->lpVerb) == CMD_RESELECT_LEFT)
	{
		// Re-select Left
		m_strPreviousPath[0] = m_strPaths[0];
		m_strPreviousPath[1].clear();
		if (reg.Open(HKEY_CURRENT_USER, f_RegDir) == ERROR_SUCCESS)
		{
			reg.WriteString(f_FirstSelection, m_strPreviousPath[0].c_str());
			reg.WriteString(f_SecondSelection, _T(""));
		}
	}
	else
		return E_INVALIDARG;

	if (bCompare == FALSE)
		return S_FALSE;

	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)
		bAlterSubFolders = TRUE;

	String strCommandLine = FormatCmdLine(strWinMergePath,
		m_strPaths[0], m_strPaths[1], m_strPaths[2], bAlterSubFolders);

	// Finally start a new WinMerge process
	BOOL retVal = FALSE;
	STARTUPINFO stInfo = {0};
	stInfo.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION processInfo = {0};

	retVal = CreateProcess(NULL, (LPTSTR)strCommandLine.c_str(),
			NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
			&stInfo, &processInfo);

	if (retVal)
	{
		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);
	}
	else if (GetLastError() == ERROR_ELEVATION_REQUIRED)
	{
		String strCommandLine = FormatCmdLine(_T(""),
			m_strPaths[0], m_strPaths[1], m_strPaths[2], bAlterSubFolders);
		HINSTANCE hInstance = ShellExecute(nullptr, _T("runas"), strWinMergePath.c_str(), strCommandLine.c_str(), 0, SW_SHOWNORMAL);
		if (reinterpret_cast<intptr_t>(hInstance) < 32)
			return S_FALSE;
	}
	else
	{
		return S_FALSE;
	}

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

/// Create menu for simple mode
int CWinMergeShell::DrawSimpleMenu(HMENU hmenu, UINT uMenuIndex,
		UINT uidFirstCmd)
{
	InsertMenuString(hmenu, uMenuIndex, uidFirstCmd, IDS_CONTEXT_MENU);

	// Add bitmap
	HBITMAP hBitmap = PathIsDirectory(m_strPaths[0].c_str()) ? m_MergeDirBmp : m_MergeBmp;
	if (hBitmap != NULL)
		SetMenuItemBitmaps(hmenu, uMenuIndex, MF_BYPOSITION, hBitmap, NULL);

	// Show menu item as grayed if more than two items selected
	if (m_nSelectedItems > MaxFileCount)
		EnableMenuItem(hmenu, uMenuIndex, MF_BYPOSITION | MF_GRAYED);

	return uidFirstCmd + CMD_LAST;
}

/// Create menu for advanced mode
int CWinMergeShell::DrawAdvancedMenu(HMENU hmenu, UINT uMenuIndex,
		UINT uidFirstCmd)
{
	int nItemsAdded = 0;

	switch (m_dwMenuState)
	{
		// No items selected earlier
		// Select item as first item to compare
	case MENU_ONESEL_NOPREV:
		InsertMenuString(hmenu, uMenuIndex++, uidFirstCmd + CMD_SELECT_LEFT, IDS_SELECT_LEFT);
		InsertMenuString(hmenu, uMenuIndex,   uidFirstCmd + CMD_COMPARE_ELLIPSE, IDS_COMPARE_ELLIPSIS);
		nItemsAdded = 2;
		break;

		// One item selected earlier:
		// Allow re-selecting first item or selecting second item
	case MENU_ONESEL_PREV:
		InsertMenuString(hmenu, uMenuIndex++, uidFirstCmd + CMD_COMPARE, IDS_COMPARE);
		InsertMenuString(hmenu, uMenuIndex++, uidFirstCmd + CMD_SELECT_MIDDLE, IDS_SELECT_MIDDLE);
		InsertMenuString(hmenu, uMenuIndex,   uidFirstCmd + CMD_RESELECT_LEFT, IDS_RESELECT_LEFT);
		nItemsAdded = 3;
		break;

		// Two items are selected earlier:
		// Allow re-selecting first item or selecting second item
	case MENU_ONESEL_TWO_PREV:
		InsertMenuString(hmenu, uMenuIndex++, uidFirstCmd + CMD_COMPARE, IDS_COMPARE);
		InsertMenuString(hmenu, uMenuIndex,   uidFirstCmd + CMD_RESELECT_LEFT, IDS_RESELECT_LEFT);
		nItemsAdded = 2;
		break;

		// Two items selected
		// Select both items for compare
	case MENU_TWOSEL:
	case MENU_THREESEL:
		InsertMenuString(hmenu, uMenuIndex, uidFirstCmd + CMD_COMPARE, IDS_COMPARE);
		nItemsAdded = 1;
		break;

	default:
		InsertMenuString(hmenu, uMenuIndex, uidFirstCmd + CMD_COMPARE, IDS_COMPARE);
		nItemsAdded = 1;
		break;
	}

	// Add bitmap
	HBITMAP hBitmap = PathIsDirectory(m_strPaths[0].c_str()) ? m_MergeDirBmp : m_MergeBmp;
	if (hBitmap != NULL)
	{
		for (int i = 0; i < nItemsAdded; i++)
			SetMenuItemBitmaps(hmenu, uMenuIndex - (nItemsAdded - 1 - i), MF_BYPOSITION, hBitmap, NULL);
	}

	// Show menu item as grayed if more than two items selected
	if (m_nSelectedItems > MaxFileCount)
	{
		for (int i = 0; i < nItemsAdded; i++)
			EnableMenuItem(hmenu, uMenuIndex - (nItemsAdded - 1 - i), MF_BYPOSITION | MF_GRAYED);
	}

	return uidFirstCmd + CMD_LAST;
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

	if (idCmd == CMD_COMPARE)
	{
		switch (m_dwMenuState)
		{
		case MENU_ONESEL_PREV:
			strHelp = GetResourceString(IDS_HELP_COMPARESAVED);
			strutils::replace(strHelp, _T("%1"), m_strPreviousPath[0]);
			break;

		case MENU_ONESEL_TWO_PREV:
			strHelp = GetResourceString(IDS_HELP_COMPARESAVED);
			strutils::replace(strHelp, _T("%1"), m_strPreviousPath[0] + _T(" - ") + m_strPreviousPath[1]);
			break;
		default:
			strHelp = GetResourceString(IDS_CONTEXT_HELP);
			break;
		}
	}
	else if (idCmd == CMD_COMPARE_ELLIPSE)
	{
		strHelp = GetResourceString(IDS_CONTEXT_HELP);
	}
	else if (idCmd == CMD_SELECT_LEFT)
	{
		strHelp = GetResourceString(IDS_HELP_SAVETHIS);
	}
	else if (idCmd == CMD_SELECT_MIDDLE)
	{
		strHelp = GetResourceString(IDS_HELP_SAVETHIS);
	}
	else if (idCmd == CMD_RESELECT_LEFT)
	{
		strHelp = GetResourceString(IDS_HELP_SAVETHIS);
	}
	return strHelp;
}

/// Format commandline used to start WinMerge
String CWinMergeShell::FormatCmdLine(const String &winmergePath,
		const String &path1, const String &path2, const String &path3, BOOL bAlterSubFolders)
{
	String strCommandline = winmergePath.empty() ? _T("") : _T("\"") + winmergePath + _T("\"");

	// Check if user wants to use context menu
	BOOL bSubfoldersByDefault = FALSE;
	CRegKeyEx reg;
	if (reg.Open(HKEY_CURRENT_USER, f_RegSettingsDir) == ERROR_SUCCESS)
		bSubfoldersByDefault = reg.ReadBool(f_Recurse, FALSE);

	if (bAlterSubFolders && !bSubfoldersByDefault)
		strCommandline += _T(" /r");
	else if (!bAlterSubFolders && bSubfoldersByDefault)
		strCommandline += _T(" /r");

	strCommandline += _T(" \"") + path1 + _T("\"");

	if (!m_strPaths[1].empty())
		strCommandline += _T(" \"") + path2 + _T("\"");

	if (!m_strPaths[2].empty())
		strCommandline += _T(" \"") + path3 + _T("\"");

	return strCommandline;
}
