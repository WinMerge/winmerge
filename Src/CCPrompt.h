#if !defined(AFX_CCPROMPT_H__7ED564F2_CCB8_11D4_92BB_00B0D0221937__INCLUDED_)
#define AFX_CCPROMPT_H__7ED564F2_CCB8_11D4_92BB_00B0D0221937__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CCPrompt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCCPrompt dialog

class CCCPrompt : public CDialog
{
// Construction
public:
	CCCPrompt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCCPrompt)
	enum { IDD = IDD_CLEARCASE };
	CString	m_comments;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCCPrompt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCCPrompt)
	afx_msg void OnSaveas();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCPROMPT_H__7ED564F2_CCB8_11D4_92BB_00B0D0221937__INCLUDED_)

