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
// MergeDoc.h : interface of the CMergeDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "afxtempl.h"

#define OP_NONE			0
#define OP_LEFTONLY		1
#define OP_DIFF			2
#define OP_RIGHTONLY	3

typedef struct tagDIFFRANGE {
	UINT begin0,end0,begin1,end1;
	UINT dbegin0,dend0,dbegin1,dend1;
	int blank0,blank1;
	BYTE op;
}DIFFRANGE;

class CDiffView;

class CUndoItem
{
public:
	UINT begin,end,diffidx;
	int blank;
	BOOL bInsert;
	CDiffView *m_pList;
	CStringList list;
};


class CMergeDoc : public CDocument
{
protected: // create from serialization only
	CMergeDoc();
	DECLARE_DYNCREATE(CMergeDoc)

// Attributes
public:
	
	// Operations
public:	
	CDiffView * m_pView;
	CPtrList m_undoList;
	CArray<DIFFRANGE,DIFFRANGE> m_diffs;
	UINT m_nDiffs;
	CString m_strLeftFile, m_strRightFile;

	BOOL Rescan();
	void AddDiffRange(UINT begin0, UINT end0, UINT begin1, UINT end1, BYTE op);
	void AddUndoAction(UINT nBegin, UINT nEnd, UINT nDiff, int nBlanks, BOOL bInsert, CDiffView *pList);
	BOOL Undo();
	void ListCopy(CDiffView * pSrcList, CDiffView * pDestList);
	BOOL DoSave(LPCTSTR szPath, CListCtrl * pList, BOOL bLeft);
	//CString ExpandTabs(LPCTSTR szText);
	//CString Tabify(LPCTSTR szText);
	int LineToDiff(UINT nLine);
	void SetDiffViewMode(BOOL bEnable);
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

// Implementation
public:
	UINT CountPrevBlanks(UINT nCurLine, BOOL bLeft);
	virtual ~CMergeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMergeDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
