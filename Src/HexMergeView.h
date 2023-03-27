/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  HexMergeView.h
 *
 * @brief Declaration of CHexMergeView class
 */
#pragma once

#include "DiffFileInfo.h"
#include "IMergeDoc.h"

class CHexMergeDoc;
class IHexEditorWindow;

/**
 * @brief Document class for bytewise merging two files presented as hexdumps
 */
class CHexMergeView : public CView
{
// Attributes
protected:
	IHexEditorWindow *m_pif;
public:
	int m_nThisPane;
	DiffFileInfo m_fileInfo;
protected: // create from serialization only
	CHexMergeView();
	DECLARE_DYNCREATE(CHexMergeView)
	std::vector<int> m_unpackerSubcodes;
public:
	HRESULT LoadFile(const tchar_t*);
	HRESULT SaveFile(const tchar_t*, bool packing = true);
	IHexEditorWindow *GetInterface() const { return m_pif; }
	BYTE *GetBuffer(size_t);
	size_t GetLength();
	bool GetModified();
	void SetSavePoint();
	void ClearUndoRecords();
	bool GetReadOnly();
	void SetReadOnly(bool);
	void ResizeWindow();
	IMergeDoc::FileChange IsFileChangedOnDisk(const tchar_t*);
	void ZoomText(int amount);
	static void CopySel(const CHexMergeView *src, CHexMergeView *dst);
	static void CopyAll(const CHexMergeView *src, CHexMergeView *dst);
	static bool IsLoadable();
	// Overrides
protected:
	virtual void OnDraw(CDC*);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual BOOL PreTranslateMessage(MSG *);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHexMergeView)
	public:
	//}}AFX_VIRTUAL

// Operations

// Implementation data

// Generated message map functions
protected:
	//{{AFX_MSG(CHexMergeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnEditFind();
	afx_msg void OnEditReplace();
	afx_msg void OnEditRepeat();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditClear();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditGoto();
	afx_msg void OnFirstdiff();
	afx_msg void OnLastdiff();
	afx_msg void OnNextdiff();
	afx_msg void OnPrevdiff();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
