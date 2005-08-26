/** 
 * @file  PropCompare.cpp
 *
 * @brief Implementation of CPropCompare propertysheet
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _PROPPAGE_COMPARE_H_
#define _PROPPAGE_COMPARE_H_

class COptionsMgr;


/////////////////////////////////////////////////////////////////////////////
// CPropCompare dialog

/**
 * @brief Property page to set compare options for WinMerge.
 *
 * Whitespace compare:
 *  - Compare all whitespaces, recommended for merging!
 *  - Ignore changes in whitespaces (amount of spaces etc)
 *  - Ignore all whitespace characters
 *
 * Compare methods:
 *  - compare by contents
 *  - compare by modified date
 *
 * @author Tim Musschoot, several modifications by Kimmo Varis
 */
class CPropCompare : public CPropertyPage
{
// Construction
public:
    enum CompareMethod { BY_CONTENTS, BY_DATE};

	CPropCompare(COptionsMgr *optionsMgr);

// Dialog Data
	//{{AFX_DATA(CPropCompare)
	enum { IDD = IDD_PROPPAGE_COMPARE };
	int		m_compareMethod;
	BOOL	m_bEolSensitive;
	BOOL	m_bIgnoreCase;
	BOOL	m_bIgnoreBlankLines;
	int		m_nIgnoreWhite;
	BOOL	m_bMovedBlocks;
	BOOL	m_bStopAfterFirst;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropCompare)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropCompare)
	afx_msg BOOL OnInitDialog();
	afx_msg void OnDefaults();
	afx_msg void OnCbnSelchangeComparemethodcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	COptionsMgr * m_pOptionsMgr;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _PROPPAGE_COMPARE_H_

