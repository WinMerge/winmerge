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
}

void PropBackups::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_BACKUP_FOLDERCMP, m_bCreateForFolderCmp);
	DDX_Check(pDX, IDC_BACKUP_FILECMP, m_bCreateForFileCmp);
	DDX_Text(pDX, IDC_BACKUP_FOLDER, m_sGlobalFolder);
	DDX_Check(pDX, IDC_BACKUP_APPEND_BAK, m_bAppendBak);
	DDX_Check(pDX, IDC_BACKUP_APPEND_TIME, m_bAppendTime);
	DDX_Radio(pDX, IDC_BACKUP_ORIGFOLD, m_nBackupFolder);
}


BEGIN_MESSAGE_MAP(PropBackups, CPropertyPage)
	ON_BN_CLICKED(IDC_BACKUP_BROWSE, OnBnClickedBackupBrowse)
END_MESSAGE_MAP()


/** 
 * @brief Reads options values from storage to UI.
 */
void PropBackups::ReadOptions()
{
	m_bCreateForFolderCmp = GetOptionsMgr()->GetBool(OPT_BACKUP_FOLDERCMP);
	m_bCreateForFileCmp = GetOptionsMgr()->GetBool(OPT_BACKUP_FILECMP);
	m_nBackupFolder = GetOptionsMgr()->GetInt(OPT_BACKUP_LOCATION);
	m_sGlobalFolder = GetOptionsMgr()->GetString(OPT_BACKUP_GLOBALFOLDER);
	m_bAppendBak = GetOptionsMgr()->GetBool(OPT_BACKUP_ADD_BAK);
	m_bAppendTime = GetOptionsMgr()->GetBool(OPT_BACKUP_ADD_TIME);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropBackups::WriteOptions()
{
	m_sGlobalFolder = strutils::trim_ws(m_sGlobalFolder);
	if (m_sGlobalFolder.length() > 3)
		m_sGlobalFolder = paths::AddTrailingSlash(m_sGlobalFolder);

	GetOptionsMgr()->SaveOption(OPT_BACKUP_FOLDERCMP, m_bCreateForFolderCmp);
	GetOptionsMgr()->SaveOption(OPT_BACKUP_FILECMP, m_bCreateForFileCmp);
	GetOptionsMgr()->SaveOption(OPT_BACKUP_LOCATION, m_nBackupFolder);
	GetOptionsMgr()->SaveOption(OPT_BACKUP_GLOBALFOLDER, m_sGlobalFolder);
	GetOptionsMgr()->SaveOption(OPT_BACKUP_ADD_BAK, m_bAppendBak);
	GetOptionsMgr()->SaveOption(OPT_BACKUP_ADD_TIME, m_bAppendTime);
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
