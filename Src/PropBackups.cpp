/** 
 * @file  PropBackups.cpp
 *
 * @brief Implementation of PropBackups propertysheet
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "Merge.h"
#include "PropBackups.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"
#include "FileOrFolderSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** 
 * @brief Constructor taking OptionsManager parameter.
 * @param [in] optionsMgr Pointer to OptionsManager.
 */
PropBackups::PropBackups(COptionsMgr *optionsMgr)
	: OptionsPanel(optionsMgr, PropBackups::IDD)
	, m_bCreateForFolderCmp(FALSE)
	, m_bCreateForFileCmp(FALSE)
	, m_bAppendBak(FALSE)
	, m_bAppendTime(FALSE)
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
	m_sGlobalFolder = GetOptionsMgr()->GetString(OPT_BACKUP_GLOBALFOLDER).c_str();
	m_bAppendBak = GetOptionsMgr()->GetBool(OPT_BACKUP_ADD_BAK);
	m_bAppendTime = GetOptionsMgr()->GetBool(OPT_BACKUP_ADD_TIME);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropBackups::WriteOptions()
{
	m_sGlobalFolder.TrimLeft();
	m_sGlobalFolder.TrimRight();
	if (m_sGlobalFolder.GetLength() > 3&&
		m_sGlobalFolder[m_sGlobalFolder.GetLength() - 1] != '\\')
	{
		m_sGlobalFolder += "\\";
	}

	GetOptionsMgr()->SaveOption(OPT_BACKUP_FOLDERCMP, m_bCreateForFolderCmp == TRUE);
	GetOptionsMgr()->SaveOption(OPT_BACKUP_FILECMP, m_bCreateForFileCmp == TRUE);
	GetOptionsMgr()->SaveOption(OPT_BACKUP_LOCATION, m_nBackupFolder);
	GetOptionsMgr()->SaveOption(OPT_BACKUP_GLOBALFOLDER, m_sGlobalFolder);
	GetOptionsMgr()->SaveOption(OPT_BACKUP_ADD_BAK, m_bAppendBak == TRUE);
	GetOptionsMgr()->SaveOption(OPT_BACKUP_ADD_TIME, m_bAppendTime == TRUE);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropBackups::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
    CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Called when user selects Browse-button.
 */
void PropBackups::OnBnClickedBackupBrowse()
{
	CString path;
	if (SelectFolder(path, m_sGlobalFolder, 0, GetSafeHwnd()))
	{
		SetDlgItemText(IDC_BACKUP_FOLDER, path);
	}
}
