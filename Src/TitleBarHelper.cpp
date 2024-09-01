// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/**
 * @file  TitleBarHelper.cpp
 *
 * @brief Implementation of the CTitleBarHelper class
 */

#include "StdAfx.h"
#include "TitleBarHelper.h"

void CTitleBarHelper::DrawIcon(CDC& dc)
{
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

void CTitleBarHelper::Update(CWnd* pWnd, UINT nType, int cx, int cy)
{
	m_size = CSize(cx, cy);
	m_nType = nType;
	m_pWnd = pWnd;
}

int CTitleBarHelper::HitTest(CPoint pt)
{
	if (!m_pWnd)
		return HTNOWHERE;
	CClientDC dc(m_pWnd);
	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	const int height = pointToPixel(24);
	const int buttonWidth = pointToPixel(24);
	CRect rc;
	m_pWnd->GetWindowRect(&rc);
	if (pt.y < rc.top + height)
	{
		const int bw = buttonWidth;
		const int m = 4;
		if (pt.y < rc.top + 4)
		{
			if (pt.x < rc.left + m)
				return HTTOPLEFT;
			else if (rc.right - m <= pt.x)
				return HTTOPRIGHT;
			return HTTOP;
		}
		if (rc.bottom - m <= pt.y)
		{
			if (pt.x < rc.left + m)
				return HTBOTTOMLEFT;
			else if (rc.right - m <= pt.x)
				return HTBOTTOMRIGHT;
			return HTBOTTOM;
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
	return HTCLIENT;
}
