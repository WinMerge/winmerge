/** 
 * @file  PropertyPageHost.h
 *
 * @brief Declaration of CPropertyPageHost class
 *
 * @note This code originates from AbstractSpoon / TodoList
 * (http://www.abstractspoon.com/) but is modified to use in
 * WinMerge.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_PROPERTYPAGEHOST_H__43CF5AE7_C70B_443D_BC8B_7DA1D0E082DD__INCLUDED_)
#define AFX_PROPERTYPAGEHOST_H__43CF5AE7_C70B_443D_BC8B_7DA1D0E082DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageHost window

struct PAGEITEM
{
	PAGEITEM(CPropertyPage* _pPage = NULL, LPCTSTR szTitle = NULL, DWORD dwData = 0) : 
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
	
	int GetActiveIndex();
	CPropertyPage* GetActivePage();
	BOOL AddPage(CPropertyPage* pPage, LPCTSTR szTitle = NULL, DWORD dwItemData = 0);
	BOOL SetActivePage(int nIndex, BOOL bAndFocus = TRUE);
	BOOL SetActivePage(CPropertyPage* pPage, BOOL bAndFocus = TRUE);
	int GetPageCount() { return m_aPages.GetSize(); }
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

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGEHOST_H__43CF5AE7_C70B_443D_BC8B_7DA1D0E082DD__INCLUDED_)
