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

/////////////////////////////////////////////////////////////////////////////
// CMergeApp construction

CMergeApp::CMergeApp()
: m_lang(IDR_MAINFRAME, IDR_MAINFRAME)
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

	// CCrystalEdit Drag and Drop functionality needs AfxOleInit.
	if(!AfxOleInit())
	{
		TRACE(_T("AfxOleInitFailed. OLE functionality disabled"));
	}

	// CG: The following block was added by the Splash Screen component.
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		CSplashWnd::EnableSplashScreen(cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew);
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

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_lang.SetLogFile(&gLog);
	m_lang.InitializeLanguage();

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
	for (int i = 1; i < __argc; i++)
	{
		LPCTSTR pszParam = __targv[i];
		if (pszParam[0] == '-' || pszParam[0] == '/')
		{
			// remove flag specifier
			++pszParam;

			if (!_tcsicmp(pszParam, _T("r")))
				recurse=TRUE;
		}
		else
		{
			files.SetAtGrow(nFiles, pszParam);
			nFiles++;
		}
	}

	if (nFiles>2)
	{
		pMainFrame->m_strSaveAsPath = files[2];
		pMainFrame->DoFileOpen(files[0], files[1], recurse);
	}
	else if (nFiles>1)
	{
		pMainFrame->m_strSaveAsPath = _T("");
		pMainFrame->DoFileOpen(files[0], files[1], recurse);
	}
	else if (nFiles>0)
	{
		pMainFrame->m_strSaveAsPath = _T("");
		pMainFrame->DoFileOpen(files[0], "", recurse);
	}

	return TRUE;
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
	AfxFormatString1(m_strVersion, IDS_VERSION_FMT, version.GetProductVersion());

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
			 UINT filter /*=0*/,
			 BOOL is_open /*=TRUE*/) 
{
	CString s;        
	TCHAR buf[MAX_PATH*2] = _T("Directory Selection");;
                   
	if (filter != 0)
		VERIFY(s.LoadString(filter)); 
	else
		VERIFY(s.LoadString(IDS_ALLFILES)); 
	CFileDialog dlg(is_open, NULL, NULL, 
				    OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, s);
	dlg.m_ofn.lpstrTitle = (LPCTSTR)title;
	dlg.m_ofn.lpstrInitialDir = (LPTSTR)root_path;

	// check if specified path is a file
	if (root_path!=NULL)
	{
		CFileStatus status;
		if (CFile::GetStatus(root_path,status)
			&& (status.m_attribute!=CFile::Attribute::directory))
		{
			split_filename(root_path, NULL, buf, NULL);
		}
	}


	
	dlg.m_ofn.lpstrFile = buf;
	dlg.m_ofn.nMaxFile = MAX_PATH*2;
	if (dlg.DoModal()==IDOK)
	{
	 	path = dlg.GetPathName(); 
	 	return TRUE;
	}
	path.Empty();
	return FALSE;	   
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
TCHAR recurse_dir_regex[MAX_PATH];

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
	TCHAR teststring[][MAX_PATH] = {
		"test.cpp",
			"test.c",
			"test.h",
			"test.x",
			"test.cpp2",
			".cpp",
			"cpp",
			"acpp",
			""
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
