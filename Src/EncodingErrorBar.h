// EncodingErrorBar.h : header file
//

#pragma once

#include "TrDialogs.h"

/////////////////////////////////////////////////////////////////////////////
// CEncodingErrorBar dialog

class CEncodingErrorBar : public CTrDialogBar
{
// Construction
public:
	CEncodingErrorBar();

	BOOL Create(CWnd* pParentWnd);
	void SetText(const CString& sText);

// Dialog Data
	//{{AFX_DATA(CEncodingErrorBar)
	enum { IDD = IDD_ENCODINGERROR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEncodingErrorBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEncodingErrorBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
