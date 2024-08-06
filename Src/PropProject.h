/** 
 * @file  PropProject.h
 *
 * @brief Declaration file for PropProject propertysheet
 *
 */
#pragma once

#include "OptionsPanel.h"
#include "SubeditList.h"
#include "OptionsProject.h"

class COptionsMgr;

/**
 * @brief Property page to set project options for WinMerge.
 */
class PropProject : public OptionsPanel
{
// Construction
public:
	explicit PropProject(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropEditor)
	enum { IDD = IDD_PROPPAGE_PROJECT };
	CSubeditList m_list;
	//}}AFX_DATA

private:
	Options::Project::Settings m_settings;

// Implementation methods
	void InitList();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PropEditor)
	afx_msg BOOL OnInitDialog() override;
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
