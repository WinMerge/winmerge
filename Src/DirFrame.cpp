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
// DirFrame.cpp : implementation file
//

#include "stdafx.h"
#include "Merge.h"
#include "DirFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
};

/////////////////////////////////////////////////////////////////////////////
// CDirFrame

IMPLEMENT_DYNCREATE(CDirFrame, CMDIChildWnd)

CDirFrame::CDirFrame()
: m_picanclose(0)
, m_param(0)
{
}

CDirFrame::~CDirFrame()
{
}


BEGIN_MESSAGE_MAP(CDirFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CDirFrame)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_DIFFNUM, OnUpdateStatusNum)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirFrame message handlers

#ifdef _DEBUG
void CDirFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CDirFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

int CDirFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Directory frame has a status bar
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}	
	
	return 0;
}

void CDirFrame::SetStatus(LPCTSTR szStatus)
{
	m_wndStatusBar.SetPaneText(0, szStatus);
}


void CDirFrame::ActivateFrame(int nCmdShow) 
{
// Diffstatus is not in this frame
//	if (m_wndStatusBar.IsWindowVisible())
//		m_wndStatusBar.SetPaneText(1, _T("")); // clear the diff status
	
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

void CDirFrame::OnUpdateStatusNum(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText("");
}

// Store callback which we check to see if we're allowed to close
// This keeps us decoupled from the doc
void CDirFrame::SetClosableCallback(bool (*canclose)(void *), void * param)
{
	m_picanclose = canclose;
	m_param = param; 
}

void CDirFrame::OnClose() 
{
	if (m_picanclose && !(*m_picanclose)(m_param))
	{
		ShowWindow(SW_MINIMIZE);
		return;
	}
	
	CMDIChildWnd::OnClose();
}
