#if !defined(AFX_VSSPROMPT_H__F767E53B_90F4_11D1_BB11_00A024706EDC__INCLUDED_)
#define AFX_VSSPROMPT_H__F767E53B_90F4_11D1_BB11_00A024706EDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VssPrompt.h : header file
//
#include "SuperComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CVssPrompt dialog

class CVssPrompt : public CDialog
{
// Construction
public:
	CVssPrompt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVssPrompt)
	enum { IDD = IDD_VSS };
	CSuperComboBox	m_ctlProject;
	CString	m_strProject;
	CString	m_strMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVssPrompt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVssPrompt)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VSSPROMPT_H__F767E53B_90F4_11D1_BB11_00A024706EDC__INCLUDED_)
