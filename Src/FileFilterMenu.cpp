#include "StdAfx.h"
#include "FileFilterMenu.h"
#include "Merge.h"
#include "resource.h"

String CFileFilterMenu::ShowMenu(int x, int y, CWnd* pParentWnd)
{
	VERIFY(LoadMenu(IDR_POPUP_FILTERMENU));
	m_menuFile.LoadMenu(IDR_POPUP_FILTERMENU_FILE);
	theApp.TranslateMenu(m_hMenu);
	theApp.TranslateMenu(m_menuFile.m_hMenu);
	CMenu* pPopup = GetSubMenu(0);
	if (pPopup)
	{
		const int command = pPopup->TrackPopupMenu(
			TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, x, y, pParentWnd);
		if (m_topMenuId == ID_FILTERMENU_NO_FILE_CONDITION_ANY)
		{

		}
	}
	DestroyMenu();
	m_menuFile.DestroyMenu();
	return _T("");
}

void CFileFilterMenu::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	m_topMenuId = pPopupMenu->GetMenuItemID(0);
	if (m_topMenuId >= ID_FILTERMENU_NO_FILE_CONDITION_ANY && m_topMenuId <= ID_FILTERMENU_NO_FILE_CONDITION_RIGHT)
	{
		pPopupMenu->DeleteMenu(0, MF_BYPOSITION);
		CMenu* pSubMenu = m_menuFile.GetSubMenu(0);
		const int count = pSubMenu->GetMenuItemCount();
		for (int i = 0; i < count; ++i) {
			CString text;
			pSubMenu->GetMenuString(i, text, MF_BYPOSITION);
			pPopupMenu->AppendMenu(MF_POPUP, (UINT_PTR)pSubMenu->GetSubMenu(i)->GetSafeHmenu(), text);
		}
	}
}
