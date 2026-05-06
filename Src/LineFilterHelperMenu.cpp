/**
 * @file  LineFilterHelperMenu.cpp
 */
#include "StdAfx.h"
#include "LineFilterHelperMenu.h"
#include "LineFilterHelper.h"
#include "FilterConditionDlg.h"
#include "resource.h"

std::optional<String> CLineFilterHelperMenu::ShowMenu(const String& masks, int x, int y, CWnd* pParentWnd)
{
	std::optional<String> result;
	VERIFY(LoadMenu(IDR_POPUP_LINEFILTERMENU));
	I18n::TranslateMenu(m_hMenu);
	for (;;)
	{
		CMenu* pPopup = GetSubMenu(0);
		if (pPopup)
		{
			for (int i = ID_FILTERMENU_CONDITION_ANY; i <= ID_FILTERMENU_CONDITION_RIGHT; i++)
				pPopup->CheckMenuItem(i,
					MF_BYCOMMAND | ((ID_FILTERMENU_CONDITION_ANY + m_targetSide) == i ? MF_CHECKED : 0));

			const int command = pPopup->TrackPopupMenu(
				TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, x, y, pParentWnd);
			if (command == 0)
			{
				// User cancelled the menu
			}
			else if (command >= ID_FILTERMENU_CONDITION_ANY && command <= ID_FILTERMENU_CONDITION_RIGHT)
			{
				m_targetSide = command - ID_FILTERMENU_CONDITION_ANY;
				continue;
			}
			else
			{
				result = OnCommand(masks, command, pParentWnd);
			}
		}
		break;
	}
	DestroyMenu();

	return result;
}

std::optional<String> CLineFilterHelperMenu::OnCommand(const String& masks, int command, CWnd* pParentWnd)
{
	std::optional<String> result;
	return result;
}

std::optional<String> CLineFilterHelperMenu::HandleMenuCommand(const String& masks, int command, CWnd* pParentWnd)
{
	return OnCommand(masks, command, pParentWnd);
}
