/** 
 * @file  PropBackups.h
 *
 * @brief Declaration file for PropBackups propertyheet
 *
 */
#pragma once

#include "OptionsPanel.h"

class COptionsMgr;

/**
 * @brief A class for Backup file options page.
 */
class PropBackups : public OptionsPanel
{
public:
	explicit PropBackups(COptionsMgr *optionsMgr);

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
};
