#pragma once

// ComboBoxEx.h : header file
//

#include <vector>
#include "UnicodeString.h"

class DropHandler;

/////////////////////////////////////////////////////////////////////////////
// CSuperComboBox window

class CSuperComboBox : public CComboBoxEx
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
	BOOL m_bHasImageList;
	BOOL m_bRecognizedMyself;
	BOOL m_bComboBoxEx;
	static HIMAGELIST m_himlSystem;

public:

	enum
	{
		AUTO_COMPLETE_DISABLED		= 0,
		AUTO_COMPLETE_FILE_SYSTEM,
		AUTO_COMPLETE_RECENTLY_USED
	};

// Operations
	void SetAutoComplete(INT nSource);

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
	BOOL IsComboBoxEx();
	BOOL AttachSystemImageList();
	int AddString(LPCTSTR lpszItem);
	int InsertString(int nIndex, LPCTSTR lpszItem);
	int GetLBTextLen(int nIndex) const;
	void GetLBText(int nIndex, CString &rString) const;

	// Generated message map functions
protected:
	CString m_strCurSel;
	virtual BOOL OnAddTemplate();
	virtual void PreSubclassWindow();
	CString m_strAutoAdd;
	BOOL m_bMustUninitOLE;
	DropHandler *m_pDropHandler;
	//{{AFX_MSG(CSuperComboBox)
	afx_msg BOOL OnEditchange();
	afx_msg BOOL OnSelchange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(const std::vector<String>& files);
	afx_msg void OnGetDispInfo(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
