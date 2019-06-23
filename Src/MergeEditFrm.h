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
 * @file  MergeEditFrm.h
 *
 * @brief interface of the CMergeEditFrame class
 *
 */
#pragma once

#include "SplitterWndEx.h"
#include "MergeEditSplitterView.h"
#include "MergeStatusBar.h"
#include "EditorFilepathBar.h"
#include "DiffViewBar.h"
#include "LocationBar.h"
#include "MergeFrameCommon.h"

class CMergeDoc;

/** 
 * @brief Frame class for file compare, handles panes, statusbar etc.
 */
class CMergeEditFrame : public CMergeFrameCommon
{
	DECLARE_DYNCREATE(CMergeEditFrame)
public:
	CMergeEditFrame();

// Operations
public:
	void UpdateResources();
	void CloseNow();
	IHeaderBar * GetHeaderInterface();
	CMergeDoc * GetMergeDoc() { return m_pMergeDoc; }

	void UpdateAutoPaneResize();
	void UpdateSplitter();
	CSplitterWndEx& GetSplitter() { return m_wndSplitter; };

// Attributes
protected:
	CSplitterWndEx m_wndSplitter;
	CEditorFilePathBar m_wndFilePathBar;
	CDiffViewBar m_wndDetailBar;
	CMergeEditSplitterView *m_pwndDetailMergeEditSplitterView;
	CMergeStatusBar m_wndStatusBar;
	CLocationBar m_wndLocationBar;

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeEditFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL DestroyWindow();
	protected:
	//}}AFX_VIRTUAL

// Implementation
private:
	bool EnsureValidDockState(CDockState& state);
	void SavePosition();
	virtual ~CMergeEditFrame();
	CSplitterWndEx& GetMergeEditSplitterWnd(int iRow)
	{ return static_cast<CMergeEditSplitterView *>(m_wndSplitter.GetPane(iRow, 0))->m_wndSplitter; }

// Generated message map functions
private:
	void UpdateHeaderSizes();
	CMergeDoc * m_pMergeDoc;

	//{{AFX_MSG(CMergeEditFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnViewSplitVertically();
	afx_msg void OnUpdateViewSplitVertically(CCmdUI* pCmdUI);
	afx_msg LRESULT OnStorePaneSizes(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnIdleUpdateCmdUI();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
