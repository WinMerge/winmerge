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
 * @file  DirFrame.cpp
 *
 * @brief Implementation file for CDirFrame
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "Merge.h"
#include "DirFrame.h"
#include "FilepathEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Bottom statusbar panels and indicators
 */
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_SEPARATOR,
};

/**
 * @brief RO status panel width
 */
static UINT RO_PANEL_WIDTH = 40;

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
	ON_WM_SIZE()
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

/**
 * @brief Create statusbar
 */
int CDirFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Merge frame has a header bar at top
	if (!m_wndFilePathBar.Create(this))
	{
		TRACE0("Failed to create dialog bar\n");
		return -1;      // fail to create
	}	

	// Directory frame has a status bar
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}	
	
	CString sText;
	VERIFY(sText.LoadString(IDS_STATUSBAR_READONLY));
	m_wndStatusBar.SetPaneInfo(1, ID_STATUS_LEFTDIR_RO, 0, RO_PANEL_WIDTH);
	m_wndStatusBar.SetPaneInfo(2, ID_STATUS_RIGHTDIR_RO, 0, RO_PANEL_WIDTH);
	m_wndStatusBar.SetPaneText(1, sText, TRUE); 
	m_wndStatusBar.SetPaneText(2, sText, TRUE);
	return 0;
}

/**
 * @brief Set statusbar text
 */
void CDirFrame::SetStatus(LPCTSTR szStatus)
{
	m_wndStatusBar.SetPaneText(0, szStatus);
}

/**
 * @brief Get the interface to the header (path) bar
 */
IHeaderBar * CDirFrame::GetHeaderInterface() {
	return &m_wndFilePathBar;
}

/**
 * @brief Restore maximized state of directory compare window
 */
void CDirFrame::ActivateFrame(int nCmdShow) 
{
	if (theApp.GetProfileInt(_T("Settings"), _T("DirViewMax"), FALSE))
		nCmdShow = SW_MAXIMIZE;

	CMDIChildWnd::ActivateFrame(nCmdShow);

	// prepare file path bar to look as a status bar
	if (m_wndFilePathBar.LookLikeThisWnd(&m_wndStatusBar) == TRUE)
		RecalcLayout();
}

void CDirFrame::OnUpdateStatusNum(CCmdUI* pCmdUI) 
{
	pCmdUI->SetText(_T(""));
}

/**
 * @brief Store callback which we check to see if we're allowed to close
 * This keeps us decoupled from the doc
 */
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

/**
 * @brief Save maximized state before destroying window
 */
BOOL CDirFrame::DestroyWindow() 
{
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wp);
	theApp.WriteProfileInt(_T("Settings"), _T("DirViewMax"), (wp.showCmd == SW_MAXIMIZE));
	
	return CMDIChildWnd::DestroyWindow();
}

void CDirFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	m_wndFilePathBar.Resize();
}
