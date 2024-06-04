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
	explicit PropShell(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	enum { IDD = IDD_PROPPAGE_SHELL };
	bool m_bContextAdded;
	bool m_bContextAdvanced;
	bool m_bContextCompareAs;
	CListCtrl m_list;

// Overrides
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnAddToExplorer();
	afx_msg void OnAddToExplorerAdvanced();
	afx_msg void OnRegisterShellExtension();
	afx_msg void OnUnregisterShellExtension();
	afx_msg void OnRegisterShellExtensionPerUser();
	afx_msg void OnUnregisterShellExtensionPerUser();
	afx_msg void OnRegisterWinMergeContextMenu();
	afx_msg void OnUnregisterWinMergeContextMenu();
	afx_msg void OnClearAllRecentItems();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void GetContextRegValues();
	void AdvancedContextMenuCheck();
	void CompareAsContextMenuCheck();
	void UpdateButtons();

	DECLARE_MESSAGE_MAP()

	void SaveMergePath();
};
