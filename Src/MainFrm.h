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

#include "bcmenu.h"

#define BACKUP_FILE_EXT   _T(".bak")


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
	BOOL m_bShowUniqueLeft;
	BOOL m_bShowUniqueRight;
	BOOL m_bShowDiff;
	BOOL m_bShowIdent;
	BOOL m_bShowBinaries;
	BOOL m_bBackup;
	LOGFONT m_lfDiff;
	BOOL m_bFontSpecified;
	BOOL m_bEolSensitive;

// Operations
public:
	void ConvertPathToSlashes( LPTSTR path );
	BOOL DeleteFileOrError(LPCTSTR szFile);
	void rptStatus(BYTE code);
	void clearStatus();
	BOOL SyncFiles(LPCTSTR pszSrc, LPCTSTR pszDest, CString * psError);
	BOOL DoSyncFiles(LPCTSTR pszSrc, LPCTSTR pszDest, CString * psError);
	void UpdateCurrentFileStatus(UINT nStatus, int idx);
	BOOL DoFileOpen(LPCTSTR pszLeft = NULL, LPCTSTR pszRight = NULL, BOOL bRecurse = FALSE);
	void ShowMergeDoc(LPCTSTR szLeft, LPCTSTR szRight);
	void UpdateResources();
	HMENU NewDefaultMenu();
	BOOL CreateBackup(LPCTSTR pszPath);
	BOOL CheckSavePath(CString& strSavePath);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

// Implementation methods
protected:
	void CleanupFileBufs();
	virtual ~CMainFrame();

// Public implementation data
public:
	BOOL m_bFirstTime;
	CString m_strSaveAsPath;
	BOOL m_bIgnoreBlankLines;
	BOOL m_bIgnoreCase;
	BOOL m_bIgnoreRegExp;
	CString m_sPattern;
	UINT m_nTabSize;
	CString m_strVssPath;
	CString m_strVssProject;
	CString m_strVssUser;      // BSP - Visual Source Safe User ID
	CString m_strVssPassword;      // BSP - Visual Source Safe Password
	int m_nVerSys;
	BOOL m_bHideBak;
	int m_nIgnoreWhitespace;
	BOOL m_bScrollToFirst;
	CMergeEditView *m_pLeft, *m_pRight;
	CMergeDoc *m_pMergeDoc;
	CDirDoc *m_pDirDoc;
	UINT m_nTabType;
	BOOL m_bViewWhitespace;

// Implementation data
protected:
	int m_nStatusFileSame;
	int m_nStatusFileBinSame;
	int m_nStatusFileDiff;
	int m_nStatusFileBinDiff;
	int m_nStatusFileError;
	int m_nStatusLeftFileOnly;
	int m_nStatusLeftDirOnly;
	int m_nStatusRightFileOnly;
	int m_nStatusRightDirOnly;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	BCMenu		m_default;

// Generated message map functions
protected:
	void GetFontProperties();
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnOptionsShowDifferent();
	afx_msg void OnOptionsShowIdentical();
	afx_msg void OnOptionsShowUniqueLeft();
	afx_msg void OnOptionsShowUniqueRight();
	afx_msg void OnOptionsShowBinaries();
	afx_msg void OnUpdateOptionsShowdifferent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowidentical(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowuniqueleft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowuniqueright(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowBinaries(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) ;
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
	afx_msg void OnViewWhitespace();
	afx_msg void OnUpdateViewWhitespace(CCmdUI* pCmdUI);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void addToMru(LPCSTR szItem, LPCSTR szRegSubKey, UINT nMaxItems = 20);
	// builds the regular expression list if the
	// user choose to ignore Ignore changes affecting only lines 
	// that match the specified regexp. 
	void RebuildRegExpList();
	// destroy the regular expression list and free up the memory
	void FreeRegExpList();
};

extern CMainFrame *mf;

BOOL DeleteFileSilently(LPCTSTR szFile, CString * psError);
BOOL DeleteDirSilently(LPCTSTR szDir, CString * psError);
CString GetSystemErrorDesc(int nerr);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
