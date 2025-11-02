// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  PropRegistry.h
 *
 * @brief Declaration file PropRegistry
 */
#pragma once

#include "OptionsPanel.h"
#include "UnicodeString.h"

class COptionsMgr;

/**
 * @brief Property page for system options; used in options property sheet.
 *
 * This class implements property sheet for what we consider System-options.
 * It allows user to select options like whether to use Recycle Bin for
 * deleted files and External text editor.
 */
class PropRegistry : public OptionsPanel
{
// Construction
public:
	explicit PropRegistry(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropRegistry)
	enum { IDD = IDD_PROPPAGE_SYSTEM };
	String  m_strEditorPath;
	bool    m_bUseRecycleBin;
	String  m_strUserFilterPath;
	int     m_tempFolderType;
	String  m_tempFolder;
	CToolTipCtrl m_tooltips;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropRegistry)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

// Implementation methods
protected:
	// Generated message map functions
	//{{AFX_MSG(PropRegistry)
	afx_msg void OnDefaults();
	afx_msg void OnBrowseEditor();
	afx_msg void OnBrowseFilterPath();
	afx_msg void OnBrowseTmpFolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
