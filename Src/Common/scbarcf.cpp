/////////////////////////////////////////////////////////////////////////
//
// CSizingControlBarCF          Version 2.44
// 
// Created: Dec 21, 1998        Last Modified: March 31, 2002
//
// See the official site at www.datamekanix.com for documentation and
// the latest news.
//
/////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998-2002 by Cristi Posea. All rights reserved.
//
// This code is free for personal and commercial use, providing this 
// notice remains intact in the source files and all eventual changes are
// clearly marked with comments.
//
// You must obtain the author's consent before you can include this code
// in a software library.
//
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc. to
// cristi@datamekanix.com or post them at the message board at the site.
/////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "scbarcf.h"
#include "RoundedRectWithShadow.h"

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarCF

IMPLEMENT_DYNAMIC(CSizingControlBarCF, baseCSizingControlBarCF);

int CALLBACK EnumFontFamProc(ENUMLOGFONT* lpelf,
                             NEWTEXTMETRIC* lpntm,
                             int FontType,
                             LPARAM lParam)
{
    UNUSED_ALWAYS(lpelf);
    UNUSED_ALWAYS(lpntm);
    UNUSED_ALWAYS(FontType);
    UNUSED_ALWAYS(lParam);

    return 0;
}
 
CSizingControlBarCF::CSizingControlBarCF()
{
    m_bActive = false;

    CDC dc;
    dc.CreateCompatibleDC(nullptr);

    m_sFontFace = (::EnumFontFamilies(dc.m_hDC,
        _T("Tahoma"), (FONTENUMPROC) EnumFontFamProc, 0) == 0) ?
        _T("Tahoma") : _T("Arial");
}

BEGIN_MESSAGE_MAP(CSizingControlBarCF, baseCSizingControlBarCF)
    //{{AFX_MSG_MAP(CSizingControlBarCF)
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

void CSizingControlBarCF::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
    __super::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);

    if (!HasGripper())
        return;

    bool bNeedPaint = false;

    CWnd* pFocus = GetFocus();
    bool bActiveOld = m_bActive;

    m_bActive = (pFocus && pFocus->GetSafeHwnd() && IsChild(pFocus));

    if (m_bActive != bActiveOld)
        bNeedPaint = true;

    if (bNeedPaint)
        SendMessage(WM_NCPAINT);
}

void CSizingControlBarCF::NcPaintGripper(CDC* pDC, const CRect& rcClient)
{
    if (!HasGripper())
        return;

    // compute the caption rectangle
    bool bHorz = IsHorzDocked();
    CRect rcGrip = rcClient;
    const int lpx = pDC->GetDeviceCaps(LOGPIXELSX);
    auto pointToPixel = [lpx](double point) { return static_cast<int>(point * lpx / 72); };
    CRect rcBtn(m_biHide.ptOrg, CSize(pointToPixel(m_biHide.dblBoxSize), pointToPixel(m_biHide.dblBoxSize)));
    if (bHorz)
    {   // right side gripper
        rcGrip.left -= pointToPixel(m_dblGripper + 0.75);
        rcGrip.right = rcGrip.left + pointToPixel(8.25);
		rcGrip.top = rcBtn.bottom + pointToPixel(2.25);
    }
    else
    {   // gripper at top
        rcGrip.top -= pointToPixel(m_dblGripper + 0.75);
        rcGrip.bottom = rcGrip.top + pointToPixel(8.25);
        rcGrip.right = rcBtn.left - pointToPixel(2.25);
    }
    rcGrip.InflateRect(bHorz ? pointToPixel(0.75) : 0, bHorz ? 0 : pointToPixel(0.75));

    // draw the caption background
    //CBrush br;
    const COLORREF clrCptn = m_bActive ?
        ::GetSysColor(COLOR_GRADIENTACTIVECAPTION) :
        ::GetSysColor(COLOR_GRADIENTINACTIVECAPTION);
    const COLORREF clrBack = ::GetSysColor(COLOR_3DFACE);
    const int radius = pointToPixel(2.25);
    DrawRoundedRect(pDC->m_hDC, rcGrip.left, rcGrip.top, rcGrip.Width(), rcGrip.Height(), radius, clrCptn, clrBack);

    // draw the caption text - first select a font
    CFont font;
    LOGFONT lf;
    bool bFont = !!font.CreatePointFont(85/*8.5 points*/, m_sFontFace);
    if (bFont)
    {
        // get the text color
        COLORREF clrCptnText = m_bActive ?
            ::GetSysColor(COLOR_CAPTIONTEXT) :
            ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);

        int nOldBkMode = pDC->SetBkMode(TRANSPARENT);
        COLORREF clrOldText = pDC->SetTextColor(clrCptnText);

        if (bHorz)
        {
            // rotate text 90 degrees CCW if horizontally docked
            font.GetLogFont(&lf);
            font.DeleteObject();
            lf.lfEscapement = 900;
            font.CreateFontIndirect(&lf);
        }
        
        CFont* pOldFont = pDC->SelectObject(&font);
        CString sTitle;
        GetWindowText(sTitle);

        CPoint ptOrg = bHorz ?
            CPoint(rcGrip.left - pointToPixel(0.75), rcGrip.bottom - pointToPixel(2.25)) :
            CPoint(rcGrip.left + pointToPixel(2.25), rcGrip.top - pointToPixel(0.75));

        pDC->ExtTextOut(ptOrg.x, ptOrg.y,
            ETO_CLIPPED, rcGrip, sTitle, nullptr);

        pDC->SelectObject(pOldFont);
        pDC->SetBkMode(nOldBkMode);
        pDC->SetTextColor(clrOldText);
    }

    // draw the button
    m_biHide.Paint(pDC);
}

LRESULT CSizingControlBarCF::OnSetText(WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = __super::OnSetText(wParam, lParam);

    SendMessage(WM_NCPAINT);

    return lResult;
}
