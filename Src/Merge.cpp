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
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "Merge.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "DirFrame.h"
#include "MergeDoc.h"
#include "DirDoc.h"
#include "DirView.h"
#include "Splash.h"
#include "version.h"
#include "statlink.h"
#include "logfile.h"
#include "coretools.h"
#include "paths.h"
#include "sinstance.h"
#include "FileFilterHelper.h"
#include "Plugins.h"
#include "DirScan.h" // for DirScan_InitializeDefaultCodepage
#include "ProjectFile.h"

#include "MergeEditView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeApp

BEGIN_MESSAGE_MAP(CMergeApp, CWinApp)
	//{{AFX_MSG_MAP(CMergeApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_VIEW_LANGUAGE, OnViewLanguage)
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

extern CLogFile gLog;

static void AddEnglishResourceHook();

/////////////////////////////////////////////////////////////////////////////
// CMergeApp construction

CMergeApp::CMergeApp() :
  m_bNeedIdleTimer(FALSE)
, m_pDiffTemplate(0)
, m_pDirTemplate(0)
, m_lang(IDR_MAINFRAME, IDR_MAINFRAME)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMergeApp object

CMergeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMergeApp initialization

BOOL CMergeApp::InitInstance()
{
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

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Set default codepage
	DirScan_InitializeDefaultCodepage();

	// WinMerge registry settings are stored under HKEY_CURRENT_USER/Software/Thingamahoochie
	// This is the name of the company of the original author (Dean Grimm)
	SetRegistryKey(_T("Thingamahoochie"));

	BOOL bSingleInstance = GetProfileInt(_T("Settings"), _T("SingleInstance"), FALSE);
	
	HANDLE hMutex = NULL;
	if (bSingleInstance)
	{
		hMutex = CreateMutex(NULL, FALSE, _T("WinMerge{05963771-8B2E-11d8-B3B9-000000000000}"));
		WaitForSingleObject(hMutex, INFINITE);
	}

	CInstanceChecker instanceChecker(_T("{05963771-8B2E-11d8-B3B9-000000000000}"));
	if (bSingleInstance)
	{
		if (instanceChecker.PreviousInstanceRunning())
		{
			// Activate previous instance and send commandline to it
			HWND hWnd = instanceChecker.ActivatePreviousInstance();
			
			TCHAR *pszArgs = new TCHAR[_tcslen(__targv[0]) + _tcslen(m_lpCmdLine) + 3];
			TCHAR *p = pszArgs;
			for (int i = 0; i < __argc; i++)
				p += wsprintf(p, _T("%s"), __targv[i]) + 1;
			*p++ = _T('\0');
			COPYDATASTRUCT data = {0};
			data.cbData = (DWORD)(p - pszArgs) * sizeof(TCHAR);
			data.lpData = pszArgs;
			data.dwData = __argc;
			SendMessage(hWnd, WM_COPYDATA, NULL, (LONG)&data);
			delete[] pszArgs;

			ReleaseMutex(hMutex);
			CloseHandle(hMutex);

			return FALSE;
		}
	}

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)
	BOOL bDisableSplash	= GetProfileInt(_T("Settings"), _T("DisableSplash"), FALSE);

	InitializeFileFilters();
	m_globalFileFilter.SetFilter(_T("*.*"));

	// CG: The following block was added by the Splash Screen component.
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		CSplashWnd::EnableSplashScreen(bDisableSplash==FALSE && cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew);
	}

	// Initialize i18n (multiple language) support

	m_lang.SetLogFile(&gLog);
	m_lang.InitializeLanguage();

	AddEnglishResourceHook(); // Use English string when l10n (foreign) string missing

	m_mainThreadScripts = new CAssureScriptsForThread;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	// Merge Edit view
	m_pDiffTemplate = new CMultiDocTemplate(
		IDR_MERGETYPE,
		RUNTIME_CLASS(CMergeDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMergeEditView));
	AddDocTemplate(m_pDiffTemplate);

	// Directory view
	m_pDirTemplate = new CMultiDocTemplate(
		IDR_MERGETYPE,
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
	m_pDirTemplate->m_hMenuShared = pMainFrame->NewDirViewMenu();
	pMainFrame->m_hMenuDefault = pMainFrame->NewDefaultMenu();

	// Set the menu
	// Note : for Windows98 compatibility, use FromHandle and not Attach/Detach
	CMenu * pNewMenu = CMenu::FromHandle(pMainFrame->m_hMenuDefault);
	pMainFrame->MDISetMenu(pNewMenu, NULL);

	// Parse command line for standard shell commands, DDE, file open
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	/* Dispatch commands specified on the command line
	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew )
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;*/

	//Track it so any other instances can find it.
	instanceChecker.TrackFirstInstanceRunning();

	// The main window has been initialized, so show and update it.
	//pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->ActivateFrame(m_nCmdShow);
	pMainFrame->UpdateWindow();

	ParseArgsAndDoOpen(__argc, __targv, pMainFrame);

	if (hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}

	return TRUE;
}

void CMergeApp::ParseArgsAndDoOpen(int argc, TCHAR *argv[], CMainFrame* pMainFrame)
{
	CStringArray files;
	UINT nFiles=0;
	BOOL recurse=FALSE;
	files.SetSize(2);
	DWORD dwLeftFlags = FFILEOPEN_NONE;
	DWORD dwRightFlags = FFILEOPEN_NONE;

	// Split commandline arguments into files & flags & recursive flag
	ParseArgs(argc, argv, pMainFrame, files, nFiles, recurse, dwLeftFlags, dwRightFlags);

	if (LoadProjectFile(files, recurse))
	{
		if (!files[0].IsEmpty())
			dwLeftFlags |= FFILEOPEN_PROJECT;
		if (!files[1].IsEmpty())
			dwRightFlags |= FFILEOPEN_PROJECT;
		pMainFrame->m_strSaveAsPath = _T("");
		pMainFrame->DoFileOpen(files[0], files[1],
			dwLeftFlags, dwRightFlags, recurse);
	}
	else if (nFiles>2)
	{
		dwLeftFlags |= FFILEOPEN_CMDLINE;
		dwRightFlags |= FFILEOPEN_CMDLINE;
		pMainFrame->m_strSaveAsPath = files[2];
		pMainFrame->DoFileOpen(files[0], files[1],
			dwLeftFlags, dwRightFlags, recurse);
	}
	else if (nFiles>1)
	{
		dwLeftFlags |= FFILEOPEN_CMDLINE;
		dwRightFlags |= FFILEOPEN_CMDLINE;
		pMainFrame->m_strSaveAsPath = _T("");
		pMainFrame->DoFileOpen(files[0], files[1],
			dwLeftFlags, dwRightFlags, recurse);
	}
	else if (nFiles>0)
	{
		dwLeftFlags |= FFILEOPEN_CMDLINE;
		pMainFrame->m_strSaveAsPath = _T("");
		pMainFrame->DoFileOpen(files[0], _T(""),
			dwLeftFlags, dwRightFlags, recurse);
	}
}

/// Process commandline arguments
void CMergeApp::ParseArgs(int argc, TCHAR *argv[], CMainFrame* pMainFrame, CStringArray & files, UINT & nFiles, BOOL & recurse,
		DWORD & dwLeftFlags, DWORD & dwRightFlags)
{
	for (int i = 1; i < argc; i++)
	{
		LPCTSTR pszParam = argv[i];
		if (pszParam[0] == '-' || pszParam[0] == '/')
		{
			// remove flag specifier
			++pszParam;

			// -? for help
			if (!_tcsicmp(pszParam, _T("?")))
			{
				CString s;
				VERIFY(s.LoadString(IDS_QUICKHELP));
				AfxMessageBox(s, MB_ICONINFORMATION);
			}

			// -r to compare recursively
			if (!_tcsicmp(pszParam, _T("r")))
				recurse = TRUE;

			// -e to allow closing with single esc press
			if (!_tcsicmp(pszParam, _T("e")))
				pMainFrame->m_bEscShutdown = TRUE;

			// -wl to open left path as read-only
			if (!_tcsicmp(pszParam, _T("wl")))
				dwLeftFlags |= FFILEOPEN_READONLY;

			// -wr to open right path as read-only
			if (!_tcsicmp(pszParam, _T("wr")))
				dwRightFlags |= FFILEOPEN_READONLY;

			// -ul to not add left path to MRU
			if (!_tcsicmp(pszParam, _T("ul")))
				dwLeftFlags |= FFILEOPEN_NOMRU;

			// -ur to not add right path to MRU
			if (!_tcsicmp(pszParam, _T("ur")))
				dwRightFlags |= FFILEOPEN_NOMRU;

			// -ub to not add paths to MRU
			if (!_tcsicmp(pszParam, _T("ub")))
			{
				dwLeftFlags |= FFILEOPEN_NOMRU;
				dwRightFlags |= FFILEOPEN_NOMRU;
			}

			// -dl "desc" - description for left file
			// Shown instead of filename
			if (!_tcsicmp(pszParam, _T("dl")))
			{
				if (i < (argc - 1))
				{
					LPCTSTR pszDesc = argv[i+1];
					pMainFrame->m_strLeftDesc = pszDesc;
					i++;	// Just read next parameter
				}
			}

			// -dr "desc" - description for left file
			// Shown instead of filename
			if (!_tcsicmp(pszParam, _T("dr")))
			{
				if (i < (argc - 1))
				{
					LPCTSTR pszDesc = argv[i+1];
					pMainFrame->m_strRightDesc = pszDesc;
					i++;	// Just read next parameter
				}
			}

			// -f "mask" - file filter mask ("*.h *.cpp")
			if (!_tcsicmp(pszParam, _T("f")))
			{
				if (i < (argc - 1))
				{
					CString sFilter = argv[i+1];
					sFilter.TrimLeft();
					sFilter.TrimRight();
					m_globalFileFilter.SetFilter(sFilter);
					i++;	// Just read next parameter
				}
			}
		}
		else
		{
			CString sParam = pszParam;
			CString sFile = paths_GetLongPath(sParam);
			files.SetAtGrow(nFiles, sFile);
			nFiles++;
		}
	}

	// if "compare file dir" make it "compare file dir\file"
	if (nFiles >= 2)
	{
		PATH_EXISTENCE p1 = paths_DoesPathExist(files[0]);
		PATH_EXISTENCE p2 = paths_DoesPathExist(files[1]);
		if (p1 == IS_EXISTING_FILE && p2 == IS_EXISTING_DIR)
		{
			TCHAR fname[_MAX_PATH], fext[_MAX_PATH];
			_tsplitpath(files[0], NULL, NULL, fname, fext);
			if (files[1].Right(1) != _T('\\'))
				files[1] += _T('\\');
			files[1] = files[1] + fname + fext;
		}
	}


	// Reload menus in case a satellite language dll was loaded above
	m_lang.ReloadMenu();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

/** 
 * @brief About-dialog class
 */
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_ctlCompany;
	CStaticLink	m_ctlWWW;
	CString	m_strVersion;
	CString m_strPrivateBuild;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenContributors();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_COMPANY, m_ctlCompany);
	DDX_Control(pDX, IDC_WWW, m_ctlWWW);
	DDX_Text(pDX, IDC_VERSION, m_strVersion);
	DDX_Text(pDX, IDC_PRIVATEBUILD, m_strPrivateBuild);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_OPEN_CONTRIBUTORS, OnBnClickedOpenContributors)
END_MESSAGE_MAP()

// App command to run the dialog
void CMergeApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/** 
 * @brief Read version info from resource to dialog.
 */
BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CVersionInfo version;
	CString sVersion = version.GetFixedProductVersion();
	AfxFormatString1(m_strVersion, IDS_VERSION_FMT, sVersion);

	CString sPrivateBuild = version.GetPrivateBuild();
	if (!sPrivateBuild.IsEmpty())
	{
		AfxFormatString1(m_strPrivateBuild, IDS_PRIVATEBUILD_FMT, sPrivateBuild);
	}

	m_ctlCompany.SetWindowText(version.GetLegalCopyright());
	m_ctlWWW.m_link = _T("http://winmerge.org");

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CMergeApp commands

/** 
 * @brief Helper function for selecting dir/file
 * @param [out] path Selected path is returned in this string
 * @param [in] root_path Initial path (and file) shown when dialog is opened
 * @param [in] title Title for path selection dialog
 * @param [in] filterid 0 or STRING ID for filter string - 0 means "All files (*.*)"
 * @param [in] is_open Selects Open/Save -dialog
 */
BOOL SelectFile(CString& path, LPCTSTR root_path /*=NULL*/, 
			 LPCTSTR title /*= _T("Open")*/, 
			 UINT filterid /*=0*/,
			 BOOL is_open /*=TRUE*/) 
{
	CString sfile;

	// check if specified path is a file
	if (root_path!=NULL)
	{
		CFileStatus status;
		if (CFile::GetStatus(root_path,status)
			&& (status.m_attribute!=CFile::Attribute::directory))
		{
			SplitFilename(root_path, 0, &sfile, 0);
		}
	}
	
	CString filters;
	if (filterid != 0)
		VERIFY(filters.LoadString(filterid));
	else
		VERIFY(filters.LoadString(IDS_ALLFILES));
	DWORD flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	CFileDialog dlg(is_open, NULL, sfile, flags, filters);
	dlg.m_ofn.lpstrTitle = (LPCTSTR)title;
	dlg.m_ofn.lpstrInitialDir = (LPTSTR)root_path;

	if (dlg.DoModal()==IDOK)
	{
	 	path = dlg.GetPathName();
	 	return TRUE;
	}
	path = _T("");
	return FALSE;	   
}

/** 
 * @brief Helper function for selecting directory
 * @param [out] path Selected path is returned in this string
 * @param [in] root_path Initial path shown when dialog is opened
 * @param [in] title Title for path selection dialog
 * @param [in] hwndOwner Handle to owner window or NULL
 * @return TRUE if valid folder selected (not cancelled)
 */
BOOL SelectFolder(CString& path, LPCTSTR root_path /*=NULL*/, 
			LPCTSTR title /*=NULL*/, 
			HWND hwndOwner /*=NULL*/) 
{
	BROWSEINFO bi;
	LPMALLOC pMalloc;
	LPITEMIDLIST pidl;
	TCHAR szPath[MAX_PATH] = {0};
	BOOL bRet = FALSE;
	
	bi.hwndOwner = hwndOwner;
	bi.pidlRoot = NULL;  // Start from desktop folder
	bi.pszDisplayName = szPath;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_RETURNONLYFSDIRS; // | BIF_EDITBOX
	bi.lpfn = NULL;
	bi.lParam = NULL;

	pidl = SHBrowseForFolder(&bi);

	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szPath))
		{
			path = szPath;
			bRet = TRUE;
		}

		SHGetMalloc(&pMalloc);
		pMalloc->Free(pidl);
		pMalloc->Release();
	}
	return bRet;
}

BOOL CMergeApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}

void CMergeApp::OnViewLanguage() 
{
	if (m_lang.DoModal()==IDOK)
	{
		//m_lang.ReloadMenu();
		//m_LangDlg.UpdateDocTitle();
		mf->UpdateResources();
	}
}

int CMergeApp::ExitInstance() 
{
	delete m_mainThreadScripts;
	return CWinApp::ExitInstance();
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
		pParentWnd = GetMainWnd()->GetLastActivePopup();
	}

	// Use our own message box implementation, which adds the
	// do not show again checkbox, and implements it on subsequent calls
	// (if caller set the style)
	
	// Create the message box dialog.
	CMessageBoxDialog dlgMessage(pParentWnd, lpszPrompt, _T(""), nType,
		nIDPrompt);
	
	// Display the message box dialog and return the result.
	return dlgMessage.DoModal();
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

/** @brief Load any known file filters */
void CMergeApp::InitializeFileFilters()
{
	m_globalFileFilter.LoadAllFileFilters();
}

/** @brief Open help from mainframe when user presses F1*/
void CMergeApp::OnHelp()
{
	if (mf)
		mf->ShowHelp();
}

/** @brief Open Contributors.rtf */
void CAboutDlg::OnBnClickedOpenContributors()
{
	CString defPath = GetModulePath();
	// Don't add quotation marks yet, CFile doesn't like them
	CString docPath = defPath + _T("\\contributors.txt");
	HINSTANCE ret = 0;
	
	CFileStatus status;
	if (CFile::GetStatus(docPath, status))
	{
		// Now, add quotation marks so ShellExecute() doesn't fail if path
		// includes spaces
		docPath.Insert(0, _T("\""));
		docPath.Insert(docPath.GetLength(), _T("\""));
		ret = ShellExecute(m_hWnd, NULL, _T("notepad"), docPath, defPath, SW_SHOWNORMAL);

		// values < 32 are errors (ref to MSDN)
		if ((int)ret < 32)
		{
			// Try to open with associated application (.rtf)
			ret = ShellExecute(m_hWnd, _T("open"), docPath, NULL, NULL, SW_SHOWNORMAL);
			if ((int)ret < 32)
			{
				CString msg;
				AfxFormatString1(msg, IDS_CANNOT_EXECUTE_FILE, _T("Notepad.exe"));
				AfxMessageBox(msg, MB_OK | MB_ICONSTOP);
			}
		}
	}
	else
	{
		CString msg;
		AfxFormatString1(msg, IDS_ERROR_FILE_NOT_FOUND, docPath);
		AfxMessageBox(msg, MB_OK | MB_ICONSTOP);
	}
}

/** 
 * @brief Read paths and filter from project file.
 *
 * Tries to find project file in files[0] and files[1] by extension
 * If cannot find one, returns FALSE
 */
BOOL CMergeApp::LoadProjectFile(CStringArray & files, BOOL & recursive)
{
	CString filterPrefix;
	CString err;
	ProjectFile pfile;
	CString ProjectFileName;
	CString ext;

	// Look for project file in files[0] and files[1]

	if (files.GetSize() < 2)
		return FALSE; // code further down assumes files[0] and files[1] exist

	SplitFilename(files[0], NULL, NULL, &ext);
	if (ext == PROJECTFILE_EXT)
	{
		ProjectFileName = files[0];
	}
	else
	{
		if (files.GetSize() == 1)
			return FALSE;
		SplitFilename(files[1], NULL, NULL, &ext);
		if (ext == PROJECTFILE_EXT)
			ProjectFileName = files[1];
		else
			return FALSE;
	}

	// We found project file, and stored it in ProjectFileName

	if (!ProjectFileName.IsEmpty())
	{
		if (!pfile.Read(ProjectFileName, &err))
		{
			if (!err.IsEmpty())
			{
				CString msg;
				AfxFormatString2(msg, IDS_ERROR_FILEOPEN, ProjectFileName, err);
				AfxMessageBox(msg, MB_ICONSTOP);
			}
			return FALSE;
		}
		else
		{
			pfile.GetPaths(files[0], files[1], recursive);
			if (pfile.HasFilter())
			{
				CString filter = pfile.GetFilter();
				filter.TrimLeft();
				filter.TrimRight();
				m_globalFileFilter.SetFilter(filter);
			}
		}
		return TRUE;
	}
	return FALSE;
}
