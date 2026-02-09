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

#include "stdafx.h"
#include "ShellExtension.h"
#include "WinMergeShell.h"
#include "../Common/WinMergeContextMenu.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>

OBJECT_ENTRY_AUTO(CLSID_WinMergeShell, CWinMergeShell)

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

HBITMAP ConvertHICONtoHBITMAP(HICON hIcon, int cx, int cy)
{
	LPVOID lpBits;
	BITMAPINFO bmi = { { sizeof(BITMAPINFOHEADER), cx, cy, 1, IsWindows8OrGreater() ? 32u : 24u } };
	HDC hdcMem = CreateCompatibleDC(NULL);
	HBITMAP hbmp = CreateDIBSection(NULL, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, &lpBits, NULL, 0);
	if (hbmp)
	{
		HBITMAP hbmpPrev = (HBITMAP)SelectObject(hdcMem, hbmp);
		RECT rc = { 0, 0, cx, cy };
		if (bmi.bmiHeader.biBitCount <= 24)
		{
			SetBkColor(hdcMem, GetSysColor(COLOR_MENU));
			ExtTextOut(hdcMem, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
		}
		DrawIconEx(hdcMem, 0, 0, hIcon, cx, cy, 0, NULL, DI_NORMAL);
		SelectObject(hdcMem, hbmpPrev);
	}
	DeleteDC(hdcMem);
	return hbmp;
}

/////////////////////////////////////////////////////////////////////////////
// CWinMergeShell

/// Default constructor, loads icon bitmap
CWinMergeShell::CWinMergeShell()
	: m_contextMenu(_AtlComModule.m_hInstTypeLib)
{
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

	std::vector<std::wstring> paths;

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

		if (uNumFilesDropped == 0)
		{
			GlobalUnlock(stg.hGlobal);
			ReleaseStgMedium(&stg);
			return E_INVALIDARG;
		}

		hr = S_OK;

		/// Max. filecount to select
		constexpr unsigned MaxFileCount = 3u;

		// Get all file names.
		for (UINT x = 0 ; x < (std::min)(MaxFileCount + 1, uNumFilesDropped); x++)
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

			paths.push_back(npszFile);

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
			paths.push_back(szPath);
			hr = S_OK;
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	m_contextMenu.UpdateMenuState(paths);
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

	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if (uFlags & CMF_DEFAULTONLY)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	DWORD dwMenuState = m_contextMenu.GetMenuState();
	if (dwMenuState == WinMergeContextMenu::MENU_HIDDEN)
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

	int uidUserLastCmd = DrawMenu(hmenu, uMenuIndex, uidFirstCmd);

	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, (uidUserLastCmd - uidFirstCmd) + 1);
}

/// Gets string shown explorer's status bar when menuitem selected
HRESULT CWinMergeShell::GetCommandString(UINT_PTR idCmd, UINT uFlags,
		UINT* pwReserved, LPSTR pszName, UINT  cchMax)
{
	return S_FALSE;
}

/// Runs WinMerge with given paths
HRESULT CWinMergeShell::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	// If lpVerb really points to a string, ignore this function call and bail out.
	if (HIWORD(pCmdInfo->lpVerb) != 0)
		return E_INVALIDARG;

	//auto menuList = m_contextMenu.GetMenuItemList();
	return m_contextMenu.InvokeCommand(LOWORD(pCmdInfo->lpVerb));//menuList[LOWORD(pCmdInfo->lpVerb)].verb);
}

/// Create menu
int CWinMergeShell::DrawMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd)
{
	for (auto& menuItem : m_contextMenu.GetMenuItemList())
	{
		MENUITEMINFO mii{ sizeof mii };
		mii.fMask = MIIM_ID | MIIM_STRING | MIIM_DATA | MIIM_BITMAP | MIIM_STATE;
		mii.wID = uidFirstCmd + menuItem.verb;
		mii.dwTypeData = const_cast<LPTSTR>(menuItem.text.c_str());
		mii.dwItemData = menuItem.strid;
		mii.hbmpItem = menuItem.icon == IDI_WINMERGE ? m_MergeBmp : m_MergeDirBmp;
		mii.fState = menuItem.enabled ? 0 : MFS_GRAYED;
		InsertMenuItem(hmenu, uMenuIndex++, TRUE, &mii);
		s_uidCmdLastAdded = mii.wID;
	}
	s_hMenuLastAdded = hmenu;
	return uidFirstCmd + WinMergeContextMenu::CMD_LAST;
}
