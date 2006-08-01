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

#include "ToolBarXPThemes.h"
#include "OptionsMgr.h"
#include "VSSHelper.h"
struct FileLocation;

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

enum { WM_NONINTERACTIVE = 888 }; // timer value

class BCMenu;
class CDiffView;
class CDirView;
class CDirDoc;
class CMergeDoc;
class CMergeEditView;
class CMergeDiffDetailView;
class SyntaxColors;


// typed lists (homogenous pointer lists)
typedef CTypedPtrList<CPtrList, CMergeEditView *> MergeEditViewList;
typedef CTypedPtrList<CPtrList, CMergeDiffDetailView *> MergeDetailViewList;
typedef CTypedPtrList<CPtrList, CDirView *> DirViewList;
typedef CTypedPtrList<CPtrList, CMergeDoc *> MergeDocList;
typedef CTypedPtrList<CPtrList, CDirDoc *> DirDocList;

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
	BOOL m_bShowErrors; /**< Show folder compare error items? */
	LOGFONT m_lfDiff; /**< MergeView user-selected font */
	LOGFONT m_lfDir; /**< DirView user-selected font */

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
		DWORD dwLeftFlags = 0, DWORD dwRightFlags = 0, BOOL bRecurse = FALSE, CDirDoc *pDirDoc = NULL, CString prediffer = _T(""));
	int ShowMergeDoc(CDirDoc * pDirDoc, const FileLocation & filelocLeft,
		const FileLocation & filelocRight, BOOL bROLeft, BOOL bRORight, PackingInfo * infoUnpacker = NULL);
	void UpdateResources();
	BOOL CreateBackup(LPCTSTR pszPath);
	int HandleReadonlySave(CString& strSavePath, BOOL bMultiFile, BOOL &bApplyToAll);
	CString SetStatus(LPCTSTR status);
	void ApplyViewWhitespace();
	BOOL OpenFileToExternalEditor(CString file);
	CString GetDefaultEditor();
	void SetEOLMixed(BOOL bAllow);
	void SelectFilter();
	void ShowVSSError(CException *e, CString strItem);
	void ShowHelp(LPCTSTR helpLocation = NULL);
	void UpdateCodepageModule();
	void GetDirViews(DirViewList * pDirViews);
	void GetMergeEditViews(MergeEditViewList * pMergeViews);
	void CheckinToClearCase(CString strDestinationPath);
	COptionsMgr * GetTheOptionsMgr() { return &m_options; }
	void ResetOptions() { OptionsInit(); }
	static void CenterToMainFrame(CDialog * dlg);
	static void SetMainIcon(CDialog * dlg);
	void StartFlashing();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation methods
protected:
	virtual ~CMainFrame();
// Implementation in SourceControl.cpp
	void InitializeSourceControlMembers();
	BOOL SaveToVersionControl(CString& strSavePath);
// End SourceControl.cpp


// Public implementation data
public:
	BOOL m_bFirstTime; /**< If first time frame activated, get  pos from reg */
	CString m_strSaveAsPath; /**< "3rd path" where output saved if given */
	BOOL m_bEscShutdown; /**< If commandline switch -e given ESC closes appliction */
	VSSHelper m_vssHelper; /**< Helper class for VSS integration */
	SyntaxColors * GetMainSyntaxColors() { return m_pSyntaxColors; }
	BOOL m_bClearCaseTool; /**< WinMerge is executed as an external Rational ClearCase compare/merge tool. */
	BOOL m_bFlashing; /**< Window is flashing. */

	/**
	 * @name Version Control System (VCS) integration.
	 */
	/*@{*/ 
protected:
	CString m_strVssUser; /**< Visual Source Safe User ID */
	CString m_strVssPassword; /**< Visual Source Safe Password */
	CString m_strVssDatabase; /**< Visual Source Safe database */
	CString m_strCCComment; /**< ClearCase comment */
public:
	BOOL m_bCheckinVCS;     /**< TRUE if files should be checked in after checkout */
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
protected:


	// control bar embedded members
	CStatusBar  m_wndStatusBar;
	ToolBarXPThemes m_wndToolBar;

	enum
	{
		MENU_DEFAULT,
		MENU_MERGEVIEW,
		MENU_DIRVIEW,
		MENU_COUNT, // Add new items before this item
	};
	BCMenu * m_pMenus[MENU_COUNT]; /**< Menus for different views */

	CRegOptionsMgr m_options; /**< Options manager */
	SyntaxColors *m_pSyntaxColors; /**< Syntax color container */

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
	afx_msg void OnHelpIndex();
	afx_msg void OnUpdateHelpIndex(CCmdUI* pCmdUI);
	afx_msg void OnHelpSearch();
	afx_msg void OnUpdateHelpSearch(CCmdUI* pCmdUI);
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
	afx_msg void OnDebugLoadConfig();
	afx_msg void OnHelpMerge7zmismatch();
	afx_msg void OnUpdateHelpMerge7zmismatch(CCmdUI* pCmdUI);
	afx_msg void OnViewStatusBar();
	afx_msg void OnViewToolbar();
	afx_msg void OnFileOpenproject();
	afx_msg LRESULT OnCopyData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUser(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnWindowCloseAll();
	afx_msg void OnUpdateWindowCloseAll(CCmdUI* pCmdUI);
	afx_msg void OnSaveProject();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
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
	void OptionsInit();
	void OpenFileOrUrl(LPCTSTR szFile, LPCTSTR szUrl);
	CMergeEditView * GetActiveMergeEditView();
};

CMainFrame * GetMainFrame(); // access to the singleton main frame object
SyntaxColors * GetMainSyntaxColors(); // access to the singleton set of syntax colors

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__BBCD4F8C_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)

