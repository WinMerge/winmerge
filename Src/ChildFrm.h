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
 * @file  ChildFrm.h
 *
 * @brief interface of the CChildFrame class
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_CHILDFRM_H__BBCD4F8E_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_CHILDFRM_H__BBCD4F8E_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#include "SplitterWndEx.h"
#include "MergeEditStatus.h"
#include "EditorFilepathBar.h"
#include "DiffViewBar.h"
#include "LocationBar.h"

/** 
 * @brief Frame class for file compare, handles panes, statusbar etc.
 */
class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();
	IHeaderBar * GetHeaderInterface();

// Attributes
protected:
	CSplitterWndEx m_wndSplitter;
	CEditorFilePathBar m_wndFilePathBar;
	CDiffViewBar m_wndDetailBar;
	CSplitterWndEx m_wndDetailSplitter;
	CStatusBar m_wndStatusBar;
	CLocationBar m_wndLocationBar;
	// Object that displays status line info for one side of a merge view
	class MergeStatus : public IMergeEditStatus
	{
	public:
		// ctr
		MergeStatus(CChildFrame * pFrame, int base);
		// Implement MergeEditStatus
		void SetLineInfo(LPCTSTR szLine, int nColumn, int nColumns,
			int nChar, int nChars, LPCTSTR szEol);
		void UpdateResources();
	protected:
		void Update();
	private:
		CChildFrame * m_pFrame;
		int m_base; /**< 0 for left, 1 for right */
		CString m_sLine;
		int m_nColumn; /**< Current column, tab-expanded */
		int m_nColumns; /**< Amount of columns, tab-expanded */
		int m_nChar; /**< Current char */
		int m_nChars; /**< Amount of chars in line */
		CString m_sEol;
		CString m_sEolDisplay;
	};
	friend class MergeStatus; // MergeStatus accesses status bar
	MergeStatus m_leftStatus;
	MergeStatus m_rightStatus;

// Operations
public:
	void UpdateResources();
private:
	BOOL EnsureValidDockState(CDockState& state);

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL DestroyWindow();
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	void UpdateDiffDockbarHeight(int DiffPanelHeight);
	void SavePosition();
	virtual ~CChildFrame();
	void CloseNow();

// Generated message map functions
protected:
	int m_nLastSplitPos;
	void UpdateHeaderSizes();
	BOOL m_bActivated;
	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__BBCD4F8E_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
