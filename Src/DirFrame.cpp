/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DirFrame.cpp
 *
 * @brief Implementation file for CDirFrame
 *
 */

#include "stdafx.h"
#include "DirFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Statusbar pane indexes
 */
enum
{
	PANE_FILTER = 1,
	PANE_COMPMETHOD,
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
const int FILTER_PANEL_WIDTH = 200;

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

IMPLEMENT_DYNCREATE(CDirFrame, CMergeFrameCommon)

CDirFrame::CDirFrame()
: CMergeFrameCommon(IDI_EQUALFOLDER, IDI_NOTEQUALFOLDER)
{
}

CDirFrame::~CDirFrame()
{
}

BEGIN_MESSAGE_MAP(CDirFrame, CMergeFrameCommon)
	//{{AFX_MSG_MAP(CDirFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_DISPLAY_FILTER_BAR_MENU, OnViewDisplayFilterBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPLAY_FILTER_BAR_MENU, OnUpdateDisplayViewFilterBar)
	ON_COMMAND(IDCANCEL, OnDisplayFilterBarClose)
	ON_COMMAND(IDC_FILTERFILE_MASK_MENU, OnDisplayFilterBarMaskMenu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirFrame message handlers

/**
 * @brief Create statusbar
 */
int CDirFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_TOP);

	// Dir frame has a header bar at top
	if (!m_wndFilePathBar.Create(this))
	{
		TRACE0("Failed to create header bar\n");
		return -1;      // fail to create
	}	

	// Directory frame has a status bar
	if (!m_wndStatusBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, AFX_IDW_CONTROLBAR_FIRST+30) ||
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
	m_wndStatusBar.SetPaneInfo(PANE_FILTER, ID_STATUS_FILTER, SBPS_CLICKABLE, pointToPixel(FILTER_PANEL_WIDTH));
	m_wndStatusBar.SetPaneInfo(PANE_COMPMETHOD, ID_STATUS_FILTER, SBPS_CLICKABLE, pointToPixel(COMPMETHOD_PANEL_WIDTH));
	m_wndStatusBar.SetPaneInfo(PANE_LEFT_RO, ID_STATUS_LEFTDIR_RO, SBPS_CLICKABLE, pointToPixel(RO_PANEL_WIDTH));
	m_wndStatusBar.SetPaneInfo(PANE_MIDDLE_RO, ID_STATUS_MIDDLEDIR_RO, SBPS_CLICKABLE, pointToPixel(RO_PANEL_WIDTH));
	m_wndStatusBar.SetPaneInfo(PANE_RIGHT_RO, ID_STATUS_RIGHTDIR_RO, SBPS_CLICKABLE, pointToPixel(RO_PANEL_WIDTH));
	m_wndStatusBar.SetPaneText(PANE_LEFT_RO, sText.c_str(), TRUE); 
	m_wndStatusBar.SetPaneText(PANE_MIDDLE_RO, sText.c_str(), TRUE); 
	m_wndStatusBar.SetPaneText(PANE_RIGHT_RO, sText.c_str(), TRUE);

	// load docking positions and sizes
	CDockState dockState;
	dockState.LoadState(_T("Settings-DirFrame"));
	SetDockState(dockState);

	return 0;
}

/**
 * @brief Set statusbar text
 */
void CDirFrame::SetStatus(const tchar_t* szStatus)
{
	m_wndStatusBar.SetPaneText(0, szStatus);
}

/**
 * @brief Set current compare method name to statusbar
 * @param [in] nCompMethod compare method to show
 */
void CDirFrame::SetCompareMethodStatusDisplay(int nCompMethod)
{
	m_wndStatusBar.SetPaneText(PANE_COMPMETHOD, I18n::LoadString(IDS_COMPMETHOD_FULL_CONTENTS + nCompMethod).c_str());
}

/**
 * @brief Set active filter name to statusbar
 * @param [in] szFilter Filtername to show
 */
void CDirFrame::SetFilterStatusDisplay(const tchar_t* szFilter)
{
	m_wndStatusBar.SetPaneText(PANE_FILTER, szFilter);
}

/**
 * @brief Restore maximized state of directory compare window
 */
void CDirFrame::ActivateFrame(int nCmdShow) 
{
	__super::ActivateFrame(nCmdShow);
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CDirFrame::UpdateResources()
{
}

void CDirFrame::OnClose() 
{	
	__super::OnClose();
}

/**
 * @brief Save maximized state before destroying window
 */
BOOL CDirFrame::DestroyWindow() 
{
	HideProgressBar();
	HideFilterBar();
	// save docking positions and sizes
	CDockState dockState;
	GetDockState(dockState);
	dockState.SaveState(_T("Settings-DirFrame"));
	SaveWindowState();
	return __super::DestroyWindow();
}

void CDirFrame::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize(nType, cx, cy);
	
	m_wndFilePathBar.Resize();
}

void CDirFrame::ShowProgressBar()
{
	if (m_pCmpProgressBar == nullptr)
		m_pCmpProgressBar.reset(new DirCompProgressBar());

	if (!::IsWindow(m_pCmpProgressBar->GetSafeHwnd()))
		m_pCmpProgressBar->Create(this);

	ShowControlBar(m_pCmpProgressBar.get(), TRUE, FALSE);
}

void CDirFrame::HideProgressBar()
{
	if (m_pCmpProgressBar != nullptr && ::IsWindow(m_pCmpProgressBar->GetSafeHwnd()))
	{
		ShowControlBar(m_pCmpProgressBar.get(), FALSE, FALSE);
		m_pCmpProgressBar->DestroyWindow();
	}
	m_pCmpProgressBar.reset();
}

void CDirFrame::OnViewDisplayFilterBar()
{
	if (!m_pDirFilterBar)
		ShowFilterBar();
	else
		HideFilterBar();
}

void CDirFrame::OnUpdateDisplayViewFilterBar(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_pDirFilterBar != nullptr);
}

void CDirFrame::OnDisplayFilterBarClose()
{
	HideFilterBar();
	GetActiveView()->SetFocus();
}

void CDirFrame::OnDisplayFilterBarMaskMenu()
{
	m_pDirFilterBar->ShowFilterMaskMenu();
}

void CDirFrame::ShowFilterBar()
{
	if (!m_pDirFilterBar)
		m_pDirFilterBar.reset(new CDirFilterBar());
	if (!::IsWindow(m_pDirFilterBar->GetSafeHwnd()) && !m_pDirFilterBar->Create(this))
	{
		TRACE0("Failed to create filter bar\n");
		m_pDirFilterBar.reset();
		return;
	}
	ShowControlBar(m_pDirFilterBar.get(), TRUE, FALSE);
}

void CDirFrame::HideFilterBar()
{
	if (m_pDirFilterBar != nullptr && ::IsWindow(m_pDirFilterBar->GetSafeHwnd()))
	{
		ShowControlBar(m_pDirFilterBar.get(), FALSE, FALSE);
		m_pDirFilterBar->DestroyWindow();
	}
	m_pDirFilterBar.reset();
}

