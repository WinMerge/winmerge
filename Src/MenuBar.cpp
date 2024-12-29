// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/**
 * @file  MenuBar.cpp
 *
 * @brief Implementation of the CMenuBar class
 */

#include "StdAfx.h"
#include "MenuBar.h"

HHOOK CMenuBar::m_hHook = nullptr;
CMenuBar* CMenuBar::m_pThis = nullptr;

#ifndef TBCDRF_USECDCOLORS
#define TBCDRF_USECDCOLORS 0x00800000
#endif

IMPLEMENT_DYNAMIC(CMenuBar, CToolBar)

BEGIN_MESSAGE_MAP(CMenuBar, CToolBar)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(UWM_SHOWPOPUPMENU, OnShowPopupMenu)
END_MESSAGE_MAP()

CMenuBar::CMenuBar()
	: m_hMenu(nullptr)
	, m_bAlwaysVisible(true)
	, m_bActive(false)
	, m_bMouseTracking(false)
	, m_nMDIButtonDown(-1)
	, m_nMDIButtonHot(-1)
	, m_hwndOldFocus(nullptr)
	, m_nCurrentMenuItemFlags(0)
	, m_nCurrentHotItem(-1)
	, m_hCurrentPopupMenu(nullptr)
	, m_bShowKeyboardCues(false)
{
	m_pThis = this;
}

static inline bool IsHighContrastEnabled()
{
	HIGHCONTRAST hc = { sizeof(HIGHCONTRAST) };
	SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0);
	return (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
}

static TBBUTTON makeTBButton(int id, const TCHAR* str)
{
	TBBUTTON btn{ I_IMAGENONE, id, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_DROPDOWN | BTNS_AUTOSIZE };
	btn.iString = reinterpret_cast<intptr_t>(str);
	return btn;
}

BOOL CMenuBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	bool ret = __super::CreateEx(pParentWnd, TBSTYLE_FLAT | TBSTYLE_LIST | TBSTYLE_TRANSPARENT, 
		dwStyle, CRect(0, 0, 0, 0), nID);

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
	SendMessage(WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEACCEL|DT_HIDEPREFIX), 0);
	toolbar.InsertButton(0, &btn);
	toolbar.SetButtonSize(CSize(tm.tmHeight + cy * 2, tm.tmHeight + cy * 2));

	return ret;
}

bool CMenuBar::AttachMenu(CMenu* pMenu)
{
	if (!m_hWnd)
		return false;

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

void CMenuBar::DrawMDIButtons(HDC hDC)
{
	int nTypes[3] = { DFCS_CAPTIONMIN | DFCS_FLAT, DFCS_CAPTIONRESTORE | DFCS_FLAT, DFCS_CAPTIONCLOSE | DFCS_FLAT };
	CRect rcButtons = GetMDIButtonsRect();
	const int bw = GetSystemMetrics(SM_CXSMICON);
	const int w = bw + GetSystemMetrics(SM_CXBORDER) * 2;
	const int h = rcButtons.Height();
	CRect rc{ rcButtons.left, rcButtons.top + (h - bw) / 2, rcButtons.left + bw, rcButtons.top + (h + bw) / 2};
	for (int i = 0; i < 3; ++i)
	{
		if (m_nMDIButtonDown == i)
			nTypes[i] |= DFCS_PUSHED;
		if (m_nMDIButtonHot == i)
			nTypes[i] |= DFCS_HOT;
		::DrawFrameControl(hDC, rc, DFC_CAPTION, nTypes[i]);
		rc.left += w;
		rc.right += w;
	}
}

int CMenuBar::GetMDIButtonIndexFromPoint(CPoint pt) const
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

CRect CMenuBar::GetMDIButtonsRect() const
{
	CRect rcClient;
	GetClientRect(&rcClient);
	const int w = GetSystemMetrics(SM_CXSMICON) + GetSystemMetrics(SM_CXBORDER) * 2;
	return { rcClient.right - w * 3, rcClient.top, rcClient.right, rcClient.bottom };
}

CRect CMenuBar::GetMDIButtonRect(int nItem) const
{
	CRect rcButtons = GetMDIButtonsRect();
	const int w = GetSystemMetrics(SM_CXSMICON) + GetSystemMetrics(SM_CXBORDER) * 2;
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

void CMenuBar::ShowKeyboardCues(bool show)
{
	m_bShowKeyboardCues = show;
	SendMessage(WM_UPDATEUISTATE, MAKEWPARAM(show ? UIS_CLEAR : UIS_SET, UISF_HIDEACCEL|UISF_HIDEFOCUS), 0);
}

void CMenuBar::LoseFocus()
{
	m_bActive = false;
	m_hwndOldFocus = nullptr;
	ShowKeyboardCues(false);
	GetToolBarCtrl().SetHotItem(-1);
	if (!m_bAlwaysVisible)
		SetTimer(MENUBAR_TIMER_ID, 200, nullptr);
}

void CMenuBar::Show(bool visible)
{
	static_cast<CFrameWnd*>(AfxGetMainWnd())->ShowControlBar(this, visible, 0);
}

void CMenuBar::OnSetFocus(CWnd* pOldWnd)
{
	m_bActive = true;
	m_hwndOldFocus = pOldWnd->m_hWnd;
	if (!m_bAlwaysVisible)
	{
		KillTimer(MENUBAR_TIMER_ID);
		Show(true);
	}
	__super::OnSetFocus(pOldWnd);
}

void CMenuBar::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == MENUBAR_TIMER_ID)
	{
		KillTimer(MENUBAR_TIMER_ID);
		if (!m_bAlwaysVisible)
			Show(false);
	}
}

void CMenuBar::OnKillFocus(CWnd* pNewWnd)
{
	LoseFocus();
	__super::OnKillFocus(pNewWnd);
}

void CMenuBar::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
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
		pNMCD->clrHighlightHotTrack = GetSysColor(IsHighContrastEnabled() ? COLOR_HIGHLIGHT : COLOR_3DFACE);
		pNMCD->clrText = GetSysColor(COLOR_BTNTEXT);
		*pResult = CDRF_DODEFAULT | TBCDRF_USECDCOLORS | TBCDRF_HILITEHOTTRACK;
		return;
	}
	else if (dwDrawState == CDDS_POSTPAINT &&
		(m_bMouseTracking && IsMDIChildMaximized()))
	{
		DrawMDIButtons(pNMCD->nmcd.hdc);
	}
	*pResult = 0;
}

void CMenuBar::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rcMDIButtons = GetMDIButtonsRect();
	InvalidateRect(&rcMDIButtons);
	m_nMDIButtonHot = rcMDIButtons.PtInRect(point) ? GetMDIButtonIndexFromPoint(point) : -1;
	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme = { sizeof TRACKMOUSEEVENT, TME_LEAVE, m_hWnd };
		TrackMouseEvent(&tme);
		m_bMouseTracking = true;
	}
	__super::OnMouseMove(nFlags, point);
}

void CMenuBar::OnMouseLeave()
{
	TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_CANCEL, m_hWnd };
	TrackMouseEvent(&tme);
	m_bMouseTracking = false;
	CRect rcButtons = GetMDIButtonsRect();
	InvalidateRect(&rcButtons);
	m_nMDIButtonDown = -1;
	m_nMDIButtonHot = -1;
	__super::OnMouseLeave();
}

void CMenuBar::OnLButtonDown(UINT nFlags, CPoint point)
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

void CMenuBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rcButtons = GetMDIButtonsRect();
	if (m_nMDIButtonDown != -1 && rcButtons.PtInRect(point))
	{
		CMDIFrameWnd* pMDIFrameWnd = DYNAMIC_DOWNCAST(CMDIFrameWnd, AfxGetMainWnd());
		if (pMDIFrameWnd)
		{
			CFrameWnd* pFrameWnd = pMDIFrameWnd->GetActiveFrame();
			if (pFrameWnd)
			{
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
	}
	InvalidateRect(rcButtons);
	m_nMDIButtonDown = -1;
	__super::OnLButtonUp(nFlags, point);
}

void CMenuBar::OnMenuBarMenuItem(UINT nID)
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
	m_ptCurrentCursor = {};
	m_pThis = this;

	if (m_bShowKeyboardCues)
	{
		AfxGetMainWnd()->PostMessage(WM_KEYDOWN, VK_DOWN, 0);
		AfxGetMainWnd()->PostMessage(WM_KEYUP, VK_DOWN, 0);
	}

	if (!m_bAlwaysVisible)
	{
		KillTimer(MENUBAR_TIMER_ID);
		Show(true);
	}

	m_hHook = SetWindowsHookEx(WH_MSGFILTER, MsgFilterProc, nullptr, GetCurrentThreadId());

	CRect rc;
	toolbar.GetItemRect(nHotItem, &rc);
	ClientToScreen(&rc);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rc.left, rc.bottom, AfxGetMainWnd());

	UnhookWindowsHookEx(m_hHook);
	m_hHook = nullptr;

	if (!m_bAlwaysVisible)
		SetTimer(MENUBAR_TIMER_ID, 200, nullptr);
}

void CMenuBar::OnUpdateMenuBarMenuItem(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

LRESULT CMenuBar::OnShowPopupMenu(WPARAM wParam, LPARAM lParam)
{
	OnMenuBarMenuItem(static_cast<UINT>(wParam));
	return 0;
}

BOOL CMenuBar::PreTranslateMessage(MSG* pMsg)
{
	if (!m_hWnd)
		return FALSE;
	if (pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP)
	{
		const BOOL bShift = ::GetAsyncKeyState(VK_SHIFT) & 0x8000;
		if (!bShift && (pMsg->wParam == VK_F10 || pMsg->wParam == VK_MENU))
		{
			if (pMsg->message == WM_SYSKEYDOWN)
			{
				ShowKeyboardCues(true);
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
		const TCHAR key = static_cast<TCHAR>(pMsg->wParam);
		const bool alnum = (key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z');
		if ((pMsg->message == WM_SYSKEYDOWN) && alnum && GetToolBarCtrl().MapAccelerator(key, &uId) != 0)
		{
			ShowKeyboardCues(true);
			OnMenuBarMenuItem(uId);
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
			OnMenuBarMenuItem(FIRST_MENUID + GetToolBarCtrl().GetHotItem());
		}
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK CMenuBar::MsgFilterProc(int nCode, WPARAM wParam, LPARAM lParam)
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
		CPoint ptPrev = m_pThis->m_ptCurrentCursor;
		m_pThis->m_ptCurrentCursor = { GET_X_LPARAM(pMsg->lParam) - rc.left, GET_Y_LPARAM(pMsg->lParam) - rc.top };
		if (ptPrev == CPoint{} || ptPrev == m_pThis->m_ptCurrentCursor)
			return 0;
		const int nItem = m_pThis->GetToolBarCtrl().HitTest(&m_pThis->m_ptCurrentCursor);
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
