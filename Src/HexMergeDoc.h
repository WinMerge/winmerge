/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  HexMergeDoc.h
 *
 * @brief Declaration of CHexMergeDoc class
 */
#pragma once

#include "PathContext.h"
#include "FileLocation.h"
#include "IMergeDoc.h"
#include "FileTransform.h"

class CDirDoc;
class CHexMergeFrame;
class CHexMergeView;

/**
 * @brief Document class for bytewise merging two files presented as hexdumps
 */
class CHexMergeDoc : public CDocument, public IMergeDoc
{
public:
	enum class BUFFERTYPE
	{
		NORMAL = 0, /**< Normal, file loaded from disk */
		NORMAL_NAMED, /**< Normal, description given */
		UNNAMED, /**< Empty, created buffer */
		UNNAMED_SAVED, /**< Empty buffer saved with filename */
	};

// Attributes
public:
	static int m_nBuffersTemp;
	int m_nBuffers;
	PathContext m_filePaths; /**< Filepaths for this document */

// Begin declaration of CHexMergeDoc

protected: // create from serialization only
	CHexMergeDoc();
	DECLARE_DYNCREATE(CHexMergeDoc)

	
	// Operations
public:	
	void SetMergeViews(CHexMergeView * pView[]);
	CHexMergeView *GetActiveMergeView() const;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDoc)
	public:
	virtual BOOL SaveModified();
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CHexMergeDoc();
	int UpdateDiffItem(CDirDoc * pDirDoc);
	bool PromptAndSaveIfNeeded(bool bAllowCancel);
	CDirDoc* GetDirDoc() const override { return m_pDirDoc; };
	void SetDirDoc(CDirDoc * pDirDoc) override;
	void DirDocClosing(CDirDoc * pDirDoc) override;
	bool CloseNow() override;
	bool GenerateReport(const String& sFileName) const override { return true; }
	const PackingInfo* GetUnpacker() const override { return &m_infoUnpacker; };
	PackingInfo* GetUnpacker() { return &m_infoUnpacker; };
	void SetUnpacker(const PackingInfo* infoUnpacker) override { if (infoUnpacker) m_infoUnpacker = *infoUnpacker;  };
	const PrediffingInfo* GetPrediffer() const override { return nullptr; };
	int GetFileCount() const override { return m_filePaths.GetSize(); }
	String GetPath(int pane) const override { return m_filePaths[pane]; }
	bool GetReadOnly(int pane) const override;
	CString GetTooltipString() const override;
	CHexMergeFrame * GetParentFrame() const;
	void UpdateHeaderPath(int pane);
	void RefreshOptions();
	bool OpenDocs(int nFiles, const FileLocation fileloc[], const bool bRO[], const String strDesc[]);
	void MoveOnLoad(int nPane = -1, int nLineIndex = -1);
	void ChangeFile(int nBuffer, const String& path, int nLineIndex = -1);
	void CheckFileChanged(void) override;
	String GetDescription(int pane) const override { return m_strDesc[pane]; };
	void SetDescription(int pane, const String& strDesc) {  m_strDesc[pane] = strDesc; };
	void SaveAs(int nBuffer, bool packing = true) { DoFileSaveAs(nBuffer, packing); }
private:
	bool DoFileSave(int nBuffer);
	bool DoFileSaveAs(int nBuffer, bool packing = true);
	HRESULT LoadOneFile(int index, const tchar_t* filename, bool readOnly, const String& strDesc);
	void RecompareAs(UINT id);
// Implementation data
protected:
	CHexMergeView * m_pView[3]; /**< Pointer to left/right view */
	CDirDoc * m_pDirDoc;
	String m_strDesc[3]; /**< Left/right side description text */
	BUFFERTYPE m_nBufferType[3];
	PackingInfo m_infoUnpacker;

// Generated message map functions
protected:
	//{{AFX_MSG(CMergeDoc)
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveLeft();
	afx_msg void OnFileSaveMiddle();
	afx_msg void OnFileSaveRight();
	afx_msg void OnFileSaveAsLeft();
	afx_msg void OnFileSaveAsMiddle();
	afx_msg void OnFileSaveAsRight();
	afx_msg void OnFileReload();
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveMiddle(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnCopyX2Y(UINT nID);
	afx_msg void OnUpdateX2Y(CCmdUI* pCmdUI);
	afx_msg void OnL2r();
	afx_msg void OnUpdateL2r(CCmdUI* pCmdUI);
	afx_msg void OnR2l();
	afx_msg void OnUpdateR2l(CCmdUI* pCmdUI);
	afx_msg void OnUpdateL2RNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateR2LNext(CCmdUI* pCmdUI);
	afx_msg void OnCopyFromLeft();
	afx_msg void OnUpdateCopyFromLeft(CCmdUI* pCmdUI);
	afx_msg void OnCopyFromRight();
	afx_msg void OnUpdateCopyFromRight(CCmdUI* pCmdUI);
	afx_msg void OnAllRight();
	afx_msg void OnUpdateAllRight(CCmdUI* pCmdUI);
	afx_msg void OnAllLeft();
	afx_msg void OnUpdateAllLeft(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewZoomNormal();
	afx_msg void OnRefresh();
	afx_msg void OnFileRecompareAs(UINT nID);
	afx_msg void OnUpdateFileRecompareAs(CCmdUI* pCmdUI);
	afx_msg void OnOpenWithUnpacker();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

