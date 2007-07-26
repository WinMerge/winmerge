/** 
 * @file  PropBackups.h
 *
 * @brief Declaration file for CPropBackups propertyheet
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _PROPBACKUPS_H_
#define _PROPBACKUPS_H_

#ifndef _IOPTIONSPANEL_H_
#include "IOptionsPanel.h"
#endif

class COptionsMgr;

/**
 * @brief A class for Backup file options page.
 */
class CPropBackups : public CPropertyPage
{
public:
	CPropBackups(COptionsMgr *optionsMgr);
	virtual ~CPropBackups();

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_BACKUPS };

	/** @brief Backup file locations. */
	enum BACKUP_FOLDER
	{
		FOLDER_ORIGINAL = 0,
		FOLDER_GLOBAL,
	};

	BOOL m_bCreateForFolderCmp;
	BOOL m_bCreateForFileCmp;
	CString m_sGlobalFolder;
	BOOL m_bAppendBak;
	BOOL m_bAppendTime;
	int m_nBackupFolder;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBackupBrowse();

	DECLARE_MESSAGE_MAP()

private:
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};

#endif // _PROPBACKUPS_H_
