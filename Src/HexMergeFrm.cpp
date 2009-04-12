/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  HexMergeFrm.cpp
 *
 * @brief Implementation file for CHexMergeFrame
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "Merge.h"
#include "MainFrm.h"
#include "HexMergeFrm.h"
#include "HexMergeDoc.h"
#include "HexMergeView.h"
#include "OptionsDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHexMergeFrame

IMPLEMENT_DYNCREATE(CHexMergeFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CHexMergeFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CHexMergeFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DETAIL_BAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_DETAIL_BAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCATION_BAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_LOCATION_BAR, OnBarCheck)
	ON_MESSAGE(MSG_STORE_PANESIZES, OnStorePaneSizes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/**
 * @brief Statusbar pane indexes
 */
enum
{
	PANE_LEFT_INFO = 0,
	PANE_LEFT_RO,
	PANE_LEFT_EOL,
	PANE_RIGHT_INFO,
	PANE_RIGHT_RO,
	PANE_RIGHT_EOL,
};

/////////////////////////////////////////////////////////////////////////////
// CHexMergeFrame construction/destruction

CHexMergeFrame::CHexMergeFrame()
: m_hIdentical(NULL)
, m_hDifferent(NULL)
{
	m_bActivated = FALSE;
	m_nLastSplitPos = 0;
	m_pMergeDoc = 0;
}

CHexMergeFrame::~CHexMergeFrame()
{
}

/**
 * @brief Customize a heksedit control's settings
 */
static void Customize(IHexEditorWindow::Settings *settings)
{
	settings->bSaveIni = FALSE;
	settings->iAutomaticBPL = FALSE;
	settings->iBytesPerLine = 16;
	settings->iFontSize = 8;
}

/**
 * @brief Customize a heksedit control's colors
 */
static void Customize(IHexEditorWindow::Colors *colors)
{
	COptionsMgr *pOptionsMgr = GetOptionsMgr();
	colors->iSelBkColorValue = RGB(224, 224, 224);
	colors->iDiffBkColorValue = pOptionsMgr->GetInt(OPT_CLR_DIFF);
	colors->iSelDiffBkColorValue = pOptionsMgr->GetInt(OPT_CLR_SELECTED_DIFF);
	colors->iDiffTextColorValue = pOptionsMgr->GetInt(OPT_CLR_DIFF_TEXT);
	colors->iSelDiffTextColorValue = pOptionsMgr->GetInt(OPT_CLR_SELECTED_DIFF_TEXT);
}

/**
 * @brief Customize a heksedit control's settings and colors
 */
static void Customize(IHexEditorWindow *pif)
{
	Customize(pif->get_settings());
	Customize(pif->get_colors());
	LANGID wLangID = (LANGID)GetThreadLocale();
	pif->load_lang(wLangID);
}

/**
 * @brief Create a status bar to be associated with a heksedit control
 */
void CHexMergeFrame::CreateHexWndStatusBar(CStatusBar &wndStatusBar, CWnd *pwndPane)
{
	wndStatusBar.Create(pwndPane, WS_CHILD|WS_VISIBLE);
	wndStatusBar.SetIndicators(0, 3);
	wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH, 0);
	wndStatusBar.SetPaneInfo(1, 0, 0, 80);
	wndStatusBar.SetPaneInfo(2, 0, 0, 80);
	wndStatusBar.SetParent(this);
	wndStatusBar.SetWindowPos(&wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

/**
 * @brief Create the splitter, the filename bar, the status bar, and the two views
 */
BOOL CHexMergeFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL) )
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	if (!m_wndSplitter.CreateView(0, 0,
		RUNTIME_CLASS(CHexMergeView), CSize(-1, 200), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}
	
	// add the second splitter pane - an input view in column 1
	if (!m_wndSplitter.CreateView(0, 1,
		RUNTIME_CLASS(CHexMergeView), CSize(-1, 200), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	m_wndSplitter.ResizablePanes(TRUE);
	m_wndSplitter.AutoResizePanes(GetOptionsMgr()->GetBool(OPT_RESIZE_PANES));

	// Merge frame has a header bar at top
	if (!m_wndFilePathBar.Create(this))
	{
		TRACE0("Failed to create dialog bar\n");
		return FALSE;      // fail to create
	}
	// Set filename bars inactive so colors get initialized
	m_wndFilePathBar.SetActive(0, FALSE);
	m_wndFilePathBar.SetActive(1, FALSE);

	CHexMergeView *pLeft = static_cast<CHexMergeView *>(m_wndSplitter.GetPane(0, 0));
	CHexMergeView *pRight = static_cast<CHexMergeView *>(m_wndSplitter.GetPane(0, 1));

	m_wndLeftStatusBar.m_cxRightBorder = 4;
	CreateHexWndStatusBar(m_wndLeftStatusBar, pLeft);
	pRight->ModifyStyle(0, WS_THICKFRAME); // Create an SBARS_SIZEGRIP
	CreateHexWndStatusBar(m_wndRightStatusBar, pRight);
	pRight->ModifyStyle(WS_THICKFRAME, 0);
	CSize size = m_wndLeftStatusBar.CalcFixedLayout(TRUE, TRUE);
	m_rectBorder.bottom = size.cy;

	m_hIdentical = AfxGetApp()->LoadIcon(IDI_EQUALBINARY);
	m_hDifferent = AfxGetApp()->LoadIcon(IDI_BINARYDIFF);

	// get the IHexEditorWindow interfaces
	IHexEditorWindow *pifLeft = pLeft->GetInterface();
	IHexEditorWindow *pifRight = pRight->GetInterface();

	// tell the heksedit controls about each other
	pifLeft->set_sibling(pifRight);
	pifRight->set_sibling(pifLeft);

	// adjust a few settings and colors
	Customize(pifLeft);
	Customize(pifRight);

	// tell merge doc about these views
	m_pMergeDoc = dynamic_cast<CHexMergeDoc *>(pContext->m_pCurrentDoc);
	m_pMergeDoc->SetMergeViews(pLeft, pRight);
	pLeft->m_nThisPane = 0;
	pRight->m_nThisPane = 1;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CHexMergeFrame message handlers

/**
 * @brief Handle translation of default messages on the status bar
 */
void CHexMergeFrame::GetMessageString(UINT nID, CString& rMessage) const
{
	// load appropriate string
	const String s = theApp.LoadString(nID);
	if (!AfxExtractSubString(rMessage, &*s.begin(), 0))
	{
		// not found
		TRACE1("Warning: no message line prompt for ID 0x%04X.\n", nID);
	}
}

/**
 * @brief Save the window's position, free related resources, and destroy the window
 */
BOOL CHexMergeFrame::DestroyWindow() 
{
	SavePosition();
	// If we are active, save the restored/maximized state
	// If we are not, do nothing and let the active frame do the job.
 	if (GetParentFrame()->GetActiveFrame() == this)
	{
		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(&wp);
		theApp.WriteProfileInt(_T("Settings"), _T("ActiveFrameMax"), (wp.showCmd == SW_MAXIMIZE));
	}

	if (m_hIdentical != NULL)
	{
		DestroyIcon(m_hIdentical);
		m_hIdentical = NULL;
	}

	if (m_hDifferent != NULL)
	{
		DestroyIcon(m_hDifferent);
		m_hDifferent = NULL;
	}

	return CMDIChildWnd::DestroyWindow();
}

/**
 * @brief Save coordinates of the frame, splitters, and bars
 *
 * @note Do not save the maximized/restored state here. We are interested
 * in the state of the active frame, and maybe this frame is not active
 */
void CHexMergeFrame::SavePosition()
{
	if (CWnd *pLeft = m_wndSplitter.GetPane(0,0))
	{
		CRect rc;
		pLeft->GetWindowRect(&rc);
		theApp.WriteProfileInt(_T("Settings"), _T("WLeft"), rc.Width());
	}
}

void CHexMergeFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	UpdateHeaderSizes();
}

/// update splitting position for panels 1/2 and headerbar and statusbar 
void CHexMergeFrame::UpdateHeaderSizes()
{
	if (IsWindowVisible())
	{
		int w0, w1, wmin;
		m_wndSplitter.GetColumnInfo(0, w0, wmin);
		m_wndSplitter.GetColumnInfo(1, w1, wmin);
		if (w0 < 1) w0 = 1; // Perry 2003-01-22 (I don't know why this happens)
		if (w1 < 1) w1 = 1; // Perry 2003-01-22 (I don't know why this happens)
		// resize controls in header dialog bar
		m_wndFilePathBar.Resize(w0, w1);
		RECT rc;
		GetClientRect(&rc);
		rc.top = rc.bottom - m_rectBorder.bottom;
		rc.left = w0 + 8;
		m_wndRightStatusBar.MoveWindow(&rc);
		rc.right = rc.left;
		rc.left = 0;
		m_wndLeftStatusBar.MoveWindow(&rc);
	}
}

IHeaderBar *CHexMergeFrame::GetHeaderInterface()
{
	return &m_wndFilePathBar;
}

/**
 * @brief Reflect comparison result in window's icon.
 * @param nResult [in] Last comparison result which the application returns.
 */
void CHexMergeFrame::SetLastCompareResult(int nResult)
{
	HICON hCurrent = GetIcon(FALSE);
	HICON hReplace = (nResult == 0) ? m_hIdentical : m_hDifferent;

	if (hCurrent != hReplace)
	{
		SetIcon(hReplace, TRUE);

		BOOL bMaximized;
		GetMDIFrame()->MDIGetActive(&bMaximized);

		// When MDI maximized the window icon is drawn on the menu bar, so we
		// need to notify it that our icon has changed.
		if (bMaximized)
		{
			GetMDIFrame()->DrawMenuBar();
		}
	}

	theApp.SetLastCompareResult(nResult);
}

void CHexMergeFrame::UpdateAutoPaneResize()
{
	m_wndSplitter.AutoResizePanes(GetOptionsMgr()->GetBool(OPT_RESIZE_PANES));
}

void CHexMergeFrame::UpdateSplitter()
{
	m_wndSplitter.RecalcLayout();
}

/**
 * @brief Synchronize control and status bar placements with splitter position,
 * update mod indicators, synchronize scrollbars
 */
void CHexMergeFrame::OnIdleUpdateCmdUI()
{
	if (IsWindowVisible())
	{
		int w,wmin;
		m_wndSplitter.GetColumnInfo(0, w, wmin);
		if (w != m_nLastSplitPos && w > 0)
		{
			UpdateHeaderSizes();
			m_nLastSplitPos = w;
		}
		CHexMergeView *pLeft = (CHexMergeView *)m_wndSplitter.GetPane(0, MERGE_VIEW_LEFT);
		CHexMergeView *pRight = (CHexMergeView *)m_wndSplitter.GetPane(0, MERGE_VIEW_RIGHT);

		// Update mod indicators
		TCHAR ind[2];

		if (m_wndFilePathBar.GetDlgItemText(IDC_STATIC_TITLE_LEFT, ind, 2))
			if (pLeft->GetModified() ? ind[0] != _T('*') : ind[0] == _T('*'))
				m_pMergeDoc->UpdateHeaderPath(MERGE_VIEW_LEFT);

		if (m_wndFilePathBar.GetDlgItemText(IDC_STATIC_TITLE_RIGHT, ind, 2))
			if (pRight->GetModified() ? ind[0] != _T('*') : ind[0] == _T('*'))
				m_pMergeDoc->UpdateHeaderPath(MERGE_VIEW_RIGHT);

		// Synchronize scrollbars
		SCROLLINFO si, siLeft, siRight;
		// Synchronize horizontal scrollbars
		pLeft->GetScrollInfo(SB_HORZ, &si, SIF_ALL | SIF_DISABLENOSCROLL);
		siLeft = si;
		pRight->GetScrollInfo(SB_HORZ, &si, SIF_ALL | SIF_DISABLENOSCROLL);
		siRight = si;
		if (si.nMin > siLeft.nMin)
			si.nMin = siLeft.nMin;
		if (si.nPage < siLeft.nPage)
			si.nPage = siLeft.nPage;
		if (si.nMax < siLeft.nMax)
			si.nMax = siLeft.nMax;
		if (GetFocus() != pRight)
		{
			si.nPos = siLeft.nPos;
			si.nTrackPos = siLeft.nTrackPos;
		}
		if (memcmp(&si, &siLeft, sizeof si))
		{
			pLeft->SetScrollInfo(SB_HORZ, &si);
			pLeft->SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBTRACK, si.nTrackPos));
		}
		if (memcmp(&si, &siRight, sizeof si))
		{
			pRight->SetScrollInfo(SB_HORZ, &si);
			pRight->SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBTRACK, si.nTrackPos));
		}
		m_wndSplitter.GetScrollBarCtrl(pLeft, SB_HORZ)->SetScrollInfo(&si);
		m_wndSplitter.GetScrollBarCtrl(pRight, SB_HORZ)->SetScrollInfo(&si);
		// Synchronize vertical scrollbars
		pLeft->GetScrollInfo(SB_VERT, &si, SIF_ALL | SIF_DISABLENOSCROLL);
		siLeft = si;
		pRight->GetScrollInfo(SB_VERT, &si, SIF_ALL | SIF_DISABLENOSCROLL);
		siRight = si;
		if (si.nMin > siLeft.nMin)
			si.nMin = siLeft.nMin;
		if (si.nMax < siLeft.nMax)
			si.nMax = siLeft.nMax;
		if (GetFocus() != pRight)
		{
			si.nPos = siLeft.nPos;
			si.nTrackPos = siLeft.nTrackPos;
		}
		if (memcmp(&si, &siLeft, sizeof si))
		{
			pLeft->SetScrollInfo(SB_VERT, &si);
			pLeft->SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, si.nTrackPos));
		}
		if (memcmp(&si, &siRight, sizeof si))
		{
			pRight->SetScrollInfo(SB_VERT, &si);
			pRight->SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBTRACK, si.nTrackPos));
		}
		m_wndSplitter.GetScrollBarCtrl(pRight, SB_VERT)->SetScrollInfo(&si);
	}
	CMDIChildWnd::OnIdleUpdateCmdUI();
}

/// Document commanding us to close
void CHexMergeFrame::CloseNow()
{
	SavePosition(); // Save settings before closing!
	MDIActivate();
	MDIDestroy();
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CHexMergeFrame::UpdateResources()
{
}

/**
 * @brief Save pane sizes and positions when one of panes requests it.
 */
LRESULT CHexMergeFrame::OnStorePaneSizes(WPARAM wParam, LPARAM lParam)
{
	SavePosition();
	return 0;
}
