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

class CDirDoc;
class CHexMergeFrame;
class CHexMergeView;

/**
 * @brief Document class for bytewise merging two files presented as hexdumps
 */
class CHexMergeDoc : public CDocument, public IMergeDoc
{
public:
	enum BUFFERTYPE
	{
		BUFFER_NORMAL = 0, /**< Normal, file loaded from disk */
		BUFFER_NORMAL_NAMED, /**< Normal, description given */
		BUFFER_UNNAMED, /**< Empty, created buffer */
		BUFFER_UNNAMED_SAVED, /**< Empty buffer saved with filename */
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
	void SetDirDoc(CDirDoc * pDirDoc) override;
	void DirDocClosing(CDirDoc * pDirDoc) override;
	bool CloseNow() override;
	bool GenerateReport(const String& sFileName) const override { return true; }
	CHexMergeFrame * GetParentFrame() const;
	void UpdateHeaderPath(int pane);
	void RefreshOptions();
	bool OpenDocs(int nFiles, const FileLocation fileloc[], const bool bRO[], const String strDesc[]);
	void MoveOnLoad(int nPane = -1, int nLineIndex = -1);
	void CheckFileChanged(void) override;
	String GetDescription(int pane) const { return m_strDesc[pane]; };
private:
	void DoFileSave(int nBuffer);
	void DoFileSaveAs(int nBuffer);
	HRESULT LoadOneFile(int index, LPCTSTR filename, bool readOnly, const String& strDesc);
	void RecompareAs(UINT id);
// Implementation data
protected:
	CHexMergeView * m_pView[3]; /**< Pointer to left/right view */
	CDirDoc * m_pDirDoc;
	String m_strDesc[3]; /**< Left/right side description text */
	BUFFERTYPE m_nBufferType[3];

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
	afx_msg void OnL2r();
	afx_msg void OnR2l();
	afx_msg void OnCopyFromLeft();
	afx_msg void OnCopyFromRight();
	afx_msg void OnAllRight();
	afx_msg void OnAllLeft();
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewZoomNormal();
	afx_msg void OnRefresh();
	afx_msg void OnFileRecompareAs(UINT nID);
	afx_msg void OnUpdateFileRecompareAs(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

