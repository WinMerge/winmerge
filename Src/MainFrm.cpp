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
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Merge.h"

#include <direct.h>
#include "MainFrm.h"
#include "ChildFrm.h"
#include "DirView.h"
#include "DirDoc.h"
#include "OpenDlg.h"
#include "MergeEditView.h"

#include "diff.h"
#include "getopt.h"
#include "fnmatch.h"
#include "coretools.h"
#include "Splash.h"
#include "VssPrompt.h"
#include "CCPrompt.h"
#include "PropVss.h"
#include "PropGeneral.h"
#include "PropFilter.h"
#include "PropColors.h"
#include "RegKey.h"
#include "logfile.h"
#include "PropSyntax.h"
#include "ssapi.h"      // BSP - Includes for Visual Source Safe COM interface

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int recursive;
CMainFrame *mf = NULL;
extern CLogFile gLog;
extern bool gWriteLog;

// add a 
static void add_regexp PARAMS((struct regexp_list **, char const*));
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_COMMAND(ID_OPTIONS_SHOWDIFFERENT, OnOptionsShowDifferent)
	ON_COMMAND(ID_OPTIONS_SHOWIDENTICAL, OnOptionsShowIdentical)
	ON_COMMAND(ID_OPTIONS_SHOWUNIQUELEFT, OnOptionsShowUniqueLeft)
	ON_COMMAND(ID_OPTIONS_SHOWUNIQUERIGHT, OnOptionsShowUniqueRight)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWDIFFERENT, OnUpdateOptionsShowdifferent)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWIDENTICAL, OnUpdateOptionsShowidentical)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWUNIQUELEFT, OnUpdateOptionsShowuniqueleft)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWUNIQUERIGHT, OnUpdateOptionsShowuniqueright)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_HIDE_BACKUP_FILES, OnUpdateHideBackupFiles)
	ON_COMMAND(ID_HELP_GNULICENSE, OnHelpGnulicense)
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_HIDE_BACKUP_FILES, OnHideBackupFiles)
	ON_COMMAND(ID_VIEW_SELECTFONT, OnViewSelectfont)
	ON_COMMAND(ID_VIEW_USEDEFAULTFONT, OnViewUsedefaultfont)
	ON_UPDATE_COMMAND_UI(ID_VIEW_USEDEFAULTFONT, OnUpdateViewUsedefaultfont)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_UPDATE_COMMAND_UI(ID_HELP_CONTENTS, OnUpdateHelpContents)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_pLeft = m_pRight = NULL;
	m_pMergeDoc=NULL;
	m_pDirDoc=NULL;
	m_bFontSpecified=FALSE;
	m_strSaveAsPath = _T("");
	m_bFirstTime = TRUE;

	m_bIgnoreBlankLines = theApp.GetProfileInt(_T("Settings"), _T("IgnoreBlankLines"), FALSE)!=0;
	m_bEolSensitive = theApp.GetProfileInt(_T("Settings"), _T("EolSensitive"), FALSE)!=0;
	m_bIgnoreCase = theApp.GetProfileInt(_T("Settings"), _T("IgnoreCase"), FALSE)!=0;
	m_bShowUniqueLeft = theApp.GetProfileInt(_T("Settings"), _T("ShowUniqueLeft"), TRUE)!=0;
	m_bShowUniqueRight = theApp.GetProfileInt(_T("Settings"), _T("ShowUniqueRight"), TRUE)!=0;
	m_bShowDiff = theApp.GetProfileInt(_T("Settings"), _T("ShowDifferent"), TRUE)!=0;
	m_bShowIdent = theApp.GetProfileInt(_T("Settings"), _T("ShowIdentical"), TRUE)!=0;
	m_bBackup = theApp.GetProfileInt(_T("Settings"), _T("BackupFile"), TRUE)!=0;
	m_bScrollToFirst = theApp.GetProfileInt(_T("Settings"), _T("ScrollToFirst"), FALSE)!=0;
	m_nIgnoreWhitespace = theApp.GetProfileInt(_T("Settings"), _T("IgnoreSpace"), 1);
	m_bHideBak = theApp.GetProfileInt(_T("Settings"), _T("HideBak"), TRUE)!=0;
	m_nVerSys = theApp.GetProfileInt(_T("Settings"), _T("VersionSystem"), 0);
	m_strVssProject = theApp.GetProfileString(_T("Settings"), _T("VssProject"), _T(""));
	m_strVssUser = theApp.GetProfileString(_T("Settings"), _T("VssUser"), _T(""));
	m_strVssPassword = theApp.GetProfileString(_T("Settings"), _T("VssPassword"), _T(""));
	m_strVssPath = theApp.GetProfileString(_T("Settings"), _T("VssPath"), _T(""));
	m_nTabSize = theApp.GetProfileInt(_T("Settings"), _T("TabSize"), 4);
	m_bIgnoreRegExp = theApp.GetProfileInt(_T("Settings"), _T("IgnoreRegExp"), FALSE);
	m_sPattern = theApp.GetProfileString(_T("Settings"), _T("RegExps"), NULL);

	if (m_strVssPath.IsEmpty())
	{
		CRegKeyEx reg;
		if (reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\SourceSafe")) == ERROR_SUCCESS)
		{
			TCHAR temp[MAX_PATH],path[MAX_PATH];
			reg.ReadChars(_T("SCCServerPath"), temp, MAX_PATH, _T(""));
			split_filename(temp, path, NULL, NULL);
			m_strVssPath.Format(_T("%s\\Ss.exe"), path);
		}
	}
}

CMainFrame::~CMainFrame()
{
	// destroy the reg expression list
	FreeRegExpList();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	mf = this;
	ignore_space_change_flag = (m_nIgnoreWhitespace==1);
	ignore_all_space_flag = (m_nIgnoreWhitespace==2);
	length_varies = (m_nIgnoreWhitespace!=0);
	ignore_case_flag = m_bIgnoreCase;
	ignore_blank_lines_flag = m_bIgnoreBlankLines;
	ignore_eol_diff = !m_bEolSensitive;
	ignore_some_changes = (m_nIgnoreWhitespace!=0) || m_bIgnoreCase || m_bIgnoreBlankLines || !m_bEolSensitive;
	// build the initial reg expression list
	RebuildRegExpList();

	heuristic = 1;
	output_style = OUTPUT_NORMAL;
    context = -1;
    line_end_char = '\n';
	//ignore_blank_lines_flag = 1;
	GetFontProperties();
	
	if (!m_wndToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_GRIPPER|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetPaneInfo(1, ID_DIFFNUM, 0, 150); 
	m_wndStatusBar.SetPaneInfo(2, ID_DIFFSTATUS, 0, 200); 

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this);

	return 0;
}


HMENU CMainFrame::NewDefaultMenu()
{
	m_default.LoadMenu(IDR_MAINFRAME);
	m_default.LoadToolbar(IDR_MAINFRAME);
	return(m_default.Detach());
}



BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnFileOpen() 
{
	DoFileOpen();
}


void CMainFrame::ShowMergeDoc(LPCTSTR szLeft, LPCTSTR szRight)
{
	BOOL docNull = (m_pMergeDoc == NULL);
	if (docNull)
		m_pMergeDoc = (CMergeDoc*)theApp.m_pDiffTemplate->OpenDocumentFile(NULL);
	else if (m_pLeft)
		m_pLeft->SendMessage(WM_COMMAND, ID_FILE_SAVE);

	if (m_pMergeDoc != NULL)
	{
		m_pMergeDoc->m_strLeftFile = szLeft;
		m_pMergeDoc->m_strRightFile = szRight;
		m_pMergeDoc->m_ltBuf.FreeAll();
		m_pMergeDoc->m_rtBuf.FreeAll();
		m_pMergeDoc->m_ltBuf.SetEolSensitivity(m_bEolSensitive);
		m_pMergeDoc->m_rtBuf.SetEolSensitivity(m_bEolSensitive);
		m_pMergeDoc->m_ltBuf.LoadFromFile(szLeft);
		m_pMergeDoc->m_rtBuf.LoadFromFile(szRight);
		
		if (m_pMergeDoc->Rescan())
		{
			if (docNull)
			{
				CWnd* pWnd = m_pMergeDoc->m_pView->GetParent();
				MDIActivate(pWnd);
			}
			else
				MDINext();
			
			// scroll to first diff
			if(m_bScrollToFirst && m_pMergeDoc->m_diffs.GetSize()!=0)
			{
				m_pLeft->SelectDiff(0, TRUE, FALSE);
			}

			// set the document types
			TCHAR name[MAX_PATH],ext[MAX_PATH];
			split_filename(szLeft, NULL, name, ext);
			m_pLeft->SetTextType(ext);
			split_filename(szRight, NULL, name, ext);
			m_pRight->SetTextType(ext);
			
			// SetTextType will revert to language dependent defaults for tab
			m_pLeft->SetTabSize(mf->m_nTabSize);
			m_pRight->SetTabSize(mf->m_nTabSize);
			
			// set the frame window header
			CChildFrame *pf = static_cast<CChildFrame *>(m_pMergeDoc->m_pView->GetParentFrame());
			if (pf != NULL)
			{
				pf->SetHeaderText(0, szLeft);
				pf->SetHeaderText(1, szRight);
			}

		}
		else
		{
			m_pMergeDoc->m_pView->GetParentFrame()->DestroyWindow();
			m_pMergeDoc=NULL;
			m_pLeft = m_pRight = NULL;
		}
	}
}



void CMainFrame::OnOptionsShowDifferent() 
{
	m_bShowDiff = !m_bShowDiff;
	theApp.WriteProfileInt(_T("Settings"), _T("ShowDifferent"), m_bShowDiff);
	if (m_pDirDoc != NULL)
		m_pDirDoc->Redisplay();
}

void CMainFrame::OnOptionsShowIdentical() 
{
	m_bShowIdent = !m_bShowIdent;
	theApp.WriteProfileInt(_T("Settings"), _T("ShowIdentical"), m_bShowIdent);
	if (m_pDirDoc != NULL)
		m_pDirDoc->Redisplay();
}

void CMainFrame::OnOptionsShowUniqueLeft() 
{
	m_bShowUniqueLeft = !m_bShowUniqueLeft;
	theApp.WriteProfileInt(_T("Settings"), _T("ShowUniqueLeft"), m_bShowUniqueLeft);
	if (m_pDirDoc != NULL)
		m_pDirDoc->Redisplay();
}

void CMainFrame::OnOptionsShowUniqueRight() 
{
	m_bShowUniqueRight = !m_bShowUniqueRight;
	theApp.WriteProfileInt(_T("Settings"), _T("ShowUniqueRight"), m_bShowUniqueRight);
	if (m_pDirDoc != NULL)
		m_pDirDoc->Redisplay();
}

void CMainFrame::OnUpdateOptionsShowdifferent(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowDiff);
}

void CMainFrame::OnUpdateOptionsShowidentical(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowIdent);
}

void CMainFrame::OnUpdateOptionsShowuniqueleft(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowUniqueLeft);
}

void CMainFrame::OnUpdateOptionsShowuniqueright(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowUniqueRight);
}



void CMainFrame::OnHideBackupFiles() 
{
	m_bHideBak = ! m_bHideBak;
	theApp.WriteProfileInt(_T("Settings"), _T("HideBak"), m_bHideBak);
	if (m_pDirDoc != NULL)
		m_pDirDoc->Redisplay();
}

void CMainFrame::OnUpdateHideBackupFiles(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bHideBak);
}

void CMainFrame::OnHelpGnulicense() 
{
	TCHAR path[MAX_PATH], temp[MAX_PATH];
	GetModuleFileName(NULL, temp, MAX_PATH);
	split_filename(temp, path, NULL, NULL);
	_tcscat(path, _T("\\Copying"));

	CFileStatus status;
	if (CFile::GetStatus(path, status))
		ShellExecute(m_hWnd, _T("open"), _T("notepad.exe"),path, NULL, SW_SHOWNORMAL);
	else
		ShellExecute(NULL, _T("open"), _T("http://www.gnu.org/copyleft/gpl.html"), NULL, NULL, SW_SHOWNORMAL);
}





BOOL CMainFrame::CheckSavePath(CString& strSavePath)
{
	BOOL needCheck;
	CFileStatus status;
	CString s;

	// check if file is writeable
	do
	{
		needCheck=FALSE;
		if (CFile::GetStatus(strSavePath, status))
		{
			if (status.m_attribute & CFile::Attribute::readOnly)
			{
				int userChoice = IDCANCEL;

				switch(m_nVerSys)
				{
				case 0:	//no versioning system
					// prompt for user choice
					AfxFormatString1(s, IDS_SAVEREADONLY_FMT, strSavePath);
					if (AfxMessageBox(s, MB_YESNO|MB_ICONQUESTION) == IDYES)
					{
						userChoice = IDSAVEAS;
					}
					break;
				case 1:	// Visual Source Safe
				{
						// prompt for user choice
					CVssPrompt dlg;
						dlg.m_strMessage.FormatMessage(IDS_SAVE_FMT, strSavePath);
					dlg.m_strProject = m_strVssProject;
					dlg.m_strUser = m_strVssUser;          // BSP - Add VSS user name to dialog box
					dlg.m_strPassword = m_strVssPassword;
						userChoice = dlg.DoModal();
						// process versioning system specific action
						if(userChoice==IDOK)
						{
							CWaitCursor wait;
							m_strVssProject = dlg.m_strProject;
							theApp.WriteProfileString(_T("Settings"), _T("VssProject"), mf->m_strVssProject);
							TCHAR args[1024];
							TCHAR path[MAX_PATH],name[MAX_PATH];
							split_filename(strSavePath,path,name,NULL);
							_chdrive(toupper(path[0])-'A'+1);
							_chdir(path);
							DWORD code;
							_stprintf(args,_T("checkout %s/%s"), m_strVssProject,name);
							HANDLE hVss = RunIt(m_strVssPath, args, TRUE, FALSE);
							if (hVss!=INVALID_HANDLE_VALUE)
							{
								WaitForSingleObject(hVss, INFINITE);
								GetExitCodeProcess(hVss, &code);
								CloseHandle(hVss);
								if (code != 0)
								{
									AfxMessageBox(IDS_VSSERROR, MB_ICONSTOP);
									return FALSE;
								}
								needCheck=FALSE;
							}
							else
							{
								AfxMessageBox(IDS_VSS_RUN_ERROR, MB_ICONSTOP);
								return FALSE;
							}
						}
					}
					break;
				case 2: // CVisual SourceSafe 5.0+ (COM)
							{
					// prompt for user choice
					CVssPrompt dlg;
					dlg.m_strMessage.FormatMessage(IDS_SAVE_FMT, strSavePath);
					dlg.m_strProject = m_strVssProject;
					dlg.m_strUser = m_strVssUser;          // BSP - Add VSS user name to dialog box
					dlg.m_strPassword = m_strVssPassword;
					userChoice = dlg.DoModal();
					// process versioning system specific action
					if(userChoice==IDOK)
					{
						CWaitCursor wait;
						m_strVssProject = dlg.m_strProject;
						m_strVssUser = dlg.m_strUser;
						m_strVssPassword = dlg.m_strPassword;

						theApp.WriteProfileString(_T("Settings"), _T("VssProject"), m_strVssProject);
						theApp.WriteProfileString(_T("Settings"), _T("VssUser"), m_strVssUser);
						theApp.WriteProfileString(_T("Settings"), _T("VssPassword"), m_strVssPassword);


						IVSSDatabase	vssdb;
						IVSSItems		m_vssis;
						IVSSItem		m_vssi;


                        COleException *eOleException = new COleException;
							
						// BSP - Create the COM interface pointer to VSS
						if (FAILED(vssdb.CreateDispatch("SourceSafe", eOleException)))
						{
							throw eOleException;	// catch block deletes.
						}
						else
						{
							eOleException->Delete();
						}

                        // BSP - Open the specific VSS data file  using info from VSS dialog box
						vssdb.Open(m_strVssPath, m_strVssUser, m_strVssPassword);
						
						TCHAR path[MAX_PATH],name[MAX_PATH];
						split_filename(strSavePath,path,name,NULL);

                        // BSP - Combine the project entered on the dialog box with the file name...
						CString strItem = m_strVssProject+"/"+name; 

                        //  BSP - ...to get the specific source safe item to be checked out
						m_vssi = vssdb.GetVSSItem( strItem, 0 );

                        // BSP - Get the working directory where VSS will put the file...
						CString strLocalSpec = m_vssi.GetLocalSpec();

                        // BSP - ...and compare it to the directory WinMerge is using.
						if (strLocalSpec.CompareNoCase(strSavePath))
						{
						   // BSP - if the directories are different, let the user confirm the CheckOut
							int iRes = AfxMessageBox("The VSS Working Folder and the location of the current file do not match.  Continue?", MB_YESNO);

							if (iRes != IDYES)
								return FALSE;   // BSP - if not Yes, bail.
						}

                        // BSP - Finally! Check out the file!
						m_vssi.Checkout("", strSavePath, 0);

					}
				}
				break;
				case 3:	// ClearCase
					{
						// prompt for user choice
						CCCPrompt dlg;
						userChoice = dlg.DoModal();
						// process versioning system specific action
						if(userChoice == IDOK)
						{
							CWaitCursor wait;
							TCHAR args[1024];
							TCHAR path[MAX_PATH],name[MAX_PATH];
							split_filename(strSavePath,path,name,NULL);
							_chdrive(toupper(path[0])-'A'+1);
							_chdir(path);
							DWORD code;
							_stprintf(args,_T("checkout -c \"%s\" %s"), dlg.m_comments, name);
							HANDLE hVss = RunIt(m_strVssPath, args, TRUE, FALSE);
							if (hVss!=INVALID_HANDLE_VALUE)
							{
								WaitForSingleObject(hVss, INFINITE);
								GetExitCodeProcess(hVss, &code);
								CloseHandle(hVss);
								
								if (code != 0)
								{
									AfxMessageBox(IDS_VSSERROR, MB_ICONSTOP);
									return FALSE;
								}
								needCheck=FALSE;
							}
							else
							{
								AfxMessageBox(IDS_VSS_RUN_ERROR, MB_ICONSTOP);
								return FALSE;
							}
						}
						}
						break;
				}	//switch(m_nVerSys)

				// common processing for all version systems
				switch(userChoice)
				{
					case IDCANCEL:
						return FALSE;
					case IDSAVEAS:
						CString title;
						VERIFY(title.LoadString(IDS_SAVE_AS_TITLE));
						if (SelectFile(s, strSavePath, title, NULL, FALSE))
						{
							strSavePath = s;
							needCheck=TRUE;
						}
						else
							return FALSE;
						break;
					}
				}
			}
	} while (needCheck);
	return TRUE;
}

void CMainFrame::OnProperties() 
{
	CPropertySheet sht(IDS_PROPERTIES_TITLE);
	CPropVss vss;
	CPropGeneral gen;
	CPropSyntax syn;
	CPropFilter filter;
	CPropColors colors( theApp.GetDiffColor(), theApp.GetSelDiffColor() );
	sht.AddPage(&gen);
	sht.AddPage(&syn);
	sht.AddPage(&filter);
	sht.AddPage(&vss);
	sht.AddPage(&colors);
	
	vss.m_nVerSys = m_nVerSys;
	vss.m_strPath = m_strVssPath;

	gen.m_bBackup = m_bBackup;
	gen.m_nIgnoreWhite = m_nIgnoreWhitespace;
	gen.m_bIgnoreCase = m_bIgnoreCase;
	gen.m_bIgnoreBlankLines = m_bIgnoreBlankLines;
	gen.m_bEolSensitive = m_bEolSensitive;
	gen.m_bScroll = m_bScrollToFirst;
	gen.m_nTabSize = m_nTabSize;
	gen.m_bDisableSplash = theApp.m_bDisableSplash;

	syn.m_bHiliteSyntax = theApp.m_bHiliteSyntax;
	filter.m_bIgnoreRegExp = m_bIgnoreRegExp;
	filter.m_sPattern = m_sPattern;
	
	if (sht.DoModal()==IDOK)
	{
		m_nVerSys = vss.m_nVerSys;
		m_strVssPath = vss.m_strPath;
		
		m_bBackup = gen.m_bBackup;
		m_bScrollToFirst = gen.m_bScroll;
		m_nTabSize = gen.m_nTabSize;
		theApp.m_bDisableSplash = gen.m_bDisableSplash;

		m_nIgnoreWhitespace = gen.m_nIgnoreWhite;
		ignore_all_space_flag = (m_nIgnoreWhitespace==2);
		ignore_space_change_flag = (m_nIgnoreWhitespace==1);
		ignore_blank_lines_flag = m_bIgnoreBlankLines = gen.m_bIgnoreBlankLines;
		m_bEolSensitive = gen.m_bEolSensitive;
		ignore_eol_diff = !m_bEolSensitive;
		ignore_case_flag = m_bIgnoreCase = gen.m_bIgnoreCase;
		ignore_some_changes = (m_nIgnoreWhitespace!=0) || m_bIgnoreCase || m_bIgnoreBlankLines || !m_bEolSensitive;
		length_varies = (m_nIgnoreWhitespace!=0);
		
		m_bIgnoreRegExp = filter.m_bIgnoreRegExp;
		m_sPattern = filter.m_sPattern;

		theApp.SetDiffColor( colors.m_clrDiff );
		theApp.SetSelDiffColor( colors.m_clrSelDiff );

		theApp.WriteProfileInt(_T("Settings"), _T("VersionSystem"), m_nVerSys);
		theApp.WriteProfileInt(_T("Settings"), _T("IgnoreSpace"), m_nIgnoreWhitespace);
		theApp.WriteProfileInt(_T("Settings"), _T("ScrollToFirst"), m_bScrollToFirst);
		theApp.WriteProfileInt(_T("Settings"), _T("BackupFile"), m_bBackup);
		theApp.WriteProfileString(_T("Settings"), _T("VssPath"), m_strVssPath);
		theApp.WriteProfileInt(_T("Settings"), _T("TabSize"), m_nTabSize);
		theApp.WriteProfileInt(_T("Settings"), _T("IgnoreBlankLines"), m_bIgnoreBlankLines);
		theApp.WriteProfileInt(_T("Settings"), _T("IgnoreCase"), m_bIgnoreCase);
		theApp.WriteProfileInt(_T("Settings"), _T("IgnoreRegExp"), m_bIgnoreRegExp);
		theApp.WriteProfileString(_T("Settings"), _T("RegExps"), m_sPattern);
		theApp.WriteProfileInt(_T("Settings"), _T("DisableSplash"), theApp.m_bDisableSplash);

		theApp.m_bHiliteSyntax = syn.m_bHiliteSyntax;
		theApp.WriteProfileInt(_T("Settings"), _T("HiliteSyntax"), theApp.m_bHiliteSyntax);

		RebuildRegExpList();

		// make an attempt at rescanning any open diff sessions
		if (m_pLeft != NULL && m_pRight != NULL)
		{
			if (m_pMergeDoc->SaveHelper())
			{
				m_pLeft->GetDocument()->Rescan();
			}
			// mods have been made, so just warn
			else
			{
				AfxMessageBox(IDS_DIFF_OPEN_NO_SET_PROPS,MB_ICONEXCLAMATION);
			}
		}
	}
}

// callback for progress during diff
class MainFrmStatus : public IDiffStatus
{
public:
	MainFrmStatus(CMainFrame * pFrame) : m_pFrame(pFrame) { m_pFrame->clearStatus(); }
	virtual void rptFile(BYTE code) { m_pFrame->rptStatus(code); }
private:
	CMainFrame * m_pFrame;
};

// clear counters used to track diff progress
void CMainFrame::clearStatus()
{
	m_nStatusFileSame = m_nStatusFileDiff = m_nStatusFileBinDiff = m_nStatusFileError
		 = m_nStatusLeftFileOnly = m_nStatusLeftDirOnly = m_nStatusRightFileOnly = m_nStatusRightDirOnly
		 = 0;
}

// diff completed another file
void CMainFrame::rptStatus(BYTE code)
{
	switch(code)
	{
	case FILE_SAME:
		++m_nStatusFileSame;
		break;
	case FILE_DIFF:
		++m_nStatusFileDiff;
		break;
	case FILE_BINDIFF:
		++m_nStatusFileBinDiff;
		break;
	case FILE_ERROR:
		++m_nStatusFileError;
		break;
	case FILE_LUNIQUE:
		++m_nStatusLeftFileOnly;
		break;
	case FILE_LDIRUNIQUE:
		++m_nStatusLeftDirOnly;
		break;
	case FILE_RUNIQUE:
		++m_nStatusRightFileOnly;
		break;
	case FILE_RDIRUNIQUE:
		++m_nStatusRightDirOnly;
		break;
	}
	CString s;
	// TODO: Load the format string from resource
	s.Format(_T("s:%d d:%d bd:%d lf:%d ld:%d rf:%d rd:%d e:%d")
		, m_nStatusFileSame, m_nStatusFileDiff, m_nStatusFileBinDiff
		, m_nStatusLeftFileOnly, m_nStatusLeftDirOnly, m_nStatusRightFileOnly, m_nStatusRightDirOnly
		, m_nStatusFileError);
	m_wndStatusBar.SetPaneText(2, s);

}

BOOL CMainFrame::DoFileOpen(LPCTSTR pszLeft /*=NULL*/, LPCTSTR pszRight /*=NULL*/, BOOL bRecurse /*= FALSE*/)
{
	CString strLeft(pszLeft);
	CString strRight(pszRight);
	CString strExt;
	CFileStatus status;

	// if files weren't specified, popup dialog
	if ((pszLeft == NULL || !CFile::GetStatus(pszLeft, status))
		|| (pszRight == NULL || !CFile::GetStatus(pszRight, status)))
		
	{
		COpenDlg dlg;
		dlg.m_strLeft = strLeft;
		dlg.m_strRight = strRight;
		dlg.m_bRecurse = bRecurse;
		if (dlg.DoModal() != IDOK)
			return FALSE;

		strLeft = dlg.m_strLeft;
		strRight = dlg.m_strRight;
		bRecurse = dlg.m_bRecurse;
		strExt = dlg.m_strParsedExt;
	}
	else
	{
		//save the MRU left and right files.
		addToMru(pszLeft, _T("Files\\Left"));
		addToMru(pszRight, _T("Files\\Right"));
	}


	// check to make sure they are same type
	TCHAR name[MAX_PATH];
	BOOL bLeftIsDir = GetFileAttributes(strLeft)&FILE_ATTRIBUTE_DIRECTORY;
	BOOL bRightIsDir = GetFileAttributes(strRight)&FILE_ATTRIBUTE_DIRECTORY;
	if (bLeftIsDir && !bRightIsDir)
	{
		split_filename(strRight, NULL, name, NULL);
		strLeft += _T("\\");
		strLeft += name;
		bLeftIsDir = FALSE;
	}
	else if (!bLeftIsDir && bRightIsDir)
	{
		split_filename(strLeft, NULL, name, NULL);
		strRight += _T("\\");
		strRight += name;
		bRightIsDir = FALSE;
	}

	if (gWriteLog)
	{
		gLog.Write(_T("### Begin Comparison Parameters #############################\r\n")
				  _T("\tLeft: %s\r\n")
				   _T("\tRight: %s\r\n")
				  _T("\tRecurse: %d\r\n")
				  _T("\tShowUniqueLeft: %d\r\n")
				  _T("\tShowUniqueRight: %d\r\n")
				  _T("\tShowIdentical: %d\r\n")
				  _T("\tShowDiff: %d\r\n")
				  _T("\tHideBak: %d\r\n")
				  _T("\tVerSys: %d\r\n")
				  _T("\tVssPath: %s\r\n")
				  _T("\tBackups: %d\r\n")
				  _T("\tIgnoreWS: %d\r\n")
				  _T("\tScrollToFirst: %d\r\n")
				  _T("### End Comparison Parameters #############################\r\n"),
				  strLeft,
				  strRight,
				  bRecurse,
				  m_bShowUniqueLeft,
				  m_bShowUniqueRight,
				  m_bShowIdent,
				  m_bShowDiff,
				  m_bHideBak,
				  m_nVerSys,
				  m_strVssPath,
				  m_bBackup,
				  m_nIgnoreWhitespace,
				  m_bScrollToFirst);
	}

	// open the diff
	if (bLeftIsDir)
	{
		recursive = bRecurse;
		if (m_pDirDoc == NULL)
			m_pDirDoc = (CDirDoc*)theApp.m_pDirTemplate->OpenDocumentFile(NULL);
		if (m_pDirDoc != NULL)
		{
			MainFrmStatus mfst(this);
			CDiffContext *pCtxt = new CDiffContext(strLeft, strRight, &mfst);
			if (pCtxt != NULL)
			{
				m_pDirDoc->SetDiffContext(pCtxt);
				pCtxt->SetRegExp(strExt);
				m_pDirDoc->Rescan();
			}
			pCtxt->ClearStatus();
		}
	}
	else
	{
		recursive = FALSE;
		ShowMergeDoc(strLeft, strRight);
	}
	return TRUE;
}

BOOL CMainFrame::CreateBackup(LPCTSTR pszPath)
{
	// first, make a backup copy of the original
	CFileStatus status;

	// create backup copy of file if destination file exists
	if (m_bBackup
		&& CFile::GetStatus(pszPath, status))
	{
		TCHAR path[MAX_PATH], name[_MAX_FNAME], ext[_MAX_EXT];
		CString s;

		// build the backup filename
		split_filename(pszPath, path, name, ext);
		if (*ext != _T('\0'))
			s.Format(_T("%s\\%s.%s") BACKUP_FILE_EXT, path, name, ext);
		else
			s.Format(_T("%s\\%s")  BACKUP_FILE_EXT, path, name);

		// get rid of the dest file
		DeleteFile(s); // (errors are handled from MoveFile below)

		// move the sucker
		if (!MoveFile(pszPath, s)
			&& AfxMessageBox(IDS_BACKUP_FAILED_PROMPT, MB_YESNO|MB_ICONQUESTION) != IDYES)
		{
			return FALSE;
		}

		return TRUE;
	}

	// we got here because we're either not backing up of there was nothing to backup
	return TRUE;
}

// Get user language description of error, if available
CString GetSystemErrorDesc(int nerr)
{
	LPVOID lpMsgBuf;
	CString str = _T("?");
	if (FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		nerr,
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		))
	{
		str = (LPCTSTR)lpMsgBuf;
	}
	// Free the buffer.
	LocalFree( lpMsgBuf );
	return str;
}

// trim trailing line returns
static void RemoveLineReturns(CString & str)
{
	str.Replace(_T("\n"), _T(""));
	str.Replace(_T("\r"), _T(""));
}

// TODO: Can we move this into DirActions.cpp ?
// Delete file (return TRUE if deleted, else put up error & return FALSE)
BOOL CMainFrame::DeleteFileOrError(LPCTSTR szFile)
{
	if (!DeleteFile(szFile))
	{
		CString sError = GetSystemErrorDesc(GetLastError());
		RemoveLineReturns(sError);
		sError += (CString)_T(" [") + szFile + _T("]");
		CString s;
		AfxFormatString1(s, IDS_DELETE_FILE_FAILED, sError);
		AfxMessageBox(s, MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}

// Delete file (return TRUE if successful, else sets error string & returns FALSE)
BOOL DeleteFileSilently(LPCTSTR szFile, CString * psError)
{
	if (!DeleteFile(szFile))
	{
		CString sError = GetSystemErrorDesc(GetLastError());
		RemoveLineReturns(sError);
		AfxFormatString2(*psError, IDS_DELETE_FILE_FAILED, szFile, sError);
		return FALSE;
	}
	return TRUE;
}


// delete directory by recursively deleting all contents
// gives up on first error
BOOL DeleteDirSilently(LPCTSTR szDir, CString * psError)
{
	CFileFind finder;
	CString sSpec = szDir;
	sSpec += _T("\\*.*");
	if (finder.FindFile(sSpec))
	{
		BOOL done=FALSE;
		while (!done)
		{
			done = !finder.FindNextFile();
			if (finder.IsDots()) continue;
			if (finder.IsDirectory())
			{
				if (!DeleteDirSilently(finder.GetFilePath(), psError))
					return FALSE;
			}
			else
			{
				if (!DeleteFileSilently(finder.GetFilePath(), psError))
					return FALSE;
			}
		}
	}
	finder.Close(); // must close the handle or RemoveDirectory will fail
	if (!RemoveDirectory(szDir))
	{
		CString sError = GetSystemErrorDesc(GetLastError());
		RemoveLineReturns(sError);
		AfxFormatString2(*psError, IDS_DELETE_FILE_FAILED, szDir, sError);
		return FALSE;
	}
	return TRUE;
}

// wrapper for DoSyncFiles which adds filename to error string reported
BOOL CMainFrame::SyncFiles(LPCTSTR pszSrc, LPCTSTR pszDest, CString * psError)
{
	if (!DoSyncFiles(pszSrc, pszDest, psError))
	{
		AfxFormatString2(*psError, IDS_COPY_FILE_FAILED, *psError, pszSrc);
		return FALSE;
	}
	return TRUE;
}

// (error string reported does not include filename
BOOL CMainFrame::DoSyncFiles(LPCTSTR pszSrc, LPCTSTR pszDest, CString * psError)
{
	CString sActionError;
	CString strSavePath(pszDest);

	if (!CheckSavePath(strSavePath))
	{
		psError->LoadString(IDS_ERROR_FILE_WRITEABLE);
		return FALSE;
	}
	
	if (!CreateBackup(strSavePath))
	{
		psError->LoadString(IDS_ERROR_BACKUP);
		return FALSE;
	}
	
	// Now it's just a matter of copying the right file to the left
	DeleteFile(strSavePath); // (errors are handled from CopyFile below)
	if (!CopyFile(pszSrc, strSavePath, FALSE))
	{
		*psError = GetSystemErrorDesc(GetLastError());
		return FALSE;
	}
	
	// tell the dir view to update itself
	return TRUE;
}

void CMainFrame::UpdateCurrentFileStatus(UINT nStatus, int idx)
{
	ASSERT(m_pDirDoc);
	CDirView *pv = m_pDirDoc->GetMainView();
	ASSERT(pv);
	// first change it in the dirlist
	POSITION diffpos = pv->GetItemKey(idx);

	// TODO: Why is the update broken into these pieces ?
	// Someone could figure out these pieces and probably simplify this.

	// update DIFFITEM code
	m_pDirDoc->m_pCtxt->UpdateStatusCode(diffpos, (BYTE)nStatus);
	// update DIFFITEM time, and also tell views
	m_pDirDoc->UpdateItemStatus(idx);
	//m_pDirDoc->Redisplay();
}

void CMainFrame::OnViewSelectfont() 
{
	CHOOSEFONT cf;
	memset(&cf, 0, sizeof(CHOOSEFONT));
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.Flags = CF_INITTOLOGFONTSTRUCT|CF_FORCEFONTEXIST|CF_SCREENFONTS|CF_FIXEDPITCHONLY;
	cf.lpLogFont = &m_lfDiff;
	if (ChooseFont(&cf))
	{
		m_bFontSpecified = TRUE;
		theApp.WriteProfileInt(_T("Font"), _T("Specified"), m_bFontSpecified);
		theApp.WriteProfileInt(_T("Font"), _T("Height"), m_lfDiff.lfHeight);
		theApp.WriteProfileInt(_T("Font"), _T("Width"), m_lfDiff.lfWidth);
		theApp.WriteProfileInt(_T("Font"), _T("Escapement"), m_lfDiff.lfEscapement);
		theApp.WriteProfileInt(_T("Font"), _T("Orientation"), m_lfDiff.lfOrientation);
		theApp.WriteProfileInt(_T("Font"), _T("Weight"), m_lfDiff.lfWeight);
		theApp.WriteProfileInt(_T("Font"), _T("Italic"), m_lfDiff.lfItalic);
		theApp.WriteProfileInt(_T("Font"), _T("Underline"), m_lfDiff.lfUnderline);
		theApp.WriteProfileInt(_T("Font"), _T("StrikeOut"), m_lfDiff.lfStrikeOut);
		theApp.WriteProfileInt(_T("Font"), _T("CharSet"), m_lfDiff.lfCharSet);
		theApp.WriteProfileInt(_T("Font"), _T("OutPrecision"), m_lfDiff.lfOutPrecision);
		theApp.WriteProfileInt(_T("Font"), _T("ClipPrecision"), m_lfDiff.lfClipPrecision);
		theApp.WriteProfileInt(_T("Font"), _T("Quality"), m_lfDiff.lfQuality);
		theApp.WriteProfileInt(_T("Font"), _T("PitchAndFamily"), m_lfDiff.lfPitchAndFamily);
		theApp.WriteProfileString(_T("Font"), _T("FaceName"), m_lfDiff.lfFaceName);

		AfxMessageBox(IDS_FONT_CHANGE, MB_ICONINFORMATION);
	}
}

void CMainFrame::GetFontProperties()
{
	m_lfDiff.lfHeight = theApp.GetProfileInt(_T("Font"), _T("Height"), -16);
	m_lfDiff.lfWidth = theApp.GetProfileInt(_T("Font"), _T("Width"), 0);
	m_lfDiff.lfEscapement = theApp.GetProfileInt(_T("Font"), _T("Escapement"), 0);
	m_lfDiff.lfOrientation = theApp.GetProfileInt(_T("Font"), _T("Orientation"), 0);
	m_lfDiff.lfWeight = theApp.GetProfileInt(_T("Font"), _T("Weight"), FW_NORMAL);
	m_lfDiff.lfItalic = (BYTE)theApp.GetProfileInt(_T("Font"), _T("Italic"), FALSE);
	m_lfDiff.lfUnderline = (BYTE)theApp.GetProfileInt(_T("Font"), _T("Underline"), FALSE);
	m_lfDiff.lfStrikeOut = (BYTE)theApp.GetProfileInt(_T("Font"), _T("StrikeOut"), FALSE);
	m_lfDiff.lfCharSet = (BYTE)theApp.GetProfileInt(_T("Font"), _T("CharSet"), ANSI_CHARSET);
	m_lfDiff.lfOutPrecision = (BYTE)theApp.GetProfileInt(_T("Font"), _T("OutPrecision"), OUT_STRING_PRECIS);
	m_lfDiff.lfClipPrecision = (BYTE)theApp.GetProfileInt(_T("Font"), _T("ClipPrecision"), CLIP_STROKE_PRECIS);
	m_lfDiff.lfQuality = (BYTE)theApp.GetProfileInt(_T("Font"), _T("Quality"), DRAFT_QUALITY);
	m_lfDiff.lfPitchAndFamily = (BYTE)theApp.GetProfileInt(_T("Font"), _T("PitchAndFamily"), FF_MODERN | FIXED_PITCH);
	_tcscpy(m_lfDiff.lfFaceName, theApp.GetProfileString(_T("Font"), _T("FaceName"), _T("Courier")));
}

void CMainFrame::OnViewUsedefaultfont() 
{
	m_bFontSpecified=FALSE;
	theApp.WriteProfileInt(_T("Font"), _T("Specified"), m_bFontSpecified);
}

void CMainFrame::OnUpdateViewUsedefaultfont(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bFontSpecified);
}

void CMainFrame::UpdateResources()
{
	CString s;
	VERIFY(s.LoadString(AFX_IDS_IDLEMESSAGE));
	m_wndStatusBar.SetPaneText(0, s);

	if (m_pDirDoc != NULL)
		m_pDirDoc->UpdateResources();

	if (m_pLeft != NULL)
		m_pLeft->UpdateResources();

	if (m_pRight != NULL)
		m_pRight->UpdateResources();
}


void CMainFrame::OnHelpContents() 
{
	TCHAR path[MAX_PATH], temp[MAX_PATH];
	GetModuleFileName(NULL, temp, MAX_PATH);
	split_filename(temp, path, NULL, NULL);
	_tcscat(path, _T("\\Docs\\index.html"));

	CFileStatus status;
	if (CFile::GetStatus(path, status))
		ShellExecute(NULL, _T("open"), path, NULL, NULL, SW_SHOWNORMAL);
	else
		ShellExecute(NULL, _T("open"), _T("http://winmerge.sourceforge.net/docs/index.html"), NULL, NULL, SW_SHOWNORMAL);

}

void CMainFrame::OnUpdateHelpContents(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::ActivateFrame(int nCmdShow) 
{
	if (!m_bFirstTime)
	{
		CMDIFrameWnd::ActivateFrame(nCmdShow);
		return;
	}

	m_bFirstTime = FALSE;

	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wp);
	wp.rcNormalPosition.left=theApp.GetProfileInt(_T("Settings"), _T("MainLeft"),0);
	wp.rcNormalPosition.top=theApp.GetProfileInt(_T("Settings"), _T("MainTop"),0);
	wp.rcNormalPosition.right=theApp.GetProfileInt(_T("Settings"), _T("MainRight"),0);
	wp.rcNormalPosition.bottom=theApp.GetProfileInt(_T("Settings"), _T("MainBottom"),0);
	wp.showCmd = nCmdShow;

	CRect dsk_rc,rc(wp.rcNormalPosition);
	GetDesktopWindow()->GetWindowRect(&dsk_rc);
	if (theApp.GetProfileInt(_T("Settings"), _T("MainMax"), FALSE))
	{
		CMDIFrameWnd::ActivateFrame(SW_MAXIMIZE);	
	}
	else if (rc.Width() != 0
		&& rc.Height() != 0
		&& wp.rcNormalPosition.left >= dsk_rc.left  // only show in saved position if it fits on screen
		&& wp.rcNormalPosition.top >= dsk_rc.top
		&& wp.rcNormalPosition.right <= dsk_rc.right
		&& wp.rcNormalPosition.bottom <= dsk_rc.bottom)
	{
		SetWindowPlacement(&wp);
	}
	else
		CMDIFrameWnd::ActivateFrame(nCmdShow);
}

void CMainFrame::OnClose() 
{
	// check if we have a diff window open and need to save
	if ((m_pLeft && m_pLeft->IsModified())
		|| (m_pRight && m_pRight->IsModified()))
	{
		if ( (m_pLeft->IsModified()||m_pLeft->IsModified()) && !m_pMergeDoc->SaveHelper())
				return;
	}
	

	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wp);
	theApp.WriteProfileInt(_T("Settings"), _T("MainLeft"),wp.rcNormalPosition.left);
	theApp.WriteProfileInt(_T("Settings"), _T("MainTop"),wp.rcNormalPosition.top);
	theApp.WriteProfileInt(_T("Settings"), _T("MainRight"),wp.rcNormalPosition.right);
	theApp.WriteProfileInt(_T("Settings"), _T("MainBottom"),wp.rcNormalPosition.bottom);
	theApp.WriteProfileInt(_T("Settings"), _T("MainMax"), (wp.showCmd == SW_MAXIMIZE));
	if(m_pMergeDoc != NULL && m_pLeft)
		((CChildFrame*)m_pLeft->GetParentFrame())->SavePosition();
	CMDIFrameWnd::OnClose();
}


void CMainFrame::FreeRegExpList()
{
	struct regexp_list *r;
	r = ignore_regexp_list;
	// iterate through the list, free the reg expression
	// list item
	while (ignore_regexp_list)
	{
		r = r->next;
		free((ignore_regexp_list->buf).fastmap);
		free((ignore_regexp_list->buf).buffer);
		free(ignore_regexp_list);
		ignore_regexp_list = r;
	}

}


void CMainFrame::RebuildRegExpList()
{
	_TCHAR tmp[MAX_PATH];
	_TCHAR* token;
	_TCHAR sep[] = "\r\n";
	
	// destroy the old list if the it is not NULL
	FreeRegExpList();

	// build the new list if the user choose to
	// ignore lines matching the reg expression patterns
	if (m_bIgnoreRegExp)
	{
		// find each regular expression and add to list
		_tcscpy(tmp, m_sPattern);

		token = _tcstok(tmp, sep);
		while (token)
		{
			add_regexp(&ignore_regexp_list, token);
			token = _tcstok(NULL, sep);
		}
	}

	if (ignore_regexp_list)
	{
		ignore_some_changes = 1;
	}

}

// Add the compiled form of regexp pattern to reglist
static void
add_regexp(struct regexp_list **reglist,
     char const* pattern)
{
  struct regexp_list *r;
  char const *m;

  r = (struct regexp_list *) xmalloc (sizeof (*r));
  bzero (r, sizeof (*r));
  r->buf.fastmap = (char*) xmalloc (256);
  m = re_compile_pattern (pattern, strlen (pattern), &r->buf);
  if (m != 0)
    error ("%s: %s", pattern, m);

  /* Add to the start of the list, since it's easier than the end.  */
  r->next = *reglist;
  *reglist = r;
}

// utility function to update CSuperComboBox format MRU
void CMainFrame::addToMru(LPCSTR szItem, LPCSTR szRegSubKey, UINT nMaxItems)
{
	CString s,s2;
	UINT cnt = AfxGetApp()->GetProfileInt(szRegSubKey, "Count", 0);
	++cnt;	// add new string
	if(cnt>nMaxItems)
	{
		cnt=nMaxItems;
	}
	// move items down a step
	for (UINT i=cnt ; i!=0; --i)
	{
		s2.Format("Item_%d", i-1);
		s = AfxGetApp()->GetProfileString(szRegSubKey, s2);
		s2.Format("Item_%d", i);
		AfxGetApp()->WriteProfileString(szRegSubKey, s2, s);
	}
	// add most recent item
	AfxGetApp()->WriteProfileString(szRegSubKey, "Item_0", szItem);
	// update count
	AfxGetApp()->WriteProfileInt(szRegSubKey, "Count", cnt);
}
