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
#include "DiffView.h"
#include "MainFrm.h"

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR
};

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	m_bActivated = FALSE;
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	//lpcs->style |= WS_MAXIMIZE;
	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, 2)) //, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL))
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
		RUNTIME_CLASS(CDiffView), CSize(-1, 200), pContext))
	{
		TRACE0("Failed to create first pane\n");
		return FALSE;
	}
	
	// add the second splitter pane - an input view in column 1
	if (!m_wndSplitter.CreateView(0, 1,
		RUNTIME_CLASS(CDiffView), CSize(-1, 200), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}
	
	mf->m_pLeft = (CDiffView *)m_wndSplitter.GetPane(0,0);
	//mf->m_pLeft->OnInitialUpdate();
	mf->m_pLeft->m_bIsLeft=TRUE;
	mf->m_pRight = (CDiffView *)m_wndSplitter.GetPane(0,1);
	//mf->m_pRight->OnInitialUpdate();
	mf->m_pRight->m_bIsLeft=FALSE;
	mf->m_pLeft->UpdateWindow();

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
	
	//MDIMaximize();
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
	mf->m_pLeft = mf->m_pRight = NULL;
	SavePosition();
	CMDIChildWnd::OnClose();
}

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	CRect rc;
	GetClientRect(&rc);
	if(IsWindowVisible())
	{
		m_wndSplitter.SetColumnInfo(0, rc.Width()/2, 10);
		m_wndSplitter.RecalcLayout();
	}
}
