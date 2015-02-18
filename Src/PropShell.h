/**
 * @file  PropShell.h
 *
 * @brief Declaration of Shell options dialog class
 */
#pragma once

#include "OptionsPanel.h"

/**
 * @brief Class for Shell options -propertypage.
 */
class PropShell : public OptionsPanel
{
// Construction
public:
	PropShell(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_SHELL };
	BOOL m_bContextAdded;
	BOOL m_bContextAdvanced;
	BOOL m_bContextSubfolders;
	BOOL m_bEnableShellContextMenu;

// Overrides
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnAddToExplorer();

	void GetContextRegValues();
	void AdvancedContextMenuCheck();
	void SubfolderOptionCheck();

	DECLARE_MESSAGE_MAP()

	void SaveMergePath();
};
