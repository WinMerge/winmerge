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
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "resource.h"
#include "PropRegistry.h"
#include "RegKey.h"
#include "coretools.h"
#include "FileOrFolderSelect.h"
#include "Merge.h" // GetDefaultEditor()
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
, m_bUseRecycleBin(TRUE)
, m_tempFolderType(0)
{
}

void CPropRegistry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropRegistry)
	DDX_Text(pDX, IDC_EXT_EDITOR_PATH, m_strEditorPath);
	DDX_Check(pDX, IDC_USE_RECYCLE_BIN, m_bUseRecycleBin);
	DDX_Text(pDX, IDC_FILTER_USER_PATH, m_strUserFilterPath);
	DDX_Radio(pDX, IDC_TMPFOLDER_SYSTEM, m_tempFolderType);
	DDX_Text(pDX, IDC_TMPFOLDER_NAME, m_tempFolder);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPropRegistry, CDialog)
	//{{AFX_MSG_MAP(CPropRegistry)
	ON_BN_CLICKED(IDC_EXT_EDITOR_BROWSE, OnBrowseEditor)
	ON_BN_CLICKED(IDC_FILTER_USER_BROWSE, OnBrowseFilterPath)
	ON_BN_CLICKED(IDC_TMPFOLDER_BROWSE, OnBrowseTmpFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void CPropRegistry::ReadOptions()
{
	m_strEditorPath = m_pOptionsMgr->GetString(OPT_EXT_EDITOR_CMD).c_str();
	m_bUseRecycleBin = m_pOptionsMgr->GetBool(OPT_USE_RECYCLE_BIN);
	m_strUserFilterPath = m_pOptionsMgr->GetString(OPT_FILTER_USERPATH).c_str();
	m_tempFolderType = m_pOptionsMgr->GetBool(OPT_USE_SYSTEM_TEMP_PATH) ? 0 : 1;
	m_tempFolder = m_pOptionsMgr->GetString(OPT_CUSTOM_TEMP_PATH).c_str();
}

/** 
 * @brief Writes options values from UI to storage.
 */
void CPropRegistry::WriteOptions()
{
	CMergeApp *app = static_cast<CMergeApp*>(AfxGetApp());
	CString sDefaultEditor = app->GetDefaultEditor();

	m_pOptionsMgr->SaveOption(OPT_USE_RECYCLE_BIN, m_bUseRecycleBin == TRUE);

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

	bool useSysTemp = m_tempFolderType == 0;
	m_pOptionsMgr->SaveOption(OPT_USE_SYSTEM_TEMP_PATH, useSysTemp);

	CString tempFolder = m_tempFolder;
	tempFolder.TrimLeft();
	tempFolder.TrimRight();
	m_pOptionsMgr->SaveOption(OPT_CUSTOM_TEMP_PATH, tempFolder);
}

/////////////////////////////////////////////////////////////////////////////
// CPropRegistry message handlers

BOOL CPropRegistry::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/// Open file browse dialog to locate editor
void CPropRegistry::OnBrowseEditor()
{
	CString path;
	if (SelectFile(GetSafeHwnd(), path, NULL, IDS_OPEN_TITLE, IDS_PROGRAMFILES, TRUE))
	{
		m_strEditorPath = path;
		UpdateData(FALSE);
	}
}

/// Open Folder selection dialog for user to select filter folder.
void CPropRegistry::OnBrowseFilterPath()
{
	CString path;
	if (SelectFolder(path, NULL, IDS_OPEN_TITLE, GetSafeHwnd()))
	{
		m_strUserFilterPath = path;
		UpdateData(FALSE);
	}
}

/// Select temporary files folder.
void CPropRegistry::OnBrowseTmpFolder()
{
	CString path;
	if (SelectFolder(path, NULL, NULL, GetSafeHwnd()))
	{
		m_tempFolder = path;
		UpdateData(FALSE);
	}
}
