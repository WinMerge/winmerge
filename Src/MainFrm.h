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
// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define BACKUP_FILE_EXT   _T(".bak")

#define ID_DIFFNUM    1


class CDiffView;
class CDirView;
class CDirDoc;
class CMergeEditView;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:	
	BOOL m_bShowUnique;
	BOOL m_bShowDiff;
	BOOL m_bShowIdent;
	BOOL m_bBackup;
	LOGFONT m_lfDiff;
	BOOL m_bFontSpecified;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL m_bHiliteSyntax;
	void SetDiffStatus(int nDiff, int nDiffCnt);
	BOOL m_bFirstTime;
	CString m_strSaveAsPath;
	void CleanupFileBufs();
	BOOL m_bIgnoreBlankLines;
	BOOL m_bIgnoreCase;
	void UpdateResources();
	void UpdateCurrentFileStatus(UINT nStatus);
	BOOL SyncFiles(LPCTSTR pszSrc, LPCTSTR pszDest);
	BOOL CreateBackup(LPCTSTR pszPath);
	UINT m_nTabSize;
	BOOL DoFileOpen(LPCTSTR pszLeft = NULL, LPCTSTR pszRight = NULL, BOOL bRecurse = FALSE);
	BOOL CheckSavePath(CString& strSavePath);
	CString m_strVssPath;
	CString m_strVssProject;
	int m_nVerSys;
	BOOL m_bHideBak;
	BOOL m_bIgnoreWhitespace;
	BOOL m_bScrollToFirst;
	void ShowMergeDoc(LPCTSTR szLeft, LPCTSTR szRight);
	CMergeEditView *m_pLeft, *m_pRight;
	CMergeDoc *m_pMergeDoc;
	CDirDoc *m_pDirDoc;
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	void GetFontProperties();
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnOptionsShowDifferent();
	afx_msg void OnOptionsShowIdentical();
	afx_msg void OnOptionsShowUnique();
	afx_msg void OnUpdateOptionsShowdifferent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowidentical(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowunique(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateHideBackupFiles(CCmdUI* pCmdUI);
	afx_msg void OnHelpGnulicense();
	afx_msg void OnProperties();
	afx_msg void OnHideBackupFiles();
	afx_msg void OnViewSelectfont();
	afx_msg void OnViewUsedefaultfont();
	afx_msg void OnUpdateViewUsedefaultfont(CCmdUI* pCmdUI);
	afx_msg void OnHelpContents();
	afx_msg void OnUpdateHelpContents(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CMainFrame *mf;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
