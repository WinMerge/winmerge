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
// Merge.cpp : Defines the class behaviors for the application.
//

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
#include "FileFilterMgr.h"

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
	//}}AFX_MSG_MAP
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

#ifdef _DEBUG
void SillyTestCrap();
#endif

extern CLogFile gLog;

static void AddEnglishResourceHook();

/////////////////////////////////////////////////////////////////////////////
// CMergeApp construction

CMergeApp::CMergeApp()
: m_bHiliteSyntax(TRUE)
, m_bDisableSplash(FALSE)
, m_clrDiff(RGB(255,255,92))
, m_clrSelDiff(RGB(255,0,92))
, m_bNeedIdleTimer(FALSE)
, m_pDiffTemplate(0)
, m_pDirTemplate(0)
, m_lang(IDR_MAINFRAME, IDR_MAINFRAME)
, m_fileFilterMgr(0)
, m_currentFilter(0)
, m_bEscCloses(FALSE)
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
#ifdef _DEBUG
	SillyTestCrap();
#endif

	// Runtime switch so programmer may set this in interactive debugger
	int dbgmem = 0;
	if (dbgmem)
	{
		// get current setting
		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
		// Keep freed memory blocks in the heap’s linked list and mark them as freed
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

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Thingamahoochie"));

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	m_bDisableSplash = GetProfileInt(_T("Settings"), _T("DisableSplash"), FALSE);
	m_bHiliteSyntax = GetProfileInt(_T("Settings"), _T("HiliteSyntax"), TRUE)!=0;

	m_clrDiff    = GetProfileInt(_T("Settings"), _T("DifferenceColor"), m_clrDiff);
	m_clrSelDiff = GetProfileInt(_T("Settings"), _T("SelectedDifferenceColor"), m_clrSelDiff);

	// CG: The following block was added by the Splash Screen component.
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		CSplashWnd::EnableSplashScreen(m_bDisableSplash==FALSE && cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew);
	}

	// Initialize i18n (multiple language) support

	m_lang.SetLogFile(&gLog);
	m_lang.InitializeLanguage();

	AddEnglishResourceHook(); // Use English string when l10n (foreign) string missing

	InitializeFileFilters();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_pDiffTemplate = new CMultiDocTemplate(
		IDR_MERGETYPE,
		RUNTIME_CLASS(CMergeDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CMergeEditView));
	AddDocTemplate(m_pDiffTemplate);
	m_pDirTemplate = new CMultiDocTemplate(
		IDR_MERGETYPE,
		RUNTIME_CLASS(CDirDoc),
		RUNTIME_CLASS(CDirFrame), // custom MDI child frame
		RUNTIME_CLASS(CDirView));
	AddDocTemplate(m_pDirTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	// Enable drag&drop files
	pMainFrame->ModifyStyleEx(NULL, WS_EX_ACCEPTFILES);

	pMainFrame->m_hMenuDefault=pMainFrame->NewDefaultMenu();

	// This simulates a window being opened if you don't have
	// a default window displayed at startup
	pMainFrame->OnUpdateFrameMenu(pMainFrame->m_hMenuDefault);

	// Parse command line for standard shell commands, DDE, file open
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	/* Dispatch commands specified on the command line
	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew )
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;*/

	// The main window has been initialized, so show and update it.
	//pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->ActivateFrame(m_nCmdShow);
	pMainFrame->UpdateWindow();

	CStringArray files;
	UINT nFiles=0;
	BOOL recurse=FALSE;
	files.SetSize(2);
	DWORD dwLeftFlags = 0;
	DWORD dwRightFlags = 0;
	// Split commandline arguments into files & flags & recursive flag
	ParseArgs(files, nFiles, recurse, dwLeftFlags, dwRightFlags);

	if (nFiles>2)
	{
		pMainFrame->m_strSaveAsPath = files[2];
		pMainFrame->DoFileOpen(files[0], files[1],
			dwLeftFlags, dwRightFlags, recurse);
	}
	else if (nFiles>1)
	{
		pMainFrame->m_strSaveAsPath = _T("");
		pMainFrame->DoFileOpen(files[0], files[1],
			dwLeftFlags, dwRightFlags, recurse);
	}
	else if (nFiles>0)
	{
		pMainFrame->m_strSaveAsPath = _T("");
		pMainFrame->DoFileOpen(files[0], "",
			dwLeftFlags, dwRightFlags, recurse);
	}
	return TRUE;
}

// Process commandline arguments
void CMergeApp::ParseArgs(CStringArray & files, UINT & nFiles, BOOL & recurse, DWORD & dwLeftFlags, DWORD & dwRightFlags)
{
	for (int i = 1; i < __argc; i++)
	{
		LPCTSTR pszParam = __targv[i];
		if (pszParam[0] == '-' || pszParam[0] == '/')
		{
			// remove flag specifier
			++pszParam;

			// -r to compare recursively
			if (!_tcsicmp(pszParam, _T("r")))
				recurse=TRUE;

			// -e to allow closing with single esc press
			if (!_tcsicmp(pszParam, _T("e")))
				m_bEscCloses = TRUE;

			// -ur to not add left path to MEU
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

		}
		else
		{
			CString sParam = pszParam;
			CString sFile = paths_GetLongPath(sParam, DIRSLASH);
			files.SetAtGrow(nFiles, sFile);
			nFiles++;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

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
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strVersion = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_COMPANY, m_ctlCompany);
	DDX_Control(pDX, IDC_WWW, m_ctlWWW);
	DDX_Text(pDX, IDC_VERSION, m_strVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CMergeApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CVersionInfo version;
	CString sVersion = version.GetFixedProductVersion();
	AfxFormatString1(m_strVersion, IDS_VERSION_FMT, sVersion);

	m_ctlCompany.SetWindowText(version.GetLegalCopyright());
	m_ctlWWW.m_link = _T("http://winmerge.sourceforge.net");

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CMergeApp commands

BOOL SelectFile(CString& path, LPCTSTR root_path /*=NULL*/, 
			 LPCTSTR title /*= _T("Open")*/, 
			 UINT filterid /*=0*/,
			 BOOL is_open /*=TRUE*/) 
{
	CString sfile = _T("Directory Selection");

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

BOOL CMergeApp::PreTranslateMessage(MSG* pMsg)
{
	// Check if we got 'ESC pressed' -message
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE)) 
	{
		// If /e commandline parameter were given close WinMerge
		// NOTE: Without /e commandline parameter we only close
		// active window/dialog. See CDirView::PreTranslateMessage()
		// and CMergeEditView::PreTranslateMessage()
		if (m_bEscCloses)
		{
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return FALSE;
		}
	}

	// CG: The following lines were added by the Splash Screen component.
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}

void CMergeApp::OnViewLanguage() 
{
	if (m_lang.DoModal()==IDOK)
	{
		m_lang.ReloadMenu();
		//m_LangDlg.UpdateDocTitle();
		mf->UpdateResources();
	}
}


#ifdef _DEBUG
#define __STDC__ 1
#include "RegExp.h"
#include "direct.h"

typedef BOOL (*RecursiveFindCallback)(WIN32_FIND_DATA &fd, LPCTSTR pszPath, LPVOID pUserData);
TCHAR recurse_dir_regex[_MAX_PATH] = {0};

BOOL MyRecursiveFindCallback(WIN32_FIND_DATA &fd, LPCTSTR pszPath, LPVOID pUserData)
{
	TRACE(_T("%s\\%s\n"), pszPath, fd.cFileName);
	return TRUE;
}

BOOL recursive_find_regex(CRegExp& regex, 
						  RecursiveFindCallback pCallback,
						  LPVOID pUserData)
{
	WIN32_FIND_DATA fd;
	HANDLE hff;
	TCHAR *p;
	
	// open the directory for reading
	if ((hff = FindFirstFile(_T("*.*"), &fd)) != INVALID_HANDLE_VALUE)
	{
		do {

			// if the current entry is a directory, recurse into it
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(fd.cFileName,_T("."))
					&& _tcscmp(fd.cFileName,_T("..")))
				{
					if (_tchdir(fd.cFileName)==0)
					{ 
						_tcscat(recurse_dir_regex,_T("\\"));
						_tcscat(recurse_dir_regex,fd.cFileName);

						if (regex.RegFind(fd.cFileName) != -1)
							if (!pCallback(fd, recurse_dir_regex, pUserData))
								return FALSE;

						if (!recursive_find_regex(regex, pCallback, pUserData))
							return FALSE;

						_tchdir(_T(".."));
						if ((p=_tcsrchr(recurse_dir_regex,_T('\\')))!=NULL)
							*p=_T('\0');
					}
					//else
					//	add_err(recurse_dir_regex, fd.cFileName, _T("Couldn't read folder"));
					
				}
			}
			// entry is a file, delete it
			else
			{
				if (regex.RegFind(fd.cFileName) != -1)
					if (!pCallback(fd, recurse_dir_regex, pUserData))
						return FALSE;
			}
		} while (FindNextFile(hff,&fd));
		FindClose(hff);
	}
	else
	{
		//add_err(recurse_dir, _T(""), _T("No permission to open folder"));
		return FALSE;
	}
	return TRUE;
}


BOOL RecursiveFindRegex(LPCTSTR szRegex, 
						LPCTSTR szStartPath, 
						RecursiveFindCallback pCallback,
						LPVOID pUserData)
{
	CRegExp regex;
	regex.RegComp( szRegex );

	// change the current drive if drive mapped
	if(szStartPath[1]==_T(':'))
	{
		CString s(szStartPath[0]);
		s.MakeUpper();
		int drive = s[0]-_T('A')+1;
		if( _chdrive(drive) != 0)
			return FALSE;
	}	
	

	// change to the folder we want to delete
	CString s(szStartPath);
	if (s.Right(1) == ":")
		s += '\\';
	if (_tchdir(s)!=0)
		return FALSE;
	
	_tcscpy(recurse_dir_regex, szStartPath);
	return recursive_find_regex(regex, pCallback, pUserData);
}


void SillyTestCrap()
{
	TCHAR teststring[][_MAX_PATH] = {
		_T("test.cpp"),
			_T("test.c"),
			_T("test.h"),
			_T("test.x"),
			_T("test.cpp2"),
			_T(".cpp"),
			_T("cpp"),
			_T("acpp"),
			_T("")
	};
	TCHAR ext[] = _T("*.cpp;*.h;*.c");
	LPTSTR p;
	CString strPattern(_T(".*\\.("));

	// parse the extensions
	p = _tcstok(ext, _T(";,|*. \n\r\n"));
	if (p == NULL)
		return;

	while (p != NULL)
	{
		strPattern += p;		
		p = _tcstok(NULL, _T(";,|*. \n\r\n"));
		if (p != NULL)
			strPattern += _T('|');
	}
	strPattern += _T(")$");

	RecursiveFindRegex(strPattern, _T("f:\\programs\\merge"), MyRecursiveFindCallback, (LPVOID)AfxGetApp());

	/*CRegExp r;
	r.RegComp( strPattern );
	for (UINT i=0; *teststring[i] != NULL; i++)
	{
		if (r.RegFind((LPTSTR)teststring[i]) != -1)
			TRACE("%s: Match\n", teststring[i]);
		else
			TRACE("%s: No match\n", teststring[i]);
	}*/

}
#endif


void CMergeApp::SetDiffColor(COLORREF clrValue)
{
	m_clrDiff = clrValue;
}


void CMergeApp::SetSelDiffColor(COLORREF clrValue)
{
	m_clrSelDiff = clrValue;
}


int CMergeApp::ExitInstance() 
{
	WriteProfileInt(_T("Settings"), _T("DifferenceColor"), m_clrDiff);
	WriteProfileInt(_T("Settings"), _T("SelectedDifferenceColor"), m_clrSelDiff);
	
	delete m_fileFilterMgr;

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


int CMergeApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt) 
{
	// This is just a convenient point for breakpointing
	return CWinApp::DoMessageBox(lpszPrompt, nType, nIDPrompt);
}

// Set flag so that application will broadcast notification at next
// idle time (via WM_TIMER id=IDLE_TIMER)
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

// Load any known file filters
void CMergeApp::InitializeFileFilters()
{
	if (!m_fileFilterMgr)
		m_fileFilterMgr = new FileFilterMgr;

	CString sPattern = GetModulePath() + _T("\\Filters\\*.flt");
	m_fileFilterMgr->LoadFromDirectory(sPattern, _T(".flt"));
}

// fill list with names of known filters
void CMergeApp::GetFileFilterNameList(CStringList & filefilters, CString & selected) const
{
	if (!m_fileFilterMgr) return;
	for (int i=0; i<m_fileFilterMgr->GetFilterCount(); ++i)
	{
		filefilters.AddTail(m_fileFilterMgr->GetFilterName(i));
	}
	selected = m_sFileFilterName;
}

// Store current filter (if filter manager validates the name)
void CMergeApp::SetFileFilterName(LPCTSTR szFileFilterName)
{
	m_sFileFilterName = _T("<None>");
	if (!m_fileFilterMgr) return;
	m_currentFilter = m_fileFilterMgr->GetFilter(szFileFilterName);
	if (m_currentFilter)
		m_sFileFilterName = szFileFilterName;
}

BOOL CMergeApp::includeFile(LPCTSTR szFileName)
{
	if (!m_fileFilterMgr || !m_currentFilter) return TRUE;
	return m_fileFilterMgr->TestFileNameAgainstFilter(m_currentFilter, szFileName);
}

BOOL CMergeApp::includeDir(LPCTSTR szDirName)
{
	if (!m_fileFilterMgr || !m_currentFilter) return TRUE;
	return m_fileFilterMgr->TestDirNameAgainstFilter(m_currentFilter, szDirName);
}

