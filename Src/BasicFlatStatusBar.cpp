// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/**
 * @file  BasicFlatStatusBar.cpp
 *
 * @brief Implementation of the CBasicFlatStatusBar class
 */

#include "StdAfx.h"
#include "BasicFlatStatusBar.h"
#include "RoundedRectWithShadow.h"

IMPLEMENT_DYNAMIC(CBasicFlatStatusBar, CStatusBar)

BEGIN_MESSAGE_MAP(CBasicFlatStatusBar, CStatusBar)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

CBasicFlatStatusBar::CBasicFlatStatusBar() : m_bMouseTracking(false), m_nTrackingPane(-1)
{
}

CPoint CBasicFlatStatusBar::GetClientCursorPos() const
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	return pt;
}

int CBasicFlatStatusBar::GetIndexFromPoint(const CPoint& pt) const
{
	CStatusBarCtrl& ctrl = GetStatusBarCtrl();
	int parts[32];
	const int nParts = ctrl.GetParts(32, parts);
	for (int i = 0; i < nParts; i++)
	{
		CRect rcPart;
		ctrl.GetRect(i, &rcPart);
		if (PtInRect(rcPart, pt))
			return i;
	}
	return -1;
}

COLORREF CBasicFlatStatusBar::LightenColor(COLORREF color, double amount)
{
	BYTE red = GetRValue(color);
	BYTE green = GetGValue(color);
	BYTE blue = GetBValue(color);
	red = static_cast<BYTE>(red + (255 - red) * amount);
	green = static_cast<BYTE>(green + (255 - green) * amount);
	blue = static_cast<BYTE>(blue + (255 - blue) * amount);
	return RGB(red, green, blue);
}

void CBasicFlatStatusBar::OnPaint()
{
	const COLORREF clr3DFace = GetSysColor(COLOR_3DFACE);
	const COLORREF clr3DFaceLight = LightenColor(clr3DFace, 0.5);

	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);

	CStatusBarCtrl& ctrl = GetStatusBarCtrl();
	int parts[32];
	const int nParts = ctrl.GetParts(32, parts);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&bmp);

	memDC.FillSolidRect(&rect, clr3DFace);
	memDC.SetTextColor(GetSysColor(COLOR_BTNTEXT));
	memDC.SetBkMode(TRANSPARENT);

	CFont* pFont = GetFont();
	CFont* pOldFont = pFont ? memDC.SelectObject(pFont) : nullptr;
	const int radius = MulDiv(3, memDC.GetDeviceCaps(LOGPIXELSY), 72);

	for (int i = 0; i < nParts; i++)
	{
		const unsigned style = GetPaneStyle(i);
		CRect rcPart;
		ctrl.GetRect(i, &rcPart);

		if (m_bMouseTracking && (style & SBPS_CLICKABLE) != 0 && i == m_nTrackingPane)
			DrawRoundedRect(memDC.m_hDC, rcPart.left, rcPart.top, rcPart.Width(), rcPart.Height(), radius, clr3DFaceLight, clr3DFace);

		const bool disabled = (style & SBPS_DISABLED) != 0;
		if (!disabled)
		{
			CRect rcText = rcPart;
			rcText.left += radius;
			CString text = ctrl.GetText(i);
			if (text.Find('\t') >= 0)
			{
				text.Trim();
				text.Replace(_T("\t"), _T("  "));
			}
			memDC.DrawText(text, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
	}

	if (pOldFont)
		memDC.SelectObject(pOldFont);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBmp);
}

BOOL CBasicFlatStatusBar::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, ::GetSysColor(COLOR_BTNFACE));
	return TRUE;
}

void CBasicFlatStatusBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme = { sizeof TRACKMOUSEEVENT, TME_LEAVE, m_hWnd };
		TrackMouseEvent(&tme);
		m_bMouseTracking = true;
	}
	int i = GetIndexFromPoint(GetClientCursorPos());
	for (int pane : {i, m_nTrackingPane})
	{
		if (pane >= 0 && (GetPaneStyle(pane) & SBPS_CLICKABLE) != 0)
		{
			CRect rcPart;
			GetStatusBarCtrl().GetRect(pane, &rcPart);
			InvalidateRect(&rcPart, false);
		}
	}
	m_nTrackingPane = i;
}

void CBasicFlatStatusBar::OnMouseLeave()
{
	TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_CANCEL, m_hWnd };
	TrackMouseEvent(&tme);
	m_bMouseTracking = false;
	if (m_nTrackingPane >= 0 && (GetPaneStyle(m_nTrackingPane) & SBPS_CLICKABLE) != 0)
	{
		CRect rcPart;
		GetStatusBarCtrl().GetRect(m_nTrackingPane, &rcPart);
		InvalidateRect(&rcPart, false);
	}
	m_nTrackingPane = -1;
}

BOOL CBasicFlatStatusBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	const int i = GetIndexFromPoint(GetClientCursorPos());
	LPCTSTR icon = (i >= 0 && (GetPaneStyle(i) & SBPS_CLICKABLE) != 0) ? IDC_HAND : IDC_ARROW;
	::SetCursor(::LoadCursor(nullptr, icon));
	return TRUE;
}