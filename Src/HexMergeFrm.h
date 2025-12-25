/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
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
#include "BasicFlatStatusBar.h"
#include "MergeFrameCommon.h"

#define HEKSEDIT_INTERFACE_VERSION 3
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
	IHeaderBar * GetHeaderInterface() override;
	CHexMergeDoc * GetMergeDoc() { return m_pMergeDoc; }

	void UpdateAutoPaneResize();
	void UpdateSplitter();
	int GetActivePane();
	void SetActivePane(int nPane);
	CWnd* GetView(int nPane);

// Attributes
protected:
	CSplitterWndEx m_wndSplitter;
	CEditorFilePathBar m_wndFilePathBar;
	CBasicFlatStatusBar m_wndStatusBar[3];
	SCROLLINFO m_HScrollInfo[3];
	SCROLLINFO m_VScrollInfo[3];
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
	void SaveActivePane();
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
	afx_msg void OnViewSplitVertically();
	afx_msg void OnUpdateViewSplitVertically(CCmdUI* pCmdUI);
	afx_msg LRESULT OnStorePaneSizes(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

inline IHeaderBar *CHexMergeFrame::GetHeaderInterface()
{
	return &m_wndFilePathBar;
}

