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
 * @file  Merge.cpp
 *
 * @brief Defines the class behaviors for the application.
 *
 */

#include "stdafx.h"
#include "Merge.h"
#include "Constants.h"
#include "UnicodeString.h"
#include "unicoder.h"
#include "Environment.h"
#include "OptionsMgr.h"
#include "OptionsInit.h"
#include "RegOptionsMgr.h"
#include "OpenDoc.h"
#include "OpenFrm.h"
#include "OpenView.h"
#include "HexMergeDoc.h"
#include "HexMergeFrm.h"
#include "HexMergeView.h"
#include "AboutDlg.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "DirFrame.h"
#include "MergeDoc.h"
#include "DirDoc.h"
#include "DirView.h"
#include "PropBackups.h"
#include "FileOrFolderSelect.h"
#include "paths.h"
#include "FileFilterHelper.h"
#include "LineFiltersList.h"
#include "FilterCommentsManager.h"
#include "SyntaxColors.h"
#include "CCrystalTextMarkers.h"
#include "OptionsSyntaxColors.h"
#include "Plugins.h"
#include "ProjectFile.h"
#include "MergeEditView.h"
#include "LanguageSelect.h"
#include "OptionsDef.h"
#include "MergeCmdLineInfo.h"
#include "ConflictFileParser.h"
#include "JumpList.h"
#include "stringdiffs.h"
#include "TFile.h"
#include "SourceControl.h"
#include "paths.h"
#include "CompareStats.h"
#include "TestMain.h"

// For shutdown cleanup
#include "charsets.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



/** @brief Location for command line help to open. */
static TCHAR CommandLineHelpLocation[] = _T("::/htmlhelp/Command_line.html");

// registry dir to WinMerge
static String f_RegDir = _T("Software\\Thingamahoochie\\WinMerge");

/** @brief Backup file extension. */
static const TCHAR BACKUP_FILE_EXT[] = _T("bak");

#ifndef WIN64
/**
 * @brief Turn STL exceptions into MFC exceptions.
 * Based on the article "Visual C++ Exception-Handling Instrumentation"
 * by Eugene Gershnik, published at http://www.drdobbs.com/184416600.
 * Rethrow fix inspired by http://www.spinics.net/lists/wine/msg05996.html.
 */
/*
namespace Turn_STL_exceptions_into_MFC_exceptions
{
#	ifndef _STATIC_CPPLIB
#	error This hack only works with _STATIC_CPPLIB defined.
#	endif

	class CDisguisedSTLException : public CException
	{
	private:
		std::exception *m_pSTLException;
	public:
		CDisguisedSTLException(std::exception *pSTLException)
		: m_pSTLException(pSTLException)
		{
		}
		virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT)
		{
			StringCchPrintf(lpszError, nMaxError, _T("%hs"), m_pSTLException->what());
			return TRUE;
		}
	};

	const DWORD CPP_EXCEPTION = 0xE06D7363;
	const DWORD MS_MAGIC = 0x19930520;

	extern "C" void __stdcall _CxxThrowException(void *pObject, _s__ThrowInfo const *pObjectInfo)
	{
		__declspec(thread) static ULONG_PTR args[3] = { MS_MAGIC, 0, 0 };
		if (pObject == NULL)
		{
			pObject = reinterpret_cast<void *>(args[1]);
			pObjectInfo = reinterpret_cast<_s__ThrowInfo const *>(args[2]);
		}
		else
		{
			args[1] = (ULONG_PTR)pObject;
			args[2] = (ULONG_PTR)pObjectInfo;
		}
		int i;
		if (pObjectInfo->pCatchableTypeArray && (i = pObjectInfo->pCatchableTypeArray->nCatchableTypes))
		{
			const char *name = typeid(std::exception).raw_name();
			if (pObjectInfo->pCatchableTypeArray->arrayOfCatchableTypes[i - 1]->pType->name == name)
			{
				throw new CDisguisedSTLException(static_cast<std::exception *>(pObject));
			}
		}
		RaiseException(CPP_EXCEPTION, EXCEPTION_NONCONTINUABLE, sizeof(args)/sizeof(args[0]), args);
	}
}
*/
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeApp

BEGIN_MESSAGE_MAP(CMergeApp, CWinApp)
	//{{AFX_MSG_MAP(CMergeApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND_EX_RANGE(ID_FILE_PROJECT_MRU_FIRST, ID_FILE_PROJECT_MRU_LAST, OnOpenRecentFile)
	ON_UPDATE_COMMAND_UI(ID_FILE_PROJECT_MRU_FIRST, &CWinApp::OnUpdateRecentFileMenu)
	ON_COMMAND(ID_FILE_MERGINGMODE, OnMergingMode)
	ON_UPDATE_COMMAND_UI(ID_FILE_MERGINGMODE, OnUpdateMergingMode)
	ON_UPDATE_COMMAND_UI(ID_STATUS_MERGINGMODE, OnUpdateMergingStatus)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/**
* @brief Mapping from command line argument name (eg, ignorews) to WinMerge
* option name (eg, Settings/IgnoreSpace).
*
* These arguments take an optional colon and number, like so:
*
*  "/ignoreblanklines"  (makes WinMerge ignore blank lines)
*  "/ignoreblanklines:1"  (makes WinMerge ignore blank lines)
*  "/ignoreblanklines:0"  (makes WinMerge not ignore blank lines)
*/
struct ArgSetting
{
	LPCTSTR CmdArgName;
	LPCTSTR WinMergeOptionName;
};


/////////////////////////////////////////////////////////////////////////////
// CMergeApp construction

CMergeApp::CMergeApp() :
  m_bNeedIdleTimer(FALSE)
, m_pOpenTemplate(0)
, m_pDiffTemplate(0)
, m_pHexMergeTemplate(0)
, m_pDirTemplate(0)
, m_mainThreadScripts(NULL)
, m_nLastCompareResult(0)
, m_bNonInteractive(false)
, m_pOptions(new CRegOptionsMgr())
, m_pGlobalFileFilter(new FileFilterHelper())
, m_nActiveOperations(0)
, m_pLangDlg(new CLanguageSelect())
, m_bEscShutdown(FALSE)
, m_bClearCaseTool(FALSE)
, m_bExitIfNoDiff(MergeCmdLineInfo::Disabled)
, m_pLineFilters(new LineFiltersList())
, m_pFilterCommentsManager(new FilterCommentsManager())
, m_pSyntaxColors(new SyntaxColors())
, m_pMarkers(new CCrystalTextMarkers())
, m_pSourceControl(new SourceControl())
, m_bMergingMode(FALSE)
{
	// add construction code here,
	// Place all significant initialization in InitInstance
}

CMergeApp::~CMergeApp()
{
	strdiff::Close();
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CMergeApp object

CMergeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMergeApp initialization

/**
 * @brief Initialize WinMerge application instance.
 * @return TRUE if application initialization succeeds (and we'll run it),
 *   FALSE if something failed and we exit the instance.
 * @todo We could handle these failure situations more gratefully, i.e. show
 *  at least some error message to the user..
 */
BOOL CMergeApp::InitInstance()
{
	// Prevents DLL hijacking
	HMODULE hLibrary = GetModuleHandle(_T("kernel32.dll"));
	BOOL (WINAPI *pfnSetSearchPathMode)(DWORD) = (BOOL (WINAPI *)(DWORD))GetProcAddress(hLibrary, "SetSearchPathMode");
	if (pfnSetSearchPathMode)
		pfnSetSearchPathMode(0x00000001L /*BASE_SEARCH_PATH_ENABLE_SAFE_SEARCHMODE*/ | 0x00008000L /*BASE_SEARCH_PATH_PERMANENT*/);
	BOOL (WINAPI *pfnSetDllDirectoryA)(LPCSTR) = (BOOL (WINAPI *)(LPCSTR))GetProcAddress(hLibrary, "SetDllDirectoryA");
	if (pfnSetDllDirectoryA)
		pfnSetDllDirectoryA("");

	JumpList::SetCurrentProcessExplicitAppUserModelID(L"Thingamahoochie.WinMerge");

	InitCommonControls();    // initialize common control library
	CWinApp::InitInstance(); // call parent class method

	// Runtime switch so programmer may set this in interactive debugger
	int dbgmem = 0;
	if (dbgmem)
	{
		// get current setting
		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
		// Keep freed memory blocks in the heap's linked list and mark them as freed
		tmpFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
		// Call _CrtCheckMemory at every allocation and deallocation request.
		// WARNING: This slows down WinMerge *A LOT*
		tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
		// Set the new state for the flag
		_CrtSetDbgFlag( tmpFlag );
	}

	// CCrystalEdit Drag and Drop functionality needs AfxOleInit.
	if(!AfxOleInit())
	{
		TRACE(_T("AfxOleInitFailed. OLE functionality disabled"));
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Revoke the standard OLE Message Filter to avoid drawing frame while loading files.
	COleMessageFilter* pOldFilter = AfxOleGetMessageFilter();
	pOldFilter->Revoke();

	// Load registry keys from WinMerge.reg if existing WinMerge.reg
	env::LoadRegistryFromFile(paths::ConcatPath(env::GetProgPath(), _T("WinMerge.reg")));

	// Parse command-line arguments.
	MergeCmdLineInfo cmdInfo(GetCommandLine());
	if (cmdInfo.m_bNoPrefs)
		m_pOptions->SetSerializing(false); // Turn off serializing to registry.

	Options::Init(m_pOptions.get()); // Implementation in OptionsInit.cpp

	for (const auto& it : cmdInfo.m_Options)
		m_pOptions->Set(it.first, it.second);

	// Initialize temp folder
	SetupTempPath();

	// If paths were given to commandline we consider this being an invoke from
	// commandline (from other application, shellextension etc).
	BOOL bCommandLineInvoke = cmdInfo.m_Files.GetSize() > 0;

	// WinMerge registry settings are stored under HKEY_CURRENT_USER/Software/Thingamahoochie
	// This is the name of the company of the original author (Dean Grimm)
	SetRegistryKey(_T("Thingamahoochie"));

	BOOL bSingleInstance = GetOptionsMgr()->GetBool(OPT_SINGLE_INSTANCE) ||
		(true == cmdInfo.m_bSingleInstance);

	// Create exclusion mutex name
	TCHAR szDesktopName[MAX_PATH] = _T("Win9xDesktop");
	DWORD dwLengthNeeded;
	GetUserObjectInformation(GetThreadDesktop(GetCurrentThreadId()), UOI_NAME, 
		szDesktopName, sizeof(szDesktopName), &dwLengthNeeded);
	TCHAR szMutexName[MAX_PATH + 40];
	// Combine window class name and desktop name to form a unique mutex name.
	// As the window class name is decorated to distinguish between ANSI and
	// UNICODE build, so will be the mutex name.
	wsprintf(szMutexName, _T("%s-%s"), CMainFrame::szClassName, szDesktopName);
	HANDLE hMutex = CreateMutex(NULL, FALSE, szMutexName);
	if (hMutex)
		WaitForSingleObject(hMutex, INFINITE);
	if (bSingleInstance && GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// Activate previous instance and send commandline to it
		HWND hWnd = FindWindow(CMainFrame::szClassName, NULL);
		if (hWnd)
		{
			if (IsIconic(hWnd))
				ShowWindow(hWnd, SW_RESTORE);
			SetForegroundWindow(GetLastActivePopup(hWnd));
			LPTSTR cmdLine = GetCommandLine();
			COPYDATASTRUCT data = { 0, (lstrlen(cmdLine) + 1) * sizeof(TCHAR), cmdLine};
			if (SendMessage(hWnd, WM_COPYDATA, NULL, (LPARAM)&data))
			{
				ReleaseMutex(hMutex);
				CloseHandle(hMutex);
				return FALSE;
			}
		}
	}

	LoadStdProfileSettings(GetOptionsMgr()->GetInt(OPT_MRU_MAX));  // Load standard INI file options (including MRU)

	InitializeFileFilters();

	// Read last used filter from registry
	// If filter fails to set, reset to default
	const String filterString = m_pOptions->GetString(OPT_FILEFILTER_CURRENT);
	BOOL bFilterSet = m_pGlobalFileFilter->SetFilter(filterString);
	if (!bFilterSet)
	{
		String filter = m_pGlobalFileFilter->GetFilterNameOrMask();
		m_pOptions->SaveOption(OPT_FILEFILTER_CURRENT, filter);
	}

	charsets_init();
	UpdateCodepageModule();

	if (m_pSourceControl)
		m_pSourceControl->InitializeSourceControlMembers();

	FileTransform::g_bUnpackerMode = theApp.GetProfileInt(_T("Settings"), _T("UnpackerMode"), PLUGIN_MANUAL);
	FileTransform::g_bPredifferMode = theApp.GetProfileInt(_T("Settings"), _T("PredifferMode"), PLUGIN_MANUAL);

	NONCLIENTMETRICS ncm = { sizeof NONCLIENTMETRICS };
	if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof NONCLIENTMETRICS, &ncm, 0))
	{
		const int lfHeight = -MulDiv(9, CClientDC(CWnd::GetDesktopWindow()).GetDeviceCaps(LOGPIXELSY), 72);
		if (abs(ncm.lfMenuFont.lfHeight) > abs(lfHeight))
			ncm.lfMenuFont.lfHeight = lfHeight;
		if (wcscmp(ncm.lfMenuFont.lfFaceName, L"Meiryo") == 0 || wcscmp(ncm.lfMenuFont.lfFaceName, L"\U000030e1\U000030a4\U000030ea\U000030aa"/* "Meiryo" in Japanese */) == 0)
			wcscpy_s(ncm.lfMenuFont.lfFaceName, L"Meiryo UI");
		m_fontGUI.CreateFontIndirect(&ncm.lfMenuFont);
	}

	if (m_pSyntaxColors)
		Options::SyntaxColors::Load(GetOptionsMgr(), m_pSyntaxColors.get());

	if (m_pMarkers)
		m_pMarkers->LoadFromRegistry();

	if (m_pLineFilters)
		m_pLineFilters->Initialize(GetOptionsMgr());

	// If there are no filters loaded, and there is filter string in previous
	// option string, import old filters to new place.
	if (m_pLineFilters->GetCount() == 0)
	{
		String oldFilter = theApp.GetProfileString(_T("Settings"), _T("RegExps"));
		if (!oldFilter.empty())
			m_pLineFilters->Import(oldFilter);
	}

	// Check if filter folder is set, and create it if not
	String pathMyFolders = GetOptionsMgr()->GetString(OPT_FILTER_USERPATH);
	if (pathMyFolders.empty())
	{
		// No filter path, set it to default and make sure it exists.
		pathMyFolders = GetOptionsMgr()->GetDefault<String>(OPT_FILTER_USERPATH);
		GetOptionsMgr()->SaveOption(OPT_FILTER_USERPATH, pathMyFolders);
		theApp.m_pGlobalFileFilter->SetUserFilterPath(pathMyFolders);
	}
	if (!paths::CreateIfNeeded(pathMyFolders))
	{
		// Failed to create a folder, check it didn't already
		// exist.
		DWORD errCode = GetLastError();
		if (errCode != ERROR_ALREADY_EXISTS)
		{
			// Failed to create a folder for filters, fallback to
			// "My Documents"-folder. It is not worth the trouble to
			// bother user about this or user more clever solutions.
			GetOptionsMgr()->SaveOption(OPT_FILTER_USERPATH, env::GetMyDocuments());
		}
	}

	strdiff::Init(); // String diff init
	strdiff::SetBreakChars(GetOptionsMgr()->GetString(OPT_BREAK_SEPARATORS).c_str());

	m_bMergingMode = GetOptionsMgr()->GetBool(OPT_MERGE_MODE);

	// Initialize i18n (multiple language) support

	m_pLangDlg->InitializeLanguage((WORD)GetOptionsMgr()->GetInt(OPT_SELECTED_LANGUAGE));

	m_mainThreadScripts = new CAssureScriptsForThread;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	// Open view
	m_pOpenTemplate = new CMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(COpenDoc),
		RUNTIME_CLASS(COpenFrame), // custom MDI child frame
		RUNTIME_CLASS(COpenView));
	AddDocTemplate(m_pOpenTemplate);

	// Merge Edit view
	m_pDiffTemplate = new CMultiDocTemplate(
		IDR_MERGEDOCTYPE,
		RUNTIME_CLASS(CMergeDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMergeEditView));
	AddDocTemplate(m_pDiffTemplate);

	// Merge Edit view
	m_pHexMergeTemplate = new CMultiDocTemplate(
		IDR_MERGEDOCTYPE,
		RUNTIME_CLASS(CHexMergeDoc),
		RUNTIME_CLASS(CHexMergeFrame), // custom MDI child frame
		RUNTIME_CLASS(CHexMergeView));
	AddDocTemplate(m_pHexMergeTemplate);

	// Directory view
	m_pDirTemplate = new CMultiDocTemplate(
		IDR_DIRDOCTYPE,
		RUNTIME_CLASS(CDirDoc),
		RUNTIME_CLASS(CDirFrame), // custom MDI child frame
		RUNTIME_CLASS(CDirView));
	AddDocTemplate(m_pDirTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// Init menus -- hMenuDefault is for MainFrame
	pMainFrame->m_hMenuDefault = pMainFrame->NewDefaultMenu();

	// Set the menu
	// Note : for Windows98 compatibility, use FromHandle and not Attach/Detach
	CMenu * pNewMenu = CMenu::FromHandle(pMainFrame->m_hMenuDefault);
	pMainFrame->MDISetMenu(pNewMenu, NULL);

	// The main window has been initialized, so activate and update it.
	pMainFrame->ActivateFrame(cmdInfo.m_nCmdShow);
	pMainFrame->UpdateWindow();

	// Since this function actually opens paths for compare it must be
	// called after initializing CMainFrame!
	BOOL bContinue = TRUE;
	if (ParseArgsAndDoOpen(cmdInfo, pMainFrame) == FALSE && bCommandLineInvoke)
		bContinue = FALSE;

	if (hMutex)
		ReleaseMutex(hMutex);

	// If user wants to cancel the compare, close WinMerge
	if (bContinue == FALSE)
	{
		pMainFrame->PostMessage(WM_CLOSE, 0, 0);
	}

#ifdef TEST_WINMERGE
	WinMergeTest::TestAll();
#endif

	return bContinue;
}

static void OpenContributersFile(int&)
{
	theApp.OpenFileToExternalEditor(paths::ConcatPath(env::GetProgPath(), ContributorsPath));
}

// App command to run the dialog
void CMergeApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.m_onclick_contributers += Poco::delegate(OpenContributersFile);
	aboutDlg.DoModal();
	aboutDlg.m_onclick_contributers.clear();
}

/////////////////////////////////////////////////////////////////////////////
// CMergeApp commands

/**
 * @brief Called when application is about to exit.
 * This functions is called when application is exiting, so this is
 * good place to do cleanups.
 * @return Application's exit value (returned from WinMain()).
 */
int CMergeApp::ExitInstance() 
{
	charsets_cleanup();

	//  Save registry keys if existing WinMerge.reg
	env::SaveRegistryToFile(paths::ConcatPath(env::GetProgPath(), _T("WinMerge.reg")), RegDir);

	// Remove tempfolder
	const String temp = env::GetTemporaryPath();
	ClearTempfolder(temp);

	// Cleanup left over tempfiles from previous instances.
	// Normally this should not neet to do anything - but if for some reason
	// WinMerge did not delete temp files this makes sure they are removed.
	CleanupWMtemp();

	delete m_mainThreadScripts;
	CWinApp::ExitInstance();
	return 0;
}

int CMergeApp::DoMessageBox( LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt )
{
	// This is a convenient point for breakpointing !!!

	// Create a handle to store the parent window of the message box.
	CWnd* pParentWnd = CWnd::GetActiveWindow();
	
	// Check whether an active window was retrieved successfully.
	if ( pParentWnd == NULL )
	{
		// Try to retrieve a handle to the last active popup.
		CWnd * mainwnd = GetMainWnd();
		if (mainwnd)
			pParentWnd = mainwnd->GetLastActivePopup();
	}

	// Use our own message box implementation, which adds the
	// do not show again checkbox, and implements it on subsequent calls
	// (if caller set the style)

	if (m_bNonInteractive)
		return IDCANCEL;

	// Create the message box dialog.
	CMessageBoxDialog dlgMessage(pParentWnd, lpszPrompt, _T(""), nType | MB_RIGHT_ALIGN,
		nIDPrompt);
	
	if (m_pMainWnd->IsIconic())
		m_pMainWnd->ShowWindow(SW_RESTORE);

	// Display the message box dialog and return the result.
	return static_cast<int>(dlgMessage.DoModal());
}

/** 
 * @brief Set flag so that application will broadcast notification at next
 * idle time (via WM_TIMER id=IDLE_TIMER)
 */
void CMergeApp::SetNeedIdleTimer()
{
	m_bNeedIdleTimer = TRUE; 
}

bool CMergeApp::IsReallyIdle() const
{
	bool idle = true;
	POSITION pos = m_pDirTemplate->GetFirstDocPosition();
	while (pos)
	{
		CDirDoc *pDirDoc = static_cast<CDirDoc *>(m_pDirTemplate->GetNextDoc(pos));
		if (const CompareStats *pCompareStats = pDirDoc->GetCompareStats())
		{
			if (!pCompareStats->IsCompareDone() || pDirDoc->GetGeneratingReport())
				idle = false;
		}
	}
    return idle;
}

BOOL CMergeApp::OnIdle(LONG lCount) 
{
	if (CWinApp::OnIdle(lCount))
		return TRUE;

	// If anyone has requested notification when next idle occurs, send it
	if (m_bNeedIdleTimer)
	{
		m_bNeedIdleTimer = FALSE;
		m_pMainWnd->SendMessageToDescendants(WM_TIMER, IDLE_TIMER, lCount, TRUE, FALSE);
	}

	if (m_bNonInteractive && IsReallyIdle())
		m_pMainWnd->PostMessage(WM_CLOSE, 0, 0);

	return FALSE;
}

/**
 * @brief Load any known file filters.
 *
 * This function loads filter files from paths we know contain them.
 * @note User's filter location may not be set yet.
 */
void CMergeApp::InitializeFileFilters()
{
	CString filterPath = GetProfileString(_T("Settings"), _T("UserFilterPath"), _T(""));

	if (!filterPath.IsEmpty())
	{
		m_pGlobalFileFilter->SetUserFilterPath((LPCTSTR)filterPath);
	}
	m_pGlobalFileFilter->LoadAllFileFilters();
}

/** @brief Read command line arguments and open files for comparison.
 *
 * The name of the function is a legacy code from the time that this function
 * actually parsed the command line. Today the parsing is done using the
 * MergeCmdLineInfo class.
 * @param [in] cmdInfo Commandline parameters info.
 * @param [in] pMainFrame Pointer to application main frame.
 * @return TRUE if we opened the compare, FALSE if the compare was canceled.
 */
BOOL CMergeApp::ParseArgsAndDoOpen(MergeCmdLineInfo& cmdInfo, CMainFrame* pMainFrame)
{
	BOOL bCompared = FALSE;
	String strDesc[3];
	m_bNonInteractive = cmdInfo.m_bNonInteractive;

	// Set the global file filter.
	if (!cmdInfo.m_sFileFilter.empty())
	{
		m_pGlobalFileFilter->SetFilter(cmdInfo.m_sFileFilter);
	}

	// Set codepage.
	if (cmdInfo.m_nCodepage)
	{
		UpdateDefaultCodepage(2,cmdInfo.m_nCodepage);
	}

	// Unless the user has requested to see WinMerge's usage open files for
	// comparison.
	if (cmdInfo.m_bShowUsage)
	{
		ShowHelp(CommandLineHelpLocation);
	}
	else
	{
		// Set the required information we need from the command line:

		m_bClearCaseTool = cmdInfo.m_bClearCaseTool;
		m_bExitIfNoDiff = cmdInfo.m_bExitIfNoDiff;
		m_bEscShutdown = cmdInfo.m_bEscShutdown;

		m_strSaveAsPath = cmdInfo.m_sOutputpath;

		strDesc[0] = cmdInfo.m_sLeftDesc;
		if (cmdInfo.m_Files.GetSize() < 3)
		{
			strDesc[1] = cmdInfo.m_sRightDesc;
		}
		else
		{
			strDesc[1] = cmdInfo.m_sMiddleDesc;
			strDesc[2] = cmdInfo.m_sRightDesc;
		}

		if (cmdInfo.m_Files.GetSize() > 2)
		{
			cmdInfo.m_dwLeftFlags |= FFILEOPEN_CMDLINE;
			cmdInfo.m_dwMiddleFlags |= FFILEOPEN_CMDLINE;
			cmdInfo.m_dwRightFlags |= FFILEOPEN_CMDLINE;
			DWORD dwFlags[3] = {cmdInfo.m_dwLeftFlags, cmdInfo.m_dwMiddleFlags, cmdInfo.m_dwRightFlags};
			bCompared = pMainFrame->DoFileOpen(&cmdInfo.m_Files,
				dwFlags, strDesc, cmdInfo.m_sReportFile, cmdInfo.m_bRecurse, NULL,
				cmdInfo.m_sPreDiffer);
		}
		else if (cmdInfo.m_Files.GetSize() > 1)
		{
			DWORD dwFlags[3] = {cmdInfo.m_dwLeftFlags, cmdInfo.m_dwRightFlags, FFILEOPEN_NONE};
			bCompared = pMainFrame->DoFileOpen(&cmdInfo.m_Files,
				dwFlags, strDesc, cmdInfo.m_sReportFile, cmdInfo.m_bRecurse, NULL,
				cmdInfo.m_sPreDiffer);
		}
		else if (cmdInfo.m_Files.GetSize() == 1)
		{
			String sFilepath = cmdInfo.m_Files[0];
			if (IsProjectFile(sFilepath))
			{
				bCompared = LoadAndOpenProjectFile(sFilepath);
			}
			else if (IsConflictFile(sFilepath))
			{
				//For a conflict file, load the descriptions in their respective positions:  (they will be reordered as needed)
				strDesc[0] = cmdInfo.m_sLeftDesc;
				strDesc[1] = cmdInfo.m_sMiddleDesc;
				strDesc[2] = cmdInfo.m_sRightDesc;
				bCompared = pMainFrame->DoOpenConflict(sFilepath, strDesc);
			}
			else
			{
				DWORD dwFlags[3] = {cmdInfo.m_dwLeftFlags, cmdInfo.m_dwRightFlags, FFILEOPEN_NONE};
				bCompared = pMainFrame->DoFileOpen(&cmdInfo.m_Files,
					dwFlags, strDesc, cmdInfo.m_sReportFile, cmdInfo.m_bRecurse, NULL, 
					cmdInfo.m_sPreDiffer);
			}
		}
		else if (cmdInfo.m_Files.GetSize() == 0) // if there are no input args, we can check the display file dialog flag
		{
			BOOL showFiles = m_pOptions->GetBool(OPT_SHOW_SELECT_FILES_AT_STARTUP);
			if (showFiles)
				pMainFrame->DoFileOpen();
		}
	}
	return bCompared;
}

void CMergeApp::UpdateDefaultCodepage(int cpDefaultMode, int cpCustomCodepage)
{
	int wLangId;

	switch (cpDefaultMode)
	{
		case 0:
			ucr::setDefaultCodepage(GetACP());
			break;
		case 1:
			TCHAR buff[32];
			wLangId = GetLangId();
			if (GetLocaleInfo(wLangId, LOCALE_IDEFAULTANSICODEPAGE, buff, sizeof(buff)/sizeof(buff[0])))
				ucr::setDefaultCodepage(_ttol(buff));
			else
				ucr::setDefaultCodepage(GetACP());
			break;
		case 2:
			ucr::setDefaultCodepage(cpCustomCodepage);
			break;
		default:
			// no other valid option
			assert (0);
			ucr::setDefaultCodepage(GetACP());
	}
}

/**
 * @brief Send current option settings into codepage module
 */
void CMergeApp::UpdateCodepageModule()
{
	// Get current codepage settings from the options module
	// and push them into the codepage module
	UpdateDefaultCodepage(GetOptionsMgr()->GetInt(OPT_CP_DEFAULT_MODE), GetOptionsMgr()->GetInt(OPT_CP_DEFAULT_CUSTOM));
}

/** @brief Open help from mainframe when user presses F1*/
void CMergeApp::OnHelp()
{
	ShowHelp();
}

/**
 * @brief Open given file to external editor specified in options.
 * @param [in] file Full path to file to open.
 *
 * Opens file to defined (in Options/system), Notepad by default,
 * external editor. Path is decorated with quotation marks if needed
 * (contains spaces). Also '$file' in editor path is replaced by
 * filename to open.
 * @param [in] file Full path to file to open.
 * @param [in] nLineNumber Line number to go to.
 */
void CMergeApp::OpenFileToExternalEditor(const String& file, int nLineNumber/* = 1*/)
{
	String sCmd = GetOptionsMgr()->GetString(OPT_EXT_EDITOR_CMD);
	String sFile(file);
	strutils::replace(sCmd, _T("$linenum"), strutils::to_str(nLineNumber));

	size_t nIndex = sCmd.find(_T("$file"));
	if (nIndex != String::npos)
	{
		sFile.insert(0, _T("\""));
		strutils::replace(sCmd, _T("$file"), sFile);
		nIndex = sCmd.find(' ', nIndex + sFile.length());
		if (nIndex != String::npos)
			sCmd.insert(nIndex, _T("\""));
		else
			sCmd += '"';
	}
	else
	{
		sCmd += _T(" \"");
		sCmd += sFile;
		sCmd += _T("\"");
	}

	BOOL retVal = FALSE;
	STARTUPINFO stInfo = { sizeof STARTUPINFO };
	PROCESS_INFORMATION processInfo;

	retVal = CreateProcess(NULL, (LPTSTR)sCmd.c_str(),
		NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
		&stInfo, &processInfo);

	if (!retVal)
	{
		// Error invoking external editor
		String msg = strutils::format_string1(_("Failed to execute external editor: %1"), sCmd);
		AfxMessageBox(msg.c_str(), MB_ICONSTOP);
	}
	else
	{
		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);
	}
}

/**
 * @brief Open file, if it exists, else open url
 */
void CMergeApp::OpenFileOrUrl(LPCTSTR szFile, LPCTSTR szUrl)
{
	if (paths::DoesPathExist(szFile) == paths::IS_EXISTING_FILE)
		ShellExecute(NULL, _T("open"), _T("notepad.exe"), szFile, NULL, SW_SHOWNORMAL);
	else
		ShellExecute(NULL, _T("open"), szUrl, NULL, NULL, SW_SHOWNORMAL);
}

/**
 * @brief Show Help - this is for opening help from outside mainframe.
 * @param [in] helpLocation Location inside help, if NULL main help is opened.
 */
void CMergeApp::ShowHelp(LPCTSTR helpLocation /*= NULL*/)
{
	String sPath = env::GetProgPath();
	LANGID LangId = GetLangId();
	sPath = paths::ConcatPath(sPath, DocsPath);
	if (helpLocation == NULL)
	{
		if (paths::DoesPathExist(sPath) == paths::IS_EXISTING_FILE)
			::HtmlHelp(NULL, sPath.c_str(), HH_DISPLAY_TOC, NULL);
		else
			ShellExecute(NULL, _T("open"), DocsURL, NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		if (paths::DoesPathExist(sPath) == paths::IS_EXISTING_FILE)
		{
			sPath += helpLocation;
			::HtmlHelp(NULL, sPath.c_str(), HH_DISPLAY_TOPIC, NULL);
		}
	}
}

/**
 * @brief Creates backup before file is saved or copied over.
 * This function handles formatting correct path and filename for
 * backup file. Formatting is done based on several options available
 * for users in Options/Backups dialog. After path is formatted, file
 * is simply just copied. Not much error checking, just if copying
 * succeeded or failed.
 * @param [in] bFolder Are we creating backup in folder compare?
 * @param [in] pszPath Full path to file to backup.
 * @return TRUE if backup succeeds, or isn't just done.
 */
BOOL CMergeApp::CreateBackup(BOOL bFolder, const String& pszPath)
{
	// If user doesn't want to backups in folder compare, return
	// success so operations don't abort.
	if (bFolder && !(GetOptionsMgr()->GetBool(OPT_BACKUP_FOLDERCMP)))
		return TRUE;
	// Likewise if user doesn't want backups in file compare
	else if (!bFolder && !(GetOptionsMgr()->GetBool(OPT_BACKUP_FILECMP)))
		return TRUE;

	// create backup copy of file if destination file exists
	if (paths::DoesPathExist(pszPath) == paths::IS_EXISTING_FILE)
	{
		String bakPath;
		String path;
		String filename;
		String ext;
	
		paths::SplitFilename(paths::GetLongPath(pszPath), &path, &filename, &ext);

		// Determine backup folder
		if (GetOptionsMgr()->GetInt(OPT_BACKUP_LOCATION) ==
			PropBackups::FOLDER_ORIGINAL)
		{
			// Put backups to same folder than original file
			bakPath = path;
		}
		else if (GetOptionsMgr()->GetInt(OPT_BACKUP_LOCATION) ==
			PropBackups::FOLDER_GLOBAL)
		{
			// Put backups to global folder defined in options
			bakPath = GetOptionsMgr()->GetString(OPT_BACKUP_GLOBALFOLDER);
			if (bakPath.empty())
				bakPath = path;
			else
				bakPath = paths::GetLongPath(bakPath);
		}
		else
		{
			_RPTF0(_CRT_ERROR, "Unknown backup location!");
		}

		BOOL success = FALSE;
		if (GetOptionsMgr()->GetBool(OPT_BACKUP_ADD_BAK))
		{
			// Don't add dot if there is no existing extension
			if (ext.size() > 0)
				ext += _T(".");
			ext += BACKUP_FILE_EXT;
		}

		// Append time to filename if wanted so
		// NOTE just adds timestamp at the moment as I couldn't figure out
		// nice way to add a real time (invalid chars etc).
		if (GetOptionsMgr()->GetBool(OPT_BACKUP_ADD_TIME))
		{
			struct tm tm;
			time_t curtime = 0;
			time(&curtime);
			::localtime_s(&tm, &curtime);
			CString timestr;
			timestr.Format(_T("%04d%02d%02d%02d%02d%02d"), tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			filename += _T("-");
			filename += timestr;
		}

		// Append filename and extension (+ optional .bak) to path
		if ((bakPath.length() + filename.length() + ext.length())
			< MAX_PATH)
		{
			success = TRUE;
			bakPath = paths::ConcatPath(bakPath, filename);
			bakPath += _T(".");
			bakPath += ext;
		}

		if (success)
			success = CopyFile(pszPath.c_str(), bakPath.c_str(), FALSE);
		
		if (!success)
		{
			String msg = strutils::format_string1(
				_("Unable to backup original file:\n%1\n\nContinue anyway?"),
				pszPath);
			if (AfxMessageBox(msg.c_str(), MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN) != IDYES)
				return FALSE;
		}
		return TRUE;
	}

	// we got here because we're either not backing up of there was nothing to backup
	return TRUE;
}

/**
 * @brief Sync file to Version Control System
 * @param pszDest [in] Where to copy (incl. filename)
 * @param bApplyToAll [in,out] Apply user selection to all items
 * @param psError [out] Error string that can be shown to user in caller func
 * @return User selection or -1 if error happened
 * @sa CMainFrame::HandleReadonlySave()
 * @sa CDirView::PerformActionList()
 */
int CMergeApp::SyncFileToVCS(const String& pszDest, BOOL &bApplyToAll,
	String& sError)
{
	String sActionError;
	String strSavePath(pszDest);
	int nVerSys = 0;

	nVerSys = GetOptionsMgr()->GetInt(OPT_VCS_SYSTEM);
	
	int nRetVal = HandleReadonlySave(strSavePath, TRUE, bApplyToAll);
	if (nRetVal == IDCANCEL || nRetVal == IDNO)
		return nRetVal;
	
	// If VC project opened from VSS sync and version control used
	if ((nVerSys == SourceControl::VCS_VSS4 || nVerSys == SourceControl::VCS_VSS5) && m_pSourceControl->m_bVCProjSync)
	{
		if (!m_pSourceControl->m_vssHelper.ReLinkVCProj(strSavePath, sError))
			nRetVal = -1;
	}
	return nRetVal;
}

/**
 * @brief Checks if path (file/folder) is read-only and asks overwriting it.
 *
 * @param strSavePath [in,out] Path where to save (file or folder)
 * @param bMultiFile [in] Single file or multiple files/folder
 * @param bApplyToAll [in,out] Apply last user selection for all items?
 * @return Users selection:
 * - IDOK: Item was not readonly, no actions
 * - IDYES/IDYESTOALL: Overwrite readonly item
 * - IDNO: User selected new filename (single file) or user wants to skip
 * - IDCANCEL: Cancel operation
 * @sa CMainFrame::SyncFileToVCS()
 * @sa CMergeDoc::DoSave()
 */
int CMergeApp::HandleReadonlySave(String& strSavePath, BOOL bMultiFile,
		BOOL &bApplyToAll)
{
	CFileStatus status;
	int nRetVal = IDOK;
	BOOL bFileRO = FALSE;
	BOOL bFileExists = FALSE;
	String s;
	String str;
	CString title;
	int nVerSys = 0;

	if (!strSavePath.empty())
	{
		try
		{
			TFile file(strSavePath);
			bFileExists = file.exists();
			if (bFileExists)
				bFileRO = !file.canWrite();
		}
		catch (...)
		{
		}
	}

	nVerSys = GetOptionsMgr()->GetInt(OPT_VCS_SYSTEM);
	
	if (bFileExists && bFileRO)
	{
		UINT userChoice = 0;
		// Version control system used?
		// Checkout file from VCS and modify, don't ask about overwriting
		// RO files etc.
		if (nVerSys != SourceControl::VCS_NONE)
		{
			bool bRetVal = m_pSourceControl->SaveToVersionControl(strSavePath);
			if (bRetVal)
				return IDYES;
			else
				return IDCANCEL;
		}
		
		// Don't ask again if its already asked
		if (bApplyToAll)
			userChoice = IDYES;
		else
		{
			// Prompt for user choice
			if (bMultiFile)
			{
				// Multiple files or folder
				str = strutils::format_string1(_("%1\nis marked read-only. Would you like to override the read-only item?"), strSavePath);
				userChoice = AfxMessageBox(str.c_str(), MB_YESNOCANCEL |
						MB_ICONWARNING | MB_DEFBUTTON3 | MB_DONT_ASK_AGAIN |
						MB_YES_TO_ALL, IDS_SAVEREADONLY_MULTI);
			}
			else
			{
				// Single file
				str = strutils::format_string1(_("%1 is marked read-only. Would you like to override the read-only file ? (No to save as new filename.)"), strSavePath);
				userChoice = AfxMessageBox(str.c_str(), MB_YESNOCANCEL |
						MB_ICONWARNING | MB_DEFBUTTON2 | MB_DONT_ASK_AGAIN,
						IDS_SAVEREADONLY_FMT);
			}
		}
		switch (userChoice)
		{
		// Overwrite read-only file
		case IDYESTOALL:
			bApplyToAll = TRUE;  // Don't ask again (no break here)
		case IDYES:
			CFile::GetStatus(strSavePath.c_str(), status);
			status.m_mtime = 0;		// Avoid unwanted changes
			status.m_attribute &= ~CFile::readOnly;
			CFile::SetStatus(strSavePath.c_str(), status);
			nRetVal = IDYES;
			break;
		
		// Save to new filename (single) /skip this item (multiple)
		case IDNO:
			if (!bMultiFile)
			{
				if (SelectFile(AfxGetMainWnd()->GetSafeHwnd(), s, strSavePath.c_str(), _("Save As"), _T(""), FALSE))
				{
					strSavePath = s;
					nRetVal = IDNO;
				}
				else
					nRetVal = IDCANCEL;
			}
			else
				nRetVal = IDNO;
			break;

		// Cancel saving
		case IDCANCEL:
			nRetVal = IDCANCEL;
			break;
		}
	}
	return nRetVal;
}

/**
 * @brief Shows VSS error from exception and writes log.
 */
void CMergeApp::ShowVSSError(CException *e, const String& strItem)
{
	TCHAR errStr[1024] = {0};
	if (e->GetErrorMessage(errStr, 1024))
	{
		String errMsg = theApp.LoadString(IDS_VSS_ERRORFROM);
		String logMsg = errMsg;
		errMsg += _T("\n");
		errMsg += errStr;
		logMsg += _T(" ");
		logMsg += errStr;
		if (!strItem.empty())
		{
			errMsg += _T("\n\n");
			errMsg += strItem;
			logMsg += _T(": ");
			logMsg += strItem;
		}
		LogErrorString(logMsg);
		AfxMessageBox(errMsg.c_str(), MB_ICONSTOP);
	}
	else
	{
		LogErrorString(_T("VSSError (unable to GetErrorMessage)"));
		e->ReportError(MB_ICONSTOP, IDS_VSS_RUN_ERROR);
	}
}

/**
 * @brief Is specified file a project file?
 * @param [in] filepath Full path to file to check.
 * @return true if file is a projectfile.
 */
bool CMergeApp::IsProjectFile(const String& filepath) const
{
	String ext;
	paths::SplitFilename(filepath, NULL, NULL, &ext);
	if (strutils::compare_nocase(ext, ProjectFile::PROJECTFILE_EXT) == 0)
		return true;
	else
		return false;
}

bool CMergeApp::LoadProjectFile(const String& sProject, ProjectFile &project)
{
	if (sProject.empty())
		return false;

	try
	{
        project.Read(sProject);
	}
	catch (Poco::Exception& e)
	{
		String sErr = _("Unknown error attempting to open project file");
		sErr += ucr::toTString(e.displayText());
		String msg = strutils::format_string2(_("Cannot open file\n%1\n\n%2"), sProject, sErr);
		AfxMessageBox(msg.c_str(), MB_ICONSTOP);
		return false;
	}

	return true;
}

bool CMergeApp::SaveProjectFile(const String& sProject, const ProjectFile &project)
{
	try
	{
		project.Save(sProject);
	}
	catch (Poco::Exception& e)
	{
		String sErr = _("Unknown error attempting to save project file");
		sErr += ucr::toTString(e.displayText());
		String msg = strutils::format_string2(_("Cannot open file\n%1\n\n%2"), sProject, sErr);
		AfxMessageBox(msg.c_str(), MB_ICONSTOP);
		return false;
	}

	return true;
}

/** 
 * @brief Read project and perform comparison specified
 * @param [in] sProject Full path to project file.
 * @return TRUE if loading project file and starting compare succeeded.
 */
bool CMergeApp::LoadAndOpenProjectFile(const String& sProject, const String& sReportFile)
{
	ProjectFile project;
	if (!LoadProjectFile(sProject, project))
		return false;
	
	PathContext files;
	BOOL bLeftReadOnly = FALSE;
	BOOL bMiddleReadOnly = FALSE;
	BOOL bRightReadOnly = FALSE;
	bool bRecursive = FALSE;
	project.GetPaths(files, bRecursive);
	bLeftReadOnly = project.GetLeftReadOnly();
	bMiddleReadOnly = project.GetMiddleReadOnly();
	bRightReadOnly = project.GetRightReadOnly();
	if (project.HasFilter())
	{
		String filter = project.GetFilter();
		filter = strutils::trim_ws(filter);
		m_pGlobalFileFilter->SetFilter(filter);
	}
	if (project.HasSubfolders())
		bRecursive = project.GetSubfolders() > 0;

	DWORD dwFlags[3] = {
		static_cast<DWORD>(files.GetPath(0).empty() ? FFILEOPEN_NONE : FFILEOPEN_PROJECT),
		static_cast<DWORD>(files.GetPath(1).empty() ? FFILEOPEN_NONE : FFILEOPEN_PROJECT),
		static_cast<DWORD>(files.GetPath(2).empty() ? FFILEOPEN_NONE : FFILEOPEN_PROJECT)
	};
	if (bLeftReadOnly)
		dwFlags[0] |= FFILEOPEN_READONLY;
	if (files.GetSize() == 2)
	{
		if (bRightReadOnly)
			dwFlags[1] |= FFILEOPEN_READONLY;
	}
	else
	{
		if (bMiddleReadOnly)
			dwFlags[1] |= FFILEOPEN_READONLY;
		if (bRightReadOnly)
			dwFlags[2] |= FFILEOPEN_READONLY;
	}

	GetOptionsMgr()->SaveOption(OPT_CMP_INCLUDE_SUBDIRS, bRecursive);
	
	BOOL rtn = GetMainFrame()->DoFileOpen(&files, dwFlags, NULL, sReportFile, bRecursive);

	AddToRecentProjectsMRU(sProject.c_str());
	return !!rtn;
}

/**
 * @brief Return windows language ID of current WinMerge GUI language
 */
WORD CMergeApp::GetLangId() const
{
	return m_pLangDlg->GetLangId();
}

/**
 * @brief Lang aware version of CStatusBar::SetIndicators()
 */
void CMergeApp::SetIndicators(CStatusBar &sb, const UINT *rgid, int n) const
{
	m_pLangDlg->SetIndicators(sb, rgid, n);
}

/**
 * @brief Translate menu to current WinMerge GUI language
 */
void CMergeApp::TranslateMenu(HMENU h) const
{
	m_pLangDlg->TranslateMenu(h);
}

/**
 * @brief Translate dialog to current WinMerge GUI language
 */
void CMergeApp::TranslateDialog(HWND h) const
{
	CWnd *pWnd = CWnd::FromHandle(h);
	pWnd->SetFont(const_cast<CFont *>(&m_fontGUI));
	pWnd->SendMessageToDescendants(WM_SETFONT, (WPARAM)m_fontGUI.m_hObject, MAKELPARAM(FALSE, 0), TRUE);

	m_pLangDlg->TranslateDialog(h);
}

/**
 * @brief Load string and translate to current WinMerge GUI language
 */
String CMergeApp::LoadString(UINT id) const
{
	return m_pLangDlg->LoadString(id);
}

bool CMergeApp::TranslateString(const std::string& str, String& translated_str) const
{
	return m_pLangDlg->TranslateString(str, translated_str);
}

/**
 * @brief Load dialog caption and translate to current WinMerge GUI language
 */
std::wstring CMergeApp::LoadDialogCaption(LPCTSTR lpDialogTemplateID) const
{
	return m_pLangDlg->LoadDialogCaption(lpDialogTemplateID);
}

/**
 * @brief Adds specified file to the recent projects list.
 * @param [in] sPathName Path to project file
 */
void CMergeApp::AddToRecentProjectsMRU(LPCTSTR sPathName)
{
	// sPathName will be added to the top of the MRU list. 
	// If sPathName already exists in the MRU list, it will be moved to the top
	if (m_pRecentFileList != NULL)    {
		m_pRecentFileList->Add(sPathName);
		m_pRecentFileList->WriteList();
	}
}

void CMergeApp::SetupTempPath()
{
	String instTemp = env::GetPerInstanceString(TempFolderPrefix);
	if (GetOptionsMgr()->GetBool(OPT_USE_SYSTEM_TEMP_PATH))
		env::SetTemporaryPath(paths::ConcatPath(env::GetSystemTempPath(), instTemp));
	else
		env::SetTemporaryPath(paths::ConcatPath(GetOptionsMgr()->GetString(OPT_CUSTOM_TEMP_PATH), instTemp));
}

/**
 * @brief Handles menu selection from recent projects list
 * @param [in] nID Menu ID of the selected item
 */
BOOL CMergeApp::OnOpenRecentFile(UINT nID)
{
	return LoadAndOpenProjectFile(static_cast<const TCHAR *>(m_pRecentFileList->m_arrNames[nID-ID_FILE_PROJECT_MRU_FIRST]));
}

/**
 * @brief Return if doc is in Merging/Editing mode
 */
bool CMergeApp::GetMergingMode() const
{
	return m_bMergingMode;
}

/**
 * @brief Set doc to Merging/Editing mode
 */
void CMergeApp::SetMergingMode(bool bMergingMode)
{
	m_bMergingMode = bMergingMode;
	GetOptionsMgr()->SaveOption(OPT_MERGE_MODE, m_bMergingMode);
}

/**
 * @brief Switch Merging/Editing mode and update
 * buffer read-only states accordingly
 */
void CMergeApp::OnMergingMode()
{
	bool bMergingMode = GetMergingMode();

	if (!bMergingMode)
		LangMessageBox(IDS_MERGE_MODE, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN);
	SetMergingMode(!bMergingMode);
}

/**
 * @brief Update Menuitem for Merging Mode
 */
void CMergeApp::OnUpdateMergingMode(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
	pCmdUI->SetCheck(GetMergingMode());
}

/**
 * @brief Update MergingMode UI in statusbar
 */
void CMergeApp::OnUpdateMergingStatus(CCmdUI *pCmdUI)
{
	String text = theApp.LoadString(IDS_MERGEMODE_MERGING);
	pCmdUI->SetText(text.c_str());
	pCmdUI->Enable(GetMergingMode());
}

