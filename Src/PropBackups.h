/** 
 * @file  PropBackups.h
 *
 * @brief Declaration file for PropBackups propertyheet
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _PROPBACKUPS_H_
#define _PROPBACKUPS_H_

#include "OptionsPanel.h"
#include "UnicodeString.h"

class COptionsMgr;

/**
 * @brief A class for Backup file options page.
 */
class PropBackups : public OptionsPanel
{
public:
	PropBackups(COptionsMgr *optionsMgr);

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

	bool m_bCreateForFolderCmp;
	bool m_bCreateForFileCmp;
	String m_sGlobalFolder;
	bool m_bAppendBak;
	bool m_bAppendTime;
	int m_nBackupFolder;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBackupBrowse();

	DECLARE_MESSAGE_MAP()
};

#endif // _PROPBACKUPS_H_
