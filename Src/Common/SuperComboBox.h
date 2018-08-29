#pragma once

// SuperComboBox.h : header file
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
	CSuperComboBox(bool bAdd = true, UINT idstrAddText = 0);
	virtual ~CSuperComboBox();

// Attributes
protected:
	bool m_bEditChanged;
	bool m_bAutoComplete;
	bool m_bDoComplete;
	bool m_bHasImageList;
	bool m_bRecognizedMyself;
	bool m_bComboBoxEx;
	bool m_bMustUninitOLE;
	static HIMAGELIST m_himlSystem;
	CString m_strCurSel;
	CString m_strAutoAdd;

	DropHandler *m_pDropHandler;

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
	void SetAutoAdd(bool bAdd = true, UINT idstrAddText = 0);
	void SaveState(LPCTSTR szRegSubKey, bool bCanBeEmpty = false, int nMaxItems = 20);
	void LoadState(LPCTSTR szRegSubKey, bool bCanBeEmpty = false, int nMaxItems = 20);
	bool AttachSystemImageList();
	int AddString(LPCTSTR lpszItem);
	int InsertString(int nIndex, LPCTSTR lpszItem);
	int GetLBTextLen(int nIndex) const;
	void GetLBText(int nIndex, CString &rString) const;

	// Generated message map functions
protected:
	bool IsComboBoxEx() const;

	virtual BOOL OnAddTemplate();
	virtual void PreSubclassWindow();

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
