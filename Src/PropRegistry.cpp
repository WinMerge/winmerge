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
 * @brief PropRegistry implementation file
 */

#include "stdafx.h"
#include "PropRegistry.h"
#include "RegKey.h"
#include "FileOrFolderSelect.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// registry values
static LPCTSTR f_RegValueEnabled = _T("ContextMenuEnabled");
static LPCTSTR f_RegValuePath = _T("Executable");

PropRegistry::PropRegistry(COptionsMgr *optionsMgr)
: OptionsPanel(optionsMgr, PropRegistry::IDD)
, m_bUseRecycleBin(true)
, m_tempFolderType(0)
{
}

void PropRegistry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropRegistry)
	DDX_Text(pDX, IDC_EXT_EDITOR_PATH, m_strEditorPath);
	DDX_Check(pDX, IDC_USE_RECYCLE_BIN, m_bUseRecycleBin);
	DDX_Text(pDX, IDC_FILTER_USER_PATH, m_strUserFilterPath);
	DDX_Radio(pDX, IDC_TMPFOLDER_SYSTEM, m_tempFolderType);
	DDX_Text(pDX, IDC_TMPFOLDER_NAME, m_tempFolder);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PropRegistry, CDialog)
	//{{AFX_MSG_MAP(PropRegistry)
	ON_BN_CLICKED(IDC_EXT_EDITOR_BROWSE, OnBrowseEditor)
	ON_BN_CLICKED(IDC_FILTER_USER_BROWSE, OnBrowseFilterPath)
	ON_BN_CLICKED(IDC_TMPFOLDER_BROWSE, OnBrowseTmpFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropRegistry::ReadOptions()
{
	m_strEditorPath = GetOptionsMgr()->GetString(OPT_EXT_EDITOR_CMD);
	m_bUseRecycleBin = GetOptionsMgr()->GetBool(OPT_USE_RECYCLE_BIN);
	m_strUserFilterPath = GetOptionsMgr()->GetString(OPT_FILTER_USERPATH);
	m_tempFolderType = GetOptionsMgr()->GetBool(OPT_USE_SYSTEM_TEMP_PATH) ? 0 : 1;
	m_tempFolder = GetOptionsMgr()->GetString(OPT_CUSTOM_TEMP_PATH);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropRegistry::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_USE_RECYCLE_BIN, m_bUseRecycleBin);

	String sExtEditor = strutils::trim_ws(m_strEditorPath);
	if (sExtEditor.empty())
		sExtEditor = GetOptionsMgr()->GetDefault<String>(OPT_EXT_EDITOR_CMD);
	GetOptionsMgr()->SaveOption(OPT_EXT_EDITOR_CMD, sExtEditor);

	String sFilterPath = strutils::trim_ws(m_strUserFilterPath);
	GetOptionsMgr()->SaveOption(OPT_FILTER_USERPATH, sFilterPath);

	bool useSysTemp = m_tempFolderType == 0;
	GetOptionsMgr()->SaveOption(OPT_USE_SYSTEM_TEMP_PATH, useSysTemp);

	String tempFolder = strutils::trim_ws(m_tempFolder);
	GetOptionsMgr()->SaveOption(OPT_CUSTOM_TEMP_PATH, tempFolder);
}

BOOL PropRegistry::OnInitDialog()
{
	OptionsPanel::OnInitDialog();
	m_tooltips.Create(this);
	m_tooltips.SetMaxTipWidth(600);
	m_tooltips.AddTool(GetDlgItem(IDC_EXT_EDITOR_PATH), 
		_("You can specify the following parameters to the path:\n"
		  "$file: Path name of the current file\n"
		  "$linenum: Line number of the current cursor position").c_str());
	return TRUE;
}

/// Open file browse dialog to locate editor
void PropRegistry::OnBrowseEditor()
{
	String path;
	if (SelectFile(GetSafeHwnd(), path, true, m_strEditorPath.c_str(), _T(""), _("Programs|*.exe;*.bat;*.cmd|All Files (*.*)|*.*||")))
	{
		SetDlgItemText(IDC_EXT_EDITOR_PATH, path);
	}
}

/// Open Folder selection dialog for user to select filter folder.
void PropRegistry::OnBrowseFilterPath()
{
	String path;
	if (SelectFolder(path, m_strUserFilterPath.c_str(), _("Open"), GetSafeHwnd()))
	{
		SetDlgItemText(IDC_FILTER_USER_PATH, path);
	}
}

/// Select temporary files folder.
void PropRegistry::OnBrowseTmpFolder()
{
	String path;
	if (SelectFolder(path, m_tempFolder.c_str(), _T(""), GetSafeHwnd()))
	{
		SetDlgItemText(IDC_TMPFOLDER_NAME, path);
	}
}

BOOL PropRegistry::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_LBUTTONUP ||
		pMsg->message == WM_MOUSEMOVE)
	{
		m_tooltips.RelayEvent(pMsg);
	}

	return OptionsPanel::PreTranslateMessage(pMsg);
}