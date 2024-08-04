/**
 * @file  CommandBar.cpp
 *
 * @brief Implementation of the CCommandBar class
 */

#include "StdAfx.h"
#include "CommandBar.h"

IMPLEMENT_DYNAMIC(CCommandBar, CToolBar)

BEGIN_MESSAGE_MAP(CCommandBar, CToolBar)
END_MESSAGE_MAP()

CCommandBar::CCommandBar() : m_hMenu(nullptr)
{
}

static void setTBButton(TBBUTTON& btn, int id, const TCHAR* str)
{
	btn.iBitmap = I_IMAGENONE;
	btn.idCommand = id;
	btn.fsState = TBSTATE_ENABLED;
	btn.fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE;
	btn.dwData = 0;
	btn.iString = reinterpret_cast<intptr_t>(str);
}

BOOL CCommandBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	bool ret = __super::CreateEx(pParentWnd, TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TRANSPARENT, dwStyle,
		CRect(0, 0, 0, 0), nID);

	CToolBarCtrl& toolbar = GetToolBarCtrl();
	toolbar.SetButtonStructSize(sizeof(TBBUTTON));

	TBBUTTON btn;
	setTBButton(btn, FIRST_MENUID, _T(""));
	toolbar.InsertButton(0, &btn);

	TEXTMETRIC tm;
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(toolbar.GetFont());
	dc.GetTextMetrics(&tm);
	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](float point) -> int { return static_cast<int>(point * lpx / 72.0f); };
	const int cy = pointToPixel(1.5);
	toolbar.SetButtonSize(CSize(tm.tmHeight + cy * 2, tm.tmHeight + cy * 2));
	dc.SelectObject(pOldFont);
	return ret;
}

bool CCommandBar::AttachMenu(CMenu* pMenu)
{
	CToolBarCtrl& toolbar = GetToolBarCtrl();

	toolbar.SetRedraw(false);

	const int nCount = toolbar.GetButtonCount();
	for (int i = 0; i < nCount; i++)
		toolbar.DeleteButton(0);


	if (pMenu && pMenu->m_hMenu)
	{
		m_hMenu = pMenu->m_hMenu;
		
		const int nItems = pMenu->GetMenuItemCount();
		std::vector<TBBUTTON> btns(nItems);
		std::vector<std::basic_string<TCHAR>> menuStrings(nItems);

		for (int i = 0; i < nItems; i++)
		{
			TCHAR szString[256]{};
			MENUITEMINFO mii{ sizeof MENUITEMINFO };
			mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_SUBMENU;
			mii.fType = MFT_STRING;
			mii.dwTypeData = szString;
			mii.cch = sizeof(szString) / sizeof(szString[0]);
			pMenu->GetMenuItemInfo(i, &mii, TRUE);
			menuStrings[i] = std::basic_string<TCHAR>(L" ") + szString + L" ";

			setTBButton(btns[i], FIRST_MENUID + i, menuStrings[i].c_str());
		}

		toolbar.SendMessage(TB_ADDBUTTONSW, nItems, (LPARAM)btns.data());
	}

	toolbar.SetRedraw(true);

	return true;
}

void CCommandBar::OnCommandBarMenuItem(UINT nID)
{
	CToolBarCtrl& toolbar = GetToolBarCtrl();
	CRect rc;
	toolbar.GetItemRect(nID - 10000, &rc);
	ClientToScreen(&rc);
	CMenu* pMenu = CMenu::FromHandle(m_hMenu);
	CMenu* pPopup = pMenu->GetSubMenu(nID - 10000);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rc.left, rc.bottom, GetParentFrame());
}

void CCommandBar::OnUpdateCommandBarMenuItem(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

