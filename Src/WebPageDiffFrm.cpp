/** 
 * @file  WebPageDiffFrm.cpp
 *
 * @brief Implementation file for CWebPageDiffFrame
 *
 */

#include "stdafx.h"
#include "WebPageDiffFrm.h"
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
#include <wrl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Microsoft::WRL;

/** @brief Location for image compare specific help to open. */
static const TCHAR WebPageDiffFrameHelpLocation[] = _T("::/htmlhelp/Compare_webpages.html");

/////////////////////////////////////////////////////////////////////////////
// CWebPageDiffFrame

IMPLEMENT_DYNCREATE(CWebPageDiffFrame, CMergeFrameCommon)

BEGIN_MESSAGE_MAP(CWebPageDiffFrame, CMergeFrameCommon)
	//{{AFX_MSG_MAP(CWebPageDiffFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_MDIACTIVATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS ()	
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_MESSAGE(MSG_STORE_PANESIZES, OnStorePaneSizes)
	// [File] menu
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_RESCAN, OnFileReload)
	ON_COMMAND_RANGE(ID_MERGE_COMPARE_TEXT, ID_MERGE_COMPARE_WEBPAGE, OnFileRecompareAs)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MERGE_COMPARE_TEXT, ID_MERGE_COMPARE_WEBPAGE, OnUpdateFileRecompareAs)
	// [Edit] menu
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
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
	// [Web] menu
	ON_COMMAND(ID_WEB_COMPARE_SCREENSHOTS, OnWebCompareScreenshots)
	ON_COMMAND(ID_WEB_COMPARE_HTMLS, OnWebCompareHTMLs)
	ON_COMMAND(ID_WEB_COMPARE_RESOURCETREES, OnWebCompareResourceTrees)
	// [Image] menu
//	ON_COMMAND(ID_IMG_VIEWDIFFERENCES, OnImgViewDifferences)
//	ON_UPDATE_COMMAND_UI(ID_IMG_VIEWDIFFERENCES, OnUpdateImgViewDifferences)
//	ON_COMMAND_RANGE(ID_IMG_ZOOM_25, ID_IMG_ZOOM_800, OnImgZoom)
//	ON_UPDATE_COMMAND_UI_RANGE(ID_IMG_ZOOM_25, ID_IMG_ZOOM_800, OnUpdateImgZoom)
	// [Tools] menu
//	ON_COMMAND(ID_TOOLS_GENERATEREPORT, OnToolsGenerateReport)
	// [Plugins] menu
	ON_COMMAND_RANGE(ID_UNPACKERS_FIRST, ID_UNPACKERS_LAST, OnFileRecompareAs)
	ON_COMMAND(ID_OPEN_WITH_UNPACKER, OnOpenWithUnpacker)
	// [Window] menu
	ON_COMMAND(ID_WINDOW_CHANGE_PANE, OnWindowChangePane)
	// [Help] menu
	ON_COMMAND(ID_HELP, OnHelp)
	// Status bar
	ON_UPDATE_COMMAND_UI(ID_STATUS_DIFFNUM, OnUpdateStatusNum)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMenu CWebPageDiffFrame::menu;

/////////////////////////////////////////////////////////////////////////////
// CWebPageDiffFrame construction/destruction

CWebPageDiffFrame::CWebPageDiffFrame()
: CMergeFrameCommon(IDI_EQUALWEBPAGE, IDI_NOTEQUALWEBPAGE)
, m_pDirDoc(nullptr)
, m_bAutoMerged(false)
, m_pWebDiffWindow(nullptr)
//, m_pWebToolWindow(nullptr)
, m_nBufferType{BUFFERTYPE::NORMAL, BUFFERTYPE::NORMAL, BUFFERTYPE::NORMAL}
, m_bRO{}
, m_nActivePane(-1)
{
}

CWebPageDiffFrame::~CWebPageDiffFrame()
{
	if (m_pDirDoc != nullptr)
	{
		m_pDirDoc->MergeDocClosing(this);
		m_pDirDoc = nullptr;
	}

	HMODULE hModule = GetModuleHandleW(L"WinWebDiffLib.dll");
	if (hModule != nullptr)
	{
		bool (*pfnWinWebDiff_DestroyWindow)(IWebDiffWindow *) = 
			(bool (*)(IWebDiffWindow *))GetProcAddress(hModule, "WinWebDiff_DestroyWindow");
//		bool (*pfnWinWebDiff_DestroyToolWindow)(IWebToolWindow *) = 
//			(bool (*)(IWebToolWindow *))GetProcAddress(hModule, "WinWebDiff_DestroyToolWindow");
		if (pfnWinWebDiff_DestroyWindow != nullptr/* && pfnWinWebDiff_DestroyToolWindow != nullptr*/)
		{
			if (m_pWebDiffWindow != nullptr)
				pfnWinWebDiff_DestroyWindow(m_pWebDiffWindow);
//			if (m_pWebToolWindow != nullptr)
//				pfnWinWebDiff_DestroyToolWindow(m_pWebToolWindow);
			m_pWebDiffWindow = nullptr;
//			m_pWebToolWindow = nullptr;
		}
	}
}

bool CWebPageDiffFrame::OpenDocs(int nFiles, const FileLocation fileloc[], const bool bRO[], const String strDesc[], CMDIFrameWnd *pParent)
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

	LPCTSTR lpszWndClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
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
		m_pWebDiffWindow->FirstDiff();

	return true;
}

void CWebPageDiffFrame::MoveOnLoad(int nPane, int)
{
	if (nPane < 0)
	{
		nPane = GetOptionsMgr()->GetInt(OPT_ACTIVE_PANE);
		if (nPane < 0 || nPane >= m_pWebDiffWindow->GetPaneCount())
			nPane = 0;
	}

	m_pWebDiffWindow->SetActivePane(nPane);
}

void CWebPageDiffFrame::ChangeFile(int nBuffer, const String& path)
{
	m_filePaths[nBuffer] = path;
	m_nBufferType[nBuffer] = BUFFERTYPE::NORMAL;
	m_strDesc[nBuffer] = _T("");
	int nActivePane = m_pWebDiffWindow->GetActivePane();

	OpenUrls();
	for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
	{
		m_fileInfo[pane].Update(m_filePaths[pane]);
	}

	MoveOnLoad(nActivePane);

	UpdateHeaderPath(nBuffer);
	UpdateLastCompareResult();
}

/**
 * @brief DirDoc gives us its identity just after it creates us
 */
void CWebPageDiffFrame::SetDirDoc(CDirDoc * pDirDoc)
{
	ASSERT(pDirDoc != nullptr && m_pDirDoc == nullptr);
	m_pDirDoc = pDirDoc;
}

IMergeDoc::FileChange CWebPageDiffFrame::IsFileChangedOnDisk(int pane) const
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

void CWebPageDiffFrame::CheckFileChanged(void)
{
	for (int pane = 0; pane < m_pWebDiffWindow->GetPaneCount(); ++pane)
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
void CWebPageDiffFrame::CreateImgWndStatusBar(CStatusBar &wndStatusBar, CWnd *pwndPane)
{
	wndStatusBar.Create(pwndPane, WS_CHILD|WS_VISIBLE);
	wndStatusBar.SetIndicators(0, 1);
	wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH, 0);
	wndStatusBar.SetParent(this);
	wndStatusBar.SetWindowPos(&wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

/**
 * @brief returns true if WinWebDiffLib.dll is loadable
 */
bool CWebPageDiffFrame::IsLoadable()
{
	static HMODULE hModule;
	if (hModule == nullptr)
	{
		hModule = LoadLibraryW(L"WinWebDiff\\WinWebDiffLib.dll");
		if (hModule == nullptr)
			return false;
	}
	return true;
}

/**
 * @brief Create the splitter, the filename bar, the status bar, and the two views
 */
BOOL CWebPageDiffFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	if (!IsLoadable())
		return FALSE;

	HMODULE hModule = GetModuleHandleW(L"WinWebDiffLib.dll");
	if (hModule == nullptr)
		return FALSE;

	IWebDiffWindow * (*pfnWinWebDiff_CreateWindow)(HINSTANCE hInstance, HWND hWndParent, int nID) =
		(IWebDiffWindow * (*)(HINSTANCE hInstance, HWND hWndParent, int nID))GetProcAddress(hModule, "WinWebDiff_CreateWindow");
	if (pfnWinWebDiff_CreateWindow == nullptr ||
		(m_pWebDiffWindow = pfnWinWebDiff_CreateWindow(hModule, m_hWnd, AFX_IDW_PANE_FIRST)) == nullptr)
	{
		FreeLibrary(hModule);
		return FALSE;
	}

	if (!m_pWebDiffWindow->IsWebView2Installed())
	{
		AfxMessageBox(_("WebView2 is not installed").c_str(), MB_OK);
		return FALSE;
	}

	COLORSETTINGS colors;
	Options::DiffColors::Load(GetOptionsMgr(), colors);
	m_pWebDiffWindow->SetDiffColor(colors.clrDiff);
	m_pWebDiffWindow->SetSelDiffColor(colors.clrSelDiff);
	LoadOptions();

	bool bResult;
	if (std::count(m_nBufferType, m_nBufferType + m_filePaths.GetSize(), BUFFERTYPE::UNNAMED) == m_filePaths.GetSize())
	{
		bResult = m_pWebDiffWindow->New(m_filePaths.GetSize());
	}
	else
	{
		bResult = OpenUrls();
	}

	for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
	{
		m_fileInfo[pane].Update(m_filePaths[pane]);
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

//	IWebToolWindow * (*pfnWinWebDiff_CreateToolWindow)(HINSTANCE hInstance, HWND hWndParent, IWebDiffWindow *) =
//		(IWebToolWindow * (*)(HINSTANCE hInstance, HWND hWndParent, IWebDiffWindow *pWebPageDiffWindow))GetProcAddress(hModule, "WinWebDiff_CreateToolWindow");
//	if (pfnWinWebDiff_CreateToolWindow == nullptr ||
//		(m_pWebToolWindow = pfnWinWebDiff_CreateToolWindow(hModule, m_wndLocationBar.m_hWnd, m_pWebDiffWindow)) == nullptr)
//	{
//		return FALSE;
//	}

//	m_pWebToolWindow->Translate(TranslateLocationPane);

	m_wndLocationBar.SetFrameHwnd(GetSafeHwnd());

	return TRUE;
}

void CWebPageDiffFrame::TranslateLocationPane(int id, const wchar_t *org, size_t dstbufsize, wchar_t *dst)
{
	swprintf_s(dst, dstbufsize, L"%s", tr("WebPageDiffFrame|LocationPane", ucr::toUTF8(org)).c_str());
}

/////////////////////////////////////////////////////////////////////////////
// CWebPageDiffFrame message handlers

int CWebPageDiffFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	m_wndFilePathBar.SetPaneCount(m_pWebDiffWindow->GetPaneCount());
	m_wndFilePathBar.SetOnSetFocusCallback([&](int pane) { m_pWebDiffWindow->SetActivePane(pane); });

	// Merge frame also has a dockable bar at the very left
	// created in OnCreateClient 
	m_wndLocationBar.SetBarStyle(m_wndLocationBar.GetBarStyle() |
		CBRS_SIZE_DYNAMIC | CBRS_ALIGN_LEFT);
	m_wndLocationBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar(&m_wndLocationBar, AFX_IDW_DOCKBAR_LEFT);

	for (int nPane = 0; nPane < m_pWebDiffWindow->GetPaneCount(); nPane++)
	{
//		m_pWebDiffWindow->SetReadOnly(nPane, m_bRO[nPane]);

		m_wndFilePathBar.SetActive(nPane, FALSE);
		CreateImgWndStatusBar(m_wndStatusBar[nPane], CWnd::FromHandle(m_pWebDiffWindow->GetPaneHWND(nPane)));
		UpdateHeaderPath(nPane);
	}

	CSize size = m_wndStatusBar[0].CalcFixedLayout(TRUE, TRUE);
	m_rectBorder.bottom = size.cy;

	CDockState pDockState;
	pDockState.LoadState(_T("Settings-WebPageDiffFrame"));
	if (EnsureValidDockState(pDockState)) // checks for valid so won't ASSERT
		SetDockState(pDockState);
	// for the dimensions of the diff and location pane, use the CSizingControlBar loader
	m_wndLocationBar.LoadState(_T("Settings-WebPageDiffFrame"));

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
bool CWebPageDiffFrame::EnsureValidDockState(CDockState& state)
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
BOOL CWebPageDiffFrame::DestroyWindow() 
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

void CWebPageDiffFrame::LoadOptions()
{
//	m_pWebDiffWindow->SetShowDifferences(GetOptionsMgr()->GetBool(OPT_CMP_IMG_SHOWDIFFERENCES));
//	m_pWebDiffWindow->SetZoom(GetOptionsMgr()->GetInt(OPT_CMP_IMG_ZOOM) / 1000.0);
	COLORREF clrBackColor = GetOptionsMgr()->GetInt(OPT_CMP_IMG_BACKCOLOR);
	RGBQUAD backColor = { GetBValue(clrBackColor), GetGValue(clrBackColor), GetRValue(clrBackColor) };
//	m_pWebDiffWindow->SetDiffColorAlpha(GetOptionsMgr()->GetInt(OPT_CMP_IMG_DIFFCOLORALPHA) / 100.0);
}

void CWebPageDiffFrame::SaveOptions()
{
//	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_SHOWDIFFERENCES, m_pWebDiffWindow->GetShowDifferences());
//	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_ZOOM, static_cast<int>(m_pWebDiffWindow->GetZoom() * 1000));
//	GetOptionsMgr()->SaveOption(OPT_CMP_IMG_DIFFCOLORALPHA, static_cast<int>(m_pWebDiffWindow->GetDiffColorAlpha() * 100.0));
}
/**
 * @brief Save coordinates of the frame, splitters, and bars
 *
 * @note Do not save the maximized/restored state here. We are interested
 * in the state of the active frame, and maybe this frame is not active
 */
void CWebPageDiffFrame::SavePosition()
{
	CRect rc;
	GetWindowRect(&rc);

	// save the bars layout
	// save docking positions and sizes
	CDockState m_pDockState;
	GetDockState(m_pDockState);
	m_pDockState.SaveState(_T("Settings-WebPageDiffFrame"));
	// for the dimensions of the diff pane, use the CSizingControlBar save
	m_wndLocationBar.SaveState(_T("Settings-WebPageDiffFrame"));
}

void CWebPageDiffFrame::SaveActivePane()
{
	GetOptionsMgr()->SaveOption(OPT_ACTIVE_PANE, m_pWebDiffWindow->GetActivePane());
}

void CWebPageDiffFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMergeFrameCommon::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	if (bActivate)
	{
		GetMainFrame()->PostMessage(WM_USER + 1);
	}
}

void CWebPageDiffFrame::OnClose() 
{
	// clean up pointers.
	CMergeFrameCommon::OnClose();
}

void CWebPageDiffFrame::OnDestroy()
{
	if (!m_pWebDiffWindow)
		return;
}

/**
 * @brief Reloads the opened files
 */
void CWebPageDiffFrame::OnFileReload()
{
	int nActivePane = m_pWebDiffWindow->GetActivePane();
	OpenUrls();
	MoveOnLoad(nActivePane);
	for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
		m_fileInfo[pane].Update(m_filePaths[pane]);
}

void CWebPageDiffFrame::OnFileClose() 
{
	OnClose();
}

void CWebPageDiffFrame::OnFileRecompareAs(UINT nID)
{
	FileLocation fileloc[3];
	DWORD dwFlags[3];
	String strDesc[3];
	int nBuffers = m_filePaths.GetSize();
	CDirDoc *pDirDoc = m_pDirDoc->GetMainView() ? m_pDirDoc :
		static_cast<CDirDoc*>(theApp.m_pDirTemplate->CreateNewDocument());
	PackingInfo infoUnpacker(m_infoUnpacker.GetPluginPipeline());

	for (int nBuffer = 0; nBuffer < nBuffers; ++nBuffer)
	{
		fileloc[nBuffer].setPath(m_filePaths[nBuffer]);
		dwFlags[nBuffer] = m_bRO[nBuffer] ? FFILEOPEN_READONLY : 0;
		strDesc[nBuffer] = m_strDesc[nBuffer];
	}
	if (ID_UNPACKERS_FIRST <= nID && nID <= ID_UNPACKERS_LAST)
	{
		infoUnpacker.SetPluginPipeline(CMainFrame::GetPluginPipelineByMenuId(nID, FileTransform::UnpackerEventNames, ID_UNPACKERS_FIRST));
		nID = GetOptionsMgr()->GetBool(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE) ? ID_MERGE_COMPARE_WEBPAGE : -1;
	}

	CloseNow();
	GetMainFrame()->ShowMergeDoc(nID, pDirDoc, nBuffers, fileloc, dwFlags, strDesc, _T(""), &infoUnpacker);
}

void CWebPageDiffFrame::OnUpdateFileRecompareAs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(pCmdUI->m_nID != ID_MERGE_COMPARE_WEBPAGE);
}

void CWebPageDiffFrame::OnOpenWithUnpacker()
{
	CSelectPluginDlg dlg(m_infoUnpacker.GetPluginPipeline(),
		strutils::join(m_filePaths.begin(), m_filePaths.end(), _T("|")),
		CSelectPluginDlg::PluginType::Unpacker, false);
	if (dlg.DoModal() == IDOK)
	{
		PackingInfo infoUnpacker(dlg.GetPluginPipeline());
		PathContext paths = m_filePaths;
		DWORD dwFlags[3] = { FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, FFILEOPEN_NOMRU };
		String strDesc[3] = { m_strDesc[0], m_strDesc[1], m_strDesc[2] };
		CloseNow();
		GetMainFrame()->DoFileOpen(
			GetOptionsMgr()->GetBool(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE) ? ID_MERGE_COMPARE_WEBPAGE: -1,
			&paths, dwFlags, strDesc, _T(""), &infoUnpacker);
	}
}

void  CWebPageDiffFrame::OnWindowChangePane() 
{
	m_pWebDiffWindow->SetActivePane((m_pWebDiffWindow->GetActivePane() + 1) % m_pWebDiffWindow->GetPaneCount());
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CWebPageDiffFrame::UpdateHeaderPath(int pane)
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

	m_wndFilePathBar.SetText(pane, sText.c_str());

	SetTitle(nullptr);
}

/// update splitting position for panels 1/2 and headerbar and statusbar 
void CWebPageDiffFrame::UpdateHeaderSizes()
{
	if (m_pWebDiffWindow != nullptr)
	{
		int w[3];
		CRect rc, rcMergeWindow;
		int nPaneCount = m_pWebDiffWindow->GetPaneCount();
		GetClientRect(&rc);
		::GetWindowRect(m_pWebDiffWindow->GetHWND(), &rcMergeWindow);
		ScreenToClient(rcMergeWindow);
		if (!m_pWebDiffWindow->GetHorizontalSplit())
		{
			for (int pane = 0; pane < nPaneCount; pane++)
			{
				RECT rc1 = m_pWebDiffWindow->GetPaneWindowRect(pane);
				w[pane] = rc1.right - rc1.left - 4;
				if (w[pane]<1) w[pane]=1; // Perry 2003-01-22 (I don't know why this happens)
			}
		}
		else
		{
			for (int pane = 0; pane < nPaneCount; pane++)
				w[pane] = rcMergeWindow.Width() / nPaneCount - 4;
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
void CWebPageDiffFrame::SetTitle(LPCTSTR lpszTitle)
{
	String sTitle = (lpszTitle != nullptr) ? lpszTitle : CMergeFrameCommon::GetTitleString(m_filePaths, m_strDesc, &m_infoUnpacker, nullptr);
	CMergeFrameCommon::SetTitle(sTitle.c_str());
	if (m_hWnd != nullptr)
		SetWindowText(sTitle.c_str());
}

void CWebPageDiffFrame::UpdateLastCompareResult()
{
	SetLastCompareResult(m_pWebDiffWindow->GetDiffCount() > 0 ? 1 : 0);
}

void CWebPageDiffFrame::UpdateAutoPaneResize()
{
}

void CWebPageDiffFrame::UpdateSplitter()
{
}

bool CWebPageDiffFrame::OpenUrls()
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
		bResult = m_pWebDiffWindow->Open(ucr::toUTF16(strTempFileName[0]).c_str(), ucr::toUTF16(strTempFileName[1]).c_str());
	else
		bResult = m_pWebDiffWindow->Open(ucr::toUTF16(strTempFileName[0]).c_str(), ucr::toUTF16(strTempFileName[1]).c_str(), ucr::toUTF16(strTempFileName[2]).c_str());
	return bResult;
}

/**
 * @brief Update associated diff item
 */
int CWebPageDiffFrame::UpdateDiffItem(CDirDoc *pDirDoc)
{
	// If directory compare has results
	if (pDirDoc && pDirDoc->HasDiffs())
	{
		const String &pathLeft = m_filePaths.GetLeft();
		const String &pathRight = m_filePaths.GetRight();
		CDiffContext &ctxt = const_cast<CDiffContext &>(pDirDoc->GetDiffContext());
// FIXME:
//		if (UINT_PTR pos = pDirDoc->FindItemFromPaths(pathLeft, pathRight))
//		{
//			DIFFITEM &di = pDirDoc->GetDiffRefByKey(pos);
//			::UpdateDiffItem(m_nBuffers, di, &ctxt);
//		}
	}
	int result = m_pWebDiffWindow->GetDiffCount() > 0 ? 1 : 0;
	SetLastCompareResult(result != 0);
	return result;
}

/// Document commanding us to close
bool CWebPageDiffFrame::CloseNow()
{
	DestroyWindow();
	return true;
}

/**
 * @brief A string to display as a tooltip for MDITabbar
 */
CString CWebPageDiffFrame::GetTooltipString() const
{
	return CMergeFrameCommon::GetTooltipString(m_filePaths, m_strDesc, &m_infoUnpacker, nullptr).c_str();
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CWebPageDiffFrame::UpdateResources()
{
	//m_pWebToolWindow->Translate(TranslateLocationPane);
}

/**
 * @brief Handle some keys when in merging mode
 */
bool CWebPageDiffFrame::MergeModeKeyDown(MSG* pMsg)
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
BOOL CWebPageDiffFrame::PreTranslateMessage(MSG* pMsg)
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

void CWebPageDiffFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMergeFrameCommon::OnSize(nType, cx, cy);
	UpdateHeaderSizes();
}

/**
 * @brief Synchronize control and status bar placements with splitter position,
 * update mod indicators, synchronize scrollbars
 */
void CWebPageDiffFrame::OnIdleUpdateCmdUI()
{
	if (IsWindowVisible())
	{
		int nActivePane = m_pWebDiffWindow->GetActivePane();
		if (nActivePane != -1)
			m_nActivePane = nActivePane;

		UpdateHeaderSizes();
		for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
		{
			// Update mod indicators
			m_wndFilePathBar.SetActive(pane, pane == nActivePane);
		}
		UpdateLastCompareResult();
	}
	CMergeFrameCommon::OnIdleUpdateCmdUI();
}

/**
 * @brief Save pane sizes and positions when one of panes requests it.
 */
LRESULT CWebPageDiffFrame::OnStorePaneSizes(WPARAM wParam, LPARAM lParam)
{
	SavePosition();
	return 0;
}

void CWebPageDiffFrame::OnUpdateStatusNum(CCmdUI* pCmdUI) 
{
	TCHAR sIdx[32] = { 0 };
	TCHAR sCnt[32] = { 0 };
	String s;
	const int nDiffs = m_pWebDiffWindow->GetDiffCount();
	
	// Files are identical - show text "Identical"
	if (nDiffs <= 0)
		s = theApp.LoadString(IDS_IDENTICAL);
	
	// There are differences, but no selected diff
	// - show amount of diffs
	else if (m_pWebDiffWindow->GetCurrentDiffIndex() < 0)
	{
		s = theApp.LoadString(nDiffs == 1 ? IDS_1_DIFF_FOUND : IDS_NO_DIFF_SEL_FMT);
		_itot_s(nDiffs, sCnt, 10);
		strutils::replace(s, _T("%1"), sCnt);
	}
	
	// There are differences and diff selected
	// - show diff number and amount of diffs
	else
	{
		s = theApp.LoadString(IDS_DIFF_NUMBER_STATUS_FMT);
		const int signInd = m_pWebDiffWindow->GetCurrentDiffIndex();
		_itot_s(signInd + 1, sIdx, 10);
		strutils::replace(s, _T("%1"), sIdx);
		_itot_s(nDiffs, sCnt, 10);
		strutils::replace(s, _T("%2"), sCnt);
	}
	pCmdUI->SetText(s.c_str());
}
	
/**
 * @brief Cut current selection to clipboard
 */
void CWebPageDiffFrame::OnEditCut()
{
	if (CWnd *pWnd = GetFocus())
		pWnd->SendMessage(WM_CUT);
}

/**
 * @brief Copy current selection to clipboard
 */
void CWebPageDiffFrame::OnEditCopy()
{
	if (CWnd *pWnd = GetFocus())
		pWnd->SendMessage(WM_COPY);
}

/**
 * @brief Paste clipboard content over selected content
 */
void CWebPageDiffFrame::OnEditPaste()
{
	if (CWnd *pWnd = GetFocus())
		pWnd->SendMessage(WM_PASTE);
}

/**
 * @brief Undo last action
 */
void CWebPageDiffFrame::OnEditUndo()
{
	if (CWnd *pWnd = GetFocus())
		pWnd->SendMessage(WM_UNDO);
}

/**
 * @brief Redo last action
 */
void CWebPageDiffFrame::OnEditRedo()
{
//	GetFocus()->SendMessage(WM_REDO);
}

/**
 * @brief Select entire image
 */
void CWebPageDiffFrame::OnEditSelectAll()
{
	if (CWnd *pWnd = GetFocus())
		pWnd->SendMessage(EM_SETSEL, 0, -1);
}

/**
 * @brief Called when user selects View/Zoom In from menu.
 */
void CWebPageDiffFrame::OnViewZoomIn()
{
	m_pWebDiffWindow->SetZoom(m_pWebDiffWindow->GetZoom() + 0.1);
}

/**
 * @brief Called when user selects View/Zoom Out from menu.
 */
void CWebPageDiffFrame::OnViewZoomOut()
{
	m_pWebDiffWindow->SetZoom(m_pWebDiffWindow->GetZoom() - 0.1);
}

/**
 * @brief Called when user selects View/Zoom Normal from menu.
 */
void CWebPageDiffFrame::OnViewZoomNormal()
{
	m_pWebDiffWindow->SetZoom(1.0);
}

/**
 * @brief Split panes vertically
 */
void CWebPageDiffFrame::OnViewSplitVertically() 
{
	bool bSplitVertically = !m_pWebDiffWindow->GetHorizontalSplit();
	bSplitVertically = !bSplitVertically; // toggle
	GetOptionsMgr()->SaveOption(OPT_SPLIT_HORIZONTALLY, !bSplitVertically);
	m_pWebDiffWindow->SetHorizontalSplit(!bSplitVertically);
}

/**
 * @brief Update "Split Vertically" UI items
 */
void CWebPageDiffFrame::OnUpdateViewSplitVertically(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(!m_pWebDiffWindow->GetHorizontalSplit());
}

/**
 * @brief Go to first diff
 *
 * Called when user selects "First Difference"
 * @sa CWebPageDiffFrame::SelectDiff()
 */
void CWebPageDiffFrame::OnFirstdiff()
{
	m_pWebDiffWindow->FirstDiff();
}

/**
 * @brief Update "First diff" UI items
 */
void CWebPageDiffFrame::OnUpdateFirstdiff(CCmdUI* pCmdUI)
{
	OnUpdatePrevdiff(pCmdUI);
}

/**
 * @brief Go to last diff
 */
void CWebPageDiffFrame::OnLastdiff()
{
	m_pWebDiffWindow->LastDiff();
}

/**
 * @brief Update "Last diff" UI items
 */
void CWebPageDiffFrame::OnUpdateLastdiff(CCmdUI* pCmdUI)
{
	OnUpdateNextdiff(pCmdUI);
}

/**
 * @brief Go to next diff and select it.
 */
void CWebPageDiffFrame::OnNextdiff()
{
	if (m_pWebDiffWindow->GetCurrentDiffIndex() != m_pWebDiffWindow->GetDiffCount() - 1)
		m_pWebDiffWindow->NextDiff();
	else if (m_pDirDoc != nullptr)
		m_pDirDoc->MoveToNextDiff(this);
}

/**
 * @brief Update "Next diff" UI items
 */
void CWebPageDiffFrame::OnUpdateNextdiff(CCmdUI* pCmdUI)
{
	bool enabled =
		m_pWebDiffWindow->GetNextDiffIndex() >= 0 ||
		(m_pWebDiffWindow->GetDiffCount() > 0 && m_pWebDiffWindow->GetCurrentDiffIndex() == -1);

	if (!enabled && m_pDirDoc != nullptr)
		enabled = m_pDirDoc->MoveableToNextDiff();

	pCmdUI->Enable(enabled);
}

/**
 * @brief Go to previous diff and select it.
 */
void CWebPageDiffFrame::OnPrevdiff()
{
	if (m_pWebDiffWindow->GetCurrentDiffIndex() > 0)
	{
		m_pWebDiffWindow->PrevDiff();
	}
	else if (m_pDirDoc != nullptr)
		m_pDirDoc->MoveToPrevDiff(this);
}

/**
 * @brief Update "Previous diff" UI items
 */
void CWebPageDiffFrame::OnUpdatePrevdiff(CCmdUI* pCmdUI)
{
	bool enabled =
		m_pWebDiffWindow->GetPrevDiffIndex() >= 0 ||
		(m_pWebDiffWindow->GetDiffCount() > 0 && m_pWebDiffWindow->GetCurrentDiffIndex() == -1);

	if (!enabled && m_pDirDoc != nullptr)
		enabled = m_pDirDoc->MoveableToPrevDiff();

	pCmdUI->Enable(enabled);
}

/**
 * @brief Go to next conflict and select it.
 */
void CWebPageDiffFrame::OnNextConflict()
{
	m_pWebDiffWindow->NextConflict();
}

/**
 * @brief Update "Next Conflict" UI items
 */
void CWebPageDiffFrame::OnUpdateNextConflict(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(
		m_pWebDiffWindow->GetPaneCount() > 2 && (
			m_pWebDiffWindow->GetNextConflictIndex() >= 0 ||
			(m_pWebDiffWindow->GetConflictCount() > 0 && m_pWebDiffWindow->GetCurrentDiffIndex() == -1)
		)
	);
}

/**
 * @brief Go to previous diff and select it.
 */
void CWebPageDiffFrame::OnPrevConflict()
{
	m_pWebDiffWindow->PrevConflict();
}

/**
 * @brief Update "Previous diff" UI items
 */
void CWebPageDiffFrame::OnUpdatePrevConflict(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(
		m_pWebDiffWindow->GetPaneCount() > 2 && (
			m_pWebDiffWindow->GetPrevConflictIndex() >= 0 ||
			(m_pWebDiffWindow->GetConflictCount() > 0 && m_pWebDiffWindow->GetCurrentDiffIndex() == -1)
		)
	);
}

void CWebPageDiffFrame::OnWebCompareScreenshots()
{
	PathContext paths;
	const wchar_t *spaths[3];
	std::vector<String> descs;
	const int nPanes = m_pWebDiffWindow->GetPaneCount();
	for (int pane = 0; pane < nPanes; ++pane)
	{
		std::shared_ptr<TempFile> pTempFile(new TempFile());
		pTempFile->Create(_T("SCR"), _T(".png"));
		paths.SetPath(pane, pTempFile->GetPath());
		spaths[pane] = paths[pane].c_str();
		descs.push_back(m_filePaths[pane]);
		m_tempFiles.push_back(pTempFile);
	}
	m_pWebDiffWindow->SaveScreenshots(spaths,
		Callback<IWebDiffCallback>([paths, descs](HRESULT hr) -> HRESULT
			{
				GetMainFrame()->DoFileOpen(0, &paths, nullptr, descs.data());
				return S_OK;
			}).Get());
}

void CWebPageDiffFrame::OnWebCompareHTMLs()
{
	PathContext paths;
	const wchar_t *spaths[3];
	std::vector<String> descs;
	const int nPanes = m_pWebDiffWindow->GetPaneCount();
	for (int pane = 0; pane < nPanes; ++pane)
	{
		std::shared_ptr<TempFile> pTempFile(new TempFile());
		pTempFile->Create(_T("HTM"), _T(".html"));
		paths.SetPath(pane, pTempFile->GetPath());
		spaths[pane] = paths[pane].c_str();
		descs.push_back(m_filePaths[pane]);
		m_tempFiles.push_back(pTempFile);
	}
	m_pWebDiffWindow->SaveHTMLs(spaths,
		Callback<IWebDiffCallback>([paths, descs](HRESULT hr) -> HRESULT
			{
				PackingInfo infoUnpacker(String(_T("PrettifyHTML")));
				GetMainFrame()->DoFileOpen(0, &paths, nullptr, descs.data(), _T(""), &infoUnpacker);
				return S_OK;
			}).Get());
}

void CWebPageDiffFrame::OnWebCompareResourceTrees()
{
	PathContext paths;
	const wchar_t *spaths[3];
	std::vector<String> descs;
	const int nPanes = m_pWebDiffWindow->GetPaneCount();
	for (int pane = 0; pane < nPanes; ++pane)
	{
		std::shared_ptr<TempFolder> pTempFolder(new TempFolder());
		pTempFolder->Create();
		paths.SetPath(pane, pTempFolder->GetPath());
		spaths[pane] = paths[pane].c_str();
		descs.push_back(m_filePaths[pane]);
		m_tempFolders.push_back(pTempFolder);
	}
	m_pWebDiffWindow->SaveResourceTrees(spaths,
		Callback<IWebDiffCallback>([paths, descs](HRESULT hr) -> HRESULT
			{
				GetMainFrame()->DoFileOrFolderOpen(&paths, nullptr, descs.data(), _T(""), true);
				return S_OK;
			}) .Get());
}

//void CWebPageDiffFrame::OnImgViewDifferences()
//{
//	m_pWebDiffWindow->SetShowDifferences(!m_pWebDiffWindow->GetShowDifferences());
//	SaveOptions();
//}
//
//void CWebPageDiffFrame::OnUpdateImgViewDifferences(CCmdUI* pCmdUI)
//{
//	pCmdUI->SetCheck(m_pWebDiffWindow->GetShowDifferences() ? 1 : 0);
//}
//
//void CWebPageDiffFrame::OnImgZoom(UINT nId)
//{
//	m_pWebDiffWindow->SetZoom(pow(2.0, int(nId - ID_IMG_ZOOM_100)));
//	SaveOptions();
//}
//
//void CWebPageDiffFrame::OnUpdateImgZoom(CCmdUI* pCmdUI)
//{
//	pCmdUI->SetRadio(pow(2.0, int(pCmdUI->m_nID - ID_IMG_ZOOM_100)) == m_pWebDiffWindow->GetZoom());
//}
//
bool CWebPageDiffFrame::GenerateReport(const String& sFileName) const
{
	//return GenerateReport(sFileName, true);
	return false;
}
//
///**
// * @brief Generate report from file compare results.
// */
//bool CWebPageDiffFrame::GenerateReport(const String& sFileName, bool allPages) const
//{
//	return false;
//}
//
///**
// * @brief Generate report from file compare results.
// */
//void CWebPageDiffFrame::OnToolsGenerateReport()
//{
//}

void CWebPageDiffFrame::OnRefresh()
{
	if (UpdateDiffItem(m_pDirDoc) == 0)
	{
		CMergeFrameCommon::ShowIdenticalMessage(m_filePaths, true,
			[](LPCTSTR msg, UINT flags, UINT id) -> int { return AfxMessageBox(msg, flags, id); });
	}
}

void CWebPageDiffFrame::OnSetFocus(CWnd* pNewWnd)
{
	if (m_nActivePane != -1)
		m_pWebDiffWindow->SetActivePane(m_nActivePane);
}


/**
 * @brief Open help from mainframe when user presses F1
 */
void CWebPageDiffFrame::OnHelp()
{
	theApp.ShowHelp(WebPageDiffFrameHelpLocation);
}
