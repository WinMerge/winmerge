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
 * @brief interface of the CHexMergeFrame class
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "SplitterWndEx.h"
#include "EditorFilepathBar.h"

#define HEKSEDIT_INTERFACE_VERSION 1
#include "heksedit.h"

class CHexMergeDoc;

/** 
 * @brief Frame class for file compare, handles panes, statusbar etc.
 */
class CHexMergeFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CHexMergeFrame)
public:
	CHexMergeFrame();

// Operations
public:
	void UpdateResources();
	void CloseNow();
	IHeaderBar * GetHeaderInterface();
	void SetSharedMenu(HMENU hMenu) { m_hMenuShared = hMenu; };
	CHexMergeDoc * GetMergeDoc() { return m_pMergeDoc; }
	void SetLastCompareResult(int nResult);

	void UpdateAutoPaneResize();
	void UpdateSplitter();


// Attributes
protected:
	CSplitterWndEx m_wndSplitter;
	CEditorFilePathBar m_wndFilePathBar;
	CStatusBar m_wndLeftStatusBar;
	CStatusBar m_wndRightStatusBar;
// Overrides
public:
	virtual void GetMessageString(UINT nID, CString& rMessage) const;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHexMergeFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
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
	int m_nLastSplitPos;
	void UpdateHeaderSizes();
	BOOL m_bActivated;
	CHexMergeDoc * m_pMergeDoc;
	HICON m_hIdentical;
	HICON m_hDifferent;

	//{{AFX_MSG(CHexMergeFrame)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnIdleUpdateCmdUI();
	afx_msg LRESULT OnStorePaneSizes(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
