#include "StdAfx.h"
#include "FileFilterMenu.h"
#include "Merge.h"
#include "resource.h"

static CMenu* CloneMenuRecursive(CMenu* pSrcMenu)
{
	if (!pSrcMenu) return nullptr;

	CMenu* pNewMenu = new CMenu;
	if (!pNewMenu->CreatePopupMenu())
	{
		delete pNewMenu;
		return nullptr;
	}
	const int itemCount = pSrcMenu->GetMenuItemCount();
	for (int i = 0; i < itemCount; ++i)
	{
		UINT state = pSrcMenu->GetMenuState(i, MF_BYPOSITION);
		CString text;
		pSrcMenu->GetMenuString(i, text, MF_BYPOSITION);

		if (state & MF_SEPARATOR)
		{
			pNewMenu->AppendMenu(MF_SEPARATOR);
		}
		else if (state & MF_POPUP)
		{
			CMenu* pSrcSub = pSrcMenu->GetSubMenu(i);
			CMenu* pClonedSub = CloneMenuRecursive(pSrcSub);
			if (pClonedSub)
			{
				pNewMenu->AppendMenu(MF_POPUP, (UINT_PTR)pClonedSub->GetSafeHmenu(), text);
			}
		}
		else
		{
			UINT id = pSrcMenu->GetMenuItemID(i);
			pNewMenu->AppendMenu(MF_STRING, id, text);
		}
	}
	return pNewMenu;
}

String CFileFilterMenu::ShowMenu(int x, int y, CWnd* pParentWnd)
{
	String result;
	VERIFY(LoadMenu(IDR_POPUP_FILTERMENU));
	m_menuFile.LoadMenu(IDR_POPUP_FILTERMENU_FILE);
	theApp.TranslateMenu(m_hMenu);
	theApp.TranslateMenu(m_menuFile.m_hMenu);
	CMenu* pPopup = GetSubMenu(0);
	if (pPopup)
	{
		const int command = pPopup->TrackPopupMenu(
			TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, x, y, pParentWnd);
		const String Sides[] = { _T(""), _T("Left"), _T("Middle"), _T("Right") };
		if (command >= ID_FILTERMENU_SIZE_LT_1KB && command <= ID_FILTERMENU_SIZE_GE_1GB)
		{
			static const String Sizes[] = {
				_T("%s < 1KB"), _T("%s >= 1KB"),
				_T("%s < 10KB"), _T("%s >= 10KB"),
				_T("%s < 100KB"), _T("%s >= 100KB"),
				_T("%s < 1MB"), _T("%s >= 1MB"),
				_T("%s < 10MB"), _T("%s >= 10MB"),
				_T("%s < 100MB"), _T("%s >= 100MB"),
				_T("%s < 1GB"), _T("%s >= 1GB")
			};
			const String identifier = Sides[m_topMenuId - ID_FILTERMENU_NO_FILE_CONDITION_ANY] + _T("Size");
			result = _T("|fe:") + strutils::format(Sizes[command - ID_FILTERMENU_SIZE_LT_1KB], identifier);
		}
	}
	DestroyMenu();
	m_menuFile.DestroyMenu();
	return result;
}

void CFileFilterMenu::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	const int topMenuId = pPopupMenu->GetMenuItemID(0);
	if (topMenuId >= ID_FILTERMENU_NO_FILE_CONDITION_ANY && topMenuId <= ID_FILTERMENU_NO_FILE_CONDITION_RIGHT)
	{
		m_topMenuId = topMenuId;
		pPopupMenu->DeleteMenu(0, MF_BYPOSITION);
		CMenu* pSubMenu = m_menuFile.GetSubMenu(0);
		const int count = pSubMenu->GetMenuItemCount();
		for (int i = 0; i < count; ++i) {
			CString text;
			pSubMenu->GetMenuString(i, text, MF_BYPOSITION);
			CMenu* pSubMenu2 = CloneMenuRecursive(pSubMenu->GetSubMenu(i));
			if (pSubMenu2)
			{
				pPopupMenu->AppendMenu(MF_POPUP, (UINT_PTR)pSubMenu2->GetSafeHmenu(), text);
				pSubMenu2->Detach();
				delete pSubMenu2;
			}
		}
	}
}
