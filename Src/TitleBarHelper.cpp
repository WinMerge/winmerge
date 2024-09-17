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

void CTitleBarHelper::DrawIcon(CWnd* pWnd, CDC& dc, int leftMarginWidth)
{
	HICON hIcon = (HICON)pWnd->SendMessage(WM_GETICON, ICON_SMALL2, 0);
	if (hIcon == nullptr)
		hIcon = (HICON)GetClassLongPtr(pWnd->m_hWnd, GCLP_HICONSM);
	if (hIcon != nullptr)
	{
		const int my = (m_maximized ? 8 : 0);
		const int height = m_size.cy - my;
		const int cx = PointToPixel(12.f);
		const int cy = PointToPixel(12.f);
		const int x = (leftMarginWidth - cx) / 2;
		const int y = (height - cy) / 2 + my;
		DrawIconEx(dc.m_hDC, x, y, hIcon, 
			cx, cy, 0, nullptr, DI_NORMAL);
	}
}

void CTitleBarHelper::DrawButtons(CDC& dc)
{
}

CRect CTitleBarHelper::GetIconRect()
{
	return CRect{};
}

CRect CTitleBarHelper::GetButtonsRect()
{
	return CRect{};
}

void CTitleBarHelper::OnSize(CWnd* pWnd, bool maximized, int cx, int cy)
{
	m_size = CSize(cx, cy);
	m_maximized = maximized;
	m_pWnd = pWnd;
	CClientDC dc(pWnd);
	m_dpi = dc.GetDeviceCaps(LOGPIXELSX);
}

int CTitleBarHelper::HitTest(CPoint pt)
{
	if (!m_pWnd)
		return HTNOWHERE;
	CClientDC dc(m_pWnd);
	const int height = PointToPixel(24);
	const int buttonWidth = PointToPixel(24);
	CRect rc;
	const int bw = buttonWidth;
	const int m = 8;
	m_pWnd->GetWindowRect(&rc);
	if (pt.y < rc.top + 4)
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
	if (pt.x < rc.left + bw)
		return HTSYSMENU;
	if (rc.right - bw * 3 <= pt.x && pt.x < rc.right - bw * 2)
	{
		return HTMINBUTTON;
	}
	else if (rc.right - bw * 2 <= pt.x && pt.x < rc.right - bw)
	{
		return HTMAXBUTTON;
	}
	else if (rc.right - bw <= pt.x && pt.x < rc.right)
	{
		return HTCLOSE;
	}
	return HTCAPTION;
}

int CTitleBarHelper::PointToPixel(float point)
{
	return static_cast<int>(point * m_dpi / 72.f);
};
