/** 
 * @file  PropCompareBinary.h
 *
 * @brief Declaration of PropCompareBinary propertysheet
 */
#pragma once

#include "OptionsPanel.h"
#include "UnicodeString.h"

class COptionsMgr;

/**
 * @brief Property page to set image compare options for WinMerge.
 */
class PropCompareBinary : public OptionsPanel
{
// Construction
public:
	explicit PropCompareBinary(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropCompareBinary)
	enum { IDD = IDD_PROPPAGE_COMPARE_BINARY };
	String m_sFilePatterns;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropCompareBinary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropCompareBinary)
	afx_msg void OnViewSettings();
	afx_msg void OnBinaryMode();
	afx_msg void OnCharacterSet();
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
