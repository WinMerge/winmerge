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
// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "Merge.h"

#include "ChildFrm.h"
#include "MainFrm.h"
#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "DiffViewBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_VIEW_DETAIL_BAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_DETAIL_BAR, OnBarCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicatorsBottom[] =
{
	ID_SEPARATOR,
	ID_SEPARATOR
};

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
#pragma warning(disable:4355) // 'this' : used in base member initializer list
: m_leftStatus(this, 0)
, m_rightStatus(this, 1)
#pragma warning(default:4355)
{
	m_bActivated = FALSE;
	m_nLastSplitPos=0;
}

CChildFrame::~CChildFrame()
{
	m_wndDetailBar.setSplitter(0);
}

BOOL CChildFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{

	//lpcs->style |= WS_MAXIMIZE;
	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL) )
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}
	
	// add the first splitter pane - the default view in column 0
	//int width=theApp.GetProfileInt(_T("Settings"), _T("WLeft"), 0);
	//if (width<=0)
	//	width = rc.Width()/2;

	if (!m_wndSplitter.CreateView(0, 0,
		RUNTIME_CLASS(CMergeEditView), CSize(-1, 200), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}
	
	// add the second splitter pane - an input view in column 1
	if (!m_wndSplitter.CreateView(0, 1,
		RUNTIME_CLASS(CMergeEditView), CSize(-1, 200), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	m_wndSplitter.ResizablePanes(TRUE);


	// Merge frame has also a dockable bar at the very bottom
	// This is not the client area, but we create it now because we want
	// to use the CCreateContext
	if (!m_wndDetailBar.Create(this, "", WS_CHILD | WS_VISIBLE, ID_VIEW_DETAIL_BAR))
	{
		TRACE0("Failed to create DiffViewBar\n");
		return FALSE;
	}

	// create a splitter with 2 rows, 1 column
	// this is not a vertical scrollable splitter (see MergeDiffDetailView.h)
	if (!m_wndDetailSplitter.CreateStatic(&m_wndDetailBar, 2, 1, WS_CHILD | WS_VISIBLE | WS_HSCROLL, AFX_IDW_PANE_FIRST+1) )
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}
	// add the first splitter pane - the default view in column 0
	if (!m_wndDetailSplitter.CreateView(0, 0,
		RUNTIME_CLASS(CMergeDiffDetailView), CSize(-1, 200), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}
	// add the second splitter pane - an input view in column 1
	if (!m_wndDetailSplitter.CreateView(1, 0,
		RUNTIME_CLASS(CMergeDiffDetailView), CSize(-1, 200), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	m_wndDetailSplitter.LockBar(TRUE);
	m_wndDetailSplitter.ResizablePanes(TRUE);
	m_wndDetailBar.setSplitter(&m_wndDetailSplitter);



	// stash left & right pointers into the mergedoc
	CMergeEditView * pLeft = (CMergeEditView *)m_wndSplitter.GetPane(0,0);
	CMergeEditView * pRight = (CMergeEditView *)m_wndSplitter.GetPane(0,1);
	// connect merge views up to display of status info
	pLeft->SetStatusInterface(&m_leftStatus);
	pRight->SetStatusInterface(&m_rightStatus);
	// tell merge doc about these views
	CMergeDoc * pDoc = dynamic_cast<CMergeDoc *>(pContext->m_pCurrentDoc);
	pDoc->SetMergeViews(pLeft, pRight);
	pLeft->m_bIsLeft = TRUE;
	pRight->m_bIsLeft = FALSE;

	// stash left & right detail pointers into the mergedoc
	CMergeDiffDetailView * pLeftDetail = (CMergeDiffDetailView *)m_wndDetailSplitter.GetPane(0,0);
	CMergeDiffDetailView * pRightDetail = (CMergeDiffDetailView *)m_wndDetailSplitter.GetPane(1,0);
	// tell merge doc about these views
	pDoc->SetMergeDetailViews(pLeftDetail, pRightDetail);
	pLeftDetail->m_bIsLeft = TRUE;
	pRightDetail->m_bIsLeft = FALSE;
	
	return TRUE;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{

	return CMDIChildWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

/**
* @brief Create the child frame, the splitter, the filename bar, the status bar,
* the diff dockable bar, and the four views
*
* @note  the panels layout is 
* <ul>
*  <li>   -----------
*  <li>		!  header !
*  <li>		!.........!
*  <li>		!.   .   .!
*  <li>		!. 1 . 2 .!
*  <li>		!.   .   .!
*  <li>		!.........!
*  <li>		!.........!
*  <li>		!  status !
*  <li>		-----------
*  <li>		!.........!
*  <li>		!.   3   .!
*  <li>		!.dockable!
*  <li>		! splitbar!
*  <li>		!.   4   .!
*  <li>		!.........!
*  <li>		-----------
* </ul>
*/
int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);

//	ModifyStyle(WS_THICKFRAME,0); // this is necessary to prevent the sizing tab on right

	// Merge frame has a header bar at top
	if (!m_wndFilePathBar.Create(this))
	{
		TRACE0("Failed to create dialog bar\n");
		return -1;      // fail to create
	}	

//	ModifyStyle(0,WS_THICKFRAME);

	// Merge frame also has a dockable bar at the very bottom
	// created in OnCreateClient 
	m_wndDetailBar.SetBarStyle(m_wndDetailBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);
	m_wndDetailBar.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
	DockControlBar(&m_wndDetailBar, AFX_IDW_DOCKBAR_BOTTOM);

	// Merge frame also has a status bar at bottom, 
	// m_wndDetailBar is below, so we create this bar after m_wndDetailBar
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicatorsBottom,
		  sizeof(indicatorsBottom)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}	

	m_wndStatusBar.SetPaneStyle(0, SBPS_NORMAL);
	m_wndStatusBar.SetPaneStyle(1, SBPS_NORMAL);


	SetTimer(0, 250, NULL); // used to update the title headers
	return 0;
}

void CChildFrame::ActivateFrame(int nCmdShow) 
{
	if (!m_bActivated) 
	{
		m_bActivated = TRUE;
		if(theApp.GetProfileInt(_T("Settings"), _T("LeftMax"), TRUE))
			nCmdShow = SW_SHOWMAXIMIZED;
		else
			nCmdShow = SW_SHOWNORMAL;
	}
	CMDIChildWnd::ActivateFrame(nCmdShow);
	UpdateHeaderSizes();

	// load the diff pane state
	// load dimension : default dimension, then LoadState if it was saved before
	int initialDiffHeight = ((CMergeDiffDetailView*)m_wndDetailSplitter.GetPane(1,0))->ComputeInitialHeight();
	UpdateDiffDockbarHeight(initialDiffHeight);
	m_wndDetailBar.LoadState(_T("Settings"));
	// load docking position
	CDockState m_pDockState;
	m_pDockState.LoadState(_T("Settings"));
	SetDockState(m_pDockState);
}

BOOL CChildFrame::DestroyWindow() 
{
	
	return CMDIChildWnd::DestroyWindow();
}

void CChildFrame::SavePosition()
{
	CRect rc;
	CWnd* pLeft = m_wndSplitter.GetPane(0,0);
	if (pLeft != NULL)
	{
		pLeft->GetWindowRect(&rc);
		theApp.WriteProfileInt(_T("Settings"), _T("WLeft"), rc.Width());
	}
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	theApp.WriteProfileInt(_T("Settings"), _T("LeftMax"), (wp.showCmd == SW_MAXIMIZE));

	// save the diff pane state
	// save dimension
	m_wndDetailBar.SaveState(_T("Settings"));
	// save docking position
	CDockState m_pDockState;
	GetDockState(m_pDockState);
	m_pDockState.SaveState(_T("Settings"));
}

void CChildFrame::OnClose() 
{
	// clean up pointers.
	SavePosition();
	CMDIChildWnd::OnClose();
}

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	UpdateHeaderSizes();
}


/// update height for panels 3/4
void CChildFrame::UpdateDiffDockbarHeight(int DiffPanelHeight)
{
	m_wndDetailBar.UpdateBarHeight(DiffPanelHeight);
}



/// update splitting position for panels 1/2 and headerbar and statusbar 
void CChildFrame::UpdateHeaderSizes()
{
	if(IsWindowVisible())
	{
		int w,wmin;
		m_wndSplitter.GetColumnInfo(0, w, wmin);
		int w1;
		m_wndSplitter.GetColumnInfo(1, w1, wmin);
		if (w<1) w=1; // Perry 2003-01-22 (I don't know why this happens)
		if (w1<1) w1=1; // Perry 2003-01-22 (I don't know why this happens)

		// for bottom status bar
		m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NORMAL, w-1);
		m_wndStatusBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_STRETCH, 0);

		// prepare file path bar to look as a status bar
		if (m_wndFilePathBar.LookLikeThisWnd(&m_wndStatusBar) == TRUE)
			RecalcLayout();

		// resize controls in header dialog bar
		m_wndFilePathBar.Resize(w, w1);
	}

}

BOOL CChildFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CMDIChildWnd::OnNotify(wParam, lParam, pResult);
}

void CChildFrame::OnTimer(UINT nIDEvent) 
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
	}
	CMDIChildWnd::OnTimer(nIDEvent);
}

void CChildFrame::SetHeaderText(int nPane, const CString &text)
{
	m_wndFilePathBar.SetFilePath(nPane, text);
}

// document commanding us to close
void CChildFrame::CloseNow()
{
	MDIActivate();
	MDIDestroy();
}

// Bridge class which implements the interface from crystal editor to frame status line display
CChildFrame::MergeStatus::MergeStatus(CChildFrame * pFrame, int base)
: m_pFrame(pFrame)
, m_base(base)
// CString m_sLine
, m_nChars(0)
// CString m_sEol
// CString m_sEolDisplay
{
}

// Send status line info (about one side of merge view) to screen
void CChildFrame::MergeStatus::Update()
{
	if (IsWindow(m_pFrame->m_wndStatusBar.m_hWnd))
	{
		CString str;
		if (m_nChars == -1)
		{
			str.Format(IDS_EMPTY_LINE_STATUS_INFO, m_sLine);
		}
		else
		{
			str.Format(IDS_LINE_STATUS_INFO, m_sLine, m_nChars, m_sEolDisplay);
		}

		m_pFrame->m_wndStatusBar.SetPaneText(m_base, str);
	}
}

// Visible representation of eol
static CString EolString(const CString & sEol)
{
	if (sEol == _T("\r\n")) return _T("CRLF");
	if (sEol == _T("\n")) return _T("LF");
	if (sEol == _T("\r")) return _T("CR");
	if (sEol.IsEmpty()) return _T("None");
	return _T("?");
}

// Receive status line info from crystal window and display
void CChildFrame::MergeStatus::SetLineInfo(LPCTSTR szLine, int nChars, LPCTSTR szEol)
{
	if (m_sLine!=szLine || m_nChars!=nChars || m_sEol != szEol)
	{
		m_sLine = szLine;
		m_nChars = nChars;
		m_sEol = szEol;
		m_sEolDisplay = EolString(m_sEol);
		Update();
	}
}
