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
 * @file  MainFrm.h
 *
 * @brief Declaration file for CMainFrame
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#include "bcmenu.h"
#include "OptionsMgr.h"

#define BACKUP_FILE_EXT   _T(".bak")

/**
 * @brief Flags used when opening files
 */
enum
{
	FFILEOPEN_NONE		= 0x0000,
	FFILEOPEN_NOMRU		= 0x0001,
	FFILEOPEN_READONLY	= 0x0002,
};

class CDiffView;
class CDirView;
class CDirDoc;
class CMergeDoc;
class CMergeEditView;
class CMergeDiffDetailView;


// typed lists (homogenous pointer lists)
typedef CTypedPtrList<CPtrList, CMergeEditView *> MergeEditViewList;
typedef CTypedPtrList<CPtrList, CMergeDiffDetailView *> MergeDetailViewList;
typedef CTypedPtrList<CPtrList, CDirView *> DirViewList;
typedef CTypedPtrList<CPtrList, CMergeDoc *> MergeDocList;
typedef CTypedPtrList<CPtrList, CDirDoc *> DirDocList;

class CRegOptions;

/**
 * @brief Frame class containing save-routines etc
 */
class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:	
	BOOL m_bShowErrors;
	LOGFONT m_lfDiff;
	BOOL m_bFontSpecified;
	BOOL m_bReuseDirDoc; // policy to reuse existing dir doc

// Operations
public:
	BOOL DeleteFileOrError(LPCTSTR szFile);
	BOOL SyncFiles(LPCTSTR pszSrc, LPCTSTR pszDest, CString * psError);
	BOOL DoSyncFiles(LPCTSTR pszSrc, LPCTSTR pszDest, CString * psError);
	BOOL DoFileOpen(LPCTSTR pszLeft = NULL, LPCTSTR pszRight = NULL,
		DWORD dwLeftFlags = 0, DWORD dwRightFlags = 0, BOOL bRecurse = FALSE);
	void ShowMergeDoc(CDirDoc * pDirDoc, LPCTSTR szLeft, LPCTSTR szRight, BOOL bROLeft, BOOL bRORight, int cpleft, int cpright, PackingInfo * infoUnpacker = NULL);
	void UpdateResources();
	HMENU NewDefaultMenu();
	BOOL CreateBackup(LPCTSTR pszPath);
	BOOL CheckSavePath(CString& strSavePath);
	BOOL SaveToVersionControl(CString& strSavePath);
	CString SetStatus(LPCTSTR status);
	void ApplyViewWhitespace();
	BOOL OpenFileToExternalEditor(CString file);
	CString GetDefaultEditor();
	void SetEOLMixed(BOOL bAllow);

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
	CRegOptions m_options;
	BOOL m_bFirstTime;
	CString m_strSaveAsPath; /**< "3rd path" where output saved if given */
	BOOL m_bIgnoreRegExp; /**< Are regular expression linefilters enabled? */
	CString m_sPattern; /**< Regular expression linefilters */
	UINT m_nTabSize; /**< Tabsize in spaces in editor */
	CString m_strVssPath;
	CString m_strVssProjectBase;
	CString m_strVssUser;      // BSP - Visual Source Safe User ID
	CString m_strVssPassword;      // BSP - Visual Source Safe Password
	CString m_strVssDatabase;
	CString m_strVssProjectFull;

	int m_nVerSys;
	UINT m_nTabType; /**< 0-editor inserts tabs, 1-editor inserts spaces */
	CString m_sExtEditorPath; /**< Path to external editor */
	CString m_strLeftDesc; /**< Left side description text proxy */
	CString m_strRightDesc; /**< Right side description text proxy */
	BOOL m_CheckOutMulti;
	BOOL m_bVCProjSync;

    int m_nCompMethod; /**< CompareMethod : CPropCompare::BY_CONTENTS or CPropCompare::BY_DATE */

// Implementation data

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
	afx_msg void OnOptionsShowSkipped();
	afx_msg void OnUpdateOptionsShowdifferent(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowidentical(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowuniqueleft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowuniqueright(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowBinaries(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsShowSkipped(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) ;
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateHideBackupFiles(CCmdUI* pCmdUI);
	afx_msg void OnHelpGnulicense();
	afx_msg void OnOptions();
	afx_msg void OnHideBackupFiles();
	afx_msg void OnViewSelectfont();
	afx_msg void OnViewUsedefaultfont();
	afx_msg void OnUpdateViewUsedefaultfont(CCmdUI* pCmdUI);
	afx_msg void OnHelpContents();
	afx_msg void OnUpdateHelpContents(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnViewWhitespace();
	afx_msg void OnUpdateViewWhitespace(CCmdUI* pCmdUI);
	afx_msg void OnToolsGeneratePatch();
	afx_msg void OnDropFiles(HDROP dropInfo);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPluginUnpackMode(UINT nID);
	afx_msg void OnUpdatePluginUnpackMode(CCmdUI* pCmdUI);
	afx_msg void OnSaveConfigData();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void addToMru(LPCTSTR szItem, LPCTSTR szRegSubKey, UINT nMaxItems = 20);
	// builds the regular expression list if the
	// user choose to ignore Ignore changes affecting only lines 
	// that match the specified regexp. 
	void RebuildRegExpList();
	// destroy the regular expression list and free up the memory
	void FreeRegExpList();
	void GetAllViews(MergeEditViewList * pEditViews, MergeDetailViewList * pDetailViews, DirViewList * pDirViews);
	void GetAllMergeDocs(MergeDocList * pMergeDocs);
	void GetAllDirDocs(DirDocList * pDirDocs);
	void RedisplayAllDirDocs();
	CMergeDoc * GetMergeDocToShow(CDirDoc * pDirDoc, BOOL * pNew);
	CDirDoc * GetDirDocToShow(BOOL * pNew);
	BOOL GetWordFile(HANDLE pfile, TCHAR * buffer, TCHAR * charset = NULL);
	BOOL ReLinkVCProj(CString strSavePath,CString * psError);
};

extern CMainFrame *mf;

BOOL DeleteFileSilently(LPCTSTR szFile, CString * psError);
BOOL DeleteDirSilently(LPCTSTR szDir, CString * psError);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)

