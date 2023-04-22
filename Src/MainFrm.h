/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MainFrm.h
 *
 * @brief Declaration file for CMainFrame
 *
 */
#pragma once

#include <vector>
#include <memory>
#include <optional>
#include "MDITabBar.h"
#include "PathContext.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "FileOpenFlags.h"

class BCMenu;
class CDirView;
class COpenDoc;
class CDirDoc;
class CMergeDoc;
class CHexMergeDoc;
class CMergeEditView;
class SyntaxColors;
class LineFiltersList;
class TempFile;
struct FileLocation;
class DropHandler;
class CMainFrame;
class CImgMergeFrame;
class CWebPageDiffFrame;
class DirWatcher;

typedef std::shared_ptr<TempFile> TempFilePtr;

// typed lists (homogenous pointer lists)
typedef CTypedPtrList<CPtrList, COpenDoc *> OpenDocList;
typedef CTypedPtrList<CPtrList, CMergeDoc *> MergeDocList;
typedef CTypedPtrList<CPtrList, CDirDoc *> DirDocList;
typedef CTypedPtrList<CPtrList, CHexMergeDoc *> HexMergeDocList;

class PackingInfo;
class PrediffingInfo;
class CLanguageSelect;
struct IMergeDoc;

CMainFrame * GetMainFrame(); // access to the singleton main frame object

/**
 * @brief Frame class containing save-routines etc
 */
class CMainFrame : public CMDIFrameWnd
{
	friend CLanguageSelect;
	DECLARE_DYNAMIC(CMainFrame)
public:
	/**
	 * @brief Frame/View/Document types.
	 */
	enum FRAMETYPE
	{
		FRAME_FOLDER, /**< Folder compare frame. */
		FRAME_FILE, /**< File compare frame. */
		FRAME_HEXFILE, /**< Hex file compare frame. */
		FRAME_IMGFILE, /**< Image file compare frame. */
		FRAME_WEBPAGE, /**< Web page compare frame. */
		FRAME_OTHER, /**< No frame? */
	};

	struct OpenFileParams
	{
		virtual ~OpenFileParams() {}
	};

	struct OpenTextFileParams : public OpenFileParams
	{
		virtual ~OpenTextFileParams() {}
		int m_line = -1;
		int m_char = -1;
		String m_fileExt;
	};

	struct OpenTableFileParams : public OpenTextFileParams
	{
		virtual ~OpenTableFileParams() {}
		std::optional<tchar_t> m_tableDelimiter;
		std::optional<tchar_t> m_tableQuote;
		std::optional<bool> m_tableAllowNewlinesInQuotes;
	};

	struct OpenBinaryFileParams : public OpenFileParams
	{
		virtual ~OpenBinaryFileParams() {}
		int m_address = -1;
	};

	struct OpenImageFileParams : public OpenFileParams
	{
		virtual ~OpenImageFileParams() {}
		int m_x = -1;
		int m_y = -1;
	};

	struct OpenWebPageParams : public OpenFileParams
	{
		virtual ~OpenWebPageParams() {}
	};

	struct OpenAutoFileParams : public OpenTableFileParams, public OpenBinaryFileParams, public OpenImageFileParams
	{
		virtual ~OpenAutoFileParams() {}
	};

	struct OpenFolderParams : public OpenFileParams
	{
		virtual ~OpenFolderParams() {}
		std::vector<String> m_hiddenItems;
	};

	CMainFrame();

// Attributes
public:	
	bool m_bShowErrors; /**< Show folder compare error items? */
	LOGFONT m_lfDiff; /**< MergeView user-selected font */
	LOGFONT m_lfDir; /**< DirView user-selected font */
	static const tchar_t szClassName[];

// Operations
public:
	HMENU NewDirViewMenu();
	HMENU NewMergeViewMenu();
	HMENU NewHexMergeViewMenu();
	HMENU NewImgMergeViewMenu();
	HMENU NewWebPageDiffViewMenu();
	HMENU NewOpenViewMenu();
	HMENU NewDefaultMenu(int ID = 0);
	HMENU GetPrediffersSubmenu(HMENU mainMenu);
	void UpdatePrediffersMenu();

	bool DoFileOrFolderOpen(const PathContext *pFiles = nullptr,
		const fileopenflags_t dwFlags[] = nullptr, const String strDesc[] = nullptr,
		const String& sReportFile = _T(""), bool bRecurse = false, CDirDoc *pDirDoc = nullptr,
		const PackingInfo * infoUnpacker = nullptr, const PrediffingInfo * infoPrediffer = nullptr,
		UINT nID = 0, const OpenFileParams *pOpenParams = nullptr);
	bool DoFileOpen(UINT nID, const PathContext* pFiles,
		const fileopenflags_t dwFlags[] = nullptr, const String strDesc[] = nullptr,
		const String& sReportFile = _T(""),
		const PackingInfo* infoUnpacker = nullptr, const PrediffingInfo * infoPrediffer = nullptr,
		const OpenFileParams *pOpenParams = nullptr);
	bool DoFileNew(UINT nID, int nPanes,
		const fileopenflags_t dwFlags[] = nullptr, const String strDesc[] = nullptr,
		const PrediffingInfo * infoPrediffer = nullptr,
		const OpenFileParams *pOpenParams = nullptr);
	bool DoOpenConflict(const String& conflictFile, const String strDesc[] = nullptr, bool checked = false);
	bool DoOpenClipboard(UINT nID = 0, int nBuffers = 2, const fileopenflags_t dwFlags[] = nullptr, const String strDesc[] = nullptr,
		const PackingInfo* infoUnpacker = nullptr, const PrediffingInfo * infoPrediffer = nullptr,
		const OpenFileParams* pOpenParams = nullptr);
	bool DoSelfCompare(UINT nID, const String& file, const String strDesc[] = nullptr,
		const PackingInfo* infoUnpacker = nullptr, const PrediffingInfo * infoPrediffer = nullptr,
		const OpenFileParams* pOpenParams = nullptr);
	bool ShowAutoMergeDoc(UINT nID, CDirDoc * pDirDoc, int nFiles, const FileLocation fileloc[],
		const fileopenflags_t dwFlags[], const String strDesc[], const String& sReportFile = _T(""),
		const PackingInfo * infoUnpacker = nullptr, const OpenFileParams *pOpenParams = nullptr);
	bool ShowMergeDoc(UINT nID, CDirDoc * pDirDoc, int nFiles, const FileLocation fileloc[],
		const fileopenflags_t dwFlags[], const String strDesc[], const String& sReportFile = _T(""),
		const PackingInfo * infoUnpacker = nullptr, const OpenFileParams *pOpenParams = nullptr);
	bool ShowTextOrTableMergeDoc(std::optional<bool> table, CDirDoc * pDirDoc, int nFiles, const FileLocation fileloc[],
		const fileopenflags_t dwFlags[], const String strDesc[], const String& sReportFile = _T(""),
		const PackingInfo * infoUnpacker = nullptr, const OpenTextFileParams *pOpenParams = nullptr);
	bool ShowTextMergeDoc(CDirDoc * pDirDoc, int nFiles, const FileLocation fileloc[],
		const fileopenflags_t dwFlags[], const String strDesc[], const String& sReportFile = _T(""),
		const PackingInfo * infoUnpacker = nullptr, const OpenTextFileParams *pOpenParams = nullptr);
	bool ShowTextMergeDoc(CDirDoc* pDirDoc, int nBuffers, const String text[],
		const String strDesc[], const String& strFileExt, const OpenTextFileParams *pOpenParams = nullptr);
	bool ShowTableMergeDoc(CDirDoc * pDirDoc, int nFiles, const FileLocation fileloc[],
		const fileopenflags_t dwFlags[], const String strDesc[], const String& sReportFile = _T(""),
		const PackingInfo * infoUnpacker = nullptr, const OpenTextFileParams *pOpenParams = nullptr);
	bool ShowHexMergeDoc(CDirDoc * pDirDoc, int nFiles, const FileLocation fileloc[],
		const fileopenflags_t dwFlags[], const String strDesc[], const String& sReportFile = _T(""),
		const PackingInfo * infoUnpacker = nullptr, const OpenBinaryFileParams *pOpenParams = nullptr);
	bool ShowImgMergeDoc(CDirDoc * pDirDoc, int nFiles, const FileLocation fileloc[],
		const fileopenflags_t dwFlags[], const String strDesc[], const String& sReportFile = _T(""),
		const PackingInfo * infoUnpacker = nullptr, const OpenImageFileParams *pOpenParams = nullptr);
	bool ShowWebDiffDoc(CDirDoc * pDirDoc, int nFiles, const FileLocation fileloc[],
		const fileopenflags_t dwFlags[], const String strDesc[], const String& sReportFile = _T(""),
		const PackingInfo * infoUnpacker = nullptr, const OpenWebPageParams *pOpenParams = nullptr);

	void UpdateResources();
	void ApplyDiffOptions();
	void SelectFilter();
	void StartFlashing();
	bool AskCloseConfirmation();
	static FRAMETYPE GetFrameType(const CFrameWnd * pFrame);
	static void UpdateDocTitle();
	static void ReloadMenu();
	static void AppendPluginMenus(CMenu* pMenu, const String& filteredFilenames,
		const std::vector<std::wstring>& events, bool addAllMenu, unsigned baseId);
	static String GetPluginPipelineByMenuId(unsigned idSearch, const std::vector<std::wstring>& events, unsigned baseId);
	DropHandler *GetDropHandler() const { return m_pDropHandler; }
	const CTypedPtrArray<CPtrArray, CMDIChildWnd*>* GetChildArray() const { return &m_arrChild; }
	IMergeDoc* GetActiveIMergeDoc();
	DirWatcher* GetDirWatcher() { return m_pDirWatcher.get(); }
	void WatchDocuments(IMergeDoc* pMergeDoc);
	void UnwatchDocuments(IMergeDoc* pMergeDoc);
	CToolBar* GetToolbar() { return &m_wndToolBar; }

// Overrides
	virtual void GetMessageString(UINT nID, CString& rMessage) const;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
public:
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation methods
protected:
	virtual ~CMainFrame();

// Public implementation data
public:
	bool m_bFirstTime; /**< If first time frame activated, get  pos from reg */

// Implementation data
protected:
	// control bar embedded members
	CStatusBar  m_wndStatusBar;
	CReBar m_wndReBar;
	CToolBar m_wndToolBar;
	CMDITabBar m_wndTabBar;
	CTypedPtrArray<CPtrArray, CMDIChildWnd*> m_arrChild;

	// Tweak MDI client window behavior
	class CMDIClient : public CWnd
	{
		static UINT_PTR const m_nRedrawTimer = 1612;
		virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_MDICREATE:
			case WM_MDIACTIVATE:
			{
				// To reduce flicker in maximized state, disable drawing while messing with MDI child frames
				BOOL bMaximized;
				HWND hwndActive = reinterpret_cast<HWND>(SendMessage(WM_MDIGETACTIVE, 0, reinterpret_cast<LPARAM>(&bMaximized)));
				if ((bMaximized || (message == WM_MDICREATE && !hwndActive)) &&
					SetTimer(m_nRedrawTimer, USER_TIMER_MINIMUM, nullptr))
				{
					SetRedraw(FALSE);
				}
				break;
			}
			case WM_TIMER:
				if (wParam == m_nRedrawTimer)
				{
					KillTimer(m_nRedrawTimer);
					SetRedraw(TRUE);
					RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE);
				}
				break;
			}
			return CWnd::WindowProc(message, wParam, lParam);
		}
	} m_wndMDIClient;

	enum
	{
		MENU_DEFAULT,
		MENU_MERGEVIEW,
		MENU_DIRVIEW,
		MENU_HEXMERGEVIEW,
		MENU_IMGMERGEVIEW,
		MENU_WEBPAGEDIFFVIEW,
		MENU_OPENVIEW,
		MENU_COUNT, // Add new items before this item
	};
	/**
	 * Menu frames - for which frame(s) the menu is.
	 */
	enum
	{
		MENU_MAINFRM = 0x000001,
		MENU_FILECMP = 0x000002,
		MENU_FOLDERCMP = 0x000004,
		MENU_ALL = MENU_MAINFRM | MENU_FILECMP | MENU_FOLDERCMP
	};
	enum
	{
		AUTO_RELOAD_MODIFIED_FILES_DISABLED,
		AUTO_RELOAD_MODIFIED_FILES_ONWINDOWACTIVATED,
		AUTO_RELOAD_MODIFIED_FILES_IMMEDIATELY
	};

	/**
	 * A structure attaching a menu item, icon and menu types to apply to.
	 */
	struct MENUITEM_ICON
	{
		int menuitemID;   /**< Menu item's ID. */
		int iconResID;    /**< Icon's resource ID. */
		int menusToApply; /**< For which menus to apply. */
	};

	static const MENUITEM_ICON m_MenuIcons[];

	std::unique_ptr<BCMenu> m_pMenus[MENU_COUNT]; /**< Menus for different views */
	std::unique_ptr<BCMenu> m_pImageMenu;
	std::unique_ptr<BCMenu> m_pWebPageMenu;
	std::vector<TempFilePtr> m_tempFiles; /**< List of possibly needed temp files. */
	DropHandler *m_pDropHandler;
	std::unique_ptr<DirWatcher> m_pDirWatcher;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) ;
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpen();
	afx_msg void OnHelpGnulicense();
	afx_msg void OnOptions();
	afx_msg void OnViewSelectfont();
	afx_msg void OnViewUsedefaultfont();
	afx_msg void OnHelpContents();
	afx_msg void OnClose();
	afx_msg void OnToolsGeneratePatch();
	afx_msg void OnDropFiles(const std::vector<String>& files);
	afx_msg void OnUpdatePluginUnpackMode(CCmdUI* pCmdUI);
	afx_msg void OnPluginUnpackMode(UINT nID);
	afx_msg void OnUpdatePluginPrediffMode(CCmdUI* pCmdUI);
	afx_msg void OnPluginPrediffMode(UINT nID);
	afx_msg void OnUpdatePluginRelatedMenu(CCmdUI* pCmdUI);
	afx_msg void OnReloadPlugins();
	afx_msg void OnSaveConfigData();
	template <int nFiles, unsigned nID>
	afx_msg void OnFileNew() { DoFileNew(nID, nFiles); }
	afx_msg void OnToolsFilters();
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdateViewTabBar(CCmdUI* pCmdUI);
	afx_msg void OnViewTabBar();
	afx_msg void OnUpdateResizePanes(CCmdUI* pCmdUI);
	afx_msg void OnResizePanes();
	afx_msg void OnFileOpenProject();
	afx_msg LRESULT OnCopyData(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUser1(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWindowCloseAll();
	afx_msg void OnUpdateWindowCloseAll(CCmdUI* pCmdUI);
	afx_msg void OnSaveProject();
#if _MFC_VER > 0x0600
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif
	afx_msg void OnToolbarSize(UINT id);
	afx_msg void OnUpdateToolbarSize(CCmdUI* pCmdUI);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHelpReleasenotes();
	afx_msg void OnHelpTranslations();
	afx_msg void OnFileOpenConflict();
	afx_msg void OnFileOpenClipboard();
	afx_msg void OnPluginsList();
	afx_msg void OnUpdatePluginName(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	afx_msg void OnToolbarButtonDropDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDiffWhitespace(UINT nID);
	afx_msg void OnUpdateDiffWhitespace(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreBlankLines();
	afx_msg void OnUpdateDiffIgnoreBlankLines(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreCase();
	afx_msg void OnUpdateDiffIgnoreCase(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreNumbers();
	afx_msg void OnUpdateDiffIgnoreNumbers(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreEOL();
	afx_msg void OnUpdateDiffIgnoreEOL(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreCP();
	afx_msg void OnUpdateDiffIgnoreCP(CCmdUI* pCmdUI);
	afx_msg void OnDiffIgnoreComments();
	afx_msg void OnUpdateDiffIgnoreComments(CCmdUI* pCmdUI);
	afx_msg void OnIncludeSubfolders();
	afx_msg void OnUpdateIncludeSubfolders(CCmdUI* pCmdUI);
	afx_msg void OnCompareMethod(UINT nID);
	afx_msg void OnUpdateCompareMethod(CCmdUI* pCmdUI);
	afx_msg void OnMRUs(UINT nID);
	afx_msg void OnUpdateNoMRUs(CCmdUI* pCmdUI);
	afx_msg void OnFirstFile();
	afx_msg void OnUpdateFirstFile(CCmdUI* pCmdUI);
	afx_msg void OnPrevFile();
	afx_msg void OnUpdatePrevFile(CCmdUI* pCmdUI);
	afx_msg void OnNextFile();
	afx_msg void OnUpdateNextFile(CCmdUI* pCmdUI);
	afx_msg void OnLastFile();
	afx_msg void OnUpdateLastFile(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnAccelQuit();
	afx_msg LRESULT OnChildFrameAdded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChildFrameRemoved(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChildFrameActivate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnChildFrameActivated(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void addToMru(const tchar_t* szItem, const tchar_t* szRegSubKey, UINT nMaxItems = 20);
	OpenDocList &GetAllOpenDocs();
	MergeDocList &GetAllMergeDocs();
	DirDocList &GetAllDirDocs();
	HexMergeDocList &GetAllHexMergeDocs();
	std::vector<CImgMergeFrame *> GetAllImgMergeFrames();
	std::vector<CWebPageDiffFrame *> GetAllWebPageDiffFrames();
	void UpdateFont(FRAMETYPE frame);
	BOOL CreateToolbar();
	CMergeEditView * GetActiveMergeEditView();
	void LoadToolbarImages();
	HMENU NewMenu( int view, int ID );
};
