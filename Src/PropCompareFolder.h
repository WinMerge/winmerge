/** 
 * @file  PropCompareFolder.h
 *
 * @brief Declaration of PropCompareFolder propertysheet
 */
#pragma once

#include "OptionsPanel.h"

class COptionsMgr;

/////////////////////////////////////////////////////////////////////////////
// PropCompareFolder dialog

/**
 * @brief Property page to set folder compare options for WinMerge.
 *
 * Compare methods:
 *  - compare by contents
 *  - compare by modified date
 *  - compare by file size
 *  - compare by date and size
 *  - compare by quick contents
 */
class PropCompareFolder : public OptionsPanel
{
// Construction
public:
	explicit PropCompareFolder(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions() override;
	virtual void WriteOptions() override;

// Dialog Data
	//{{AFX_DATA(PropCompareFolder)
	enum { IDD = IDD_PROPPAGE_COMPARE_FOLDER };
	int     m_compareMethod;
	bool    m_bStopAfterFirst;
	bool    m_bIgnoreSmallTimeDiff;
	bool    m_bIncludeUniqFolders;
	bool    m_bIncludeSubdirs;
	bool    m_bExpandSubdirs;
	bool    m_bIgnoreReparsePoints;
	unsigned m_nQuickCompareLimit;
	unsigned m_nBinaryCompareLimit;
	int     m_nCompareThreads;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PropCompareFolder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropCompare)
	afx_msg BOOL OnInitDialog() override;
	afx_msg void OnDefaults();
	afx_msg void OnCbnSelchangeComparemethodcombo();
	afx_msg void OnBnClickedRecursCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void UpdateControls();
};
