// WebPageDiffBar.h : header file
//

#pragma once

#include "TrDialogs.h"

/////////////////////////////////////////////////////////////////////////////
// CWebPageDiffBar dialog

class CWebPageDiffBar : public CTrDialogBar
{
// Construction
public:
	CWebPageDiffBar();

	BOOL Create(CWnd* pParentWnd);

// Dialog Data
	//{{AFX_DATA(CWebPageDiffBar)
	enum { IDD = IDD_WEBPAGEDIFF_BAR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebPageDiffBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWebPageDiffBar)
	afx_msg void OnUpdateBnClickedCompare(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

public:
};
