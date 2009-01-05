/**
 * @file  PropShell.h
 *
 * @brief Declaration of Shell options dialog class
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _PROP_SHELL_H_
#define _PROP_SHELL_H_

#include "IOptionsPanel.h"

/**
 * @brief Class for Shell options -propertypage.
 */
class PropShell : public CPropertyPage, public IOptionsPanel
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

private:
	COptionsMgr *m_pOptionsMgr;

};

#endif // _PROP_SHELL_H_
