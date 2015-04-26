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

// Overrides
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnAddToExplorer();
	afx_msg void OnRegisterShellExtension();
	afx_msg void OnUnregisterShellExtension();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void GetContextRegValues();
	void AdvancedContextMenuCheck();
	void SubfolderOptionCheck();
	void UpdateButtons();

	DECLARE_MESSAGE_MAP()

	void SaveMergePath();
};
