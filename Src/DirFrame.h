/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DirFrame.h
 *
 * @brief Declaration file for CDirFrame
 *
 */
#pragma once

#include "EditorFilepathBar.h"
#include "MergeFrameCommon.h"

/////////////////////////////////////////////////////////////////////////////
// CDirFrame frame

/**
 * @brief Frame window for Directory Compare window
 */
class CDirFrame : public CMergeFrameCommon
{
	DECLARE_DYNCREATE(CDirFrame)
protected:
	CDirFrame();           // protected constructor used by dynamic creation

// Attributes
public:

private:

// Operations
public:
	void SetStatus(const tchar_t* szStatus);
	void SetCompareMethodStatusDisplay(int nCompMethod);
	void SetFilterStatusDisplay(const tchar_t* szFilter);
	CStatusBar  m_wndStatusBar;
	IHeaderBar * GetHeaderInterface();
	void UpdateResources();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirFrame)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

protected:
	CEditorFilePathBar m_wndFilePathBar;
	virtual ~CDirFrame();

	// Generated message map functions
	//{{AFX_MSG(CDirFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/**
 * @brief Get the interface to the header (path) bar
 */
inline IHeaderBar * CDirFrame::GetHeaderInterface() {
	return &m_wndFilePathBar;
}

