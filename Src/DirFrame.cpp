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

#include "stdafx.h"
#include "DirFrame.h"
#include "Merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Statusbar pane indexes
 */
enum
{
	PANE_COMPMETHOD = 1,
	PANE_FILTER,
	PANE_LEFT_RO,
	PANE_MIDDLE_RO,
	PANE_RIGHT_RO,
};

/**
 * @brief Width of compare method name pane in statusbar
 */
const int COMPMETHOD_PANEL_WIDTH = 100;
/**
 * @brief Width of filter name pane in statusbar
 */
const int FILTER_PANEL_WIDTH = 100;

/**
 * @brief Bottom statusbar panels and indicators
 */
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
};

/**
 * @brief RO status panel width
 */
static UINT RO_PANEL_WIDTH = 30;

/////////////////////////////////////////////////////////////////////////////
// CDirFrame

IMPLEMENT_DYNCREATE(CDirFrame, CMDIChildWnd)

CDirFrame::CDirFrame()
: m_hIdentical(NULL)
, m_hDifferent(NULL)
{
}

CDirFrame::~CDirFrame()
{
}


BEGIN_MESSAGE_MAP(CDirFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CDirFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirFrame message handlers

/**
 * @brief Create statusbar
 */
int CDirFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_TOP);

	// Dir frame has a header bar at top
	if (!m_wndFilePathBar.Create(this))
	{
		TRACE0("Failed to create header bar\n");
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
	
	String sText = _("RO");
	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };
	m_wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH | SBPS_NOBORDERS, 0);
	m_wndStatusBar.SetPaneInfo(PANE_COMPMETHOD, ID_STATUS_FILTER, 0, pointToPixel(COMPMETHOD_PANEL_WIDTH));
	m_wndStatusBar.SetPaneInfo(PANE_FILTER, ID_STATUS_FILTER, 0, pointToPixel(FILTER_PANEL_WIDTH));
	m_wndStatusBar.SetPaneInfo(PANE_LEFT_RO, ID_STATUS_LEFTDIR_RO, 0, pointToPixel(RO_PANEL_WIDTH));
	m_wndStatusBar.SetPaneInfo(PANE_MIDDLE_RO, ID_STATUS_MIDDLEDIR_RO, 0, pointToPixel(RO_PANEL_WIDTH));
	m_wndStatusBar.SetPaneInfo(PANE_RIGHT_RO, ID_STATUS_RIGHTDIR_RO, 0, pointToPixel(RO_PANEL_WIDTH));
	m_wndStatusBar.SetPaneText(PANE_LEFT_RO, sText.c_str(), TRUE); 
	m_wndStatusBar.SetPaneText(PANE_MIDDLE_RO, sText.c_str(), TRUE); 
	m_wndStatusBar.SetPaneText(PANE_RIGHT_RO, sText.c_str(), TRUE);

	m_hIdentical = AfxGetApp()->LoadIcon(IDI_EQUALFOLDER);
	m_hDifferent = AfxGetApp()->LoadIcon(IDI_NOTEQUALFOLDER);

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
 * @brief Set current compare method name to statusbar
 * @param [in] nCompMethod compare method to show
 */
void CDirFrame::SetCompareMethodStatusDisplay(int nCompMethod)
{
	m_wndStatusBar.SetPaneText(PANE_COMPMETHOD, LoadResString(IDS_COMPMETHOD_FULL_CONTENTS + nCompMethod).c_str());
}

/**
 * @brief Set active filter name to statusbar
 * @param [in] szFilter Filtername to show
 */
void CDirFrame::SetFilterStatusDisplay(LPCTSTR szFilter)
{
	m_wndStatusBar.SetPaneText(PANE_FILTER, szFilter);
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
	// get the active child frame, and a flag whether it is maximized
	BOOL bMaximized;
	CMDIChildWnd * oldActiveFrame = GetMDIFrame()->MDIGetActive(&bMaximized);
	if (oldActiveFrame == NULL)
		// for the first frame, get the restored/maximized state from the registry
		bMaximized = theApp.GetProfileInt(_T("Settings"), _T("ActiveFrameMax"), TRUE);
	if (bMaximized)
		nCmdShow = SW_SHOWMAXIMIZED;
	else
		nCmdShow = SW_SHOWNORMAL;

	CMDIChildWnd::ActivateFrame(nCmdShow);
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CDirFrame::UpdateResources()
{
}

/**
* @brief Reflect comparison result in window's icon.
* @param nResult [in] Last comparison result which the application returns.
*/
void CDirFrame::SetLastCompareResult(int nResult)
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
		GetMDIFrame()->OnUpdateFrameTitle(FALSE);
	}

	theApp.SetLastCompareResult(nResult);
}

void CDirFrame::OnClose() 
{	
	CMDIChildWnd::OnClose();
}

/**
 * @brief Save maximized state before destroying window
 */
BOOL CDirFrame::DestroyWindow() 
{
	// If we are active, save the restored/maximized state
	// If we are not, do nothing and let the active frame do the job.
	if (this->GetParentFrame()->GetActiveFrame() == (CFrameWnd*)this)
	{
		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(&wp);
		theApp.WriteProfileInt(_T("Settings"), _T("ActiveFrameMax"), (wp.showCmd == SW_MAXIMIZE));
	}

	return CMDIChildWnd::DestroyWindow();
}

void CDirFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
	m_wndFilePathBar.Resize();
}

void CDirFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
	// [Fix for MFC 8.0 MDI Maximizing Child Window bug on Vista]
	// https://groups.google.com/forum/#!topic/microsoft.public.vc.mfc/iajCdW5DzTM
#if _MFC_VER >= 0x0800
	lpMMI->ptMaxTrackSize.x = max(lpMMI->ptMaxTrackSize.x, lpMMI->ptMaxSize.x);
	lpMMI->ptMaxTrackSize.y = max(lpMMI->ptMaxTrackSize.y, lpMMI->ptMaxSize.y);
#endif
}
