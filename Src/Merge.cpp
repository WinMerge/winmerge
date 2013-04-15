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
// ID line follows -- this is updated by SVN
// $Id: Merge.cpp 6861 2009-06-25 12:11:07Z kimmov $

#include "stdafx.h"
#include "Constants.h"
#include "UnicodeString.h"
#include "unicoder.h"
#include "Environment.h"
#include "OptionsMgr.h"
#include "Merge.h"
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
#include "Splash.h"
#include "logfile.h"
#include "paths.h"
#include "FileFilterHelper.h"
#include "Plugins.h"
#include "ProjectFile.h"
#include "MergeEditView.h"
#include "LanguageSelect.h"
#include "OptionsDef.h"
#include "MergeCmdLineInfo.h"
#include "ConflictFileParser.h"
#include "codepage.h"
#include "JumpList.h"

// For shutdown cleanup
#include "charsets.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/** @brief Location for command line help to open. */
static TCHAR CommandLineHelpLocation[] = _T("::/htmlhelp/Command_line.html");

// registry dir to WinMerge
static String f_RegDir = _T("Software\\Thingamahoochie\\WinMerge");

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
			_sntprintf(lpszError, nMaxError, _T("%hs"), m_pSTLException->what());
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
	ON_COMMAND(ID_VIEW_LANGUAGE, OnViewLanguage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LANGUAGE, OnUpdateViewLanguage)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

static void AddEnglishResourceHook();

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
, m_pDiffTemplate(0)
, m_pHexMergeTemplate(0)
, m_pDirTemplate(0)
, m_mainThreadScripts(NULL)
, m_nLastCompareResult(0)
, m_bNonInteractive(false)
, m_pOptions(new CRegOptionsMgr())
, m_pLog(new CLogFile())
, m_nActiveOperations(0)
, m_pLangDlg(new CLanguageSelect(IDR_MAINFRAME, IDR_MAINFRAME))
{
	// add construction code here,
	// Place all significant initialization in InitInstance
}

CMergeApp::~CMergeApp()
{
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

	// Only needed by VC6
#if _MSC_VER < 1300
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	// Load registry keys from WinMerge.reg if existing WinMerge.reg
	env_LoadRegistryFromFile(env_GetProgPath() + _T("\\WinMerge.reg"));

	OptionsInit(); // Implementation in OptionsInit.cpp

	// Initialize temp folder
	String instTemp = env_GetPerInstanceString(TempFolderPrefix);
	if (GetOptionsMgr()->GetBool(OPT_USE_SYSTEM_TEMP_PATH))
		env_SetTempPath(paths_ConcatPath(env_GetSystemTempPath(), instTemp));
	else
		env_SetTempPath(paths_ConcatPath(GetOptionsMgr()->GetString(OPT_CUSTOM_TEMP_PATH), instTemp));

	// Cleanup left over tempfiles from previous instances.
	// Normally this should not neet to do anything - but if for some reason
	// WinMerge did not delete temp files this makes sure they are removed.
	CleanupWMtemp();

	int logging = GetOptionsMgr()->GetInt(OPT_LOGGING);
	if (logging > 0)
	{
		m_pLog->EnableLogging(TRUE);
		String logfile = env_GetMyDocuments();
		logfile = paths_ConcatPath(logfile, _T("WinMerge\\WinMerge.log"));
		m_pLog->SetFile(logfile);

		if (logging == 1)
			m_pLog->SetMaskLevel(CLogFile::LALL);
		else if (logging == 2)
			m_pLog->SetMaskLevel(CLogFile::LERROR | CLogFile::LWARNING);
	}

	// Parse command-line arguments.
	MergeCmdLineInfo cmdInfo = GetCommandLine();

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

	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)
	BOOL bDisableSplash	= GetOptionsMgr()->GetBool(OPT_DISABLE_SPLASH);

	InitializeFileFilters();

	// Read last used filter from registry
	// If filter fails to set, reset to default
	const String filterString = m_pOptions->GetString(OPT_FILEFILTER_CURRENT);
	BOOL bFilterSet = theApp.m_globalFileFilter.SetFilter(filterString.c_str());
	if (!bFilterSet)
	{
		String filter = theApp.m_globalFileFilter.GetFilterNameOrMask();
		m_pOptions->SaveOption(OPT_FILEFILTER_CURRENT, filter);
	}

	CSplashWnd::EnableSplashScreen(!bDisableSplash && !bCommandLineInvoke);

	// Initialize i18n (multiple language) support

	m_pLangDlg->SetLogFile(GetLog());
	m_pLangDlg->InitializeLanguage();

	AddEnglishResourceHook(); // Use English string when l10n (foreign) string missing

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
	// Enable drag&drop files
	pMainFrame->ModifyStyleEx(NULL, WS_EX_ACCEPTFILES);

	// Init menus -- hMenuDefault is for MainFrame, other
	// two are for dirdoc and mergedoc (commented out for now)
	m_pDiffTemplate->m_hMenuShared = pMainFrame->NewMergeViewMenu();
	m_pHexMergeTemplate->m_hMenuShared = pMainFrame->NewHexMergeViewMenu();
	m_pDirTemplate->m_hMenuShared = pMainFrame->NewDirViewMenu();
	m_pOpenTemplate->m_hMenuShared = pMainFrame->NewOpenViewMenu();
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

	if (m_bNonInteractive)
	{
		bContinue = FALSE;
	}

	// If user wants to cancel the compare, close WinMerge
	if (bContinue == FALSE)
	{
		pMainFrame->PostMessage(WM_CLOSE, 0, 0);
	}

	return bContinue;
}

// App command to run the dialog
void CMergeApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMergeApp commands


BOOL CMergeApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}

void CMergeApp::OnViewLanguage() 
{
	if (m_pLangDlg->DoModal()==IDOK)
	{
		//m_lang.ReloadMenu();
		//m_LangDlg.UpdateDocTitle();
		GetMainFrame()->UpdateResources();
	}
}

/**
 * @brief Updates Language select menu item.
 * If there are no languages installed we disable menuitem to
 * open language selection dialog.
 */
void CMergeApp::OnUpdateViewLanguage(CCmdUI* pCmdUI)
{
	BOOL bLangsInstalled = m_pLangDlg->AreLangsInstalled();
	pCmdUI->Enable(bLangsInstalled);
}

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
	env_SaveRegistryToFile(env_GetProgPath() + _T("\\WinMerge.reg"), f_RegDir);

	// Remove tempfolder
	const String temp = env_GetTempPath();
	ClearTempfolder(temp);
	delete m_mainThreadScripts;
	CWinApp::ExitInstance();
	return 0;
}

static void AddEnglishResourceHook()
{
	// After calling AfxSetResourceHandle to point to a language
	// resource DLL, then the application is no longer on the
	// resource lookup (defined by AfxFindResourceHandle).
	
	// Add a dummy extension DLL record whose resource handle
	// points to the application resources, just to provide
	// fallback to English for any resources missing from 
	// the language resource DLL.
	
	// (Why didn't Microsoft think of this? Bruno Haible who
	// made gettext certainly thought of this.)

	// NB: This does not fix the problem that if a control is
	// missing from a dialog (because it was added only to the
	// English version, for example) then the DDX_ function is
	// going to fail. I see no easy way to intercept all DDX
	// functions except by macro overriding the call--Perry, 2002-12-07.

	static AFX_EXTENSION_MODULE FakeEnglishDLL = { NULL, NULL };
	memset(&FakeEnglishDLL, 0, sizeof(FakeEnglishDLL));
	FakeEnglishDLL.hModule = AfxGetApp()->m_hInstance;
	FakeEnglishDLL.hResource = FakeEnglishDLL.hModule;
	FakeEnglishDLL.bInitialized = TRUE;
	new CDynLinkLibrary(FakeEnglishDLL); // hook into MFC extension DLL chain
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
	CMessageBoxDialog dlgMessage(pParentWnd, lpszPrompt, _T(""), nType,
		nIDPrompt);
	
	// Display the message box dialog and return the result.
	return (int) dlgMessage.DoModal();
}

/** 
 * @brief Set flag so that application will broadcast notification at next
 * idle time (via WM_TIMER id=IDLE_TIMER)
 */
void CMergeApp::SetNeedIdleTimer()
{
	m_bNeedIdleTimer = TRUE; 
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
		m_globalFileFilter.SetUserFilterPath((LPCTSTR)filterPath);
	}
	m_globalFileFilter.LoadAllFileFilters();
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
	m_bNonInteractive = cmdInfo.m_bNonInteractive;

	// Set the global file filter.
	if (!cmdInfo.m_sFileFilter.empty())
	{
		m_globalFileFilter.SetFilter(cmdInfo.m_sFileFilter.c_str());
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
		pMainFrame->ShowHelp(CommandLineHelpLocation);
	}
	else
	{
		// Set the required information we need from the command line:

		pMainFrame->m_bClearCaseTool = cmdInfo.m_bClearCaseTool;
		pMainFrame->m_bExitIfNoDiff = cmdInfo.m_bExitIfNoDiff;
		pMainFrame->m_bEscShutdown = cmdInfo.m_bEscShutdown;

		pMainFrame->m_strSaveAsPath = cmdInfo.m_sOutputpath.c_str();

		pMainFrame->m_strDescriptions[0] = cmdInfo.m_sLeftDesc;
		if (cmdInfo.m_Files.GetSize() < 3)
		{
			pMainFrame->m_strDescriptions[1] = cmdInfo.m_sRightDesc;
		}
		else
		{
			pMainFrame->m_strDescriptions[1] = cmdInfo.m_sMiddleDesc;
			pMainFrame->m_strDescriptions[2] = cmdInfo.m_sRightDesc;
		}

		if (cmdInfo.m_Files.GetSize() > 2)
		{
			cmdInfo.m_dwLeftFlags |= FFILEOPEN_CMDLINE;
			cmdInfo.m_dwMiddleFlags |= FFILEOPEN_CMDLINE;
			cmdInfo.m_dwRightFlags |= FFILEOPEN_CMDLINE;
			DWORD dwFlags[3] = {cmdInfo.m_dwLeftFlags, cmdInfo.m_dwMiddleFlags, cmdInfo.m_dwRightFlags};
			bCompared = pMainFrame->DoFileOpen(&cmdInfo.m_Files,
				dwFlags, cmdInfo.m_bRecurse, NULL,
				cmdInfo.m_sPreDiffer.c_str());
		}
		else if (cmdInfo.m_Files.GetSize() > 1)
		{
			DWORD dwFlags[3] = {cmdInfo.m_dwLeftFlags, cmdInfo.m_dwRightFlags, FFILEOPEN_NONE};
			bCompared = pMainFrame->DoFileOpen(&cmdInfo.m_Files,
				dwFlags, cmdInfo.m_bRecurse, NULL,
				cmdInfo.m_sPreDiffer.c_str());
		}
		else if (cmdInfo.m_Files.GetSize() == 1)
		{
			String sFilepath = cmdInfo.m_Files[0];
			if (IsProjectFile(sFilepath.c_str()))
			{
				bCompared = LoadAndOpenProjectFile(sFilepath.c_str());
			}
			else if (IsConflictFile(sFilepath.c_str()))
			{
				bCompared = pMainFrame->DoOpenConflict(sFilepath.c_str());
			}
			else
			{
				DWORD dwFlags[3] = {cmdInfo.m_dwLeftFlags, cmdInfo.m_dwRightFlags, FFILEOPEN_NONE};
				bCompared = pMainFrame->DoFileOpen(&cmdInfo.m_Files,
					dwFlags, cmdInfo.m_bRecurse, NULL,
					cmdInfo.m_sPreDiffer.c_str());
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
			wLangId = theApp.GetLangId();
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


/** @brief Open help from mainframe when user presses F1*/
void CMergeApp::OnHelp()
{
	GetMainFrame()->ShowHelp();
}

/**
 * @brief Is specified file a project file?
 * @param [in] filepath Full path to file to check.
 * @return true if file is a projectfile.
 */
bool CMergeApp::IsProjectFile(const String& filepath) const
{
	String ext;
	paths_SplitFilename(filepath, NULL, NULL, &ext);
	if (string_compare_nocase(ext, ProjectFile::PROJECTFILE_EXT) == 0)
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
		String sErr = theApp.LoadString(IDS_UNK_ERROR_READING_PROJECT);
		sErr += ucr::toTString(e.displayText());
		String msg = LangFormatString2(IDS_ERROR_FILEOPEN, sProject.c_str(), sErr.c_str());
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
		String sErr = theApp.LoadString(IDS_UNK_ERROR_SAVING_PROJECT);
		sErr += ucr::toTString(e.displayText());
		String msg = LangFormatString2(IDS_ERROR_FILEOPEN, sProject.c_str(), sErr.c_str());
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
bool CMergeApp::LoadAndOpenProjectFile(const String& sProject)
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
		filter = string_trim_ws(filter);
		m_globalFileFilter.SetFilter(filter);
	}
	if (project.HasSubfolders())
		bRecursive = project.GetSubfolders() > 0;

	DWORD dwFlags[3] = {
		files.GetPath(0).empty() ? FFILEOPEN_NONE : FFILEOPEN_PROJECT,
		files.GetPath(1).empty() ? FFILEOPEN_NONE : FFILEOPEN_PROJECT,
		files.GetPath(2).empty() ? FFILEOPEN_NONE : FFILEOPEN_PROJECT
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

	WriteProfileInt(_T("Settings"), _T("Recurse"), bRecursive);
	
	BOOL rtn = GetMainFrame()->DoFileOpen(&files, dwFlags, bRecursive);

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
	m_pLangDlg->TranslateDialog(h);
}

/**
 * @brief Load string and translate to current WinMerge GUI language
 */
String CMergeApp::LoadString(UINT id) const
{
	return m_pLangDlg->LoadString(id);
}

/**
 * @brief Load dialog caption and translate to current WinMerge GUI language
 */
std::wstring CMergeApp::LoadDialogCaption(LPCTSTR lpDialogTemplateID) const
{
	return m_pLangDlg->LoadDialogCaption(lpDialogTemplateID);
}

/**
 * @brief Reload main menu(s) (for language change)
 */
void CMergeApp::ReloadMenu()
{
	m_pLangDlg->ReloadMenu();
}

/** @brief Wrap one line of cmdline help in appropriate whitespace */
static String CmdlineOption(int idres)
{
	String str = theApp.LoadString(idres) + _T(" \n");
	return str;
}

/**
 * @brief Get default editor path.
 * @return full path to the editor program executable.
 */
String CMergeApp::GetDefaultEditor()
{
	String path = env_GetWindowsDirectory();
	path += _T("\\NOTEPAD.EXE");
	return path;
}

/**
 * @brief Get default user filter folder path.
 * This function returns the default filter path for user filters.
 * If wanted so (@p bCreate) path can be created if it does not
 * exist yet. But you really want to create the patch only when
 * there is no user path defined.
 * @param [in] bCreate If TRUE filter path is created if it does
 *  not exist.
 * @return Default folder for user filters.
 */
String CMergeApp::GetDefaultFilterUserPath(BOOL bCreate /*=FALSE*/)
{
	String pathMyFolders = env_GetMyDocuments();
	String pathFilters(pathMyFolders);
	pathFilters = paths_ConcatPath(pathFilters, DefaultRelativeFilterPath);

	if (bCreate && !paths_CreateIfNeeded(pathFilters))
	{
		// Failed to create a folder, check it didn't already
		// exist.
		DWORD errCode = GetLastError();
		if (errCode != ERROR_ALREADY_EXISTS)
		{
			// Failed to create a folder for filters, fallback to
			// "My Documents"-folder. It is not worth the trouble to
			// bother user about this or user more clever solutions.
			pathFilters = pathMyFolders;
		}
	}
	return pathFilters;
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

/**
 * @brief Handles menu selection from recent projects list
 * @param [in] nID Menu ID of the selected item
 */
BOOL CMergeApp::OnOpenRecentFile(UINT nID)
{
	return LoadAndOpenProjectFile((const TCHAR *)m_pRecentFileList->m_arrNames[nID-ID_FILE_MRU_FILE1]);
}
