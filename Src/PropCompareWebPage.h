/** 
 * @file  PropCompareWebPage.h
 *
 * @brief Declaration of PropCompareWebPage propertysheet
 */
#pragma once

#include "OptionsPanel.h"
#include "UnicodeString.h"

class COptionsMgr;

/**
 * @brief Property page to set image compare options for WinMerge.
 */
class PropCompareWebPage : public OptionsPanel
{
// Construction
public:
	explicit PropCompareWebPage(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropCompareWebPage)
	enum { IDD = IDD_PROPPAGE_COMPARE_WEBPAGE };
	bool m_bUserDataFolderPerPane;
	int  m_nUserDataFolderType;
	String m_sURLPatternToInclude;
	String m_sURLPatternToExclude;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropCompareWebPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropCompareWebPage)
	afx_msg BOOL OnInitDialog() override;
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
