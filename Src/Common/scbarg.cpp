/////////////////////////////////////////////////////////////////////////
//
// CSizingControlBarG           Version 2.44
// 
// Created: Jan 24, 1998        Last Modified: March 31, 2002
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

// sizecbar.cpp : implementation file
//

#include "StdAfx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarG

IMPLEMENT_DYNAMIC(CSizingControlBarG, baseCSizingControlBarG);

CSizingControlBarG::CSizingControlBarG()
{
}

CSizingControlBarG::~CSizingControlBarG()
{
}

BEGIN_MESSAGE_MAP(CSizingControlBarG, baseCSizingControlBarG)
    //{{AFX_MSG_MAP(CSizingControlBarG)
    ON_WM_NCLBUTTONUP()
    ON_WM_NCHITTEST()
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarG message handlers

/////////////////////////////////////////////////////////////////////////
// Mouse Handling
//

void CSizingControlBarG::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
    if (nHitTest == HTCLOSE)
        m_pDockSite->ShowControlBar(this, FALSE, FALSE); // hide

    baseCSizingControlBarG::OnNcLButtonUp(nHitTest, point);
}

void CSizingControlBarG::NcCalcClient(LPRECT pRc, UINT nDockBarID)
{
    CRect rcBar(pRc); // save the bar rect
    // subtract edges
    baseCSizingControlBarG::NcCalcClient(pRc, nDockBarID);

    if (!HasGripper())
        return;

    CRect rc(pRc); // the client rect as calculated by the base class

    bool bHorz = (nDockBarID == AFX_IDW_DOCKBAR_TOP) ||
                 (nDockBarID == AFX_IDW_DOCKBAR_BOTTOM);

    const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
    auto pointToPixel = [lpx](double point) { return static_cast<int>(point * lpx / 72); };

    if (bHorz)
        rc.DeflateRect(pointToPixel(m_dblGripper), 0, 0, 0);
    else
        rc.DeflateRect(0, pointToPixel(m_dblGripper), 0, 0);

    // set position for the "x" (hide bar) button
    CPoint ptOrgBtn;
    if (bHorz)
        ptOrgBtn = CPoint(rc.left - pointToPixel(9.75), rc.top);
    else
        ptOrgBtn = CPoint(rc.right - pointToPixel(9.0), rc.top - pointToPixel(9.75));

    m_biHide.Move(ptOrgBtn - rcBar.TopLeft());

    *pRc = rc;
}

void CSizingControlBarG::NcPaintGripper(CDC* pDC, CRect rcClient)
{
    if (!HasGripper())
        return;

    // paints a simple "two raised lines" gripper
    // override this if you want a more sophisticated gripper
    const int lpx = pDC->GetDeviceCaps(LOGPIXELSX);
    auto pointToPixel = [lpx](double point) { return static_cast<int>(point * lpx / 72); };
    CRect gripper = rcClient;
    CRect rcbtn(m_biHide.ptOrg, CSize(pointToPixel(m_biHide.dblBoxSize), pointToPixel(m_biHide.dblBoxSize)));
    bool bHorz = IsHorzDocked();

    gripper.DeflateRect(1, 1);
    if (bHorz)
    {   // gripper at left
        gripper.left -= pointToPixel(m_dblGripper);
        gripper.right = gripper.left + pointToPixel(2.25);
        gripper.top = rcbtn.bottom + pointToPixel(2.25);
    }
    else
    {   // gripper at top
        gripper.top -= pointToPixel(m_dblGripper);
        gripper.bottom = gripper.top + pointToPixel(2.25);
        gripper.right = rcbtn.left - pointToPixel(2.25);
    }

    pDC->Draw3dRect(gripper, ::GetSysColor(COLOR_BTNHIGHLIGHT),
        ::GetSysColor(COLOR_BTNSHADOW));

    gripper.OffsetRect(bHorz ? pointToPixel(2.25) : 0, bHorz ? 0 : pointToPixel(2.25));

    pDC->Draw3dRect(gripper, ::GetSysColor(COLOR_BTNHIGHLIGHT),
        ::GetSysColor(COLOR_BTNSHADOW));

    m_biHide.Paint(pDC);
}

NCHITTEST_RESULT CSizingControlBarG::OnNcHitTest(CPoint point)
{
    CRect rcBar;
    GetWindowRect(rcBar);

    LRESULT nRet = baseCSizingControlBarG::OnNcHitTest(point);
    if (nRet != HTCLIENT)
        return nRet;

    const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
    auto pointToPixel = [lpx](double point) { return static_cast<int>(point * lpx / 72); };
    CRect rc(m_biHide.ptOrg, CSize(pointToPixel(m_biHide.dblBoxSize), pointToPixel(m_biHide.dblBoxSize)));
    rc.OffsetRect(rcBar.TopLeft());
    if (rc.PtInRect(point))
        return HTCLOSE;

    return HTCLIENT;
}

/////////////////////////////////////////////////////////////////////////
// CSizingControlBarG implementation helpers

void CSizingControlBarG::OnUpdateCmdUI(CFrameWnd* pTarget,
                                      BOOL bDisableIfNoHndler)
{
    UNUSED_ALWAYS(bDisableIfNoHndler);
    UNUSED_ALWAYS(pTarget);

    if (!HasGripper())
        return;

    bool bNeedPaint = false;

    CPoint pt;
    ::GetCursorPos(&pt);
    bool bHit = (OnNcHitTest(pt) == HTCLOSE);
    bool bLButtonDown = (::GetKeyState(VK_LBUTTON) < 0);

    bool bWasPushed = m_biHide.bPushed;
    m_biHide.bPushed = bHit && bLButtonDown;

    bool bWasRaised = m_biHide.bRaised;
    m_biHide.bRaised = bHit && !bLButtonDown;

    bNeedPaint |= (m_biHide.bPushed ^ bWasPushed) ||
                  (m_biHide.bRaised ^ bWasRaised);

    if (bNeedPaint)
        SendMessage(WM_NCPAINT);
}

/////////////////////////////////////////////////////////////////////////
// CSCBButton

CSCBButton::CSCBButton()
{
    bRaised = false;
    bPushed = false;
}

void CSCBButton::Paint(CDC* pDC)
{
    const int lpx = pDC->GetDeviceCaps(LOGPIXELSX);
    auto pointToPixel = [lpx](double point) { return static_cast<int>(point * lpx / 72); };
    CRect rc(ptOrg, CSize(pointToPixel(dblBoxSize), pointToPixel(dblBoxSize)));

    if (bPushed)
        pDC->Draw3dRect(rc, ::GetSysColor(COLOR_BTNSHADOW),
            ::GetSysColor(COLOR_BTNHIGHLIGHT));
    else
        if (bRaised)
            pDC->Draw3dRect(rc, ::GetSysColor(COLOR_BTNHIGHLIGHT),
                ::GetSysColor(COLOR_BTNSHADOW));

    COLORREF clrOldTextColor = pDC->GetTextColor();
    pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
    int nPrevBkMode = pDC->SetBkMode(TRANSPARENT);
    CFont font;
    font.CreatePointFont(60/*6 points*/, _T("Marlett"));
    CFont* oldfont = pDC->SelectObject(&font);

    pDC->TextOut(ptOrg.x + pointToPixel(1.5), ptOrg.y + pointToPixel(1.5), CString(_T("r"))); // x-like

    pDC->SelectObject(oldfont);
    pDC->SetBkMode(nPrevBkMode);
    pDC->SetTextColor(clrOldTextColor);
}

bool CSizingControlBarG::HasGripper() const
{
#if defined(_SCB_MINIFRAME_CAPTION) || !defined(_SCB_REPLACE_MINIFRAME)
    // if the miniframe has a caption, don't display the gripper
    if (IsFloating())
        return false;
#endif //_SCB_MINIFRAME_CAPTION

    return true;
}
