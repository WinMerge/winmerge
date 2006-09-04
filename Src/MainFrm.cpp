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

#include <htmlhelp.h>  // From HTMLHelp Workshop (incl. in Platform SDK)
#include <mlang.h>
#include <shlwapi.h>
#include "Merge.h"
#include "BCMenu.h"
#include "MainFrm.h"
#include "DirFrame.h"		// Include type information
#include "ChildFrm.h"
#include "DirView.h"
#include "DirDoc.h"
#include "OpenDlg.h"
#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "LocationView.h"
#include "SyntaxColors.h"

#include "diff.h"
#include "coretools.h"
#include "Splash.h"
#include "PropLineFilter.h"
#include "logfile.h"
#include "paths.h"
#include "WaitStatusCursor.h"
#include "PatchTool.h"
#include "FileTransform.h"
#include "Plugins.h"
#include "SelectUnpackerDlg.h"
#include "files.h"
#include "ConfigLog.h"
#include "7zCommon.h"
#include "FileFiltersDlg.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "codepage_detect.h"
#include "unicoder.h"
#include "VSSHelper.h"
#include "codepage.h"
#include "ProjectFile.h"
#include "PreferencesDlg.h"
#include "AppSerialize.h"
#include "ProjectFilePathsDlg.h"

/*
 One source file must compile the stubs for multimonitor
 by defining the symbol COMPILE_MULTIMON_STUBS & including <multimon.h>
*/
#ifdef COMPILE_MULTIMON_STUBS
#undef COMPILE_MULTIMON_STUBS
#endif
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CLogFile gLog;

static BOOL add_regexp PARAMS((struct regexp_list **, char const*, BOOL bShowError));

static void LoadToolbarImageList(UINT nIDResource, CImageList& ImgList);

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_MENUCHAR()
	ON_WM_MEASUREITEM()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_OPTIONS_SHOWDIFFERENT, OnOptionsShowDifferent)
	ON_COMMAND(ID_OPTIONS_SHOWIDENTICAL, OnOptionsShowIdentical)
	ON_COMMAND(ID_OPTIONS_SHOWUNIQUELEFT, OnOptionsShowUniqueLeft)
	ON_COMMAND(ID_OPTIONS_SHOWUNIQUERIGHT, OnOptionsShowUniqueRight)
	ON_COMMAND(ID_OPTIONS_SHOWBINARIES, OnOptionsShowBinaries)
	ON_COMMAND(ID_OPTIONS_SHOWSKIPPED, OnOptionsShowSkipped)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWDIFFERENT, OnUpdateOptionsShowdifferent)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWIDENTICAL, OnUpdateOptionsShowidentical)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWUNIQUELEFT, OnUpdateOptionsShowuniqueleft)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWUNIQUERIGHT, OnUpdateOptionsShowuniqueright)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWBINARIES, OnUpdateOptionsShowBinaries)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_SHOWSKIPPED, OnUpdateOptionsShowSkipped)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_HELP_GNULICENSE, OnHelpGnulicense)
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_COMMAND(ID_VIEW_SELECTFONT, OnViewSelectfont)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SELECTFONT, OnUpdateViewSelectfont)
	ON_COMMAND(ID_VIEW_USEDEFAULTFONT, OnViewUsedefaultfont)
	ON_UPDATE_COMMAND_UI(ID_VIEW_USEDEFAULTFONT, OnUpdateViewUsedefaultfont)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_UPDATE_COMMAND_UI(ID_HELP_CONTENTS, OnUpdateHelpContents)
	ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
	ON_UPDATE_COMMAND_UI(ID_HELP_INDEX, OnUpdateHelpIndex)
	ON_COMMAND(ID_HELP_SEARCH, OnHelpSearch)
	ON_UPDATE_COMMAND_UI(ID_HELP_SEARCH, OnUpdateHelpSearch)
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_WHITESPACE, OnViewWhitespace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WHITESPACE, OnUpdateViewWhitespace)
	ON_COMMAND(ID_TOOLS_GENERATEPATCH, OnToolsGeneratePatch)
	ON_WM_DROPFILES()
	ON_WM_SETCURSOR()
	ON_COMMAND_RANGE(ID_UNPACK_MANUAL, ID_UNPACK_AUTO, OnPluginUnpackMode)
	ON_UPDATE_COMMAND_UI_RANGE(ID_UNPACK_MANUAL, ID_UNPACK_AUTO, OnUpdatePluginUnpackMode)
	ON_UPDATE_COMMAND_UI(ID_RELOAD_PLUGINS, OnUpdateReloadPlugins)
	ON_COMMAND(ID_RELOAD_PLUGINS, OnReloadPlugins)
	ON_COMMAND(ID_HELP_GETCONFIG, OnSaveConfigData)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_TOOLS_FILTERS, OnToolsFilters)
	ON_COMMAND(ID_DEBUG_LOADCONFIG, OnDebugLoadConfig)
	ON_COMMAND(ID_HELP_MERGE7ZMISMATCH, OnHelpMerge7zmismatch)
	ON_UPDATE_COMMAND_UI(ID_HELP_MERGE7ZMISMATCH, OnUpdateHelpMerge7zmismatch)
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_COMMAND(ID_FILE_OPENPROJECT, OnFileOpenproject)
	ON_MESSAGE(WM_COPYDATA, OnCopyData)
	ON_MESSAGE(WM_USER, OnUser)
	ON_COMMAND(ID_WINDOW_CLOSEALL, OnWindowCloseAll)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CLOSEALL, OnUpdateWindowCloseAll)
	ON_COMMAND(ID_FILE_SAVEPROJECT, OnSaveProject)
	ON_WM_TIMER()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/**
 * @brief MainFrame statusbar panels/indicators
 */
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,           // Merge mode
	ID_SEPARATOR,           // Diff number
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/** @brief Relative (to WinMerge executable ) path to local help file. */
static const TCHAR DocsPath[] = _T("\\Docs\\WinMerge.chm");

/**
 * @brief URL to help indes in internet.
 * We use internet help when local help file is not found (not installed).
 */
static const TCHAR DocsURL[] = _T("http://winmerge.org/2.4/manual/index.html");

/**
 * @brief Default relative path to "My Documents" for private filters.
 * We want to use WinMerge folder as general user-file folder in future.
 * So it makes sense to have own subfolder for filters.
 */
static const TCHAR DefaultRelativeFilterPath[] = _T("WinMerge\\Filters");

/** @brief Timer ID for window flashing timer. */
static const UINT ID_TIMER_FLASH = 1;

/** @brief Timeout for window flashing timer, in milliseconds. */
static const UINT WINDOW_FLASH_TIMEOUT = 500;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

/**
 * @brief MainFrame constructor. Loads settings from registry.
 * @todo Preference for logging?
 */
CMainFrame::CMainFrame()
: m_bFlashing(FALSE)
, m_bFirstTime(TRUE)
, m_bEscShutdown(FALSE)
, m_bClearCaseTool(FALSE)
, m_bExitIfNoDiff(FALSE)
, m_bShowErrors(TRUE)
, m_CheckOutMulti(FALSE)
, m_bVCProjSync(FALSE)
, m_bVssSuppressPathCheck(FALSE)
{
	ZeroMemory(&m_pMenus[0], sizeof(m_pMenus));
	OptionsInit(); // Implementation in OptionsInit.cpp
	UpdateCodepageModule();

	InitializeSourceControlMembers();
	g_bUnpackerMode = theApp.GetProfileInt(_T("Settings"), _T("UnpackerMode"), PLUGIN_MANUAL);
	// uncomment this when the GUI allows to toggle the mode
//	g_bPredifferMode = theApp.GetProfileInt(_T("Settings"), _T("PredifferMode"), PLUGIN_MANUAL);

	// TODO: read preference for logging

	int logging = m_options.GetInt(OPT_LOGGING);
	if (logging > 0)
	{
		gLog.EnableLogging(TRUE);
		gLog.SetFile(_T("WinMerge.log"));

		if (logging == 1)
			gLog.SetMaskLevel(LOGLEVEL::LALL);
		else if (logging == 2)
			gLog.SetMaskLevel(LOGLEVEL::LERROR | LOGLEVEL::LWARNING);
	}

	m_pSyntaxColors = new SyntaxColors();
	if (m_pSyntaxColors)
		m_pSyntaxColors->Initialize(&m_options);

	// Check if filter folder is set, and create it if not
	CString pathMyFolders = m_options.GetString(OPT_FILTER_USERPATH);
	if (pathMyFolders.IsEmpty())
	{
		pathMyFolders = paths_GetMyDocuments(GetSafeHwnd());
		CString pathFilters(pathMyFolders);
		if (pathFilters.Right(1) != _T("\\"))
			pathFilters += _T("\\");
		pathFilters += DefaultRelativeFilterPath;

		if (!paths_CreateIfNeeded(pathFilters))
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

		m_options.SaveOption(OPT_FILTER_USERPATH, pathFilters);
		theApp.m_globalFileFilter.SetFileFilterPath(pathFilters);
	}
}

CMainFrame::~CMainFrame()
{
	gLog.EnableLogging(FALSE);

	// destroy the reg expression list
	FreeRegExpList();
	// Delete all temporary folders belonging to this process
	GetClearTempPath(NULL, NULL);

	delete m_pMenus[MENU_DEFAULT];
	delete m_pMenus[MENU_MERGEVIEW];
	delete m_pMenus[MENU_DIRVIEW];
	delete m_pSyntaxColors;
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

	// build the initial reg expression list
	RebuildRegExpList(FALSE);
	GetFontProperties();
	
	if (!CreateToobar())
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
	m_wndStatusBar.SetPaneInfo(1, ID_STATUS_MERGINGMODE, 0, 100); 
	m_wndStatusBar.SetPaneInfo(2, ID_STATUS_DIFFNUM, 0, 150); 
	if (m_options.GetBool(OPT_SHOW_STATUSBAR) == false)
		CMDIFrameWnd::ShowControlBar(&m_wndStatusBar, false, 0);

	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen(this);

	// Start handling status messages from CustomStatusCursors
	myStatusDisplay.SetFrame(this);
	CustomStatusCursor::SetStatusDisplay(&myStatusDisplay);

	return 0;
}

/** 
 * @brief Find the scripts submenu from the main menu
 * As now this is the first submenu in "Edit" menu
 * We find the "Edit" menu by looking for a menu 
 *  starting with ID_EDIT_UNDO.
 */
HMENU CMainFrame::GetScriptsSubmenu(HMENU mainMenu)
{
	// look for "Edit" menu
	int i;
	for (i = 0 ; i < ::GetMenuItemCount(mainMenu) ; i++)
		if (::GetMenuItemID(::GetSubMenu(mainMenu, i), 0) == ID_EDIT_UNDO)
			break;
	HMENU editMenu = ::GetSubMenu(mainMenu, i);

	// look for "script" submenu (first submenu)
	for (i = 0 ; i < ::GetMenuItemCount(editMenu) ; i++)
		if (::GetSubMenu(editMenu, i) != NULL)
			return ::GetSubMenu(editMenu, i);

	// error, submenu not found
	return NULL;
}

/**
 * @brief Find the scripts submenu from the main menu
 * As now this is the first submenu in "Plugins" menu
 * We find the "Plugins" menu by looking for a menu 
 *  starting with ID_UNPACK_MANUAL.
 */
HMENU CMainFrame::GetPrediffersSubmenu(HMENU mainMenu)
{
	// look for "Plugins" menu
	int i;
	for (i = 0 ; i < ::GetMenuItemCount(mainMenu) ; i++)
		if (::GetMenuItemID(::GetSubMenu(mainMenu, i), 0) == ID_UNPACK_MANUAL)
			break;
	HMENU editMenu = ::GetSubMenu(mainMenu, i);

	// look for "script" submenu (first submenu)
	for (i = 0 ; i < ::GetMenuItemCount(editMenu) ; i++)
		if (::GetSubMenu(editMenu, i) != NULL)
			return ::GetSubMenu(editMenu, i);

	// error, submenu not found
	return NULL;
}

/**
 * @brief Remove debug menu if this is not a debug build
 * @param [in] menu Pointer to main menu.
 */
static void FixupDebugMenu(BCMenu * menu)
{
	bool DebugMenu = false;
#ifdef _DEBUG
	DebugMenu = true;
#endif

	if (DebugMenu)
		return;

	// Remove debug menu
	// Finds debug menu by looking for a submenu which
	//  starts with item ID_DEBUG_LOADCONFIG

	for (int i=0; i< menu->GetMenuItemCount(); ++i)
	{
		if (menu->GetSubMenu(i)->GetMenuItemID(0) == ID_DEBUG_LOADCONFIG)
		{
			menu->RemoveMenu(i, MF_BYPOSITION);
			return;
		}
	}
}

/**
 * @brief Create new default (CMainFrame) menu
 */
HMENU CMainFrame::NewDefaultMenu(int ID /*=0*/)
{
	if (ID == 0)
		ID = IDR_MAINFRAME;

	if (m_pMenus[MENU_DEFAULT] == NULL)
		m_pMenus[MENU_DEFAULT] = new BCMenu();
	if (m_pMenus[MENU_DEFAULT] == NULL)
		return NULL;

	if (!m_pMenus[MENU_DEFAULT]->LoadMenu(ID))
	{
		ASSERT(FALSE);
		return NULL;
	}
	
	// Load bitmaps to menuitems
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_EDIT_COPY, IDB_EDIT_COPY);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_EDIT_CUT, IDB_EDIT_CUT);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_EDIT_PASTE, IDB_EDIT_PASTE);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_EDIT_FIND, IDB_EDIT_SEARCH);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_WINDOW_CASCADE, IDB_WINDOW_CASCADE);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_WINDOW_TILE_HORZ, IDB_WINDOW_HORIZONTAL);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_WINDOW_TILE_VERT, IDB_WINDOW_VERTICAL);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_FILE_CLOSE, IDB_WINDOW_CLOSE);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_WINDOW_CHANGE_PANE, IDB_WINDOW_CHANGEPANE);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_EDIT_WMGOTO, IDB_EDIT_GOTO);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_EDIT_REPLACE, IDB_EDIT_REPLACE);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_VIEW_LANGUAGE, IDB_VIEW_LANGUAGE);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_VIEW_SELECTFONT, IDB_VIEW_SELECTFONT);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_APP_EXIT, IDB_FILE_EXIT);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_HELP_CONTENTS, IDB_HELP_CONTENTS);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_EDIT_SELECT_ALL, IDB_EDIT_SELECTALL);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_TOOLS_FILTERS, IDB_TOOLS_FILTERS);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_TOOLS_CUSTOMIZECOLUMNS, IDB_TOOLS_COLUMNS);
	m_pMenus[MENU_DEFAULT]->ModifyODMenu(NULL, ID_TOOLS_GENERATEPATCH, IDB_TOOLS_GENERATEPATCH);
	m_pMenus[MENU_DEFAULT]->LoadToolbar(IDR_MAINFRAME);

	FixupDebugMenu(m_pMenus[MENU_DEFAULT]);

	return(m_pMenus[MENU_DEFAULT]->Detach());
}

/**
 * @brief Create new File compare (CMergeEditView) menu
 */
HMENU CMainFrame::NewMergeViewMenu()
{
	if (m_pMenus[MENU_MERGEVIEW] == NULL)
		m_pMenus[MENU_MERGEVIEW] = new BCMenu();
	if (m_pMenus[MENU_MERGEVIEW] == NULL)
		return NULL;

	m_pMenus[MENU_MERGEVIEW]->LoadMenu(IDR_MERGEDOCTYPE);
	// Load bitmaps to menuitems
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_EDIT_COPY, IDB_EDIT_COPY);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_EDIT_CUT, IDB_EDIT_CUT);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_EDIT_PASTE, IDB_EDIT_PASTE);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_EDIT_FIND, IDB_EDIT_SEARCH);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_WINDOW_CASCADE, IDB_WINDOW_CASCADE);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_WINDOW_TILE_HORZ, IDB_WINDOW_HORIZONTAL);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_WINDOW_TILE_VERT, IDB_WINDOW_VERTICAL);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_FILE_CLOSE, IDB_WINDOW_CLOSE);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_WINDOW_CHANGE_PANE, IDB_WINDOW_CHANGEPANE);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_EDIT_WMGOTO, IDB_EDIT_GOTO);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_EDIT_REPLACE, IDB_EDIT_REPLACE);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_VIEW_LANGUAGE, IDB_VIEW_LANGUAGE);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_VIEW_SELECTFONT, IDB_VIEW_SELECTFONT);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_APP_EXIT, IDB_FILE_EXIT);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_HELP_CONTENTS, IDB_HELP_CONTENTS);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_EDIT_SELECT_ALL, IDB_EDIT_SELECTALL);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_TOOLS_FILTERS, IDB_TOOLS_FILTERS);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_TOOLS_CUSTOMIZECOLUMNS, IDB_TOOLS_COLUMNS);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_FILE_PRINT, IDB_FILE_PRINT);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_TOOLS_GENERATEPATCH, IDB_TOOLS_GENERATEPATCH);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_EDIT_TOGGLE_BOOKMARK, IDB_EDIT_TOGGLE_BOOKMARK);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_EDIT_GOTO_NEXT_BOOKMARK, IDB_EDIT_GOTO_NEXT_BOOKMARK);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_EDIT_GOTO_PREV_BOOKMARK, IDB_EDIT_GOTO_PREV_BOOKMARK);
	m_pMenus[MENU_MERGEVIEW]->ModifyODMenu(NULL, ID_EDIT_CLEAR_ALL_BOOKMARKS, IDB_EDIT_CLEAR_ALL_BOOKMARKS);
	m_pMenus[MENU_MERGEVIEW]->LoadToolbar(IDR_MAINFRAME);

	FixupDebugMenu(m_pMenus[MENU_MERGEVIEW]);

	return(m_pMenus[MENU_MERGEVIEW]->Detach());
}

/**
 * @brief Create new Dir compare (CDirView) menu
 */
HMENU CMainFrame::NewDirViewMenu()
{
	if (m_pMenus[MENU_DIRVIEW] == NULL)
		m_pMenus[MENU_DIRVIEW] = new BCMenu();
	if (m_pMenus[MENU_DIRVIEW] == NULL)
		return NULL;

	m_pMenus[MENU_DIRVIEW]->LoadMenu(IDR_DIRDOCTYPE);
	// Load bitmaps to menuitems
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_EDIT_COPY, IDB_EDIT_COPY);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_EDIT_CUT, IDB_EDIT_CUT);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_EDIT_PASTE, IDB_EDIT_PASTE);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_EDIT_FIND, IDB_EDIT_SEARCH);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_WINDOW_CASCADE, IDB_WINDOW_CASCADE);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_WINDOW_TILE_HORZ, IDB_WINDOW_HORIZONTAL);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_WINDOW_TILE_VERT, IDB_WINDOW_VERTICAL);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_FILE_CLOSE, IDB_WINDOW_CLOSE);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_WINDOW_CHANGE_PANE, IDB_WINDOW_CHANGEPANE);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_EDIT_WMGOTO, IDB_EDIT_GOTO);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_EDIT_REPLACE, IDB_EDIT_REPLACE);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_VIEW_LANGUAGE, IDB_VIEW_LANGUAGE);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_VIEW_SELECTFONT, IDB_VIEW_SELECTFONT);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_APP_EXIT, IDB_FILE_EXIT);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_HELP_CONTENTS, IDB_HELP_CONTENTS);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_EDIT_SELECT_ALL, IDB_EDIT_SELECTALL);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_TOOLS_FILTERS, IDB_TOOLS_FILTERS);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_TOOLS_CUSTOMIZECOLUMNS, IDB_TOOLS_COLUMNS);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_MERGE_DELETE, IDB_MERGE_DELETE);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_TOOLS_GENERATEPATCH, IDB_TOOLS_GENERATEPATCH);
	m_pMenus[MENU_DIRVIEW]->ModifyODMenu(NULL, ID_TOOLS_GENERATEREPORT, IDB_TOOLS_GENERATEREPORT);

	m_pMenus[MENU_DIRVIEW]->LoadToolbar(IDR_MAINFRAME);

	FixupDebugMenu(m_pMenus[MENU_DIRVIEW]);

	return(m_pMenus[MENU_DIRVIEW]->Detach());
}

/**
 * @brief This handler ensures that the popup menu items are drawn correctly.
 */
void CMainFrame::OnMeasureItem(int nIDCtl,
	LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	BOOL setflag = FALSE;
	if (lpMeasureItemStruct->CtlType == ODT_MENU)
	{
		if (IsMenu((HMENU)lpMeasureItemStruct->itemID))
		{
			CMenu* cmenu =
				CMenu::FromHandle((HMENU)lpMeasureItemStruct->itemID);

			if (m_pMenus[MENU_DEFAULT]->IsMenu(cmenu))
			{
				m_pMenus[MENU_DEFAULT]->MeasureItem(lpMeasureItemStruct);
				setflag = TRUE;
			}
		}
	}

	if (!setflag)
		CMDIFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

/**
 * @brief This handler ensures that keyboard shortcuts work.
 */
LRESULT CMainFrame::OnMenuChar(UINT nChar, UINT nFlags, 
	CMenu* pMenu) 
{
	LRESULT lresult;
	if(m_pMenus[MENU_DEFAULT]->IsMenu(pMenu))
		lresult=BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	else
		lresult=CMDIFrameWnd::OnMenuChar(nChar, nFlags, pMenu);
	return(lresult);
}

/**
 * @brief This handler updates the menus from time to time.
 */
void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	if (!bSysMenu)
	{
		if (BCMenu::IsMenu(pPopupMenu))
			BCMenu::UpdateMenu(pPopupMenu);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnFileOpen() 
{
	DoFileOpen();
}

/**
 * @brief Check for BOM, and also, if bGuessEncoding, try to deduce codepage
 *
 * Unpacks info from FileLocation & delegates all work to codepage_detect module
 */
static void
FileLocationGuessEncodings(FileLocation & fileloc, BOOL bGuessEncoding)
{
	GuessCodepageEncoding(fileloc.filepath, &fileloc.encoding, bGuessEncoding);
}

/**
 * @brief Creates new MergeDoc instance and shows documents
 *
 * @param cpleft, cpright : left and right codepages
 * = -1 when the file must be parsed
 */
int /* really an OPENRESULTS_TYPE, but MainFrm.h doesn't know that type */
CMainFrame::ShowMergeDoc(CDirDoc * pDirDoc,  const FileLocation & ifilelocLeft, const FileLocation & ifilelocRight,
	BOOL bROLeft, BOOL bRORight,
	PackingInfo * infoUnpacker /*= NULL*/)
{
	BOOL docNull;
	BOOL bOpenSuccess = FALSE;
	CMergeDoc * pMergeDoc = GetMergeDocToShow(pDirDoc, &docNull);

	// Make local copies, so we can change encoding if we guess it below
	FileLocation filelocLeft = ifilelocLeft, filelocRight = ifilelocRight;

	ASSERT(pMergeDoc);		// must ASSERT to get an answer to the question below ;-)
	if (!pMergeDoc)
		return OPENRESULTS_FAILED_MISC; // when does this happen ?

	// if an unpacker is selected, it must be used during LoadFromFile
	// MergeDoc must memorize it for SaveToFile
	// Warning : this unpacker may differ from the pDirDoc one
	// (through menu : "Plugins"->"Open with unpacker")
	pMergeDoc->SetUnpacker(infoUnpacker);

	// detect codepage
	BOOL bGuessEncoding =GetOptionsMgr()->GetBool(OPT_CP_DETECT);
	if (filelocLeft.encoding.m_unicoding == -1)
		filelocLeft.encoding.m_unicoding = ucr::NONE;
	if (filelocLeft.encoding.m_unicoding == ucr::NONE && filelocLeft.encoding.m_codepage == -1)
	{
		FileLocationGuessEncodings(filelocLeft, bGuessEncoding);
	}
	if (filelocRight.encoding.m_unicoding == -1)
		filelocRight.encoding.m_unicoding = ucr::NONE;
	if (filelocRight.encoding.m_unicoding == ucr::NONE && filelocRight.encoding.m_codepage == -1)
	{
		FileLocationGuessEncodings(filelocRight, bGuessEncoding);
	}

	if (!IsUnicodeBuild())
	{
		// In ANSI (8-bit) build, character loss can occur in merging
		// if the two buffers use different encodings
		if (filelocLeft.encoding.m_unicoding == ucr::NONE
			&& filelocRight.encoding.m_unicoding == ucr::NONE
			&& filelocLeft.encoding.m_codepage != filelocRight.encoding.m_codepage)
		{
			CString msg;
			msg.Format(IDS_SUGGEST_IGNORECODEPAGE, filelocLeft.encoding.m_codepage, filelocRight.encoding.m_codepage);
			int msgflags = MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN;
			// Two files with different codepages
			// Warn and propose to use the default codepage for both
			int userChoice = AfxMessageBox(msg, msgflags);
			if (userChoice == IDYES)
			{
				if (filelocLeft.encoding.m_codepage != getDefaultCodepage())
				{
					filelocLeft.encoding.SetCodepage(getDefaultCodepage());
					filelocLeft.encoding.m_bom = false;
					filelocLeft.encoding.m_guessed = false;
				}
				if (filelocRight.encoding.m_codepage != getDefaultCodepage())
				{
					filelocRight.encoding.SetCodepage(getDefaultCodepage());
					filelocRight.encoding.m_bom = false;
					filelocRight.encoding.m_guessed = false;
				}
			}
		}
		else if (filelocLeft.encoding.m_unicoding != filelocRight.encoding.m_unicoding)
		{
			CString leftEncoding = filelocLeft.encoding.GetName();
			CString rightEnicoding = filelocRight.encoding.GetName();
			CString msg;
			msg.Format(IDS_DIFFERENT_UNICODINGS, leftEncoding, rightEnicoding);
			int msgflags = MB_OK | MB_ICONWARNING | MB_DONT_ASK_AGAIN;
			// Two files with different codepages
			// Warn and propose to use the default codepage for both
			AfxMessageBox(msg, msgflags);
		}
	}

	// Note that OpenDocs() takes care of closing compare window when needed.
	OPENRESULTS_TYPE openResults = pMergeDoc->OpenDocs(filelocLeft, filelocRight,
			bROLeft, bRORight);

	if (openResults == OPENRESULTS_SUCCESS)
	{
		if (docNull)
		{
			CWnd* pWnd = pMergeDoc->GetParentFrame();
			MDIActivate(pWnd);
		}
		else
			MDINext();
	}
	return openResults;
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

/**
 * @brief Show/Hide different files/directories
 */
void CMainFrame::OnOptionsShowDifferent() 
{
	bool val = m_options.GetBool(OPT_SHOW_DIFFERENT);
	m_options.SaveOption(OPT_SHOW_DIFFERENT, !val); // reverse
	RedisplayAllDirDocs();
}

/**
 * @brief Show/Hide identical files/directories
 */
void CMainFrame::OnOptionsShowIdentical() 
{
	bool val = m_options.GetBool(OPT_SHOW_IDENTICAL);
	m_options.SaveOption(OPT_SHOW_IDENTICAL, !val); // reverse
	RedisplayAllDirDocs();
}

/**
 * @brief Show/Hide left-only files/directories
 */
void CMainFrame::OnOptionsShowUniqueLeft() 
{
	bool val = m_options.GetBool(OPT_SHOW_UNIQUE_LEFT);
	m_options.SaveOption(OPT_SHOW_UNIQUE_LEFT, !val); // reverse
	RedisplayAllDirDocs();
}

/**
 * @brief Show/Hide right-only files/directories
 */
void CMainFrame::OnOptionsShowUniqueRight() 
{
	bool val = m_options.GetBool(OPT_SHOW_UNIQUE_RIGHT);
	m_options.SaveOption(OPT_SHOW_UNIQUE_RIGHT, !val); // reverse
	RedisplayAllDirDocs();
}

/**
 * @brief Show/Hide binary files
 */
void CMainFrame::OnOptionsShowBinaries()
{
	bool val = m_options.GetBool(OPT_SHOW_BINARIES);
	m_options.SaveOption(OPT_SHOW_BINARIES, !val); // reverse
	RedisplayAllDirDocs();
}

/**
 * @brief Show/Hide skipped files/directories
 */
void CMainFrame::OnOptionsShowSkipped()
{
	bool val = m_options.GetBool(OPT_SHOW_SKIPPED);
	m_options.SaveOption(OPT_SHOW_SKIPPED, !val); // reverse
	RedisplayAllDirDocs();
}

void CMainFrame::OnUpdateOptionsShowdifferent(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_options.GetBool(OPT_SHOW_DIFFERENT));
}

void CMainFrame::OnUpdateOptionsShowidentical(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_options.GetBool(OPT_SHOW_IDENTICAL));
}

void CMainFrame::OnUpdateOptionsShowuniqueleft(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_options.GetBool(OPT_SHOW_UNIQUE_LEFT));
}

void CMainFrame::OnUpdateOptionsShowuniqueright(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_options.GetBool(OPT_SHOW_UNIQUE_RIGHT));
}

void CMainFrame::OnUpdateOptionsShowBinaries(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_options.GetBool(OPT_SHOW_BINARIES));
}

void CMainFrame::OnUpdateOptionsShowSkipped(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_options.GetBool(OPT_SHOW_SKIPPED));
}

/**
 * @brief Show GNU licence information in notepad (local file) or in Web Browser
 */
void CMainFrame::OnHelpGnulicense() 
{
	CString spath = GetModulePath() + _T("\\Copying");
	CString url = _T("http://www.gnu.org/copyleft/gpl.html");
	
	OpenFileOrUrl(spath, url);
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
int CMainFrame::HandleReadonlySave(CString& strSavePath, BOOL bMultiFile,
		BOOL &bApplyToAll)
{
	CFileStatus status;
	UINT userChoice = 0;
	int nRetVal = IDOK;
	BOOL bFileRO = FALSE;
	BOOL bFileExists = FALSE;
	CString s;
	CString title;
	int nVerSys = 0;

	bFileRO = files_isFileReadOnly(strSavePath, &bFileExists);
	nVerSys = m_options.GetInt(OPT_VCS_SYSTEM);
	
	if (bFileExists && bFileRO)
	{
		// Version control system used?
		// Checkout file from VCS and modify, don't ask about overwriting
		// RO files etc.
		if (nVerSys != VCS_NONE)
		{
			BOOL bRetVal = SaveToVersionControl(strSavePath);
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
				AfxFormatString1(s, IDS_SAVEREADONLY_MULTI, strSavePath);
				userChoice = AfxMessageBox(s, MB_YESNOCANCEL |
						MB_ICONWARNING | MB_DEFBUTTON3 | MB_DONT_ASK_AGAIN |
						MB_YES_TO_ALL, IDS_SAVEREADONLY_MULTI);
			}
			else
			{
				// Single file
				AfxFormatString1(s, IDS_SAVEREADONLY_FMT, strSavePath);
				userChoice = AfxMessageBox(s, MB_YESNOCANCEL |
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
			CFile::GetStatus(strSavePath, status);
			status.m_mtime = 0;		// Avoid unwanted changes
			status.m_attribute &= ~CFile::Attribute::readOnly;
			CFile::SetStatus(strSavePath, status);
			nRetVal = IDYES;
			break;
		
		// Save to new filename (single) /skip this item (multiple)
		case IDNO:
			if (!bMultiFile)
			{
				VERIFY(title.LoadString(IDS_SAVE_AS_TITLE));
				if (SelectFile(s, strSavePath, title, NULL, FALSE))
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

/// Wrapper to set the global option 'm_bAllowMixedEol'
void CMainFrame::SetEOLMixed(BOOL bAllow)
{
	m_options.SaveOption(OPT_ALLOW_MIXED_EOL, bAllow == TRUE);
	ApplyViewWhitespace();
}

/**
 * @brief Opens Options-dialog and saves changed options
 */
void CMainFrame::OnOptions() 
{
	// Using singleton shared syntax colors
	CPreferencesDlg dlg(&m_options, m_pSyntaxColors);
	int rv = dlg.DoModal();

	if (rv == IDOK)
	{
		// Set new filterpath
		CString filterPath = m_options.GetString(OPT_FILTER_USERPATH);
		theApp.m_globalFileFilter.SetUserFilterPath(filterPath);

		UpdateCodepageModule();
		// Call the wrapper to set m_bAllowMixedEol (the wrapper updates the registry)
		SetEOLMixed(m_options.GetBool(OPT_ALLOW_MIXED_EOL));

		// make an attempt at rescanning any open diff sessions
		MergeDocList docs;
		GetAllMergeDocs(&docs);
		while (!docs.IsEmpty())
		{
			CMergeDoc * pMergeDoc = docs.RemoveHead();
			CMergeEditView * pLeft = pMergeDoc->GetLeftView();
			CMergeEditView * pRight = pMergeDoc->GetRightView();

			// Re-read MergeDoc settings (also updates view settings)
			// and rescan using new options
			pMergeDoc->RefreshOptions();
			pMergeDoc->FlushAndRescan(TRUE);
		}

		// Update all dirdoc settings
		DirDocList dirDocs;
		GetAllDirDocs(&dirDocs);

		while (!dirDocs.IsEmpty())
		{
			CDirDoc *pDirDoc = dirDocs.RemoveHead();
			pDirDoc->RefreshOptions();
		}
	}
}

/**
 * @brief Begin a diff: open dirdoc if it is directories, else open a mergedoc for editing
 */
BOOL CMainFrame::DoFileOpen(LPCTSTR pszLeft /*=NULL*/, LPCTSTR pszRight /*=NULL*/,
	DWORD dwLeftFlags /*=0*/, DWORD dwRightFlags /*=0*/, BOOL bRecurse /*=FALSE*/, CDirDoc *pDirDoc/*=NULL*/,
	CString prediffer /*=_T("")*/)
{
	// If the dirdoc we are supposed to use is busy doing a diff, bail out
	if (IsComparing())
		return FALSE;

	if (pDirDoc && !pDirDoc->CloseMergeDocs())
		return FALSE;

	CString strLeft = pszLeft;
	CString strRight = pszRight;
	PackingInfo infoUnpacker;

	BOOL bROLeft = dwLeftFlags & FFILEOPEN_READONLY;
	BOOL bRORight = dwRightFlags & FFILEOPEN_READONLY;
	// jtuc: docNull used to be uninitialized so you couldn't tell whether
	// pDirDoc->ReusingDirDoc() would be called for passed-in pDirDoc.
	// However, pDirDoc->ReusingDirDoc() kills temp path contexts, and I
	// need to avoid that. This is why I'm initializing docNull to TRUE here.
	// Note that call to pDirDoc->CloseMergeDocs() above preserves me from
	// keeping orphaned MergeDocs in that case.
	BOOL docNull = TRUE;

	// pop up dialog unless arguments exist (and are compatible)
	PATH_EXISTENCE pathsType = GetPairComparability(strLeft, strRight);
	if (pathsType == DOES_NOT_EXIST)
	{
		COpenDlg dlg;
		dlg.m_strLeft = strLeft;
		dlg.m_strRight = strRight;
		dlg.m_bRecurse = bRecurse;

		if (dwLeftFlags & FFILEOPEN_PROJECT || dwRightFlags & FFILEOPEN_PROJECT)
			dlg.m_bOverwriteRecursive = TRUE; // Use given value, not previously used value
		if (dwLeftFlags & FFILEOPEN_CMDLINE || dwRightFlags & FFILEOPEN_CMDLINE)
			dlg.m_bOverwriteRecursive = TRUE; // Use given value, not previously used value

		if (dlg.DoModal() != IDOK)
			return FALSE;

		strLeft = dlg.m_strLeft;
		strRight = dlg.m_strRight;
		bRecurse = dlg.m_bRecurse;
		infoUnpacker = dlg.m_infoHandler;
		if (dlg.m_pProjectFile != NULL)
		{
			// User loaded a project file, set additional information
			if (dlg.m_pProjectFile->GetLeftReadOnly())
				bROLeft = TRUE;
			if (dlg.m_pProjectFile->GetRightReadOnly())
				bRORight = TRUE;
			// Set value in both cases because we want to override Open-dialog
			// value.
			int projRecurse = dlg.m_pProjectFile->GetSubfolders();
			if (projRecurse == 0)
				bRecurse = FALSE;
			else if (projRecurse > 0)
				bRecurse = TRUE;
		}
		pathsType = static_cast<PATH_EXISTENCE>(dlg.m_pathsType);
		// TODO: add codepage options to open dialog ?
	}
	else
	{
		// Add trailing '\' for directories if its missing
		if (pathsType == IS_EXISTING_DIR)
		{
			if (!paths_EndsWithSlash(strLeft))
				strLeft += '\\';
			if (!paths_EndsWithSlash(strRight))
				strRight += '\\';
		}

		//save the MRU left and right files.
		if (!(dwLeftFlags & FFILEOPEN_NOMRU))
			addToMru(strLeft, _T("Files\\Left"));
		if (!(dwRightFlags & FFILEOPEN_NOMRU))
			addToMru(strRight, _T("Files\\Right"));
	}


	if (1)
	{
		gLog.Write(_T("### Begin Comparison Parameters #############################\r\n")
				  _T("\tLeft: %s\r\n")
				  _T("\tRight: %s\r\n")
				  _T("\tRecurse: %d\r\n")
				  _T("### End Comparison Parameters #############################\r\n"),
				  strLeft,
				  strRight,
				  bRecurse);
	}

	CTempPathContext *pTempPathContext = NULL;
	try
	{
		// Handle archives using 7-zip
		if (Merge7z::Format *piHandler = ArchiveGuessFormat(strLeft))
		{
			pTempPathContext = new CTempPathContext;
			CString path = GetClearTempPath(pTempPathContext, _T("0"));
			pTempPathContext->m_strLeftDisplayRoot = strLeft;
			pTempPathContext->m_strRightDisplayRoot = strRight;
			pathsType = IS_EXISTING_DIR;
			if (strRight == strLeft)
			{
				strRight.Empty();
			}
			do
			{
				if FAILED(piHandler->DeCompressArchive(m_hWnd, strLeft, path))
					break;
				if (strLeft.Find(path) == 0)
				{
					VERIFY(::DeleteFile(strLeft) || gLog::DeleteFileFailed(strLeft));
				}
				SysFreeString(Assign(strLeft, piHandler->GetDefaultName(m_hWnd, strLeft)));
				strLeft.Insert(0, '\\');
				strLeft.Insert(0, path);
			} while (piHandler = ArchiveGuessFormat(strLeft));
			strLeft = path;
			if (Merge7z::Format *piHandler = ArchiveGuessFormat(strRight))
			{
				path = GetClearTempPath(pTempPathContext, _T("1"));
				do
				{
					if FAILED(piHandler->DeCompressArchive(m_hWnd, strRight, path))
						break;;
					if (strRight.Find(path) == 0)
					{
						VERIFY(::DeleteFile(strRight) || gLog::DeleteFileFailed(strRight));
					}
					SysFreeString(Assign(strRight, piHandler->GetDefaultName(m_hWnd, strRight)));
					strRight.Insert(0, '\\');
					strRight.Insert(0, path);
				} while (piHandler = ArchiveGuessFormat(strRight));
				strRight = path;
			}
			else if (strRight.IsEmpty())
			{
				// assume Perry style patch
				strRight = path;
				strLeft += _T("\\ORIGINAL");
				strRight += _T("\\ALTERED");
				if (!PathFileExists(strLeft) || !PathFileExists(strRight))
				{
					// not a Perry style patch: diff with itself...
					strLeft = strRight = path;
				}
				else
				{
					pTempPathContext->m_strLeftDisplayRoot += _T("\\ORIGINAL");
					pTempPathContext->m_strRightDisplayRoot += _T("\\ALTERED");
				}
			}
		}
	}
	catch (CException *e)
	{
		e->ReportError(MB_ICONSTOP);
		e->Delete();
	}

	// Determine if we want new a dirview open now that we know if it was
	// and archive. Don't open new dirview if we are comparing files.
	if (!pDirDoc)
	{
		if (pathsType == IS_EXISTING_DIR)
		{
			pDirDoc = GetDirDocToShow(&docNull);
		}
		else
		{
			pDirDoc = (CDirDoc*)theApp.m_pDirTemplate->CreateNewDocument();
			docNull = TRUE;
		}
	}

	if (!docNull)
	{
		// If reusing an existing doc, give it a chance to save its data
		// and close any merge views, and clear its window
		if (!pDirDoc->ReusingDirDoc())
			return FALSE;
	}
	
	// open the diff
	if (pathsType == IS_EXISTING_DIR)
	{
		if (pDirDoc)
		{
			PathContext paths(strLeft, strRight);
			// Anything that can go wrong inside InitCompare() will yield an
			// exception. There is no point in checking return value.
			pDirDoc->InitCompare(paths, bRecurse, pTempPathContext);
			gLog.Write(LOGLEVEL::LNOTICE, _T("Open dirs: Left: %s\n\tRight: %s."),
				strLeft, strRight);

			pDirDoc->SetReadOnly(TRUE, bROLeft);
			pDirDoc->SetReadOnly(FALSE, bRORight);
			pDirDoc->SetDescriptions(m_strLeftDesc, m_strRightDesc);
			pDirDoc->SetTitle(NULL);
			m_strLeftDesc.Empty();
			m_strRightDesc.Empty();

			pDirDoc->Rescan();
		}
	}
	else
	{
		gLog.Write(LOGLEVEL::LNOTICE, _T("Open files: Left: %s\n\tRight: %s."),
			strLeft, strRight);
		
		FileLocation filelocLeft(strLeft);
		FileLocation filelocRight(strRight);

		if (!prediffer.IsEmpty())
		{
			CString strBothFilenames = strLeft + _T("|") + strRight;
			pDirDoc->SetPluginPrediffer(strBothFilenames, prediffer);
		}

		ShowMergeDoc(pDirDoc, filelocLeft, filelocRight, bROLeft, bRORight,
			&infoUnpacker);
	}
	return TRUE;
}

/// Creates backup before file is saved over
BOOL CMainFrame::CreateBackup(LPCTSTR pszPath)
{
	// first, make a backup copy of the original
	// create backup copy of file if destination file exists
	if (m_options.GetBool(OPT_CREATE_BACKUPS) 
		&& paths_DoesPathExist(pszPath) == IS_EXISTING_FILE)
	{
		// Add backup extension if pathlength allows it
		BOOL success = TRUE;
		CString s = pszPath;
		if (s.GetLength() >= (MAX_PATH - _tcslen(BACKUP_FILE_EXT)))
			success = FALSE;
		else
			s += BACKUP_FILE_EXT;

		if (success)
			success = CopyFile(pszPath, s, FALSE);
		
		if (!success)
		{
			if (ResMsgBox1(IDS_BACKUP_FAILED_PROMPT, pszPath,
					MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN, 
					IDS_BACKUP_FAILED_PROMPT) != IDYES)
				return FALSE;
		}
		return TRUE;
	}

	// we got here because we're either not backing up of there was nothing to backup
	return TRUE;
}

/**
 * @brief Trim trailing line returns.
 */
static void RemoveLineReturns(CString & str)
{
	str.Remove('\n');
	str.Remove('\r');
}

/**
 * @brief Sync file to Version Control System
 * @param pszSrc [in] File to copy
 * @param pszDest [in] Where to copy (incl. filename)
 * @param bApplyToAll [in,out] Apply user selection to all items
 * @param psError [out] Error string that can be shown to user in caller func
 * @return User selection or -1 if error happened
 * @sa CMainFrame::HandleReadonlySave()
 * @sa CDirView::PerformActionList()
 */
int CMainFrame::SyncFileToVCS(LPCTSTR pszSrc, LPCTSTR pszDest,
	BOOL &bApplyToAll, CString *psError)
{
	CString sActionError;
	CString strSavePath(pszDest);
	int nVerSys = 0;

	nVerSys = m_options.GetInt(OPT_VCS_SYSTEM);
	
	int nRetVal = HandleReadonlySave(strSavePath, TRUE, bApplyToAll);
	if (nRetVal == IDCANCEL || nRetVal == IDNO)
		return nRetVal;
	
	if (!CreateBackup(strSavePath))
	{
		psError->LoadString(IDS_ERROR_BACKUP);
		return -1;
	}
	
	// If VC project opened from VSS sync and version control used
	if ((nVerSys == VCS_VSS4 || nVerSys == VCS_VSS5) && m_bVCProjSync)
	{
		if (!m_vssHelper.ReLinkVCProj(strSavePath, psError))
			nRetVal = -1;
	}
	return nRetVal;
}

/**
 * @brief Select font for Merge/Dir view
 * 
 * Shows font selection dialog to user, sets current font and saves
 * selected font properties to registry. Selects fon type to active
 * view (Merge/Dir compare). If there is no open views, then font
 * is selected for Merge view (for example user may want to change to
 * unicode font before comparing files).
 */
void CMainFrame::OnViewSelectfont() 
{
	const TCHAR fileFontPath[] = _T("Font");
	const TCHAR dirFontPath[] = _T("FontDirCompare");
	CString sFontPath = fileFontPath; // Default to change file compare font

	CFrameWnd * pFrame = GetActiveFrame();
	BOOL bMergeFrame = pFrame->IsKindOf(RUNTIME_CLASS(CChildFrame));
	BOOL bDirFrame = pFrame->IsKindOf(RUNTIME_CLASS(CDirFrame));

	if (bDirFrame)
		sFontPath = dirFontPath;

	CHOOSEFONT cf;
	LOGFONT *lf = NULL;
	ZeroMemory(&cf, sizeof(CHOOSEFONT));
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.Flags = CF_INITTOLOGFONTSTRUCT|CF_FORCEFONTEXIST|CF_SCREENFONTS;
	if (!bDirFrame)
		cf.Flags |= CF_FIXEDPITCHONLY; // Only fixed-width fonts for merge view

	// CF_FIXEDPITCHONLY = 0x00004000L
	// in case you are a developer and want to disable it to test with, eg, a Chinese capable font
	if (bDirFrame)
		lf = &m_lfDir;
	else
		lf = &m_lfDiff;

	cf.lpLogFont = lf;

	if (ChooseFont(&cf))
	{
		if (bDirFrame)
			m_options.SaveOption(OPT_FONT_DIRCMP_USECUSTOM, true);
		else
			m_options.SaveOption(OPT_FONT_FILECMP_USECUSTOM, true);

		AppSerialize appser(AppSerialize::Save, sFontPath);
		appser.SerializeFont(_T(""), *lf); // unnamed font

		if (bDirFrame)
			m_lfDir = *lf;
		else
			m_lfDiff = *lf;

		// TODO: Update document fonts
		/*
		for (POSITION pos = editViews.GetHeadPosition(); pos; editViews.GetNext(pos))
		{
			CMergeEditView * pEditView = editViews.GetAt(pos);
			// update pEditView for font change
		}
		*/

		ShowFontChangeMessage();
	}
}

/**
 * @brief Enable 'Select font'.
 */
void CMainFrame::OnUpdateViewSelectfont(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/**
 * @brief Selects font for Merge view.
 *
 * Cheks if default font or user-selected font should be used in
 * Merge or dir -view and sets correct font properties. Loads user-selected
 * font properties from registry if needed.
 */
void CMainFrame::GetFontProperties()
{
	USES_CONVERSION;

	LOGFONT lfDefault;
	ZeroMemory(&lfDefault, sizeof(LOGFONT));

	MIMECPINFO cpi = {0};
	cpi.bGDICharset = ANSI_CHARSET;
	wcscpy(cpi.wszFixedWidthFont, L"Courier New");
	IMultiLanguage *pMLang = NULL;

	HRESULT hr = CoCreateInstance(CLSID_CMultiLanguage, NULL,
		CLSCTX_INPROC_SERVER, IID_IMultiLanguage, (void **)&pMLang);
	if (SUCCEEDED(hr))
	{
		hr = pMLang->GetCodePageInfo(GetACP(), &cpi);
		pMLang->Release();
	}

	lfDefault.lfHeight = -16;
	lfDefault.lfWidth = 0;
	lfDefault.lfEscapement = 0;
	lfDefault.lfOrientation = 0;
	lfDefault.lfWeight = FW_NORMAL;
	lfDefault.lfItalic = FALSE;
	lfDefault.lfUnderline = FALSE;
	lfDefault.lfStrikeOut = FALSE;
	lfDefault.lfCharSet = cpi.bGDICharset;
	lfDefault.lfOutPrecision = OUT_STRING_PRECIS;
	lfDefault.lfClipPrecision = CLIP_STROKE_PRECIS;
	lfDefault.lfQuality = DRAFT_QUALITY;
	lfDefault.lfPitchAndFamily = FF_MODERN | FIXED_PITCH;
	_tcscpy(lfDefault.lfFaceName, W2T(cpi.wszFixedWidthFont));
	LOGFONT lfnew;
	ZeroMemory(&lfnew, sizeof(LOGFONT));

	// Get MergeView font
	if (m_options.GetBool(OPT_FONT_FILECMP_USECUSTOM))
	{
		AppSerialize appser(AppSerialize::Load, _T("Font"));
		appser.SerializeFont(_T(""), lfnew); // unnamed font 

		m_lfDiff = lfnew;
	}
	else
		m_lfDiff = lfDefault;

	// Get DirView font
	ZeroMemory(&lfnew, sizeof(LOGFONT));
	if (m_options.GetBool(OPT_FONT_DIRCMP_USECUSTOM))
	{
		AppSerialize appser(AppSerialize::Load, _T("FontDirCompare"));
		appser.SerializeFont(_T(""), lfnew); // unnamed font 

		m_lfDir = lfnew;
	}
	else
		m_lfDir = lfDefault;
}

/**
 * @brief Use default font for active view type
 *
 * Disable user-selected font for active view type (Merge/Dir compare).
 * If there is no open views, then Merge view font is changed.
 */
void CMainFrame::OnViewUsedefaultfont() 
{
	CFrameWnd * pFrame = GetActiveFrame();
	BOOL bDirFrame = pFrame->IsKindOf(RUNTIME_CLASS(CDirFrame));

	if (bDirFrame)
		m_options.SaveOption(OPT_FONT_DIRCMP_USECUSTOM, false);
	else
		m_options.SaveOption(OPT_FONT_FILECMP_USECUSTOM, false);

	GetFontProperties();
	ShowFontChangeMessage();
}

/**
 * @brief Enable 'Use Default font'.
 */
void CMainFrame::OnUpdateViewUsedefaultfont(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
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
		pDoc->UpdateResources();
	}
}

BOOL CMainFrame::IsComparing()
{
	DirDocList dirdocs;
	GetAllDirDocs(&dirdocs);
	while (!dirdocs.IsEmpty())
	{
		CDirDoc * pDirDoc = dirdocs.RemoveHead();
		UINT threadState = pDirDoc->m_diffThread.GetThreadState();
		if (threadState == THREAD_COMPARING)
			return TRUE;
	}
	return FALSE;
}

/**
 * @brief Open file, if it exists, else open url
 */
void CMainFrame::OpenFileOrUrl(LPCTSTR szFile, LPCTSTR szUrl)
{
	if (paths_DoesPathExist(szFile) == IS_EXISTING_FILE)
		ShellExecute(m_hWnd, _T("open"), _T("notepad.exe"), szFile, NULL, SW_SHOWNORMAL);
	else
		ShellExecute(NULL, _T("open"), szUrl, NULL, NULL, SW_SHOWNORMAL);
}

/**
 * @brief Open help contents.
 *
 * If local HTMLhelp file is found, open it, otherwise open HTML page from web.
 */
void CMainFrame::OnHelpContents()
{
	CString sPath = GetModulePath(0) + DocsPath;
	if (paths_DoesPathExist(sPath) == IS_EXISTING_FILE)
		::HtmlHelp(GetSafeHwnd(), sPath, HH_DISPLAY_TOC, NULL);
	else
		ShellExecute(NULL, _T("open"), DocsURL, NULL, NULL, SW_SHOWNORMAL);
}

/**
 * @brief Enable Open help contents -menuitem.
 */
void CMainFrame::OnUpdateHelpContents(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/**
 * @brief Open help index.
 *
 * If local HTMLhelp file is found, open it, otherwise open HTML page from web.
 */
void CMainFrame::OnHelpIndex()
{
	CString sPath = GetModulePath(0) + DocsPath;
	if (paths_DoesPathExist(sPath) == IS_EXISTING_FILE)
		::HtmlHelp(GetSafeHwnd(), sPath, HH_DISPLAY_INDEX, NULL);
	else
		ShellExecute(NULL, _T("open"), DocsURL, NULL, NULL, SW_SHOWNORMAL);
}

/**
 * @brief Disable Open help index -menuitem.
 */
void CMainFrame::OnUpdateHelpIndex(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/**
 * @brief Open help search.
 *
 * If local HTMLhelp file is found, open it, otherwise open HTML page from web.
 */
void CMainFrame::OnHelpSearch()
{
	CString sPath = GetModulePath(0) + DocsPath;
	if (paths_DoesPathExist(sPath) == IS_EXISTING_FILE)
	{
		HH_FTS_QUERY q = {0};
		q.fExecute = TRUE;
		q.fTitleOnly = FALSE;
		q.fUniCodeStrings = TRUE;
		q.fStemmedSearch = FALSE;
		q.pszSearchQuery = _T("");
		q.pszWindow = NULL;
		q.cbStruct = sizeof(q);

		::HtmlHelp(GetSafeHwnd(), sPath, HH_DISPLAY_SEARCH, (DWORD)&q);
	}
	else
		ShellExecute(NULL, _T("open"), DocsURL, NULL, NULL, SW_SHOWNORMAL);
}

/**
 * @brief Enable Open help search -menuitem.
 */
void CMainFrame::OnUpdateHelpSearch(CCmdUI* pCmdUI) 
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
	if (nCmdShow != SW_MINIMIZE && theApp.GetProfileInt(_T("Settings"), _T("MainMax"), FALSE))
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
			// Allow user to cancel closing
			if (!pMergeDoc->PromptAndSaveIfNeeded(TRUE))
				return;
			else
			{
				// Set modified status to false so that we are not asking
				// about saving again. 
				pMergeDoc->m_ptBuf[0]->SetModified(FALSE);
				pMergeDoc->m_ptBuf[1]->SetModified(FALSE);
			}
		}
	}

	// Save last selected filter
	CString filter = theApp.m_globalFileFilter.GetFilterNameOrMask();
	m_options.SaveOption(OPT_FILEFILTER_CURRENT, filter);

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
	// don't call SavePosition, it is called when the child frame is destroyed
	/*
	while (!mergedocs.IsEmpty())
	{
		CMergeDoc * pMergeDoc = mergedocs.RemoveHead();
		CMergeEditView * pLeft = pMergeDoc->GetLeftView();
		if (pLeft)
			pMergeDoc->GetParentFrame()->SavePosition();
	}
	*/
	
	// Stop handling status messages from CustomStatusCursors
	CustomStatusCursor::SetStatusDisplay(0);
	myStatusDisplay.SetFrame(0);
	
	CMDIFrameWnd::OnClose();
}

/// Empty regexp list used internally
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

/**
 * @brief Add regexps from options to internal list
 * @param [in] bShowError When TRUE error messages are shown to user,
 * otherwise just written to log.
 */
void CMainFrame::RebuildRegExpList(BOOL bShowError)
{
	USES_CONVERSION;

	TCHAR tmp[_MAX_PATH] = {0};
	TCHAR tokenStr[_MAX_PATH] = {0};
	TCHAR* token;
	TCHAR sep[] = _T("\r\n");
	BOOL valid = TRUE;
	
	// destroy the old list if the it is not NULL
	FreeRegExpList();

	// build the new list if the user choose to
	// ignore lines matching the reg expression patterns
	if (m_options.GetBool(OPT_LINEFILTER_ENABLED))
	{
		// find each regular expression and add to list
		_tcsncpy(tmp, m_options.GetString(OPT_LINEFILTER_REGEXP), _MAX_PATH);

		token = _tcstok(tmp, sep);
		while (token && valid)
		{
			valid = add_regexp(&ignore_regexp_list, T2A(token), bShowError);
			token = _tcstok(NULL, sep);
		}
	}

	if (ignore_regexp_list)
	{
		ignore_some_changes = 1;
	}
}

/// Add the compiled form of regexp pattern to reglist
static BOOL add_regexp(struct regexp_list **reglist, char const* pattern, BOOL bShowError)
{
	struct regexp_list *r;
	int m;
	BOOL ret = FALSE;

	r = (struct regexp_list *) malloc (sizeof (*r));
	if (r)
	{
		bzero (r, sizeof (*r));
		r->buf.fastmap = (char*) malloc (256);
		if (r->buf.fastmap)
		{
			m = re_compile_pattern (pattern, strlen (pattern), &r->buf);

			if (m > 0)
			{
				CString msg;
				CString errMsg;
				VERIFY(errMsg.LoadString(IDS_REGEXP_ERROR));
				errMsg += _T(":\n\n");
				errMsg += pattern;
				errMsg += _T("\n\n");
				int errID = IDS_REGEXP_ERROR + m;
				VERIFY(msg.LoadString(errID));
				errMsg += msg;
				LogErrorString(errMsg);
				if (bShowError)
					AfxMessageBox(errMsg, MB_ICONWARNING);
			}

			/* Add to the start of the list, since it's easier than the end.  */
			r->next = *reglist;
			*reglist = r;
			ret = TRUE;
		}
		else
		{
			free(r->buf.fastmap);
			r->buf.fastmap = NULL;
		}
	}
	return ret;
}

/**
 * @brief Utility function to update CSuperComboBox format MRU
 */
void CMainFrame::addToMru(LPCTSTR szItem, LPCTSTR szRegSubKey, UINT nMaxItems)
{
	CString s,s2;
	UINT cnt = AfxGetApp()->GetProfileInt(szRegSubKey, _T("Count"), 0);
	++cnt;	// add new string
	if(cnt>nMaxItems)
	{
		cnt=nMaxItems;
	}
	// move items down a step
	for (UINT i=cnt ; i!=0; --i)
	{
		s2.Format(_T("Item_%d"), i-1);
		s = AfxGetApp()->GetProfileString(szRegSubKey, s2);
		s2.Format(_T("Item_%d"), i);
		AfxGetApp()->WriteProfileString(szRegSubKey, s2, s);
	}
	// add most recent item
	AfxGetApp()->WriteProfileString(szRegSubKey, _T("Item_0"), szItem);
	// update count
	AfxGetApp()->WriteProfileInt(szRegSubKey, _T("Count"), cnt);
}

/**
 * @brief Apply tabs and eols settings to all merge documents
 */
void CMainFrame::ApplyViewWhitespace() 
{
	MergeDocList mergedocs;
	GetAllMergeDocs(&mergedocs);
	while (!mergedocs.IsEmpty())
	{
		CMergeDoc * pMergeDoc = mergedocs.RemoveHead();
		CMergeEditView * pLeft = pMergeDoc->GetLeftView();
		CMergeEditView * pRight = pMergeDoc->GetRightView();
		CMergeDiffDetailView * pLeftDetail = pMergeDoc->GetLeftDetailView();
		CMergeDiffDetailView * pRightDetail = pMergeDoc->GetRightDetailView();
		if (pLeft)
		{
			pLeft->SetViewTabs(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE));
			pLeft->SetViewEols(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE),
				GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL));
		}
		if (pRight)
		{
			pRight->SetViewTabs(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE));
			pRight->SetViewEols(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE),
				GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL));
		}
		if (pLeftDetail)
		{
			pLeftDetail->SetViewTabs(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE));
			pLeftDetail->SetViewEols(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE),
				GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL));
		}
		if (pRightDetail)
		{
			pRightDetail->SetViewTabs(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE));
			pRightDetail->SetViewEols(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE),
				GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL));
		}
	}
}

void CMainFrame::OnViewWhitespace() 
{
	bool bViewWhitespace = m_options.GetBool(OPT_VIEW_WHITESPACE);
	m_options.SaveOption(OPT_VIEW_WHITESPACE, !bViewWhitespace);
	ApplyViewWhitespace();
}

/// Enables View/View Whitespace menuitem when merge view is active
void CMainFrame::OnUpdateViewWhitespace(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_options.GetBool(OPT_VIEW_WHITESPACE));
}

/// Get list of MergeDocs (documents underlying edit sessions)
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

/// Get list of DirDocs (documents underlying a scan)
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

/// Get list of all dirviews
void CMainFrame::GetDirViews(DirViewList * pDirViews)
{
	GetAllViews(NULL, NULL, pDirViews);
}

/// Get list of all merge edit views
void CMainFrame::GetMergeEditViews(MergeEditViewList * pMergeViews)
{
	GetAllViews(pMergeViews, NULL, NULL);
}

/// Get pointers to all views into typed lists (both arguments are optional)
void CMainFrame::GetAllViews(MergeEditViewList * pEditViews, MergeDetailViewList * pDetailViews, DirViewList * pDirViews)
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
				// Don't get Location View (font don't change for this view)
				if (pView->IsKindOf(RUNTIME_CLASS(CLocationView)))
					continue;
				if (pMergeDoc)
				{
					if (pEditViews || pDetailViews)
					{
						// a merge doc only has merge edit views or diff detail views
						CMergeEditView * pEditView = dynamic_cast<CMergeEditView *>(pView);
						CMergeDiffDetailView * pDetailView = dynamic_cast<CMergeDiffDetailView *>(pView);
						ASSERT(pEditView || pDetailView);
						if (pEditView)
						{
							if (pEditViews)
								pEditViews->AddTail(pEditView);
						}
						else if (pDetailView)
						{
							if (pDetailViews)
								pDetailViews->AddTail(pDetailView);
						}
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

/// Obtain a merge doc to display a difference in files
CMergeDoc * CMainFrame::GetMergeDocToShow(CDirDoc * pDirDoc, BOOL * pNew)
{
	CMergeDoc * pMergeDoc = pDirDoc->GetMergeDocForDiff(pNew);
	return pMergeDoc;
}

/// Get pointer to a dir doc for displaying a scan
CDirDoc * CMainFrame::GetDirDocToShow(BOOL * pNew)
{
	CDirDoc * pDirDoc = 0;
	if (!m_options.GetBool(OPT_MULTIDOC_DIRDOCS))
	{
		POSITION pos = theApp.m_pDirTemplate->GetFirstDocPosition();
		while (pos)
		{			
			CDirDoc *pDirDocTemp = static_cast<CDirDoc *>(theApp.m_pDirTemplate->GetNextDoc(pos));
			if (pDirDocTemp->HasDirView())
			{
				*pNew = FALSE;
				pDirDoc = pDirDocTemp;
				break;
			}
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

/**
 * @brief Generate patch from files selected.
 *
 * Creates a patch from selected files in active directory compare, or
 * active file compare. Files in file compare must be saved before
 * creating a patch.
 */
void CMainFrame::OnToolsGeneratePatch()
{
	CPatchTool patcher;
	CFrameWnd * pFrame = GetActiveFrame();
	BOOL bOpenDialog = TRUE;

	// Mergedoc active?
	if (pFrame->IsKindOf(RUNTIME_CLASS(CChildFrame)))
	{
		CMergeDoc * pMergeDoc = (CMergeDoc *) pFrame->GetActiveDocument();
		// If there are changes in files, tell user to save them first
		if (pMergeDoc->m_ptBuf[0]->IsModified() || pMergeDoc->m_ptBuf[0]->IsModified())
		{
			bOpenDialog = FALSE;
			AfxMessageBox(IDS_SAVEFILES_FORPATCH, MB_ICONSTOP);
		}
		else
		{
			patcher.AddFiles(pMergeDoc->m_filePaths.GetLeft(),
					pMergeDoc->m_filePaths.GetRight());
		}
	}
	// Dirview active
	else if (pFrame->IsKindOf(RUNTIME_CLASS(CDirFrame)))
	{
		CDirDoc * pDoc = (CDirDoc*)pFrame->GetActiveDocument();
		CDirView *pView = pDoc->GetMainView();

		// Get selected items from folder compare
		BOOL bValidFiles = TRUE;
		int ind = pView->GetFirstSelectedInd();
		while (ind != -1 && bValidFiles)
		{
			const DIFFITEM item = pView->GetItemAt(ind);
			if (item.isBin())
			{
				AfxMessageBox(IDS_CANNOT_CREATE_BINARYPATCH, MB_ICONWARNING |
					MB_DONT_DISPLAY_AGAIN, IDS_CANNOT_CREATE_BINARYPATCH);
				bValidFiles = FALSE;
			}
			else if (item.isDirectory())
			{
				AfxMessageBox(IDS_CANNOT_CREATE_DIRPATCH, MB_ICONWARNING |
					MB_DONT_DISPLAY_AGAIN, IDS_CANNOT_CREATE_DIRPATCH);
				bValidFiles = FALSE;
			}

			if (bValidFiles)
			{
				// Format full paths to files (leftFile/rightFile)
				CString leftFile = item.getLeftFilepath(pDoc->GetLeftBasePath());
				if (!leftFile.IsEmpty())
					leftFile += _T("\\") + item.sLeftFilename;
				CString rightFile = item.getRightFilepath(pDoc->GetRightBasePath());
				if (!rightFile.IsEmpty())
					rightFile += _T("\\") + item.sRightFilename;

				// Format relative paths to files in folder compare
				CString leftpatch = item.sLeftSubdir;
				if (!leftpatch.IsEmpty())
					leftpatch += _T("/");
				leftpatch += item.sLeftFilename;
				CString rightpatch = item.sRightSubdir;
				if (!rightpatch.IsEmpty())
					rightpatch += _T("/");
				rightpatch += item.sRightFilename;
				patcher.AddFiles(leftFile, leftpatch, rightFile, rightpatch);
				pView->GetNextSelectedInd(ind);
			}
		}
	}

	if (bOpenDialog)
	{
		if (patcher.CreatePatch())
		{
			if (patcher.GetOpenToEditor())
			{
				OpenFileToExternalEditor(patcher.GetPatchFile());
			}
		}
	}
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

	// If user has <Shift> pressed with one file selected,
	// assume it is an archive and set filenames to same
	if (::GetAsyncKeyState(VK_SHIFT) < 0 && fileCount == 1)
	{
		files[1] = files[0];
	}

	gLog.Write(LOGLEVEL::LNOTICE, _T("D&D open: Left: %s\n\tRight: %s."),
		files[0], files[1]);

	// Check if they dropped a project file
	if (wNumFilesDropped == 1 && theApp.IsProjectFile(files[0]))
	{
		theApp.LoadAndOpenProjectFile(files[0]);
		return;
	}

	DoFileOpen(files[0], files[1], FFILEOPEN_NONE, FFILEOPEN_NONE, ctrlKey);
}

BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (CustomStatusCursor::HasWaitCursor())
	{
		CustomStatusCursor::RestoreWaitCursor();
		return TRUE;
	}
	return CMDIFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CMainFrame::OnPluginUnpackMode(UINT nID )
{
	switch (nID)
	{
	case ID_UNPACK_MANUAL:
		g_bUnpackerMode = PLUGIN_MANUAL;
		break;
	case ID_UNPACK_AUTO:
		g_bUnpackerMode = PLUGIN_AUTO;
		break;
	}
	theApp.WriteProfileInt(_T("Settings"), _T("UnpackerMode"), g_bUnpackerMode);
}

void CMainFrame::OnUpdatePluginUnpackMode(CCmdUI* pCmdUI) 
{
	if (pCmdUI->m_nID == ID_UNPACK_MANUAL)
		pCmdUI->SetRadio(PLUGIN_MANUAL == g_bUnpackerMode);
	if (pCmdUI->m_nID == ID_UNPACK_AUTO)
		pCmdUI->SetRadio(PLUGIN_AUTO == g_bUnpackerMode);
}

/**
 * @brief Called when "Reload Plugins" item is updated
 */
void CMainFrame::OnUpdateReloadPlugins(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnReloadPlugins()
{
	// delete all script interfaces
	// (interfaces will be created again automatically when WinMerge needs them)
	CAllThreadsScripts::GetActiveSet()->FreeAllScripts();

	// update the editor scripts submenu
	HMENU scriptsSubmenu = GetScriptsSubmenu(m_hMenuDefault);
	if (scriptsSubmenu != NULL)
		CMergeEditView::createScriptsSubmenu(scriptsSubmenu);
	UpdatePrediffersMenu();
}

/** @brief Return active merge edit view, if can figure it out/is available */
CMergeEditView * CMainFrame::GetActiveMergeEditView()
{
	// NB: GetActiveDocument does not return the Merge Doc 
	//     even when the merge edit view is in front
	// NB: CChildFrame::GetActiveView returns NULL when location view active
	// So we have this rather complicated logic to try to get a merge edit view
	// We look at the front child window, which should be a frame
	// and we can get a MergeEditView from it, if it is a CChildFrame
	// (DirViews use a different frame type)
	CChildFrame * pFrame = dynamic_cast<CChildFrame *>(GetActiveFrame());
	if (!pFrame) return 0;
	// Try to get the active MergeEditView (ie, left or right)
	if (pFrame->GetActiveView() && pFrame->GetActiveView()->IsKindOf(RUNTIME_CLASS(CMergeEditView)))
	{
		return dynamic_cast<CMergeEditView *>(pFrame->GetActiveView());
	}
	return pFrame->GetMergeDoc()->GetActiveMergeView();
}

void CMainFrame::UpdatePrediffersMenu()
{
	CMenu* menu = GetMenu();
	if (menu == NULL)
	{
		return;
	}

	HMENU hMainMenu = menu->m_hMenu;
	HMENU prediffersSubmenu = GetPrediffersSubmenu(hMainMenu);
	if (prediffersSubmenu != NULL)
	{
		CMergeEditView * pEditView = GetActiveMergeEditView();
		if (pEditView)
			pEditView->createPrediffersSubmenu(prediffersSubmenu);
		else
		{
			// no view or dir view : display an empty submenu
			int i = GetMenuItemCount(prediffersSubmenu);
			while (i --)
				::DeleteMenu(prediffersSubmenu, 0, MF_BYPOSITION);
			::AppendMenu(prediffersSubmenu, MF_SEPARATOR, 0, NULL);
		}
	}
}

/**
 * @brief Open given file to external editor specified in options
 */
BOOL CMainFrame::OpenFileToExternalEditor(CString file)
{
	CString sExtEditor;
	CString ext;
	CString sExecutable;
	CString sCmd;
	
	sExtEditor = m_options.GetString(OPT_EXT_EDITOR_CMD);
	GetDecoratedCmdLine(sExtEditor, sCmd, sExecutable);

	SplitFilename(sExecutable, NULL, NULL, &ext);
	ext.MakeLower();

	if (ext == _T("exe") || ext == _T("cmd") || ext == ("bat"))
	{
		sCmd += _T(" \"") + file + _T("\"");

		BOOL retVal = FALSE;
		STARTUPINFO stInfo = {0};
		stInfo.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION processInfo;

		retVal = CreateProcess(NULL, (LPTSTR)(LPCTSTR) sCmd,
			NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
			&stInfo, &processInfo);

		if (!retVal)
		{
			// Error invoking external editor
			CString msg;
			AfxFormatString1(msg, IDS_ERROR_EXECUTE_FILE, sExtEditor);
			AfxMessageBox(msg, MB_ICONSTOP);
		}
	}
	else
	{
		// Don't know how to invoke external editor (it doesn't end with
		// an obvious executable extension)
		ResMsgBox1(IDS_UNKNOWN_EXECUTE_FILE, sExtEditor, MB_ICONSTOP);
	}
	return TRUE;
}

/**
 * @brief Get default editor path
 */
CString CMainFrame::GetDefaultEditor()
{
	CString path = paths_GetWindowsDirectory() + _T("\\NOTEPAD.EXE");
	return path;
}

typedef enum { ToConfigLog, FromConfigLog } ConfigLogDirection;

/**
 * @brief Copy one piece of data from options object to config log, or vice-versa
 */
static void
LoadConfigIntSetting(int * cfgval, COptionsMgr & options, const CString & name, ConfigLogDirection cfgdir)
{
	if (cfgdir == ToConfigLog)
	{
			*cfgval = options.GetInt(name);
	}
	else
	{
		options.SetInt(name, *cfgval);
	}
}

/**
 * @brief Copy one piece of data from options object to config log, or vice-versa
 */
static void
LoadConfigBoolSetting(BOOL * cfgval, COptionsMgr & options, const CString & name, ConfigLogDirection cfgdir)
{
	if (cfgdir == ToConfigLog)
	{
			*cfgval = options.GetBool(name);
	}
	else
	{
		options.SetBool(name, !!(*cfgval));
	}
}

/**
 * @brief Pass options settings from options manager object to config log, or vice-versa
 */
static void LoadConfigLog(CConfigLog & configLog, COptionsMgr & options,
	LOGFONT & lfDiff, ConfigLogDirection cfgdir)
{
	LoadConfigIntSetting(&configLog.m_diffOptions.nIgnoreWhitespace, options, OPT_CMP_IGNORE_WHITESPACE, cfgdir);
	LoadConfigBoolSetting(&configLog.m_diffOptions.bIgnoreBlankLines, options, OPT_CMP_IGNORE_BLANKLINES, cfgdir);
	LoadConfigBoolSetting(&configLog.m_diffOptions.bFilterCommentsLines, options, OPT_CMP_FILTER_COMMENTLINES, cfgdir);
	LoadConfigBoolSetting(&configLog.m_diffOptions.bIgnoreCase, options, OPT_CMP_IGNORE_CASE, cfgdir);
	LoadConfigBoolSetting(&configLog.m_diffOptions.bIgnoreEol, options, OPT_CMP_IGNORE_EOL, cfgdir);
	LoadConfigIntSetting(&configLog.m_compareSettings.nCompareMethod, options, OPT_CMP_METHOD, cfgdir);
	LoadConfigBoolSetting(&configLog.m_compareSettings.bStopAfterFirst, options, OPT_CMP_STOP_AFTER_FIRST, cfgdir);

	LoadConfigBoolSetting(&configLog.m_viewSettings.bShowIdent, options, OPT_SHOW_IDENTICAL, cfgdir);
	LoadConfigBoolSetting(&configLog.m_viewSettings.bShowDiff, options, OPT_SHOW_DIFFERENT, cfgdir);
	LoadConfigBoolSetting(&configLog.m_viewSettings.bShowUniqueLeft, options, OPT_SHOW_UNIQUE_LEFT, cfgdir);
	LoadConfigBoolSetting(&configLog.m_viewSettings.bShowUniqueRight, options, OPT_SHOW_UNIQUE_RIGHT, cfgdir);
	LoadConfigBoolSetting(&configLog.m_viewSettings.bShowBinaries, options, OPT_SHOW_BINARIES, cfgdir);
	LoadConfigBoolSetting(&configLog.m_viewSettings.bShowSkipped, options, OPT_SHOW_SKIPPED, cfgdir);

	LoadConfigBoolSetting(&configLog.m_miscSettings.bAutomaticRescan, options, OPT_AUTOMATIC_RESCAN, cfgdir);
	LoadConfigBoolSetting(&configLog.m_miscSettings.bAllowMixedEol, options, OPT_ALLOW_MIXED_EOL, cfgdir);
	LoadConfigBoolSetting(&configLog.m_miscSettings.bScrollToFirst, options, OPT_SCROLL_TO_FIRST, cfgdir);
	LoadConfigBoolSetting(&configLog.m_miscSettings.bBackup, options, OPT_CREATE_BACKUPS, cfgdir);
	LoadConfigBoolSetting(&configLog.m_miscSettings.bViewWhitespace, options, OPT_VIEW_WHITESPACE, cfgdir);
	LoadConfigBoolSetting(&configLog.m_miscSettings.bMovedBlocks, options, OPT_CMP_MOVED_BLOCKS, cfgdir);
	LoadConfigBoolSetting(&configLog.m_miscSettings.bShowLinenumbers, options, OPT_VIEW_LINENUMBERS, cfgdir);
	LoadConfigBoolSetting(&configLog.m_miscSettings.bWrapLines, options, OPT_WORDWRAP, cfgdir);
	LoadConfigBoolSetting(&configLog.m_miscSettings.bMergeMode, options, OPT_MERGE_MODE, cfgdir);

	LoadConfigIntSetting(&configLog.m_cpSettings.nDefaultMode, options, OPT_CP_DEFAULT_MODE, cfgdir);
	LoadConfigIntSetting(&configLog.m_cpSettings.nDefaultCustomValue, options, OPT_CP_DEFAULT_CUSTOM, cfgdir);
	LoadConfigBoolSetting(&configLog.m_cpSettings.bDetectCodepage, options, OPT_CP_DETECT, cfgdir);

	if (cfgdir == ToConfigLog)
	{
		configLog.m_fontSettings.nCharset = lfDiff.lfCharSet;
		configLog.m_fontSettings.sFacename = lfDiff.lfFaceName;
	}
	else
	{
		lfDiff.lfCharSet = configLog.m_fontSettings.nCharset;
		_tcsncpy(lfDiff.lfFaceName, configLog.m_fontSettings.sFacename, sizeof(lfDiff.lfFaceName)/sizeof(lfDiff.lfFaceName[0]));
	}
}


/**
 * @brief Save WinMerge configuration and info to file
 */
void CMainFrame::OnSaveConfigData()
{
	CConfigLog configLog;
	CString sError;

	LoadConfigLog(configLog, m_options, m_lfDiff, ToConfigLog);

	if (configLog.WriteLogFile(sError))
	{
		CString sFileName = configLog.GetFileName();
		OpenFileToExternalEditor(sFileName);
	}
	else
	{
		CString msg;
		CString sFileName = configLog.GetFileName();
		AfxFormatString2(msg, IDS_ERROR_FILEOPEN, sFileName, sError);
		AfxMessageBox(msg, MB_OK | MB_ICONSTOP);
	}
}

/**
 * @brief Open two new empty docs, 'Scratchpads'
 * 
 * Allows user to open two empty docs, to paste text to
 * compare from clipboard.
 * @note File filenames are set emptys and filedescriptors
 * are loaded from resource.
 * @sa CMergeDoc::OpenDocs()
 * @sa CMergeDoc::TrySaveAs()
 */
void CMainFrame::OnFileNew() 
{
	BOOL docNull;
	CDirDoc *pDirDoc;

	// If the dirdoc we are supposed to use is busy doing a diff, bail out
	if (IsComparing())
		return;

	if (!m_options.GetBool(OPT_MULTIDOC_DIRDOCS))
	{
		pDirDoc = GetDirDocToShow(&docNull);
		if (!docNull)
		{
			// If dircompare contains results, warn user that they are lost
			if (pDirDoc->HasDiffs())
			{
				int res = AfxMessageBox(IDS_DIR_RESULTS_EMPTIED, MB_OKCANCEL |
					MB_ICONWARNING | MB_DONT_DISPLAY_AGAIN, IDS_DIR_RESULTS_EMPTIED);
				if (res == IDCANCEL)
					return;
			}

			// If reusing an existing doc, give it a chance to save its data
			// and close any merge views, and clear its window
			if (!pDirDoc->ReusingDirDoc())
				return;
		}
	}
	else
		pDirDoc = (CDirDoc*)theApp.m_pDirTemplate->CreateNewDocument();
	
	// Load emptyfile descriptors and open empty docs
	// Use default codepage
	VERIFY(m_strLeftDesc.LoadString(IDS_EMPTY_LEFT_FILE));
	VERIFY(m_strRightDesc.LoadString(IDS_EMPTY_RIGHT_FILE));
	FileLocation filelocLeft; // empty, unspecified (so default) encoding
	FileLocation filelocRight;
	ShowMergeDoc(pDirDoc, filelocLeft, filelocRight, FALSE, FALSE);


	// Empty descriptors now that docs are open
	m_strLeftDesc.Empty();
	m_strRightDesc.Empty();
}

/**
 * @brief Open Filters dialog
 */
void CMainFrame::OnToolsFilters()
{
	CPropertySheet sht(IDS_FILTER_TITLE);
	CPropLineFilter filter;
	FileFiltersDlg fileFiltersDlg;
	FILEFILTER_INFOLIST fileFilters;
	CString selectedFilter;
	sht.AddPage(&fileFiltersDlg);
	sht.AddPage(&filter);
	sht.m_psh.dwFlags |= PSH_NOAPPLYNOW; // Hide 'Apply' button since we don't need it

	// Make sure all filters are up-to-date
	theApp.m_globalFileFilter.ReloadUpdatedFilters();

	theApp.m_globalFileFilter.GetFileFilters(&fileFilters, selectedFilter);
	fileFiltersDlg.SetFilterArray(&fileFilters);
	fileFiltersDlg.SetSelected(selectedFilter);
	filter.m_bIgnoreRegExp = m_options.GetBool(OPT_LINEFILTER_ENABLED);
	filter.m_sPattern = m_options.GetString(OPT_LINEFILTER_REGEXP);

	if (sht.DoModal() == IDOK)
	{
		CString strNone;
		VERIFY(strNone.LoadString(IDS_USERCHOICE_NONE));
		CString path = fileFiltersDlg.GetSelected();
		if (path.Find(strNone) > -1)
		{
			// Don't overwrite mask we already have
			if (!theApp.m_globalFileFilter.IsUsingMask())
			{
				CString sFilter = _T("*.*");
				theApp.m_globalFileFilter.SetFilter(sFilter);
				m_options.SaveOption(OPT_FILEFILTER_CURRENT, sFilter);
			}
		}
		else
		{
			theApp.m_globalFileFilter.SetFileFilterPath(path);
			theApp.m_globalFileFilter.UseMask(FALSE);
			CString sFilter = theApp.m_globalFileFilter.GetFilterNameOrMask();
			m_options.SaveOption(OPT_FILEFILTER_CURRENT, sFilter);
		}
		m_options.SaveOption(OPT_LINEFILTER_ENABLED, filter.m_bIgnoreRegExp == TRUE);
		m_options.SaveOption(OPT_LINEFILTER_REGEXP, filter.m_sPattern);

		RebuildRegExpList(TRUE);
	}
}

/**
 * @brief Open Filters dialog.
 */
void CMainFrame::SelectFilter()
{
	OnToolsFilters();
}

/**
 * @brief Closes application with ESC.
 *
 * Application is closed if:
 * - 'Close Windows with ESC' option is enabled and
 *    there is no open document windows
 * - '-e' commandline switch is given
 */
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// Check if we got 'ESC pressed' -message
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		if (m_bEscShutdown)
		{
			AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_APP_EXIT);
			return TRUE;
		}
		else
		{
			if (m_options.GetBool(OPT_CLOSE_WITH_ESC))
			{
				MergeDocList docs;
				GetAllMergeDocs(&docs);
				DirDocList dirDocs;
				GetAllDirDocs(&dirDocs);

				if (docs.IsEmpty() && dirDocs.IsEmpty())
				{
					AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_APP_EXIT);
					return FALSE;
				}
			}
		}
	}
	return CMDIFrameWnd::PreTranslateMessage(pMsg);
}

/**
 * @brief Shows VSS error from exception and writes log.
 */
void CMainFrame::ShowVSSError(CException *e, CString strItem)
{
	CString errMsg;
	CString logMsg;
	TCHAR errStr[1024] = {0};

	VERIFY(errMsg.LoadString(IDS_VSS_ERRORFROM));
	if (e->GetErrorMessage(errStr, 1024))
	{
		logMsg = errMsg;
		errMsg += _T("\n");
		errMsg += errStr;
		logMsg += _T(" ");
		logMsg += errStr;
		if (!strItem.IsEmpty())
		{
			errMsg += _T("\n\n");
			errMsg += strItem;
			logMsg += _T(": ");
			logMsg += strItem;
		}
		LogErrorString(logMsg);
		AfxMessageBox(errMsg, MB_ICONSTOP);
	}
	else
	{
		LogErrorString(_T("VSSError (unable to GetErrorMessage)"));
		e->ReportError(MB_ICONSTOP, IDS_VSS_RUN_ERROR);
	}
}

/**
 * @brief Show Help - this is for opening help from outside mainframe.
 * @param [in] helpLocation Location inside help, if NULL main help is opened.
 */
void CMainFrame::ShowHelp(LPCTSTR helpLocation /*= NULL*/)
{
	if (helpLocation == NULL)
	{
		OnHelpContents();
	}
	else
	{
		CString sPath = GetModulePath(0) + DocsPath;
		if (paths_DoesPathExist(sPath) == IS_EXISTING_FILE)
		{
			sPath += helpLocation;
			::HtmlHelp(GetSafeHwnd(), sPath, HH_DISPLAY_TOPIC, NULL);
		}
	}
}

/**
 * @brief Tell user why archive support is not available.
 */
void CMainFrame::OnHelpMerge7zmismatch() 
{
	Recall7ZipMismatchError();
}

/**
 * @brief Remove ID_HELP_MERGE7ZMISMATCH if archive support is available.
 */
void CMainFrame::OnUpdateHelpMerge7zmismatch(CCmdUI* pCmdUI) 
{
	if (HasZipSupport())
	{
		pCmdUI->m_pMenu->RemoveMenu(ID_HELP_MERGE7ZMISMATCH, MF_BYCOMMAND);
	}
}

/**
 * @brief Show/hide statusbar.
 */
void CMainFrame::OnViewStatusBar()
{
	bool bShow = !m_options.GetBool(OPT_SHOW_STATUSBAR);
	m_options.SaveOption(OPT_SHOW_STATUSBAR, bShow);

	CMDIFrameWnd::ShowControlBar(&m_wndStatusBar, bShow, 0);
}

/**
 * @brief Show/hide toolbar.
 */
void CMainFrame::OnViewToolbar()
{
	bool bShow = !m_options.GetBool(OPT_SHOW_TOOLBAR);
	m_options.SaveOption(OPT_SHOW_TOOLBAR, bShow);

	CMDIFrameWnd::ShowControlBar(&m_wndToolBar, bShow, 0);
}

/**
 * @brief Open project-file.
 */
void CMainFrame::OnFileOpenproject()
{
	CString sFilepath;
	CString title;
	VERIFY(title.LoadString(IDS_OPEN_TITLE));
	
	// get the default projects path
	CString strProjectPath = m_options.GetString(OPT_PROJECTS_PATH);
	if (!SelectFile(sFilepath, strProjectPath, title, IDS_PROJECTFILES, TRUE))
		return;
	
	strProjectPath = paths_GetParentPath(sFilepath);
	// store this as the new project path
	m_options.SaveOption(OPT_PROJECTS_PATH,strProjectPath);

	theApp.LoadAndOpenProjectFile(sFilepath);
}

/**
 * @brief Receive commandline from another instance.
 *
 * This function receives commandline when only single-instance
 * is allowed. New instance tried to start sends its commandline
 * to here so we can open paths it was meant to.
 */
LRESULT CMainFrame::OnCopyData(WPARAM wParam, LPARAM lParam)
{
	COPYDATASTRUCT *pCopyData = (COPYDATASTRUCT*)lParam;
	LPWSTR p = (LPWSTR)(pCopyData->lpData);
	int argc = pCopyData->dwData;
	TCHAR **argv = new (TCHAR *[argc]);
	USES_CONVERSION;
	
	for (int i = 0; i < argc; i++)
	{
		argv[i] = new TCHAR[lstrlenW(p) * (sizeof(WCHAR)/sizeof(TCHAR)) + 1];
		lstrcpy(argv[i], W2T(p));
		while (*p) p++;
		p++;
	}

	PostMessage(WM_USER, (WPARAM)argc, (LPARAM)argv);

	return TRUE;
}

LRESULT CMainFrame::OnUser(WPARAM wParam, LPARAM lParam)
{
	int argc = (int)wParam;
	TCHAR **argv = (TCHAR **)lParam;

	theApp.ParseArgsAndDoOpen(argc, argv, this);
	
	for (int i = 0; i < argc; i++)
		delete[] argv[i];
	delete [] argv;

	return TRUE;
}

/**
 * @brief When font is changed open views must be closed.
 */
void CMainFrame::ShowFontChangeMessage()
{
	DirViewList dirViews;
	MergeEditViewList editViews;
	GetAllViews(&editViews, NULL, &dirViews);

	if (editViews.GetCount() > 0 || dirViews.GetCount() > 0)
		AfxMessageBox(IDS_FONT_CHANGE, MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN, IDS_FONT_CHANGE);
}

/**
 * @brief Prompt user to select configuration file, and then load settings from it
 */
void CMainFrame::OnDebugLoadConfig()
{
	const TCHAR filetypes[] = _T("WinMerge Config files (*.txt)|*.txt|All files (*.*)|*.*||");
	CFileDialog dlg(true, _T(".log"),0,0, filetypes);
	if (dlg.DoModal() != IDOK)
		return;

	CString filepath = dlg.GetPathName();

	CConfigLog configLog;

	// set configLog settings to current
	LoadConfigLog(configLog, m_options, m_lfDiff, ToConfigLog);

	// update any settings found in actual config file
	configLog.ReadLogFile(filepath);

	// set our current settings from configLog settings
	LoadConfigLog(configLog, m_options, m_lfDiff, FromConfigLog);
}

/**
 * @brief Send current option settings into codepage module
 */
void CMainFrame::UpdateCodepageModule()
{
	// Get current codepage settings from the options module
	// and push them into the codepage module
	updateDefaultCodepage(m_options.GetInt(OPT_CP_DEFAULT_MODE), m_options.GetInt(OPT_CP_DEFAULT_CUSTOM));
}

/**
 * @brief Handle timer events.
 * @param [in] nIDEvent Timer that timed out.
 */
void CMainFrame::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case WM_NONINTERACTIVE:
		KillTimer(WM_NONINTERACTIVE);
		PostMessage(WM_CLOSE);
		break;
	
	case ID_TIMER_FLASH:
		// This timer keeps running until window is activated
		// See OnActivate()
		FlashWindow(TRUE);
		break;
	}
	CMDIFrameWnd::OnTimer(nIDEvent);
}

/**
 * @brief Close all open windows.
 * 
 * Asks about saving unsaved files and then closes all open windows.
 */
void CMainFrame::OnWindowCloseAll()
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
			// Allow user to cancel closing
			if (!pMergeDoc->PromptAndSaveIfNeeded(TRUE))
				return;
			else
			{
				// Set modified status to false so that we are not asking
				// about saving again. 
				pMergeDoc->m_ptBuf[0]->SetModified(FALSE);
				pMergeDoc->m_ptBuf[1]->SetModified(FALSE);
			}
		}
	}

	DirDocList dirdocs;
	GetAllDirDocs(&dirdocs);
	while (!dirdocs.IsEmpty())
	{
		CDirDoc * pDirDoc = dirdocs.RemoveHead();
		if (pDirDoc->HasDirView())
		{
			pDirDoc->CloseMergeDocs();
			pDirDoc->OnCloseDocument();
		}
		else
		{
			// When comparing files from Open-Dialog, pDirDoc has no View.
			pDirDoc->CloseMergeDocs();
			// pDirDoc has no View and has already been deleted by CloseMergeDocs. So no need to call OnCloseDocument.
			//pDirDoc->OnCloseDocument();
		}
	}
}

/**
 * @brief Enables Window/Close All item if there are open windows.
 */ 
void CMainFrame::OnUpdateWindowCloseAll(CCmdUI* pCmdUI)
{
	MergeDocList mergedocs;
	GetAllMergeDocs(&mergedocs);
	if (!mergedocs.IsEmpty())
	{
		pCmdUI->Enable(TRUE);
		return;
	}

	DirDocList dirdocs;
	GetAllDirDocs(&dirdocs);
	if (!dirdocs.IsEmpty())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Checkin in file into ClearCase.
 */ 
void CMainFrame::CheckinToClearCase(CString strDestinationPath)
{
	CString spath, sname;
	SplitFilename(strDestinationPath, &spath, &sname, 0);
	DWORD code;
	CString args;
	
	// checkin operation
	args.Format(_T("checkin -nc \"%s\""), sname);
	CString vssPath = m_options.GetString(OPT_VSS_PATH);
	HANDLE hVss = RunIt(vssPath, args, TRUE, FALSE);
	if (hVss!=INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(hVss, INFINITE);
		GetExitCodeProcess(hVss, &code);
		CloseHandle(hVss);
				
		if (code != 0)
		{
			if (AfxMessageBox(IDS_VSS_CHECKINERROR, MB_ICONWARNING | MB_YESNO) == IDYES)
			{
				// undo checkout operation
				args.Format(_T("uncheckout -rm \"%s\""), sname);
				HANDLE hVss = RunIt(vssPath, args, TRUE, TRUE);
				if (hVss!=INVALID_HANDLE_VALUE)
				{
					WaitForSingleObject(hVss, INFINITE);
					GetExitCodeProcess(hVss, &code);
					CloseHandle(hVss);
					
					if (code != 0)
					{
						AfxMessageBox(IDS_VSS_UNCOERROR, MB_ICONSTOP);
						return;
					}
				}
				else
				{
					AfxMessageBox(IDS_VSS_RUN_ERROR, MB_ICONSTOP);
					return;
				}				
			}
			return;
		}
	}
	else
	{
		AfxMessageBox(IDS_VSS_RUN_ERROR, MB_ICONSTOP);
		return;
	}
}

/**
 * @brief Access to the singleton options manager
 */
COptionsMgr *
GetOptionsMgr() 
{
	return GetMainFrame()->GetTheOptionsMgr();
}

/**
 * @brief Access to the singleton main frame (where we have some globals)
 */
CMainFrame * GetMainFrame()
{
	CWnd * mainwnd = AfxGetMainWnd();
	ASSERT(mainwnd);
	CMainFrame *pMainframe = dynamic_cast<CMainFrame*>(mainwnd);
	ASSERT(pMainframe);
	return pMainframe;
}

/**
 * @brief Access to the singleton set of syntax colors
 */
SyntaxColors * GetMainSyntaxColors()
{
	return GetMainFrame()->GetMainSyntaxColors();
}

/** 
 * @brief Move dialog to center of MainFrame
 */
void CMainFrame::CenterToMainFrame(CDialog * dlg)
{
	CRect rectFrame;
	CRect rectBar;
	AfxGetMainWnd()->GetWindowRect(&rectFrame);
	dlg->GetClientRect(&rectBar);
	// Middlepoint of MainFrame
	int x = rectFrame.left + (rectFrame.right - rectFrame.left) / 2;
	int y = rectFrame.top + (rectFrame.bottom - rectFrame.top) / 2;
	// Reduce by half of dialog's size
	x -= rectBar.right / 2;
	y -= rectBar.bottom / 2;

	// This refreshes dialog size after m_constraint rezizing dialog so we get
	// correct dialog positioning
	dlg->CenterWindow();

	// Calculate real desktop coordinates (if we have multiple monitors or
	// virtual desktops
	CRect dsk_rc;
	dsk_rc.left = ::GetSystemMetrics(SM_XVIRTUALSCREEN);
	dsk_rc.top = ::GetSystemMetrics(SM_YVIRTUALSCREEN);
	dsk_rc.right = dsk_rc.left + ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
	dsk_rc.bottom = dsk_rc.top + ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

	// Only move Open-dialog if its fully visible in new position
	CPoint ptLeftTop(x, y);
	CPoint ptRightBottom(x + rectBar.right, y + rectBar.bottom);
	if (dsk_rc.PtInRect(ptLeftTop) && dsk_rc.PtInRect(ptRightBottom))
	{
		dlg->SetWindowPos(&CWnd::wndTop, x, y, rectBar.right,
			rectBar.bottom, SWP_NOOWNERZORDER | SWP_NOSIZE );
	}
}

/**
 * @brief Assign the main WinMerge 16x16 icon to dialog
 */
void CMainFrame::SetMainIcon(CDialog * dlg)
{
	// Note: LoadImage gets shared icon, don't need to destroy
	HICON hMergeIcon = (HICON) LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16,
			LR_DEFAULTSIZE | LR_SHARED);
	dlg->SetIcon(hMergeIcon, TRUE);
}

/** 
 * @brief Opens dialog for user to Load, edit and save project files.
 */
void CMainFrame::OnSaveProject()
{
	CPropertySheet sht(IDS_PROJFILEDLG_CAPTION);
	ProjectFilePathsDlg pathsDlg;
	sht.AddPage(&pathsDlg);
	sht.m_psh.dwFlags |= PSH_NOAPPLYNOW; // Hide 'Apply' button since we don't need it

	CString left;
	CString right;
	CFrameWnd * pFrame = GetActiveFrame();
	BOOL bMergeFrame = pFrame->IsKindOf(RUNTIME_CLASS(CChildFrame));
	BOOL bDirFrame = pFrame->IsKindOf(RUNTIME_CLASS(CDirFrame));

	if (bMergeFrame)
	{
		CMergeDoc * pMergeDoc = (CMergeDoc *) pFrame->GetActiveDocument();
		left = pMergeDoc->m_filePaths.GetLeft();
		right = pMergeDoc->m_filePaths.GetRight();
		pathsDlg.SetPaths(left, right);
	}
	else if (bDirFrame)
	{
		CDirDoc * pDoc = (CDirDoc*)pFrame->GetActiveDocument();
		CDirView *pView = pDoc->GetMainView();
		int ind = pView->GetFirstSelectedInd();
		const DIFFITEM item = pView->GetItemAt(ind);
		left = item.getLeftFilepath(pDoc->GetLeftBasePath());
		left += "\\";
		right = item.getRightFilepath(pDoc->GetRightBasePath());
		right += "\\";
		pathsDlg.SetPaths(left, right);
	}

	CString filterNameOrMask = theApp.m_globalFileFilter.GetFilterNameOrMask();
	pathsDlg.m_sFilter = filterNameOrMask;
	sht.DoModal();
}

/** 
 * @brief Start flashing window if window is inactive.
 */
void CMainFrame::StartFlashing()
{
	CWnd * activeWindow = GetActiveWindow();
	if (activeWindow != this)
	{
		FlashWindow(TRUE);
		m_bFlashing = TRUE;
		SetTimer(ID_TIMER_FLASH, WINDOW_FLASH_TIMEOUT, NULL);
	}
}

/** 
 * @brief Stop flashing window when window is activated.
 *
 * If WinMerge is inactive when compare finishes, we start flashing window
 * to alert user. When user activates WinMerge window we stop flashing.
 * @param [in] nState Tells if window is being activated or deactivated.
 * @param [in] pWndOther Pointer to window whose status is changing.
 * @param [in] Is window minimized?
 */
void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE)
	{
		if (m_bFlashing)
		{
			m_bFlashing = FALSE;
			FlashWindow(FALSE);
			KillTimer(ID_TIMER_FLASH);
		}
	}
}

BOOL CMainFrame::CreateToobar()
{
	if (!m_wndToolBar.Create(this, WS_CHILD|WS_VISIBLE|CBRS_GRIPPER|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		return FALSE;
	}

	m_wndToolBar.SetBorders(1, 1, 1, 1);
	VERIFY(m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT));

	// Remove this if you don't want tool tips or a resizable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// Delete these three lines if you don't want the toolbar to
	// be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	LoadToolbarImageList(IDB_TOOLBAR_ENABLED, m_ToolbarImages[TOOLBAR_IMAGES_ENABLED]);
	LoadToolbarImageList(IDB_TOOLBAR_DISABLED, m_ToolbarImages[TOOLBAR_IMAGES_DISABLED]);

	CToolBarCtrl& BarCtrl = m_wndToolBar.GetToolBarCtrl();
	BarCtrl.SetImageList(&m_ToolbarImages[TOOLBAR_IMAGES_ENABLED]);
	BarCtrl.SetDisabledImageList(&m_ToolbarImages[TOOLBAR_IMAGES_DISABLED]);

	if (m_options.GetBool(OPT_SHOW_TOOLBAR) == false)
	{
		CMDIFrameWnd::ShowControlBar(&m_wndToolBar, false, 0);
	}

	return TRUE;
}

/**
 * @brief Load a transparent 24-bit color image list.
 */
static void LoadHiColImageList(UINT nIDResource, int nWidth, int nHeight, int nCount, CImageList& ImgList, COLORREF crMask = RGB(255,0,255))
{
	CBitmap bm;
	VERIFY(bm.LoadBitmap(nIDResource));
	VERIFY(ImgList.Create(nWidth, nHeight, ILC_COLORDDB|ILC_MASK, nCount, 0));
	int nIndex = ImgList.Add(&bm, crMask);
	ASSERT(-1 != nIndex);
}

/**
 * @brief Load toolbar image list.
 */
static void LoadToolbarImageList(UINT nIDResource, CImageList& ImgList)
{
	static const int ImageWidth = 16;
	static const int ImageHeight = 15;
	static const int ImageCount	= 19;

	LoadHiColImageList(nIDResource, ImageWidth, ImageHeight, ImageHeight, ImgList);
}
