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
 * @file  MainFrm.cpp
 *
 * @brief Implementation of the CMainFrame class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

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
#include "coretools.h"
#include "Splash.h"
#include "VssPrompt.h"
#include "CCPrompt.h"
#include "PropVss.h"
#include "PropGeneral.h"
#include "PropFilter.h"
#include "PropColors.h"
#include "PropRegistry.h"
#include "RegKey.h"
#include "logfile.h"
#include "PropSyntax.h"
#include "ssapi.h"      // BSP - Includes for Visual Source Safe COM interface
#include "multimon.h"
#include "paths.h"
#include "WaitStatusCursor.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int recursive;
CMainFrame *mf = NULL;
CLogFile gLog(_T("WinMerge.log"), NULL, TRUE);

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
	ON_COMMAND(ID_OPTIONS_SHOWBINARIES, OnOptionsShowBinaries)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWDIFFERENT, OnUpdateOptionsShowdifferent)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWIDENTICAL, OnUpdateOptionsShowidentical)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWUNIQUELEFT, OnUpdateOptionsShowuniqueleft)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWUNIQUERIGHT, OnUpdateOptionsShowuniqueright)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWBINARIES, OnUpdateOptionsShowBinaries)
	ON_WM_CREATE()
	ON_WM_MENUCHAR()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_HIDE_BACKUP_FILES, OnUpdateHideBackupFiles)
	ON_COMMAND(ID_HELP_GNULICENSE, OnHelpGnulicense)
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_COMMAND(ID_HIDE_BACKUP_FILES, OnHideBackupFiles)
	ON_COMMAND(ID_VIEW_SELECTFONT, OnViewSelectfont)
	ON_COMMAND(ID_VIEW_USEDEFAULTFONT, OnViewUsedefaultfont)
	ON_UPDATE_COMMAND_UI(ID_VIEW_USEDEFAULTFONT, OnUpdateViewUsedefaultfont)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_UPDATE_COMMAND_UI(ID_HELP_CONTENTS, OnUpdateHelpContents)
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_WHITESPACE, OnViewWhitespace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WHITESPACE, OnUpdateViewWhitespace)
	ON_WM_DROPFILES()
	ON_MESSAGE(MSG_STAT_UPDATE, OnUpdateStatusMessage)
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

	m_bFontSpecified=FALSE;
	m_strSaveAsPath = _T("");
	m_bFirstTime = TRUE;

	m_bAutomaticRescan = theApp.GetProfileInt(_T("Settings"), _T("AutomaticRescan"), TRUE)!=0;
	m_bIgnoreBlankLines = theApp.GetProfileInt(_T("Settings"), _T("IgnoreBlankLines"), FALSE)!=0;
	m_bEolSensitive = theApp.GetProfileInt(_T("Settings"), _T("EolSensitive"), FALSE)!=0;
	m_bIgnoreCase = theApp.GetProfileInt(_T("Settings"), _T("IgnoreCase"), FALSE)!=0;
	m_bShowUniqueLeft = theApp.GetProfileInt(_T("Settings"), _T("ShowUniqueLeft"), TRUE)!=0;
	m_bShowUniqueRight = theApp.GetProfileInt(_T("Settings"), _T("ShowUniqueRight"), TRUE)!=0;
	m_bShowDiff = theApp.GetProfileInt(_T("Settings"), _T("ShowDifferent"), TRUE)!=0;
	m_bShowIdent = theApp.GetProfileInt(_T("Settings"), _T("ShowIdentical"), TRUE)!=0;
	m_bShowBinaries = theApp.GetProfileInt(_T("Settings"), _T("ShowBinaries"), TRUE)!=0;
	m_bBackup = theApp.GetProfileInt(_T("Settings"), _T("BackupFile"), TRUE)!=0;
	m_bViewWhitespace = theApp.GetProfileInt(_T("Settings"), _T("ViewWhitespace"), FALSE)!=0;
	m_bScrollToFirst = theApp.GetProfileInt(_T("Settings"), _T("ScrollToFirst"), FALSE)!=0;
	m_nIgnoreWhitespace = theApp.GetProfileInt(_T("Settings"), _T("IgnoreSpace"), 1);
	m_bHideBak = theApp.GetProfileInt(_T("Settings"), _T("HideBak"), TRUE)!=0;
	m_nVerSys = theApp.GetProfileInt(_T("Settings"), _T("VersionSystem"), 0);
	m_strVssProject = theApp.GetProfileString(_T("Settings"), _T("VssProject"), _T(""));
	m_strVssUser = theApp.GetProfileString(_T("Settings"), _T("VssUser"), _T(""));
	m_strVssPassword = theApp.GetProfileString(_T("Settings"), _T("VssPassword"), _T(""));
	m_strVssPath = theApp.GetProfileString(_T("Settings"), _T("VssPath"), _T(""));
	m_nTabSize = theApp.GetProfileInt(_T("Settings"), _T("TabSize"), 4);
	m_nTabType = theApp.GetProfileInt(_T("Settings"), _T("TabType"), 0);
	m_bIgnoreRegExp = theApp.GetProfileInt(_T("Settings"), _T("IgnoreRegExp"), FALSE);
	m_sPattern = theApp.GetProfileString(_T("Settings"), _T("RegExps"), NULL);
	theApp.SetFileFilterName(theApp.GetProfileString(_T("Settings"), _T("FileFilterName"), _T("")));
	m_bReuseDirDoc = TRUE;
	// TODO: read preference for logging

	if (m_strVssPath.IsEmpty())
	{
		CRegKeyEx reg;
		if (reg.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\SourceSafe")) == ERROR_SUCCESS)
		{
			TCHAR temp[_MAX_PATH] = {0};
			reg.ReadChars(_T("SCCServerPath"), temp, _MAX_PATH, _T(""));
			CString spath = GetPathOnly(temp);
			m_strVssPath = spath + _T("\\Ss.exe");
		}
	}
}

CMainFrame::~CMainFrame()
{
	// destroy the reg expression list
	FreeRegExpList();
}

// This is a bridge to implement IStatusDisplay for WaitStatusCursor
// by forwarding all calls to the main frame
class StatusDisplay : public IStatusDisplay
{
public:
	StatusDisplay() : m_pfrm(0) { }
	void SetFrame(CMainFrame * frm) { m_pfrm = frm; }
// Implement IStatusDisplay
	virtual CString BeginStatus(LPCTSTR str) { return m_pfrm->SetStatus(str); }
	virtual void ChangeStatus(LPCTSTR str) { m_pfrm->SetStatus(str); }
	virtual void EndStatus(LPCTSTR str, LPCTSTR oldstr) { m_pfrm->SetStatus(oldstr); }

protected:
	CMainFrame * m_pfrm;
};

static StatusDisplay myStatusDisplay;

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
	m_wndStatusBar.SetPaneInfo(2, ID_DIFFSTATUS, 0, 250); 

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

	// Start handling status messages from WaitStatusCursors
	myStatusDisplay.SetFrame(this);
	WaitStatusCursor::SetStatusDisplay(&myStatusDisplay);

	return 0;
}

HMENU CMainFrame::NewDefaultMenu()
{
	m_default.LoadMenu(IDR_MAINFRAME);
	m_default.LoadToolbar(IDR_MAINFRAME);
	return(m_default.Detach());
}

//This handler ensures that keyboard shortcuts work
LRESULT CMainFrame::OnMenuChar(UINT nChar, UINT nFlags, 
	CMenu* pMenu) 
{
	LRESULT lresult;
	if(m_default.IsMenu(pMenu))
		lresult=BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	else
		lresult=CMDIFrameWnd::OnMenuChar(nChar, nFlags, pMenu);
	return(lresult);
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

/// Creates new MergeDoc instance and shows documents
void CMainFrame::ShowMergeDoc(CDirDoc * pDirDoc, LPCTSTR szLeft, LPCTSTR szRight)
{
	BOOL docNull;
	BOOL bOpenSuccess = FALSE;
	int nRescanResult = RESCAN_OK;
	CMergeDoc * pMergeDoc = GetMergeDocToShow(pDirDoc, &docNull);

	ASSERT(pMergeDoc);		// must ASSERT to get an answer to the question below ;-)
	if (!pMergeDoc) return; // when does this happen ?

	bOpenSuccess = pMergeDoc->OpenDocs(szLeft, szRight);

	if (bOpenSuccess)
	{
		if (docNull)
		{
			CWnd* pWnd = pMergeDoc->GetParentFrame();
			MDIActivate(pWnd);
		}
		else
			MDINext();
	}
}

void CMainFrame::RedisplayAllDirDocs()
{
	DirDocList dirdocs;
	GetAllDirDocs(&dirdocs);
	while (!dirdocs.IsEmpty())
	{
		CDirDoc * pDirDoc = dirdocs.RemoveHead();
		pDirDoc->Redisplay();
	}
}

void CMainFrame::OnOptionsShowDifferent() 
{
	m_bShowDiff = !m_bShowDiff;
	theApp.WriteProfileInt(_T("Settings"), _T("ShowDifferent"), m_bShowDiff);
	RedisplayAllDirDocs();
}

void CMainFrame::OnOptionsShowIdentical() 
{
	m_bShowIdent = !m_bShowIdent;
	theApp.WriteProfileInt(_T("Settings"), _T("ShowIdentical"), m_bShowIdent);
	RedisplayAllDirDocs();
}

void CMainFrame::OnOptionsShowUniqueLeft() 
{
	m_bShowUniqueLeft = !m_bShowUniqueLeft;
	theApp.WriteProfileInt(_T("Settings"), _T("ShowUniqueLeft"), m_bShowUniqueLeft);
	RedisplayAllDirDocs();
}

void CMainFrame::OnOptionsShowUniqueRight() 
{
	m_bShowUniqueRight = !m_bShowUniqueRight;
	theApp.WriteProfileInt(_T("Settings"), _T("ShowUniqueRight"), m_bShowUniqueRight);
	RedisplayAllDirDocs();
}

void CMainFrame::OnOptionsShowBinaries()
{
	m_bShowBinaries = !m_bShowBinaries;
	theApp.WriteProfileInt(_T("Settings"), _T("ShowBinaries"), m_bShowBinaries);
	RedisplayAllDirDocs();
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

void CMainFrame::OnUpdateOptionsShowBinaries(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowBinaries);
}

void CMainFrame::OnHideBackupFiles() 
{
	m_bHideBak = ! m_bHideBak;
	theApp.WriteProfileInt(_T("Settings"), _T("HideBak"), m_bHideBak);
	RedisplayAllDirDocs();
}

void CMainFrame::OnUpdateHideBackupFiles(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bHideBak);
}

void CMainFrame::OnHelpGnulicense() 
{
	CString spath = GetModulePath() + _T("\\Copying");
	CString url = _T("http://www.gnu.org/copyleft/gpl.html");
	
	CFileStatus status;
	if (CFile::GetStatus(spath, status))
		ShellExecute(m_hWnd, _T("open"), _T("notepad.exe"), spath, NULL, SW_SHOWNORMAL);
	else
		ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);
}

/**
* @brief Check if file is read-only and save to version control if one is used.
*
*
* @param strSavePath Path where to save including filename
*
* @return Tells if caller can continue (no errors happened)
*
* @note If user selects "Cancel" FALSE is returned and caller must assume file
* is not saved.
*
* @sa SaveToVersionControl()
*
*/
BOOL CMainFrame::CheckSavePath(CString& strSavePath)
{
	CFileStatus status;
	UINT userChoice = 0;
	BOOL bRetVal = TRUE;
	CString s;

	if (CFile::GetStatus(strSavePath, status))
	{
		// If file is read-only
		if (status.m_attribute & CFile::Attribute::readOnly)
		{
			// Version control system used?
			if (m_nVerSys > 0)
				bRetVal = SaveToVersionControl(strSavePath);
			else
			{
				CString title;
				VERIFY(title.LoadString(IDS_SAVE_AS_TITLE));
				
				// Prompt for user choice
				AfxFormatString1(s, IDS_SAVEREADONLY_FMT, strSavePath);
				userChoice = AfxMessageBox(s, MB_YESNOCANCEL |
						MB_ICONQUESTION | MB_DEFBUTTON2);
				switch (userChoice)
				{
				// Overwrite read-only file
				case IDYES:
					status.m_mtime = 0;		// Avoid unwanted changes
					status.m_attribute &= ~CFile::Attribute::readOnly;
					CFile::SetStatus(strSavePath, status);
					break;
				
				// Save to new filename
				case IDNO:
					if (SelectFile(s, strSavePath, title, NULL, FALSE))
						strSavePath = s;
					else
						bRetVal = FALSE;
					break;
				
				// Cancel saving
				case IDCANCEL:
					bRetVal = FALSE;
					break;
				}
			}
		}
	}
	return bRetVal;
}

/**
* @brief Saves file to selected version control system
*
* @param strSavePath Path where to save including filename
*
* @return Tells if caller can continue (no errors happened)
*
* @note
*
* @sa CheckSavePath()
*
*/
BOOL CMainFrame::SaveToVersionControl(CString& strSavePath)
{
	CFileStatus status;
	CString s;
	UINT userChoice = 0;

	switch(m_nVerSys)
	{
	case 0:	//no versioning system
		// Already handled in CheckSavePath()
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
			CString spath, sname;
			SplitFilename(strSavePath, &spath, &sname, 0);
			if (!spath.IsEmpty())
			{
				_chdrive(toupper(spath[0])-'A'+1);
				_chdir(spath);
			}
			CString args;
			args.Format(_T("checkout %s/%s"), m_strVssProject, sname);
			HANDLE hVss = RunIt(m_strVssPath, args, TRUE, FALSE);
			if (hVss!=INVALID_HANDLE_VALUE)
			{
				WaitForSingleObject(hVss, INFINITE);
				DWORD code;
				GetExitCodeProcess(hVss, &code);
				CloseHandle(hVss);
				if (code != 0)
				{
					AfxMessageBox(IDS_VSSERROR, MB_ICONSTOP);
					return FALSE;
				}
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

			CString spath, sname;
			SplitFilename(strSavePath, &spath, &sname, 0);

            // BSP - Combine the project entered on the dialog box with the file name...
			CString strItem = m_strVssProject + '/' + sname;

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
			CString spath, sname;
			SplitFilename(strSavePath, &spath, &sname, 0);
			if (!spath.IsEmpty())
			{
				_chdrive(toupper(spath[0])-'A'+1);
				_chdir(spath);
			}
			DWORD code;
			CString args;
			args.Format(_T("checkout -c \"%s\" %s"), dlg.m_comments, sname);
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

	return TRUE;
}

void CMainFrame::OnOptions() 
{
	CStringList filefilters;
	CString selectedFilter;
	theApp.GetFileFilterNameList(filefilters, selectedFilter);

	CPropertySheet sht(IDS_OPTIONS_TITLE);
	CPropVss vss;
	CPropGeneral gen;
	CPropSyntax syn;
	CPropFilter filter(filefilters, selectedFilter);
	CPropColors colors(theApp.GetDiffColor(), theApp.GetSelDiffColor(), theApp.GetDiffDeletedColor(), theApp.GetSelDiffDeletedColor(), theApp.GetDiffTextColor(), theApp.GetSelDiffTextColor());
	CPropRegistry regpage;
	sht.AddPage(&gen);
	sht.AddPage(&syn);
	sht.AddPage(&filter);
	sht.AddPage(&vss);
	sht.AddPage(&colors);
	sht.AddPage(&regpage);
	
	vss.m_nVerSys = m_nVerSys;
	vss.m_strPath = m_strVssPath;

	gen.m_bBackup = m_bBackup;
	gen.m_nIgnoreWhite = m_nIgnoreWhitespace;
	gen.m_bIgnoreCase = m_bIgnoreCase;
	gen.m_bIgnoreBlankLines = m_bIgnoreBlankLines;
	gen.m_bEolSensitive = m_bEolSensitive;
	gen.m_bScroll = m_bScrollToFirst;
	gen.m_nTabSize = m_nTabSize;
	gen.m_nTabType = m_nTabType;
	gen.m_bDisableSplash = theApp.m_bDisableSplash;
	gen.m_bAutomaticRescan = m_bAutomaticRescan;

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
		m_nTabType = gen.m_nTabType;
		theApp.m_bDisableSplash = gen.m_bDisableSplash;
		m_bAutomaticRescan = gen.m_bAutomaticRescan;

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
		theApp.SetFileFilterName(filter.m_sFileFilterName);

		theApp.SetDiffColor(colors.m_clrDiff);
		theApp.SetSelDiffColor(colors.m_clrSelDiff);
		theApp.SetDiffDeletedColor(colors.m_clrDiffDeleted);
		theApp.SetSelDiffDeletedColor(colors.m_clrSelDiffDeleted);
		theApp.SetDiffTextColor(colors.m_clrDiffText);
		theApp.SetSelDiffTextColor(colors.m_clrSelDiffText);

		theApp.WriteProfileInt(_T("Settings"), _T("VersionSystem"), m_nVerSys);
		theApp.WriteProfileInt(_T("Settings"), _T("IgnoreSpace"), m_nIgnoreWhitespace);
		theApp.WriteProfileInt(_T("Settings"), _T("ScrollToFirst"), m_bScrollToFirst);
		theApp.WriteProfileInt(_T("Settings"), _T("BackupFile"), m_bBackup);
		theApp.WriteProfileString(_T("Settings"), _T("VssPath"), m_strVssPath);
		theApp.WriteProfileInt(_T("Settings"), _T("TabSize"), m_nTabSize);
		theApp.WriteProfileInt(_T("Settings"), _T("TabType"), m_nTabType);
		theApp.WriteProfileInt(_T("Settings"), _T("EolSensitive"), m_bEolSensitive);
		theApp.WriteProfileInt(_T("Settings"), _T("AutomaticRescan"), m_bAutomaticRescan);
		theApp.WriteProfileInt(_T("Settings"), _T("IgnoreBlankLines"), m_bIgnoreBlankLines);
		theApp.WriteProfileInt(_T("Settings"), _T("IgnoreCase"), m_bIgnoreCase);
		theApp.WriteProfileInt(_T("Settings"), _T("IgnoreRegExp"), m_bIgnoreRegExp);
		theApp.WriteProfileString(_T("Settings"), _T("RegExps"), m_sPattern);
		theApp.WriteProfileInt(_T("Settings"), _T("DisableSplash"), theApp.m_bDisableSplash);
		theApp.WriteProfileString(_T("Settings"), _T("FileFilterName"), filter.m_sFileFilterName);

		theApp.m_bHiliteSyntax = syn.m_bHiliteSyntax;
		theApp.WriteProfileInt(_T("Settings"), _T("HiliteSyntax"), theApp.m_bHiliteSyntax);

		RebuildRegExpList();

		// make an attempt at rescanning any open diff sessions
		MergeDocList docs;
		GetAllMergeDocs(&docs);
		BOOL savedAll=TRUE;
		while (!docs.IsEmpty())
		{
			CMergeDoc * pMergeDoc = docs.RemoveHead();
			CMergeEditView * pLeft = pMergeDoc->GetLeftView();
			CMergeEditView * pRight = pMergeDoc->GetRightView();

			// Enable/disable automatic rescan (rescan after editing)
			pLeft->EnableRescan(m_bAutomaticRescan);
			pRight->EnableRescan(m_bAutomaticRescan);

			// Set tab type (tabs/spaces)
			if (m_nTabType == 0)
			{
				pLeft->SetInsertTabs(TRUE);
				pRight->SetInsertTabs(TRUE);
			}
			else
			{
				pLeft->SetInsertTabs(FALSE);
				pRight->SetInsertTabs(FALSE);
			}

			if (pMergeDoc->SaveHelper())
			{
				int nRescanResult = RESCAN_OK;
				nRescanResult = pMergeDoc->Rescan(TRUE);
				if (nRescanResult != RESCAN_OK)
					pMergeDoc->ShowRescanError(nRescanResult);
			}
			// mods have been made, so just warn
			else
			{
				savedAll = FALSE;
			}
		}
		if (!savedAll)
			AfxMessageBox(IDS_DIFF_OPEN_NO_SET_PROPS,MB_ICONEXCLAMATION);
	}
}

// clear counters used to track diff progress
void CMainFrame::clearStatus()
{
	m_nStatusFileSame = m_nStatusFileBinSame = m_nStatusFileDiff = m_nStatusFileBinDiff = m_nStatusFileError
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
	case FILE_BINSAME:
		++m_nStatusFileBinSame;
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
	s.Format(_T("s:%d bs:%d d:%d bd:%d lf:%d ld:%d rf:%d rd:%d e:%d")
		, m_nStatusFileSame, m_nStatusFileBinSame, m_nStatusFileDiff, m_nStatusFileBinDiff
		, m_nStatusLeftFileOnly, m_nStatusLeftDirOnly, m_nStatusRightFileOnly, m_nStatusRightDirOnly
		, m_nStatusFileError);
	m_wndStatusBar.SetPaneText(2, s);
}

BOOL CMainFrame::DoFileOpen(LPCTSTR pszLeft /*=NULL*/, LPCTSTR pszRight /*=NULL*/,
	DWORD dwLeftFlags /*=0*/, DWORD dwRightFlags /*=0*/, BOOL bRecurse /*=FALSE*/)
{
	CString strLeft(pszLeft);
	CString strRight(pszRight);
	CString strExt;

	BOOL docNull;
	CDirDoc * pDirDoc = GetDirDocToShow(&docNull);

	if (!docNull)
	{
		// If reusing an existing doc, give it a chance to save its data
		// and close any merge views, and clear its window
		if (!pDirDoc->ReusingDirDoc())
			return FALSE;
	}

	// pop up dialog unless arguments exist (and are compatible)
	PATH_EXISTENCE pathsType = GetPairComparability(pszLeft, pszRight);
	if (pathsType == DOES_NOT_EXIST)
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
		pathsType = static_cast<PATH_EXISTENCE>(dlg.m_pathsType);
	}
	else
	{
		//save the MRU left and right files.
		if (!(dwLeftFlags & FFILEOPEN_NOMRU))
			addToMru(pszLeft, _T("Files\\Left"));
		if (!(dwRightFlags & FFILEOPEN_NOMRU))
			addToMru(pszRight, _T("Files\\Right"));
	}

	if (1)
	{
		gLog.Write(_T("### Begin Comparison Parameters #############################\r\n")
				  _T("\tLeft: %s\r\n")
				   _T("\tRight: %s\r\n")
				  _T("\tRecurse: %d\r\n")
				  _T("\tShowUniqueLeft: %d\r\n")
				  _T("\tShowUniqueRight: %d\r\n")
				  _T("\tShowIdentical: %d\r\n")
				  _T("\tShowDiff: %d\r\n")
				  _T("\tShowBinaries: %d\r\n")
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
				  m_bShowBinaries,
				  m_bHideBak,
				  m_nVerSys,
				  m_strVssPath,
				  m_bBackup,
				  m_nIgnoreWhitespace,
				  m_bScrollToFirst);
	}

	// open the diff
	if (pathsType == IS_EXISTING_DIR)
	{
		recursive = bRecurse;

		if (pDirDoc)
		{
			CDiffContext *pCtxt = new CDiffContext(strLeft, strRight);
			if (pCtxt != NULL)
			{
				pDirDoc->SetDiffContext(pCtxt);
				pCtxt->SetRegExp(strExt);
				pDirDoc->Rescan();
				if (m_bScrollToFirst)
				{
					CDirView * pDirView = pDirDoc->GetMainView();
					pDirView->GotoFirstDiff();
				}

			}
		}
	}
	else
	{
		recursive = FALSE;
		ShowMergeDoc(pDirDoc, strLeft, strRight);
	}
	return TRUE;
}

BOOL CMainFrame::CreateBackup(LPCTSTR pszPath)
{
	// first, make a backup copy of the original
	CFileStatus status;

	// create backup copy of file if destination file exists
	if (m_bBackup && CFile::GetStatus(pszPath, status))
	{
		// build the backup filename
		CString spath, sname, sext;
		SplitFilename(pszPath, &spath, &sname, &sext);
		CString s;
		if (!sext.IsEmpty())
			s.Format(_T("%s\\%s.%s") BACKUP_FILE_EXT, spath, sname, sext);
		else
			s.Format(_T("%s\\%s")  BACKUP_FILE_EXT, spath, sname);

		// Copy the file
		if (!CopyFile(pszPath, s, FALSE))
		{
			if (AfxMessageBox(IDS_BACKUP_FAILED_PROMPT,
					MB_YESNO | MB_ICONQUESTION) != IDYES)
				return FALSE;
		}
		return TRUE;
	}

	// we got here because we're either not backing up of there was nothing to backup
	return TRUE;
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
		CString sError = GetSysError(GetLastError());
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
		CString sError = GetSysError(GetLastError());
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
		CString sError = GetSysError(GetLastError());
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
		CString msg;
		AfxFormatString2(msg, IDS_COPY_FILE_FAILED, *psError, pszSrc);
		*psError = msg;
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
	if (!CopyFile(pszSrc, strSavePath, FALSE))
	{
		*psError = GetSysError(GetLastError());
		return FALSE;
	}
	
	// tell the dir view to update itself
	return TRUE;
}

void CMainFrame::UpdateCurrentFileStatus(CDirDoc * pDirDoc, UINT nStatus, int idx)
{
	ASSERT(pDirDoc);
	CDirView *pv = pDirDoc->GetMainView();
	ASSERT(pv);
	// first change it in the dirlist
	POSITION diffpos = pv->GetItemKey(idx);

	// TODO: Why is the update broken into these pieces ?
	// Someone could figure out these pieces and probably simplify this.

	// update DIFFITEM code
	pDirDoc->m_pCtxt->UpdateStatusCode(diffpos, (BYTE)nStatus);
	// update DIFFITEM time, and also tell views
	pDirDoc->ReloadItemStatus(idx);
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

		MergeEditViewList editViews;
		GetAllViews(&editViews, NULL);
		for (POSITION pos = editViews.GetHeadPosition(); pos; editViews.GetNext(pos))
		{
			CMergeEditView * pEditView = editViews.GetAt(pos);
			// update pEditView for font change
		}
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

/// Update any language-dependent data
void CMainFrame::UpdateResources()
{
	m_wndStatusBar.SetPaneText(0, LoadResString(AFX_IDS_IDLEMESSAGE));

	DirDocList dirdocs;
	GetAllDirDocs(&dirdocs);
	while (!dirdocs.IsEmpty())
	{
		CDirDoc * pDoc = dirdocs.RemoveHead();
		pDoc->UpdateResources();
	}

	MergeDocList mergedocs;
	GetAllMergeDocs(&mergedocs);
	while (!mergedocs.IsEmpty())
	{
		CMergeDoc * pDoc = mergedocs.RemoveHead();
		pDoc->GetLeftView()->UpdateResources();
		pDoc->GetRightView()->UpdateResources();
	}
}

void CMainFrame::OnHelpContents() 
{
	CString spath = GetModulePath(0) + _T("\\Docs\\index.html");
	CString url = _T("http://winmerge.sourceforge.net/docs20/index.html");

	CFileStatus status;
	if (CFile::GetStatus(spath, status))
		ShellExecute(NULL, _T("open"), spath, NULL, NULL, SW_SHOWNORMAL);
	else
		ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);

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

	dsk_rc.left = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
	dsk_rc.top = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
	dsk_rc.right = dsk_rc.left + ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
	dsk_rc.bottom = dsk_rc.top + ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
	if (theApp.GetProfileInt(_T("Settings"), _T("MainMax"), FALSE))
	{
		CMDIFrameWnd::ActivateFrame(SW_MAXIMIZE);	
	}
	else if (rc.Width() != 0 && rc.Height() != 0)
	{
		// Ensure top-left corner is on visible area,
		// 20 points margin is added to prevent "lost" window
		CPoint ptTopLeft(rc.TopLeft());
		ptTopLeft += CPoint(20, 20);

		if (dsk_rc.PtInRect(ptTopLeft))
			SetWindowPlacement(&wp);
		else
			CMDIFrameWnd::ActivateFrame(nCmdShow);
	}
	else
		CMDIFrameWnd::ActivateFrame(nCmdShow);
}

void CMainFrame::OnClose() 
{
	// save any dirty edit views
	MergeDocList mergedocs;
	GetAllMergeDocs(&mergedocs);
	for (POSITION pos = mergedocs.GetHeadPosition(); pos; mergedocs.GetNext(pos))
	{
		CMergeDoc * pMergeDoc = mergedocs.GetAt(pos);
		CMergeEditView * pLeft = pMergeDoc->GetLeftView();
		CMergeEditView * pRight = pMergeDoc->GetRightView();
		if ((pLeft && pLeft->IsModified())
			|| (pRight && pRight->IsModified()))
		{
			if (!pMergeDoc->SaveHelper())
				return;
			else
			{
				// Set modified status to false so that we are not asking
				// about saving again. 
				pMergeDoc->m_ltBuf.SetModified(FALSE);
				pMergeDoc->m_rtBuf.SetModified(FALSE);
			}
		}
	}

	// save main window position
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wp);
	theApp.WriteProfileInt(_T("Settings"), _T("MainLeft"),wp.rcNormalPosition.left);
	theApp.WriteProfileInt(_T("Settings"), _T("MainTop"),wp.rcNormalPosition.top);
	theApp.WriteProfileInt(_T("Settings"), _T("MainRight"),wp.rcNormalPosition.right);
	theApp.WriteProfileInt(_T("Settings"), _T("MainBottom"),wp.rcNormalPosition.bottom);
	theApp.WriteProfileInt(_T("Settings"), _T("MainMax"), (wp.showCmd == SW_MAXIMIZE));

	// tell all merge docs to save position
	while (!mergedocs.IsEmpty())
	{
		CMergeDoc * pMergeDoc = mergedocs.RemoveHead();
		CMergeEditView * pLeft = pMergeDoc->GetLeftView();
		if (pLeft)
			pMergeDoc->GetParentFrame()->SavePosition();
	}
	
	// Stop handling status messages from WaitStatusCursors
	WaitStatusCursor::SetStatusDisplay(0);
	myStatusDisplay.SetFrame(0);
	
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
	_TCHAR tmp[_MAX_PATH] = {0};
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

void CMainFrame::OnViewWhitespace() 
{
	m_bViewWhitespace = !m_bViewWhitespace;
	theApp.WriteProfileInt(_T("Settings"), _T("ViewWhitespace"), m_bViewWhitespace);

	MergeDocList mergedocs;
	GetAllMergeDocs(&mergedocs);
	while (!mergedocs.IsEmpty())
	{
		CMergeDoc * pMergeDoc = mergedocs.RemoveHead();
		CMergeEditView * pLeft = pMergeDoc->GetLeftView();
		CMergeEditView * pRight = pMergeDoc->GetRightView();
		if (pLeft)
		{
			pLeft->SetViewTabs(mf->m_bViewWhitespace);
			pLeft->SetViewEols(mf->m_bViewWhitespace);
		}
		if (pRight)
		{
			pRight->SetViewTabs(mf->m_bViewWhitespace);
			pRight->SetViewEols(mf->m_bViewWhitespace);
		}
	}
}

void CMainFrame::OnUpdateViewWhitespace(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bViewWhitespace);
}

// get list of MergeDocs (documents underlying edit sessions)
void CMainFrame::GetAllMergeDocs(MergeDocList * pMergeDocs)
{
	CMultiDocTemplate * pTemplate = theApp.m_pDiffTemplate;
	for (POSITION pos = pTemplate->GetFirstDocPosition(); pos; )
	{
		CDocument * pDoc = pTemplate->GetNextDoc(pos);
		CMergeDoc * pMergeDoc = static_cast<CMergeDoc *>(pDoc);
		pMergeDocs->AddTail(pMergeDoc);
	}
}

// get list of DirDocs (documents underlying a scan)
void CMainFrame::GetAllDirDocs(DirDocList * pDirDocs)
{
	CMultiDocTemplate * pTemplate = theApp.m_pDirTemplate;
	for (POSITION pos = pTemplate->GetFirstDocPosition(); pos; )
	{
		CDocument * pDoc = pTemplate->GetNextDoc(pos);
		CDirDoc * pDirDoc = static_cast<CDirDoc *>(pDoc);
		pDirDocs->AddTail(pDirDoc);
	}
}

// get pointers to all views into typed lists (both arguments are optional)
void CMainFrame::GetAllViews(MergeEditViewList * pEditViews, DirViewList * pDirViews)
{
	for (POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition(); pos; )
	{
		CDocTemplate * pTemplate = AfxGetApp()->GetNextDocTemplate(pos);
		for (POSITION pos2 = pTemplate->GetFirstDocPosition(); pos2; )
		{
			CDocument * pDoc = pTemplate->GetNextDoc(pos2);
			CMergeDoc * pMergeDoc = dynamic_cast<CMergeDoc *>(pDoc);
			CDirDoc * pDirDoc = dynamic_cast<CDirDoc *>(pDoc);
			for (POSITION pos3 = pDoc->GetFirstViewPosition(); pos3; )
			{
				CView * pView = pDoc->GetNextView(pos3);
				if (pMergeDoc)
				{
					if (pEditViews)
					{
						// a merge doc only has merge edit views
						CMergeEditView * pEditView = dynamic_cast<CMergeEditView *>(pView);
						ASSERT(pEditView);
						pEditViews->AddTail(pEditView);
					}
				}
				else if (pDirDoc)
				{
					if (pDirViews)
					{
						// a dir doc only has dir views
						CDirView * pDirView = dynamic_cast<CDirView *>(pView);
						ASSERT(pDirView);
						pDirViews->AddTail(pDirView);
					}
				}
				else
				{
					// There are currently only two types of docs 2003-02-20
					ASSERT(0);
				}
			}
		}
	}
}

// Obtain a merge doc to display a difference in files
CMergeDoc * CMainFrame::GetMergeDocToShow(CDirDoc * pDirDoc, BOOL * pNew)
{
	CMergeDoc * pMergeDoc = pDirDoc->GetMergeDocForDiff(pNew);
	return pMergeDoc;
}

// get pointer to a dir doc for displaying a scan
CDirDoc * CMainFrame::GetDirDocToShow(BOOL * pNew)
{
	CDirDoc * pDirDoc = 0;
	if (m_bReuseDirDoc)
	{
		POSITION pos = theApp.m_pDirTemplate->GetFirstDocPosition();
		if (pos)
		{
			pDirDoc = static_cast<CDirDoc *>(theApp.m_pDirTemplate->GetNextDoc(pos));
			*pNew = FALSE;
		}
	}
	if (!pDirDoc)
	{
		pDirDoc = (CDirDoc*)theApp.m_pDirTemplate->OpenDocumentFile(NULL);
		*pNew = TRUE;
	}
	return pDirDoc;
}

// Set status in the main status pane
CString CMainFrame::SetStatus(LPCTSTR status)
{
	CString old = m_wndStatusBar.GetPaneText(0);
	m_wndStatusBar.SetPaneText(0, status);
	return old;
}


/////////////////////////////////////////////////////////////////////////////
//
//	OnDropFiles code from CDropEdit
//	Copyright 1997 Chris Losinger
//
//	shortcut expansion code modified from :
//	CShortcut, 1996 Rob Warner
//
void CMainFrame::OnDropFiles(HDROP dropInfo)
{
	// Get the number of pathnames that have been dropped
	UINT wNumFilesDropped = DragQueryFile(dropInfo, 0xFFFFFFFF, NULL, 0);
	CString files[2];
	UINT fileCount = 0;

	// get all file names. but we'll only need the first one.
	for (WORD x = 0 ; x < wNumFilesDropped; x++)
	{
		// Get the number of bytes required by the file's full pathname
		UINT wPathnameSize = DragQueryFile(dropInfo, x, NULL, 0);

		// Allocate memory to contain full pathname & zero byte
		wPathnameSize += 1;
		LPTSTR npszFile = (TCHAR *) new TCHAR[wPathnameSize];

		// If not enough memory, skip this one
		if (npszFile == NULL)
			continue;

		// Copy the pathname into the buffer
		DragQueryFile(dropInfo, x, npszFile, wPathnameSize);

		if (x < 2)
		{
			files[x] = npszFile;
			fileCount++;
		}
		delete[] npszFile;
	}

	// Free the memory block containing the dropped-file information
	DragFinish(dropInfo);

	for (UINT i = 0; i < fileCount; i++)
	{
		// if this was a shortcut, we need to expand it to the target path
		CString expandedFile = ExpandShortcut(files[i]);

		// if that worked, we should have a real file name
		if (!expandedFile.IsEmpty()) 
			files[i] = expandedFile;
	}

	// If Ctrl pressed, do recursive compare
	BOOL ctrlKey = ::GetAsyncKeyState(VK_CONTROL);
	DoFileOpen(files[0], files[1], FFILEOPEN_NONE, FFILEOPEN_NONE, ctrlKey);
}

void CMainFrame::OnUpdateStatusMessage(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0xFF)
		clearStatus();
	else
		rptStatus(wParam);
}
