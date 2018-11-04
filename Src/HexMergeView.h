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
 * @file  HexMergeView.h
 *
 * @brief Declaration of CHexMergeView class
 */
#pragma once

#include "DiffFileInfo.h"

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
public:
	HRESULT LoadFile(LPCTSTR);
	HRESULT SaveFile(LPCTSTR);
	IHexEditorWindow *GetInterface() const { return m_pif; }
	BYTE *GetBuffer(int);
	int GetLength();
	bool GetModified();
	void SetSavePoint();
	void ClearUndoRecords();
	bool GetReadOnly();
	void SetReadOnly(bool);
	void ResizeWindow();
	bool IsFileChangedOnDisk(LPCTSTR);
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
	afx_msg void OnFirstdiff();
	afx_msg void OnLastdiff();
	afx_msg void OnNextdiff();
	afx_msg void OnPrevdiff();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
