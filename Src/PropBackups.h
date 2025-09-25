/** 
 * @file  PropBackups.h
 *
 * @brief Declaration file for PropBackups propertyheet
 *
 */
#pragma once

#include "OptionsPanel.h"
#include "UnicodeString.h"

class COptionsMgr;

/**
 * @brief A class for Backup file options page.
 */
class PropBackups : public OptionsPanel
{
public:
	explicit PropBackups(COptionsMgr *optionsMgr);

// Dialog Data
	enum { IDD = IDD_PROPPAGE_BACKUPS };

	/** @brief Backup file locations. */
	enum BACKUP_FOLDER
	{
		FOLDER_ORIGINAL = 0,
		FOLDER_GLOBAL,
	};

	bool m_bCreateForFolderCmp;
	bool m_bCreateForFileCmp;
	String m_sGlobalFolder;
	bool m_bAppendBak;
	bool m_bAppendTime;
	int m_nBackupFolder;

protected:
	afx_msg void OnBnClickedBackupBrowse();

	DECLARE_MESSAGE_MAP()
};
