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

HICON CBasicFlatStatusBar::GetPaneIcon(int nIndex) const
{
	return m_paneIcons.count(nIndex) ? m_paneIcons.at(nIndex) : nullptr;
}

void CBasicFlatStatusBar::SetPaneIcon(int nIndex, HICON hIcon)
{
	if (hIcon)
		m_paneIcons[nIndex] = hIcon;
	else
		m_paneIcons.erase(nIndex);
	Invalidate();
}

void CBasicFlatStatusBar::SetSubPaneButtons(int nIndex, std::vector<SubPaneButton> buttons)
{
	if (buttons.empty())
		m_subPaneButtons.erase(nIndex);
	else
		m_subPaneButtons[nIndex] = std::move(buttons);
}

void CBasicFlatStatusBar::GetSubPaneButtonRects(int nIndex, std::vector<CRect>& rects) const
{
	rects.clear();
	auto it = m_subPaneButtons.find(nIndex);
	if (it == m_subPaneButtons.end() || it->second.empty())
		return;

	CRect rcPart;
	GetStatusBarCtrl().GetRect(nIndex, &rcPart);

	const int n = static_cast<int>(it->second.size());
	const int w = rcPart.Width() / n;
	int x = rcPart.left;
	for (int i = 0; i < n; ++i)
	{
		int right = (i == n - 1) ? rcPart.right : x + w;
		rects.emplace_back(x, rcPart.top, right, rcPart.bottom);
		x = right;
	}
}

int CBasicFlatStatusBar::HitTestSubPaneButton(int nIndex, const CPoint& pt) const
{
	std::vector<CRect> rects;
	GetSubPaneButtonRects(nIndex, rects);
	for (size_t i = 0; i < rects.size(); ++i)
		if (rects[i].PtInRect(pt))
			return static_cast<int>(i);
	return -1;
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

		auto it = m_subPaneButtons.find(i);
		const bool bShowSubButtons = (i == m_nTrackingPane) && it != m_subPaneButtons.end() && !it->second.empty();

		if (bShowSubButtons)
		{
			std::vector<CRect> rects;
			GetSubPaneButtonRects(i, rects);
			for (size_t b = 0; b < rects.size(); ++b)
			{
				const bool bHot = (m_nHotSubButton == static_cast<int>(b));
				if (bHot)
					DrawRoundedRect(memDC.m_hDC, rects[b].left, rects[b].top,
						rects[b].Width(), rects[b].Height(), radius, clr3DFaceLight, clr3DFace);
				memDC.DrawText(it->second[b].text.c_str(), rects[b], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			continue;
		}

		if (m_bMouseTracking && (style & SBPS_CLICKABLE) != 0 && i == m_nTrackingPane)
			DrawRoundedRect(memDC.m_hDC, rcPart.left, rcPart.top, rcPart.Width(), rcPart.Height(), radius, clr3DFaceLight, clr3DFace);

		const bool disabled = (style & SBPS_DISABLED) != 0;
		if (!disabled)
		{
			CRect rcText = rcPart;
			rcText.left += radius;

			auto itIcon = m_paneIcons.find(i);
			if (itIcon != m_paneIcons.end() && itIcon->second)
			{
				const int iconSize = GetSystemMetrics(SM_CXSMICON);
				const int iconY = rcPart.top + (rcPart.Height() - iconSize) / 2;
				::DrawIconEx(memDC.m_hDC, rcText.left, iconY, itIcon->second,
					iconSize, iconSize, 0, nullptr, DI_NORMAL);
				rcText.left += iconSize + 4;
			}

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

	int nNewHotSubButton = -1;
	if (i >= 0 && m_subPaneButtons.count(i))
		nNewHotSubButton = HitTestSubPaneButton(i, point);

	for (int pane : {i, m_nTrackingPane})
	{
		if (pane >= 0 && ((GetPaneStyle(pane) & SBPS_CLICKABLE) != 0 || m_subPaneButtons.count(pane)))
		{
			CRect rcPart;
			GetStatusBarCtrl().GetRect(pane, &rcPart);
			InvalidateRect(&rcPart, false);
		}
	}
	m_nHotSubButton = nNewHotSubButton;
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