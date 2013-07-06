/**
 * @file  PropShell.h
 *
 * @brief Declaration of Shell options dialog class
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _PROP_SHELL_H_
#define _PROP_SHELL_H_

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
	bool m_bContextAdded;
	bool m_bContextAdvanced;
	bool m_bContextSubfolders;
	bool m_bEnableShellContextMenu;

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

#endif // _PROP_SHELL_H_
