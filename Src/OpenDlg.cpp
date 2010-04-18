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
// $Id: OpenDlg.cpp 6861 2009-06-25 12:11:07Z kimmov $

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
#include "7zCommon.h"

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
	, m_pUpdateButtonStatusThread(NULL)
{
}

/**
 * @brief Standard destructor.
 */
COpenDlg::~COpenDlg()
{
	TerminateThreadIfRunning();
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
	DDX_Control(pDX, IDC_PATH0_COMBO, m_ctlPath[0]);
	DDX_Control(pDX, IDC_PATH1_COMBO, m_ctlPath[1]);
	DDX_Control(pDX, IDC_PATH2_COMBO, m_ctlPath[2]);
	DDX_CBStringExact(pDX, IDC_PATH0_COMBO, m_strPath[0]);
	DDX_CBStringExact(pDX, IDC_PATH1_COMBO, m_strPath[1]);
	DDX_CBStringExact(pDX, IDC_PATH2_COMBO, m_strPath[2]);
	DDX_Check(pDX, IDC_RECURS_CHECK, m_bRecurse);
	DDX_CBStringExact(pDX, IDC_EXT_COMBO, m_strExt);
	DDX_Text(pDX, IDC_UNPACKER_EDIT, m_strUnpacker);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenDlg, CDialog)
	//{{AFX_MSG_MAP(COpenDlg)
	ON_BN_CLICKED(IDC_PATH0_BUTTON, OnPath0Button)
	ON_BN_CLICKED(IDC_PATH1_BUTTON, OnPath1Button)
	ON_BN_CLICKED(IDC_PATH2_BUTTON, OnPath2Button)
	ON_CBN_SELCHANGE(IDC_PATH0_COMBO, OnSelchangePath0Combo)
	ON_CBN_SELCHANGE(IDC_PATH1_COMBO, OnSelchangePath1Combo)
	ON_CBN_SELCHANGE(IDC_PATH2_COMBO, OnSelchangePath2Combo)
	ON_CBN_EDITCHANGE(IDC_PATH0_COMBO, OnEditEvent)
	ON_CBN_EDITCHANGE(IDC_PATH1_COMBO, OnEditEvent)
	ON_CBN_EDITCHANGE(IDC_PATH2_COMBO, OnEditEvent)
	ON_BN_CLICKED(IDC_SELECT_UNPACKER, OnSelectUnpacker)
	ON_CBN_SELENDCANCEL(IDC_PATH0_COMBO, UpdateButtonStates)
	ON_CBN_SELENDCANCEL(IDC_PATH1_COMBO, UpdateButtonStates)
	ON_CBN_SELENDCANCEL(IDC_PATH2_COMBO, UpdateButtonStates)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SELECT_FILTER, OnSelectFilter)
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_HELP, OnHelp)
	ON_WM_DROPFILES()
	ON_MESSAGE(WM_USER + 1, OnUpdateStatus)
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
	m_constraint.ConstrainItem(IDC_PATH0_COMBO, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_PATH1_COMBO, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_PATH2_COMBO, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_EXT_COMBO, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_UNPACKER_EDIT, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_FILES_DIRS_GROUP, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_PATH0_BUTTON, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDC_PATH1_BUTTON, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDC_PATH2_BUTTON, 1, 0, 0, 0); // slides right
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

	for (int file = 0; file < m_files.GetSize(); file++)
	{
		m_strPath[file] = m_files[file].c_str();
		m_ctlPath[file].SetWindowText(m_files[file].c_str());
	}

	m_ctlPath[0].AttachSystemImageList();
	m_ctlPath[1].AttachSystemImageList();
	m_ctlPath[2].AttachSystemImageList();
	m_ctlPath[0].LoadState(_T("Files\\Left"));
	m_ctlPath[1].LoadState(_T("Files\\Right"));
	m_ctlPath[2].LoadState(_T("Files\\Option"));
	m_ctlExt.LoadState(_T("Files\\Ext"));
	
	BOOL bIsEmptyThirdItem = theApp.GetProfileInt(_T("Files\\Option"), _T("Empty"), TRUE);
	if (bIsEmptyThirdItem)
		m_ctlPath[2].SetWindowText(_T(""));
	
	BOOL bDoUpdateData = TRUE;
	for (int index = 0; index < countof(m_strPath); index++)
	{
		if (!m_strPath[index].IsEmpty())
			bDoUpdateData = FALSE;
	}
	UpdateData(bDoUpdateData);
	
	int nSource = GetOptionsMgr()->GetInt(OPT_AUTO_COMPLETE_SOURCE);
	if (nSource > 0)
	{
		m_ctlPath[0].SetAutoComplete(nSource);
		m_ctlPath[1].SetAutoComplete(nSource);
		m_ctlPath[2].SetAutoComplete(nSource);
	}

	String filterNameOrMask = theApp.m_globalFileFilter.GetFilterNameOrMask();
	BOOL bMask = theApp.m_globalFileFilter.IsUsingMask();

	if (!bMask)
	{
		String filterPrefix = theApp.LoadString(IDS_FILTER_PREFIX);
		filterNameOrMask = filterPrefix + filterNameOrMask;
	}

	int ind = m_ctlExt.FindStringExact(0, filterNameOrMask.c_str());
	if (ind != CB_ERR)
		m_ctlExt.SetCurSel(ind);
	else
	{
		ind = m_ctlExt.InsertString(0, filterNameOrMask.c_str());
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

void COpenDlg::OnButton(int index)
{
	CString s;
	String sfolder;
	UpdateData(TRUE); 

	PATH_EXISTENCE existence = paths_DoesPathExist(m_strPath[index]);
	switch (existence)
	{
	case IS_EXISTING_DIR:
		sfolder = m_strPath[index];
		break;
	case IS_EXISTING_FILE:
		sfolder = GetPathOnly(m_strPath[index]);
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
		m_strPath[index] = s;
		m_strBrowsePath[index] = s;
		UpdateData(FALSE);
		UpdateButtonStates();
	}	
}

/** 
 * @brief Called when "Browse..." button is selected for first path.
 */
void COpenDlg::OnPath0Button()
{
	OnButton(0);
}

/** 
 * @brief Called when "Browse..." button is selected for second path.
 */
void COpenDlg::OnPath1Button() 
{
	OnButton(1);
}

/** 
 * @brief Called when "Browse..." button is selected for third path.
 */
void COpenDlg::OnPath2Button() 
{
	OnButton(2);
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
	SplitFilename(m_strPath[0], NULL, NULL, &ext);
	CString sExt(ext.c_str());
	if (m_strPath[1].IsEmpty() && sExt.CompareNoCase(PROJECTFILE_EXT) == 0)
		LoadProjectFile(m_strPath[0]);

	int index;
	int nFiles = 0;
	for (index = 0; index < countof(m_strPath); index++)
	{
		if (index == 2 && m_strPath[index].IsEmpty())
			break;
		m_files.SetSize(nFiles + 1);
		m_files[nFiles] = m_strPath[index];
		nFiles++;
	}
	m_pathsType = GetPairComparability(m_files, IsArchiveFile);

	if (m_pathsType == DOES_NOT_EXIST)
	{
		LangMessageBox(IDS_ERROR_INCOMPARABLE, MB_ICONSTOP);
		return;
	}

	for (index = 0; index < nFiles; index++)
	{
		// If user has edited path by hand, expand environment variables
		BOOL bExpand = FALSE;
		if (m_strBrowsePath[index].CompareNoCase(m_files[index].c_str()) != 0)
			bExpand = TRUE;

		m_files[index] = paths_GetLongPath(m_files[index].c_str(), bExpand);
	
		// Add trailing '\' for directories if its missing
		if (paths_DoesPathExist(m_files[index].c_str()) == IS_EXISTING_DIR)
		{
			if (!paths_EndsWithSlash(m_files[index].c_str()))
				m_files[index] += '\\';
		}
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
			m_strExt = theApp.m_globalFileFilter.GetFilterNameOrMask().c_str();
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
	m_ctlPath[0].SaveState(_T("Files\\Left"));
	m_ctlPath[1].SaveState(_T("Files\\Right"));
	m_ctlPath[2].SaveState(_T("Files\\Option"));
	m_ctlExt.SaveState(_T("Files\\Ext"));

	CString strOption;
	m_ctlPath[2].GetWindowText(strOption);
	theApp.WriteProfileInt(_T("Files\\Option"), _T("Empty"), strOption.IsEmpty());
}

struct UpdateButtonStatesThreadParams
{
	HWND m_hWnd;
	PathContext m_paths;
};

UINT UpdateButtonStatesThread(LPVOID lpParam)
{
	MSG msg;
	BOOL bRet;
	while( (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
	{ 
		if (bRet == -1)
			break;
		if (msg.message != WM_USER)
			continue;

		BOOL bButtonEnabled = TRUE;
		BOOL bInvalid[3] = {FALSE, FALSE, FALSE};
		int iStatusMsgId;
		int iUnpackerStatusMsgId;

		UpdateButtonStatesThreadParams *pParams = (UpdateButtonStatesThreadParams *)msg.wParam;
		PathContext paths = pParams->m_paths;
		HWND hWnd = pParams->m_hWnd;
		delete pParams;

		// Check if we have project file as left side path
		BOOL bProject = FALSE;
		String ext;
		SplitFilename(paths[0].c_str(), NULL, NULL, &ext);
		CString sExt(ext.c_str());
		if (paths[1].empty() && sExt.CompareNoCase(PROJECTFILE_EXT) == 0)
			bProject = TRUE;

		if (!bProject)
		{
			if (paths_DoesPathExist(paths[0].c_str()) == DOES_NOT_EXIST)
				bInvalid[0] = TRUE;
			if (paths_DoesPathExist(paths[1].c_str()) == DOES_NOT_EXIST)
				bInvalid[1] = TRUE;
			if (paths.GetSize() > 2 && paths_DoesPathExist(paths[2].c_str()) == DOES_NOT_EXIST)
				bInvalid[2] = TRUE;
		}

		PATH_EXISTENCE pathsType = DOES_NOT_EXIST;

		if (paths.GetSize() <= 2)
		{
			if (bInvalid[0] && bInvalid[1])
				iStatusMsgId = IDS_OPEN_BOTHINVALID;
			else if (bInvalid[0])
				iStatusMsgId = IDS_OPEN_LEFTINVALID;
			else if (bInvalid[1])
				iStatusMsgId = IDS_OPEN_RIGHTINVALID;
			else if (!bInvalid[0] && !bInvalid[1])
			{
				pathsType = GetPairComparability(paths, IsArchiveFile);
				if (pathsType == DOES_NOT_EXIST)
					iStatusMsgId = IDS_OPEN_MISMATCH;
				else
					iStatusMsgId = IDS_OPEN_FILESDIRS;
			}
		}
		else
		{
			if (bInvalid[0] && bInvalid[1] && bInvalid[2])
				iStatusMsgId = IDS_OPEN_ALLINVALID;
			else if (!bInvalid[0] && bInvalid[1] && bInvalid[2])
				iStatusMsgId = IDS_OPEN_MIDDLERIGHTINVALID;
			else if (bInvalid[0] && !bInvalid[1] && bInvalid[2])
				iStatusMsgId = IDS_OPEN_LEFTRIGHTINVALID;
			else if (!bInvalid[0] && !bInvalid[1] && bInvalid[2])
				iStatusMsgId = IDS_OPEN_RIGHTINVALID;
			else if (bInvalid[0] && bInvalid[1] && !bInvalid[2])
				iStatusMsgId = IDS_OPEN_LEFTMIDDLEINVALID;
			else if (!bInvalid[0] && bInvalid[1] && !bInvalid[2])
				iStatusMsgId = IDS_OPEN_MIDDLEINVALID;
			else if (bInvalid[0] && !bInvalid[1] && !bInvalid[2])
				iStatusMsgId = IDS_OPEN_LEFTINVALID;
			else if (!bInvalid[0] && !bInvalid[1] && !bInvalid[2])
			{
				pathsType = GetPairComparability(paths, IsArchiveFile);
				if (pathsType == DOES_NOT_EXIST)
					iStatusMsgId = IDS_OPEN_MISMATCH;
				else
					iStatusMsgId = IDS_OPEN_FILESDIRS;
			}
		}
		if (pathsType == IS_EXISTING_FILE || bProject)
			iUnpackerStatusMsgId = 0;	//Empty field
		else
			iUnpackerStatusMsgId = IDS_OPEN_UNPACKERDISABLED;

		// Enable buttons as appropriate
		if (GetOptionsMgr()->GetBool(OPT_VERIFY_OPEN_PATHS))
		{
			if (bProject)
				bButtonEnabled = TRUE;
			else
				bButtonEnabled = (pathsType != DOES_NOT_EXIST);
		}

		PostMessage(hWnd, WM_USER + 1, bButtonEnabled, MAKELPARAM(iStatusMsgId, iUnpackerStatusMsgId)); 
	}

	return 0;
}

/** 
 * @brief Enable/disable components based on validity of paths.
 */
void COpenDlg::UpdateButtonStates()
{
	UpdateData(TRUE); // load member variables from screen
	KillTimer(IDT_CHECKFILES);
	TrimPaths();
	
	if (!m_pUpdateButtonStatusThread)
	{
		m_pUpdateButtonStatusThread = AfxBeginThread(
			UpdateButtonStatesThread, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		m_pUpdateButtonStatusThread->m_bAutoDelete = FALSE;
		m_pUpdateButtonStatusThread->ResumeThread();
		while (PostThreadMessage(m_pUpdateButtonStatusThread->m_nThreadID, WM_NULL, 0, 0) == FALSE)
			Sleep(1);
	}

	UpdateButtonStatesThreadParams *pParams = new UpdateButtonStatesThreadParams;
	pParams->m_hWnd = this->m_hWnd;
	if (m_strPath[2].IsEmpty())
		pParams->m_paths = PathContext(m_strPath[0], m_strPath[1]);
	else
		pParams->m_paths = PathContext(m_strPath[0], m_strPath[1], m_strPath[2]);

	PostThreadMessage(m_pUpdateButtonStatusThread->m_nThreadID, WM_USER, (WPARAM)pParams, 0);
}

void COpenDlg::TerminateThreadIfRunning()
{
	if (!m_pUpdateButtonStatusThread)
		return;

	PostThreadMessage(m_pUpdateButtonStatusThread->m_nThreadID, WM_QUIT, 0, 0);
	DWORD dwResult = WaitForSingleObject(m_pUpdateButtonStatusThread->m_hThread, 100);
	if (dwResult != WAIT_OBJECT_0)
	{
		m_pUpdateButtonStatusThread->SuspendThread();
		TerminateThread(m_pUpdateButtonStatusThread->m_hThread, 0);
	}
	delete m_pUpdateButtonStatusThread;
	m_pUpdateButtonStatusThread = NULL;
}

/**
 * @brief Called when user changes selection in left/middle/right path's combo box.
 */
void COpenDlg::OnSelchangeCombo(int index) 
{
	int sel = m_ctlPath[index].GetCurSel();
	if (sel != CB_ERR)
	{
		m_ctlPath[index].GetLBText(sel, m_strPath[index]);
		m_ctlPath[index].SetWindowText(m_strPath[index]);
		UpdateData(TRUE);
	}
	UpdateButtonStates();
}

void COpenDlg::OnSelchangePath0Combo() 
{
	OnSelchangeCombo(0);
}

void COpenDlg::OnSelchangePath1Combo() 
{
	OnSelchangeCombo(1);
}

void COpenDlg::OnSelchangePath2Combo() 
{
	OnSelchangeCombo(2);
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

	int index;
	int nFiles = 0;
	for (index = 0; index < countof(m_strPath); index++)
	{
		if (index == 2 && m_strPath[index].IsEmpty())
			break;
		m_files.SetSize(nFiles + 1);
		m_files[nFiles] = m_strPath[index];
		nFiles++;
	}
	m_pathsType = GetPairComparability(m_files);

	if (m_pathsType != IS_EXISTING_FILE) 
		return;

	// let the user select a handler
	CSelectUnpackerDlg dlg(m_files[0].c_str(), this);
	dlg.SetInitialInfoHandler(&m_infoHandler);

	if (dlg.DoModal() == IDOK)
	{
		m_infoHandler = dlg.GetInfoHandler();

		m_strUnpacker = m_infoHandler.pluginName.c_str();

		UpdateData(FALSE);
	}
}

LRESULT COpenDlg::OnUpdateStatus(WPARAM wParam, LPARAM lParam)
{
	BOOL bEnabledButtons = (BOOL)wParam;

	m_ctlOk.EnableWindow(bEnabledButtons);
	m_ctlUnpacker.EnableWindow(bEnabledButtons);
	m_ctlSelectUnpacker.EnableWindow(bEnabledButtons);

	SetStatus(HIWORD(lParam));
	SetStatus(LOWORD(lParam));

	return 0;
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
	
	String filterNameOrMask = theApp.m_globalFileFilter.GetFilterNameOrMask();
	if (theApp.m_globalFileFilter.IsUsingMask())
	{
		// If we had filter chosen and now has mask we can overwrite filter
		if (!bUseMask || curFilter[0] != '*')
		{
			SetDlgItemText(IDC_EXT_COMBO, filterNameOrMask.c_str());
		}
	}
	else
	{
		filterNameOrMask = filterPrefix + filterNameOrMask;
		SetDlgItemText(IDC_EXT_COMBO, filterNameOrMask.c_str());
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
		m_pProjectFile->GetPaths(m_files, m_bRecurse);
		if (m_pProjectFile->HasFilter())
		{
			m_strExt = m_pProjectFile->GetFilter().c_str();
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
	for (int index = 0; index < countof(m_strPath); index++)
	{
		m_strPath[index].TrimLeft();
		m_strPath[index].TrimRight();
	}
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
	if (fileCount == 3)
	{
		m_strPath[0] = files[0];
		m_strPath[1] = files[1];
		m_strPath[2] = files[2];
		UpdateData(FALSE);
		UpdateButtonStates();
	}
	else if (fileCount == 2)
	{
		m_strPath[0] = files[0];
		m_strPath[1] = files[1];
		UpdateData(FALSE);
		UpdateButtonStates();
	}
	else if (fileCount == 1)
	{
		if (m_strPath[0].IsEmpty())
			m_strPath[0] = files[0];
		else if (m_strPath[1].IsEmpty())
			m_strPath[1] = files[0];
		else if (m_strPath[2].IsEmpty())
			m_strPath[2] = files[0];
		else
			m_strPath[0] = files[0];
		UpdateData(FALSE);
		UpdateButtonStates();
	}
}
