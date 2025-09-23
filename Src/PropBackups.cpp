/** 
 * @file  PropBackups.cpp
 *
 * @brief Implementation of PropBackups propertysheet
 */

#include "stdafx.h"
#include "PropBackups.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "FileOrFolderSelect.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Constructor taking OptionsManager parameter.
 * @param [in] optionsMgr Pointer to OptionsManager.
 */
PropBackups::PropBackups(COptionsMgr *optionsMgr)
	: OptionsPanel(optionsMgr, PropBackups::IDD)
	, m_bCreateForFolderCmp(false)
	, m_bCreateForFileCmp(false)
	, m_bAppendBak(false)
	, m_bAppendTime(false)
	, m_nBackupFolder(0)
{
	BindOption(OPT_BACKUP_FOLDERCMP, m_bCreateForFolderCmp, IDC_BACKUP_FOLDERCMP, DDX_Check);
	BindOption(OPT_BACKUP_FILECMP, m_bCreateForFileCmp, IDC_BACKUP_FILECMP, DDX_Check);
	BindOption(OPT_BACKUP_GLOBALFOLDER, m_sGlobalFolder, IDC_BACKUP_FOLDER, DDX_Text);
	BindOption(OPT_BACKUP_ADD_BAK, m_bAppendBak, IDC_BACKUP_APPEND_BAK, DDX_Check);
	BindOption(OPT_BACKUP_ADD_TIME, m_bAppendTime, IDC_BACKUP_APPEND_TIME, DDX_Check);
	BindOption(OPT_BACKUP_LOCATION, m_nBackupFolder, IDC_BACKUP_ORIGFOLD, DDX_Radio);
}

void PropBackups::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DoDataExchangeBindOptions(pDX);
}


BEGIN_MESSAGE_MAP(PropBackups, OptionsPanel)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_BN_CLICKED(IDC_BACKUP_BROWSE, OnBnClickedBackupBrowse)
END_MESSAGE_MAP()


/** 
 * @brief Writes options values from UI to storage.
 */
void PropBackups::WriteOptions()
{
	m_sGlobalFolder = strutils::trim_ws(m_sGlobalFolder);
	if (m_sGlobalFolder.length() > 3)
		m_sGlobalFolder = paths::AddTrailingSlash(m_sGlobalFolder);
	WriteOptionBindings();
}

/** 
 * @brief Called when user selects Browse-button.
 */
void PropBackups::OnBnClickedBackupBrowse()
{
	String path;
	if (SelectFolder(path, m_sGlobalFolder.c_str(), _T(""), GetSafeHwnd()))
	{
		SetDlgItemText(IDC_BACKUP_FOLDER, path);
	}
}
