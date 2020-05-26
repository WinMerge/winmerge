/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeEditSplitterView.h
 *
 * @brief Declaration file for CMergeEditSplitterView
 *
 */
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMergeEditSplitterView view
#include "SplitterWndEx.h"

class CMergeEditSplitterView : public CView
{
public:
	CMergeEditSplitterView();
	virtual ~CMergeEditSplitterView();
	DECLARE_DYNCREATE(CMergeEditSplitterView)

// Attributes
public:
	CSplitterWndEx m_wndSplitter;
	bool m_bDetailView;
	int m_nThisGroup;

protected:

private:

// Operations
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeEditSplitterView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr) override;
	virtual void OnDraw(CDC* pDC) override;
	virtual void OnInitialUpdate() override;
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
protected:
	using CWnd::OnMouseActivate;
	//{{AFX_MSG(CMergeEditSplitterView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC * pdc);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
