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
#include "DiffView.h"
#include "DirView.h"
#include "DirDoc.h"
#include "OpenDlg.h"

#include "diff.h"
#include "getopt.h"
#include "fnmatch.h"
#include "coretools.h"
#include "Splash.h"
#include "VssPrompt.h"
#include "CCPrompt.h"
#include "PropVss.h"
#include "PropGeneral.h"
#include "RegKey.h"
#include "logfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int recursive;
CMainFrame *mf = NULL;
extern CLogFile gLog;
extern bool gWriteLog;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_COMMAND(ID_OPTIONS_SHOWDIFFERENT, OnOptionsShowDifferent)
	ON_COMMAND(ID_OPTIONS_SHOWIDENTICAL, OnOptionsShowIdentical)
	ON_COMMAND(ID_OPTIONS_SHOWUNIQUE, OnOptionsShowUnique)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWDIFFERENT, OnUpdateOptionsShowdifferent)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWIDENTICAL, OnUpdateOptionsShowidentical)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWUNIQUE, OnUpdateOptionsShowunique)
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
	m_bIgnoreCase = theApp.GetProfileInt(_T("Settings"), _T("IgnoreCase"), FALSE)!=0;
	m_bShowUnique = theApp.GetProfileInt(_T("Settings"), _T("ShowUnique"), TRUE)!=0;
	m_bShowDiff = theApp.GetProfileInt(_T("Settings"), _T("ShowDifferent"), TRUE)!=0;
	m_bShowIdent = theApp.GetProfileInt(_T("Settings"), _T("ShowIdentical"), TRUE)!=0;
	m_bBackup = theApp.GetProfileInt(_T("Settings"), _T("BackupFile"), TRUE)!=0;
	m_bScrollToFirst = theApp.GetProfileInt(_T("Settings"), _T("ScrollToFirst"), FALSE)!=0;
	m_bIgnoreWhitespace = theApp.GetProfileInt(_T("Settings"), _T("IgnoreSpace"), TRUE)!=0;
	m_bHideBak = theApp.GetProfileInt(_T("Settings"), _T("HideBak"), TRUE)!=0;
	m_nVerSys = theApp.GetProfileInt(_T("Settings"), _T("VersionSystem"), 0);
	m_strVssProject = theApp.GetProfileString(_T("Settings"), _T("VssProject"), _T(""));
	m_strVssPath = theApp.GetProfileString(_T("Settings"), _T("VssPath"), _T(""));
	m_nTabSize = theApp.GetProfileInt(_T("Settings"), _T("TabSize"), 4);
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
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	mf = this;
	ignore_all_space_flag = m_bIgnoreWhitespace;
	length_varies = m_bIgnoreWhitespace;
	ignore_case_flag = m_bIgnoreCase;
	ignore_blank_lines_flag = m_bIgnoreBlankLines;
	ignore_some_changes = m_bIgnoreWhitespace || m_bIgnoreCase || m_bIgnoreBlankLines;

	heuristic = 1;
	output_style = OUTPUT_NORMAL;
    context = -1;
    line_end_char = '\n';
	//ignore_blank_lines_flag = 1;
	GetFontProperties();
	
	if (!m_wndToolBar.Create(this) ||
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
	else if (mf->m_pLeft)
		mf->m_pLeft->SendMessage(WM_COMMAND, ID_FILE_SAVE);

	if (m_pMergeDoc != NULL)
	{
		m_pMergeDoc->m_strLeftFile = szLeft;
		m_pMergeDoc->m_strRightFile = szRight;
		if (m_pMergeDoc->Rescan())
		{
			if (docNull)
				MDIActivate(m_pMergeDoc->m_pView->GetParent());
			else
				MDINext();
		}
		else
		{
			m_pMergeDoc->m_pView->GetParentFrame()->DestroyWindow();
			m_pMergeDoc=NULL;
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

void CMainFrame::OnOptionsShowUnique() 
{
	m_bShowUnique = !m_bShowUnique;
	theApp.WriteProfileInt(_T("Settings"), _T("ShowUnique"), m_bShowUnique);
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

void CMainFrame::OnUpdateOptionsShowunique(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowUnique);
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
	ShellExecute(m_hWnd, _T("open"), _T("notepad.exe"),_T("Copying"), NULL, SW_SHOWNORMAL);
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
				case 2:	// ClearCase
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
	sht.AddPage(&gen);
	sht.AddPage(&vss);
	
	vss.m_nVerSys = m_nVerSys;
	vss.m_strPath = m_strVssPath;

	gen.m_bBackup = m_bBackup;
	gen.m_bIgnoreWhite = m_bIgnoreWhitespace;
	gen.m_bIgnoreCase = m_bIgnoreCase;
	gen.m_bIgnoreBlankLines = m_bIgnoreBlankLines;
	gen.m_bScroll = m_bScrollToFirst;
	gen.m_nTabSize = m_nTabSize;
	
	if (sht.DoModal()==IDOK)
	{
		m_nVerSys = vss.m_nVerSys;
		m_strVssPath = vss.m_strPath;
		
		m_bBackup = gen.m_bBackup;
		m_bScrollToFirst = gen.m_bScroll;
		m_nTabSize = gen.m_nTabSize;

		ignore_all_space_flag = m_bIgnoreWhitespace = gen.m_bIgnoreWhite;
		ignore_blank_lines_flag = m_bIgnoreBlankLines = gen.m_bIgnoreBlankLines;
		ignore_case_flag = m_bIgnoreCase = gen.m_bIgnoreCase;
		ignore_some_changes = m_bIgnoreWhitespace || m_bIgnoreCase || m_bIgnoreBlankLines;
		length_varies = m_bIgnoreWhitespace;

		theApp.WriteProfileInt(_T("Settings"), _T("VersionSystem"), m_nVerSys);
		theApp.WriteProfileInt(_T("Settings"), _T("IgnoreSpace"), m_bIgnoreWhitespace);
		theApp.WriteProfileInt(_T("Settings"), _T("ScrollToFirst"), m_bScrollToFirst);
		theApp.WriteProfileInt(_T("Settings"), _T("BackupFile"), m_bBackup);
		theApp.WriteProfileString(_T("Settings"), _T("VssPath"), m_strVssPath);
		theApp.WriteProfileInt(_T("Settings"), _T("TabSize"), m_nTabSize);
		theApp.WriteProfileInt(_T("Settings"), _T("IgnoreBlankLines"), m_bIgnoreBlankLines);
		theApp.WriteProfileInt(_T("Settings"), _T("IgnoreCase"), m_bIgnoreCase);

		// make an attempt at rescanning any open diff sessions
		if (m_pLeft != NULL && m_pRight != NULL)
		{
			if (m_pLeft->SaveHelper())
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


	// check to make sure they are same type
	TCHAR name[MAX_PATH];
	BOOL bLeftIsDir=CFile::GetStatus(strLeft, status) && (status.m_attribute & CFile::Attribute::directory);
	BOOL bRightIsDir = CFile::GetStatus(strRight, status) && (status.m_attribute & CFile::Attribute::directory);
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
				  _T("\tShowUnique: %d\r\n")
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
				  m_bShowUnique,
				  m_bShowIdent,
				  m_bShowDiff,
				  m_bHideBak,
				  m_nVerSys,
				  m_strVssPath,
				  m_bBackup,
				  m_bIgnoreWhitespace,
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
			CDiffContext *pCtxt = new CDiffContext(strLeft, strRight);
			if (pCtxt != NULL)
			{
				m_pDirDoc->SetDiffContext(pCtxt);
				pCtxt->SetRegExp(strExt);
				m_pDirDoc->Rescan();
			}
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
		DeleteFile(s);

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

BOOL CMainFrame::SyncFiles(LPCTSTR pszSrc, LPCTSTR pszDest)
{
	CString strSavePath(pszDest);

	if (!CheckSavePath(strSavePath))
		return FALSE;
	
	if (!CreateBackup(strSavePath))
		return FALSE;
	
	// Now it's just a matter of copying the right file to the left
	DeleteFile(strSavePath);
	if (!CopyFile(pszSrc, strSavePath, FALSE))
	{
		
		return FALSE;
	}
	
	// tell the dir view to update itself
	return TRUE;
}

void CMainFrame::UpdateCurrentFileStatus(UINT nStatus)
{
	if (NULL != m_pDirDoc)
	{
		CDirView *pv = m_pDirDoc->GetMainView();
		if (NULL != pv)
		{
			CListCtrl& lc = pv->GetListCtrl();
			int sel = lc.GetNextItem(-1, LVNI_SELECTED);
			if (sel != -1)
			{
				// first change it in the dirlist
				POSITION pos = reinterpret_cast<POSITION>(lc.GetItemData(sel));
				DIFFITEM di = m_pDirDoc->m_pCtxt->m_dirlist.GetAt(pos);
				di.code = (BYTE)nStatus;
				m_pDirDoc->m_pCtxt->m_dirlist.SetAt(pos, di);
				m_pDirDoc->UpdateItemStatus(sel);
				//m_pDirDoc->Redisplay();
			}
		}
	}
}

void CMainFrame::OnViewSelectfont() 
{
	CHOOSEFONT cf;
	memset(&cf, 0, sizeof(CHOOSEFONT));
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.Flags = CF_INITTOLOGFONTSTRUCT|CF_FORCEFONTEXIST|CF_SCREENFONTS|CF_SCRIPTSONLY;
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
	m_bFontSpecified = theApp.GetProfileInt(_T("Font"), _T("Specified"), FALSE)!=FALSE;
	if (m_bFontSpecified)
	{
		m_lfDiff.lfHeight = theApp.GetProfileInt(_T("Font"), _T("Height"), 10);
		m_lfDiff.lfWidth = theApp.GetProfileInt(_T("Font"), _T("Width"), 0);
		m_lfDiff.lfEscapement = theApp.GetProfileInt(_T("Font"), _T("Escapement"), 0);
		m_lfDiff.lfOrientation = theApp.GetProfileInt(_T("Font"), _T("Orientation"), 0);
		m_lfDiff.lfWeight = theApp.GetProfileInt(_T("Font"), _T("Weight"), FW_NORMAL);
		m_lfDiff.lfItalic = (BYTE)theApp.GetProfileInt(_T("Font"), _T("Italic"), FALSE);
		m_lfDiff.lfUnderline = (BYTE)theApp.GetProfileInt(_T("Font"), _T("Underline"), FALSE);
		m_lfDiff.lfStrikeOut = (BYTE)theApp.GetProfileInt(_T("Font"), _T("StrikeOut"), FALSE);
		m_lfDiff.lfCharSet = (BYTE)theApp.GetProfileInt(_T("Font"), _T("CharSet"), ANSI_CHARSET);
		m_lfDiff.lfOutPrecision = (BYTE)theApp.GetProfileInt(_T("Font"), _T("OutPrecision"), OUT_TT_PRECIS);
		m_lfDiff.lfClipPrecision = (BYTE)theApp.GetProfileInt(_T("Font"), _T("ClipPrecision"), CLIP_TT_ALWAYS);
		m_lfDiff.lfQuality = (BYTE)theApp.GetProfileInt(_T("Font"), _T("Quality"), DEFAULT_QUALITY);
		m_lfDiff.lfPitchAndFamily = (BYTE)theApp.GetProfileInt(_T("Font"), _T("PitchAndFamily"), FF_SWISS | DEFAULT_PITCH);
		_tcscpy(m_lfDiff.lfFaceName, theApp.GetProfileString(_T("Font"), _T("FaceName"), _T("MS Sans Serif")));
	}
	else
		memset(&m_lfDiff, 0, sizeof(LOGFONT));
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
		ShellExecute(NULL, _T("open"), _T("http://www.geocities.com/SiliconValley/Vista/8632/WinMerge/index.html"), NULL, NULL, SW_SHOWNORMAL);

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
		if (m_pLeft->IsModified() && !m_pLeft->SaveHelper())
				return;
		if (m_pRight->IsModified() && !m_pRight->SaveHelper())
				return;
	}
	

	WINDOWPLACEMENT wp;
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

void CMainFrame::SetDiffStatus(int nDiff, int nDiffCnt)
{
	CString sIdx,sCnt,s;
	if (nDiff < 0 &&  nDiffCnt <= 0)
		s = _T("");
	else if (nDiff < 0)
	{
		sCnt.Format(_T("%ld"), nDiffCnt);
		AfxFormatString1(s, IDS_NO_DIFF_SEL_FMT, sCnt); 
	}
	else
	{
		sIdx.Format(_T("%ld"), nDiff+1);
		sCnt.Format(_T("%ld"), nDiffCnt);
		AfxFormatString2(s, IDS_DIFF_NUMBER_STATUS_FMT, sIdx, sCnt); 
	}
	m_wndStatusBar.SetPaneText(ID_DIFFNUM, s);
}
