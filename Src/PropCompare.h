/** 
 * @file  PropCompare.h
 *
 * @brief Declaration of PropCompare propertysheet
 */
#pragma once

#include "OptionsPanel.h"

class COptionsMgr;

/**
 * @brief Property page to set compare options for WinMerge.
 *
 * Whitespace compare:
 *  - Compare all whitespaces, recommended for merging!
 *  - Ignore changes in whitespaces (amount of spaces etc)
 *  - Ignore all whitespace characters
 */
class PropCompare : public OptionsPanel
{
// Construction
public:
	explicit PropCompare(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
	//{{AFX_DATA(PropCompare)
	enum { IDD = IDD_PROPPAGE_COMPARE };
	bool    m_bIgnoreCodepage;
	bool    m_bIgnoreEol;
	bool    m_bIgnoreCase;
	bool    m_bIgnoreBlankLines;
	int     m_nIgnoreWhite;
	bool    m_bMovedBlocks;
	bool    m_bMatchSimilarLines;
	bool    m_bFilterCommentsLines;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropCompare)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropCompare)
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
