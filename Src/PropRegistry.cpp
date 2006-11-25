/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  PropRegistry.cpp
 *
 * @brief CPropRegistry implementation file
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "resource.h"
#include "PropRegistry.h"
#include "RegKey.h"
#include "coretools.h"
#include "FileOrFolderSelect.h"
#include "MainFrm.h" // GetDefaultEditor()
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/// Flags for enabling and mode of extension
#define CONTEXT_F_ENABLED 0x01
#define CONTEXT_F_ADVANCED 0x02
#define CONTEXT_F_SUBFOLDERS 0x04

// registry dir to WinMerge
static LPCTSTR f_RegDir = _T("Software\\Thingamahoochie\\WinMerge");

// registry values
static LPCTSTR f_RegValueEnabled = _T("ContextMenuEnabled");
static LPCTSTR f_RegValuePath = _T("Executable");

/////////////////////////////////////////////////////////////////////////////
// CPropRegistry dialog


CPropRegistry::CPropRegistry(COptionsMgr *optionsMgr)
	: CPropertyPage(CPropRegistry::IDD)
, m_pOptionsMgr(optionsMgr)
, m_bContextAdded(FALSE)
, m_bUseRecycleBin(TRUE)
, m_bContextAdvanced(FALSE)
, m_bIgnoreSmallTimeDiff(FALSE)
, m_bContextSubfolders(FALSE)
{
}

void CPropRegistry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropRegistry)
	DDX_Check(pDX, IDC_EXPLORER_CONTEXT, m_bContextAdded);
	DDX_Text(pDX, IDC_EXT_EDITOR_PATH, m_strEditorPath);
	DDX_Check(pDX, IDC_USE_RECYCLE_BIN, m_bUseRecycleBin);
	DDX_Check(pDX, IDC_EXPLORER_ADVANCED, m_bContextAdvanced);
	DDX_Check(pDX, IDC_IGNORE_SMALLTIMEDIFF, m_bIgnoreSmallTimeDiff);
	DDX_Text(pDX, IDC_FILTER_USER_PATH, m_strUserFilterPath);
	DDX_Check(pDX, IDC_EXPLORER_SUBFOLDERS, m_bContextSubfolders);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPropRegistry, CDialog)
	//{{AFX_MSG_MAP(CPropRegistry)
	ON_BN_CLICKED(IDC_EXPLORER_CONTEXT, OnAddToExplorer)
	ON_BN_CLICKED(IDC_EXT_EDITOR_BROWSE, OnBrowseEditor)
	ON_BN_CLICKED(IDC_FILTER_USER_BROWSE, OnBrowseFilterPath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void CPropRegistry::ReadOptions()
{
	m_strEditorPath = m_pOptionsMgr->GetString(OPT_EXT_EDITOR_CMD);
	GetContextRegValues();
	m_bUseRecycleBin = m_pOptionsMgr->GetBool(OPT_USE_RECYCLE_BIN);
	m_bIgnoreSmallTimeDiff = m_pOptionsMgr->GetBool(OPT_IGNORE_SMALL_FILETIME);
	m_strUserFilterPath = m_pOptionsMgr->GetString(OPT_FILTER_USERPATH);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void CPropRegistry::WriteOptions()
{
	CString sDefaultEditor = GetMainFrame()->GetDefaultEditor();

	m_pOptionsMgr->SaveOption(OPT_USE_RECYCLE_BIN, m_bUseRecycleBin == TRUE);
	m_pOptionsMgr->SaveOption(OPT_IGNORE_SMALL_FILETIME, m_bIgnoreSmallTimeDiff == TRUE);

	SaveMergePath(); // saves context menu settings as well

	CString sExtEditor = m_strEditorPath;
	sExtEditor.TrimLeft();
	sExtEditor.TrimRight();
	if (sExtEditor.IsEmpty())
		sExtEditor = sDefaultEditor;
	m_pOptionsMgr->SaveOption(OPT_EXT_EDITOR_CMD, sExtEditor);

	CString sFilterPath = m_strUserFilterPath;
	sFilterPath.TrimLeft();
	sFilterPath.TrimRight();
	m_pOptionsMgr->SaveOption(OPT_FILTER_USERPATH, sFilterPath);
}

/////////////////////////////////////////////////////////////////////////////
// CPropRegistry message handlers

BOOL CPropRegistry::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Update shell extension checkboxes
	GetContextRegValues();
	AdvancedContextMenuCheck();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/// Get registry values for ShellExtension
void CPropRegistry::GetContextRegValues()
{
	CRegKeyEx reg;
	LONG retVal = 0;
	retVal = reg.Open(HKEY_CURRENT_USER, f_RegDir);
	if (retVal != ERROR_SUCCESS)
	{
		CString msg;
		msg.Format(_T("Failed to open registry key HKCU/%s:\n\t%d : %s"),
			f_RegDir, retVal, GetSysError(retVal));
		LogErrorString(msg);
		return;
	}

	// Read bitmask for shell extension settings
	DWORD dwContextEnabled = reg.ReadDword(f_RegValueEnabled, 0);

	if (dwContextEnabled & CONTEXT_F_ENABLED)
		m_bContextAdded = TRUE;

	if (dwContextEnabled & CONTEXT_F_ADVANCED)
		m_bContextAdvanced = TRUE;

	if (dwContextEnabled & CONTEXT_F_SUBFOLDERS)
		m_bContextSubfolders = TRUE;
}

/// Set registry values for ShellExtension
void CPropRegistry::OnAddToExplorer()
{
	AdvancedContextMenuCheck();
	SubfolderOptionCheck();
}

/// Saves given path to registry for ShellExtension, and Context Menu settings
void CPropRegistry::SaveMergePath()
{
	TCHAR temp[MAX_PATH] = {0};
	LONG retVal = 0;
	GetModuleFileName(AfxGetInstanceHandle(), temp, MAX_PATH);

	CRegKeyEx reg;
	retVal = reg.Open(HKEY_CURRENT_USER, f_RegDir);
	if (retVal != ERROR_SUCCESS)
	{
		CString msg;
		msg.Format(_T("Failed to open registry key HKCU/%s:\n\t%d : %s"),
			f_RegDir, retVal, GetSysError(retVal));
		LogErrorString(msg);
		return;
	}

	// Save path to WinMerge(U).exe
	retVal = reg.WriteString(f_RegValuePath, temp);
	if (retVal != ERROR_SUCCESS)
	{
		CString msg;
		msg.Format(_T("Failed to set registry value %s:\n\t%d : %s"),
			f_RegValuePath, retVal, GetSysError(retVal));
		LogErrorString(msg);
	}

	// Determine bitmask for shell extension
	DWORD dwContextEnabled = reg.ReadDword(f_RegValueEnabled, 0);
	if (m_bContextAdded)
		dwContextEnabled |= CONTEXT_F_ENABLED;
	else
		dwContextEnabled &= ~CONTEXT_F_ENABLED;

	if (m_bContextAdvanced)
		dwContextEnabled |= CONTEXT_F_ADVANCED;
	else
		dwContextEnabled &= ~CONTEXT_F_ADVANCED;

	if (m_bContextSubfolders)
		dwContextEnabled |= CONTEXT_F_SUBFOLDERS;
	else
		dwContextEnabled &= ~CONTEXT_F_SUBFOLDERS;

	retVal = reg.WriteDword(f_RegValueEnabled, dwContextEnabled);
	if (retVal != ERROR_SUCCESS)
	{
		CString msg;
		msg.Format(_T("Failed to set registry value %s to %d:\n\t%d : %s"),
			f_RegValueEnabled, dwContextEnabled, retVal, GetSysError(retVal));
		LogErrorString(msg);
	}
}

/// Open file browse dialog to locate editor
void CPropRegistry::OnBrowseEditor()
{
	CString path;
	CString title;
	VERIFY(title.LoadString(IDS_OPEN_TITLE));

	if (SelectFile(GetSafeHwnd(), path, NULL, title, IDS_PROGRAMFILES, TRUE))
	{
		m_strEditorPath = path;
		UpdateData(FALSE);
	}
}

/// Enable/Disable "Advanced menu" checkbox.
void CPropRegistry::AdvancedContextMenuCheck()
{
	if (IsDlgButtonChecked(IDC_EXPLORER_CONTEXT))
		GetDlgItem(IDC_EXPLORER_ADVANCED)->EnableWindow(TRUE);
	else
	{
		GetDlgItem(IDC_EXPLORER_ADVANCED)->EnableWindow(FALSE);
		CheckDlgButton(IDC_EXPLORER_ADVANCED, FALSE);
		m_bContextAdvanced = FALSE;
	}
}

/// Enable/Disable "Include subfolders by default" checkbox.
void CPropRegistry::SubfolderOptionCheck()
{
	if (IsDlgButtonChecked(IDC_EXPLORER_CONTEXT))
		GetDlgItem(IDC_EXPLORER_SUBFOLDERS)->EnableWindow(TRUE);
	else
	{
		GetDlgItem(IDC_EXPLORER_SUBFOLDERS)->EnableWindow(FALSE);
		CheckDlgButton(IDC_EXPLORER_SUBFOLDERS, FALSE);
		m_bContextSubfolders = FALSE;
	}
}

/// Open Folder selection dialog for user to select filter folder.
void CPropRegistry::OnBrowseFilterPath()
{
	CString path;
	CString title;
	VERIFY(title.LoadString(IDS_OPEN_TITLE));

	if (SelectFolder(path, NULL, title, GetSafeHwnd()))
	{
		m_strUserFilterPath = path;
		UpdateData(FALSE);
	}
}
