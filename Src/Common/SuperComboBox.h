#if !defined(AFX_SuperComboBox_H__8027D162_6B2C_11D1_BAEE_00A024706EDC__INCLUDED_)
#define AFX_SuperComboBox_H__8027D162_6B2C_11D1_BAEE_00A024706EDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ComboBoxEx.h : header file
//
#include <shlobj.h>

/////////////////////////////////////////////////////////////////////////////
// CSuperComboBox window

class CSuperComboBox : public CComboBox
{
// Construction
public:
	CSuperComboBox(BOOL bAdd = TRUE, UINT idstrAddText = 0);
	virtual ~CSuperComboBox();

// Attributes
protected:
	BOOL m_bEditChanged;
	BOOL m_bAutoComplete;
	BOOL m_bDoComplete;

public:

// Operations
	void SetAutoComplete( BOOL bAutoComplete ) { m_bAutoComplete = bAutoComplete; }

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuperComboBox)
	public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

// Implementation
public:
	void ResetContent();
	void SetAutoAdd(BOOL bAdd = TRUE, UINT idstrAddText = 0);
	void SaveState(LPCTSTR szRegSubKey, UINT nMaxItems = 20);
	void LoadState(LPCTSTR szRegSubKey, UINT nMaxItems = 20);

	// Generated message map functions
protected:
	CString m_strCurSel;
	CString ExpandShortcut(CString &inFile);
	virtual BOOL OnAddTemplate();
	CString m_strAutoAdd;
	BOOL m_bMustUninitOLE;
	//{{AFX_MSG(CSuperComboBox)
	afx_msg BOOL OnEditchange();
	afx_msg BOOL OnSelchange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP dropInfo);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SuperComboBox_H__8027D162_6B2C_11D1_BAEE_00A024706EDC__INCLUDED_)
