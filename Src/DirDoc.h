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
#if !defined(AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_)
#define AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DirDoc.h : header file
//
#include "DirView.h"
#include "DiffContext.h"
class CMergeDoc;

/////////////////////////////////////////////////////////////////////////////
// CDirDoc document

class CDirDoc : public CDocument
{
protected:
	CDirDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDirDoc)

// Attributes
public:
// Operations
public:
	CDirView * GetMainView();
	CMergeDoc * GetMergeDoc() { return m_pMergeDoc; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL UpdateItemTimes(LPCTSTR pathLeft, LPCTSTR pathRight);
	BOOL UpdateItemStatus( LPCTSTR pathLeft, LPCTSTR pathRight, UINT status );
	void SetDiffContext(CDiffContext *pCtxt);
	void UpdateResources();
	void InitStatusStrings();
	void UpdateItemStatus(UINT nIdx);
	void SetItemStatus(UINT nIdx, LPCTSTR szStatus, int image, const time_t * ltime, const time_t * rtime);
	void Redisplay();
	void Rescan();
	CDiffContext *m_pCtxt;
	virtual ~CDirDoc();
	void SetDirView( CDirView *newView ); // TODO Perry
	void SetMergeDoc(CMergeDoc * pMergeDoc);
	void ClearMergeDoc(CMergeDoc * pMergeDoc);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void UpdateItemStatus(UINT nIdx, DIFFITEM di);

	// Generated message map functions
	//{{AFX_MSG(CDirDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Implementation data
private:
	CDirView *m_pDirView;
	CMergeDoc *m_pMergeDoc;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_)
