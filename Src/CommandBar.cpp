// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/**
 * @file  CommandBar.cpp
 *
 * @brief Implementation of the CCommandBar class
 */

#include "StdAfx.h"
#include "CommandBar.h"

static const UINT UWM_SHOWPOPUPMENU = WM_APP + 1;
HHOOK CCommandBar::m_hHook = nullptr;
CCommandBar* CCommandBar::m_pThis = nullptr;

IMPLEMENT_DYNAMIC(CCommandBar, CToolBar)

BEGIN_MESSAGE_MAP(CCommandBar, CToolBar)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(UWM_SHOWPOPUPMENU, OnShowPopupMenu)
END_MESSAGE_MAP()

CCommandBar::CCommandBar()
	: m_hMenu(nullptr)
	, m_bActive(false)
	, m_bMouseTracking(false)
	, m_nMDIButtonDown(-1)
	, m_hwndOldFocus(nullptr)
	, m_nCurrentMenuItemFlags(0)
	, m_nCurrentHotItem(-1)
	, m_hCurrentPopupMenu(nullptr)
	, m_bShowKeyboardCues(false)
{
	m_pThis = this;
}

static TBBUTTON makeTBButton(int id, const TCHAR* str)
{
	TBBUTTON btn{ I_IMAGENONE, id, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_DROPDOWN | BTNS_AUTOSIZE };
	btn.iString = reinterpret_cast<intptr_t>(str);
	return btn;
}

BOOL CCommandBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	bool ret = __super::CreateEx(pParentWnd, TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TRANSPARENT, dwStyle,
		CRect(0, 0, 0, 0), nID);

	CToolBarCtrl& toolbar = GetToolBarCtrl();
	TEXTMETRIC tm;
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(toolbar.GetFont());
	dc.GetTextMetrics(&tm);
	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](float point) -> int { return static_cast<int>(point * lpx / 72.0f); };
	const int cy = pointToPixel(3);
	dc.SelectObject(pOldFont);

	TBBUTTON btn = makeTBButton(FIRST_MENUID, _T(""));
	toolbar.SetButtonStructSize(sizeof(TBBUTTON));
	toolbar.SetDrawTextFlags(DT_HIDEPREFIX, DT_HIDEPREFIX);
	toolbar.InsertButton(0, &btn);
	toolbar.SetButtonSize(CSize(tm.tmHeight + cy * 2, tm.tmHeight + cy * 2));

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

			btns[i] = makeTBButton(FIRST_MENUID + i, menuStrings[i].c_str());
		}

		toolbar.SendMessage(TB_ADDBUTTONSW, nItems, (LPARAM)btns.data());
	}

	toolbar.SetRedraw(true);

	return true;
}

void CCommandBar::DrawMDIButtons(HDC hDC)
{
	const int nTypes[3] = {
		DFCS_CAPTIONMIN     | DFCS_FLAT | (m_nMDIButtonDown == 0 ? DFCS_PUSHED : 0),
		DFCS_CAPTIONRESTORE | DFCS_FLAT | (m_nMDIButtonDown == 1 ? DFCS_PUSHED : 0),
		DFCS_CAPTIONCLOSE   | DFCS_FLAT | (m_nMDIButtonDown == 2 ? DFCS_PUSHED : 0)
	};
	CRect rcButtons = GetMDIButtonsRect();
	CRect rcItem;
	GetToolBarCtrl().GetItemRect(0, &rcItem);
	const int bw = rcItem.Height();
	const int w = rcButtons.Height();
	CRect rc{ rcButtons.left, rcButtons.top + (w - bw) / 2, rcButtons.left + bw, rcButtons.top + (w + bw) / 2};
	for (int i = 0; i < 3; ++i)
	{
		::DrawFrameControl(hDC, rc, DFC_CAPTION, nTypes[i]);
		rc.left += w;
		rc.right += w;
	}
}

int CCommandBar::GetMDIButtonIndexFromPoint(CPoint pt) const
{
	CRect rcButtons = GetMDIButtonsRect();
	if (!rcButtons.PtInRect(pt))
		return -1;
	for (int i = 0; i < 3; ++i)
	{
		if (GetMDIButtonRect(i).PtInRect(pt))
			return i;
	}
	return -1;
}

CRect CCommandBar::GetMDIButtonsRect() const
{
	CRect rcClient;
	GetClientRect(&rcClient);
	return { rcClient.right - rcClient.Height() * 3, rcClient.top, rcClient.right, rcClient.bottom };
}

CRect CCommandBar::GetMDIButtonRect(int nItem) const
{
	CRect rcButtons = GetMDIButtonsRect();
	const int w = rcButtons.Height();
	return { rcButtons.left + w * nItem, rcButtons.top, rcButtons.left + w * nItem + w, rcButtons.bottom };
}

static bool IsMDIChildMaximized()
{
	CMDIFrameWnd* pMDIFrameWnd = DYNAMIC_DOWNCAST(CMDIFrameWnd, AfxGetMainWnd());
	if (!pMDIFrameWnd)
		return false;
	BOOL bMaximized = FALSE;
	pMDIFrameWnd->MDIGetActive(&bMaximized);
	return bMaximized;
}

void CCommandBar::LoseFocus()
{
	m_bActive = false;
	m_hwndOldFocus = nullptr;
	m_bShowKeyboardCues = false;
	GetToolBarCtrl().SetDrawTextFlags(DT_HIDEPREFIX, DT_HIDEPREFIX);
	GetToolBarCtrl().SetHotItem(-1);
	Invalidate();
}

void CCommandBar::OnSetFocus(CWnd* pOldWnd)
{
	m_bActive = true;
	m_hwndOldFocus = pOldWnd->m_hWnd;
	__super::OnSetFocus(pOldWnd);
}

void CCommandBar::OnKillFocus(CWnd* pNewWnd)
{
	LoseFocus();
	__super::OnKillFocus(pNewWnd);
}

void CCommandBar::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTBCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTBCUSTOMDRAW>(pNMHDR);
	DWORD dwDrawState = pNMCD->nmcd.dwDrawStage;
	if (dwDrawState == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		return;
	}
	else if (dwDrawState == CDDS_ITEMPREPAINT)
	{
		pNMCD->clrText = GetSysColor(COLOR_MENUTEXT);
		*pResult = CDRF_DODEFAULT | TBCDRF_USECDCOLORS;
		return;
	}
	else if (dwDrawState == CDDS_POSTPAINT &&
		(m_bMouseTracking && IsMDIChildMaximized()))
	{
		DrawMDIButtons(pNMCD->nmcd.hdc);
	}
	*pResult = 0;
}

void CCommandBar::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rcMDIButtons = GetMDIButtonsRect();
	InvalidateRect(&rcMDIButtons);
	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme = { sizeof TRACKMOUSEEVENT, TME_LEAVE, m_hWnd };
		TrackMouseEvent(&tme);
		m_bMouseTracking = true;
	}
	__super::OnMouseMove(nFlags, point);
}

void CCommandBar::OnMouseLeave()
{
	TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_CANCEL, m_hWnd };
	TrackMouseEvent(&tme);
	m_bMouseTracking = false;
	CRect rcButtons = GetMDIButtonsRect();
	InvalidateRect(&rcButtons);
	m_nMDIButtonDown = -1;
	__super::OnMouseLeave();
}

void CCommandBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rcButtons = GetMDIButtonsRect();
	if (rcButtons.PtInRect(point))
	{
		m_nMDIButtonDown = GetMDIButtonIndexFromPoint(point);
		InvalidateRect(rcButtons);
		return;
	}
	__super::OnLButtonDown(nFlags, point);
}

void CCommandBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rcButtons = GetMDIButtonsRect();
	if (m_nMDIButtonDown != -1 && rcButtons.PtInRect(point))
	{
		CMDIFrameWnd* pMDIFrameWnd = DYNAMIC_DOWNCAST(CMDIFrameWnd, AfxGetMainWnd());
		if (pMDIFrameWnd)
		{
			CFrameWnd* pFrameWnd = pMDIFrameWnd->GetActiveFrame();
			switch (GetMDIButtonIndexFromPoint(point))
			{
			case 0:
				pFrameWnd->ShowWindow(SW_MINIMIZE);
				break;
			case 1:
				::SendMessage(pMDIFrameWnd->m_hWndMDIClient, WM_MDIRESTORE, (WPARAM)(pFrameWnd->m_hWnd), 0);
				break;
			case 2:
				pFrameWnd->PostMessage(WM_CLOSE);
				break;
			}
		}
	}
	InvalidateRect(rcButtons);
	m_nMDIButtonDown = -1;
	__super::OnLButtonUp(nFlags, point);
}

void CCommandBar::OnCommandBarMenuItem(UINT nID)
{
	const int nHotItem = nID - FIRST_MENUID;
	CMenu* pPopup = CMenu::FromHandle(::GetSubMenu(m_hMenu, nHotItem));
	if (!pPopup)
		return;

	CToolBarCtrl& toolbar = GetToolBarCtrl();
	toolbar.SetHotItem(nHotItem);
	m_nCurrentMenuItemFlags = 0;
	m_nCurrentHotItem = nHotItem;
	m_hCurrentPopupMenu = pPopup->m_hMenu;
	m_pThis = this;

	m_hHook = SetWindowsHookEx(WH_MSGFILTER, MsgFilterProc, nullptr, GetCurrentThreadId());

	CRect rc;
	toolbar.GetItemRect(nHotItem, &rc);
	ClientToScreen(&rc);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rc.left, rc.bottom, AfxGetMainWnd());

	UnhookWindowsHookEx(m_hHook);
}

void CCommandBar::OnUpdateCommandBarMenuItem(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

LRESULT CCommandBar::OnShowPopupMenu(WPARAM wParam, LPARAM lParam)
{
	OnCommandBarMenuItem(static_cast<UINT>(wParam));
	return 0;
}

BOOL CCommandBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP)
	{
		if (pMsg->wParam == VK_F10 || pMsg->wParam == VK_MENU)
		{
			if (pMsg->message == WM_SYSKEYDOWN)
			{
				m_bShowKeyboardCues = true;
				GetToolBarCtrl().SetDrawTextFlags(DT_HIDEPREFIX, 0);
				Invalidate();
			}
			else if (pMsg->message == WM_SYSKEYUP && m_bShowKeyboardCues)
			{
				if (!m_bActive)
					SetFocus();
				else if (m_hwndOldFocus != nullptr && IsWindow(m_hwndOldFocus))
					::SetFocus(m_hwndOldFocus);
			}
			return TRUE;
		}
		UINT uId = 0;
		if ((pMsg->message == WM_SYSKEYDOWN) && GetToolBarCtrl().MapAccelerator(static_cast<TCHAR>(pMsg->wParam), &uId) != 0)
		{
			OnCommandBarMenuItem(uId);
			return TRUE;
		}
	}
	else if (pMsg->message == WM_KEYDOWN && m_bActive && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			if (m_hwndOldFocus != nullptr && IsWindow(m_hwndOldFocus))
				::SetFocus(m_hwndOldFocus);
		}
		else if (pMsg->wParam == VK_RETURN)
		{
			OnCommandBarMenuItem(FIRST_MENUID + GetToolBarCtrl().GetHotItem());
		}
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK CCommandBar::MsgFilterProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode != MSGF_MENU)
		return CallNextHookEx(m_hHook, nCode, wParam, lParam);
	MSG* pMsg = reinterpret_cast<MSG*>(lParam);
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT))
	{
		if ((pMsg->wParam == VK_LEFT && ::GetSubMenu(m_pThis->m_hMenu, m_pThis->m_nCurrentHotItem) == m_pThis->m_hCurrentPopupMenu) ||
			(pMsg->wParam == VK_RIGHT && (m_pThis->m_nCurrentMenuItemFlags & MF_POPUP) == 0))
		{
			AfxGetMainWnd()->PostMessage(WM_CANCELMODE);
			CToolBarCtrl& toolbar = m_pThis->GetToolBarCtrl();
			const int nCount = toolbar.GetButtonCount();
			int nHotItem = toolbar.GetHotItem();
			nHotItem = (nHotItem + nCount + ((pMsg->wParam == VK_RIGHT) ? 1 : -1)) % nCount;
			m_pThis->PostMessage(UWM_SHOWPOPUPMENU, FIRST_MENUID + nHotItem);
			return 0;
		}
	}
	else if (pMsg->message == WM_MOUSEMOVE)
	{
		CRect rc;
		m_pThis->GetWindowRect(&rc);
		CPoint pt{ GET_X_LPARAM(pMsg->lParam) - rc.left, GET_Y_LPARAM(pMsg->lParam) - rc.top };
		const int nItem = m_pThis->GetToolBarCtrl().HitTest(&pt);
		if (nItem >= 0 && m_pThis->m_nCurrentHotItem != nItem)
		{
			AfxGetMainWnd()->PostMessage(WM_CANCELMODE);
			m_pThis->PostMessage(UWM_SHOWPOPUPMENU, FIRST_MENUID + nItem);
			return 0;
		}
	}
	else if (pMsg->message == WM_MENUSELECT)
	{
		m_pThis->m_nCurrentMenuItemFlags = HIWORD(pMsg->wParam);
		m_pThis->m_hCurrentPopupMenu = reinterpret_cast<HMENU>(pMsg->lParam);
		if (m_pThis->m_nCurrentMenuItemFlags == 0xffff && m_pThis->m_hCurrentPopupMenu == nullptr) // Menu closing
		{
			if (m_pThis->m_hwndOldFocus && IsWindow(m_pThis->m_hwndOldFocus))
				::SetFocus(m_pThis->m_hwndOldFocus);
			else
				m_pThis->LoseFocus();
		}
	}
	return CallNextHookEx(m_hHook, nCode, wParam, lParam);
}
