/**
 * @file  CommandBar.cpp
 *
 * @brief Implementation of the CCommandBar class
 */

#include "StdAfx.h"
#include "CommandBar.h"

IMPLEMENT_DYNAMIC(CCommandBar, CToolBar)

BEGIN_MESSAGE_MAP(CCommandBar, CToolBar)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

HHOOK CCommandBar::m_hHook = nullptr;
CCommandBar* CCommandBar::m_pThis = nullptr;

CCommandBar::CCommandBar()
	: m_hMenu(nullptr)
	, m_bActive(false)
{
	m_pThis = this;
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
	const int cy = pointToPixel(3);
	toolbar.SetButtonSize(CSize(tm.tmHeight + cy * 2, tm.tmHeight + cy * 2));
	dc.SelectObject(pOldFont);

	if (!m_hHook)
		m_hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, AfxGetInstanceHandle(), GetCurrentThreadId());

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
			MENUITEMINFO mii{ sizeof MENUITEMINFO, MIIM_TYPE | MIIM_STATE | MIIM_SUBMENU, MFT_STRING, 0, 0, nullptr, nullptr, nullptr, 0, szString, sizeof(szString) / sizeof(szString[0]) };
			pMenu->GetMenuItemInfo(i, &mii, TRUE);
			menuStrings[i] = std::basic_string<TCHAR>(L" ") + szString + L" ";

			setTBButton(btns[i], FIRST_MENUID + i, menuStrings[i].c_str());
		}

		toolbar.SendMessage(TB_ADDBUTTONSW, nItems, (LPARAM)btns.data());
	}

	toolbar.SetRedraw(true);

	return true;
}

void CCommandBar::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTBCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTBCUSTOMDRAW>(pNMHDR);
	if (pNMCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;
	}
	else if (pNMCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		pNMCD->clrText = GetSysColor(COLOR_MENUTEXT);
		*pResult = CDRF_DODEFAULT | TBCDRF_USECDCOLORS;
		return;
	}
	*pResult = 0;
}

void CCommandBar::OnCommandBarMenuItem(UINT nID)
{
	CToolBarCtrl& toolbar = GetToolBarCtrl();
	CRect rc;
	toolbar.GetItemRect(nID - FIRST_MENUID, &rc);
	ClientToScreen(&rc);
	CMenu* pMenu = CMenu::FromHandle(m_hMenu);
	CMenu* pPopup = pMenu->GetSubMenu(nID - FIRST_MENUID);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rc.left, rc.bottom, GetParentFrame());
}

void CCommandBar::OnUpdateCommandBarMenuItem(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

LRESULT CALLBACK CCommandBar::GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION && wParam == PM_REMOVE)
	{
		MSG* pMsg = reinterpret_cast<MSG*>(lParam);
		if (pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP)
		{
			CToolBarCtrl& toolbar = m_pThis->GetToolBarCtrl();
			UINT uId = 0;
			if (pMsg->wParam == VK_F10 || pMsg->wParam == VK_MENU)
			{
				if (pMsg->message == WM_SYSKEYUP)
				{
					toolbar.SetHotItem(!m_pThis->m_bActive ? 0 : -1);
					m_pThis->m_bActive = !m_pThis->m_bActive;
				}
			}
			else if (toolbar.MapAccelerator(static_cast<TCHAR>(pMsg->wParam), &uId) != 0)
			{
				m_pThis->OnCommandBarMenuItem(uId);
				return -1;
			}
		}
		else if(pMsg->message == WM_KEYDOWN)
		{
			CToolBarCtrl& toolbar = m_pThis->GetToolBarCtrl();
			if ((m_pThis->m_bActive) && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_RETURN))
			{
				const int nCount = toolbar.GetButtonCount();
				const int nHotItem = toolbar.GetHotItem();
				if (pMsg->wParam == VK_ESCAPE)
				{
					toolbar.SetHotItem(-1);
					m_pThis->m_bActive = !m_pThis->m_bActive;
				}
				else if (pMsg->wParam == VK_LEFT && nHotItem != -1 && nHotItem != 0)
				{
					toolbar.SetHotItem(nHotItem - 1);
				}
				else if (pMsg->wParam == VK_RIGHT && nHotItem != -1 && nHotItem < nCount)
				{
					toolbar.SetHotItem(nHotItem + 1);
				}
				else if ((pMsg->wParam == VK_RETURN || pMsg->wParam == VK_DOWN) && nHotItem >= 0)
				{
					m_pThis->OnCommandBarMenuItem(FIRST_MENUID + nHotItem);
				}
			}
		}
	}
	return CallNextHookEx(m_hHook, nCode, wParam, lParam);
}
