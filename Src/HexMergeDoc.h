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
 * @file  HexMergeDoc.h
 *
 * @brief Declaration of CHexMergeDoc class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "TempFile.h"
#include "PathContext.h"
#include "DiffFileInfo.h"

class CDirDoc;
class CHexMergeFrame;
class CHexMergeView;

/**
 * @brief Document class for bytewise merging two files presented as hexdumps
 */
class CHexMergeDoc : public CDocument
{
// Attributes
public:
	PathContext m_filePaths; /**< Filepaths for this document */

// Begin declaration of CHexMergeDoc

protected: // create from serialization only
	CHexMergeDoc();
	DECLARE_DYNCREATE(CHexMergeDoc)

	
	// Operations
public:	
	void SetMergeViews(CHexMergeView * pLeft, CHexMergeView * pRight);

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
	void UpdateDiffItem(CDirDoc * pDirDoc);
	BOOL PromptAndSaveIfNeeded(BOOL bAllowCancel);
	void SetDirDoc(CDirDoc * pDirDoc);
	void DirDocClosing(CDirDoc * pDirDoc);
	BOOL CloseNow();
	CHexMergeFrame * GetParentFrame();
	void UpdateHeaderPath(int pane);
	HRESULT OpenDocs(LPCTSTR pathLeft, LPCTSTR pathRight, BOOL bROLeft, BOOL bRORight);
protected:
	static void CopySel(CHexMergeView *pViewSrc, CHexMergeView *pViewDst);
	static void CopyAll(CHexMergeView *pViewSrc, CHexMergeView *pViewDst);
// Implementation data
protected:
	CHexMergeView * m_pView[MERGE_VIEW_COUNT]; /**< Pointer to left/right view */
	CDirDoc * m_pDirDoc;
	TempFile m_tempFiles[2]; /**< Temp files for compared files */
	String m_strDesc[2]; /**< Left/right side description text */
	BUFFERTYPE m_nBufferType[2];

// Generated message map functions
protected:
	//{{AFX_MSG(CMergeDoc)
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveLeft();
	afx_msg void OnFileSaveRight();
	afx_msg void OnFileSaveAsLeft();
	afx_msg void OnFileSaveAsRight();
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnL2r();
	afx_msg void OnR2l();
	afx_msg void OnAllRight();
	afx_msg void OnAllLeft();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
