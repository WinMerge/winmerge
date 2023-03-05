/** 
 * @file  PropertyPageHost.h
 *
 * @brief Declaration of CPropertyPageHost class
 *
 * @note This code originates from AbstractSpoon / TodoList
 * (http://www.abstractspoon.com/) but is modified to use in
 * WinMerge.
 */
#pragma once

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageHost window

struct PAGEITEM
{
	PAGEITEM(CPropertyPage* _pPage = nullptr, const tchar_t* szTitle = nullptr, DWORD dwData = 0) : 
			pPage(_pPage), sTitle(szTitle), dwItemData(dwData) {}

	CPropertyPage* pPage;
	CString sTitle;
	DWORD dwItemData;
};

class CPropertyPageHost : public CWnd
{
// Construction
public:
	CPropertyPageHost();

	BOOL Create(LPRECT lpRect, CWnd* pParent, UINT uCtrlID = AFX_IDW_PANE_FIRST);
	BOOL Create(UINT nRefCtrlID, CWnd* pParent, UINT uCtrlID = AFX_IDW_PANE_FIRST);
	void OnOK();
	void UpdatePagesData();
	
	int GetActiveIndex();
	CPropertyPage* GetActivePage();
	bool AddPage(CPropertyPage* pPage, const tchar_t* szTitle = nullptr, DWORD dwItemData = 0);
	bool SetActivePage(int nIndex, bool bAndFocus = true);
	bool SetActivePage(CPropertyPage* pPage, bool bAndFocus = true);
	int GetPageCount() { return (int) m_aPages.GetSize(); }
	CString GetPageTitle(int nIndex);
	DWORD GetPageItemData(int nIndex);
	CPropertyPage* GetPage(int nIndex);
	CPropertyPage* FindPage(DWORD dwItemData);

protected:
	CArray<PAGEITEM, PAGEITEM&> m_aPages;
	int m_nSelIndex;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageHost)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertyPageHost();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyPageHost)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnGetCurrentPageHwnd(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	int FindPage(CPropertyPage* pPage);

};
