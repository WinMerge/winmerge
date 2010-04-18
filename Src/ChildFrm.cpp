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
 * @file  ChildFrm.cpp
 *
 * @brief Implementation file for CChildFrame
 *
 */
// ID line follows -- this is updated by SVN
// $Id: ChildFrm.cpp 7075 2009-12-30 22:57:20Z kimmov $

#include "stdafx.h"
#include "Merge.h"

#include "ChildFrm.h"
#include "MainFrm.h"
#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "LocationView.h"
#include "DiffViewBar.h"
#include "charsets.h"
#include "OptionsDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** @brief RO status panel width */
static UINT RO_PANEL_WIDTH = 40;
/** @brief Encoding status panel width */
static UINT ENCODING_PANEL_WIDTH = 80;
/** @brief EOL type status panel width */
static UINT EOL_PANEL_WIDTH = 60;

static String EolString(const String & sEol);

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_MDIACTIVATE()
	ON_WM_TIMER()
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
	PANE_PANE0_INFO = 0,
	PANE_PANE0_RO,
	PANE_PANE0_EOL,
	PANE_PANE1_INFO,
	PANE_PANE1_RO,
	PANE_PANE1_EOL,
	PANE_PANE2_INFO,
	PANE_PANE2_RO,
	PANE_PANE2_EOL,
};

/**
 * @brief Bottom statusbar panels and indicators
 */
static UINT indicatorsBottom[] =
{
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
};

#define IDT_SAVEPOSITION 2

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

/**
 * @brief Constructor.
 */
CChildFrame::CChildFrame()
#pragma warning(disable:4355) // 'this' : used in base member initializer list
: m_hIdentical(NULL)
, m_hDifferent(NULL)
#pragma warning(default:4355)
{
	for (int pane = 0; pane < countof(m_status); pane++)
	{
		m_status[pane].m_pFrame = this;
		m_status[pane].m_base = PANE_PANE0_INFO + pane * 3;
	}
	m_bActivated = FALSE;
//	m_nLastSplitPos = 0;
	m_pMergeDoc = 0;
}

/**
 * Destructor.
 */
CChildFrame::~CChildFrame()
{
	m_wndDetailBar.setSplitter(0);
}

BOOL CChildFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	CMergeDoc * pDoc = dynamic_cast<CMergeDoc *>(pContext->m_pCurrentDoc);

	// create a splitter with 1 row, 2 columns
	if (!m_wndSplitter.CreateStatic(this, 1, pDoc->m_nBuffers, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL) )
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}

	int pane;
	for (pane = 0; pane < pDoc->m_nBuffers; pane++)
	{
		if (!m_wndSplitter.CreateView(0, pane,
			RUNTIME_CLASS(CMergeEditView), CSize(-1, 200), pContext))
		{
			TRACE1("Failed to create pane%d\n", pane);
			return FALSE;
		}
	}
	
	m_wndSplitter.ResizablePanes(TRUE);
	m_wndSplitter.AutoResizePanes(GetOptionsMgr()->GetBool(OPT_RESIZE_PANES));

	// Merge frame has also a dockable bar at the very left
	// This is not the client area, but we create it now because we want
	// to use the CCreateContext
	String sCaption = theApp.LoadString(IDS_LOCBAR_CAPTION);
	if (!m_wndLocationBar.Create(this, sCaption.c_str(), WS_CHILD | WS_VISIBLE, ID_VIEW_LOCATION_BAR))
	{
		TRACE0("Failed to create LocationBar\n");
		return FALSE;
	}

	CWnd* pWnd = new CLocationView;
	DWORD dwStyle = AFX_WS_DEFAULT_VIEW ;// & ~WS_BORDER;
	pWnd->Create(NULL, NULL, dwStyle, CRect(0,0,40,100), &m_wndLocationBar, 152, pContext);

	// Merge frame has also a dockable bar at the very bottom
	// This is not the client area, but we create it now because we want
	// to use the CCreateContext
	sCaption = theApp.LoadString(IDS_DIFFBAR_CAPTION);
	if (!m_wndDetailBar.Create(this, sCaption.c_str(), WS_CHILD | WS_VISIBLE, ID_VIEW_DETAIL_BAR))
	{
		TRACE0("Failed to create DiffViewBar\n");
		return FALSE;
	}

	// create a splitter with 2 rows, 1 column
	// this is not a vertical scrollable splitter (see MergeDiffDetailView.h)
	if (!m_wndDetailSplitter.CreateStatic(&m_wndDetailBar, pDoc->m_nBuffers, 1, WS_CHILD | WS_VISIBLE | WS_HSCROLL, AFX_IDW_PANE_FIRST+1) )
	{
		TRACE0("Failed to CreateStaticSplitter\n");
		return FALSE;
	}
	for (pane = 0; pane < pDoc->m_nBuffers; pane++)
	{
		// add splitter pane - the default view in column (pane)
		if (!m_wndDetailSplitter.CreateView(pane, 0,
			RUNTIME_CLASS(CMergeDiffDetailView), CSize(-1, 200), pContext))
		{
			TRACE1("Failed to create pane %d\n", pane);
			return FALSE;
		}
	}
	m_wndDetailSplitter.LockBar(TRUE);
	m_wndDetailSplitter.ResizablePanes(TRUE);
	m_wndDetailBar.setSplitter(&m_wndDetailSplitter);

	// stash left & right pointers into the mergedoc
	CMergeEditView * pView[3];
	for (pane = 0; pane < pDoc->m_nBuffers; pane++)
	{
		pView[pane] = (CMergeEditView *)m_wndSplitter.GetPane(0,pane);
		// connect merge views up to display of status info
		pView[pane]->SetStatusInterface(&m_status[pane]);
		pView[pane]->m_nThisPane = pane;
	}
	// tell merge doc about these views
	m_pMergeDoc = dynamic_cast<CMergeDoc *>(pContext->m_pCurrentDoc);
	m_pMergeDoc->SetMergeViews(pView);

	// stash left & right detail pointers into the mergedoc
	CMergeDiffDetailView * pDetail[3];
	for (pane = 0; pane < pDoc->m_nBuffers; pane++)
	{
		pDetail[pane] = (CMergeDiffDetailView *)m_wndDetailSplitter.GetPane(pane,0);
		pDetail[pane]->m_nThisPane = pane;
	}
	// tell merge doc about these views
	m_pMergeDoc->SetMergeDetailViews(pDetail);

	m_wndFilePathBar.SetPaneCount(pDoc->m_nBuffers);
	
	// Set frame window handles so we can post stage changes back
	((CLocationView *)pWnd)->SetFrameHwnd(GetSafeHwnd());
	for (pane = 0; pane < pDoc->m_nBuffers; pane++)
		pDetail[pane]->SetFrameHwnd(GetSafeHwnd());
	m_wndLocationBar.SetFrameHwnd(GetSafeHwnd());
	m_wndDetailBar.SetFrameHwnd(GetSafeHwnd());

	return TRUE;
}

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

	EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM|CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);

	// Merge frame has a header bar at top
	if (!m_wndFilePathBar.Create(this))
	{
		TRACE0("Failed to create dialog bar\n");
		return -1;      // fail to create
	}

	// Set filename bars inactive so colors get initialized
	m_wndFilePathBar.SetActive(0, FALSE);
	m_wndFilePathBar.SetActive(1, FALSE);
	m_wndFilePathBar.SetActive(2, FALSE);

	// Merge frame also has a dockable bar at the very left
	// created in OnCreateClient 
	m_wndLocationBar.SetBarStyle(m_wndLocationBar.GetBarStyle() |
		CBRS_SIZE_DYNAMIC | CBRS_ALIGN_LEFT);
	m_wndLocationBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar(&m_wndLocationBar, AFX_IDW_DOCKBAR_LEFT);

	// Merge frame also has a dockable bar at the very bottom
	// created in OnCreateClient 
	m_wndDetailBar.SetBarStyle(m_wndDetailBar.GetBarStyle() |
		CBRS_SIZE_DYNAMIC | CBRS_ALIGN_TOP);
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

	// Set text to read-only info panes
	// Text is hidden if file is writable
	String sText = theApp.LoadString(IDS_STATUSBAR_READONLY);
	m_wndStatusBar.SetPaneText(PANE_PANE0_RO, sText.c_str(), TRUE);
	m_wndStatusBar.SetPaneText(PANE_PANE1_RO, sText.c_str(), TRUE);
	m_wndStatusBar.SetPaneText(PANE_PANE2_RO, sText.c_str(), TRUE);
	// load active pane column
	int iCol = theApp.GetProfileInt(_T("Settings"), _T("ActivePane"), 0);
	if (iCol < 0 || iCol >= m_wndSplitter.GetColumnCount()) iCol = 0;
	m_wndSplitter.SetActivePane(0, iCol, NULL);

	m_hIdentical = AfxGetApp()->LoadIcon(IDI_EQUALTEXTFILE);
	m_hDifferent = AfxGetApp()->LoadIcon(IDI_NOTEQUALTEXTFILE);

	return 0;
}

/**
 * @brief We must use this function before a call to SetDockState
 *
 * @note Without this, SetDockState will assert or crash if a bar from the 
 * CDockState is missing in the current CChildFrame.
 * The bars are identified with their ID. This means the missing bar bug is triggered
 * when we run WinMerge after changing the ID of a bar. 
 */
BOOL CChildFrame::EnsureValidDockState(CDockState& state) 
{
	for (int i = state.m_arrBarInfo.GetSize()-1 ; i >= 0; i--) 
	{
		BOOL barIsCorrect = TRUE;
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
		if (! pInfo) 
			barIsCorrect = FALSE;
		else
		{
			if (! pInfo->m_bFloating) 
			{
				pInfo->m_pBar = GetControlBar(pInfo->m_nBarID);
				if (!pInfo->m_pBar) 
					barIsCorrect = FALSE; //toolbar id's probably changed	
			}
		}

		if (! barIsCorrect)
			state.m_arrBarInfo.RemoveAt(i);
	}
	return TRUE;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_NCPAINT:
	case WM_PAINT:
		return 0;
	case WM_NCACTIVATE:
		return 1;
	case WM_SIZE:
		if (wParam != SIZE_RESTORED)
			return 0;
	}
	WNDPROC pfnOldWndProc = (WNDPROC)GetProp(hwnd, _T("OldWndProc"));
	return CallWindowProc(pfnOldWndProc, hwnd, uMsg, wParam, lParam);
}

static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	if (lParam == FALSE)
	{
		if (IsWindowVisible(hwnd))
			::SendMessage(hwnd, WM_SETREDRAW, (WPARAM)lParam, 0);
		else
			SetProp(hwnd, _T("Hidden"), (HANDLE)1);
	}
	else
	{
		BOOL bHidden = (BOOL)RemoveProp(hwnd, _T("Hidden"));
		if (!bHidden)
			::SendMessage(hwnd, WM_SETREDRAW, (WPARAM)lParam, 0);
	}
	return TRUE;
}

/**
 * @brief Alternative LockWindowUpdate(hWnd) API.
 * See the comment near the code that calls this function.
 */
static BOOL MyLockWindowUpdate(HWND hwnd)
{
	WNDPROC pfnOldWndProc;

	EnumChildWindows(hwnd, EnumChildProc, FALSE);

	pfnOldWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
	SetProp(hwnd, _T("OldWndProc"), (HANDLE)pfnOldWndProc);
	return TRUE;
}

/**
 * @brief Alternative LockWindowUpdate(NULL) API.
 * See the comment near the code that calls this function.
 */
static BOOL MyUnlockWindowUpdate(HWND hwnd)
{
	WNDPROC pfnOldWndProc = (WNDPROC)RemoveProp(hwnd, _T("OldWndProc"));
	if (pfnOldWndProc)
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)pfnOldWndProc);

	EnumChildWindows(hwnd, EnumChildProc, TRUE);

	return TRUE;
}

/**
 * @brief Handle translation of default messages on the status bar
 */
void CChildFrame::GetMessageString(UINT nID, CString& rMessage) const
{
	// load appropriate string
	const String s = theApp.LoadString(nID);
	if (!AfxExtractSubString(rMessage, &*s.begin(), 0))
	{
		// not found
		TRACE1("Warning: no message line prompt for ID 0x%04X.\n", nID);
	}
}

void CChildFrame::ActivateFrame(int nCmdShow) 
{
	BOOL bMaximized = FALSE;
	CMDIChildWnd * oldActiveFrame = GetMDIFrame()->MDIGetActive(&bMaximized);

	if (!m_bActivated) 
	{
		m_bActivated = TRUE;

		// get the active child frame, and a flag whether it is maximized
		if (oldActiveFrame == NULL)
			// for the first frame, get the restored/maximized state from the registry
			bMaximized = theApp.GetProfileInt(_T("Settings"), _T("ActiveFrameMax"), TRUE);
		if (bMaximized)
			nCmdShow = SW_SHOWMAXIMIZED;
		else
			nCmdShow = SW_SHOWNORMAL;
	}

	// load the bars layout
	// initialize the diff pane state with default dimension
	int initialDiffHeight = ((CMergeDiffDetailView*)m_wndDetailSplitter.GetPane(1,0))->ComputeInitialHeight();
	UpdateDiffDockbarHeight(initialDiffHeight);
	// load docking positions and sizes
	CDockState pDockState;
	pDockState.LoadState(_T("Settings"));
	if (EnsureValidDockState(pDockState)) // checks for valid so won't ASSERT
		SetDockState(pDockState);
	// for the dimensions of the diff and location pane, use the CSizingControlBar loader
	m_wndLocationBar.LoadState(_T("Settings"));
	m_wndDetailBar.LoadState(_T("Settings"));

	if (bMaximized)
	{
		// If ActivateFrame() is called without tricks, Resizing panes in MergeView window could be visible.
		// Here, two tricks are used.
		// [First trick]
		// To complete resizing panes before displaying MergeView window, 
		// it needs to send WM_SIZE message with SIZE_MAXIMIZED to MergeView window before calling ActivateFrame().
		// [Second trick]
		// But it causes side effect that DirView window becomes restored window from maximized window
		// and the process could be visible.
		// To avoid it, it needs to block the redrawing DirView window.
		// I had tried to use LockWindowUpdate for this purpose. However, it had caused flickering desktop icons.
		// So instead of using LockWindowUpdate(), 
		// I wrote My[Lock/Unlock]WindowUpdate() function that uses subclassing window.
		// 
		if (oldActiveFrame)
			MyLockWindowUpdate(oldActiveFrame->m_hWnd);
		
		RECT rc;
		GetClientRect(&rc);
		SendMessage(WM_SIZE, SIZE_MAXIMIZED, MAKELPARAM(rc.right, rc.bottom));

		CMDIChildWnd::ActivateFrame(nCmdShow);

		if (oldActiveFrame)
			MyUnlockWindowUpdate(oldActiveFrame->m_hWnd);
	}
	else
	{
		RecalcLayout();

		CMDIChildWnd::ActivateFrame(nCmdShow);
	}

	if (oldActiveFrame)
		((CChildFrame *)oldActiveFrame)->PostMessage(WM_TIMER);
}

BOOL CChildFrame::DestroyWindow() 
{
	SavePosition();
	// If we are active, save the restored/maximized state
	// If we are not, do nothing and let the active frame do the job.
 	if (this->GetParentFrame()->GetActiveFrame() == (CFrameWnd*)this)
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
void CChildFrame::SavePosition()
{
	CRect rc;
	CWnd* pLeft = m_wndSplitter.GetPane(0,0);
	if (pLeft != NULL)
	{
		pLeft->GetWindowRect(&rc);
		theApp.WriteProfileInt(_T("Settings"), _T("WLeft"), rc.Width());
	}

	// save the bars layout
	// save docking positions and sizes
	CDockState m_pDockState;
	GetDockState(m_pDockState);
	m_pDockState.SaveState(_T("Settings"));
	// for the dimensions of the diff pane, use the CSizingControlBar save
	m_wndLocationBar.SaveState(_T("Settings"));
	m_wndDetailBar.SaveState(_T("Settings"));

	int iCol;
	m_wndSplitter.GetActivePane(NULL, &iCol);
	theApp.WriteProfileInt(_T("Settings"), _T("ActivePane"), iCol);
}

void CChildFrame::OnClose() 
{
	// clean up pointers.
	CMDIChildWnd::OnClose();

	GetMainFrame()->ClearStatusbarItemCount();
}


/// update height for panels 3/4
void CChildFrame::UpdateDiffDockbarHeight(int DiffPanelHeight)
{
	m_wndDetailBar.UpdateBarHeight(DiffPanelHeight);
}

/// update splitting position for panels 1/2 and headerbar and statusbar 
void CChildFrame::UpdateHeaderSizes()
{
	if(!::IsWindow(m_wndFilePathBar.m_hWnd) || !::IsWindow(m_wndSplitter.m_hWnd))
		return;
	
	if(IsWindowVisible())
	{
		m_wndFilePathBar.ShowWindow(m_wndSplitter.GetPane(0, 0)->IsWindowVisible());
		m_wndStatusBar.ShowWindow(m_wndSplitter.GetPane(0, 0)->IsWindowVisible());
				
		int w[3],wmin;
		int pane;
		for (pane = 0; pane < m_wndSplitter.GetColumnCount(); pane++)
		{
			m_wndSplitter.GetColumnInfo(pane, w[pane], wmin);
			if (w[pane]<1) w[pane]=1; // Perry 2003-01-22 (I don't know why this happens)
		}

		// prepare file path bar to look as a status bar
		if (m_wndFilePathBar.LookLikeThisWnd(&m_wndStatusBar) == TRUE)
			RecalcLayout();

		// resize controls in header dialog bar
		m_wndFilePathBar.Resize(w);

		// Set bottom statusbar panel widths
		// Kimmo - I don't know why 4 seems to be right for me
		int borderWidth = 4; // GetSystemMetrics(SM_CXEDGE);
		for (pane = 0; pane < m_wndSplitter.GetColumnCount(); pane++)
		{
			int paneWidth = w[pane] - (RO_PANEL_WIDTH + EOL_PANEL_WIDTH +
				(2 * borderWidth));
			if (paneWidth < borderWidth)
				paneWidth = borderWidth;

			m_wndStatusBar.SetPaneStyle(PANE_PANE0_INFO + pane * 3, SBPS_NORMAL);
			m_wndStatusBar.SetPaneInfo(PANE_PANE0_INFO + pane * 3, ID_STATUS_PANE0FILE_INFO + pane,
				SBPS_NORMAL, paneWidth);
			m_wndStatusBar.SetPaneStyle(PANE_PANE0_RO + pane * 3, SBPS_NORMAL);
			m_wndStatusBar.SetPaneInfo(PANE_PANE0_RO + pane * 3, ID_STATUS_PANE0FILE_RO + pane,
				SBPS_NORMAL, RO_PANEL_WIDTH - borderWidth);
			m_wndStatusBar.SetPaneStyle(PANE_PANE0_EOL + pane * 3, SBPS_NORMAL);
			m_wndStatusBar.SetPaneInfo(PANE_PANE0_EOL + pane * 3, ID_STATUS_PANE0FILE_EOL + pane,
				SBPS_NORMAL, EOL_PANEL_WIDTH - borderWidth);
		}
	}
}

IHeaderBar * CChildFrame::GetHeaderInterface()
{
	return &m_wndFilePathBar;
}

/**
* @brief Reflect comparison result in window's icon.
* @param nResult [in] Last comparison result which the application returns.
*/
void CChildFrame::SetLastCompareResult(int nResult)
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

void CChildFrame::UpdateAutoPaneResize()
{
	m_wndSplitter.AutoResizePanes(GetOptionsMgr()->GetBool(OPT_RESIZE_PANES));
}

void CChildFrame::UpdateSplitter()
{
	m_wndSplitter.RecalcLayout();
	m_wndDetailBar.UpdateBarHeight(0);
}

void CChildFrame::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent == IDT_SAVEPOSITION)
	{
		SavePosition();
		KillTimer(IDT_SAVEPOSITION);
	}
	else
	{
		UpdateHeaderSizes();
	}
	CMDIChildWnd::OnTimer(nIDEvent);
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	CMergeDoc *pDoc = GetMergeDoc();
	if (bActivate && pDoc)
		this->GetParentFrame()->PostMessage(WM_USER+1);
	return;
}

/// Document commanding us to close
void CChildFrame::CloseNow()
{
	SavePosition(); // Save settings before closing!
	MDIActivate();
	MDIDestroy();
}

/// Bridge class which implements the interface from crystal editor to frame status line display
CChildFrame::MergeStatus::MergeStatus()
: m_nColumn(0)
, m_nColumns(0)
, m_nChar(0)
, m_nChars(0)
, m_nCodepage(-1)
{
}

/// Send status line info (about one side of merge view) to screen
void CChildFrame::MergeStatus::Update()
{
	if (IsWindow(m_pFrame->m_wndStatusBar.m_hWnd))
	{
		CString str;
		if (m_nChars == -1)
		{
			str.Format(theApp.LoadString(IDS_EMPTY_LINE_STATUS_INFO).c_str(),
				m_sLine.c_str());
		}
		else if (m_sEolDisplay.empty())
		{
			str.Format(theApp.LoadString(IDS_LINE_STATUS_INFO).c_str(),
				m_sLine.c_str(), m_nColumn, m_nColumns, m_nChar, m_nChars, m_nCodepage, m_sCodepageName.c_str());
		}
		else
		{
			str.Format(theApp.LoadString(IDS_LINE_STATUS_INFO_EOL).c_str(),
				m_sLine.c_str(), m_nColumn, m_nColumns, m_nChar, m_nChars, m_sEolDisplay.c_str(), m_nCodepage, m_sCodepageName.c_str());
		}

		m_pFrame->m_wndStatusBar.SetPaneText(m_base, str);
	}
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CChildFrame::MergeStatus::UpdateResources()
{
	Update();
}

/// Visible representation of eol
static String EolString(const String & sEol)
{
	if (sEol == _T("\r\n"))
	{
		return LoadResString(IDS_EOL_CRLF);
	}
	if (sEol == _T("\n"))
	{
		return LoadResString(IDS_EOL_LF);
	}
	if (sEol == _T("\r"))
	{
		return LoadResString(IDS_EOL_CR);
	}
	if (sEol.empty())
	{
		return LoadResString(IDS_EOL_NONE);
	}
	if (sEol == _T("hidden"))
		return _T("");
	return _T("?");
}

/// Receive status line info from crystal window and display
void CChildFrame::MergeStatus::SetLineInfo(LPCTSTR szLine, int nColumn,
		int nColumns, int nChar, int nChars, LPCTSTR szEol, int nCodepage)
{
	if (m_sLine != szLine || m_nColumn != nColumn || m_nColumns != nColumns ||
		m_nChar != nChar || m_nChars != nChars || m_sEol != szEol != 0 || m_nCodepage != nCodepage)
	{
		USES_CONVERSION;
		m_sLine = szLine;
		m_nColumn = nColumn;
		m_nColumns = nColumns;
		m_nChar = nChar;
		m_nChars = nChars;
		m_sEol = szEol;
		m_sEolDisplay = EolString(m_sEol);
		if (m_nCodepage != nCodepage)
		{
			const char *pszCodepageName = GetEncodingNameFromCodePage(nCodepage);
			m_sCodepageName = pszCodepageName ? A2CT(pszCodepageName) : _T("");
		}
		m_nCodepage = nCodepage;
		Update();
	}
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CChildFrame::UpdateResources()
{
	for (int pane = 0; pane < m_wndSplitter.GetColumnCount(); pane++)
		m_status[pane].UpdateResources();
	m_wndLocationBar.UpdateResources();
	m_wndDetailBar.UpdateResources();
}

/**
 * @brief Save pane sizes and positions when one of panes requests it.
 */
LRESULT CChildFrame::OnStorePaneSizes(WPARAM wParam, LPARAM lParam)
{
	KillTimer(IDT_SAVEPOSITION);
	SetTimer(IDT_SAVEPOSITION, 300, NULL);
	return 0;
}
