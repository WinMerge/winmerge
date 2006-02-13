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
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include <sys/types.h>
#include <sys/stat.h>

#include "Merge.h"
#include "OpenDlg.h"
#include "coretools.h"
#include "paths.h"
#include "SelectUnpackerDlg.h"
#include "OptionsDef.h"
#include "MainFrm.h"
#include "OptionsMgr.h"
#include "ProjectFile.h"
#include "dlgutil.h"

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

/////////////////////////////////////////////////////////////////////////////
// COpenDlg dialog


COpenDlg::COpenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenDlg)
	m_bRecurse = FALSE;
	//}}AFX_DATA_INIT

	m_pathsType = DOES_NOT_EXIST;
	m_bOverwriteRecursive = FALSE;
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
	m_constraint.DisallowHeightGrowth();
	m_constraint.SubclassWnd(); // install subclassing
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("OpenDlg"), false); // persist size via registry

	dlgutil_CenterToMainFrame(this);

	m_ctlLeft.LoadState(_T("Files\\Left"));
	m_ctlRight.LoadState(_T("Files\\Right"));
	m_ctlExt.LoadState(_T("Files\\Ext"));
	UpdateData(m_strLeft.IsEmpty() && m_strRight.IsEmpty());
	
	CString FilterNameOrMask = theApp.m_globalFileFilter.GetFilterNameOrMask();
	BOOL bMask = theApp.m_globalFileFilter.IsUsingMask();

	if (!bMask)
	{
		CString filterPrefix;
		VERIFY(filterPrefix.LoadString(IDS_FILTER_PREFIX));
		FilterNameOrMask.Insert(0, filterPrefix);
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

	m_strUnpacker = m_infoHandler.pluginName;
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
	CString sfolder, sext;
	CString dirSelTag;
	UpdateData(TRUE); 


	VERIFY(dirSelTag.LoadString(IDS_DIRSEL_TAG));
	if (paths_DoesPathExist(m_strLeft) == IS_EXISTING_DIR)
		sfolder = m_strLeft;
	else
		sfolder = GetPathOnly(m_strLeft);
	if (SelectFile(s, sfolder))
	{
		SplitFilename(s, NULL, NULL, &sext);
		if (sext.CompareNoCase(PROJECTFILE_EXT) == 0)
			LoadProjectFile(s);
		else
			m_strLeft = s;
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
	CString sfolder, sext;
	CString dirSelTag;
	UpdateData(TRUE);

	VERIFY(dirSelTag.LoadString(IDS_DIRSEL_TAG));
	if (paths_DoesPathExist(m_strRight) == IS_EXISTING_DIR)
		sfolder = m_strRight;
	else 
		sfolder = GetPathOnly(m_strRight);
	if (SelectFile(s, sfolder))
	{
		SplitFilename(s, NULL, NULL, &sext);
		if (sext.CompareNoCase(PROJECTFILE_EXT) == 0)
			LoadProjectFile(s);
		else
			m_strRight = s;
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
	CString filterPrefix;
	VERIFY(filterPrefix.LoadString(IDS_FILTER_PREFIX));

	UpdateData(TRUE);
	TrimPaths();

	m_pathsType = GetPairComparability(m_strLeft, m_strRight);

	if (m_pathsType == DOES_NOT_EXIST)
	{
		AfxMessageBox(IDS_ERROR_INCOMPARABLE, MB_ICONSTOP);
		return;
	}

	m_strRight = paths_GetLongPath(m_strRight);
	m_strLeft = paths_GetLongPath(m_strLeft);

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

	m_strExt.TrimLeft();
	m_strExt.TrimRight();

	// If prefix found from start..
	if (m_strExt.Find(filterPrefix, 0) == 0)
	{
		// Remove prefix + space
		m_strExt.Delete(0, filterPrefix.GetLength());
		if (!theApp.m_globalFileFilter.SetFilter(m_strExt))
		{
			// If filtername is not found use default *.* mask
			theApp.m_globalFileFilter.SetFilter(_T("*.*"));
			m_strExt = _T("*.*");
		}
		GetOptionsMgr()->SaveOption(OPT_FILEFILTER_CURRENT, m_strExt);
	}
	else
	{
		BOOL bFilterSet = theApp.m_globalFileFilter.SetFilter(m_strExt);
		if (!bFilterSet)
			m_strExt = theApp.m_globalFileFilter.GetFilterNameOrMask();
		GetOptionsMgr()->SaveOption(OPT_FILEFILTER_CURRENT, m_strExt);
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
	
	// Enable buttons as appropriate
	PATH_EXISTENCE pathsType = GetPairComparability(m_strLeft, m_strRight);
	if (GetOptionsMgr()->GetBool(OPT_VERIFY_OPEN_PATHS))
	{
		m_ctlOk.EnableWindow(pathsType != DOES_NOT_EXIST);
		m_ctlUnpacker.EnableWindow(pathsType == IS_EXISTING_FILE);
		m_ctlSelectUnpacker.EnableWindow(pathsType == IS_EXISTING_FILE);
	}

	if (paths_DoesPathExist(m_strLeft) == DOES_NOT_EXIST)
		bLeftInvalid = TRUE;
	if (paths_DoesPathExist(m_strRight) == DOES_NOT_EXIST)
		bRightInvalid = TRUE;

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

	if (pathsType == IS_EXISTING_FILE)
		SetUnpackerStatus(0);	//Empty field
	else
		SetUnpackerStatus(IDS_OPEN_UNPACKERDISABLED);
}

/** 
 * @brief Shows file/folder selection dialog.
 *
 * We need this custom function so we can select files and folders with the
 * same dialog.
 * - If existing filename is selected return it
 * - If filename in (CFileDialog) editbox and current folder doesn't form
 * a valid path to file, return current folder.
 * @param [out] path Selected folder/filename
 * @param [in] pszFolder Initial folder shown
 * @return TRUE if user choosed a file/folder, FALSE if user canceled dialog.
 */
BOOL COpenDlg::SelectFile(CString& path, LPCTSTR pszFolder) 
{
	TCHAR filterStr[MAX_PATH] = {0};
	TCHAR fileStr[MAX_PATH] = {0};
	CString s;
	CString dirSelTag;
	CString title;

	VERIFY(dirSelTag.LoadString(IDS_DIRSEL_TAG));
	VERIFY(s.LoadString(IDS_ALLFILES));
	VERIFY(title.LoadString(IDS_OPEN_TITLE));

	// Set initial filename to folder selection tag
	dirSelTag += _T("."); // Treat it as filename
	_tcsncpy(fileStr, dirSelTag, dirSelTag.GetLength() + 1);
	// Convert extension mask
	_tcsncpy(filterStr, s, s.GetLength());
	ConvertFilter(filterStr);

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetSafeHwnd();
	ofn.lpstrFilter = filterStr;
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = fileStr;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = pszFolder;
	ofn.lpstrTitle = title;
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOTESTFILECREATE;

	BOOL bRetVal = GetOpenFileName(&ofn);
	SetCurrentDirectory(paths_GetWindowsDirectory()); // Free handle held by GetOpenFileName

	if (bRetVal)
	{
		path = fileStr;
		struct _stati64 statBuffer;
		int nRetVal = _tstati64(path, &statBuffer);
		if (nRetVal == -1)
		{
			// We have a valid folder name, but propably garbage as a filename.
			// Return folder name
			CString folder = GetPathOnly(fileStr);
			path = folder + '\\';
		}
	}
	return bRetVal;
}

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

void COpenDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_CHECKFILES)
		UpdateButtonStates();

	CDialog::OnTimer(nIDEvent);
}

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

		m_strUnpacker = m_infoHandler.pluginName;

		UpdateData(FALSE);
	}
}

void COpenDlg::SetStatus(UINT msgID)
{
	CString msg;
	if (msgID > 0)
		VERIFY(msg.LoadString(msgID));
	SetDlgItemText(IDC_OPEN_STATUS, msg);
}

void COpenDlg::SetUnpackerStatus(UINT msgID)
{
	CString msg;
	if (msgID > 0)
		VERIFY(msg.LoadString(msgID));
	SetDlgItemText(IDC_UNPACKER_EDIT, msg);
}

/** 
 * @brief Called when "Select..." button for filters is selected.
 */
void COpenDlg::OnSelectFilter()
{
	CString filterPrefix;
	CString curFilter;
	VERIFY(filterPrefix.LoadString(IDS_FILTER_PREFIX));

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
		FilterNameOrMask.Insert(0, filterPrefix);
		SetDlgItemText(IDC_EXT_COMBO, FilterNameOrMask);
	}
}


/** 
 * @brief Read paths and filter from project file.
 */
BOOL COpenDlg::LoadProjectFile(CString path)
{
	CString filterPrefix;
	CString err;
	ProjectFile pfile;

	VERIFY(filterPrefix.LoadString(IDS_FILTER_PREFIX));
	if (!pfile.Read(path, &err))
	{
		if (!err.IsEmpty())
		{
			CString msg;
			AfxFormatString2(msg, IDS_ERROR_FILEOPEN, path, err);
			AfxMessageBox(msg, MB_ICONSTOP);
		}
		return FALSE;
	}
	else
	{
		pfile.GetPaths(m_strLeft, m_strRight, m_bRecurse);
		if (pfile.HasFilter())
		{
			m_strExt = pfile.GetFilter();
			m_strExt.TrimLeft();
			m_strExt.TrimRight();
			if (m_strExt[0] != '*')
				m_strExt.Insert(0, filterPrefix);
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
