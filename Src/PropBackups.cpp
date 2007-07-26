/** 
 * @file  PropBackups.cpp
 *
 * @brief Implementation of CPropBackups propertysheet
 */
// ID line follows -- this is updated by SVN
// $Id: PropArchive.cpp 3126 2006-03-04 02:36:46Z elsapo $

#include "stdafx.h"
#include "Merge.h"
#include "PropBackups.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "FileOrFolderSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CPropBackups dialog

/** 
 * @brief Constructor taking OptionsManager parameter.
 * @param [in] optionsMgr Pointer to OptionsManager.
 */
CPropBackups::CPropBackups(COptionsMgr *optionsMgr)
	: CPropertyPage(CPropBackups::IDD)
	, m_pOptionsMgr(optionsMgr)
	, m_bCreateForFolderCmp(FALSE)
	, m_bCreateForFileCmp(FALSE)
	, m_bAppendBak(FALSE)
	, m_bAppendTime(FALSE)
	, m_nBackupFolder(0)
{
}

/** 
 * @brief Destructor.
 */
CPropBackups::~CPropBackups()
{
}

void CPropBackups::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_BACKUP_FOLDERCMP, m_bCreateForFolderCmp);
	DDX_Check(pDX, IDC_BACKUP_FILECMP, m_bCreateForFileCmp);
	DDX_Text(pDX, IDC_BACKUP_FOLDER, m_sGlobalFolder);
	DDX_Check(pDX, IDC_BACKUP_APPEND_BAK, m_bAppendBak);
	DDX_Check(pDX, IDC_BACKUP_APPEND_TIME, m_bAppendTime);
	DDX_Radio(pDX, IDC_BACKUP_ORIGFOLD, m_nBackupFolder);
}


BEGIN_MESSAGE_MAP(CPropBackups, CPropertyPage)
	ON_BN_CLICKED(IDC_BACKUP_BROWSE, OnBnClickedBackupBrowse)
END_MESSAGE_MAP()


/** 
 * @brief Reads options values from storage to UI.
 */
void CPropBackups::ReadOptions()
{
	m_bCreateForFolderCmp = m_pOptionsMgr->GetBool(OPT_BACKUP_FOLDERCMP);
	m_bCreateForFileCmp = m_pOptionsMgr->GetBool(OPT_BACKUP_FILECMP);
	m_nBackupFolder = m_pOptionsMgr->GetInt(OPT_BACKUP_LOCATION);
	m_sGlobalFolder = m_pOptionsMgr->GetString(OPT_BACKUP_GLOBALFOLDER).c_str();
	m_bAppendBak = m_pOptionsMgr->GetBool(OPT_BACKUP_ADD_BAK);
	m_bAppendTime = m_pOptionsMgr->GetBool(OPT_BACKUP_ADD_TIME);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void CPropBackups::WriteOptions()
{
	m_sGlobalFolder.TrimLeft();
	m_sGlobalFolder.TrimRight();
	if (m_sGlobalFolder.GetLength() > 3&&
		m_sGlobalFolder[m_sGlobalFolder.GetLength() - 1] != '\\')
	{
		m_sGlobalFolder += "\\";
	}

	m_pOptionsMgr->SaveOption(OPT_BACKUP_FOLDERCMP, m_bCreateForFolderCmp == TRUE);
	m_pOptionsMgr->SaveOption(OPT_BACKUP_FILECMP, m_bCreateForFileCmp == TRUE);
	m_pOptionsMgr->SaveOption(OPT_BACKUP_LOCATION, m_nBackupFolder);
	m_pOptionsMgr->SaveOption(OPT_BACKUP_GLOBALFOLDER, m_sGlobalFolder);
	m_pOptionsMgr->SaveOption(OPT_BACKUP_ADD_BAK, m_bAppendBak == TRUE);
	m_pOptionsMgr->SaveOption(OPT_BACKUP_ADD_TIME, m_bAppendTime == TRUE);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL CPropBackups::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Called when user selects Browse-button.
 */
void CPropBackups::OnBnClickedBackupBrowse()
{
	CString path;
	if (SelectFolder(path, NULL, _T(""), GetSafeHwnd()))
	{
		m_sGlobalFolder = path;
		UpdateData(FALSE);
	}
}
