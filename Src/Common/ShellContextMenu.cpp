/////////////////////////////////////////////////////////////////////////////
//	  WinMerge:  an interactive diff/merge utility
//	  Copyright (C) 1997-2000  Thingamahoochie Software
//	  Author: Dean Grimm
//	  SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ShellContextMenu.cpp
 *
 * @brief Main implementation file for CShellContextMenu
 */

#include "pch.h"
#include "ShellContextMenu.h"
#include "PidlContainer.h"
#include <Shlwapi.h>

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

void CShellContextMenu::AddItem(const std::wstring& fullpath)
{
	m_files.insert(m_files.end(), fullpath);
}

void CShellContextMenu::AddItem(const std::wstring& path,
								const std::wstring& filename)
{
	AddItem(path + L"\\" + filename);
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

static HRESULT _stdcall dfmCallback(IShellFolder* /*psf*/, HWND /*hwnd*/, IDataObject* /*pdtobj*/, UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	switch (uMsg)
	{
	case DFM_MERGECONTEXTMENU:
		return S_OK;
	case DFM_INVOKECOMMAND:
	case DFM_INVOKECOMMANDEX:
	case DFM_GETDEFSTATICID: // Required for Windows 7 to pick a default
		return S_FALSE;
	}
	return E_NOTIMPL;
}

bool CShellContextMenu::QueryShellContextMenu()
{
	//HRESULT hr = E_FAIL;
	IShellFolderPtr pDesktop;
	if (FAILED(/*hr = */SHGetDesktopFolder(&pDesktop)))
		return false;

	CPidlContainer pidls;

	for (FilenamesContainer::const_iterator iter = m_files.begin(); iter != m_files.end(); ++iter)
	{
		const std::wstring& path = *iter;
		LPITEMIDLIST pidl;
		if (FAILED(/*hr = */pDesktop->ParseDisplayName(nullptr,				   // hwnd
												   nullptr,					   // pbc
												   const_cast<wchar_t *>(path.c_str()),	// pszDisplayName
												   nullptr,					   // pchEaten
												   &pidl,					   // ppidl
												   nullptr					   // pdwAttributes
												   )))
		{
			return false;
		}
		pidls.Add(pidl);
	} // for (FilenamesContainer::const_iterator iter = m_files.begin(); iter != m_files.end(); ++iter)
	
	if (0 == pidls.Size()) // no items to show menu for
	{
		return false;
	}

	// The following was created with reference to https://github.com/stefankueng/grepWin/blob/main/src/ShellContextMenu.cpp.
	HKEY ahkeys[16]{};
	int nKeys = 0;
	const std::wstring& path = m_files.front();
	nKeys += RegOpenKey(HKEY_CLASSES_ROOT, L"*", &ahkeys[nKeys]) == ERROR_SUCCESS ? 1 : 0;
	nKeys += RegOpenKey(HKEY_CLASSES_ROOT, L"AllFileSystemObjects", &ahkeys[nKeys]) == ERROR_SUCCESS ? 1 : 0;
	if (PathIsDirectory(path.c_str()))
	{
		nKeys += RegOpenKey(HKEY_CLASSES_ROOT, L"Folder", &ahkeys[nKeys]) == ERROR_SUCCESS ? 1 : 0;
		nKeys += RegOpenKey(HKEY_CLASSES_ROOT, L"Directory", &ahkeys[nKeys]) == ERROR_SUCCESS ? 1 : 0;
	}
	HKEY hkey;
	const wchar_t* ext = PathFindExtension(path.c_str());
	if (ext && *ext == '.' && RegOpenKey(HKEY_CLASSES_ROOT, ext, &hkey) == ERROR_SUCCESS)
	{
		wchar_t buf[MAX_PATH] = { 0 };
		DWORD dwSize = MAX_PATH;
		if (RegQueryValueEx(hkey, L"", nullptr, nullptr, reinterpret_cast<LPBYTE>(buf), &dwSize) == ERROR_SUCCESS)
			nKeys += RegOpenKey(HKEY_CLASSES_ROOT, buf, &ahkeys[nKeys]) == ERROR_SUCCESS ? 1 : 0;
		RegCloseKey(hkey);
	}

	IContextMenuPtr pCMenu1;
	HRESULT hr = CDefFolderMenu_Create2(nullptr, nullptr,
		static_cast<unsigned>(pidls.Size()),
		pidls.GetList(), pDesktop, dfmCallback, nKeys, ahkeys, &pCMenu1);

	for (int i = 0; i < nKeys; ++i)
		RegCloseKey(ahkeys[i]);

	if (FAILED(hr))
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

	m_pShellContextMenu2 = std::move(pCMenu2);
	m_pShellContextMenu3 = std::move(pCMenu3);

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
