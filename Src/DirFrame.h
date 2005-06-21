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
 * @file  DirFrame.h
 *
 * @brief Declaration file for CDirFrame
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_DIRFRAME_H__95565903_35C4_11D1_BAA7_00A024706EDC__INCLUDED_)
#define AFX_DIRFRAME_H__95565903_35C4_11D1_BAA7_00A024706EDC__INCLUDED_

#include "EditorFilepathBar.h"
#include "DirCompStateBar.h"

class CompareStats;

/////////////////////////////////////////////////////////////////////////////
// CDirFrame frame

/**
 * @brief Frame window for Directory Compare window
 */
class CDirFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CDirFrame)
protected:
	CDirFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	void SetStatus(LPCTSTR szStatus);
	void SetFilterStatusDisplay(LPCTSTR szFilter);
	CStatusBar  m_wndStatusBar;
	void SetClosableCallback(bool (*canclose)(void *), void * param);
	IHeaderBar * GetHeaderInterface();
	void clearStatus();
	void ShowProcessingBar(BOOL bShow);
	void NotifyHideStateBar();
	void UpdateResources();
	void SetCompareStats(CompareStats *pCompareStats);
	void UpdateStats();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirFrame)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

protected:
	CEditorFilePathBar m_wndFilePathBar;
	CDirCompStateBar m_wndCompStateBar;
	/* @brief The state bar is active when 
	 * <ul>
	 *	<li> visible 
	 *	<li> the frame is unactive, and the state bar was visible when the frame was active
	 * </ul>
	 */
	BOOL m_bStateBarIsActive;
	/* @brief Track the activity of this frame, without delay.
	 * This flag must be updated when CMDIChildWnd::OnMDIActivate is called. 
	 * GetParentFrame()->GetActiveFrame() introduces some delay.
	 */
	BOOL m_bFrameIsActive;

	virtual ~CDirFrame();
	void ShowControlBar( CControlBar* pBar, BOOL bShow, BOOL bDelay );
	bool CreateStateBar();
	void SetStateBarLoc();

	// Generated message map functions
	//{{AFX_MSG(CDirFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool (*m_picanclose)(void *);
	void * m_param;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRFRAME_H__95565903_35C4_11D1_BAA7_00A024706EDC__INCLUDED_)
