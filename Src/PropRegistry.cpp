// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  PropRegistry.cpp
 *
 * @brief PropRegistry implementation file
 */

#include "stdafx.h"
#include "PropRegistry.h"
#include "FileOrFolderSelect.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "Constants.h"
#include "Environment.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

PropRegistry::PropRegistry(COptionsMgr *optionsMgr)
: OptionsPanel(optionsMgr, PropRegistry::IDD)
, m_bUseRecycleBin(true)
, m_tempFolderType(0)
{
	auto readconv = +[](String v) { return v; };
	auto writeconv = +[](String v) { return strutils::trim_ws(v); };
	BindOptionCustom(OPT_EXT_EDITOR_CMD, m_strEditorPath, IDC_EXT_EDITOR_PATH, DDX_Text, readconv, writeconv);
	BindOption(OPT_USE_RECYCLE_BIN, m_bUseRecycleBin, IDC_USE_RECYCLE_BIN, DDX_Check);
	BindOptionCustom(OPT_FILTER_USERPATH, m_strUserFilterPath, IDC_FILTER_USER_PATH, DDX_Text, readconv, writeconv);
	BindOptionCustom(OPT_CUSTOM_TEMP_PATH, m_tempFolder, IDC_TMPFOLDER_NAME, DDX_Text, readconv, writeconv);
	BindOptionCustom<int, bool>(OPT_USE_SYSTEM_TEMP_PATH, m_tempFolderType, IDC_TMPFOLDER_SYSTEM, DDX_Radio,
		+[](bool v) { return v ? 0 : 1; }, +[](int v) { return v == 0; });
}

BEGIN_MESSAGE_MAP(PropRegistry, OptionsPanel)
	//{{AFX_MSG_MAP(PropRegistry)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_BN_CLICKED(IDC_EXT_EDITOR_BROWSE, OnBrowseEditor)
	ON_BN_CLICKED(IDC_FILTER_USER_BROWSE, OnBrowseFilterPath)
	ON_BN_CLICKED(IDC_TMPFOLDER_BROWSE, OnBrowseTmpFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Writes options values from UI to storage.
 */
void PropRegistry::WriteOptions()
{
	m_strEditorPath = strutils::trim_ws(m_strEditorPath);
	if (m_strEditorPath.empty())
		m_strEditorPath = GetOptionsMgr()->GetDefault<String>(OPT_EXT_EDITOR_CMD);
	WriteOptionBindings();
}

BOOL PropRegistry::OnInitDialog()
{
	OptionsPanel::OnInitDialog();
	m_tooltips.Create(this);
	m_tooltips.SetMaxTipWidth(600);
	m_tooltips.AddTool(GetDlgItem(IDC_EXT_EDITOR_PATH), 
		_("Parameters:\n"
		  "$file: Current file path\n"
		  "$linenum: Current cursor line number").c_str());
	return TRUE;
}

/**
 * @brief Sets options to defaults
 * @remarks We must specify the default value of OPT_FILTER_USERPATH explicitly because it is not set for faster startup.
 *          (Refer to Options::Init().)
 */
void PropRegistry::OnDefaults()
{
	ResetOptionBindings();
	if (m_strUserFilterPath.empty())
		m_strUserFilterPath = paths::ConcatPath(env::GetMyDocuments(), DefaultRelativeFilterPath);
	UpdateData(FALSE);
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