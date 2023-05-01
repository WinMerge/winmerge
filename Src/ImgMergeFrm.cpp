/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ImgMergeFrm.cpp
 *
 * @brief Implementation file for CImgMergeFrame
 *
 */

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
#include "FileOrFolderSelect.h"
#include "UniFile.h"
#include "SaveClosingDlg.h"
#include "SelectPluginDlg.h"
#include "FileLocation.h"
#include "Constants.h"
#include "DropHandler.h"
#include "Environment.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CImgMergeFrame

IMPLEMENT_DYNCREATE(CImgMergeFrame, CMergeFrameCommon)

BEGIN_MESSAGE_MAP(CImgMergeFrame, CMergeFrameCommon)
	//{{AFX_MSG_MAP(CImgMergeFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_MDIACTIVATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS ()	
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_MESSAGE(MSG_STORE_PANESIZES, OnStorePaneSizes)
	// [File] menu
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE_LEFT, OnFileSaveLeft)
	ON_COMMAND(ID_FILE_SAVE_MIDDLE, OnFileSaveMiddle)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_MIDDLE, OnUpdateFileSaveMiddle)
	ON_COMMAND(ID_FILE_SAVE_RIGHT, OnFileSaveRight)
	ON_COMMAND(ID_FILE_SAVEAS_LEFT, OnFileSaveAsLeft)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS_MIDDLE, OnUpdateFileSaveAsMiddle)
	ON_COMMAND(ID_FILE_SAVEAS_MIDDLE, OnFileSaveAsMiddle)
	ON_COMMAND(ID_FILE_SAVEAS_RIGHT, OnFileSaveAsRight)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_FILE_LEFT_READONLY, OnFileReadOnlyLeft)
	ON_UPDATE_COMMAND_UI(ID_FILE_LEFT_READONLY, OnUpdateFileReadOnlyLeft)
	ON_COMMAND(ID_FILE_MIDDLE_READONLY, OnFileReadOnlyMiddle)
	ON_UPDATE_COMMAND_UI(ID_FILE_MIDDLE_READONLY, OnUpdateFileReadOnlyMiddle)
	ON_COMMAND(ID_FILE_RIGHT_READONLY, OnFileReadOnlyRight)
	ON_UPDATE_COMMAND_UI(ID_FILE_RIGHT_READONLY, OnUpdateFileReadOnlyRight)
	ON_COMMAND(ID_RESCAN, OnFileReload)
	ON_COMMAND_RANGE(ID_MERGE_COMPARE_TEXT, ID_MERGE_COMPARE_WEBPAGE, OnFileRecompareAs)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MERGE_COMPARE_TEXT, ID_MERGE_COMPARE_WEBPAGE, OnUpdateFileRecompareAs)
	// [Edit] menu
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	// [View] menu
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCATION_BAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_LOCATION_BAR, OnBarCheck)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOMNORMAL, OnViewZoomNormal)
	ON_COMMAND(ID_VIEW_SPLITVERTICALLY, OnViewSplitVertically)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SPLITVERTICALLY, OnUpdateViewSplitVertically)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	// [Merge] menu
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
	ON_COMMAND(ID_L2R, OnL2r)
	ON_UPDATE_COMMAND_UI(ID_L2R, OnUpdateL2r)
	ON_COMMAND(ID_R2L, OnR2l)
	ON_UPDATE_COMMAND_UI(ID_R2L, OnUpdateR2l)
	ON_UPDATE_COMMAND_UI(ID_R2LNEXT, OnUpdateR2LNext)
	ON_UPDATE_COMMAND_UI(ID_L2RNEXT, OnUpdateL2RNext)
	ON_COMMAND(ID_COPY_FROM_LEFT, OnCopyFromLeft)
	ON_UPDATE_COMMAND_UI(ID_COPY_FROM_LEFT, OnUpdateCopyFromLeft)
	ON_COMMAND(ID_COPY_FROM_RIGHT, OnCopyFromRight)
	ON_UPDATE_COMMAND_UI(ID_COPY_FROM_RIGHT, OnUpdateCopyFromRight)
	ON_COMMAND(ID_ALL_LEFT, OnAllLeft)
	ON_UPDATE_COMMAND_UI(ID_ALL_LEFT, OnUpdateAllLeft)
	ON_COMMAND(ID_ALL_RIGHT, OnAllRight)
	ON_UPDATE_COMMAND_UI(ID_ALL_RIGHT, OnUpdateAllRight)
	ON_COMMAND_RANGE(ID_COPY_TO_MIDDLE_L, ID_COPY_FROM_LEFT_R, OnCopyX2Y)
	ON_UPDATE_COMMAND_UI_RANGE(ID_COPY_TO_MIDDLE_L, ID_COPY_FROM_LEFT_R, OnUpdateCopyFromXToY)
	ON_COMMAND(ID_AUTO_MERGE, OnAutoMerge)
	ON_UPDATE_COMMAND_UI(ID_AUTO_MERGE, OnUpdateAutoMerge)
	// [Image] menu
	ON_COMMAND(ID_IMG_VIEWDIFFERENCES, OnImgViewDifferences)
	ON_UPDATE_COMMAND_UI(ID_IMG_VIEWDIFFERENCES, OnUpdateImgViewDifferences)
	ON_COMMAND_RANGE(ID_IMG_ZOOM_25, ID_IMG_ZOOM_800, OnImgZoom)
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMG_ZOOM_25, ID_IMG_ZOOM_800, OnUpdateImgZoom)
	ON_COMMAND_RANGE(ID_IMG_OVERLAY_NONE, ID_IMG_OVERLAY_ALPHABLEND_ANIM, OnImgOverlayMode)
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMG_OVERLAY_NONE, ID_IMG_OVERLAY_ALPHABLEND_ANIM, OnUpdateImgOverlayMode)
	ON_COMMAND_RANGE(ID_IMG_DRAGGINGMODE_NONE, ID_IMG_DRAGGINGMODE_RECTANGLE_SELECT, OnImgDraggingMode)
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMG_DRAGGINGMODE_NONE, ID_IMG_DRAGGINGMODE_RECTANGLE_SELECT, OnUpdateImgDraggingMode)
	ON_COMMAND_RANGE(ID_IMG_DIFFBLOCKSIZE_1, ID_IMG_DIFFBLOCKSIZE_32, OnImgDiffBlockSize)
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMG_DIFFBLOCKSIZE_1, ID_IMG_DIFFBLOCKSIZE_32, OnUpdateImgDiffBlockSize)
	ON_COMMAND_RANGE(ID_IMG_THRESHOLD_0, ID_IMG_THRESHOLD_64, OnImgThreshold)
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMG_THRESHOLD_0, ID_IMG_THRESHOLD_64, OnUpdateImgThreshold)
	ON_COMMAND_RANGE(ID_IMG_INSERTIONDELETIONDETECTION_NONE, ID_IMG_INSERTIONDELETIONDETECTION_HORIZONTAL, OnImgInsertionDeletionDetectionMode)
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMG_INSERTIONDELETIONDETECTION_NONE, ID_IMG_INSERTIONDELETIONDETECTION_HORIZONTAL, OnUpdateImgInsertionDeletionDetectionMode)
	ON_COMMAND(ID_IMG_PREVPAGE, OnImgPrevPage)
	ON_UPDATE_COMMAND_UI(ID_IMG_PREVPAGE, OnUpdateImgPrevPage)
	ON_COMMAND(ID_IMG_NEXTPAGE, OnImgNextPage)
	ON_UPDATE_COMMAND_UI(ID_IMG_NEXTPAGE, OnUpdateImgNextPage)
	ON_COMMAND(ID_IMG_CURPANE_ROTATE_RIGHT_90, OnImgCurPaneRotateRight90)
	ON_COMMAND(ID_IMG_CURPANE_ROTATE_LEFT_90, OnImgCurPaneRotateLeft90)
	ON_COMMAND(ID_IMG_CURPANE_FLIP_VERTICALLY, OnImgCurPaneFlipVertically)
	ON_COMMAND(ID_IMG_CURPANE_FLIP_HORIZONTALLY, OnImgCurPaneFlipHorizontally)
	ON_COMMAND(ID_IMG_CURPANE_PREVPAGE, OnImgCurPanePrevPage)
	ON_UPDATE_COMMAND_UI(ID_IMG_CURPANE_PREVPAGE, OnUpdateImgCurPanePrevPage)
	ON_COMMAND(ID_IMG_CURPANE_NEXTPAGE, OnImgCurPaneNextPage)
	ON_UPDATE_COMMAND_UI(ID_IMG_CURPANE_NEXTPAGE, OnUpdateImgCurPaneNextPage)
	ON_COMMAND(ID_IMG_USEBACKCOLOR, OnImgUseBackColor)
	ON_UPDATE_COMMAND_UI(ID_IMG_USEBACKCOLOR, OnUpdateImgUseBackColor)
	ON_COMMAND_RANGE(ID_IMG_VECTORIMAGESCALING_25, ID_IMG_VECTORIMAGESCALING_800, OnImgVectorImageScaling)
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMG_VECTORIMAGESCALING_25, ID_IMG_VECTORIMAGESCALING_800, OnUpdateImgVectorImageScaling)
	ON_COMMAND(ID_IMG_COMPARE_EXTRACTED_TEXT, OnImgCompareExtractedText)
	// [Tools] menu
	ON_COMMAND(ID_TOOLS_GENERATEREPORT, OnToolsGenerateReport)
	// [Plugins] menu
	ON_COMMAND_RANGE(ID_UNPACKERS_FIRST, ID_UNPACKERS_LAST, OnFileRecompareAs)
	ON_COMMAND(ID_OPEN_WITH_UNPACKER, OnOpenWithUnpacker)
	// [Window] menu
	ON_COMMAND_RANGE(ID_NEXT_PANE, ID_PREV_PANE, OnWindowChangePane)
	// [Help] menu
	ON_COMMAND(ID_HELP, OnHelp)
	// Status bar
	ON_UPDATE_COMMAND_UI(ID_STATUS_DIFFNUM, OnUpdateStatusNum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMenu CImgMergeFrame::menu;

/////////////////////////////////////////////////////////////////////////////
// CImgMergeFrame construction/destruction

CImgMergeFrame::CImgMergeFrame()
: CMergeFrameCommon(IDI_EQUALIMAGE, IDI_NOTEQUALIMAGE)
, m_pDirDoc(nullptr)
, m_bAutoMerged(false)
, m_pImgMergeWindow(nullptr)
, m_pImgToolWindow(nullptr)
, m_nBufferType{BUFFERTYPE::NORMAL, BUFFERTYPE::NORMAL, BUFFERTYPE::NORMAL}
, m_bRO{}
, m_nActivePane(-1)
{
}

CImgMergeFrame::~CImgMergeFrame()
{
	GetMainFrame()->UnwatchDocuments(this);

	if (m_pDirDoc != nullptr)
	{
		m_pDirDoc->MergeDocClosing(this);
		m_pDirDoc = nullptr;
	}

	HMODULE hModule = GetModuleHandleW(L"WinIMergeLib.dll");
	if (hModule != nullptr)
	{
		bool (*pfnWinIMerge_DestroyWindow)(IImgMergeWindow *) = 
			(bool (*)(IImgMergeWindow *))GetProcAddress(hModule, "WinIMerge_DestroyWindow");
		bool (*pfnWinIMerge_DestroyToolWindow)(IImgToolWindow *) = 
			(bool (*)(IImgToolWindow *))GetProcAddress(hModule, "WinIMerge_DestroyToolWindow");
		if (pfnWinIMerge_DestroyWindow != nullptr && pfnWinIMerge_DestroyToolWindow != nullptr)
		{
			if (m_pImgMergeWindow != nullptr)
				pfnWinIMerge_DestroyWindow(m_pImgMergeWindow);
			if (m_pImgToolWindow != nullptr)
				pfnWinIMerge_DestroyToolWindow(m_pImgToolWindow);
			m_pImgMergeWindow = nullptr;
			m_pImgToolWindow = nullptr;
		}
	}
}

bool CImgMergeFrame::OpenDocs(int nFiles, const FileLocation fileloc[], const bool bRO[], const String strDesc[], CMDIFrameWnd *pParent)
{
	CWaitCursor waitstatus;
	int nNormalBuffer = 0;
	for (int pane = 0; pane < nFiles; ++pane)
	{
		m_filePaths.SetPath(pane, fileloc[pane].filepath, false);
		m_bRO[pane] = bRO[pane];
		m_strDesc[pane] = strDesc ? strDesc[pane] : _T("");
		if (fileloc[pane].filepath.empty())
			m_nBufferType[pane] = BUFFERTYPE::UNNAMED;
		else
		{
			m_nBufferType[pane] = (!strDesc || strDesc[pane].empty()) ? BUFFERTYPE::NORMAL : BUFFERTYPE::NORMAL_NAMED;
			++nNormalBuffer;
		}
	}
	SetTitle(nullptr);

	const tchar_t* lpszWndClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
			::LoadCursor(nullptr, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1), nullptr);

	if (!CMergeFrameCommon::Create(lpszWndClass, GetTitle(), WS_OVERLAPPEDWINDOW | WS_CHILD, rectDefault, pParent))
		return false;

	int nCmdShow = SW_SHOW;
	if (GetOptionsMgr()->GetBool(OPT_ACTIVE_FRAME_MAX))
		nCmdShow = SW_SHOWMAXIMIZED;
	ShowWindow(nCmdShow);
	BringToTop(nCmdShow);

	GetParent()->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME);

	if (nNormalBuffer > 0)
		OnRefresh();
	else
		UpdateDiffItem(m_pDirDoc);

	if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST))
		m_pImgMergeWindow->FirstDiff();

	GetMainFrame()->WatchDocuments(this);

	return true;
}

void CImgMergeFrame::MoveOnLoad(int nPane, int)
{
	if (nPane < 0)
	{
		nPane = (m_nBufferType[0] != BUFFERTYPE::UNNAMED) ? GetOptionsMgr()->GetInt(OPT_ACTIVE_PANE) : 0;
		if (nPane < 0 || nPane >= m_pImgMergeWindow->GetPaneCount())
			nPane = 0;
	}

	m_pImgMergeWindow->SetActivePane(nPane);
}

void CImgMergeFrame::ChangeFile(int nBuffer, const String& path)
{
	if (!PromptAndSaveIfNeeded(true))
		return;

	for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
		RevokeDragDrop(m_pImgMergeWindow->GetPaneHWND(pane));

	m_filePaths[nBuffer] = path;
	m_nBufferType[nBuffer] = BUFFERTYPE::NORMAL;
	m_strDesc[nBuffer].clear();
	int nActivePane = m_pImgMergeWindow->GetActivePane();

	OpenImages();
	for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
	{
		m_fileInfo[pane].Update(m_filePaths[pane]);

		RegisterDragDrop(m_pImgMergeWindow->GetPaneHWND(pane),
			new DropHandler(std::bind(&CImgMergeFrame::OnDropFiles, this, pane, std::placeholders::_1)));
	}

	MoveOnLoad(nActivePane);

	UpdateHeaderPath(nBuffer);
	UpdateLastCompareResult();
}

bool CImgMergeFrame::IsModified() const
{
	for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
		if (m_pImgMergeWindow->IsModified(pane))
			return true;
	return false;
}

void CImgMergeFrame::DoAutoMerge(int dstPane)
{
	int autoMergedCount = m_pImgMergeWindow->CopyDiff3Way(dstPane);
	if (autoMergedCount > 0)
		m_bAutoMerged = true;

	// move to first conflict 
	m_pImgMergeWindow->FirstConflict();

	AfxMessageBox(
		strutils::format_string2(
			_("The number of automatically merged changes: %1\nThe number of unresolved conflicts: %2"), 
			strutils::format(_T("%d"), autoMergedCount),
			strutils::format(_T("%d"), m_pImgMergeWindow->GetConflictCount())).c_str(),
		MB_ICONINFORMATION);
}

/**
 * @brief DirDoc gives us its identity just after it creates us
 */
void CImgMergeFrame::SetDirDoc(CDirDoc * pDirDoc)
{
	ASSERT(pDirDoc != nullptr && m_pDirDoc == nullptr);
	m_pDirDoc = pDirDoc;
}

IMergeDoc::FileChange CImgMergeFrame::IsFileChangedOnDisk(int pane) const
{
	DiffFileInfo dfi;
	if (!dfi.Update(m_filePaths[pane]))
		return FileChange::Removed;
	int tolerance = 0;
	if (GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME))
		tolerance = SmallTimeDiff; // From MainFrm.h
	int64_t timeDiff = dfi.mtime - m_fileInfo[pane].mtime;
	if (timeDiff < 0) timeDiff = -timeDiff;
	if ((timeDiff > tolerance * Poco::Timestamp::resolution()) || (dfi.size != m_fileInfo[pane].size))
		return FileChange::Changed;
	return FileChange::NoChange;
}

void CImgMergeFrame::CheckFileChanged(void)
{
	if (!m_pImgMergeWindow)
		return;
	for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
	{
		if (IsFileChangedOnDisk(pane) == FileChange::Changed)
		{
			String msg = strutils::format_string1(_("Another application has updated file\n%1\nsince WinMerge scanned it last time.\n\nDo you want to reload the file?"), m_filePaths[pane]);
			if (AfxMessageBox(msg.c_str(), MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN, IDS_FILECHANGED_RESCAN) == IDYES)
			{
				OnFileReload();
			}
			break;
		}
	}
}

/**
 * @brief Create a status bar to be associated with a heksedit control
 */
void CImgMergeFrame::CreateImgWndStatusBar(CStatusBar &wndStatusBar, CWnd *pwndPane)
{
	wndStatusBar.Create(pwndPane, WS_CHILD|WS_VISIBLE);
	wndStatusBar.SetIndicators(0, 1);
	wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH, 0);
	wndStatusBar.SetParent(this);
	wndStatusBar.SetWindowPos(&wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CImgMergeFrame::OnChildPaneEvent(const IImgMergeWindow::Event& evt)
{
	if (evt.eventType == IImgMergeWindow::KEYDOWN)
	{
		CImgMergeFrame *pFrame = reinterpret_cast<CImgMergeFrame *>(evt.userdata);
		switch (evt.keycode)
		{
		case VK_PRIOR:
		case VK_NEXT:
			::SendMessage(pFrame->m_pImgMergeWindow->GetPaneHWND(evt.pane), WM_VSCROLL, evt.keycode == VK_PRIOR ? SB_PAGEUP : SB_PAGEDOWN, 0);
			break;
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			if (::GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				int nActivePane = pFrame->m_pImgMergeWindow->GetActivePane();
				int m = (::GetAsyncKeyState(VK_CONTROL) & 0x8000) ? 8 : 1;
				int dx = (-(evt.keycode == VK_LEFT) + (evt.keycode == VK_RIGHT)) * m;
				int dy = (-(evt.keycode == VK_UP) + (evt.keycode == VK_DOWN)) * m;
				pFrame->m_pImgMergeWindow->AddImageOffset(nActivePane, dx, dy);
			}
			break;
		}
	}
	else if (evt.eventType == IImgMergeWindow::CONTEXTMENU)
	{
		BCMenu menuPopup;
		menuPopup.LoadMenu(MAKEINTRESOURCE(IDR_POPUP_IMG_CTXT));
		theApp.TranslateMenu(menuPopup.m_hMenu);
		BCMenu* pPopup = (BCMenu *)menuPopup.GetSubMenu(0);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			evt.x, evt.y, AfxGetMainWnd());
	}
}

/**
 * @brief returns true if WinIMergeLib.dll is loadable
 */
bool CImgMergeFrame::IsLoadable()
{
	static HMODULE hModule;
	if (hModule == nullptr)
	{
		hModule = LoadLibraryW(L"WinIMerge\\WinIMergeLib.dll");
		if (hModule == nullptr)
			return false;
	}
	return true;
}

/**
 * @brief Create the splitter, the filename bar, the status bar, and the two views
 */
BOOL CImgMergeFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	if (!IsLoadable())
		return FALSE;

	HMODULE hModule = GetModuleHandleW(L"WinIMergeLib.dll");
	if (hModule == nullptr)
		return FALSE;

	IImgMergeWindow * (*pfnWinIMerge_CreateWindow)(HINSTANCE hInstance, HWND hWndParent, int nID) =
		(IImgMergeWindow * (*)(HINSTANCE hInstance, HWND hWndParent, int nID))GetProcAddress(hModule, "WinIMerge_CreateWindow");
	if (pfnWinIMerge_CreateWindow == nullptr ||
		(m_pImgMergeWindow = pfnWinIMerge_CreateWindow(hModule, m_hWnd, AFX_IDW_PANE_FIRST)) == nullptr)
	{
		FreeLibrary(hModule);
		return FALSE;
	}

	m_pImgMergeWindow->AddEventListener(OnChildPaneEvent, this);
	LoadOptions();

	bool bResult;
	if (std::count(m_nBufferType, m_nBufferType + m_filePaths.GetSize(), BUFFERTYPE::UNNAMED) == m_filePaths.GetSize())
	{
		m_infoUnpacker.Initialize(false);
		bResult = m_pImgMergeWindow->NewImages(m_filePaths.GetSize(), 1, 256, 256);
	}
	else
	{
		bResult = OpenImages();
	}

	for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
	{
		m_fileInfo[pane].Update(m_filePaths[pane]);

		RegisterDragDrop(m_pImgMergeWindow->GetPaneHWND(pane),
			new DropHandler(std::bind(&CImgMergeFrame::OnDropFiles, this, pane, std::placeholders::_1)));
	}

	// Merge frame has also a dockable bar at the very left
	// This is not the client area, but we create it now because we want
	// to use the CCreateContext
	String sCaption = theApp.LoadString(IDS_LOCBAR_CAPTION);
	if (!m_wndLocationBar.Create(this, sCaption.c_str(), WS_CHILD | WS_VISIBLE, ID_VIEW_LOCATION_BAR))
	{
		TRACE0("Failed to create LocationBar\n");
		return FALSE;
	}

	IImgToolWindow * (*pfnWinIMerge_CreateToolWindow)(HINSTANCE hInstance, HWND hWndParent, IImgMergeWindow *) =
		(IImgToolWindow * (*)(HINSTANCE hInstance, HWND hWndParent, IImgMergeWindow *pImgMergeWindow))GetProcAddress(hModule, "WinIMerge_CreateToolWindow");
	if (pfnWinIMerge_CreateToolWindow == nullptr ||
		(m_pImgToolWindow = pfnWinIMerge_CreateToolWindow(hModule, m_wndLocationBar.m_hWnd, m_pImgMergeWindow)) == nullptr)
	{
		return FALSE;
	}

	m_pImgToolWindow->Translate(TranslateLocationPane);

	m_wndLocationBar.SetFrameHwnd(GetSafeHwnd());

	return TRUE;
}

void CImgMergeFrame::TranslateLocationPane(int id, const wchar_t *org, size_t dstbufsize, wchar_t *dst)
{
	swprintf_s(dst, dstbufsize, L"%s", tr("ImgMergeFrame|LocationPane", ucr::toUTF8(org)).c_str());
}

/////////////////////////////////////////////////////////////////////////////
// CImgMergeFrame message handlers

int CImgMergeFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMergeFrameCommon::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);

	CMergeFrameCommon::RemoveBarBorder();

	// Merge frame has a header bar at top
	if (!m_wndFilePathBar.Create(this))
	{
		TRACE0("Failed to create dialog bar\n");
		return -1;      // fail to create
	}

	m_wndFilePathBar.SetPaneCount(m_pImgMergeWindow->GetPaneCount());
	m_wndFilePathBar.SetOnSetFocusCallback([&](int pane) {
		if (m_nActivePane != pane)
			m_pImgMergeWindow->SetActivePane(pane);
	});
	m_wndFilePathBar.SetOnCaptionChangedCallback([&](int pane, const String& sText) {
		m_strDesc[pane] = sText;
		UpdateHeaderPath(pane);
	});
	m_wndFilePathBar.SetOnFileSelectedCallback([&](int pane, const String& sFilepath) {
		ChangeFile(pane, sFilepath);
	});

	// Merge frame also has a dockable bar at the very left
	// created in OnCreateClient 
	m_wndLocationBar.SetBarStyle(m_wndLocationBar.GetBarStyle() |
		CBRS_SIZE_DYNAMIC | CBRS_ALIGN_LEFT);
	m_wndLocationBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar(&m_wndLocationBar, AFX_IDW_DOCKBAR_LEFT);

	for (int nPane = 0; nPane < m_pImgMergeWindow->GetPaneCount(); nPane++)
	{
		m_pImgMergeWindow->SetReadOnly(nPane, m_bRO[nPane]);

		m_wndFilePathBar.SetActive(nPane, FALSE);
		CreateImgWndStatusBar(m_wndStatusBar[nPane], CWnd::FromHandle(m_pImgMergeWindow->GetPaneHWND(nPane)));
		UpdateHeaderPath(nPane);
	}

	CSize size = m_wndStatusBar[0].CalcFixedLayout(TRUE, TRUE);
	m_rectBorder.bottom = size.cy;

	CDockState pDockState;
	pDockState.LoadState(_T("Settings-ImgMergeFrame"));
	if (EnsureValidDockState(pDockState)) // checks for valid so won't ASSERT
		SetDockState(pDockState);
	// for the dimensions of the diff and location pane, use the CSizingControlBar loader
	m_wndLocationBar.LoadState(_T("Settings-ImgMergeFrame"));

	return 0;
}

/**
* @brief We must use this function before a call to SetDockState
*
* @note Without this, SetDockState will assert or crash if a bar from the
* CDockState is missing in the current CMergeEditFrame.
* The bars are identified with their ID. This means the missing bar bug is triggered
* when we run WinMerge after changing the ID of a bar.
*/
bool CImgMergeFrame::EnsureValidDockState(CDockState& state)
{
	for (int i = (int)state.m_arrBarInfo.GetSize() - 1; i >= 0; i--)
	{
		bool barIsCorrect = true;
		CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
		if (pInfo == nullptr)
			barIsCorrect = false;
		else
		{
			if (!pInfo->m_bFloating)
			{
				pInfo->m_pBar = GetControlBar(pInfo->m_nBarID);
				if (pInfo->m_pBar == nullptr)
					barIsCorrect = false; //toolbar id's probably changed	
			}
		}

		if (!barIsCorrect)
			state.m_arrBarInfo.RemoveAt(i);
	}
	return true;
}

/**
 * @brief Save the window's position, free related resources, and destroy the window
 */
BOOL CImgMergeFrame::DestroyWindow() 
{
	SavePosition();
	SaveActivePane();
	SaveOptions();
	SaveWindowState();
	CFrameWnd* pParentFrame = GetParentFrame();
	BOOL result = CMergeFrameCommon::DestroyWindow();
	if (pParentFrame)
		pParentFrame->OnUpdateFrameTitle(FALSE);
	return result;
}

void CImgMergeFrame::LoadOptions()
{
	RefreshOptions();

	m_pImgMergeWindow->SetHorizontalSplit(GetOptionsMgr()->GetBool(OPT_SPLIT_HORIZONTALLY));
	m_pImgMergeWindow->SetShowDifferences(GetOptionsMgr()->GetBool(OPT_CMP_IMG_SHOWDIFFERENCES));
	m_pImgMergeWindow->SetOverlayMode(static_cast<IImgMergeWindow::OVERLAY_MODE>(GetOptionsMgr()->GetInt(OPT_CMP_IMG_OVERLAYMODE)));
	m_pImgMergeWindow->SetOverlayAlpha(GetOptionsMgr()->GetInt(OPT_CMP_IMG_OVERLAYALPHA) / 100.0);
	m_pImgMergeWindow->SetDraggingMode(static_cast<IImgMergeWindow::DRAGGING_MODE>(GetOptionsMgr()->GetInt(OPT_CMP_IMG_DRAGGING_MODE)));
	m_pImgMergeWindow->SetZoom(GetOptionsMgr()->GetInt(OPT_CMP_IMG_ZOOM) / 1000.0);
	m_pImgMergeWindow->SetUseBackColor(GetOptionsMgr()->GetBool(OPT_CMP_IMG_USEBACKCOLOR));
	COLORREF clrBackColor = GetOptionsMgr()->GetInt(OPT_CMP_IMG_BACKCOLOR);
	RGBQUAD backColor = {GetBValue(clrBackColor), GetGValue(clrBackColor), GetRValue(clrBackColor)};
	m_pImgMergeWindow->SetBackColor(backColor);
	m_pImgMergeWindow->SetDiffBlockSize(GetOptionsMgr()->GetInt(OPT_CMP_IMG_DIFFBLOCKSIZE));
	m_pImgMergeWindow->SetDiffColorAlpha(GetOptionsMgr()->GetInt(OPT_CMP_IMG_DIFFCOLORALPHA) / 100.0);
	m_pImgMergeWindow->SetColorDistanceThreshold(GetOptionsMgr()->GetInt(OPT_CMP_IMG_THRESHOLD) / 1000.0);
	m_pImgMergeWindow->SetInsertionDeletionDetectionMode(static_cast<IImgMergeWindow::INSERTION_DELETION_DETECTION_MODE>(GetOptionsMgr()->GetInt(OPT_CMP_IMG_INSERTIONDELETIONDETECTION_MODE)));
	m_pImgMergeWindow->SetVectorImageZoomRatio(GetOptionsMgr()->GetInt(OPT_CMP_IMG_VECTOR_IMAGE_ZOOM_RATIO) / 1000.0f);
}

void CImgMergeFrame::SaveOptions()
{
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_SHOWDIFFERENCES, m_pImgMergeWindow->GetShowDifferences());
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_OVERLAYMODE, m_pImgMergeWindow->GetOverlayMode());
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_OVERLAYALPHA, static_cast<int>(m_pImgMergeWindow->GetOverlayAlpha() * 100));
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_DRAGGING_MODE, static_cast<int>(m_pImgMergeWindow->GetDraggingMode()));
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_ZOOM, static_cast<int>(m_pImgMergeWindow->GetZoom() * 1000));
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_USEBACKCOLOR, m_pImgMergeWindow->GetUseBackColor());
	RGBQUAD backColor = m_pImgMergeWindow->GetBackColor();
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_BACKCOLOR, static_cast<int>(RGB(backColor.rgbRed, backColor.rgbGreen, backColor.rgbBlue)));
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_DIFFBLOCKSIZE, m_pImgMergeWindow->GetDiffBlockSize());
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_DIFFCOLORALPHA, static_cast<int>(m_pImgMergeWindow->GetDiffColorAlpha() * 100.0));
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_THRESHOLD, static_cast<int>(m_pImgMergeWindow->GetColorDistanceThreshold() * 1000));
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_INSERTIONDELETIONDETECTION_MODE, static_cast<int>(m_pImgMergeWindow->GetInsertionDeletionDetectionMode()));
	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_VECTOR_IMAGE_ZOOM_RATIO, static_cast<int>(m_pImgMergeWindow->GetVectorImageZoomRatio() * 1000));
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

	// save the bars layout
	// save docking positions and sizes
	CDockState m_pDockState;
	GetDockState(m_pDockState);
	m_pDockState.SaveState(_T("Settings-ImgMergeFrame"));
	// for the dimensions of the diff pane, use the CSizingControlBar save
	m_wndLocationBar.SaveState(_T("Settings-ImgMergeFrame"));
}

void CImgMergeFrame::SaveActivePane()
{
	GetOptionsMgr()->SaveOption(OPT_ACTIVE_PANE, m_pImgMergeWindow->GetActivePane());
}

void CImgMergeFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMergeFrameCommon::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	if (bActivate)
	{
		GetMainFrame()->PostMessage(WM_USER + 1);
	}
}

void CImgMergeFrame::OnClose() 
{
	// Allow user to cancel closing
	if (!PromptAndSaveIfNeeded(true))
		return;

	// clean up pointers.
	CMergeFrameCommon::OnClose();
}

void CImgMergeFrame::OnDestroy()
{
	if (!m_pImgMergeWindow)
		return;
	for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
		RevokeDragDrop(m_pImgMergeWindow->GetPaneHWND(pane));
}

bool CImgMergeFrame::DoFileSave(int pane)
{
	if (m_pImgMergeWindow->IsModified(pane))
	{
		if (m_nBufferType[pane] == BUFFERTYPE::UNNAMED)
			DoFileSaveAs(pane);
		else
		{
			String filename = ucr::toTString(m_pImgMergeWindow->GetFileName(pane));
			bool bApplyToAll = false;
			if (CMergeApp::HandleReadonlySave(m_filePaths[pane], false, bApplyToAll) == IDCANCEL)
				return false;
			CMergeApp::CreateBackup(false, m_filePaths[pane]);
			int savepoint = m_pImgMergeWindow->GetSavePoint(pane);
			if (!m_pImgMergeWindow->SaveImage(pane))
			{
				String str = strutils::format_string2(_("Saving file failed.\n%1\n%2\nDo you want to:\n\t- use a different filename (Press OK)\n\t- abort the current operation (Press Cancel)?"), filename, GetSysError());
				int answer = AfxMessageBox(str.c_str(), MB_OKCANCEL | MB_ICONWARNING);
				if (answer == IDOK)
					return DoFileSaveAs(pane);
				return false;
			}
			if (filename != m_filePaths[pane])
			{
				if (!m_infoUnpacker.Packing(filename, m_filePaths[pane], m_unpackerSubcodes[pane], { m_filePaths[pane] }))
				{
					// Restore save point
					m_pImgMergeWindow->SetSavePoint(pane, savepoint);

					String str = CMergeApp::GetPackingErrorMessage(pane, m_pImgMergeWindow->GetPaneCount(), m_filePaths[pane], m_infoUnpacker);
					int answer = AfxMessageBox(str.c_str(), MB_OKCANCEL | MB_ICONWARNING);
					if (answer == IDOK)
						return DoFileSaveAs(pane, false);
					return false;
				}
			}
		}
		UpdateDiffItem(m_pDirDoc);
		m_fileInfo[pane].Update(m_filePaths[pane]);
	}
	return true;
}

bool CImgMergeFrame::DoFileSaveAs(int pane, bool packing)
{
	const String &path = m_filePaths.GetPath(pane);
	String strPath;
	String title;
	if (pane == 0)
		title = _("Save Left File As");
	else if (pane == m_pImgMergeWindow->GetPaneCount() - 1)
		title = _("Save Right File As");
	else
		title = _("Save Middle File As");
RETRY:
	if (SelectFile(AfxGetMainWnd()->GetSafeHwnd(), strPath, false, path.c_str(), title))
	{
		std::wstring filename = ucr::toUTF16(strPath);
		if (packing && !m_infoUnpacker.GetPluginPipeline().empty())
		{
			String tempPath = env::GetTemporaryPath();
			filename = ucr::toUTF16(env::GetTemporaryFileName(tempPath, _T("MRG_"), 0)
				+ paths::FindExtension(m_pImgMergeWindow->GetFileName(pane)));
		}
		int savepoint = m_pImgMergeWindow->GetSavePoint(pane);
		if (!m_pImgMergeWindow->SaveImageAs(pane, filename.c_str()))
		{
			String str = strutils::format_string2(_("Saving file failed.\n%1\n%2\nDo you want to:\n\t- use a different filename (Press OK)\n\t- abort the current operation (Press Cancel)?"), strPath, GetSysError());
			int answer = AfxMessageBox(str.c_str(), MB_OKCANCEL | MB_ICONWARNING);
			if (answer == IDOK)
				goto RETRY;
			return false;
		}
		if (filename != strPath)
		{
			if (!m_infoUnpacker.Packing(filename, strPath, m_unpackerSubcodes[pane], { strPath }))
			{
				// Restore save point
				m_pImgMergeWindow->SetSavePoint(pane, savepoint);

				String str = CMergeApp::GetPackingErrorMessage(pane, m_pImgMergeWindow->GetPaneCount(), strPath, m_infoUnpacker);
				int answer = AfxMessageBox(str.c_str(), MB_OKCANCEL | MB_ICONWARNING);
				if (answer == IDOK)
					return DoFileSaveAs(pane, false);
				return false;
			}
		}
		if (path.empty())
		{
			// We are saving scratchpad (unnamed file)
			m_nBufferType[pane] = BUFFERTYPE::UNNAMED_SAVED;
			m_strDesc[pane].erase();
		}

		m_filePaths.SetPath(pane, strPath);
		UpdateDiffItem(m_pDirDoc);
		m_fileInfo[pane].Update(m_filePaths[pane]);
		UpdateHeaderPath(pane);
	}
	return true;
}

/**
 * @brief Saves both files
 */
void CImgMergeFrame::OnFileSave() 
{
	for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
		DoFileSave(pane);
}

/**
 * @brief Called when "Save" item is updated
 */
void CImgMergeFrame::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsModified());
}

/**
 * @brief Saves left-side file
 */
void CImgMergeFrame::OnFileSaveLeft() 
{
	DoFileSave(0);
}

/**
 * @brief Called when "Save middle (...)" item is updated
 */
void CImgMergeFrame::OnUpdateFileSaveMiddle(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImgMergeWindow->GetPaneCount() == 3 ? true : false);
}

/**
 * @brief Saves middle-side file
 */
void CImgMergeFrame::OnFileSaveMiddle()
{
	DoFileSave(1);
}

/**
 * @brief Saves right-side file
 */
void CImgMergeFrame::OnFileSaveRight()
{
	DoFileSave(m_pImgMergeWindow->GetPaneCount() - 1);
}

/**
 * @brief Called when "Save middle (as...)" item is updated
 */
void CImgMergeFrame::OnUpdateFileSaveAsMiddle(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImgMergeWindow->GetPaneCount() == 3 ? true : false);
}

/**
 * @brief Saves left-side file with name asked
 */
void CImgMergeFrame::OnFileSaveAsLeft()
{
	DoFileSaveAs(0);
}

/**
 * @brief Saves middle-side file with name asked
 */
void CImgMergeFrame::OnFileSaveAsMiddle()
{
	DoFileSaveAs(1);
}

/**
 * @brief Saves right-side file with name asked
 */
void CImgMergeFrame::OnFileSaveAsRight()
{
	DoFileSaveAs(m_pImgMergeWindow->GetPaneCount() - 1);
}

/**
 * @brief Reloads the opened files
 */
void CImgMergeFrame::OnFileReload()
{
	if (!PromptAndSaveIfNeeded(true))
		return;
	int nActivePane = m_pImgMergeWindow->GetActivePane();
	OpenImages();
	MoveOnLoad(nActivePane);
	for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
		m_fileInfo[pane].Update(m_filePaths[pane]);
}

void CImgMergeFrame::OnFileClose() 
{
	OnClose();
}

/**
 * @brief Enable/disable left buffer read-only
 */
void CImgMergeFrame::OnFileReadOnlyLeft()
{
	m_bRO[0] = !m_bRO[0];
	m_pImgMergeWindow->SetReadOnly(0, m_bRO[0]);
}

/**
 * @brief Called when "Left read-only" item is updated
 */
void CImgMergeFrame::OnUpdateFileReadOnlyLeft(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(m_bRO[0]);
}

/**
 * @brief Enable/disable middle buffer read-only
 */
void CImgMergeFrame::OnFileReadOnlyMiddle()
{
	if (m_pImgMergeWindow->GetPaneCount() == 3)
	{
		m_bRO[1] = !m_bRO[1];
		m_pImgMergeWindow->SetReadOnly(1, m_bRO[1]);
	}
}

/**
 * @brief Called when "Middle read-only" item is updated
 */
void CImgMergeFrame::OnUpdateFileReadOnlyMiddle(CCmdUI* pCmdUI)
{
	if (m_pImgMergeWindow->GetPaneCount() < 3)
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
		pCmdUI->SetCheck(m_bRO[1]);
	}
}

/**
 * @brief Enable/disable right buffer read-only
 */
void CImgMergeFrame::OnFileReadOnlyRight()
{
	int pane = m_pImgMergeWindow->GetPaneCount() - 1;
	m_bRO[pane] = !m_bRO[pane];
	m_pImgMergeWindow->SetReadOnly(pane, m_bRO[pane]);
}

/**
 * @brief Called when "Right read-only" item is updated
 */
void CImgMergeFrame::OnUpdateFileReadOnlyRight(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(m_pImgMergeWindow->GetReadOnly(m_pImgMergeWindow->GetPaneCount() - 1));
}

void CImgMergeFrame::OnFileRecompareAs(UINT nID)
{
	PathContext paths = m_filePaths;
	fileopenflags_t dwFlags[3];
	String strDesc[3];
	int nBuffers = m_filePaths.GetSize();
	PackingInfo infoUnpacker(m_infoUnpacker.GetPluginPipeline());

	for (int nBuffer = 0; nBuffer < nBuffers; ++nBuffer)
	{
		dwFlags[nBuffer] = m_bRO[nBuffer] ? FFILEOPEN_READONLY : 0;
		strDesc[nBuffer] = m_strDesc[nBuffer];
	}
	if (ID_UNPACKERS_FIRST <= nID && nID <= ID_UNPACKERS_LAST)
	{
		infoUnpacker.SetPluginPipeline(CMainFrame::GetPluginPipelineByMenuId(nID, FileTransform::UnpackerEventNames, ID_UNPACKERS_FIRST));
		nID = GetOptionsMgr()->GetBool(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE) ? ID_MERGE_COMPARE_IMAGE : -ID_MERGE_COMPARE_IMAGE;
	}

	CloseNow();
	GetMainFrame()->DoFileOrFolderOpen(&paths, dwFlags, strDesc, _T(""),
		GetOptionsMgr()->GetBool(OPT_CMP_INCLUDE_SUBDIRS), nullptr, &infoUnpacker, nullptr, nID);
}

void CImgMergeFrame::OnUpdateFileRecompareAs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(pCmdUI->m_nID != ID_MERGE_COMPARE_IMAGE);
}

void CImgMergeFrame::OnOpenWithUnpacker()
{
	CSelectPluginDlg dlg(m_infoUnpacker.GetPluginPipeline(),
		strutils::join(m_filePaths.begin(), m_filePaths.end(), _T("|")),
		CSelectPluginDlg::PluginType::Unpacker, false);
	if (dlg.DoModal() == IDOK)
	{
		PackingInfo infoUnpacker(dlg.GetPluginPipeline());
		PathContext paths = m_filePaths;
		fileopenflags_t dwFlags[3] = { FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, FFILEOPEN_NOMRU };
		String strDesc[3] = { m_strDesc[0], m_strDesc[1], m_strDesc[2] };
		CloseNow();
		GetMainFrame()->DoFileOrFolderOpen(&paths, dwFlags, strDesc, _T(""),
			GetOptionsMgr()->GetBool(OPT_CMP_INCLUDE_SUBDIRS), nullptr, &infoUnpacker, nullptr,
			GetOptionsMgr()->GetBool(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE) ? ID_MERGE_COMPARE_IMAGE : -ID_MERGE_COMPARE_IMAGE);
	}
}

void  CImgMergeFrame::OnWindowChangePane(UINT nID) 
{
	int npanes = m_pImgMergeWindow->GetPaneCount();
	int pane = m_pImgMergeWindow->GetActivePane();
	pane = (nID == ID_NEXT_PANE) ? ((pane + 1) % npanes) : ((pane + npanes - 1) % npanes);
	m_pImgMergeWindow->SetActivePane(pane);
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CImgMergeFrame::UpdateHeaderPath(int pane)
{
	String sText;

	if (m_nBufferType[pane] == BUFFERTYPE::UNNAMED ||
		m_nBufferType[pane] == BUFFERTYPE::NORMAL_NAMED)
	{
		sText = m_strDesc[pane];
	}
	else
	{
		sText = m_filePaths.GetPath(pane);
		if (m_pDirDoc != nullptr)
			m_pDirDoc->ApplyDisplayRoot(pane, sText);
	}
	if (m_pImgMergeWindow->IsModified(pane))
		sText.insert(0, _T("* "));

	m_wndFilePathBar.SetText(pane, sText.c_str());

	SetTitle(nullptr);
}

/// update splitting position for panels 1/2 and headerbar and statusbar 
void CImgMergeFrame::UpdateHeaderSizes()
{
	if (m_pImgMergeWindow != nullptr)
	{
		const int scrollbarWidth = GetSystemMetrics(SM_CXVSCROLL);
		int w[3];
		CRect rc, rcMergeWindow;
		int nPaneCount = m_pImgMergeWindow->GetPaneCount();
		GetClientRect(&rc);
		::GetWindowRect(m_pImgMergeWindow->GetHWND(), &rcMergeWindow);
		ScreenToClient(rcMergeWindow);
		if (!m_pImgMergeWindow->GetHorizontalSplit())
		{
			for (int pane = 0; pane < nPaneCount; pane++)
			{
				RECT rc1 = m_pImgMergeWindow->GetPaneWindowRect(pane);
				w[pane] = rc1.right - rc1.left - 4;
				if (pane == nPaneCount - 1)
					w[pane] -= scrollbarWidth;
				if (w[pane]<1) w[pane]=1; // Perry 2003-01-22 (I don't know why this happens)
			}
		}
		else
		{
			for (int pane = 0; pane < nPaneCount; pane++)
				w[pane] = (rcMergeWindow.Width() - scrollbarWidth) / nPaneCount - 6;
		}

		if (!std::equal(m_nLastSplitPos, m_nLastSplitPos + nPaneCount - 1, w))
		{
			std::copy_n(w, nPaneCount - 1, m_nLastSplitPos);

			// resize controls in header dialog bar
			m_wndFilePathBar.Resize(w);

			rc.left = rcMergeWindow.left;
			rc.top = rc.bottom - m_rectBorder.bottom;
			rc.right = rc.left;
			for (int pane = 0; pane < nPaneCount; pane++)
			{
				rc.right += w[pane] + 4 + 2;
				m_wndStatusBar[pane].MoveWindow(&rc);
				rc.left = rc.right;
			}
		}
	}
}

/**
 * @brief Update document filenames to title
 */
void CImgMergeFrame::SetTitle(LPCTSTR lpszTitle)
{
	String sTitle = (lpszTitle != nullptr) ? lpszTitle : CMergeFrameCommon::GetTitleString(m_filePaths, m_strDesc, &m_infoUnpacker, nullptr);
	CMergeFrameCommon::SetTitle(sTitle.c_str());
	if (m_hWnd != nullptr)
		SetWindowText(sTitle.c_str());
}

void CImgMergeFrame::UpdateLastCompareResult()
{
	SetLastCompareResult(m_pImgMergeWindow->GetDiffCount() > 0 ? 1 : 0);
}

void CImgMergeFrame::UpdateAutoPaneResize()
{
}

void CImgMergeFrame::UpdateSplitter()
{
}

bool CImgMergeFrame::OpenImages()
{
	bool bResult;
	String filteredFilenames = strutils::join(m_filePaths.begin(), m_filePaths.end(), _T("|"));
	String strTempFileName[3];
	for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
	{
		strTempFileName[pane] = m_filePaths[pane];
		if (!m_infoUnpacker.Unpacking(&m_unpackerSubcodes[pane], strTempFileName[pane], filteredFilenames, { strTempFileName[pane] }))
		{
			//return false;
		}
	}
	if (m_filePaths.GetSize() == 2)
		bResult = m_pImgMergeWindow->OpenImages(ucr::toUTF16(strTempFileName[0]).c_str(), ucr::toUTF16(strTempFileName[1]).c_str());
	else
		bResult = m_pImgMergeWindow->OpenImages(ucr::toUTF16(strTempFileName[0]).c_str(), ucr::toUTF16(strTempFileName[1]).c_str(), ucr::toUTF16(strTempFileName[2]).c_str());
	return bResult;
}

/**
 * @brief Update associated diff item
 */
int CImgMergeFrame::UpdateDiffItem(CDirDoc *pDirDoc)
{
	// If directory compare has results
	if (pDirDoc && pDirDoc->HasDiffs())
	{
// FIXME:
//		const String &pathLeft = m_filePaths.GetLeft();
//		const String &pathRight = m_filePaths.GetRight();
//		CDiffContext &ctxt = const_cast<CDiffContext &>(pDirDoc->GetDiffContext());
//		if (UINT_PTR pos = pDirDoc->FindItemFromPaths(pathLeft, pathRight))
//		{
//			DIFFITEM &di = pDirDoc->GetDiffRefByKey(pos);
//			::UpdateDiffItem(m_nBuffers, di, &ctxt);
//		}
	}
	int result = m_pImgMergeWindow->GetDiffCount() > 0 ? 1 : 0;
	SetLastCompareResult(result != 0);
	return result;
}

/**
 * @brief Asks and then saves modified files.
 *
 * This function saves modified files. Dialog is shown for user to select
 * modified file(s) one wants to save or discard changed. Cancelling of
 * save operation is allowed unless denied by parameter. After successfully
 * save operation file statuses are updated to directory compare.
 * @param [in] bAllowCancel If false "Cancel" button is disabled.
 * @return true if user selected "OK" so next operation can be
 * executed. If false user choosed "Cancel".
 * @note If filename is empty, we assume scratchpads are saved,
 * so instead of filename, description is shown.
 * @todo If we have filename and description for file, what should
 * we do after saving to different filename? Empty description?
 * @todo Parameter @p bAllowCancel is always true in callers - can be removed.
 */
bool CImgMergeFrame::PromptAndSaveIfNeeded(bool bAllowCancel)
{
	bool bLModified = false, bMModified = false, bRModified = false;
	bool result = true;
	bool bLSaveSuccess = false, bMSaveSuccess = false, bRSaveSuccess = false;

	if (m_pImgMergeWindow->GetPaneCount() == 3)
	{
		bLModified = m_pImgMergeWindow->IsModified(0);
		bMModified = m_pImgMergeWindow->IsModified(1);
		bRModified = m_pImgMergeWindow->IsModified(2);
	}
	else
	{
		bLModified = m_pImgMergeWindow->IsModified(0);
		bRModified = m_pImgMergeWindow->IsModified(1);
	}
	if (!bLModified && !bMModified && !bRModified)
		 return true;

	SaveClosingDlg dlg;
	dlg.DoAskFor(bLModified, bMModified, bRModified);
	if (!bAllowCancel)
		dlg.m_bDisableCancel = true;
	if (!m_filePaths.GetLeft().empty())
	{
		if (theApp.m_strSaveAsPath.empty())
			dlg.m_sLeftFile = m_filePaths.GetLeft();
		else
			dlg.m_sLeftFile = theApp.m_strSaveAsPath;
	}
	else
		dlg.m_sLeftFile = m_strDesc[0];
	if (m_pImgMergeWindow->GetPaneCount() == 3)
	{
		if (!m_filePaths.GetMiddle().empty())
		{
			if (theApp.m_strSaveAsPath.empty())
				dlg.m_sMiddleFile = m_filePaths.GetMiddle();
			else
				dlg.m_sMiddleFile = theApp.m_strSaveAsPath;
		}
		else
			dlg.m_sMiddleFile = m_strDesc[1];
	}
	if (!m_filePaths.GetRight().empty())
	{
		if (theApp.m_strSaveAsPath.empty())
			dlg.m_sRightFile = m_filePaths.GetRight();
		else
			dlg.m_sRightFile = theApp.m_strSaveAsPath;
	}
	else
		dlg.m_sRightFile = m_strDesc[m_pImgMergeWindow->GetPaneCount() - 1];

	if (dlg.DoModal() == IDOK)
	{
		if (bLModified && dlg.m_leftSave == SaveClosingDlg::SAVECLOSING_SAVE)
		{
			bLSaveSuccess = DoFileSave(0);
			if (!bLSaveSuccess)
				result = false;
		}

		if (bMModified && dlg.m_middleSave == SaveClosingDlg::SAVECLOSING_SAVE)
		{
			bMSaveSuccess = DoFileSave(1);
			if (!bMSaveSuccess)
				result = false;
		}

		if (bRModified && dlg.m_rightSave == SaveClosingDlg::SAVECLOSING_SAVE)
		{
			bRSaveSuccess = DoFileSave(m_pImgMergeWindow->GetPaneCount() - 1);
			if (!bRSaveSuccess)
				result = false;
		}
	}
	else
	{	
		result = false;
	}

	// If file were modified and saving was successfull,
	// update status on dir view
	if ((bLModified && bLSaveSuccess) || 
	     (bMModified && bMSaveSuccess) ||
		 (bRModified && bRSaveSuccess))
	{
		// If directory compare has results
		if (m_pDirDoc && m_pDirDoc->HasDiffs())
		{
			// FIXME:
		}
	}

	return result;
}

/// Document commanding us to close
bool CImgMergeFrame::CloseNow()
{
	// Allow user to cancel closing
	if (!PromptAndSaveIfNeeded(true))
		return false;

	DestroyWindow();
	return true;
}

/**
 * @brief A string to display as a tooltip for MDITabbar
 */
CString CImgMergeFrame::GetTooltipString() const
{
	return CMergeFrameCommon::GetTooltipString(m_filePaths, m_strDesc, &m_infoUnpacker, nullptr).c_str();
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CImgMergeFrame::UpdateResources()
{
	m_pImgToolWindow->Translate(TranslateLocationPane);
}

void CImgMergeFrame::RefreshOptions()
{
	COLORSETTINGS colors;
	Options::DiffColors::Load(GetOptionsMgr(), colors);
	m_pImgMergeWindow->SetDiffColor(colors.clrDiff);
	m_pImgMergeWindow->SetDiffDeletedColor(colors.clrDiffDeleted);
	m_pImgMergeWindow->SetSelDiffColor(colors.clrSelDiff);
	m_pImgMergeWindow->SetSelDiffDeletedColor(colors.clrSelDiffDeleted);
	m_pImgMergeWindow->SetDiffAlgorithm(static_cast<IImgMergeWindow::DIFF_ALGORITHM>(GetOptionsMgr()->GetInt(OPT_CMP_DIFF_ALGORITHM)));
}

/**
 * @brief Handle some keys when in merging mode
 */
bool CImgMergeFrame::MergeModeKeyDown(MSG* pMsg)
{
	bool bHandled = false;

	// Allow default text selection when SHIFT pressed
	if (::GetAsyncKeyState(VK_SHIFT))
		return false;

	// Allow default editor functions when CTRL pressed
	if (::GetAsyncKeyState(VK_CONTROL))
		return false;

	// If we are in merging mode (merge with cursor keys)
	// handle some keys here
	switch (pMsg->wParam)
	{
	case VK_LEFT:
		OnR2l();
		bHandled = true;
		break;

	case VK_RIGHT:
		OnL2r();
		bHandled = true;
		break;

	case VK_UP:
		OnPrevdiff();
		bHandled = true;
		break;
	case VK_DOWN:
		OnNextdiff();
		bHandled = true;
		break;
	}

	return bHandled;
}
/**
 * @brief Check for keyboard commands
 */
BOOL CImgMergeFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		// If we are in merging mode (merge with cursor keys)
		// handle some keys here
		if (theApp.GetMergingMode())
		{
			bool bHandled = MergeModeKeyDown(pMsg);
			if (bHandled)
				return true;
		}

		// Close window in response to VK_ESCAPE if user has allowed it from options
		if (pMsg->wParam == VK_ESCAPE && GetOptionsMgr()->GetInt(OPT_CLOSE_WITH_ESC) != 0)
		{
			PostMessage(WM_CLOSE, 0, 0);
			return true;
		}
	}
	return CMergeFrameCommon::PreTranslateMessage(pMsg);
}

void CImgMergeFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMergeFrameCommon::OnSize(nType, cx, cy);
	UpdateHeaderSizes();
}

/**
 * @brief Synchronize control and status bar placements with splitter position,
 * update mod indicators, synchronize scrollbars
 */
void CImgMergeFrame::OnIdleUpdateCmdUI()
{
	if (IsWindowVisible())
	{
		POINT pt = {-1, -1}, ptCursor;
		GetCursorPos(&ptCursor);
		for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
		{
			RECT rc;
			::GetWindowRect(m_pImgMergeWindow->GetPaneHWND(pane), &rc);
			if (PtInRect(&rc, ptCursor))
				pt = m_pImgMergeWindow->GetCursorPos(pane);
		}
		
		RGBQUAD color[3];
		for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
			color[pane] = m_pImgMergeWindow->GetPixelColor(pane, pt.x, pt.y);
		double colorDistance01 = m_pImgMergeWindow->GetColorDistance(0, 1, pt.x, pt.y);
		double colorDistance12 = 0;
		if (m_pImgMergeWindow->GetPaneCount() == 3)
			colorDistance12 = m_pImgMergeWindow->GetColorDistance(1, 2, pt.x, pt.y);

		int nActivePane = m_pImgMergeWindow->GetActivePane();
		if (nActivePane != -1)
			m_nActivePane = nActivePane;

		UpdateHeaderSizes();
		for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
		{
			// Update mod indicators
			String ind = m_wndFilePathBar.GetText(pane);
			if (m_pImgMergeWindow->IsModified(pane) ? ind[0] != _T('*') : ind[0] == _T('*'))
				UpdateHeaderPath(pane);

			m_wndFilePathBar.SetActive(pane, pane == nActivePane);
			POINT ptReal;
			String text;
			if (m_pImgMergeWindow->ConvertToRealPos(pane, pt, ptReal))
			{
				text += strutils::format(_("Pt: (%d, %d)  RGBA: (%d, %d, %d, %d)  "), ptReal.x, ptReal.y,
					color[pane].rgbRed, color[pane].rgbGreen, color[pane].rgbBlue, color[pane].rgbReserved);
				if (pane == 1 && m_pImgMergeWindow->GetPaneCount() == 3)
					text += strutils::format(_("Dist: %g, %g  "), colorDistance01, colorDistance12);
				else
					text += strutils::format(_("Dist: %g  "), colorDistance01);
			}
			if (m_pImgMergeWindow->IsRectangleSelectionVisible(pane))
			{
				RECT rc = m_pImgMergeWindow->GetRectangleSelection(pane);
				text += strutils::format(_("Rc: (%d, %d)  "), rc.right - rc.left, rc.bottom - rc.top);
			}
			text += strutils::format(_("Page: %d/%d  Zoom: %d%%  %dx%dpx  %dbpp  "), 
					m_pImgMergeWindow->GetCurrentPage(pane) + 1,
					m_pImgMergeWindow->GetPageCount(pane),
					static_cast<int>(m_pImgMergeWindow->GetZoom() * 100),
					m_pImgMergeWindow->GetImageWidth(pane),
					m_pImgMergeWindow->GetImageHeight(pane),
					m_pImgMergeWindow->GetImageBitsPerPixel(pane)
					);
			bool verticalFlip = m_pImgMergeWindow->GetVerticalFlip(pane);
			bool horizontalFlip = m_pImgMergeWindow->GetHorizontalFlip(pane);
			float angle = m_pImgMergeWindow->GetRotation(pane);
			if (verticalFlip || horizontalFlip)
			{
				String flip;
				if (verticalFlip)
					flip += _T("V");
				if (horizontalFlip)
					flip += _T("H");
				text += strutils::format(_("Flipped: %s  "), flip);
			}
			if (angle > 0)
				text += strutils::format(_("Rotated: %d  "), static_cast<int>(angle));
			m_wndStatusBar[pane].SetPaneText(0, text.c_str());
		}
		UpdateLastCompareResult();
	}
	CMergeFrameCommon::OnIdleUpdateCmdUI();
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
	tchar_t sCnt[32] = { 0 };
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
		_itot_s(nDiffs, sCnt, 10);
		strutils::replace(s, _T("%1"), sCnt);
	}
	
	// There are differences and diff selected
	// - show diff number and amount of diffs
	else
	{
		tchar_t sIdx[32] = { 0 };
		s = theApp.LoadString(IDS_DIFF_NUMBER_STATUS_FMT);
		const int signInd = m_pImgMergeWindow->GetCurrentDiffIndex();
		_itot_s(signInd + 1, sIdx, 10);
		strutils::replace(s, _T("%1"), sIdx);
		_itot_s(nDiffs, sCnt, 10);
		strutils::replace(s, _T("%2"), sCnt);
	}
	pCmdUI->SetText(s.c_str());
}
	
/**
 * @brief Undo last action
 */
void CImgMergeFrame::OnEditUndo()
{
	m_pImgMergeWindow->Undo();
	if (!m_pImgMergeWindow->IsUndoable())
		m_bAutoMerged = false;
	UpdateLastCompareResult();
}

/**
 * @brief Called when "Undo" item is updated
 */
void CImgMergeFrame::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImgMergeWindow->IsUndoable());
}

/**
 * @brief Redo last action
 */
void CImgMergeFrame::OnEditRedo()
{
	m_pImgMergeWindow->Redo();
	UpdateLastCompareResult();
}

/**
 * @brief Called when "Redo" item is updated
 */
void CImgMergeFrame::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImgMergeWindow->IsRedoable());
}

/**
 * @brief Copy current selection to clipboard
 */
void CImgMergeFrame::OnEditCopy()
{
	m_pImgMergeWindow->Copy();
}

/**
 * @brief Called when "Copy" item is updated
 */
void CImgMergeFrame::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImgMergeWindow->IsCopyable());
}

/**
 * @brief Cut current selection to clipboard
 */
void CImgMergeFrame::OnEditCut()
{
	m_pImgMergeWindow->Cut();
}

/**
 * @brief Called when "Cut" item is updated
 */
void CImgMergeFrame::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImgMergeWindow->IsCuttable());
}

/**
 * @brief Paste image from clipboard
 */
void CImgMergeFrame::OnEditPaste()
{
	m_pImgMergeWindow->Paste();
}

/**
 * @brief Called when "Paste" item is updated
 */
void CImgMergeFrame::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImgMergeWindow->IsPastable());
}

/**
 * @brief Select entire image
 */
void CImgMergeFrame::OnEditSelectAll()
{
	m_pImgMergeWindow->SelectAll();
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
	if (m_pImgMergeWindow->GetCurrentDiffIndex() != m_pImgMergeWindow->GetDiffCount() - 1)
		m_pImgMergeWindow->NextDiff();
	else if (m_pImgMergeWindow->GetCurrentMaxPage() != m_pImgMergeWindow->GetMaxPageCount() - 1)
	{
		if (AfxMessageBox(_("Do you want to move to the next page?").c_str(), MB_YESNO | MB_DONT_ASK_AGAIN, IDS_MOVE_TO_NEXTPAGE) == IDYES)
		{
			m_pImgMergeWindow->SetCurrentPageAll(m_pImgMergeWindow->GetCurrentMaxPage() + 1);
			UpdateLastCompareResult();
		}
	}
	else if (m_pDirDoc != nullptr)
		m_pDirDoc->MoveToNextDiff(this);
}

/**
 * @brief Update "Next diff" UI items
 */
void CImgMergeFrame::OnUpdateNextdiff(CCmdUI* pCmdUI)
{
	bool enabled =
		m_pImgMergeWindow->GetCurrentMaxPage() < m_pImgMergeWindow->GetMaxPageCount() - 1 ||
		m_pImgMergeWindow->GetNextDiffIndex() >= 0 ||
		(m_pImgMergeWindow->GetDiffCount() > 0 && m_pImgMergeWindow->GetCurrentDiffIndex() == -1);

	if (!enabled && m_pDirDoc != nullptr)
		enabled = m_pDirDoc->MoveableToNextDiff();

	pCmdUI->Enable(enabled);
}

/**
 * @brief Go to previous diff and select it.
 */
void CImgMergeFrame::OnPrevdiff()
{
	if (m_pImgMergeWindow->GetCurrentDiffIndex() > 0)
	{
		m_pImgMergeWindow->PrevDiff();
	}
	else if (m_pImgMergeWindow->GetCurrentMaxPage() != 0)
	{
		if (AfxMessageBox(_("Do you want to move to the previous page?").c_str(), MB_YESNO | MB_DONT_ASK_AGAIN, IDS_MOVE_TO_PREVPAGE) == IDYES)
		{
			m_pImgMergeWindow->SetCurrentPageAll(m_pImgMergeWindow->GetCurrentMaxPage() - 1);
			UpdateLastCompareResult();
		}
	}
	else if (m_pDirDoc != nullptr)
		m_pDirDoc->MoveToPrevDiff(this);
}

/**
 * @brief Update "Previous diff" UI items
 */
void CImgMergeFrame::OnUpdatePrevdiff(CCmdUI* pCmdUI)
{
	bool enabled =
		m_pImgMergeWindow->GetCurrentMaxPage() > 0 ||
		m_pImgMergeWindow->GetPrevDiffIndex() >= 0 ||
		(m_pImgMergeWindow->GetDiffCount() > 0 && m_pImgMergeWindow->GetCurrentDiffIndex() == -1);

	if (!enabled && m_pDirDoc != nullptr)
		enabled = m_pDirDoc->MoveableToPrevDiff();

	pCmdUI->Enable(enabled);
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

void CImgMergeFrame::OnCopyX2Y(UINT nID)
{
	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(nID, m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetPaneCount());
	if (srcPane >= 0 && dstPane >= 0)
		OnX2Y(srcPane, dstPane);
}

void CImgMergeFrame::OnUpdateCopyFromXToY(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

void CImgMergeFrame::OnUpdateX2Y(CCmdUI* pCmdUI)
{
	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(pCmdUI->m_nID, m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetPaneCount());
	pCmdUI->Enable(m_pImgMergeWindow->GetCurrentDiffIndex() >= 0 && 
		srcPane >= 0 && srcPane <= m_pImgMergeWindow->GetPaneCount() &&
		dstPane >= 0 && dstPane <= m_pImgMergeWindow->GetPaneCount() &&
		!m_bRO[dstPane]
		);
	if (m_pImgMergeWindow->GetPaneCount() > 2)
		CMergeFrameCommon::ChangeMergeMenuText(srcPane, dstPane, pCmdUI);
}

void CImgMergeFrame::OnX2Y(int srcPane, int dstPane)
{
	m_pImgMergeWindow->CopyDiff(m_pImgMergeWindow->GetCurrentDiffIndex(), srcPane, dstPane);
	UpdateLastCompareResult();
}

/**
 * @brief Copy diff from left pane to right pane
 */
void CImgMergeFrame::OnL2r()
{
	OnCopyX2Y(ID_L2R);
}

/**
 * @brief Called when "Copy to Right" item is updated
 */
void CImgMergeFrame::OnUpdateL2r(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

/**
 * @brief Copy diff from right pane to left pane
 */
void CImgMergeFrame::OnR2l()
{
	OnCopyX2Y(ID_R2L);
}

/**
 * @brief Called when "Copy to Left" item is updated
 */
void CImgMergeFrame::OnUpdateR2l(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

/**
 * @brief Update "Copy right and advance" UI item
 */
void CImgMergeFrame::OnUpdateL2RNext(CCmdUI* pCmdUI)
{
	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(pCmdUI->m_nID, m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetPaneCount());
	pCmdUI->Enable(false);
	CMergeFrameCommon::ChangeMergeMenuText(srcPane, dstPane, pCmdUI);
}

/**
 * @brief Update "Copy left and advance" UI item
 */
void CImgMergeFrame::OnUpdateR2LNext(CCmdUI* pCmdUI)
{
	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(pCmdUI->m_nID, m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetPaneCount());
	pCmdUI->Enable(false);
	CMergeFrameCommon::ChangeMergeMenuText(srcPane, dstPane, pCmdUI);
}

void CImgMergeFrame::OnCopyFromLeft()
{
	OnCopyX2Y(ID_COPY_FROM_LEFT);
}

/**
 * @brief Called when "Copy from left" item is updated
 */
void CImgMergeFrame::OnUpdateCopyFromLeft(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

void CImgMergeFrame::OnCopyFromRight()
{
	OnCopyX2Y(ID_COPY_FROM_RIGHT);
}

/**
 * @brief Called when "Copy from right" item is updated
 */
void CImgMergeFrame::OnUpdateCopyFromRight(CCmdUI* pCmdUI)
{
	OnUpdateX2Y(pCmdUI);
}

/**
 * @brief Copy all diffs from right pane to left pane
 */
void CImgMergeFrame::OnAllLeft()
{
	UINT userChoice = 0;
	String msg = _("Are you sure you want to copy all differences to the other file?");
	userChoice = AfxMessageBox(msg.c_str(), MB_YESNO |
		MB_ICONWARNING | MB_DEFBUTTON2 | MB_DONT_ASK_AGAIN, IDS_CONFIRM_COPY_ALL_DIFFS);
	if (userChoice == IDNO)
		return;

	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(ID_ALL_LEFT, m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetPaneCount());

	CWaitCursor waitstatus;

	m_pImgMergeWindow->CopyDiffAll(srcPane, dstPane);
	UpdateLastCompareResult();
}

/**
 * @brief Called when "Copy all to left" item is updated
 */
void CImgMergeFrame::OnUpdateAllLeft(CCmdUI* pCmdUI)
{
	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(pCmdUI->m_nID, m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetPaneCount());

	pCmdUI->Enable(m_pImgMergeWindow->GetDiffCount() > 0 && !m_bRO[dstPane]);
	if (m_pImgMergeWindow->GetPaneCount() > 2)
		CMergeFrameCommon::ChangeMergeMenuText(srcPane, dstPane, pCmdUI);
}

/**
 * @brief Copy all diffs from left pane to right pane
 */
void CImgMergeFrame::OnAllRight()
{
	UINT userChoice = 0;
	String msg = _("Are you sure you want to copy all differences to the other file?");
	userChoice = AfxMessageBox(msg.c_str(), MB_YESNO |
		MB_ICONWARNING | MB_DEFBUTTON2 | MB_DONT_ASK_AGAIN, IDS_CONFIRM_COPY_ALL_DIFFS);
	if (userChoice == IDNO)
		return;

	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(ID_ALL_RIGHT, m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetPaneCount());

	CWaitCursor waitstatus;

	m_pImgMergeWindow->CopyDiffAll(srcPane, dstPane);
	UpdateLastCompareResult();
}

/**
 * @brief Called when "Copy all to right" item is updated
 */
void CImgMergeFrame::OnUpdateAllRight(CCmdUI* pCmdUI)
{
	auto [srcPane, dstPane] = CMergeFrameCommon::MenuIDtoXY(pCmdUI->m_nID, m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetPaneCount());

	pCmdUI->Enable(m_pImgMergeWindow->GetDiffCount() > 0 && !m_bRO[dstPane]);
	if (m_pImgMergeWindow->GetPaneCount() > 2)
		CMergeFrameCommon::ChangeMergeMenuText(srcPane, dstPane, pCmdUI);
}

/**
 * @brief Do Auto merge
 */
void CImgMergeFrame::OnAutoMerge()
{
	int dstPane = m_pImgMergeWindow->GetActivePane();
	
	// Check current pane is not readonly
	if (dstPane < 0 || IsModified() || m_bAutoMerged || m_bRO[dstPane])
		return;

	CWaitCursor waitstatus;

	DoAutoMerge(dstPane);
}

/**
 * @brief Called when "Auto Merge" item is updated
 */
void CImgMergeFrame::OnUpdateAutoMerge(CCmdUI* pCmdUI)
{
	int dstPane = m_pImgMergeWindow->GetActivePane();
	
	pCmdUI->Enable(m_pImgMergeWindow->GetPaneCount() == 3 && 
		dstPane >= 0 && !IsModified() && !m_bAutoMerged && !m_bRO[dstPane]);
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
	else if (nId == ID_IMG_OVERLAY_ALPHABLEND_ANIM)
		m_pImgMergeWindow->SetOverlayMode(IImgMergeWindow::OVERLAY_ALPHABLEND_ANIM);
	SaveOptions();
}

void CImgMergeFrame::OnUpdateImgOverlayMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(static_cast<IImgMergeWindow::OVERLAY_MODE>(pCmdUI->m_nID - ID_IMG_OVERLAY_NONE) == m_pImgMergeWindow->GetOverlayMode());
}

void CImgMergeFrame::OnImgDraggingMode(UINT nId)
{
	m_pImgMergeWindow->SetDraggingMode(static_cast<IImgMergeWindow::DRAGGING_MODE>(nId - ID_IMG_DRAGGINGMODE_NONE));
	SaveOptions();
}

void CImgMergeFrame::OnUpdateImgDraggingMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(static_cast<IImgMergeWindow::DRAGGING_MODE>(pCmdUI->m_nID - ID_IMG_DRAGGINGMODE_NONE) == m_pImgMergeWindow->GetDraggingMode());
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

void CImgMergeFrame::OnImgThreshold(UINT nId)
{
	if (nId == ID_IMG_THRESHOLD_0)
		m_pImgMergeWindow->SetColorDistanceThreshold(0.0);
	else
		m_pImgMergeWindow->SetColorDistanceThreshold((1 << (nId - ID_IMG_THRESHOLD_2)) * 2);
	SaveOptions();
}

void CImgMergeFrame::OnUpdateImgThreshold(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_nID == ID_IMG_THRESHOLD_0)
		pCmdUI->SetRadio(m_pImgMergeWindow->GetColorDistanceThreshold() == 0.0);
	else
		pCmdUI->SetRadio((1 << (pCmdUI->m_nID - ID_IMG_THRESHOLD_2)) * 2 == m_pImgMergeWindow->GetColorDistanceThreshold() );
}

void CImgMergeFrame::OnImgInsertionDeletionDetectionMode(UINT nId)
{
	m_pImgMergeWindow->SetInsertionDeletionDetectionMode(static_cast<IImgMergeWindow::INSERTION_DELETION_DETECTION_MODE>(nId - ID_IMG_INSERTIONDELETIONDETECTION_NONE));
	SaveOptions();
}

void CImgMergeFrame::OnUpdateImgInsertionDeletionDetectionMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(static_cast<unsigned>(m_pImgMergeWindow->GetInsertionDeletionDetectionMode() + ID_IMG_INSERTIONDELETIONDETECTION_NONE) == pCmdUI->m_nID);
}

void CImgMergeFrame::OnImgPrevPage()
{
	m_pImgMergeWindow->SetCurrentPageAll(m_pImgMergeWindow->GetCurrentMaxPage() - 1);
	UpdateLastCompareResult();
}

void CImgMergeFrame::OnUpdateImgPrevPage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImgMergeWindow->GetCurrentMaxPage() > 0);
}

void CImgMergeFrame::OnImgNextPage()
{
	m_pImgMergeWindow->SetCurrentPageAll(m_pImgMergeWindow->GetCurrentMaxPage() + 1);
	UpdateLastCompareResult();
}

void CImgMergeFrame::OnUpdateImgNextPage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(
		m_pImgMergeWindow->GetCurrentMaxPage() < m_pImgMergeWindow->GetMaxPageCount() - 1);
}

void CImgMergeFrame::OnImgCurPaneRotateRight90()
{
	int curPane = m_pImgMergeWindow->GetActivePane();
	m_pImgMergeWindow->SetRotation(curPane, m_pImgMergeWindow->GetRotation(curPane) - 90.f);
	UpdateLastCompareResult();
}

void CImgMergeFrame::OnImgCurPaneRotateLeft90()
{
	int curPane = m_pImgMergeWindow->GetActivePane();
	m_pImgMergeWindow->SetRotation(curPane, m_pImgMergeWindow->GetRotation(curPane) + 90.f);
	UpdateLastCompareResult();
}

void CImgMergeFrame::OnImgCurPaneFlipVertically()
{
	int curPane = m_pImgMergeWindow->GetActivePane();
	m_pImgMergeWindow->SetVerticalFlip(curPane, !m_pImgMergeWindow->GetVerticalFlip(curPane));
	UpdateLastCompareResult();
}

void CImgMergeFrame::OnImgCurPaneFlipHorizontally()
{
	int curPane = m_pImgMergeWindow->GetActivePane();
	m_pImgMergeWindow->SetHorizontalFlip(curPane, !m_pImgMergeWindow->GetHorizontalFlip(curPane));
	UpdateLastCompareResult();
}

void CImgMergeFrame::OnImgCurPanePrevPage()
{
	m_pImgMergeWindow->SetCurrentPage(m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetCurrentPage(m_pImgMergeWindow->GetActivePane()) - 1);
	UpdateLastCompareResult();
}

void CImgMergeFrame::OnUpdateImgCurPanePrevPage(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pImgMergeWindow->GetCurrentPage(m_pImgMergeWindow->GetActivePane()) > 0);
}

void CImgMergeFrame::OnImgCurPaneNextPage()
{
	m_pImgMergeWindow->SetCurrentPage(m_pImgMergeWindow->GetActivePane(), m_pImgMergeWindow->GetCurrentPage(m_pImgMergeWindow->GetActivePane()) + 1);
	UpdateLastCompareResult();
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
		Options::CustomColors::Load(GetOptionsMgr(), dwCustColors);
		dialog.m_cc.lpCustColors = dwCustColors;
		if (dialog.DoModal() == IDOK)
		{
			COLORREF clrBackColor = dialog.GetColor();
			RGBQUAD backColor1 = {GetBValue(clrBackColor), GetGValue(clrBackColor), GetRValue(clrBackColor)};
			m_pImgMergeWindow->SetBackColor(backColor1);
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

void CImgMergeFrame::OnImgVectorImageScaling(UINT nId)
{
	m_pImgMergeWindow->SetVectorImageZoomRatio(
		static_cast<float>(pow(2.0f, int(nId - ID_IMG_VECTORIMAGESCALING_100))));
	SaveOptions();
}

void CImgMergeFrame::OnUpdateImgVectorImageScaling(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(pow(2.0, int(pCmdUI->m_nID - ID_IMG_VECTORIMAGESCALING_100)) == m_pImgMergeWindow->GetVectorImageZoomRatio());
}

void CImgMergeFrame::OnImgCompareExtractedText()
{
	String text[3];
	String desc[3];
	for (int nBuffer = 0; nBuffer < m_filePaths.GetSize(); ++nBuffer)
	{
		BSTR bstr = m_pImgMergeWindow->ExtractTextFromImage(nBuffer,
			m_pImgMergeWindow->GetCurrentPage(nBuffer),
			static_cast<IImgMergeWindow::OCR_RESULT_TYPE>(GetOptionsMgr()->GetInt(OPT_CMP_IMG_OCR_RESULT_TYPE)));
		if (bstr)
		{
			text[nBuffer].assign(bstr, SysStringLen(bstr));
			desc[nBuffer] = m_strDesc[nBuffer].empty() ?
				paths::FindFileName(m_filePaths[nBuffer]) : m_strDesc[nBuffer];
			SysFreeString(bstr);
		}
	}
	GetMainFrame()->ShowTextMergeDoc(m_pDirDoc, m_filePaths.GetSize(), text, desc, _T(".yaml"));
}

bool CImgMergeFrame::GenerateReport(const String& sFileName) const
{
	return GenerateReport(sFileName, true);
}

/**
 * @brief Generate report from file compare results.
 */
bool CImgMergeFrame::GenerateReport(const String& sFileName, bool allPages) const
{
	String imgdir_full, imgdir, path, name, ext;
	String imgfilepath[3];
	std::vector<std::array<String, 3>> diffimg_filename;
	paths::SplitFilename(sFileName, &path, &name, &ext);
	imgdir_full = paths::ConcatPath(path, name) + _T(".files");
	imgdir = paths::FindFileName(imgdir_full);
	paths::CreateIfNeeded(imgdir_full);

	int curPages[3]{};
	for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
		curPages[pane] = m_pImgMergeWindow->GetCurrentPage(pane);
	if (allPages)
	{
		diffimg_filename.resize(m_pImgMergeWindow->GetMaxPageCount());
		for (int page = 0; page < m_pImgMergeWindow->GetMaxPageCount(); ++page)
		{
			m_pImgMergeWindow->SetCurrentPageAll(page);
			for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
			{
				imgfilepath[pane] = ucr::toTString(m_pImgMergeWindow->GetFileName(pane));
				const int curPage = m_pImgMergeWindow->GetCurrentPage(pane) + 1;
				diffimg_filename[page][pane] = strutils::format(_T("%s/%d_%d.png"),
					imgdir, pane + 1, curPage);
				m_pImgMergeWindow->SaveDiffImageAs(pane,
					ucr::toUTF16(strutils::format(_T("%s\\%d_%d.png"),
						imgdir_full, pane + 1, curPage)).c_str());
			}
		}
	}
	else
	{
		diffimg_filename.resize(1);
		for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
		{
			imgfilepath[pane] = ucr::toTString(m_pImgMergeWindow->GetFileName(pane));
			const int curPage = m_pImgMergeWindow->GetCurrentPage(pane) + 1;
			diffimg_filename[0][pane] = strutils::format(_T("%s/%d_%d.png"),
				imgdir, pane + 1, curPage);
			m_pImgMergeWindow->SaveDiffImageAs(pane,
				ucr::toUTF16(strutils::format(_T("%s\\%d_%d.png"),
					imgdir_full, pane + 1, curPage)).c_str());
		}
	}
	for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
		m_pImgMergeWindow->SetCurrentPage(pane, curPages[pane]);

	UniStdioFile file;
	if (!file.Open(sFileName, _T("wt")))
	{
		String errMsg = GetSysError(GetLastError());
		String msg = strutils::format_string1(
			_("Error creating the report:\n%1"), errMsg);
		AfxMessageBox(msg.c_str(), MB_OK | MB_ICONSTOP);
		return false;
	}

	file.SetCodepage(ucr::CP_UTF_8);

	file.WriteString(
		_T("<!DOCTYPE html>\n")
		_T("<html>\n")
		_T("<head>\n")
		_T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n")
		_T("<title>WinMerge Image Compare Report</title>\n")
		_T("<style type=\"text/css\">\n")
		_T("table { table-layout: fixed; width: 100%; height: 100%; border-collapse: collapse; }\n")
		_T("td,th { border: solid 1px black; }\n")
		_T(".title { color: white; background-color: blue; vertical-align: top; padding: 4px 4px; background: linear-gradient(mediumblue, darkblue);}\n")
		_T(".img   { overflow: scroll; text-align: center; }\n")
		_T("</style>\n")
		_T("</head>\n")
		_T("<body>\n")
		_T("<table>\n")
		_T("<tr>\n"));
	for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
		file.WriteString(strutils::format(_T("<th class=\"title\">%s</th>\n"), imgfilepath[pane]));
	file.WriteString(_T("</tr>\n"));
	for (const auto filenames: diffimg_filename)
	{
		file.WriteString(
			_T("<tr>\n"));
		for (int pane = 0; pane < m_pImgMergeWindow->GetPaneCount(); ++pane)
			file.WriteString(
				strutils::format(_T("<td><div class=\"img\"><img src=\"%s\" alt=\"%s\"></div></td>\n"),
					filenames[pane], filenames[pane]));
		file.WriteString(
			_T("</tr>\n"));
	}
	file.WriteString(
		_T("</table>\n")
		_T("</body>\n")
		_T("</html>\n"));
	return true;
}

/**
 * @brief Generate report from file compare results.
 */
void CImgMergeFrame::OnToolsGenerateReport()
{
	String s;
	BOOL allPages = true;
	
#if NTDDI_VERSION >= NTDDI_VISTA
	CFileDialog dlg(FALSE, _T("htm"), nullptr,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_("HTML Files (*.htm,*.html)|*.htm;*.html|All Files (*.*)|*.*||").c_str());
	dlg.AddCheckButton(1001, _("All pages").c_str(), true);
	if (dlg.DoModal() != IDOK)
		return;
	dlg.GetCheckButtonState(1001, allPages);
	s = dlg.GetPathName();
#else
	CString folder;
	if (!SelectFile(AfxGetMainWnd()->GetSafeHwnd(), s, false, folder, _T(""), _("HTML Files (*.htm,*.html)|*.htm;*.html|All Files (*.*)|*.*||"), _T("htm")))
		return;
#endif

	CWaitCursor waitstatus;
	if (GenerateReport(s, allPages))
		LangMessageBox(IDS_REPORT_SUCCESS, MB_OK | MB_ICONINFORMATION);
}

void CImgMergeFrame::OnRefresh()
{
	if (UpdateDiffItem(m_pDirDoc) == 0)
	{
		CMergeFrameCommon::ShowIdenticalMessage(m_filePaths, true,
			[](const tchar_t* msg, UINT flags, UINT id) -> int { return AfxMessageBox(msg, flags, id); });
	}
}

void CImgMergeFrame::OnDropFiles(int pane, const std::vector<String>& files)
{
	if (files.size() > 1 || paths::IsDirectory(files[0]))
	{
		GetMainFrame()->GetDropHandler()->GetCallback()(files);
		return;
	}

	ChangeFile(pane, files[0]);
}

void CImgMergeFrame::OnSetFocus(CWnd* pNewWnd)
{
	if (m_nActivePane != -1)
		m_pImgMergeWindow->SetActivePane(m_nActivePane);
}


/**
 * @brief Open help from mainframe when user presses F1
 */
void CImgMergeFrame::OnHelp()
{
	theApp.ShowHelp(ImgMergeFrameHelpLocation);
}
