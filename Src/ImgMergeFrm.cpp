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
 * @file  ImgMergeFrm.cpp
 *
 * @brief Implementation file for CImgMergeFrame
 *
 */
// ID line follows -- this is updated by SVN
// $Id: ImgMergeFrm.cpp 7166 2010-05-16 12:05:13Z jtuc $

#include "stdafx.h"
#include "ImgMergeFrm.h"
#include "Merge.h"
#include "MainFrm.h"
#include "BCMenu.h"
#include "DirDoc.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsDiffColors.h"
#include "OptionsCustomColors.h"
#include "paths.h"
#include "PathContext.h"
#include "unicoder.h"
#include "../Externals/winimerge/src/WinIMergeLib.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImgMergeFrame

IMPLEMENT_DYNCREATE(CImgMergeFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CImgMergeFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CImgMergeFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_MDIACTIVATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_MERGE_COMPARE_HEX, OnFileRecompareAsBinary)
	ON_COMMAND(ID_WINDOW_CHANGE_PANE, OnWindowChangePane)
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_MESSAGE(MSG_STORE_PANESIZES, OnStorePaneSizes)
	ON_UPDATE_COMMAND_UI(ID_STATUS_DIFFNUM, OnUpdateStatusNum)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOMNORMAL, OnViewZoomNormal)
	ON_COMMAND(ID_VIEW_SPLITVERTICALLY, OnViewSplitVertically)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SPLITVERTICALLY, OnUpdateViewSplitVertically)
	ON_COMMAND(ID_FIRSTDIFF, OnFirstdiff)
	ON_UPDATE_COMMAND_UI(ID_FIRSTDIFF, OnUpdateFirstdiff)
	ON_COMMAND(ID_LASTDIFF, OnLastdiff)
	ON_UPDATE_COMMAND_UI(ID_LASTDIFF, OnUpdateLastdiff)
	ON_COMMAND(ID_NEXTDIFF, OnNextdiff)
	ON_UPDATE_COMMAND_UI(ID_NEXTDIFF, OnUpdateNextdiff)
	ON_COMMAND(ID_PREVDIFF, OnPrevdiff)
	ON_UPDATE_COMMAND_UI(ID_PREVDIFF, OnUpdatePrevdiff)
	ON_COMMAND(ID_NEXTCONFLICT, OnNextConflict)
	ON_UPDATE_COMMAND_UI(ID_NEXTCONFLICT, OnUpdateNextConflict)
	ON_COMMAND(ID_PREVCONFLICT, OnPrevConflict)
	ON_UPDATE_COMMAND_UI(ID_PREVCONFLICT, OnUpdatePrevConflict)
	ON_COMMAND(ID_IMG_VIEWDIFFERENCES, OnImgViewDifferences)
	ON_UPDATE_COMMAND_UI(ID_IMG_VIEWDIFFERENCES, OnUpdateImgViewDifferences)
	ON_COMMAND_RANGE(ID_IMG_ZOOM_25, ID_IMG_ZOOM_800, OnImgZoom)
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMG_ZOOM_25, ID_IMG_ZOOM_800, OnUpdateImgZoom)
	ON_COMMAND_RANGE(ID_IMG_OVERLAY_NONE, ID_IMG_OVERLAY_ALPHABLEND, OnImgOverlayMode)
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMG_OVERLAY_NONE, ID_IMG_OVERLAY_ALPHABLEND, OnUpdateImgOverlayMode)
	ON_COMMAND_RANGE(ID_IMG_DIFFBLOCKSIZE_1, ID_IMG_DIFFBLOCKSIZE_32, OnImgDiffBlockSize)
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMG_DIFFBLOCKSIZE_1, ID_IMG_DIFFBLOCKSIZE_32, OnUpdateImgDiffBlockSize)
	ON_COMMAND(ID_IMG_PREVPAGE, OnImgPrevPage)
	ON_UPDATE_COMMAND_UI(ID_IMG_PREVPAGE, OnUpdateImgPrevPage)
	ON_COMMAND(ID_IMG_NEXTPAGE, OnImgNextPage)
	ON_UPDATE_COMMAND_UI(ID_IMG_NEXTPAGE, OnUpdateImgNextPage)
	ON_COMMAND(ID_IMG_CURPANE_PREVPAGE, OnImgCurPanePrevPage)
	ON_UPDATE_COMMAND_UI(ID_IMG_CURPANE_PREVPAGE, OnUpdateImgCurPanePrevPage)
	ON_COMMAND(ID_IMG_CURPANE_NEXTPAGE, OnImgCurPaneNextPage)
	ON_UPDATE_COMMAND_UI(ID_IMG_CURPANE_NEXTPAGE, OnUpdateImgCurPaneNextPage)
	ON_COMMAND(ID_IMG_USEBACKCOLOR, OnImgUseBackColor)
	ON_UPDATE_COMMAND_UI(ID_IMG_USEBACKCOLOR, OnUpdateImgUseBackColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMenu CImgMergeFrame::menu;

/////////////////////////////////////////////////////////////////////////////
// CImgMergeFrame construction/destruction

CImgMergeFrame::CImgMergeFrame()
: m_hIdentical(NULL)
, m_hDifferent(NULL)
, m_pDirDoc(NULL)
{
	for (int pane = 0; pane < 3; ++pane)
	{
		m_nBufferType[pane] = BUFFER_NORMAL;
		m_bRO[pane] = false;
	}
}

CImgMergeFrame::~CImgMergeFrame()
{
}

bool CImgMergeFrame::OpenImages(const PathContext& paths, const bool bRO[], int nPane, CMDIFrameWnd *pParent)
{
	m_filePaths = paths;

	for (int pane = 0; pane < paths.GetSize(); ++pane)
	{
		m_bRO[pane] = bRO[pane];
		if (theApp.m_strDescriptions[pane].empty())
			m_nBufferType[pane] = BUFFER_NORMAL;
		else
		{
			m_nBufferType[pane] = BUFFER_NORMAL_NAMED;
			m_strDesc[pane] = theApp.m_strDescriptions[pane];
			theApp.m_strDescriptions[pane].erase();
		}
	}
	SetTitle(NULL);

	LPCTSTR lpszWndClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
			LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1), NULL);

	if (!CMDIChildWnd::Create(lpszWndClass, GetTitle(), WS_OVERLAPPEDWINDOW | WS_CHILD, rectDefault, pParent))
		return false;

	int nCmdShow = SW_SHOW;
	if (theApp.GetProfileInt(_T("Settings"), _T("ActiveFrameMax"), FALSE))
		nCmdShow = SW_SHOWMAXIMIZED;
	ShowWindow(nCmdShow);
	BringToTop(nCmdShow);

	if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST))
		m_pImgMergeWindow->FirstDiff();

	if (nPane < 0)
	{
		nPane = theApp.GetProfileInt(_T("Settings"), _T("ActivePane"), 0);
		if (nPane < 0 || nPane >= m_pImgMergeWindow->GetPaneCount())
			nPane = 0;
	}

	m_pImgMergeWindow->SetActivePane(nPane);

	return true;
}

/**
 * @brief DirDoc gives us its identity just after it creates us
 */
void CImgMergeFrame::SetDirDoc(CDirDoc * pDirDoc)
{
	ASSERT(pDirDoc && !m_pDirDoc);
	m_pDirDoc = pDirDoc;
}

BOOL CImgMergeFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	CMDIChildWnd::PreCreateWindow(cs);
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return TRUE;
}
	
/**
 * @brief Create a status bar to be associated with a heksedit control
 */
void CImgMergeFrame::CreateImgWndStatusBar(CStatusBar &wndStatusBar, CWnd *pwndPane)
{
	wndStatusBar.Create(pwndPane, WS_CHILD|WS_VISIBLE);
	wndStatusBar.SetIndicators(0, 3);
	wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH, 0);
	wndStatusBar.SetPaneInfo(1, 0, 0, 80);
	wndStatusBar.SetPaneInfo(2, 0, 0, 80);
	wndStatusBar.SetParent(this);
	wndStatusBar.SetWindowPos(&wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CImgMergeFrame::OnChildPaneEvent(const IImgMergeWindow::Event& evt)
{
/*	if (evt.eventType == IImgMergeWindow::CONTEXTMENU)
	{
		CImgMergeFrame *pFrame = reinterpret_cast<CImgMergeFrame *>(evt.userdata);
		BCMenu menu;
		menu.LoadMenu(MAKEINTRESOURCE(IDR_POPUP_IMGMERGEVIEW));
		BCMenu* pPopup = (BCMenu *)menu.GetSubMenu(0);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			evt.x, evt.y, AfxGetMainWnd());
	}
	*/
}

/**
 * @brief Create the splitter, the filename bar, the status bar, and the two views
 */
BOOL CImgMergeFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	HMODULE hModule = LoadLibraryW(L"WinIMerge\\WinIMergeLib.dll");
	if (!hModule)
		return FALSE;

	IImgMergeWindow * (*WinIMerge_CreateWindow)(HINSTANCE hInstance, HWND hWndParent) = 
			(IImgMergeWindow * (*)(HINSTANCE hInstance, HWND hWndParent))GetProcAddress(hModule, "WinIMerge_CreateWindow");
	if (!WinIMerge_CreateWindow || 
		(m_pImgMergeWindow = WinIMerge_CreateWindow(hModule, m_hWnd)) == NULL)
	{
		FreeLibrary(hModule);
		return FALSE;
	}

	COLORSETTINGS colors;
	Options::DiffColors::Load(colors);
	m_pImgMergeWindow->SetDiffColor(colors.clrDiff);
	m_pImgMergeWindow->SetSelDiffColor(colors.clrSelDiff);
	m_pImgMergeWindow->AddEventListener(OnChildPaneEvent, this);
	LoadOptions();
	
	bool bResult;
	if (m_filePaths.GetSize() == 2)
		bResult = m_pImgMergeWindow->OpenImages(ucr::toUTF16(m_filePaths[0]).c_str(), ucr::toUTF16(m_filePaths[1]).c_str());
	else
		bResult = m_pImgMergeWindow->OpenImages(ucr::toUTF16(m_filePaths[0]).c_str(), ucr::toUTF16(m_filePaths[1]).c_str(), ucr::toUTF16(m_filePaths[2]).c_str());

	// Merge frame has a header bar at top
	if (!m_wndFilePathBar.Create(this))
	{
		TRACE0("Failed to create dialog bar\n");
		return FALSE;      // fail to create
	}

	m_wndFilePathBar.SetPaneCount(m_pImgMergeWindow->GetPaneCount());

	for (int nPane = 0; nPane < m_pImgMergeWindow->GetPaneCount(); nPane++)
	{
		m_wndFilePathBar.SetActive(nPane, FALSE);
		CreateImgWndStatusBar(m_wndStatusBar[nPane], CWnd::FromHandle(m_pImgMergeWindow->GetPaneHWND(nPane)));
		UpdateHeaderPath(nPane);
	}

	CSize size = m_wndStatusBar[0].CalcFixedLayout(TRUE, TRUE);
	m_rectBorder.bottom = size.cy;

	m_hIdentical = AfxGetApp()->LoadIcon(IDI_EQUALIMAGE);
	m_hDifferent = AfxGetApp()->LoadIcon(IDI_NOTEQUALIMAGE);

	SetLastCompareResult(m_pImgMergeWindow->GetDiffCount());

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CImgMergeFrame message handlers

/**
 * @brief Handle translation of default messages on the status bar
 */
void CImgMergeFrame::GetMessageString(UINT nID, CString& rMessage) const
{
	// load appropriate string
	const String s = theApp.LoadString(nID);
	if (!AfxExtractSubString(rMessage, s.c_str(), 0))
	{
		// not found
		TRACE1("Warning: no message line prompt for ID 0x%04X.\n", nID);
	}
}

/**
 * @brief Save the window's position, free related resources, and destroy the window
 */
BOOL CImgMergeFrame::DestroyWindow() 
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

	HMODULE hModule = GetModuleHandle(_T("WinIMergeLib.dll"));
	if (hModule)
	{
		bool (*WinIMerge_DestroyWindow)(IImgMergeWindow *) = 
			(bool (*)(IImgMergeWindow *))GetProcAddress(hModule, "WinIMerge_DestroyWindow");
		if (WinIMerge_DestroyWindow)
		{
			WinIMerge_DestroyWindow(m_pImgMergeWindow);
			m_pImgMergeWindow = NULL;
		}
	}

	return CMDIChildWnd::DestroyWindow();
}

void CImgMergeFrame::LoadOptions()
{
	m_pImgMergeWindow->SetShowDifferences(GetOptionsMgr()->GetBool(OPT_CMP_IMG_SHOWDIFFERENCES));
	m_pImgMergeWindow->SetOverlayMode(static_cast<IImgMergeWindow::OVERLAY_MODE>(GetOptionsMgr()->GetInt(OPT_CMP_IMG_OVERLAYMOVE)));
	m_pImgMergeWindow->SetZoom(GetOptionsMgr()->GetInt(OPT_CMP_IMG_ZOOM) / 1000.0);
	m_pImgMergeWindow->SetUseBackColor(GetOptionsMgr()->GetBool(OPT_CMP_IMG_USEBACKCOLOR));
	COLORREF clrBackColor = GetOptionsMgr()->GetInt(OPT_CMP_IMG_BACKCOLOR);
	RGBQUAD backColor = {GetRValue(clrBackColor), GetGValue(clrBackColor), GetBValue(clrBackColor)};
	m_pImgMergeWindow->SetBackColor(backColor);
	m_pImgMergeWindow->SetDiffBlockSize(GetOptionsMgr()->GetInt(OPT_CMP_IMG_DIFFBLOCKSIZE));
}

void CImgMergeFrame::SaveOptions()
{
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_SHOWDIFFERENCES, m_pImgMergeWindow->GetShowDifferences());
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_OVERLAYMOVE, m_pImgMergeWindow->GetOverlayMode());
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_ZOOM, static_cast<int>(m_pImgMergeWindow->GetZoom() * 1000));
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_USEBACKCOLOR, m_pImgMergeWindow->GetUseBackColor());
	RGBQUAD backColor = m_pImgMergeWindow->GetBackColor();
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_BACKCOLOR, static_cast<int>(RGB(backColor.rgbRed, backColor.rgbGreen, backColor.rgbBlue)));
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_DIFFBLOCKSIZE, m_pImgMergeWindow->GetDiffBlockSize());
}
/**
 * @brief Save coordinates of the frame, splitters, and bars
 *
 * @note Do not save the maximized/restored state here. We are interested
 * in the state of the active frame, and maybe this frame is not active
 */
void CImgMergeFrame::SavePosition()
{
	CRect rc;
	GetWindowRect(&rc);
	theApp.WriteProfileInt(_T("Settings"), _T("WLeft"), rc.Width());
	theApp.WriteProfileInt(_T("Settings"), _T("ActivePane"), m_pImgMergeWindow->GetActivePane());
}

void CImgMergeFrame::OnClose() 
{
	// clean up pointers.
	CMDIChildWnd::OnClose();

	GetMainFrame()->ClearStatusbarItemCount();
}

void CImgMergeFrame::OnFileClose() 
{
	OnClose();
}

void CImgMergeFrame::OnFileRecompareAsBinary()
{
	GetMainFrame()->ShowHexMergeDoc(m_pDirDoc, m_filePaths, m_bRO);
	ShowWindow(SW_RESTORE);
	DestroyWindow();
}

void  CImgMergeFrame::OnWindowChangePane() 
{
	m_pImgMergeWindow->SetActivePane((m_pImgMergeWindow->GetActivePane() + 1) % m_pImgMergeWindow->GetPaneCount());
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CImgMergeFrame::UpdateHeaderPath(int pane)
{
	String sText;

	if (m_nBufferType[pane] == BUFFER_UNNAMED ||
		m_nBufferType[pane] == BUFFER_NORMAL_NAMED)
	{
		sText = m_strDesc[pane];
	}
	else
	{
		sText = m_filePaths.GetPath(pane);
		if (m_pDirDoc)
			m_pDirDoc->ApplyDisplayRoot(pane, sText);
	}
/*	if (m_pView[pane]->GetModified())
		sText.insert(0, _T("* "));*/

	m_wndFilePathBar.SetText(pane, sText.c_str());

	SetTitle(NULL);
}

/// update splitting position for panels 1/2 and headerbar and statusbar 
void CImgMergeFrame::UpdateHeaderSizes()
{
	if (IsWindowVisible())
	{
		int w[3];
		int pane;
		CRect rc;
		GetClientRect(&rc);
		if (!m_pImgMergeWindow->GetHorizontalSplit())
		{
			for (pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); pane++)
			{
				RECT rc = m_pImgMergeWindow->GetPaneWindowRect(pane);
				w[pane] = rc.right - rc.left - 4;
				if (w[pane]<1) w[pane]=1; // Perry 2003-01-22 (I don't know why this happens)
			}
		}
		else
		{
			for (pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); pane++)
				w[pane] = rc.Width() / m_pImgMergeWindow->GetPaneCount() - 4;
		}
		// resize controls in header dialog bar
		m_wndFilePathBar.Resize(w);
		rc.top = rc.bottom - m_rectBorder.bottom;
		rc.right = 0;
		for (pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); pane++)
		{
			rc.right += w[pane] + 4 + 2;
			m_wndStatusBar[pane].MoveWindow(&rc);
			rc.left = rc.right;
		}
	}
}

/**
 * @brief Update document filenames to title
 */
void CImgMergeFrame::SetTitle(LPCTSTR lpszTitle)
{
	const TCHAR pszSeparator[] = _T(" - ");
	String sTitle;
	String sFileName[3];

	if (lpszTitle)
		sTitle = lpszTitle;
	else
	{
		for (int nBuffer = 0; nBuffer < m_filePaths.GetSize(); nBuffer++)
		{
			if (!m_strDesc[nBuffer].empty())
				sFileName[nBuffer] = m_strDesc[nBuffer];
			else
			{
				String file;
				String ext;
				paths_SplitFilename(m_filePaths[nBuffer], NULL, &file, &ext);
				sFileName[nBuffer] += file.c_str();
				if (!ext.empty())
				{
					sFileName[nBuffer] += _T(".");
					sFileName[nBuffer] += ext.c_str();
				}
			}
		}
		if (m_filePaths.GetSize() < 3)
		{
			if (sFileName[0] == sFileName[1])
			{
				sTitle = sFileName[0];
				sTitle += _T(" x 2");
			}
			else
			{
				sTitle = sFileName[0];
				sTitle += pszSeparator;
				sTitle += sFileName[1];
			}
		}
		else
		{
			if (sFileName[0] == sFileName[1] && sFileName[0] == sFileName[2])
			{
				sTitle = sFileName[0];
				sTitle += _T(" x 3");
			}
			else
			{
				sTitle = sFileName[0];
				sTitle += pszSeparator;
				sTitle += sFileName[1];
				sTitle += pszSeparator;
				sTitle += sFileName[2];
			}
		}
	}
	CMDIChildWnd::SetTitle(sTitle.c_str());
}

/**
 * @brief Reflect comparison result in window's icon.
 * @param nResult [in] Last comparison result which the application returns.
 */
void CImgMergeFrame::SetLastCompareResult(int nResult)
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

void CImgMergeFrame::UpdateAutoPaneResize()
{
}

void CImgMergeFrame::UpdateSplitter()
{
}

/// Document commanding us to close
void CImgMergeFrame::CloseNow()
{
	SavePosition(); // Save settings before closing!
	MDIActivate();
	MDIDestroy();
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CImgMergeFrame::UpdateResources()
{
}

/**
 * @brief Check for keyboard commands
 */
BOOL CImgMergeFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		// Close window in response to VK_ESCAPE if user has allowed it from options
		if (pMsg->wParam == VK_ESCAPE && GetOptionsMgr()->GetBool(OPT_CLOSE_WITH_ESC))
		{
			PostMessage(WM_CLOSE, 0, 0);
			return false;
		}
	}
	return CMDIChildWnd::PreTranslateMessage(pMsg);
}

void CImgMergeFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	if (m_pImgMergeWindow)
	{
		UpdateHeaderSizes();
		CRect rcPathBar , rcStatusBar;
		m_wndFilePathBar.GetClientRect(&rcPathBar);
		m_wndStatusBar[0].GetClientRect(&rcStatusBar);
		m_pImgMergeWindow->SetWindowRect(CRect(0, rcPathBar.Height(), cx, cy - rcStatusBar.Height()));
	}
}

/**
 * @brief Synchronize control and status bar placements with splitter position,
 * update mod indicators, synchronize scrollbars
 */
void CImgMergeFrame::OnIdleUpdateCmdUI()
{
	if (IsWindowVisible())
	{
		UpdateHeaderSizes();
		for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
		{
			m_wndFilePathBar.SetActive(pane, pane == m_pImgMergeWindow->GetActivePane());
			m_wndStatusBar[pane].SetPaneText(0, 
				string_format(_T("Size:%d x %dpx %dbpp Page:%d/%d Zoom:%d%%"), 
					m_pImgMergeWindow->GetImageWidth(pane),
					m_pImgMergeWindow->GetImageHeight(pane),
					m_pImgMergeWindow->GetImageBitsPerPixel(pane),
					m_pImgMergeWindow->GetCurrentPage(pane) + 1,
					m_pImgMergeWindow->GetPageCount(pane),
					static_cast<int>(m_pImgMergeWindow->GetZoom() * 100)
					).c_str()
					);
		}
	}
	CMDIChildWnd::OnIdleUpdateCmdUI();
}

/**
 * @brief Save pane sizes and positions when one of panes requests it.
 */
LRESULT CImgMergeFrame::OnStorePaneSizes(WPARAM wParam, LPARAM lParam)
{
	SavePosition();
	return 0;
}

void CImgMergeFrame::OnUpdateStatusNum(CCmdUI* pCmdUI) 
{
	TCHAR sIdx[32];
	TCHAR sCnt[32];
	String s;
	const int nDiffs = m_pImgMergeWindow->GetDiffCount();
	
	// Files are identical - show text "Identical"
	if (nDiffs <= 0)
		s = theApp.LoadString(IDS_IDENTICAL);
	
	// There are differences, but no selected diff
	// - show amount of diffs
	else if (m_pImgMergeWindow->GetCurrentDiffIndex() < 0)
	{
		s = theApp.LoadString(nDiffs == 1 ? IDS_1_DIFF_FOUND : IDS_NO_DIFF_SEL_FMT);
		string_replace(s, _T("%1"), _itot(nDiffs, sCnt, 10));
	}
	
	// There are differences and diff selected
	// - show diff number and amount of diffs
	else
	{
		s = theApp.LoadString(IDS_DIFF_NUMBER_STATUS_FMT);
		const int signInd = m_pImgMergeWindow->GetCurrentDiffIndex();
		string_replace(s, _T("%1"), _itot(signInd + 1, sIdx, 10));
		string_replace(s, _T("%2"), _itot(nDiffs, sCnt, 10));
	}
	pCmdUI->SetText(s.c_str());
}

/**
 * @brief Called when user selects View/Zoom In from menu.
 */
void CImgMergeFrame::OnViewZoomIn()
{
	m_pImgMergeWindow->SetZoom(m_pImgMergeWindow->GetZoom() + 0.1);
}

/**
 * @brief Called when user selects View/Zoom Out from menu.
 */
void CImgMergeFrame::OnViewZoomOut()
{
	m_pImgMergeWindow->SetZoom(m_pImgMergeWindow->GetZoom() - 0.1);
}

/**
 * @brief Called when user selects View/Zoom Normal from menu.
 */
void CImgMergeFrame::OnViewZoomNormal()
{
	m_pImgMergeWindow->SetZoom(1.0);
}

/**
 * @brief Split panes vertically
 */
void CImgMergeFrame::OnViewSplitVertically() 
{
	bool bSplitVertically = !m_pImgMergeWindow->GetHorizontalSplit();
	bSplitVertically = !bSplitVertically; // toggle
	GetOptionsMgr()->SaveOption(OPT_SPLIT_HORIZONTALLY, !bSplitVertically);
	m_pImgMergeWindow->SetHorizontalSplit(!bSplitVertically);
}

/**
 * @brief Update "Split Vertically" UI items
 */
void CImgMergeFrame::OnUpdateViewSplitVertically(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(!m_pImgMergeWindow->GetHorizontalSplit());
}

/**
 * @brief Go to first diff
 *
 * Called when user selects "First Difference"
 * @sa CImgMergeFrame::SelectDiff()
 */
void CImgMergeFrame::OnFirstdiff()
{
	m_pImgMergeWindow->FirstDiff();
}

/**
 * @brief Update "First diff" UI items
 */
void CImgMergeFrame::OnUpdateFirstdiff(CCmdUI* pCmdUI)
{
	OnUpdatePrevdiff(pCmdUI);
}

/**
 * @brief Go to last diff
 */
void CImgMergeFrame::OnLastdiff()
{
	m_pImgMergeWindow->LastDiff();
}

/**
 * @brief Update "Last diff" UI items
 */
void CImgMergeFrame::OnUpdateLastdiff(CCmdUI* pCmdUI)
{
	OnUpdateNextdiff(pCmdUI);
}

/**
 * @brief Go to next diff and select it.
 */
void CImgMergeFrame::OnNextdiff()
{
	m_pImgMergeWindow->NextDiff();
}

/**
 * @brief Update "Next diff" UI items
 */
void CImgMergeFrame::OnUpdateNextdiff(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(
		m_pImgMergeWindow->GetNextDiffIndex() >= 0 ||
		(m_pImgMergeWindow->GetDiffCount() > 0 && m_pImgMergeWindow->GetCurrentDiffIndex() == -1)
	);
}

/**
 * @brief Go to previous diff and select it.
 */
void CImgMergeFrame::OnPrevdiff()
{
	m_pImgMergeWindow->PrevDiff();
}

/**
 * @brief Update "Previous diff" UI items
 */
void CImgMergeFrame::OnUpdatePrevdiff(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(
		m_pImgMergeWindow->GetPrevDiffIndex() >= 0 ||
		(m_pImgMergeWindow->GetDiffCount() > 0 && m_pImgMergeWindow->GetCurrentDiffIndex() == -1)
	);
}

/**
 * @brief Go to next conflict and select it.
 */
void CImgMergeFrame::OnNextConflict()
{
	m_pImgMergeWindow->NextConflict();
}

/**
 * @brief Update "Next Conflict" UI items
 */
void CImgMergeFrame::OnUpdateNextConflict(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(
		m_pImgMergeWindow->GetPaneCount() > 2 && (
			m_pImgMergeWindow->GetNextConflictIndex() >= 0 ||
			(m_pImgMergeWindow->GetConflictCount() > 0 && m_pImgMergeWindow->GetCurrentDiffIndex() == -1)
		)
	);
}

/**
 * @brief Go to previous diff and select it.
 */
void CImgMergeFrame::OnPrevConflict()
{
	m_pImgMergeWindow->PrevConflict();
}

/**
 * @brief Update "Previous diff" UI items
 */
void CImgMergeFrame::OnUpdatePrevConflict(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(
		m_pImgMergeWindow->GetPaneCount() > 2 && (
			m_pImgMergeWindow->GetPrevConflictIndex() >= 0 ||
			(m_pImgMergeWindow->GetConflictCount() > 0 && m_pImgMergeWindow->GetCurrentDiffIndex() == -1)
		)
	);
}

void CImgMergeFrame::OnImgViewDifferences()
{
	m_pImgMergeWindow->SetShowDifferences(!m_pImgMergeWindow->GetShowDifferences());
	SaveOptions();
}

void CImgMergeFrame::OnUpdateImgViewDifferences(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_pImgMergeWindow->GetShowDifferences() ? 1 : 0);
}

void CImgMergeFrame::OnImgZoom(UINT nId)
{
	m_pImgMergeWindow->SetZoom(pow(2.0, int(nId - ID_IMG_ZOOM_100)));
	SaveOptions();
}

void CImgMergeFrame::OnUpdateImgZoom(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(pow(2.0, int(pCmdUI->m_nID - ID_IMG_ZOOM_100)) == m_pImgMergeWindow->GetZoom());
}

void CImgMergeFrame::OnImgOverlayMode(UINT nId)
{
	if (nId == ID_IMG_OVERLAY_NONE)
		m_pImgMergeWindow->SetOverlayMode(IImgMergeWindow::OVERLAY_NONE);
	else if (nId == ID_IMG_OVERLAY_XOR)
		m_pImgMergeWindow->SetOverlayMode(IImgMergeWindow::OVERLAY_XOR);
	else if (nId == ID_IMG_OVERLAY_ALPHABLEND)
		m_pImgMergeWindow->SetOverlayMode(IImgMergeWindow::OVERLAY_ALPHABLEND);
	SaveOptions();
}

void CImgMergeFrame::OnUpdateImgOverlayMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio((pCmdUI->m_nID - ID_IMG_OVERLAY_NONE) == m_pImgMergeWindow->GetOverlayMode());
}

void CImgMergeFrame::OnImgDiffBlockSize(UINT nId)
{
	m_pImgMergeWindow->SetDiffBlockSize(1 << (nId - ID_IMG_DIFFBLOCKSIZE_1));
	SaveOptions();
}

void CImgMergeFrame::OnUpdateImgDiffBlockSize(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(1 << (pCmdUI->m_nID - ID_IMG_DIFFBLOCKSIZE_1) == m_pImgMergeWindow->GetDiffBlockSize() );
}

void CImgMergeFrame::OnImgPrevPage()
{
	m_pImgMergeWindow->SetCurrentPageAll(m_pImgMergeWindow->GetCurrentMaxPage() - 1);
	SetLastCompareResult(m_pImgMergeWindow->GetDiffCount());
}

void CImgMergeFrame::OnUpdateImgPrevPage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImgMergeWindow->GetCurrentMaxPage() > 0);
}

void CImgMergeFrame::OnImgNextPage()
{
	m_pImgMergeWindow->SetCurrentPageAll(m_pImgMergeWindow->GetCurrentMaxPage() + 1);
	SetLastCompareResult(m_pImgMergeWindow->GetDiffCount());
}

void CImgMergeFrame::OnUpdateImgNextPage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(
		m_pImgMergeWindow->GetCurrentMaxPage() < m_pImgMergeWindow->GetMaxPageCount() - 1);
}

void CImgMergeFrame::OnImgCurPanePrevPage()
{
	m_pImgMergeWindow->SetCurrentPage(m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetCurrentPage(m_pImgMergeWindow->GetActivePane()) - 1);
	SetLastCompareResult(m_pImgMergeWindow->GetDiffCount());
}

void CImgMergeFrame::OnUpdateImgCurPanePrevPage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImgMergeWindow->GetCurrentPage(m_pImgMergeWindow->GetActivePane()) > 0);
}

void CImgMergeFrame::OnImgCurPaneNextPage()
{
	m_pImgMergeWindow->SetCurrentPage(m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetCurrentPage(m_pImgMergeWindow->GetActivePane()) + 1);
	SetLastCompareResult(m_pImgMergeWindow->GetDiffCount());
}

void CImgMergeFrame::OnUpdateImgCurPaneNextPage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(
		m_pImgMergeWindow->GetCurrentPage(m_pImgMergeWindow->GetActivePane()) < 
		m_pImgMergeWindow->GetPageCount(m_pImgMergeWindow->GetActivePane()) - 1);
}

void CImgMergeFrame::OnImgUseBackColor()
{
	bool bUseBackColor = !m_pImgMergeWindow->GetUseBackColor();
	if (bUseBackColor)
	{
		RGBQUAD backColor = m_pImgMergeWindow->GetBackColor();
		CColorDialog dialog(RGB(backColor.rgbRed, backColor.rgbGreen, backColor.rgbBlue));
		static DWORD dwCustColors[16];
		Options::CustomColors::Load(dwCustColors);
		dialog.m_cc.lpCustColors = dwCustColors;
		if (dialog.DoModal() == IDOK)
		{
			COLORREF clrBackColor = dialog.GetColor();
			RGBQUAD backColor = {GetBValue(clrBackColor), GetGValue(clrBackColor), GetRValue(clrBackColor)};
			m_pImgMergeWindow->SetBackColor(backColor);
			m_pImgMergeWindow->SetUseBackColor(bUseBackColor);
		}
	}
	else
	{
		m_pImgMergeWindow->SetUseBackColor(bUseBackColor);
	}
	SaveOptions();
}

void CImgMergeFrame::OnUpdateImgUseBackColor(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_pImgMergeWindow->GetUseBackColor() ? 1 : 0);
}

