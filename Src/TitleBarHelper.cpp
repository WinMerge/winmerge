// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/**
 * @file  TitleBarHelper.cpp
 *
 * @brief Implementation of the CTitleBarHelper class
 */

#include "StdAfx.h"
#include "TitleBarHelper.h"
#include "RoundedRectWithShadow.h"

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
	if (hIcon != nullptr)
	{
		const int topMargin = (m_maximized ? -m_rc.top : 0);
		const int height = m_size.cy - topMargin;
		const int cx = static_cast<int>(PointToPixel(12.f));
		const int cy = static_cast<int>(PointToPixel(12.f));
		const int x = static_cast<int>((PointToPixel(m_leftMargin) - cx) / 2);
		const int y = static_cast<int>((height - cy) / 2 + topMargin);
		DrawIconEx(dc.m_hDC, x, y, hIcon, 
			cx, cy, 0, nullptr, DI_NORMAL);
	}
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

void CTitleBarHelper::DrawButtons(CDC& dc)
{
	Gdiplus::Graphics graphics(dc.m_hDC);
	CRect rcIcons[3], rcButtons[3];
	const float buttonWidth = PointToPixel(m_rightMargin) / 3.f;
	const float iconSize = PointToPixel(6.75);
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
			colorref = (i == 2) ? RGB(0xE9, 0x48, 0x56) : GetSysColor(COLOR_WINDOW);
		else
			colorref = GetSysColor(COLOR_3DFACE);
		color.SetFromCOLORREF(colorref);
		Gdiplus::SolidBrush brush(color);
		graphics.FillRectangle(&brush, rcButtons[i].left, rcButtons[i].top, rcButtons[i].Width(), rcButtons[i].Height());
	}
	
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	Gdiplus::Color penColor;
	penColor.SetFromCOLORREF(GetSysColor(COLOR_WINDOWTEXT));
	Gdiplus::Pen pen(penColor, PointToPixel(0.75));
	graphics.DrawLine(&pen, Gdiplus::Point(rcIcons[2].left, rcIcons[2].top), Gdiplus::Point(rcIcons[2].right, rcIcons[2].bottom));
	graphics.DrawLine(&pen, Gdiplus::Point(rcIcons[2].left, rcIcons[2].bottom), Gdiplus::Point(rcIcons[2].right, rcIcons[2].top));

	const int r = static_cast<int>(PointToPixel(0.75));
	if (m_maximized)
	{
		DrawTopRightEdgeWithCurve(graphics, pen, Gdiplus::Rect(rcIcons[1].left + r, rcIcons[1].top - r, rcIcons[1].Width() - 2 * r, rcIcons[1].Height() - 2 * r), r * 3);
		DrawRoundedRectangle(graphics, pen, Gdiplus::Rect(rcIcons[1].left - r, rcIcons[1].top + r, rcIcons[1].Width() - 2 * r, rcIcons[1].Height() - 2 * r), r);
	}
	else
	{
		DrawRoundedRectangle(graphics, pen, Gdiplus::Rect(rcIcons[1].left, rcIcons[1].top, rcIcons[1].Width(), rcIcons[1].Height()), r);
	}

	const int y = (rcIcons[0].top + rcIcons[0].bottom) / 2;
	graphics.DrawLine(&pen, Gdiplus::Point(rcIcons[0].left, y), Gdiplus::Point(rcIcons[0].right, y));
}

CRect CTitleBarHelper::GetButtonRect(int button) const
{
	const float w1 = PointToPixel(m_rightMargin) / 3.f;
	CRect rcPart;
	const int topMargin = (m_maximized ? -m_rc.top : 0);
	rcPart.top = topMargin;
	rcPart.bottom = m_size.cy;
	rcPart.left = static_cast<int>(m_size.cx - (3 - button) * w1);
	rcPart.right = static_cast<int>(rcPart.left + w1);
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

void CTitleBarHelper::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme = { sizeof TRACKMOUSEEVENT, TME_LEAVE | TME_NONCLIENT, m_pWnd->m_hWnd };
		TrackMouseEvent(&tme);
		m_bMouseTracking = true;
	}
	int i = HitTest(point);
	if (i == HTMINBUTTON)
		i = 0;
	else if (i == HTMAXBUTTON)
		i = 1;
	else if (i == HTCLOSE)
		i = 2;
	else
		i = -1;
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
	if (nHitTest != HTMINBUTTON && nHitTest != HTMAXBUTTON && nHitTest != HTCLOSE)
		AfxGetMainWnd()->SendMessage(WM_NCLBUTTONDBLCLK, nHitTest, MAKELPARAM(point.x, point.y));
}

void CTitleBarHelper::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if (nHitTest != HTMINBUTTON && nHitTest != HTMAXBUTTON && nHitTest != HTCLOSE)
	{
		AfxGetMainWnd()->SendMessage(WM_NCLBUTTONDOWN, nHitTest, MAKELPARAM(point.x, point.y));
	}
	else if (nHitTest == HTMINBUTTON || nHitTest == HTMAXBUTTON || nHitTest == HTCLOSE)
	{
		m_nHitTest = nHitTest;
	}
}

void CTitleBarHelper::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	if (nHitTest != HTMINBUTTON && nHitTest != HTMAXBUTTON && nHitTest != HTCLOSE)
	{
		AfxGetMainWnd()->SendMessage(WM_NCLBUTTONUP, nHitTest, MAKELPARAM(point.x, point.y));
	}
	else if (m_nHitTest != HTNOWHERE && m_nHitTest == nHitTest)
	{
		if (nHitTest == HTMINBUTTON)
			AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
		else if (nHitTest == HTMAXBUTTON)
			AfxGetMainWnd()->ShowWindow(m_maximized ? SW_RESTORE : SW_MAXIMIZE);
		else if (nHitTest == HTCLOSE)
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
	}
	m_nHitTest = HTNOWHERE;
}

void CTitleBarHelper::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	AfxGetMainWnd()->SendMessage(WM_NCRBUTTONDOWN, nHitTest, MAKELPARAM(point.x, point.y));
}

void CTitleBarHelper::OnNcRButtonUp(UINT nHitTest, CPoint point)
{
	CMenu *pSysMenu = AfxGetMainWnd()->GetSystemMenu(FALSE);
	pSysMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON, point.x, point.y, m_pWnd, nullptr);
}

int CTitleBarHelper::HitTest(CPoint pt)
{
	if (!m_pWnd)
		return HTNOWHERE;
	CClientDC dc(m_pWnd);
	const int leftMargin = static_cast<int>(PointToPixel(m_leftMargin));
	const int rightMargin = static_cast<int>(PointToPixel(m_rightMargin));
	const int m = 8;
	CRect rc;
	m_pWnd->GetWindowRect(&rc);
	if (pt.y < rc.top + m)
	{
		if (pt.x < rc.left + m)
			return HTTOPLEFT;
		else if (rc.right - m <= pt.x)
			return HTTOPRIGHT;
		return HTTOP;
	}
	if (pt.x < rc.left + m)
		return HTLEFT;
	if (rc.right - m <= pt.x)
		return HTRIGHT;
	if (pt.x < rc.left + leftMargin)
		return HTSYSMENU;
	CRect rcButton = GetButtonRect(0);
	m_pWnd->ClientToScreen(&rcButton);
	if (PtInRect(&rcButton, pt))
		return HTMINBUTTON;
	rcButton = GetButtonRect(1);
	m_pWnd->ClientToScreen(&rcButton);
	if (PtInRect(&rcButton, pt))
		return HTMAXBUTTON;
	rcButton = GetButtonRect(2);
	m_pWnd->ClientToScreen(&rcButton);
	if (PtInRect(&rcButton, pt))
		return HTCLOSE;
	return HTCAPTION;
}

float CTitleBarHelper::PointToPixel(float point) const
{
	return point * m_dpi / 72.f;
}
