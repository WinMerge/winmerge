// ListVwEx.h : interface of the CListViewEx class
//
// This class provedes a full row selection mode for the report
// mode list view control.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1996 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
#ifndef LISTVIEWEX_H
#define LISTVIEWEX_H

class CListEx
{
	friend class CListViewEx;
	friend class CListCtrlEx;

// Construction
public:
	CListEx(CListCtrl & listctrl);

public:
	void SetExpandTabs(BOOL bExpand) { m_nTabFlag = bExpand? DT_EXPANDTABS : 0; }
	BOOL SetFullRowSel(BOOL bFillRowSel);
	BOOL GetFullRowSel() const { return m_bFullRowSel; }
	void AddItem(int nItem,int nSubItem,LPCTSTR strItem);
	void SetImage(int nItem, UINT nImage);

public:
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	int InsertColumn( int nCol, DWORD idstrColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1 );
	void SelectItems(int nFirst, int nLast, BOOL bSelected = TRUE);
	void ClearSelection();
	virtual ~CListEx() { }
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);
	void RepaintSelectedItems();


// Implementation - state icon width
	afx_msg LRESULT OnSetImageList(WPARAM wParam, LPARAM lParam);

// Implementation - list view colors
	afx_msg LRESULT OnSetTextColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTextBkColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam);

// message map functions
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	LRESULT Default();


// Attributes
protected:
	BOOL m_bFullRowSel;
	UINT m_nTabFlag;

	BOOL m_bClientWidthSel;

// Implementation - client area width
	int m_cxClient;

// Implementation - state icon width
	int m_cxStateImageOffset;

// Implementation - list view colors
	COLORREF m_clrText;
	COLORREF m_clrTextBk;
	COLORREF m_clrBkgnd;

// Implementation - MFC object bound to window
	CListCtrl & m_listctrl;
	CListCtrl & GetListCtrl() { return m_listctrl; }
};


#include <afxcview.h>

class CListViewEx : public CListView
{
	DECLARE_DYNCREATE(CListViewEx)

// Construction
public:
	CListViewEx() : m_listex(GetListCtrl()) { }

public:
	void SetExpandTabs(BOOL bExpand) 
		{ m_listex.SetExpandTabs(bExpand); }
	BOOL SetFullRowSel(BOOL bFillRowSel) 
		{ return m_listex.SetFullRowSel(bFillRowSel); }
	BOOL GetFullRowSel() const 
		{ return m_listex.GetFullRowSel(); }
	void AddItem(int nItem,int nSubItem, LPCTSTR strItem)
		{ m_listex.AddItem(nItem, nSubItem, strItem); }
	void SetImage(int nItem, UINT nImage)
		{ m_listex.SetImage(nItem, nImage); }

// Overrides
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
		{ m_listex.DrawItem(lpDrawItemStruct); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListViewEx)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	int InsertColumn( int nCol, DWORD idstrColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1 )
		{ return m_listex.InsertColumn(nCol, idstrColumnHeading, nFormat, nWidth, nSubItem); }
	void SelectItems(int nFirst, int nLast, BOOL bSelected = TRUE)
		{ m_listex.SelectItems(nFirst, nLast, bSelected); }
	void ClearSelection()
		{ m_listex.ClearSelection(); }
	virtual ~CListViewEx() { }
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const
		{ CListView::Dump(dc); m_listex.Dump(dc); }
#endif

protected:
	LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
		{ return m_listex.MakeShortString(pDC, lpszLong, nColumnLen, nOffset); }
	void RepaintSelectedItems()
		{ m_listex.RepaintSelectedItems(); }


// Implementation - state icon width
	afx_msg LRESULT OnSetImageList(WPARAM wParam, LPARAM lParam) 
		{ return m_listex.OnSetImageList(wParam, lParam); }

// Implementation - list view colors
	afx_msg LRESULT OnSetTextColor(WPARAM wParam, LPARAM lParam)
		{ return m_listex.OnSetTextColor(wParam, lParam); }
	afx_msg LRESULT OnSetTextBkColor(WPARAM wParam, LPARAM lParam)
		{ return m_listex.OnSetTextBkColor(wParam, lParam); }
	afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam)
		{ return m_listex.OnSetBkColor(wParam, lParam); }

// Generated message map functions
protected:
	//{{AFX_MSG(CListViewEx)
	afx_msg void OnSize(UINT nType, int cx, int cy) { m_listex.OnSize(nType, cx, cy); CListView::OnSize(nType, cx, cy); }
	afx_msg void OnPaint() { m_listex.OnPaint(); CListView::OnPaint(); }
	afx_msg void OnSetFocus(CWnd* pOldWnd) { CListView::OnSetFocus(pOldWnd); m_listex.OnSetFocus(pOldWnd); }
	afx_msg void OnKillFocus(CWnd* pNewWnd) { CListView::OnKillFocus(pNewWnd); m_listex.OnKillFocus(pNewWnd); }
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CListEx m_listex;
};

class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNCREATE(CListCtrlEx)

// Construction
public:
	CListCtrlEx();

public:
	void SetExpandTabs(BOOL bExpand) 
		{ m_listex.SetExpandTabs(bExpand); }
	BOOL SetFullRowSel(BOOL bFillRowSel) 
		{ return m_listex.SetFullRowSel(bFillRowSel); }
	BOOL GetFullRowSel() const 
		{ return m_listex.GetFullRowSel(); }
	void AddItem(int nItem,int nSubItem, LPCTSTR strItem)
		{ m_listex.AddItem(nItem, nSubItem, strItem); }
	void SetImage(int nItem, UINT nImage)
		{ m_listex.SetImage(nItem, nImage); }

// Overrides
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
		{ m_listex.DrawItem(lpDrawItemStruct); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	int InsertColumn( int nCol, DWORD idstrColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1 )
		{ return m_listex.InsertColumn(nCol, idstrColumnHeading, nFormat, nWidth, nSubItem); }
	void SelectItems(int nFirst, int nLast, BOOL bSelected = TRUE)
		{ m_listex.SelectItems(nFirst, nLast, bSelected); }
	void ClearSelection()
		{ m_listex.ClearSelection(); }
	virtual ~CListCtrlEx() { }
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const
		{ CListCtrl::Dump(dc); m_listex.Dump(dc); }
#endif

protected:
	LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
		{ return m_listex.MakeShortString(pDC, lpszLong, nColumnLen, nOffset); }
	void RepaintSelectedItems()
		{ m_listex.RepaintSelectedItems(); }

// Implementation - state icon width
	afx_msg LRESULT OnSetImageList(WPARAM wParam, LPARAM lParam)
		{ return m_listex.OnSetImageList(wParam, lParam); }

// Implementation - list view colors
	afx_msg LRESULT OnSetTextColor(WPARAM wParam, LPARAM lParam)
		{ return m_listex.OnSetTextColor(wParam, lParam); }
	afx_msg LRESULT OnSetTextBkColor(WPARAM wParam, LPARAM lParam)
		{ return m_listex.OnSetTextBkColor(wParam, lParam); }
	afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam)
		{ return m_listex.OnSetBkColor(wParam, lParam); }

// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlEx)
	afx_msg void OnSize(UINT nType, int cx, int cy) { m_listex.OnSize(nType, cx, cy); CListCtrl::OnSize(nType, cx, cy); }
	afx_msg void OnPaint() { m_listex.OnPaint(); CListCtrl::OnPaint(); }
	afx_msg void OnSetFocus(CWnd* pOldWnd) { CListCtrl::OnSetFocus(pOldWnd); m_listex.OnSetFocus(pOldWnd); }
	afx_msg void OnKillFocus(CWnd* pNewWnd) { CListCtrl::OnKillFocus(pNewWnd); m_listex.OnKillFocus(pNewWnd); }
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CListEx m_listex;
};

#endif
/////////////////////////////////////////////////////////////////////////////
