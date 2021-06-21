/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ImgMergeFrm.h
 *
 * @brief interface of the CImgMergeFrame class
 *
 */
#pragma once

#include "EditorFilepathBar.h"
#include "PathContext.h"
#include "DiffFileInfo.h"
#include "IMergeDoc.h"
#include "WinIMergeLib.h"
#include "LocationBar.h"
#include "FileLocation.h"
#include "MergeFrameCommon.h"
#include "FileTransform.h"

class CDirDoc;

/** 
 * @brief Frame class for file compare, handles panes, statusbar etc.
 */
class CImgMergeFrame : public CMergeFrameCommon,public IMergeDoc
{
	private:
	enum class BUFFERTYPE
	{
		NORMAL = 0, /**< Normal, file loaded from disk */
		NORMAL_NAMED, /**< Normal, description given */
		UNNAMED, /**< Empty, created buffer */
		UNNAMED_SAVED, /**< Empty buffer saved with filename */
	};

	using CMDIChildWnd::Create;

	DECLARE_DYNCREATE(CImgMergeFrame)
public:
	CImgMergeFrame();


// Operations
public:
	bool OpenDocs(int nFiles, const FileLocation fileloc[], const bool bRO[], const String strDesc[], CMDIFrameWnd *pParent);
	void MoveOnLoad(int nPane = -1, int nLineIndex = -1);
	void ChangeFile(int pane, const String& path);
	CDirDoc* GetDirDoc() const override { return m_pDirDoc; };
	void SetDirDoc(CDirDoc * pDirDoc) override;
	void UpdateResources();
	bool CloseNow() override;
	void DirDocClosing(CDirDoc * pDirDoc) override { m_pDirDoc = nullptr; }
	void UpdateLastCompareResult();
	void UpdateAutoPaneResize();
	void UpdateSplitter();
	bool GenerateReport(const String& sFileName) const override;
	const PackingInfo* GetUnpacker() const override { return &m_infoUnpacker; };
	void SetUnpacker(const PackingInfo* infoUnpacker) override { if (infoUnpacker) m_infoUnpacker = *infoUnpacker; };
	const PrediffingInfo* GetPrediffer() const override { return nullptr; };
	int GetFileCount() const override { return m_filePaths.GetSize(); }
	String GetPath(int pane) const override { return m_filePaths[pane]; }
	bool GetReadOnly(int pane) const override { return m_bRO[pane]; }
	void DoAutoMerge(int dstPane);
	bool IsModified() const;
	IMergeDoc::FileChange IsFileChangedOnDisk(int pane) const;
	void CheckFileChanged(void) override;
	String GetDescription(int pane) const { return m_strDesc[pane]; }
	static bool IsLoadable();

// Attributes
protected:
	CEditorFilePathBar m_wndFilePathBar;
	CStatusBar m_wndStatusBar[3];
// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImgMergeFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG *);
	static CMenu menu;

	protected:
	//}}AFX_VIRTUAL

// Implementation
private:
	bool EnsureValidDockState(CDockState& state);
	void LoadOptions();
	void SaveOptions();
	void SavePosition();
	void SaveActivePane();
	virtual ~CImgMergeFrame();
	void CreateImgWndStatusBar(CStatusBar &, CWnd *);
// Generated message map functions
private:
	bool OpenImages();
	int UpdateDiffItem(CDirDoc * pDirDoc);
	void UpdateHeaderSizes();
	void UpdateHeaderPath(int pane);
	void SetTitle(LPCTSTR lpszTitle);
	bool DoFileSave(int pane);
	bool DoFileSaveAs(int pane, bool packing = true);
	bool PromptAndSaveIfNeeded(bool bAllowCancel);
	bool MergeModeKeyDown(MSG* pMsg);
	static void OnChildPaneEvent(const IImgMergeWindow::Event& evt);
	void OnDropFiles(int pane, const std::vector<String>& files);
	static void TranslateLocationPane(int id, const wchar_t *org, size_t dstbufsize, wchar_t *dst);
	CLocationBar m_wndLocationBar;
	IImgMergeWindow *m_pImgMergeWindow;
	IImgToolWindow *m_pImgToolWindow;
	PathContext m_filePaths;
	String m_strDesc[3];
	BUFFERTYPE m_nBufferType[3];
	DiffFileInfo m_fileInfo[3];
	bool m_bRO[3];
	bool m_bAutoMerged;
	CDirDoc *m_pDirDoc;
	int m_nActivePane;
	PackingInfo m_infoUnpacker;
	std::vector<int> m_unpackerSubcodes[3];

	//{{AFX_MSG(CImgMergeFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveLeft();
	afx_msg void OnFileSaveMiddle();
	afx_msg void OnUpdateFileSaveMiddle(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveRight();
	afx_msg void OnFileSaveAsLeft();
	afx_msg void OnFileSaveAsMiddle();
	afx_msg void OnUpdateFileSaveAsMiddle(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveAsRight();
	afx_msg void OnLeftReadOnly();
	afx_msg void OnUpdateLeftReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnMiddleReadOnly();
	afx_msg void OnUpdateMiddleReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnRightReadOnly();
	afx_msg void OnUpdateRightReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnFileReload();
	afx_msg void OnFileClose();
	afx_msg void OnFileRecompareAs(UINT nID);
	afx_msg void OnUpdateFileRecompareAs(CCmdUI* pCmdUI);
	afx_msg void OnOpenWithUnpacker();
	afx_msg void OnWindowChangePane();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnIdleUpdateCmdUI();
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	afx_msg LRESULT OnStorePaneSizes(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewZoomNormal();
	afx_msg void OnViewSplitVertically();
	afx_msg void OnUpdateViewSplitVertically(CCmdUI* pCmdUI);
	afx_msg void OnFirstdiff();
	afx_msg void OnUpdateFirstdiff(CCmdUI* pCmdUI);
	afx_msg void OnLastdiff();
	afx_msg void OnUpdateLastdiff(CCmdUI* pCmdUI);
	afx_msg void OnNextdiff();
	afx_msg void OnUpdateNextdiff(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiff();
	afx_msg void OnUpdatePrevdiff(CCmdUI* pCmdUI);
	afx_msg void OnNextConflict();
	afx_msg void OnUpdateNextConflict(CCmdUI* pCmdUI);
	afx_msg void OnPrevConflict();
	afx_msg void OnUpdatePrevConflict(CCmdUI* pCmdUI);
	afx_msg void OnX2Y(int srcPane, int dstPane);
	afx_msg void OnUpdateX2Y(CCmdUI* pCmdUI, int srcPane, int dstPane);
	afx_msg void OnL2r();
	afx_msg void OnUpdateL2r(CCmdUI* pCmdUI);
	afx_msg void OnR2l();
	afx_msg void OnUpdateR2l(CCmdUI* pCmdUI);
	afx_msg void OnCopyFromLeft();
	afx_msg void OnUpdateCopyFromLeft(CCmdUI* pCmdUI);
	afx_msg void OnCopyFromRight();
	afx_msg void OnUpdateCopyFromRight(CCmdUI* pCmdUI);
	afx_msg void OnAllLeft();
	afx_msg void OnUpdateAllLeft(CCmdUI* pCmdUI);
	afx_msg void OnAllRight();
	afx_msg void OnUpdateAllRight(CCmdUI* pCmdUI);
	afx_msg void OnAutoMerge();
	afx_msg void OnUpdateAutoMerge(CCmdUI* pCmdUI);
	afx_msg void OnImgViewDifferences();
	afx_msg void OnUpdateImgViewDifferences(CCmdUI* pCmdUI);
	afx_msg void OnImgZoom(UINT nId);
	afx_msg void OnUpdateImgZoom(CCmdUI* pCmdUI);
	afx_msg void OnImgOverlayMode(UINT nId);
	afx_msg void OnUpdateImgOverlayMode(CCmdUI* pCmdUI);
	afx_msg void OnImgDraggingMode(UINT nId);
	afx_msg void OnUpdateImgDraggingMode(CCmdUI* pCmdUI);
	afx_msg void OnImgDiffBlockSize(UINT nId);
	afx_msg void OnUpdateImgDiffBlockSize(CCmdUI* pCmdUI);
	afx_msg void OnImgThreshold(UINT nId);
	afx_msg void OnUpdateImgThreshold(CCmdUI* pCmdUI);
	afx_msg void OnImgInsertionDeletionDetectionMode(UINT nId);
	afx_msg void OnUpdateImgInsertionDeletionDetectionMode(CCmdUI* pCmdUI);
	afx_msg void OnImgPrevPage();
	afx_msg void OnUpdateImgPrevPage(CCmdUI* pCmdUI);
	afx_msg void OnImgNextPage();
	afx_msg void OnUpdateImgNextPage(CCmdUI* pCmdUI);
	afx_msg void OnImgCurPanePrevPage();
	afx_msg void OnUpdateImgCurPanePrevPage(CCmdUI* pCmdUI);
	afx_msg void OnImgCurPaneNextPage();
	afx_msg void OnUpdateImgCurPaneNextPage(CCmdUI* pCmdUI);
	afx_msg void OnImgUseBackColor();
	afx_msg void OnImgVectorImageScaling(UINT nId);
	afx_msg void OnUpdateImgVectorImageScaling(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImgUseBackColor(CCmdUI* pCmdUI);
	afx_msg void OnImgCompareExtractedText();
	afx_msg void OnToolsGenerateReport();
	afx_msg void OnRefresh();
	afx_msg void OnSetFocus(CWnd *pNewWnd);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
