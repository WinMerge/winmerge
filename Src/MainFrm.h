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
#include "VSSHelper.h"

#define BACKUP_FILE_EXT   _T(".bak")

/**
 * @brief Flags used when opening files
 */
enum
{
	FFILEOPEN_NONE		= 0x0000,
	FFILEOPEN_NOMRU		= 0x0001, /**< Do not add this path to MRU list */
	FFILEOPEN_READONLY	= 0x0002, /**< Open this path as read-only */
	FFILEOPEN_CMDLINE	= 0x0010, /**< Path is read from commandline */
	FFILEOPEN_PROJECT	= 0x0020, /**< Path is read from project-file */
};

/**
 * @brief Supported versioncontrol systems.
 */
enum
{
	VCS_NONE = 0,
	VCS_VSS4,
	VCS_VSS5,
	VCS_CLEARCASE,
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
class PackingInfo;

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
	LOGFONT m_lfDiff; /**< MergeView user-selected font */
	LOGFONT m_lfDir; /**< DirView user-selected font */
	BOOL m_bReuseDirDoc; // policy to reuse existing dir doc

// Operations
public:
	HMENU NewDirViewMenu();
	HMENU NewMergeViewMenu();
	HMENU NewDefaultMenu(int ID = 0);
	HMENU GetScriptsSubmenu(HMENU mainMenu);
	HMENU GetPrediffersSubmenu(HMENU mainMenu);
	void UpdatePrediffersMenu();

	BOOL SyncFileToVCS(LPCTSTR pszSrc, LPCTSTR pszDest,	BOOL &bApplyToAll,
		CString *psError);
	BOOL DoFileOpen(LPCTSTR pszLeft = NULL, LPCTSTR pszRight = NULL,
		DWORD dwLeftFlags = 0, DWORD dwRightFlags = 0, BOOL bRecurse = FALSE, CDirDoc *pDirDoc = NULL);
	void ShowMergeDoc(CDirDoc * pDirDoc, LPCTSTR szLeft, LPCTSTR szRight, BOOL bROLeft, BOOL bRORight, int cpleft =-1, int cpright =-1, PackingInfo * infoUnpacker = NULL);
	void UpdateResources();
	BOOL CreateBackup(LPCTSTR pszPath);
	int HandleReadonlySave(CString& strSavePath, BOOL bMultiFile, BOOL &bApplyToAll);
	BOOL SaveToVersionControl(CString& strSavePath);
	CString SetStatus(LPCTSTR status);
	void ApplyViewWhitespace();
	BOOL OpenFileToExternalEditor(CString file);
	CString GetDefaultEditor();
	void SetEOLMixed(BOOL bAllow);
	void SelectFilter();
	void ShowVSSError(CException *e, CString strItem);
	void ShowHelp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation methods
protected:
	virtual ~CMainFrame();

// Public implementation data
public:
	CRegOptions m_options; /**< Options manager */
	BOOL m_bFirstTime; /**< If first time frame activated, get  pos from reg */
	CString m_strSaveAsPath; /**< "3rd path" where output saved if given */
	BOOL m_bEscShutdown; /**< If commandline switch -e given ESC closes appliction */
	VSSHelper m_vssHelper;

	/**
	 * @name Version Control System (VCS) integration.
	 */
	/*@{*/ 
	CString m_strVssUser; /**< Visual Source Safe User ID */
	CString m_strVssPassword; /**< Visual Source Safe Password */
	CString m_strVssDatabase;
	BOOL m_CheckOutMulti; /**< Suppresses VSS int. code asking checkout for every file */
	BOOL m_bVCProjSync; /**< VC project opened from VSS sync? */
	BOOL m_bVssSuppressPathCheck; /**< Suppresses VSS int code asking about different path */
	/*@}*/

	/**
	 * @name Textual labels/descriptors
	 * These descriptors overwrite dir/filename usually shown in headerbar
	 * and can be given from command-line. For example version control
	 * system can set these to "WinMerge v2.1.2.0" and "WinMerge 2.1.4.0"
	 * which is more pleasant and informative than temporary paths.
	 */
	/*@{*/ 
	/** Left descriptor */
	CString m_strLeftDesc;
	/** Right descriptor */
	CString m_strRightDesc;
	/*@}*/

// Implementation data

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	BCMenu		m_default;
	BCMenu		m_mergeViewMenu;
	BCMenu		m_dirViewMenu;

// Generated message map functions
protected:
	void GetFontProperties();
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) ;
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
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
	afx_msg void OnFileOpen();
	afx_msg void OnHelpGnulicense();
	afx_msg void OnOptions();
	afx_msg void OnViewSelectfont();
	afx_msg void OnUpdateViewSelectfont(CCmdUI* pCmdUI);
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
	afx_msg void OnUpdatePluginUnpackMode(CCmdUI* pCmdUI);
	afx_msg void OnPluginUnpackMode(UINT nID);
	afx_msg void OnUpdateReloadPlugins(CCmdUI* pCmdUI);
	afx_msg void OnReloadPlugins();
	afx_msg void OnSaveConfigData();
	afx_msg void OnFileNew();
	afx_msg void OnToolsFilters();
	afx_msg void OnHelpMerge7zmismatch();
	afx_msg void OnUpdateHelpMerge7zmismatch(CCmdUI* pCmdUI);
	afx_msg void OnViewStatusBar();
	afx_msg void OnViewToolbar();
	afx_msg void OnFileOpenproject();
	afx_msg LRESULT OnCopyData(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void addToMru(LPCTSTR szItem, LPCTSTR szRegSubKey, UINT nMaxItems = 20);
	// builds the regular expression list if the
	// user choose to ignore Ignore changes affecting only lines 
	// that match the specified regexp. 
	void RebuildRegExpList(BOOL bShowError);
	// destroy the regular expression list and free up the memory
	void FreeRegExpList();
	void GetAllViews(MergeEditViewList * pEditViews, MergeDetailViewList * pDetailViews, DirViewList * pDirViews);
	void GetAllMergeDocs(MergeDocList * pMergeDocs);
	void GetAllDirDocs(DirDocList * pDirDocs);
	BOOL IsComparing();
	void RedisplayAllDirDocs();
	CMergeDoc * GetMergeDocToShow(CDirDoc * pDirDoc, BOOL * pNew);
	CDirDoc * GetDirDocToShow(BOOL * pNew);
	void ShowFontChangeMessage();
};

extern CMainFrame *mf;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)

