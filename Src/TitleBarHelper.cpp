// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/**
 * @file  TitleBarHelper.cpp
 *
 * @brief Implementation of the CTitleBarHelper class
 */

#include "StdAfx.h"
#include "TitleBarHelper.h"

CTitleBarHelper::CTitleBarHelper()
	: m_pWnd(nullptr)
	, m_maximized(false)
	, m_dpi(96)
	, m_leftMargin(32.f)
	, m_rightMargin(35.f * 3)
	, m_bMouseTracking(false)
	, m_nTrackingButton(-1)
	, m_nHitTest(HTNOWHERE)
{
}

void CTitleBarHelper::Init(CWnd *pWnd)
{
	m_pWnd = pWnd;
}

void CTitleBarHelper::DrawIcon(CWnd* pWnd, CDC& dc)
{
	HICON hIcon = (HICON)pWnd->SendMessage(WM_GETICON, ICON_SMALL2, 0);
	if (hIcon == nullptr)
		hIcon = (HICON)GetClassLongPtr(pWnd->m_hWnd, GCLP_HICONSM);
	if (hIcon == nullptr)
		return;
	const int topMargin = (m_maximized ? -m_rc.top : 0);
	const int height = m_size.cy - topMargin;
	const int cx = PointToPixel(12.f);
	const int cy = PointToPixel(12.f);
	const int x = (PointToPixel(m_leftMargin) - cx) / 2;
	const int y = (height - cy) / 2 + topMargin;
	DrawIconEx(dc.m_hDC, x, y, hIcon, 
		cx, cy, 0, nullptr, DI_NORMAL);
}

static void DrawTopRightEdgeWithCurve(Gdiplus::Graphics& graphics, Gdiplus::Pen& pen, Gdiplus::Rect rect, int cornerRadius)
{
	Gdiplus::GraphicsPath path;
	path.AddLine(rect.X, rect.Y, rect.X + rect.Width - cornerRadius, rect.Y);
	path.AddArc(rect.X + rect.Width - cornerRadius, rect.Y, cornerRadius, cornerRadius, 270, 90);
	path.AddLine(rect.X + rect.Width, rect.Y + cornerRadius, rect.X + rect.Width, rect.Y + rect.Height);
	graphics.DrawPath(&pen, &path);
}

static void DrawRoundedRectangle(Gdiplus::Graphics& graphics, Gdiplus::Pen& pen, Gdiplus::Rect rect, int cornerRadius)
{
	Gdiplus::GraphicsPath path;
	path.AddArc(rect.X, rect.Y, cornerRadius, cornerRadius, 180, 90);
	path.AddArc(rect.X + rect.Width - cornerRadius, rect.Y, cornerRadius, cornerRadius, 270, 90);
	path.AddArc(rect.X + rect.Width - cornerRadius, rect.Y + rect.Height - cornerRadius, cornerRadius, cornerRadius, 0, 90);
	path.AddArc(rect.X, rect.Y + rect.Height - cornerRadius, cornerRadius, cornerRadius, 90, 90);
	path.CloseFigure();
	graphics.DrawPath(&pen, &path);
}

void CTitleBarHelper::DrawButtons(CDC& dc, COLORREF textColor, COLORREF backColor)
{
	Gdiplus::Graphics graphics(dc.m_hDC);
	CRect rcIcons[3], rcButtons[3];
	const float buttonWidth = PointToPixelF(m_rightMargin) / 3.f;
	const int iconSize = PointToPixel(6.75);
	for (int i = 0; i < 3; i++)
	{
		rcButtons[i] = GetButtonRect(i);
		rcIcons[i] = rcButtons[i];
		rcIcons[i].left = static_cast<int>(rcIcons[i].left + (buttonWidth - iconSize) / 2);
		rcIcons[i].right = static_cast<int>(rcIcons[i].left + iconSize);
		rcIcons[i].top = static_cast<int>(rcIcons[i].top + (rcButtons[i].Height() - iconSize) / 2);
		rcIcons[i].bottom = static_cast<int>(rcIcons[i].top + iconSize);

		COLORREF colorref;
		Gdiplus::Color color;
		if (m_nTrackingButton == i)
			colorref = (i == 2) ? RGB(0xE9, 0x48, 0x56) : GetIntermediateColor(backColor, GetSysColor(COLOR_WINDOW), 0.66f);
		else
			colorref = backColor;
		color.SetFromCOLORREF(colorref);
		Gdiplus::SolidBrush brush(color);
		graphics.FillRectangle(&brush, rcButtons[i].left, rcButtons[i].top, rcButtons[i].Width(), rcButtons[i].Height());
	}
	
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	Gdiplus::Color penColor;
	penColor.SetFromCOLORREF(textColor);
	Gdiplus::Pen pen(penColor, PointToPixelF(0.75));

	// minimize button
	const int y = (rcIcons[0].top + rcIcons[0].bottom) / 2;
	graphics.DrawLine(&pen, Gdiplus::Point(rcIcons[0].left, y), Gdiplus::Point(rcIcons[0].right, y));

	const int r = PointToPixel(0.75);
	if (m_maximized)
	{
		// maxmize button
		DrawTopRightEdgeWithCurve(graphics, pen, Gdiplus::Rect(rcIcons[1].left + r, rcIcons[1].top - r, rcIcons[1].Width() - 2 * r, rcIcons[1].Height() - 2 * r), r * 3);
		DrawRoundedRectangle(graphics, pen, Gdiplus::Rect(rcIcons[1].left - r, rcIcons[1].top + r, rcIcons[1].Width() - 2 * r, rcIcons[1].Height() - 2 * r), r);
	}
	else
	{
		// restore button
		DrawRoundedRectangle(graphics, pen, Gdiplus::Rect(rcIcons[1].left, rcIcons[1].top, rcIcons[1].Width(), rcIcons[1].Height()), r);
	}

	// close button
	penColor.SetFromCOLORREF(m_nTrackingButton != 2 ? textColor : RGB(255, 255, 255));
	Gdiplus::Pen pen2(penColor, PointToPixelF(0.75));
	graphics.DrawLine(&pen2, Gdiplus::Point(rcIcons[2].left, rcIcons[2].top), Gdiplus::Point(rcIcons[2].right, rcIcons[2].bottom));
	graphics.DrawLine(&pen2, Gdiplus::Point(rcIcons[2].left, rcIcons[2].bottom), Gdiplus::Point(rcIcons[2].right, rcIcons[2].top));
}

CRect CTitleBarHelper::GetButtonRect(int button) const
{
	CRect rcPart;
	const float buttonWidth = PointToPixelF(m_rightMargin) / 3.f;
	const int topMargin = (m_maximized ? -m_rc.top : 0);
	rcPart.top = topMargin;
	rcPart.bottom = m_size.cy;
	rcPart.left = static_cast<int>(m_size.cx - (3 - button) * buttonWidth);
	rcPart.right = static_cast<int>(rcPart.left + buttonWidth);
	return rcPart;
}

void CTitleBarHelper::OnSize(bool maximized, int cx, int cy)
{
	m_size = CSize(cx, cy);
	m_maximized = maximized;
	CClientDC dc(m_pWnd);
	m_dpi = dc.GetDeviceCaps(LOGPIXELSX);
	m_pWnd->GetWindowRect(&m_rc);
}

LRESULT CTitleBarHelper::OnNcHitTest(CPoint pt)
{
	if (!m_pWnd)
		return HTNOWHERE;
	CClientDC dc(m_pWnd);
	const int leftMargin = PointToPixel(m_leftMargin);
	const int rightMargin = PointToPixel(m_rightMargin);
	const int borderWidth = PointToPixel(6);
	CRect rc;
	m_pWnd->GetWindowRect(&rc);
	if (pt.y < rc.top + borderWidth)
	{
		if (pt.x < rc.left + borderWidth)
			return HTTOPLEFT;
		else if (rc.right - borderWidth <= pt.x)
			return HTTOPRIGHT;
		return HTTOP;
	}
	if (pt.x < rc.left + borderWidth)
		return HTLEFT;
	if (rc.right - borderWidth <= pt.x)
		return HTRIGHT;
	if (pt.x < rc.left + leftMargin)
		return HTSYSMENU;
	for (int i = 0; i < 3; i++)
	{
		static const int htbuttons[]{ HTMINBUTTON, HTMAXBUTTON, HTCLOSE };
		CRect rcButton = GetButtonRect(i);
		m_pWnd->ClientToScreen(&rcButton);
		if (PtInRect(&rcButton, pt))
			return htbuttons[i];
	}
	return HTCAPTION;
}

void CTitleBarHelper::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme = { sizeof TRACKMOUSEEVENT, TME_LEAVE | TME_NONCLIENT, m_pWnd->m_hWnd };
		TrackMouseEvent(&tme);
		m_bMouseTracking = true;
	}
	int i = -1;
	if (nHitTest == HTMINBUTTON)
		i = 0;
	else if (nHitTest == HTMAXBUTTON)
		i = 1;
	else if (nHitTest == HTCLOSE)
		i = 2;
	for (int button : {i, m_nTrackingButton})
	{
		if (button != -1)
		{
			CRect rcPart = GetButtonRect(button);
			m_pWnd->InvalidateRect(&rcPart, false);
		}
	}
	m_nTrackingButton = i;
}

void CTitleBarHelper::OnNcMouseLeave()
{
	TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_CANCEL, m_pWnd->m_hWnd };
	TrackMouseEvent(&tme);
	m_bMouseTracking = false;
	if (m_nTrackingButton >= 0)
	{
		CRect rcPart = GetButtonRect(m_nTrackingButton);
		m_pWnd->InvalidateRect(&rcPart, false);
	}
	m_nTrackingButton = -1;
}

void CTitleBarHelper::OnNcLButtonDblClk(UINT nHitTest, CPoint point)
{
	if (nHitTest != HTMINBUTTON && nHitTest != HTMAXBUTTON && nHitTest != HTCLOSE && nHitTest != HTSYSMENU)
		AfxGetMainWnd()->SendMessage(WM_NCLBUTTONDBLCLK, nHitTest, MAKELPARAM(point.x, point.y));
	else if (nHitTest == HTSYSMENU)
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
}

void CTitleBarHelper::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if (nHitTest != HTMINBUTTON && nHitTest != HTMAXBUTTON && nHitTest != HTCLOSE && nHitTest != HTSYSMENU)
		AfxGetMainWnd()->SendMessage(WM_NCLBUTTONDOWN, nHitTest, MAKELPARAM(point.x, point.y));
	else if (nHitTest == HTSYSMENU)
		ShowSysMenu(CPoint{ point.x + 1, point.y });
	else if (nHitTest == HTMINBUTTON || nHitTest == HTMAXBUTTON || nHitTest == HTCLOSE)
		m_nHitTest = nHitTest;
}

void CTitleBarHelper::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	if (nHitTest != HTMINBUTTON && nHitTest != HTMAXBUTTON && nHitTest != HTCLOSE && nHitTest != HTSYSMENU)
		AfxGetMainWnd()->SendMessage(WM_NCLBUTTONUP, nHitTest, MAKELPARAM(point.x, point.y));
	else if (m_nHitTest != HTNOWHERE && m_nHitTest == nHitTest)
	{
		if (nHitTest == HTMINBUTTON)
			AfxGetMainWnd()->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		else if (nHitTest == HTMAXBUTTON)
			AfxGetMainWnd()->SendMessage(WM_SYSCOMMAND, m_maximized ? SC_RESTORE : SC_MAXIMIZE, 0);
		else if (nHitTest == HTCLOSE)
			AfxGetMainWnd()->SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
	}
	m_nHitTest = HTNOWHERE;
}

void CTitleBarHelper::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
}

void CTitleBarHelper::OnNcRButtonUp(UINT nHitTest, CPoint point)
{
	ShowSysMenu(point);
}

void CTitleBarHelper::ShowSysMenu(CPoint point)
{
	CMenu* pSysMenu = AfxGetMainWnd()->GetSystemMenu(FALSE);
	bool maximized = AfxGetMainWnd()->IsZoomed();
	pSysMenu->EnableMenuItem(SC_MAXIMIZE,(!maximized ? MF_ENABLED : MF_DISABLED) | MF_BYCOMMAND);
	pSysMenu->EnableMenuItem(SC_SIZE,(!maximized ? MF_ENABLED : MF_DISABLED) | MF_BYCOMMAND);
	pSysMenu->EnableMenuItem(SC_RESTORE, (maximized ? MF_ENABLED : MF_DISABLED) | MF_BYCOMMAND);
	BOOL cmd = pSysMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, AfxGetMainWnd(), nullptr);
	if (cmd)
		AfxGetMainWnd()->PostMessage(WM_SYSCOMMAND, cmd, 0);
}

COLORREF CTitleBarHelper::GetIntermediateColor(COLORREF a, COLORREF b, float ratio)
{
	const uint8_t R = static_cast<int8_t>((GetRValue(a) - GetRValue(b)) * ratio) + GetRValue(b);
	const uint8_t G = static_cast<int8_t>((GetGValue(a) - GetGValue(b)) * ratio) + GetGValue(b);
	const uint8_t B = static_cast<int8_t>((GetBValue(a) - GetBValue(b)) * ratio) + GetBValue(b);
	return RGB(R, G, B);
}
