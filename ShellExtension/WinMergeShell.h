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
/**
 * @file  WinMergeShell.h
 *
 * @brief Declaration file for ShellExtension class
 */
// ID line follows -- this is updated by SVN
// $Id: WinMergeShell.h 6933 2009-07-26 14:07:03Z kimmov $

#pragma once

#include <shlobj.h>
#include <shlguid.h>
#include <comdef.h>
#include <atlconv.h>	// for ATL string conversion macros
#include "resource.h"   // main symbols
#include "UnicodeString.h"

/**
 * @brief Class for handling shell extension tasks
 */
class ATL_NO_VTABLE CWinMergeShell :
		public CComObjectRootEx<CComSingleThreadModel>,
		public CComCoClass<CWinMergeShell, &CLSID_WinMergeShell>,
		public IDispatchImpl<IWinMergeShell, &IID_IWinMergeShell, &LIBID_SHELLEXTENSIONLib>,
		public IShellExtInit,
		public IContextMenu
{
public:
	CWinMergeShell();
	~CWinMergeShell();

	DECLARE_REGISTRY_RESOURCEID(IDR_WINMERGESHELL)

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(CWinMergeShell)
	COM_INTERFACE_ENTRY(IWinMergeShell)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
	END_COM_MAP()

// IWinMergeShell
protected:
	String m_strPaths[3]; /**< Paths for selected items */
	String m_strPreviousPath; /**< Previously selected path */
	HBITMAP m_MergeBmp, m_MergeDirBmp; /**< Icon */
	UINT m_nSelectedItems; /**< Amount of selected items */
	DWORD m_dwContextMenuEnabled; /**< Is context menu enabled and in which mode? */
	DWORD m_dwMenuState; /**< Shown menuitems */

	BOOL GetWinMergeDir(String &strDir);
	int DrawSimpleMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd);
	int DrawAdvancedMenu(HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd);
	String GetHelpText(UINT_PTR idCmd);
	String FormatCmdLine(const String &winmergePath,
			const String &path1, const String &path2, const String &path3, BOOL bAlterSubFolders);

public:
	// IShellExtInit
	STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

	// IContextMenu
	STDMETHOD(GetCommandString)(UINT_PTR, UINT, UINT*, LPSTR, UINT);
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO);
	STDMETHOD(QueryContextMenu)(HMENU, UINT, UINT, UINT, UINT);
};

