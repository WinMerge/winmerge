/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
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
#include "MergeEditView.h"
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
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = nullptr);
	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate();
	virtual void OnSize(UINT nType, int cx, int cy);
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CMergeEditSplitterView)
	afx_msg BOOL OnEraseBkgnd(CDC * pdc);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};
