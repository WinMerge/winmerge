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
	

	CRect rc;
	GetClientRect(&rc);
	m_wndSplitter.SetColumnInfo(0, rc.Width()/2, 10);
	m_wndSplitter.RecalcLayout();
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

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyle(WS_THICKFRAME,0); // this is necessary to prevent the sizing tab on right

	// Merge frame has a status bar at top
	if (!m_wndFilePathBar.Create(this))
	{
		TRACE0("Failed to create dialog bar\n");
		return -1;      // fail to create
	}	

	ModifyStyle(0,WS_THICKFRAME);

	// Merge frame also has a status bar at bottom
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicatorsBottom,
		  sizeof(indicatorsBottom)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}	

	m_wndStatusBar.SetPaneStyle(0, SBPS_NORMAL);
	m_wndStatusBar.SetPaneStyle(1, SBPS_NORMAL);

	m_wndSplitter.SetScrollStyle(WS_HSCROLL|WS_VSCROLL);
	m_wndSplitter.RecalcLayout();

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
		CRect rc;
		GetClientRect(&rc);
		m_wndSplitter.SetColumnInfo(0, rc.Width()/2, 10);
		m_wndSplitter.RecalcLayout();
    }
	UpdateHeaderSizes();
	CMDIChildWnd::ActivateFrame(nCmdShow);
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
	
	if(IsWindowVisible())
	{
		CRect rc;
		GetClientRect(&rc);		
		m_wndSplitter.SetColumnInfo(0, rc.Width()/2, 10);
		m_wndSplitter.RecalcLayout();

		UpdateHeaderSizes();
	}
}

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
