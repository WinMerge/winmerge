/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
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
	class CPreviewNumPageButton : public CButton
	{
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	} m_wndPreviewNumPage;

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeEditFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL OnBarCheck(UINT nID);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL DestroyWindow();
	virtual void OnSetPreviewMode(BOOL bPreview, CPrintPreviewState* pState) override;
	protected:
	//}}AFX_VIRTUAL

// Implementation
private:
	void SavePosition();
	void SaveActivePane();
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
