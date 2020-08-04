/** 
 * @file  PropCompareImage.h
 *
 * @brief Declaration of PropCompareImage propertysheet
 */
#pragma once

#include "OptionsPanel.h"
#include "UnicodeString.h"

class COptionsMgr;

/**
 * @brief Property page to set image compare options for WinMerge.
 */
class PropCompareImage : public OptionsPanel
{
// Construction
public:
	explicit PropCompareImage(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropCompareImage)
	enum { IDD = IDD_PROPPAGE_COMPARE_IMAGE };
	CComboBox m_comboPatterns;
	String m_sFilePatterns;
	bool m_bEnableImageCompare;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropCompareImage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(PropCompareImage)
	afx_msg void OnDefaults();
	afx_msg void OnDropDownPatterns();
	afx_msg void OnCloseUpPatterns();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
