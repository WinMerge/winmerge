/////////////////////////////////////////////////////////////////////////////
//	  WinMerge:  an interactive diff/merge utility
//	  Copyright (C) 1997-2000  Thingamahoochie Software
//	  Author: Dean Grimm
//
//	  This program is free software; you can redistribute it and/or modify
//	  it under the terms of the GNU General Public License as published by
//	  the Free Software Foundation; either version 2 of the License, or
//	  (at your option) any later version.
//
//	  This program is distributed in the hope that it will be useful,
//	  but WITHOUT ANY WARRANTY; without even the implied warranty of
//	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
//	  GNU General Public License for more details.
//
//	  You should have received a copy of the GNU General Public License
//	  along with this program; if not, write to the Free Software
//	  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ShellContextMenu.cpp
 *
 * @brief Main implementation file for CShellContextMenu
 */

#include "pch.h"
#include "ShellContextMenu.h"
#include "PidlContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CShellContextMenu::CShellContextMenu(UINT cmdFirst, UINT cmdLast)
: m_pPreferredMenu(nullptr)
, m_pShellContextMenu2(nullptr)
, m_pShellContextMenu3(nullptr)
, m_hShellContextMenu(nullptr)
, m_cmdFirst(cmdFirst)
, m_cmdLast(cmdLast)
{
}

CShellContextMenu::~CShellContextMenu()
{
	if (::IsMenu(m_hShellContextMenu))
	{
		::DestroyMenu(m_hShellContextMenu);
	}
}

void CShellContextMenu::Initialize()
{
	if (!::IsMenu(m_hShellContextMenu)) // first call or menu was destroyed by parent after it was added as submenu
	{
		m_hShellContextMenu = CreatePopupMenu();
	}
	m_files.clear();
}

void CShellContextMenu::AddItem(const FileEntry& fileEntry)
{
	m_files.insert(m_files.end(), fileEntry);
}

void CShellContextMenu::AddItem(const String& path,
								const String& filename)
{
	AddItem(FileEntry(path, filename));
}

HMENU CShellContextMenu::GetHMENU() const
{
	return ::IsMenu(m_hShellContextMenu) ? m_hShellContextMenu : nullptr;
}

bool CShellContextMenu::HandleMenuMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& retval)
{
	//HRESULT hr = E_FAIL;
	switch (message)
	{
	case WM_DRAWITEM:
	case WM_INITMENUPOPUP:
	case WM_MEASUREITEM:
		if (m_pShellContextMenu3 != nullptr)
		{
			if (FAILED(/*hr = */m_pShellContextMenu3->HandleMenuMsg(message, wParam, lParam/*, &retval*/)))
			{
				//TRACE(_T("HandleMenuMsg(%x) failed with error: %lx\n"), message, hr);
				//if (message == WM_DRAWITEM)
				//{
				//	  LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;
				//	  (HMENU)hMenu = (HMENU)lpDrawItem->hwndItem;
				//	  TRACE(_T("  hMenu == 0x%p\n"), hMenu);
				//}
				//assert(false);
			}
		}
		else if (m_pShellContextMenu2 != nullptr)
		{
			if (FAILED(/*hr = */m_pShellContextMenu2->HandleMenuMsg(message, wParam, lParam)))
			{
				//TRACE(_T("HandleMenuMsg(%x) failed with error: %lx\n"), message, hr);
				//assert(false);
			}
		}
		// indicate that we've processed the message
		retval = (message == WM_INITMENUPOPUP) ? 0 : TRUE;
		return true;
	case WM_MENUCHAR:
		if (m_pShellContextMenu3 != nullptr)
		{
			retval = 0;
			if (FAILED(/*hr = */m_pShellContextMenu3->HandleMenuMsg2(message, wParam, lParam, &retval)))
			{
				//TRACE(_T("HandleMenuMsg2(%x) failed with error: %lx\n"), message, hr);
				//assert(false);
			}
			return true;
		}
	}
	return false;
}

bool CShellContextMenu::QueryShellContextMenu()
{
	//HRESULT hr = E_FAIL;
	IShellFolderPtr pDesktop;
	if (FAILED(/*hr = */SHGetDesktopFolder(&pDesktop)))
		return false;

	String parentDir; // use it to track that all selected files are in the same parent directory
	IShellFolderPtr pCurrFolder;
	CPidlContainer pidls;

	for (FilenamesContainer::const_iterator iter = m_files.begin(); iter != m_files.end(); ++iter)
	{
		const FileEntry& file = *iter;

		String currentDir = file.path;

		if (parentDir.empty()) // first iteration, initialize parentDir and pCurrFolder
		{
			parentDir = currentDir;

			LPITEMIDLIST dirPidl;
			if (FAILED(/*hr = */pDesktop->ParseDisplayName(nullptr,				   // hwnd
													   nullptr,					   // pbc
													   const_cast<wchar_t *>(currentDir.c_str()),	// pszDisplayName
													   nullptr,					   // pchEaten
													   &dirPidl,				   // ppidl
													   nullptr					   // pdwAttributes
													   )))
			{
				return false;
			}

			if (FAILED(/*hr = */pDesktop->BindToObject(dirPidl,			 // pidl
												   nullptr,				 // pbc
												   IID_IShellFolder,     // riid
												   reinterpret_cast<void**>(&pCurrFolder))))
			{
				return false;
			}
		}
		else if (currentDir != parentDir) // check whether file belongs to the same parentDir, break otherwise
		{
			return false;
		}

		LPITEMIDLIST pidl;
		if (FAILED(/*hr = */pCurrFolder->ParseDisplayName(nullptr,
													  nullptr,
													  const_cast<wchar_t *>(file.filename.c_str()), 
													  nullptr,
													  &pidl,
													  nullptr)))
		{
			return false;
		}
		
		pidls.Add(pidl);
	} // for (FilenamesContainer::const_iterator iter = m_files.begin(); iter != m_files.end(); ++iter)
	
	if (0 == pidls.Size()) // no items to show menu for
	{
		return false;
	}

	IContextMenuPtr pCMenu1;
	if (FAILED(/*hr = */pCurrFolder->GetUIObjectOf(nullptr,
											   static_cast<unsigned>(pidls.Size()),
											   pidls.GetList(),
											   IID_IContextMenu,
											   0, 
											   reinterpret_cast<void**>(&pCMenu1))))
	{
		return false;
	}

	m_pPreferredMenu = pCMenu1;

	IContextMenu2Ptr pCMenu2(pCMenu1);
	if (pCMenu2 != nullptr)
	{
		m_pPreferredMenu = pCMenu2;
	}

	IContextMenu3Ptr pCMenu3(pCMenu1);
	if (pCMenu3 != nullptr)
	{
		m_pPreferredMenu = pCMenu3;
	}

	assert(::IsMenu(m_hShellContextMenu));
	if (FAILED(/*hr = */m_pPreferredMenu->QueryContextMenu(m_hShellContextMenu,
													   0,
													   m_cmdFirst,
													   m_cmdLast,
													   CMF_EXPLORE | CMF_CANRENAME)))
	{
		return false;
	}

	m_pShellContextMenu2 = pCMenu2;
	m_pShellContextMenu3 = pCMenu3;

	return true;
}

bool CShellContextMenu::InvokeCommand(UINT nCmd, HWND hWnd)
{
	if (nCmd >= m_cmdFirst && nCmd <= m_cmdLast)
	{
		CMINVOKECOMMANDINFO ici = {sizeof(CMINVOKECOMMANDINFO)};

		ici.hwnd = hWnd;
		nCmd -= m_cmdFirst; // adjust command id
		ici.lpVerb = MAKEINTRESOURCEA(nCmd);
		ici.nShow = SW_SHOWNORMAL;

		HRESULT hr = m_pPreferredMenu->InvokeCommand(&ici);
		assert(SUCCEEDED(hr));
		return SUCCEEDED(hr);
	}
	else 
	{
		return false;
	}
}

bool CShellContextMenu::RequeryShellContextMenu()
{
	assert(::IsMenu(m_hShellContextMenu));
	while (::GetMenuItemCount(m_hShellContextMenu) > 0)
	{
		::DeleteMenu(m_hShellContextMenu, 0, MF_BYPOSITION);
	}
	assert(::GetMenuItemCount(m_hShellContextMenu) == 0);

	return QueryShellContextMenu();
}

void CShellContextMenu::ReleaseShellContextMenu()
{
	m_pShellContextMenu2 = nullptr;
	m_pShellContextMenu3 = nullptr;
	m_pPreferredMenu = nullptr;
}
