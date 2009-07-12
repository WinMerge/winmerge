/** 
 * @file  PropCompareFolder.h
 *
 * @brief Declaration of PropCompareFolder propertysheet
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _PROPCOMPARE_FOLDER_H_
#define _PROPCOMPARE_FOLDER_H_

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
	PropCompareFolder(COptionsMgr *optionsMgr);

// Implement IOptionsPanel
	virtual void ReadOptions();
	virtual void WriteOptions();

// Dialog Data
	//{{AFX_DATA(PropCompareFolder)
	enum { IDD = IDD_PROPPAGE_COMPARE_FOLDER };
	int     m_compareMethod;
	BOOL    m_bStopAfterFirst;
	BOOL    m_bIgnoreSmallTimeDiff;
	BOOL    m_bIncludeUniqFolders;
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
	afx_msg BOOL OnInitDialog();
	afx_msg void OnDefaults();
	afx_msg void OnCbnSelchangeComparemethodcombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _PROPCOMPARE_FOLDER_H_
