/** 
 * @file  WebPageDiffFrm.cpp
 *
 * @brief Implementation file for CWebPageDiffFrame
 *
 */

#include "stdafx.h"
#include "WebPageDiffFrm.h"
#include "FrameWndHelper.h"
#include "Merge.h"
#include "MainFrm.h"
#include "IDirDoc.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsDiffColors.h"
#include "OptionsDiffOptions.h"
#include "CompareOptions.h"
#include "paths.h"
#include "PathContext.h"
#include "unicoder.h"
#include "FileOrFolderSelect.h"
#include "SelectPluginDlg.h"
#include "FileLocation.h"
#include "Constants.h"
#include "Environment.h"
#include "UniFile.h"
#include "Logger.h"
#include <Poco/RegularExpression.h>
#include <Poco/Exception.h>
#include "DarkModeLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef BCN_DROPDOWN
#define BCN_DROPDOWN (BCN_FIRST + 0x0002)
#endif

template <typename T, typename Func>
struct CallbackImpl : public T
{
	CallbackImpl(Func&& callback) : m_callback(std::move(callback)) {}
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override { return E_NOTIMPL; }
	ULONG STDMETHODCALLTYPE AddRef(void) override { return ++m_nRef; }
	ULONG STDMETHODCALLTYPE Release(void) override { if (--m_nRef == 0) { delete this; return 0; } return m_nRef; }
	HRESULT STDMETHODCALLTYPE Invoke(const WebDiffCallbackResult& result) { return m_callback(result); }
	HRESULT STDMETHODCALLTYPE Invoke(const WebDiffEvent& event) { return m_callback(event); }
	Func m_callback;
	int m_nRef = 0;
};

template<typename T, typename Func>
CComPtr<T> Callback(Func&& callback)
{
	return CComPtr<T>(new CallbackImpl<T, Func>(std::move(callback)));
}
 
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
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
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
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	// [View] menu
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCATION_BAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_LOCATION_BAR, OnBarCheck)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOMNORMAL, OnViewZoomNormal)
	ON_COMMAND(ID_VIEW_LINEDIFFS, OnViewLineDiffs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINEDIFFS, OnUpdateViewLineDiffs)
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
	ON_COMMAND(ID_WEB_VIEWDIFFERENCES, OnWebViewDifferences)
	ON_UPDATE_COMMAND_UI(ID_WEB_VIEWDIFFERENCES, OnUpdateWebViewDifferences)
	ON_COMMAND(ID_WEB_SIZE_FIT_TO_WINDOW, OnWebFitToWindow)
	ON_UPDATE_COMMAND_UI(ID_WEB_SIZE_FIT_TO_WINDOW, OnUpdateWebFitToWindow)
	ON_COMMAND_RANGE(ID_WEB_SIZE_320x512, ID_WEB_SIZE_1440x900, OnWebSize)
	ON_COMMAND_RANGE(ID_WEB_COMPARE_SCREENSHOTS, ID_WEB_COMPARE_FULLSIZE_SCREENSHOTS, OnWebCompareScreenshots)
	ON_COMMAND(ID_WEB_COMPARE_HTMLS, OnWebCompareHTMLs)
	ON_COMMAND(ID_WEB_COMPARE_TEXTS, OnWebCompareTexts)
	ON_COMMAND(ID_WEB_COMPARE_RESOURCETREES, OnWebCompareResourceTrees)
	ON_COMMAND_RANGE(ID_WEB_SYNC_ENABLED, ID_WEB_SYNC_GOBACKFORWARD, OnWebSyncEvent)
	ON_UPDATE_COMMAND_UI_RANGE(ID_WEB_SYNC_ENABLED, ID_WEB_SYNC_GOBACKFORWARD, OnUpdateWebSyncEvent)
	ON_COMMAND_RANGE(ID_WEB_CLEAR_DISK_CACHE, ID_WEB_CLEAR_ALL_PROFILE, OnWebClear)
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

CMenu CWebPageDiffFrame::menu;

/////////////////////////////////////////////////////////////////////////////
// CWebPageDiffFrame construction/destruction

CWebPageDiffFrame::CWebPageDiffFrame()
: CMergeFrameCommon(IDI_EQUALWEBPAGE, IDI_NOTEQUALWEBPAGE)
, m_pDirDoc(nullptr)
, m_bAutoMerged(false)
, m_pWebDiffWindow(nullptr)
, m_pWebToolWindow(nullptr)
, m_nBufferType{BUFFERTYPE::NORMAL, BUFFERTYPE::NORMAL, BUFFERTYPE::NORMAL}
, m_bRO{}
, m_nActivePane(-1)
, m_bCompareCompleted(false)
{
}

CWebPageDiffFrame::~CWebPageDiffFrame()
{
	GetMainFrame()->UnwatchDocuments(this);

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
		bool (*pfnWinWebDiff_DestroyToolWindow)(IWebToolWindow *) = 
			(bool (*)(IWebToolWindow *))GetProcAddress(hModule, "WinWebDiff_DestroyToolWindow");
		if (pfnWinWebDiff_DestroyWindow != nullptr && pfnWinWebDiff_DestroyToolWindow != nullptr)
		{
			if (m_pWebDiffWindow != nullptr)
				pfnWinWebDiff_DestroyWindow(m_pWebDiffWindow);
			if (m_pWebToolWindow != nullptr)
				pfnWinWebDiff_DestroyToolWindow(m_pWebToolWindow);
			m_pWebDiffWindow = nullptr;
			m_pWebToolWindow = nullptr;
		}
	}
}

bool CWebPageDiffFrame::OpenDocs(int nFiles, const FileLocation fileloc[], const bool bRO[], const String strDesc[], CMDIFrameWnd *pParent, std::function<void ()> callback)
{
	CMergeFrameCommon::LogComparisonStart(nFiles, fileloc, strDesc, &m_infoUnpacker, nullptr);

	m_callbackOnOpenCompleted = callback;
	m_bCompareCompleted = false;
	
	CWaitCursor waitstatus;
	int nNormalBuffer = 0;
	for (int pane = 0; pane < nFiles; ++pane)
	{
		m_filePaths.SetPath(pane, fileloc[pane].filepath, false);
		if (paths::isFileURL(m_filePaths[pane]))
			m_filePaths[pane] = paths::FromURL(m_filePaths[pane]);
		m_bRO[pane] = bRO[pane];
		m_strDesc[pane] = strDesc ? strDesc[pane] : _T("");
		if (fileloc[pane].filepath.empty() || paths::IsNullDeviceName(fileloc[pane].filepath))
		{
			m_nBufferType[pane] = BUFFERTYPE::UNNAMED;
			if (m_strDesc[pane].empty())
				m_strDesc[pane] = (pane == 0) ? _("Untitled Left") : ((nFiles < 3 || pane == 2) ? _("Untitled Right") : _("Untitled Middle"));
			if (paths::IsNullDeviceName(fileloc[pane].filepath))
				m_filePaths.SetPath(pane, _T("about:blank"), false);
		}
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

	GetMainFrame()->WatchDocuments(this);

	return true;
}

void CWebPageDiffFrame::MoveOnLoad(int nPane, int)
{
	if (nPane < 0)
	{
		nPane = (m_nBufferType[0] != BUFFERTYPE::UNNAMED) ? GetOptionsMgr()->GetInt(OPT_ACTIVE_PANE) : 0;
		if (nPane < 0 || nPane >= m_pWebDiffWindow->GetPaneCount())
			nPane = 0;
	}

	m_pWebDiffWindow->SetActivePane(nPane);

	if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST) && m_pWebDiffWindow->GetDiffCount() > 0)
		m_pWebDiffWindow->SelectDiff(0);
}

/**
 * @brief DirDoc gives us its identity just after it creates us
 */
void CWebPageDiffFrame::SetDirDoc(IDirDoc * pDirDoc)
{
	ASSERT(pDirDoc != nullptr && m_pDirDoc == nullptr);
	m_pDirDoc = pDirDoc;
}

IMergeDoc::FileChange CWebPageDiffFrame::IsFileChangedOnDisk(int pane) const
{
	DiffFileInfo dfi;
	if (paths::IsURL(m_filePaths[pane]))
		return FileChange::NoChange;
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
	if (!m_pWebDiffWindow)
		return;
	for (int pane = 0; pane < m_pWebDiffWindow->GetPaneCount(); ++pane)
	{
		if (IsFileChangedOnDisk(pane) == FileChange::Changed)
		{
			String msg = strutils::format_string1(_("Another application updated\n%1\nsince last scan.\n\nReload?"), m_filePaths[pane]);
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
void CWebPageDiffFrame::CreateWebWndStatusBar(CStatusBar &wndStatusBar, CWnd *pwndPane)
{
	wndStatusBar.Create(pwndPane, WS_CHILD|WS_VISIBLE);
	wndStatusBar.SetIndicators(0, 1);
	wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH, 0);
	wndStatusBar.SetParent(this);
	wndStatusBar.SetWindowPos(&wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

static bool isTempFile(const String& path)
{
	String tmpDir = env::GetTemporaryPath();
	strutils::replace(tmpDir, _T("\\"), _T("/"));
	tmpDir = _T("file:///") + tmpDir;
	return (path.find(tmpDir) == 0);
}

void CWebPageDiffFrame::OnWebDiffEvent(const WebDiffEvent& event)
{
	switch (event.type)
	{
	case WebDiffEvent::SourceChanged:
	case WebDiffEvent::TabChanged:
	{
		if (m_nBufferType[event.pane] == BUFFERTYPE::UNNAMED)
		{
			m_nBufferType[event.pane] = BUFFERTYPE::NORMAL;
			m_strDesc[event.pane].clear();
		}
		const String curUrl = m_pWebDiffWindow->GetCurrentUrl(event.pane);
		if (!isTempFile(curUrl) && curUrl != _T("about:blank"))
		{
			m_filePaths[event.pane] = paths::isFileURL(curUrl) ? paths::FromURL(curUrl) : curUrl;
			UpdateHeaderPath(event.pane);
		}
		RootLogger::Info(strutils::format(_T("%s event: (pane: %d, url: %s)"),
			(event.type == WebDiffEvent::SourceChanged) ? _T("SourceChanged") : _T("TabChanged"),
			event.pane, m_filePaths[event.pane]));
		break;
	}
	case WebDiffEvent::CompareStateChanged:
		UpdateLastCompareResult();
		break;
	case WebDiffEvent::CompareScreenshotsSelected:
	case WebDiffEvent::CompareFullsizeScreenshotsSelected:
	case WebDiffEvent::CompareHTMLsSelected:
	case WebDiffEvent::CompareTextsSelected:
	case WebDiffEvent::CompareResourceTreesSelected:
		PostMessage(WM_COMMAND, ID_WEB_COMPARE_SCREENSHOTS + event.type - WebDiffEvent::CompareScreenshotsSelected, 0);
		break;
	}
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
 * @brief returns true if URL matches configured pattern
 */
bool CWebPageDiffFrame::MatchURLPattern(const String& url)
{
	const String& includePattern = GetOptionsMgr()->GetString(OPT_CMP_WEB_URL_PATTERN_TO_INCLUDE);
	if (includePattern.empty())
		return false;
	std::string textu8 = ucr::toUTF8(url);
	try
	{
		Poco::RegularExpression reInclude(ucr::toUTF8(includePattern));
		if (!reInclude.match(textu8))
			return false;
		const String& excludePattern = GetOptionsMgr()->GetString(OPT_CMP_WEB_URL_PATTERN_TO_EXCLUDE);
		if (excludePattern.empty())
			return true;
		Poco::RegularExpression reExclude(ucr::toUTF8(excludePattern));
		return !reExclude.match(textu8);
	}
	catch (Poco::RegularExpressionException& e)
	{
		RootLogger::Error(e.displayText());
		return false;
	}
}

/**
 * @brief Create the splitter, the filename bar, the status bar, and the two views
 */
BOOL CWebPageDiffFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	HMODULE hModule = nullptr;
	if (!IsLoadable() || (hModule = GetModuleHandleW(L"WinWebDiffLib.dll")) == nullptr)
	{
		RootLogger::Error(_T("Failed to load WinWebDiffLib.dll"));
		return FALSE;
	}

	IWebDiffWindow * (*pfnWinWebDiff_CreateWindow)(HINSTANCE hInstance, HWND hWndParent, int nID) =
		(IWebDiffWindow * (*)(HINSTANCE hInstance, HWND hWndParent, int nID))GetProcAddress(hModule, "WinWebDiff_CreateWindow");
	if (pfnWinWebDiff_CreateWindow == nullptr ||
		(m_pWebDiffWindow = pfnWinWebDiff_CreateWindow(hModule, m_hWnd, AFX_IDW_PANE_FIRST)) == nullptr)
	{
		FreeLibrary(hModule);
		return FALSE;
	}

	m_pWebDiffWindow->SetLogCallback(
		([](IWebDiffWindow::LogLevel level, const wchar_t* msg)
			{
				switch (level)
				{
				case IWebDiffWindow::LogLevel::ERR:
					RootLogger::Error(msg);
					break;
				case IWebDiffWindow::LogLevel::WARN:
					RootLogger::Warn(msg);
					break;
				default:
					RootLogger::Info(msg);
					break;
				}
			})
	);

	if (!m_pWebDiffWindow->IsWebView2Installed())
	{
		if (IDYES == AfxMessageBox(_("WebView2 runtime not installed. Download it?").c_str(), MB_ICONWARNING | MB_YESNO))
		{
			m_pWebDiffWindow->DownloadWebView2();
		}
		return FALSE;
	}

	m_pWebDiffWindow->AddEventListener(
		Callback<IWebDiffEventHandler>([this](const WebDiffEvent& event) -> HRESULT
		{
			OnWebDiffEvent(event);
			return S_OK;
		})
	);

	LoadOptions();

	m_pWebDiffWindow->SetUserDataFolderType(
		static_cast<IWebDiffWindow::UserdataFolderType>(GetOptionsMgr()->GetInt(OPT_CMP_WEB_USERDATAFOLDER_TYPE)),
		GetOptionsMgr()->GetBool(OPT_CMP_WEB_USERDATAFOLDER_PERPANE));

	auto callback = Callback<IWebDiffCallback>([this](const WebDiffCallbackResult& result) -> HRESULT
		{
			int nNormalBuffer = 0;
			for (int pane = 0; pane < m_pWebDiffWindow->GetPaneCount(); ++pane)
			{
				if (m_nBufferType[pane] != BUFFERTYPE::UNNAMED)
					++nNormalBuffer;
			}
			if (nNormalBuffer == 0)
				UpdateLastCompareResult();

			if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST))
				m_pWebDiffWindow->FirstDiff();

			if (m_callbackOnOpenCompleted)
			{
				m_callbackOnOpenCompleted();
				m_callbackOnOpenCompleted = nullptr;
			}
			m_bCompareCompleted = true;

			CMergeFrameCommon::LogComparisonCompleted(*this);

			return S_OK;
		});
	bool bResult;
	if (std::count(m_nBufferType, m_nBufferType + m_filePaths.GetSize(), BUFFERTYPE::UNNAMED) == m_filePaths.GetSize())
	{
		for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
			m_filePaths[pane] = _T("about:blank");
	}
	bResult = OpenUrls(callback);

	for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
		m_fileInfo[pane].Update(m_filePaths[pane]);

	// Merge frame has also a dockable bar at the very left
	// This is not the client area, but we create it now because we want
	// to use the CCreateContext
	String sCaption = I18n::LoadString(IDS_LOCBAR_CAPTION);
	if (!m_wndLocationBar.Create(this, sCaption.c_str(), WS_CHILD | WS_VISIBLE, ID_VIEW_LOCATION_BAR))
	{
		TRACE0("Failed to create LocationBar\n");
		return FALSE;
	}

	IWebToolWindow * (*pfnWinWebDiff_CreateToolWindow)(HINSTANCE hInstance, HWND hWndParent, IWebDiffWindow *) =
		(IWebToolWindow * (*)(HINSTANCE hInstance, HWND hWndParent, IWebDiffWindow *pWebPageDiffWindow))GetProcAddress(hModule, "WinWebDiff_CreateToolWindow");
	if (pfnWinWebDiff_CreateToolWindow == nullptr ||
		(m_pWebToolWindow = pfnWinWebDiff_CreateToolWindow(hModule, m_wndLocationBar.m_hWnd, m_pWebDiffWindow)) == nullptr)
	{
		return FALSE;
	}

	m_pWebToolWindow->Translate(TranslateLocationPane);

	m_wndLocationBar.SetFrameHwnd(GetSafeHwnd());
	m_pWebDiffWindow->SetDarkBackgroundEnabled(DarkMode::isEnabled());
	for (int pane = 0; pane < m_pWebDiffWindow->GetPaneCount(); ++pane)
	{
		HWND hWnd = m_pWebDiffWindow->GetPaneHWND(pane);
		if (hWnd != nullptr)
		{
			DarkMode::setWindowCtlColorSubclass(hWnd);
			DarkMode::setWindowNotifyCustomDrawSubclass(hWnd);
			DarkMode::setChildCtrlsSubclassAndThemeEx(hWnd, true, true);
			CWnd* pToolbar = FindWindowEx(hWnd, nullptr, TOOLBARCLASSNAME, nullptr);
			if (pToolbar)
				DarkMode::setWindowCtlColorSubclass(pToolbar->GetSafeHwnd());
		}
	}
	HWND hPane = m_pWebToolWindow->GetHWND();
	if (hPane != nullptr)
	{
		DarkMode::setWindowCtlColorSubclass(hPane);
		DarkMode::setWindowNotifyCustomDrawSubclass(hPane);
		DarkMode::setChildCtrlsSubclassAndThemeEx(hPane, true, true);
	}
	return TRUE;
}

void CWebPageDiffFrame::TranslateLocationPane(int id, const wchar_t *org, size_t dstbufsize, wchar_t *dst)
{
	swprintf_s(dst, dstbufsize, L"%s", I18n::tr(ucr::toUTF8(org)).c_str());
}

/////////////////////////////////////////////////////////////////////////////
// CWebPageDiffFrame message handlers

int CWebPageDiffFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMergeFrameCommon::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);

	FrameWndHelper::RemoveBarBorder(this);

	// Merge frame has a header bar at top
	if (!m_wndFilePathBar.Create(this))
	{
		TRACE0("Failed to create dialog bar\n");
		return -1;      // fail to create
	}

	m_wndFilePathBar.SetPaneCount(m_pWebDiffWindow->GetPaneCount());
	m_wndFilePathBar.SetOnSetFocusCallback([&](int pane) {
		if (m_nActivePane != pane)
			m_pWebDiffWindow->SetActivePane(pane);
	});
	m_wndFilePathBar.SetOnCaptionChangedCallback([&](int pane, const String& sText) {
		if (m_strDesc[pane] != sText)
		{
			m_strDesc[pane] = sText;
			if (m_nBufferType[pane] == BUFFERTYPE::NORMAL)
				m_nBufferType[pane] = BUFFERTYPE::NORMAL_NAMED;
			UpdateHeaderPath(pane);
		}
	});

	// Merge frame also has a dockable bar at the very left
	// created in OnCreateClient 
	m_wndLocationBar.SetBarStyle(m_wndLocationBar.GetBarStyle() |
		CBRS_SIZE_DYNAMIC | CBRS_ALIGN_LEFT);
	m_wndLocationBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar(&m_wndLocationBar, AFX_IDW_DOCKBAR_LEFT);

	for (int nPane = 0; nPane < m_pWebDiffWindow->GetPaneCount(); nPane++)
	{
		m_wndFilePathBar.SetActive(nPane, FALSE);
		//CreateWebWndStatusBar(m_wndStatusBar[nPane], CWnd::FromHandle(m_pWebDiffWindow->GetPaneHWND(nPane)));
		UpdateHeaderPath(nPane);
	}

	//CSize size = m_wndStatusBar[0].CalcFixedLayout(TRUE, TRUE);
	//m_rectBorder.bottom = size.cy;

	CDockState pDockState;
	pDockState.LoadState(_T("Settings-WebPageDiffFrame"));
	if (FrameWndHelper::EnsureValidDockState(this, pDockState)) // checks for valid so won't ASSERT
		SetDockState(pDockState);
	// for the dimensions of the diff and location pane, use the CSizingControlBar loader
	m_wndLocationBar.LoadState(_T("Settings-WebPageDiffFrame"));

	return 0;
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
	m_pWebDiffWindow->SetHorizontalSplit(GetOptionsMgr()->GetBool(OPT_SPLIT_HORIZONTALLY));
	m_pWebDiffWindow->SetZoom(GetOptionsMgr()->GetInt(OPT_CMP_WEB_ZOOM) / 1000.0);
	SIZE size{ GetOptionsMgr()->GetInt(OPT_CMP_WEB_VIEW_WIDTH), GetOptionsMgr()->GetInt(OPT_CMP_WEB_VIEW_HEIGHT) };
	m_pWebDiffWindow->SetSize(size);
	m_pWebDiffWindow->SetFitToWindow(GetOptionsMgr()->GetBool(OPT_CMP_WEB_FIT_TO_WINDOW));
	m_pWebDiffWindow->SetShowDifferences(GetOptionsMgr()->GetBool(OPT_CMP_WEB_SHOWDIFFERENCES));
	m_pWebDiffWindow->SetShowWordDifferences(GetOptionsMgr()->GetBool(OPT_WORDDIFF_HIGHLIGHT));
	m_pWebDiffWindow->SetUserAgent(GetOptionsMgr()->GetString(OPT_CMP_WEB_USER_AGENT).c_str());
	m_pWebDiffWindow->SetSyncEvents(GetOptionsMgr()->GetBool(OPT_CMP_WEB_SYNC_EVENTS));
	m_pWebDiffWindow->SetSyncEventFlags(GetOptionsMgr()->GetInt(OPT_CMP_WEB_SYNC_EVENT_FLAGS));
	COLORSETTINGS colors;
	IWebDiffWindow::ColorSettings colorSettings;
	Options::DiffColors::Load(GetOptionsMgr(), colors);
	colorSettings.clrDiff = colors.clrDiff;
	colorSettings.clrDiffText = colors.clrDiffText;
	colorSettings.clrSelDiff = colors.clrSelDiff;
	colorSettings.clrSelDiffText = colors.clrSelDiffText;
	colorSettings.clrSNP = colors.clrSNP;
	colorSettings.clrSNPText = colors.clrSNPText;
	colorSettings.clrSelSNP = colors.clrSelSNP;
	colorSettings.clrSelSNPText = colors.clrSelSNPText;
	colorSettings.clrWordDiff = colors.clrWordDiff;
	colorSettings.clrWordDiffDeleted = colors.clrWordDiffDeleted;
	colorSettings.clrWordDiffText = colors.clrWordDiffText;
	colorSettings.clrSelWordDiff = colors.clrSelWordDiff;
	colorSettings.clrSelWordDiffDeleted = colors.clrSelWordDiffDeleted;
	colorSettings.clrSelWordDiffText = colors.clrSelWordDiffText;
	m_pWebDiffWindow->SetDiffColorSettings(colorSettings);
	DIFFOPTIONS options;
	IWebDiffWindow::DiffOptions diffOptions;
	Options::DiffOptions::Load(GetOptionsMgr(), options);
	diffOptions.bFilterCommentsLines = options.bFilterCommentsLines;
	diffOptions.completelyBlankOutIgnoredChanges = options.bCompletelyBlankOutIgnoredChanges;
	diffOptions.diffAlgorithm = options.nDiffAlgorithm;
	diffOptions.ignoreBlankLines = options.bIgnoreBlankLines;
	diffOptions.ignoreCase = options.bIgnoreCase;
	diffOptions.ignoreEol = options.bIgnoreEol;
	diffOptions.ignoreNumbers = options.bIgnoreNumbers;
	diffOptions.ignoreWhitespace = options.nIgnoreWhitespace;
	m_pWebDiffWindow->SetDiffOptions(diffOptions);
}

void CWebPageDiffFrame::SaveOptions()
{
	SIZE size = m_pWebDiffWindow->GetSize();
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_VIEW_WIDTH, size.cx);
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_VIEW_HEIGHT, size.cy);
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_FIT_TO_WINDOW, m_pWebDiffWindow->GetFitToWindow());
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_SHOWDIFFERENCES, m_pWebDiffWindow->GetShowDifferences());
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_ZOOM, static_cast<int>(m_pWebDiffWindow->GetZoom() * 1000));
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_USER_AGENT, m_pWebDiffWindow->GetUserAgent());
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_SYNC_EVENTS, m_pWebDiffWindow->GetSyncEvents());
	GetOptionsMgr()->SaveOption(OPT_CMP_WEB_SYNC_EVENT_FLAGS, m_pWebDiffWindow->GetSyncEventFlags());
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
	OpenUrls(
		Callback<IWebDiffCallback>([nActivePane, this](const WebDiffCallbackResult& result) -> HRESULT
			{
				MoveOnLoad(nActivePane);
				for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
					m_fileInfo[pane].Update(m_filePaths[pane]);
				return S_OK;
			}));
}

void CWebPageDiffFrame::OnFileClose() 
{
	OnClose();
}

void CWebPageDiffFrame::OnFileRecompareAs(UINT nID)
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
		nID = GetOptionsMgr()->GetBool(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE) ? ID_MERGE_COMPARE_WEBPAGE : -ID_MERGE_COMPARE_WEBPAGE;
	}

	CloseNow();
	GetMainFrame()->DoFileOrFolderOpen(&paths, dwFlags, strDesc, _T(""),
		GetOptionsMgr()->GetBool(OPT_CMP_INCLUDE_SUBDIRS), nullptr, &infoUnpacker, nullptr, nID);
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
		fileopenflags_t dwFlags[3] = { FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, FFILEOPEN_NOMRU };
		String strDesc[3] = { m_strDesc[0], m_strDesc[1], m_strDesc[2] };
		CloseNow();
		GetMainFrame()->DoFileOpen(
			GetOptionsMgr()->GetBool(OPT_PLUGINS_OPEN_IN_SAME_FRAME_TYPE) ? ID_MERGE_COMPARE_WEBPAGE: -1,
			&paths, dwFlags, strDesc, _T(""), &infoUnpacker);
	}
}

void  CWebPageDiffFrame::OnWindowChangePane(UINT nID) 
{
	int npanes = m_pWebDiffWindow->GetPaneCount();
	int pane = m_pWebDiffWindow->GetActivePane();
	pane = (nID == ID_NEXT_PANE) ? ((pane + 1) % npanes) : ((pane + npanes - 1) % npanes);
	m_pWebDiffWindow->SetActivePane(pane);
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

		if (!std::equal(m_nLastSplitPos, m_nLastSplitPos + nPaneCount, w))
		{
			std::copy_n(w, nPaneCount, m_nLastSplitPos);

			// resize controls in header dialog bar
			m_wndFilePathBar.Resize(w);

			rc.left = rcMergeWindow.left;
			rc.top = rc.bottom - m_rectBorder.bottom;
			rc.right = rc.left;
			for (int pane = 0; pane < nPaneCount; pane++)
			{
				rc.right += w[pane] + 4 + 2;
//				m_wndStatusBar[pane].MoveWindow(&rc);
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
	String sTitle = (lpszTitle != nullptr) ? lpszTitle : CMergeFrameCommon::GetTitleString(*this);
	CMergeFrameCommon::SetTitle(sTitle.c_str());
	if (m_hWnd != nullptr)
	{
		SetWindowText(sTitle.c_str());
		GetMainFrame()->OnUpdateFrameTitle(TRUE);
	}
}

int CWebPageDiffFrame::UpdateLastCompareResult()
{
	int result = -1;
	if (m_bCompareCompleted)
	{
		result = (m_pWebDiffWindow->GetCompareState() == IWebDiffWindow::COMPARED) ? (m_pWebDiffWindow->GetDiffCount() > 0 ? 1 : 0) : -1;
		SetLastCompareResult(result);
	}
	return result;
}

void CWebPageDiffFrame::UpdateAutoPaneResize()
{
}

void CWebPageDiffFrame::UpdateSplitter()
{
}

bool CWebPageDiffFrame::OpenUrls(IWebDiffCallback* callback)
{
	bool bResult;
	String filteredFilenames = strutils::join(m_filePaths.begin(), m_filePaths.end(), _T("|"));
	String strTempFileName[3];
	m_infoUnpacker.EnableWebBrowserMode();
	for (int pane = 0; pane < m_filePaths.GetSize(); ++pane)
	{
		strTempFileName[pane] = m_filePaths[pane];
		if (!m_infoUnpacker.Unpacking(pane, &m_unpackerSubcodes[pane], strTempFileName[pane], filteredFilenames, {strTempFileName[pane]}))
		{
//			return false;
		}
	}
	if (m_filePaths.GetSize() == 2)
		bResult = SUCCEEDED(m_pWebDiffWindow->Open(ucr::toUTF16(strTempFileName[0]).c_str(), ucr::toUTF16(strTempFileName[1]).c_str(), callback));
	else
		bResult = SUCCEEDED(m_pWebDiffWindow->Open(ucr::toUTF16(strTempFileName[0]).c_str(), ucr::toUTF16(strTempFileName[1]).c_str(), ucr::toUTF16(strTempFileName[2]).c_str(), callback));
	return bResult;
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
	return CMergeFrameCommon::GetTooltipString(*this).c_str();
}

/**
 * @brief Returns the number of differences found
 */
int CWebPageDiffFrame::GetDiffCount() const
{
	return m_pWebDiffWindow->GetDiffCount();
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CWebPageDiffFrame::UpdateResources()
{
	m_pWebToolWindow->Translate(TranslateLocationPane);
}

void CWebPageDiffFrame::RefreshOptions()
{
	LoadOptions();
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
			return false;
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
	tchar_t sIdx[32] = { 0 };
	tchar_t sCnt[32] = { 0 };
	String s;
	auto compareState = m_pWebDiffWindow->GetCompareState();

	if (compareState == IWebDiffWindow::NOT_COMPARED)
	{
	}
	else if (compareState == IWebDiffWindow::COMPARING)
	{
		s = I18n::LoadString(IDS_WEBPAGE_COMPARING);
	}
	else
	{
		s = CMergeFrameCommon::GetDiffStatusString(m_pWebDiffWindow->GetCurrentDiffIndex(), m_pWebDiffWindow->GetDiffCount());
	}
	pCmdUI->SetText(s.c_str());
}
	
/**
 * @brief Cut current selection to clipboard
 */
void CWebPageDiffFrame::OnEditCut()
{
	m_pWebDiffWindow->Cut();
}

/**
 * @brief Copy current selection to clipboard
 */
void CWebPageDiffFrame::OnEditCopy()
{
	m_pWebDiffWindow->Copy();
}

/**
 * @brief Paste clipboard content over selected content
 */
void CWebPageDiffFrame::OnEditPaste()
{
	m_pWebDiffWindow->Paste();
}

/**
 * @brief Undo last action
 */
void CWebPageDiffFrame::OnEditUndo()
{
	m_pWebDiffWindow->Undo();
}

/**
 * @brief Redo last action
 */
void CWebPageDiffFrame::OnEditRedo()
{
	m_pWebDiffWindow->Redo();
}

/**
 * @brief Update the tool bar's "Undo" icon. It should be enabled when
 * renaming an item and undo is possible.
 */
void CWebPageDiffFrame::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pWebDiffWindow->CanUndo());
}

/**
 * @brief Update the tool bar's "Redo" icon. It should be enabled when
 * renaming an item and undo is possible.
 */
void CWebPageDiffFrame::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pWebDiffWindow->CanRedo());
}

/**
 * @brief Select entire image
 */
void CWebPageDiffFrame::OnEditSelectAll()
{
	m_pWebDiffWindow->SelectAll();
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
 * @brief Enables/disables linediff (different color for diffs)
 */
void CWebPageDiffFrame::OnViewLineDiffs() 
{
	bool bWordDiffHighlight = !GetOptionsMgr()->GetBool(OPT_WORDDIFF_HIGHLIGHT);
	GetOptionsMgr()->SaveOption(OPT_WORDDIFF_HIGHLIGHT, bWordDiffHighlight);
	m_pWebDiffWindow->SetShowWordDifferences(bWordDiffHighlight);
}

void CWebPageDiffFrame::OnUpdateViewLineDiffs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(GetOptionsMgr()->GetBool(OPT_WORDDIFF_HIGHLIGHT));
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
	bool enabled = m_bCompareCompleted && (
		m_pWebDiffWindow->GetNextDiffIndex() >= 0 ||
		(m_pWebDiffWindow->GetDiffCount() > 0 && m_pWebDiffWindow->GetCurrentDiffIndex() == -1));

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
	bool enabled = m_bCompareCompleted && (
		m_pWebDiffWindow->GetPrevDiffIndex() >= 0 ||
		(m_pWebDiffWindow->GetDiffCount() > 0 && m_pWebDiffWindow->GetCurrentDiffIndex() == -1));

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
	pCmdUI->Enable(m_bCompareCompleted && 
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
	pCmdUI->Enable(m_bCompareCompleted &&
		m_pWebDiffWindow->GetPaneCount() > 2 && (
			m_pWebDiffWindow->GetPrevConflictIndex() >= 0 ||
			(m_pWebDiffWindow->GetConflictCount() > 0 && m_pWebDiffWindow->GetCurrentDiffIndex() == -1)
		)
	);
}

void CWebPageDiffFrame::OnWebViewDifferences()
{
	m_pWebDiffWindow->SetShowDifferences(!m_pWebDiffWindow->GetShowDifferences());
	SaveOptions();
	m_pWebToolWindow->Sync();
}

void CWebPageDiffFrame::OnUpdateWebViewDifferences(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_pWebDiffWindow->GetShowDifferences());
}

void CWebPageDiffFrame::OnWebFitToWindow()
{
	m_pWebDiffWindow->SetFitToWindow(true);
	SaveOptions();
	m_pWebToolWindow->Sync();
}

void CWebPageDiffFrame::OnUpdateWebFitToWindow(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_pWebDiffWindow->GetFitToWindow());
}

void CWebPageDiffFrame::OnWebSize(UINT nID)
{
	switch (nID)
	{
	case ID_WEB_SIZE_320x512:
		m_pWebDiffWindow->SetFitToWindow(false);
		m_pWebDiffWindow->SetSize({ 320, 512 });
		m_pWebToolWindow->Sync();
		break;
	case ID_WEB_SIZE_375x600:
		m_pWebDiffWindow->SetFitToWindow(false);
		m_pWebDiffWindow->SetSize({ 375, 600 });
		m_pWebToolWindow->Sync();
		break;
	case ID_WEB_SIZE_1024x640:
		m_pWebDiffWindow->SetFitToWindow(false);
		m_pWebDiffWindow->SetSize({ 1024, 640 });
		SaveOptions();
		m_pWebToolWindow->Sync();
		break;
	case ID_WEB_SIZE_1280x800:
		m_pWebDiffWindow->SetFitToWindow(false);
		m_pWebDiffWindow->SetSize({ 1280, 800 });
		SaveOptions();
		m_pWebToolWindow->Sync();
		break;
	case ID_WEB_SIZE_1440x900:
		m_pWebDiffWindow->SetFitToWindow(false);
		m_pWebDiffWindow->SetSize({ 1440, 900 });
		SaveOptions();
		m_pWebToolWindow->Sync();
		break;
	}
}

void CWebPageDiffFrame::OnWebCompareScreenshots(UINT nID)
{
	std::shared_ptr<CWaitCursor> pWaitStatus{ new CWaitCursor() };
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
	m_pWebDiffWindow->SaveFiles(
		(nID == ID_WEB_COMPARE_FULLSIZE_SCREENSHOTS) ? IWebDiffWindow::FULLSIZE_SCREENSHOT : IWebDiffWindow::SCREENSHOT,
		spaths,
		Callback<IWebDiffCallback>([paths, descs, pWaitStatus](const WebDiffCallbackResult& result) -> HRESULT
			{
				fileopenflags_t dwFlags[3] = { FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, FFILEOPEN_NOMRU };
				GetMainFrame()->DoFileOpen(0, &paths, dwFlags, descs.data());
				return S_OK;
			}));
}

void CWebPageDiffFrame::OnWebCompareHTMLs()
{
	std::shared_ptr<CWaitCursor> pWaitStatus{ new CWaitCursor() };
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
	m_pWebDiffWindow->SaveFiles(IWebDiffWindow::HTML, spaths,
		Callback<IWebDiffCallback>([paths, descs, pWaitStatus](const WebDiffCallbackResult& result) -> HRESULT
			{
				PackingInfo infoUnpacker(String(_T("PrettifyHTML")));
				fileopenflags_t dwFlags[3] = { FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, FFILEOPEN_NOMRU };
				GetMainFrame()->DoFileOpen(0, &paths, dwFlags, descs.data(), _T(""), &infoUnpacker);
				return S_OK;
			}));
}

void CWebPageDiffFrame::OnWebCompareTexts()
{
	std::shared_ptr<CWaitCursor> pWaitStatus{ new CWaitCursor() };
	PathContext paths;
	const wchar_t *spaths[3];
	std::vector<String> descs;
	const int nPanes = m_pWebDiffWindow->GetPaneCount();
	for (int pane = 0; pane < nPanes; ++pane)
	{
		std::shared_ptr<TempFile> pTempFile(new TempFile());
		pTempFile->Create(_T("TXT"), _T(".txt"));
		paths.SetPath(pane, pTempFile->GetPath());
		spaths[pane] = paths[pane].c_str();
		descs.push_back(m_filePaths[pane]);
		m_tempFiles.push_back(pTempFile);
	}
	m_pWebDiffWindow->SaveFiles(IWebDiffWindow::TEXT, spaths,
		Callback<IWebDiffCallback>([paths, descs, pWaitStatus](const WebDiffCallbackResult& result) -> HRESULT
			{
				fileopenflags_t dwFlags[3] = { FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, FFILEOPEN_NOMRU };
				GetMainFrame()->DoFileOpen(0, &paths, dwFlags, descs.data(), _T(""));
				return S_OK;
			}));
}

void CWebPageDiffFrame::OnWebCompareResourceTrees()
{
	std::shared_ptr<CWaitCursor> pWaitStatus{ new CWaitCursor() };
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
	m_pWebDiffWindow->SaveFiles(IWebDiffWindow::RESOURCETREE, spaths,
		Callback<IWebDiffCallback>([paths, descs, pWaitStatus](const WebDiffCallbackResult& result) -> HRESULT
			{
				fileopenflags_t dwFlags[3]{};
				for (int pane = 0; pane < paths.GetSize(); ++pane)
					dwFlags[pane] = FFILEOPEN_NOMRU;
				GetMainFrame()->DoFileOrFolderOpen(&paths, dwFlags, descs.data(), _T(""), true);
				return S_OK;
			}));
}

void CWebPageDiffFrame::OnWebSyncEvent(UINT nID)
{
	switch (nID)
	{
	case ID_WEB_SYNC_ENABLED:
		m_pWebDiffWindow->SetSyncEvents(!m_pWebDiffWindow->GetSyncEvents());
		GetOptionsMgr()->SaveOption(OPT_CMP_WEB_SYNC_EVENTS, m_pWebDiffWindow->GetSyncEvents());
		break;
	case ID_WEB_SYNC_SCROLL:
		m_pWebDiffWindow->SetSyncEventFlag(IWebDiffWindow::EVENT_SCROLL,
			!m_pWebDiffWindow->GetSyncEventFlag(IWebDiffWindow::EVENT_SCROLL));
		GetOptionsMgr()->SaveOption(OPT_CMP_WEB_SYNC_EVENT_FLAGS, m_pWebDiffWindow->GetSyncEventFlags());
		break;
	case ID_WEB_SYNC_CLICK:
		m_pWebDiffWindow->SetSyncEventFlag(IWebDiffWindow::EVENT_CLICK,
			!m_pWebDiffWindow->GetSyncEventFlag(IWebDiffWindow::EVENT_CLICK));
		GetOptionsMgr()->SaveOption(OPT_CMP_WEB_SYNC_EVENT_FLAGS, m_pWebDiffWindow->GetSyncEventFlags());
		break;
	case ID_WEB_SYNC_INPUT:
		m_pWebDiffWindow->SetSyncEventFlag(IWebDiffWindow::EVENT_INPUT,
			!m_pWebDiffWindow->GetSyncEventFlag(IWebDiffWindow::EVENT_INPUT));
		GetOptionsMgr()->SaveOption(OPT_CMP_WEB_SYNC_EVENT_FLAGS, m_pWebDiffWindow->GetSyncEventFlags());
		break;
	case ID_WEB_SYNC_GOBACKFORWARD:
		m_pWebDiffWindow->SetSyncEventFlag(IWebDiffWindow::EVENT_GOBACKFORWARD,
			!m_pWebDiffWindow->GetSyncEventFlag(IWebDiffWindow::EVENT_GOBACKFORWARD));
		GetOptionsMgr()->SaveOption(OPT_CMP_WEB_SYNC_EVENT_FLAGS, m_pWebDiffWindow->GetSyncEventFlags());
		break;
	}
}

void CWebPageDiffFrame::OnUpdateWebSyncEvent(CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
	case ID_WEB_SYNC_ENABLED:
		pCmdUI->SetCheck(m_pWebDiffWindow->GetSyncEvents());
		break;
	case ID_WEB_SYNC_SCROLL:
		pCmdUI->SetCheck(m_pWebDiffWindow->GetSyncEventFlag(IWebDiffWindow::EVENT_SCROLL));
		break;
	case ID_WEB_SYNC_CLICK:
		pCmdUI->SetCheck(m_pWebDiffWindow->GetSyncEventFlag(IWebDiffWindow::EVENT_CLICK));
		break;
	case ID_WEB_SYNC_INPUT:
		pCmdUI->SetCheck(m_pWebDiffWindow->GetSyncEventFlag(IWebDiffWindow::EVENT_INPUT));
		break;
	case ID_WEB_SYNC_GOBACKFORWARD:
		pCmdUI->SetCheck(m_pWebDiffWindow->GetSyncEventFlag(IWebDiffWindow::EVENT_GOBACKFORWARD));
		break;
	}
}

void CWebPageDiffFrame::OnWebClear(UINT nID)
{
	IWebDiffWindow::BrowsingDataType dataKinds;
	switch (nID)
	{
	case ID_WEB_CLEAR_DISK_CACHE:       dataKinds = IWebDiffWindow::BrowsingDataType::DISK_CACHE; break;
	case ID_WEB_CLEAR_COOKIES:          dataKinds = IWebDiffWindow::BrowsingDataType::COOKIES; break;
	case ID_WEB_CLEAR_BROWSING_HISTORY: dataKinds = IWebDiffWindow::BrowsingDataType::BROWSING_HISTORY; break;
	case ID_WEB_CLEAR_ALL_PROFILE:      dataKinds = IWebDiffWindow::BrowsingDataType::ALL_PROFILE; break;
	default:
		return;
	}
	m_pWebDiffWindow->ClearBrowsingData(-1, dataKinds);
}

bool CWebPageDiffFrame::GenerateReport(const String& sFileName) const
{
	bool result = false;
	bool completed = false;
	if (!GenerateReport(sFileName, [&completed, &result](bool res) { result = res; completed = true; }))
		return false;
	CMainFrame::WaitAndDoMessageLoop(completed, 0);
	return result;
}

bool CWebPageDiffFrame::GenerateReport(const String& sFileName, std::function<void(bool)> callback) const
{
	String rptdir_full, rptdir, path, name, ext;
	String title[3];
	String diffrpt_filename[3];
	String diffrpt_filename_full[3];
	const wchar_t* pfilenames[3]{};
	paths::SplitFilename(sFileName, &path, &name, &ext);
	rptdir_full = paths::ConcatPath(path, name) + _T(".files");
	rptdir = paths::FindFileName(rptdir_full);
	paths::CreateIfNeeded(rptdir_full);

	for (int pane = 0; pane < m_pWebDiffWindow->GetPaneCount(); ++pane)
	{
		title[pane] = m_strDesc[pane].empty() ? ucr::toTString(m_pWebDiffWindow->GetCurrentUrl(pane)) : m_strDesc[pane];
		diffrpt_filename[pane] = strutils::format(_T("%s/%d.pdf"), rptdir, pane + 1);
		diffrpt_filename_full[pane] = strutils::format(_T("%s/%d.pdf"), rptdir_full, pane + 1);
		pfilenames[pane] = diffrpt_filename_full[pane].c_str();
	}

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
		_T("<meta charset=\"UTF-8\">\n")
		_T("<title>WinMerge Webpage Compare Report</title>\n")
		_T("<style>\n")
		_T("table { table-layout: fixed; width: 100%; border-collapse: collapse; }\n")
		_T("th {position: sticky; top: 0;}\n")
		_T("td,th { border: solid 1px black; }\n")
		_T("embed { width: 100%; height: calc(100vh - 56px) }\n")
		_T(".title { color: white; background-color: blue; vertical-align: top; padding: 4px 4px; background: linear-gradient(mediumblue, darkblue);}\n")
		_T("</style>\n")
		_T("</head>\n")
		_T("<body>\n")
		_T("<table>\n")
		_T("<tr>\n"));
	for (int pane = 0; pane < m_pWebDiffWindow->GetPaneCount(); ++pane)
		file.WriteString(strutils::format(_T("<th class=\"title\">%s</th>\n"), title[pane]));
	file.WriteString(_T("</tr>\n"));
	file.WriteString(
		_T("<tr>\n"));
	for (int pane = 0; pane < m_pWebDiffWindow->GetPaneCount(); ++pane)
		file.WriteString(
			strutils::format(_T("<td><embed type=\"application/pdf\" src=\"%s\" title=\"%s\"></td>\n"),
				paths::urlEncodeFileName(diffrpt_filename[pane]), diffrpt_filename[pane]));
	file.WriteString(
		_T("</tr>\n"));
	file.WriteString(
		_T("</table>\n")
		_T("</body>\n")
		_T("</html>\n"));

	return SUCCEEDED(m_pWebDiffWindow->SaveDiffFiles(IWebDiffWindow::PDF, pfilenames, 
		Callback<IWebDiffCallback>([this, callback](const WebDiffCallbackResult& result) -> HRESULT
			{
				callback(SUCCEEDED(result.errorCode));
				return S_OK;
			})));
}

/**
 * @brief Generate report from file compare results.
 */
void CWebPageDiffFrame::OnToolsGenerateReport()
{
	String s;
	CString folder;
	if (!SelectFile(AfxGetMainWnd()->GetSafeHwnd(), s, false, folder, _T(""), _("HTML Files (*.htm,*.html)|*.htm;*.html|All Files (*.*)|*.*||"), _T("htm")))
		return;

	CWaitCursor waitstatus;
	if (GenerateReport(s))
		I18n::MessageBox(IDS_REPORT_SUCCESS, MB_OK | MB_ICONINFORMATION | MB_MODELESS);
}

void CWebPageDiffFrame::OnRefresh()
{
	if (!m_bCompareCompleted)
		return;
	m_bCompareCompleted = false;
	m_pWebDiffWindow->Recompare(
		Callback<IWebDiffCallback>([this](const WebDiffCallbackResult& result) -> HRESULT
			{
				m_bCompareCompleted = true;
				if (UpdateLastCompareResult() == 0 &&
				    std::count(m_filePaths.begin(), m_filePaths.end(), L"about:blank") != m_filePaths.GetSize())
				{
					CMergeFrameCommon::ShowIdenticalMessage(m_filePaths, true);
				}
				return S_OK;
			})
		);
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
	CMergeApp::ShowHelp(WebPageDiffFrameHelpLocation);
}

/**
 * @brief Called when the system settings change.
 */
void CWebPageDiffFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	if (m_pWebToolWindow && WinMergeDarkMode::IsImmersiveColorSet(lpszSection))
	{
		for (int pane = 0; pane < m_pWebDiffWindow->GetPaneCount(); ++pane)
		{
			HWND hWnd = m_pWebDiffWindow->GetPaneHWND(pane);
			if (hWnd != nullptr)
			{
				DarkMode::setWindowCtlColorSubclass(hWnd);
				DarkMode::setWindowNotifyCustomDrawSubclass(hWnd);
				DarkMode::setChildCtrlsSubclassAndThemeEx(hWnd, true, true);
				CWnd* pToolbar = FindWindowEx(hWnd, nullptr, TOOLBARCLASSNAME, nullptr);
				if (pToolbar)
					DarkMode::setWindowCtlColorSubclass(pToolbar->GetSafeHwnd());
				::InvalidateRect(hWnd, nullptr, TRUE);
			}
		}
		m_pWebDiffWindow->SetDarkBackgroundEnabled(DarkMode::isEnabled());
		HWND hPane = m_pWebToolWindow->GetHWND();
		if (hPane != nullptr)
		{
			DarkMode::setWindowCtlColorSubclass(hPane);
			DarkMode::setWindowNotifyCustomDrawSubclass(hPane);
			DarkMode::setChildCtrlsSubclassAndThemeEx(hPane, true, true);
			::InvalidateRect(hPane, nullptr, TRUE);
		}
	}
	__super::OnSettingChange(uFlags, lpszSection);
}

