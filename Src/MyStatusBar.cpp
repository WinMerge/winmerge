/**
 * @file  MyStatusBar.cpp
 *
 * @brief Implementation of the CMyStatusBar class
 */

#include "StdAfx.h"
#include "MyStatusBar.h"

BEGIN_MESSAGE_MAP(CMyStatusBar, CStatusBar)
    ON_WM_PAINT()
END_MESSAGE_MAP()

CMyStatusBar::CMyStatusBar()
{
}

void CMyStatusBar::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);
    CStatusBarCtrl& ctrl = GetStatusBarCtrl();
    int parts[32];
    const int nParts = ctrl.GetParts(32, parts);
    dc.FillSolidRect(&rect, GetSysColor(COLOR_3DFACE));
    dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
    dc.SetBkMode(TRANSPARENT);
    CFont* pFont = GetFont();
    CFont* pOldFont = pFont ? dc.SelectObject(pFont) : nullptr;
    for (int i = 0; i < nParts; i++)
    {
	    const bool disabled = (GetPaneStyle(i) & SBPS_DISABLED) != 0;
        if (!disabled)
        {
            CRect partRect;
            CString text;
            ctrl.GetRect(i, &partRect);
            GetPaneText(i, text);
            dc.DrawText(text, &partRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
    }
    if (pOldFont)
        dc.SelectObject(pOldFont);
}