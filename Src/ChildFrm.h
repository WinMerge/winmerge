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
// ID line follows -- this is updated by SVN
// $Id$

#if !defined(AFX_CHILDFRM_H__BBCD4F8E_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_CHILDFRM_H__BBCD4F8E_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#include "SplitterWndEx.h"
#include "MergeEditStatus.h"
#include "EditorFilepathBar.h"
#include "DiffViewBar.h"
#include "LocationBar.h"

class CMergeDoc;

/** 
 * @brief Frame class for file compare, handles panes, statusbar etc.
 */
class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Operations
public:
	void UpdateResources();
	void CloseNow();
	IHeaderBar * GetHeaderInterface();
	void SetSharedMenu(HMENU hMenu) { m_hMenuShared = hMenu; };
	CMergeDoc * GetMergeDoc() { return m_pMergeDoc; }
	void SetLastCompareResult(int nResult);

	void UpdateAutoPaneResize();
	void UpdateSplitter();


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



// Overrides
public:
	virtual void GetMessageString(UINT nID, CString& rMessage) const;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL DestroyWindow();
	protected:
	//}}AFX_VIRTUAL

// Implementation
private:
	BOOL EnsureValidDockState(CDockState& state);
	void UpdateDiffDockbarHeight(int DiffPanelHeight);
	void SavePosition();
	virtual ~CChildFrame();

// Generated message map functions
private:
	int m_nLastSplitPos;
	void UpdateHeaderSizes();
	BOOL m_bActivated;
	CMergeDoc * m_pMergeDoc;
	HICON m_hIdentical;
	HICON m_hDifferent;

	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnStorePaneSizes(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__BBCD4F8E_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
