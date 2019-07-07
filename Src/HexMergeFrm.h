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
 * @file  HexMergeFrm.h
 *
 * @brief interface of the CHexMergeFrame class
 *
 */
#pragma once

#include "SplitterWndEx.h"
#include "EditorFilepathBar.h"
#include "MergeFrameCommon.h"

#define HEKSEDIT_INTERFACE_VERSION 2
#include "heksedit.h"

class CHexMergeDoc;

/** 
 * @brief Frame class for file compare, handles panes, statusbar etc.
 */
class CHexMergeFrame : public CMergeFrameCommon
{
	DECLARE_DYNCREATE(CHexMergeFrame)
public:
	CHexMergeFrame();

// Operations
public:
	void UpdateResources();
	void CloseNow();
	IHeaderBar * GetHeaderInterface();
	CHexMergeDoc * GetMergeDoc() { return m_pMergeDoc; }

	void UpdateAutoPaneResize();
	void UpdateSplitter();
	int GetActivePane();
	void SetActivePane(int nPane);

// Attributes
protected:
	CSplitterWndEx m_wndSplitter;
	CEditorFilePathBar m_wndFilePathBar;
	CStatusBar m_wndStatusBar[3];
// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHexMergeFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL DestroyWindow();
	protected:
	//}}AFX_VIRTUAL

// Implementation
private:
	void SavePosition();
	virtual ~CHexMergeFrame();
	void CreateHexWndStatusBar(CStatusBar &, CWnd *);
// Generated message map functions
private:
	void UpdateHeaderSizes();
	CHexMergeDoc * m_pMergeDoc;

	//{{AFX_MSG(CHexMergeFrame)
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnIdleUpdateCmdUI();
	afx_msg LRESULT OnStorePaneSizes(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline IHeaderBar *CHexMergeFrame::GetHeaderInterface()
{
	return &m_wndFilePathBar;
}

