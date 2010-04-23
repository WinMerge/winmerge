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
 * @file  OpenDlg.cpp
 *
 * @brief Implementation of the COpenDlg class
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "UnicodeString.h"
#include "Merge.h"
#include "ProjectFile.h"
#include "OpenDlg.h"
#include "coretools.h"
#include "paths.h"
#include "SelectUnpackerDlg.h"
#include "OptionsDef.h"
#include "MainFrm.h"
#include "OptionsMgr.h"
#include "FileOrFolderSelect.h"

#ifdef COMPILE_MULTIMON_STUBS
#undef COMPILE_MULTIMON_STUBS
#endif
#include <multimon.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Timer ID and timeout for delaying path validity check
const UINT IDT_CHECKFILES = 1;
const UINT CHECKFILES_TIMEOUT = 1000; // milliseconds
static const TCHAR EMPTY_EXTENSION[] = _T(".*");

/** @brief Location for Open-dialog specific help to open. */
static TCHAR OpenDlgHelpLocation[] = _T("::/htmlhelp/Open_paths.html");

/////////////////////////////////////////////////////////////////////////////
// COpenDlg dialog

/**
 * @brief Standard constructor.
 */
COpenDlg::COpenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenDlg::IDD, pParent)
	, m_pathsType(DOES_NOT_EXIST)
	, m_bOverwriteRecursive(FALSE)
	, m_bRecurse(FALSE)
	, m_pProjectFile(NULL)
{
}

/**
 * @brief Standard destructor.
 */
COpenDlg::~COpenDlg()
{
	delete m_pProjectFile;
}

void COpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenDlg)
	DDX_Control(pDX, IDC_SELECT_UNPACKER, m_ctlSelectUnpacker);
	DDX_Control(pDX, IDC_UNPACKER_EDIT, m_ctlUnpacker);
	DDX_Control(pDX, IDC_EXT_COMBO, m_ctlExt);
	DDX_Control(pDX, IDOK, m_ctlOk);
	DDX_Control(pDX, IDC_RECURS_CHECK, m_ctlRecurse);
	DDX_Control(pDX, IDC_RIGHT_COMBO, m_ctlRight);
	DDX_Control(pDX, IDC_LEFT_COMBO, m_ctlLeft);
	DDX_CBStringExact(pDX, IDC_LEFT_COMBO, m_strLeft);
	DDX_CBStringExact(pDX, IDC_RIGHT_COMBO, m_strRight);
	DDX_Check(pDX, IDC_RECURS_CHECK, m_bRecurse);
	DDX_CBStringExact(pDX, IDC_EXT_COMBO, m_strExt);
	DDX_Text(pDX, IDC_UNPACKER_EDIT, m_strUnpacker);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenDlg, CDialog)
	//{{AFX_MSG_MAP(COpenDlg)
	ON_BN_CLICKED(IDC_LEFT_BUTTON, OnLeftButton)
	ON_BN_CLICKED(IDC_RIGHT_BUTTON, OnRightButton)
	ON_CBN_SELCHANGE(IDC_LEFT_COMBO, OnSelchangeLeftCombo)
	ON_CBN_SELCHANGE(IDC_RIGHT_COMBO, OnSelchangeRightCombo)
	ON_CBN_EDITCHANGE(IDC_LEFT_COMBO, OnEditEvent)
	ON_BN_CLICKED(IDC_SELECT_UNPACKER, OnSelectUnpacker)
	ON_CBN_SELENDCANCEL(IDC_LEFT_COMBO, UpdateButtonStates)
	ON_CBN_EDITCHANGE(IDC_RIGHT_COMBO, OnEditEvent)
	ON_CBN_SELENDCANCEL(IDC_RIGHT_COMBO, UpdateButtonStates)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SELECT_FILTER, OnSelectFilter)
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_HELP, OnHelp)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenDlg message handlers

/**
 * @brief Handler for WM_INITDIALOG; conventional location to initialize controls
 * At this point dialog and control windows exist
 */
BOOL COpenDlg::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();
	
	// setup handler for resizing this dialog	
	m_constraint.InitializeCurrentSize(this);
	// configure how individual controls adjust when dialog resizes
	m_constraint.ConstrainItem(IDC_LEFT_COMBO, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_RIGHT_COMBO, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_EXT_COMBO, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_UNPACKER_EDIT, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_FILES_DIRS_GROUP, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_LEFT_BUTTON, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDC_RIGHT_BUTTON, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDC_SELECT_UNPACKER, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDC_OPEN_STATUS, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SELECT_FILTER, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDOK, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDCANCEL, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(ID_HELP, 1, 0, 0, 0); // slides right
	m_constraint.DisallowHeightGrowth();
	m_constraint.SubclassWnd(); // install subclassing
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("OpenDlg"), false); // persist size via registry

	CMainFrame::CenterToMainFrame(this);

	m_ctlLeft.LoadState(_T("Files\\Left"));
	m_ctlRight.LoadState(_T("Files\\Right"));
	m_ctlExt.LoadState(_T("Files\\Ext"));
	UpdateData(m_strLeft.IsEmpty() && m_strRight.IsEmpty());

	int nSource = GetOptionsMgr()->GetInt(OPT_AUTO_COMPLETE_SOURCE);
	if (nSource > 0)
	{
		m_ctlLeft.SetAutoComplete(nSource);
		m_ctlRight.SetAutoComplete(nSource);
	}

	CString FilterNameOrMask = theApp.m_globalFileFilter.GetFilterNameOrMask();
	BOOL bMask = theApp.m_globalFileFilter.IsUsingMask();

	if (!bMask)
	{
		String filterPrefix = theApp.LoadString(IDS_FILTER_PREFIX);
		FilterNameOrMask.Insert(0, filterPrefix.c_str());
	}

	int ind = m_ctlExt.FindStringExact(0, FilterNameOrMask);
	if (ind != CB_ERR)
		m_ctlExt.SetCurSel(ind);
	else
	{
		ind = m_ctlExt.InsertString(0, FilterNameOrMask);
		if (ind != CB_ERR)
			m_ctlExt.SetCurSel(ind);
		else
			LogErrorString(_T("Failed to add string to filters combo list!"));
	}

	if (!GetOptionsMgr()->GetBool(OPT_VERIFY_OPEN_PATHS))
	{
		m_ctlOk.EnableWindow(TRUE);
		m_ctlUnpacker.EnableWindow(TRUE);
		m_ctlSelectUnpacker.EnableWindow(TRUE);
	}

	UpdateButtonStates();

	if (!m_bOverwriteRecursive)
		m_bRecurse = theApp.GetProfileInt(_T("Settings"), _T("Recurse"), 0) == 1;

	m_strUnpacker = m_infoHandler.pluginName.c_str();
	UpdateData(FALSE);
	SetStatus(IDS_OPEN_FILESDIRS);
	SetUnpackerStatus(IDS_OPEN_UNPACKERDISABLED);
	return TRUE;
}

/** 
 * @brief Called when "Browse..." button is selected for left path.
 */
void COpenDlg::OnLeftButton()
{
	CString s;
	String sfolder;
	UpdateData(TRUE); 

	PATH_EXISTENCE existence = paths_DoesPathExist(m_strLeft);
	switch (existence)
	{
	case IS_EXISTING_DIR:
		sfolder = m_strLeft;
		break;
	case IS_EXISTING_FILE:
		sfolder = GetPathOnly(m_strLeft);
		break;
	case DOES_NOT_EXIST:
		// Do nothing, empty foldername will be passed to dialog
		break;
	default:
		_RPTF0(_CRT_ERROR, "Invalid return value from paths_DoesPathExist()");
		break;
	}

	if (SelectFileOrFolder(GetSafeHwnd(), s, sfolder.c_str()))
	{
		m_strLeft = s;
		m_strLeftBrowsePath = s;
		UpdateData(FALSE);
		UpdateButtonStates();
	}	
}

/** 
 * @brief Called when "Browse..." button is selected for right path.
 */
void COpenDlg::OnRightButton() 
{
	CString s;
	String sfolder;
	UpdateData(TRUE);

	PATH_EXISTENCE existence = paths_DoesPathExist(m_strRight);
	switch (existence)
	{
	case IS_EXISTING_DIR:
		sfolder = m_strRight;
		break;
	case IS_EXISTING_FILE:
		sfolder = GetPathOnly(m_strRight);
		break;
	case DOES_NOT_EXIST:
		// Do nothing, empty foldername will be passed to dialog
		break;
	default:
		_RPTF0(_CRT_ERROR, "Invalid return value from paths_DoesPathExist()");
		break;
	}

	if (SelectFileOrFolder(GetSafeHwnd(), s, sfolder.c_str()))
	{
		m_strRight = s;
		m_strRightBrowsePath = s;
		UpdateData(FALSE);
		UpdateButtonStates();
	}	
}

/** 
 * @brief Called when dialog is closed with "OK".
 *
 * Checks that paths are valid and sets filters.
 */
void COpenDlg::OnOK() 
{
	const String filterPrefix = theApp.LoadString(IDS_FILTER_PREFIX);

	UpdateData(TRUE);
	TrimPaths();

	// If left path is a project-file, load it
	String ext;
	SplitFilename(m_strLeft, NULL, NULL, &ext);
	CString sExt(ext.c_str());
	if (m_strRight.IsEmpty() && sExt.CompareNoCase(PROJECTFILE_EXT) == 0)
		LoadProjectFile(m_strLeft);

	m_pathsType = GetPairComparability(m_strLeft, m_strRight);

	if (m_pathsType == DOES_NOT_EXIST)
	{
		LangMessageBox(IDS_ERROR_INCOMPARABLE, MB_ICONSTOP);
		return;
	}

	// If user has edited path by hand, expand environment variables
	BOOL bExpandLeft = FALSE;
	BOOL bExpandRight = FALSE;
	if (m_strLeftBrowsePath.CompareNoCase(m_strLeft) != 0)
		bExpandLeft = TRUE;
	if (m_strRightBrowsePath.CompareNoCase(m_strRight) != 0)
		bExpandRight = TRUE;

	m_strRight = paths_GetLongPath(m_strRight, bExpandRight).c_str();
	m_strLeft = paths_GetLongPath(m_strLeft, bExpandLeft).c_str();

	// Add trailing '\' for directories if its missing
	if (m_pathsType == IS_EXISTING_DIR)
	{
		if (!paths_EndsWithSlash(m_strLeft))
			m_strLeft += '\\';
		if (!paths_EndsWithSlash(m_strRight))
			m_strRight += '\\';
	}

	UpdateData(FALSE);
	KillTimer(IDT_CHECKFILES);

	String filter((LPCTSTR)m_strExt);
	filter = string_trim_ws(filter);

	// If prefix found from start..
	if (filter.find(filterPrefix, 0) == 0)
	{
		// Remove prefix + space
		filter.erase(0, filterPrefix.length());
		if (!theApp.m_globalFileFilter.SetFilter(filter))
		{
			// If filtername is not found use default *.* mask
			theApp.m_globalFileFilter.SetFilter(_T("*.*"));
			filter = _T("*.*");
		}
		GetOptionsMgr()->SaveOption(OPT_FILEFILTER_CURRENT, filter.c_str());
	}
	else
	{
		BOOL bFilterSet = theApp.m_globalFileFilter.SetFilter(filter);
		if (!bFilterSet)
			m_strExt = theApp.m_globalFileFilter.GetFilterNameOrMask();
		GetOptionsMgr()->SaveOption(OPT_FILEFILTER_CURRENT, filter.c_str());
	}

	SaveComboboxStates();
	theApp.WriteProfileInt(_T("Settings"), _T("Recurse"), m_bRecurse);

	CDialog::OnOK();
}

/** 
 * @brief Called when dialog is closed via Cancel.
 *
 * Open-dialog is canceled when 'Cancel' button is selected or
 * Esc-key is pressed. Save combobox states, since user may have
 * removed items from them and don't want them to re-appear.
 */
void COpenDlg::OnCancel()
{
	SaveComboboxStates();
	CDialog::OnCancel();
}

/** 
 * @brief Save File- and filter-combobox states.
 */
void COpenDlg::SaveComboboxStates()
{
	m_ctlLeft.SaveState(_T("Files\\Left"));
	m_ctlRight.SaveState(_T("Files\\Right"));
	m_ctlExt.SaveState(_T("Files\\Ext"));
}

/** 
 * @brief Enable/disable components based on validity of paths.
 */
void COpenDlg::UpdateButtonStates()
{
	BOOL bLeftInvalid = FALSE;
	BOOL bRightInvalid = FALSE;

	UpdateData(TRUE); // load member variables from screen
	KillTimer(IDT_CHECKFILES);
	TrimPaths();

	// Check if we have project file as left side path
	BOOL bProject = FALSE;
	String ext;
	SplitFilename(m_strLeft, NULL, NULL, &ext);
	CString sExt(ext.c_str());
	if (m_strRight.IsEmpty() && sExt.CompareNoCase(PROJECTFILE_EXT) == 0)
		bProject = TRUE;

	// Enable buttons as appropriate
	PATH_EXISTENCE pathsType = GetPairComparability(m_strLeft, m_strRight);
	if (GetOptionsMgr()->GetBool(OPT_VERIFY_OPEN_PATHS))
	{
		if (bProject)
		{
			m_ctlOk.EnableWindow(TRUE);
			m_ctlUnpacker.EnableWindow(TRUE);
			m_ctlSelectUnpacker.EnableWindow(TRUE);
		}
		else
		{
			m_ctlOk.EnableWindow(pathsType != DOES_NOT_EXIST);
			m_ctlUnpacker.EnableWindow(pathsType == IS_EXISTING_FILE);
			m_ctlSelectUnpacker.EnableWindow(pathsType == IS_EXISTING_FILE);
		}
	}

	if (!bProject)
	{
		if (paths_DoesPathExist(m_strLeft) == DOES_NOT_EXIST)
			bLeftInvalid = TRUE;
		if (paths_DoesPathExist(m_strRight) == DOES_NOT_EXIST)
			bRightInvalid = TRUE;
	}

	if (bLeftInvalid && bRightInvalid)
		SetStatus(IDS_OPEN_BOTHINVALID);
	else if (bLeftInvalid)
		SetStatus(IDS_OPEN_LEFTINVALID);
	else if (bRightInvalid)
		SetStatus(IDS_OPEN_RIGHTINVALID);
	else if (!bLeftInvalid && !bRightInvalid && pathsType == DOES_NOT_EXIST)
		SetStatus(IDS_OPEN_MISMATCH);
	else
		SetStatus(IDS_OPEN_FILESDIRS);

	if (pathsType == IS_EXISTING_FILE || bProject)
		SetUnpackerStatus(0);	//Empty field
	else
		SetUnpackerStatus(IDS_OPEN_UNPACKERDISABLED);
}

/**
 * @brief Called when user changes selection in left path's combo box.
 */
void COpenDlg::OnSelchangeLeftCombo() 
{
	int sel = m_ctlLeft.GetCurSel();
	if (sel != CB_ERR)
	{
		m_ctlLeft.GetLBText(sel, m_strLeft);
		m_ctlLeft.SetWindowText(m_strLeft);
		UpdateData(TRUE);
	}
	UpdateButtonStates();
}

/**
 * @brief Called when user changes selection in right path's combo box.
 */
void COpenDlg::OnSelchangeRightCombo() 
{
	int sel = m_ctlRight.GetCurSel();
	if (sel != CB_ERR)
	{
		m_ctlRight.GetLBText(sel, m_strRight);
		m_ctlRight.SetWindowText(m_strRight);
		UpdateData(TRUE);
	}
	UpdateButtonStates();
}

/** 
 * @brief Called every time paths are edited.
 */
void COpenDlg::OnEditEvent()
{
	// (Re)start timer to path validity check delay
	// If timer starting fails, update buttonstates immediately
	if (!SetTimer(IDT_CHECKFILES, CHECKFILES_TIMEOUT, NULL))
		UpdateButtonStates();
}

/**
 * @brief Handle timer events.
 * Checks if paths are valid and sets control states accordingly.
 * @param [in] nIDEvent Timer ID that fired.
 */
void COpenDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == IDT_CHECKFILES)
		UpdateButtonStates();

	CDialog::OnTimer(nIDEvent);
}

/**
 * @brief Called when users selects plugin browse button.
 */
void COpenDlg::OnSelectUnpacker()
{
	UpdateData(TRUE);

	m_pathsType = GetPairComparability(m_strLeft, m_strRight);

	if (m_pathsType != IS_EXISTING_FILE) 
		return;

	// let the user select a handler
	CSelectUnpackerDlg dlg(m_strLeft, m_strRight, this);
	dlg.SetInitialInfoHandler(&m_infoHandler);

	if (dlg.DoModal() == IDOK)
	{
		m_infoHandler = dlg.GetInfoHandler();

		m_strUnpacker = m_infoHandler.pluginName.c_str();

		UpdateData(FALSE);
	}
}

/**
 * @brief Sets the path status text.
 * The open dialog shows a status text of selected paths. This function
 * is used to set that status text.
 * @param [in] msgID Resource ID of status text to set.
 */
void COpenDlg::SetStatus(UINT msgID)
{
	String msg = theApp.LoadString(msgID);
	SetDlgItemText(IDC_OPEN_STATUS, msg.c_str());
}

/**
 * @brief Set the plugin edit box text.
 * Plugin edit box is at the same time a plugin status view. This function
 * sets the status text.
 * @param [in] msgID Resource ID of status text to set.
 */
void COpenDlg::SetUnpackerStatus(UINT msgID)
{
	String msg = theApp.LoadString(msgID);
	SetDlgItemText(IDC_UNPACKER_EDIT, msg.c_str());
}

/** 
 * @brief Called when "Select..." button for filters is selected.
 */
void COpenDlg::OnSelectFilter()
{
	String filterPrefix = theApp.LoadString(IDS_FILTER_PREFIX);
	CString curFilter;

	const BOOL bUseMask = theApp.m_globalFileFilter.IsUsingMask();
	GetDlgItemText(IDC_EXT_COMBO, curFilter);
	curFilter.TrimLeft();
	curFilter.TrimRight();

	GetMainFrame()->SelectFilter();
	
	CString FilterNameOrMask = theApp.m_globalFileFilter.GetFilterNameOrMask();
	if (theApp.m_globalFileFilter.IsUsingMask())
	{
		// If we had filter chosen and now has mask we can overwrite filter
		if (!bUseMask || curFilter[0] != '*')
		{
			SetDlgItemText(IDC_EXT_COMBO, FilterNameOrMask);
		}
	}
	else
	{
		FilterNameOrMask.Insert(0, filterPrefix.c_str());
		SetDlgItemText(IDC_EXT_COMBO, FilterNameOrMask);
	}
}


/** 
 * @brief Read paths and filter from project file.
 * Reads the given project file. After the file is read, found paths and
 * filter is updated to dialog GUI. Other possible settings found in the
 * project file are kept in memory and used later when loading paths
 * selected.
 * @param [in] path Path to the project file.
 * @return TRUE if the project file was successfully loaded, FALSE otherwise.
 */
BOOL COpenDlg::LoadProjectFile(const CString &path)
{
	String filterPrefix = theApp.LoadString(IDS_FILTER_PREFIX);
	String err;

	m_pProjectFile = new ProjectFile;
	if (m_pProjectFile == NULL)
		return FALSE;

	if (!m_pProjectFile->Read(path, &err))
	{
		if (!err.empty())
		{
			CString msg;
			LangFormatString2(msg, IDS_ERROR_FILEOPEN, path, err.c_str());
			AfxMessageBox(msg, MB_ICONSTOP);
		}
		return FALSE;
	}
	else
	{
		m_pProjectFile->GetPaths(m_strLeft, m_strRight, m_bRecurse);
		if (m_pProjectFile->HasFilter())
		{
			m_strExt = m_pProjectFile->GetFilter();
			m_strExt.TrimLeft();
			m_strExt.TrimRight();
			if (m_strExt[0] != '*')
				m_strExt.Insert(0, filterPrefix.c_str());
		}
	}
	return TRUE;
}

/** 
 * @brief Removes whitespaces from left and right paths
 * @note Assumes UpdateData(TRUE) is called before this function.
 */
void COpenDlg::TrimPaths()
{
	m_strLeft.TrimLeft();
	m_strLeft.TrimRight();
	m_strRight.TrimLeft();
	m_strRight.TrimRight();
}

/** 
 * @brief Update control states when dialog is activated.
 *
 * Update control states when user re-activates dialog. User might have
 * switched for other program to e.g. update files/folders and then
 * swiches back to WinMerge. Its nice to see WinMerge detects updated
 * files/folders.
 */
void COpenDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE)
		UpdateButtonStates();
}

/**
 * @brief Open help from mainframe when user presses F1.
 */
void COpenDlg::OnHelp()
{
	GetMainFrame()->ShowHelp(OpenDlgHelpLocation);
}

/////////////////////////////////////////////////////////////////////////////
//
//	OnDropFiles code from CDropEdit
//	Copyright 1997 Chris Losinger
//
//	shortcut expansion code modified from :
//	CShortcut, 1996 Rob Warner
//

/**
 * @brief Drop paths(s) to the dialog.
 * One or two paths can be dropped to the dialog. The behaviour is:
 *   If 1 file:
 *     - drop to empty path edit box (check left first)
 *     - if both boxes have a path, drop to left path
 *   If two files:
 *    - overwrite both paths, empty or not
 * @param [in] dropInfo Dropped data, including paths.
 */
void COpenDlg::OnDropFiles(HDROP dropInfo)
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
		if (paths_IsShortcut((LPCTSTR)files[i]))
		{
			// if this was a shortcut, we need to expand it to the target path
			CString expandedFile = ExpandShortcut((LPCTSTR)files[i]).c_str();

			// if that worked, we should have a real file name
			if (!expandedFile.IsEmpty())
				files[i] = expandedFile;
		}
	}

	// Add dropped paths to the dialog
	UpdateData(TRUE);
	if (fileCount == 2)
	{
		m_strLeft = files[0];
		m_strRight = files[1];
		UpdateData(FALSE);
		UpdateButtonStates();
	}
	else if (fileCount == 1)
	{
		if (m_strLeft.IsEmpty())
			m_strLeft = files[0];
		else if (m_strRight.IsEmpty())
			m_strRight = files[0];
		else
			m_strLeft = files[0];
		UpdateData(FALSE);
		UpdateButtonStates();
	}
}
