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
#if !defined(AFX_DirView_H__16E7C721_351C_11D1_95CD_444553540000__INCLUDED_)
#define AFX_DirView_H__16E7C721_351C_11D1_95CD_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DirView.h : header file
//

#define DV_NAME   0
#define DV_PATH   1
#define DV_STATUS 2
#define DV_LTIME 3
#define DV_RTIME 4
#define DV_EXT		5

/////////////////////////////////////////////////////////////////////////////
// CDirView view
#include <afxcview.h>
#include "listvwex.h"
#include "SortHeaderCtrl.h"


struct tagDIFFITEM;
typedef struct tagDIFFITEM DIFFITEM;

class CDiffContext;

typedef enum { eMain, eContext } eMenuType;

class CDirDoc;

class CDirView : public CListViewEx
{
protected:
	CDirView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDirView)

// Attributes
public:
	CDirDoc* GetDocument(); // non-debug version is inline
	const CDirDoc * GetDocument() const { return const_cast<CDirView *>(this)->GetDocument(); }
private:
	CDiffContext * GetDiffContext();

// Operations
public:
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	void UpdateResources();
	POSITION GetItemKey(int idx);
	void SetItemKey(int idx, POSITION diffpos);
	void DeleteAllDisplayItems();
	int GetItemIndex(DWORD key);

	UINT GetSelectedCount() const;
	int GetFirstSelectedInd();
	DIFFITEM GetNextSelectedInd(int &ind);
	DIFFITEM GetItemAt(int ind);

// Implementation types
private:
	typedef enum { ACT_COPY=1, ACT_DEL_LEFT, ACT_DEL_RIGHT, ACT_DEL_BOTH } ACT_TYPE;
	struct action { CString src; CString dest; BOOL dirflag; int idx; };
	typedef CList<int, int> DeletedItemList; // indices into display list control
	struct ActionList
	{
		int selcount; // #items in full selection (not all may be affected)
		ACT_TYPE atype;
		CList<action, action&> actions;
		CStringList errors;
		DeletedItemList deletedItems;
		ActionList(ACT_TYPE at) : selcount(0), atype(at) { }
		int GetCount() const { return actions.GetCount(); }
	};
	typedef enum { SIDE_LEFT=1, SIDE_RIGHT } SIDE_TYPE;

// Implementation in DirActions.cpp
private:
	BOOL GetSelectedDirNames(CString& strLeft, CString& strRight) const;
	BOOL GetSelectedFileNames(CString& strLeft, CString& strRight) const;
	CString GetSelectedFileName(SIDE_TYPE stype) const;
	void GetItemFileNames(int sel, CString& strLeft, CString& strRight) const;
	BOOL IsItemLeftOnly(int code);
	BOOL IsItemRightOnly(int code);
	BOOL IsItemCopyableToLeft(int code);
	BOOL IsItemCopyableToRight(int code);
	BOOL IsItemDeletableOnLeft(int code);
	BOOL IsItemDeletableOnRight(int code);
	BOOL IsItemDeletableOnBoth(int code);
	BOOL IsItemOpenableOnLeft(int code);
	BOOL IsItemOpenableOnRight(int code);
	void DoCopyFileToRight();
	void DoCopyFileToLeft();
	void DoDelLeft();
	void DoDelRight();
	void DoDelBoth();
	void DoOpen(SIDE_TYPE stype);
	void DoOpenWith(SIDE_TYPE stype);
	void ConfirmAndPerformActions(ActionList & actions);
	BOOL ConfirmActionList(const ActionList & actions);
	void PerformActionList(ActionList & actions);
	void PerformAndRemoveTopAction(ActionList & actions);
// End DirActions.cpp


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDirView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation data
protected:
	CSortHeaderCtrl		m_ctlSortHeader;
	CImageList m_imageList;
	bool m_bSortAscending;	// is currently sorted ascending.
	int m_sortColumn;		// index to column which is sorted
	CListCtrl * m_pList;

	
	// Generated message map functions
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	//{{AFX_MSG(CDirView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDirCopyFileToLeft();
	afx_msg void OnCtxtDirCopyFileToLeft();
	afx_msg void OnUpdateDirCopyFileToLeft(CCmdUI* pCmdUI);
	afx_msg void OnDirCopyFileToRight();
	afx_msg void OnCtxtDirCopyFileToRight();
	afx_msg void OnUpdateDirCopyFileToRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxtDirCopyFileToLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxtDirCopyFileToRight(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirDelLeft();
	afx_msg void OnUpdateCtxtDirDelLeft(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirDelRight();
	afx_msg void OnUpdateCtxtDirDelRight(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirDelBoth();
	afx_msg void OnUpdateCtxtDirDelBoth(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirOpenLeft();
	afx_msg void OnUpdateCtxtDirOpenLeft(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirOpenLeftWith();
	afx_msg void OnUpdateCtxtDirOpenLeftWith(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirOpenRight();
	afx_msg void OnUpdateCtxtDirOpenRight(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirOpenRightWith();
	afx_msg void OnUpdateCtxtDirOpenRightWith(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void OpenSelection();
	void DoUpdateDirCopyFileToLeft(CCmdUI* pCmdUI, eMenuType menuType);
	void DoUpdateDirCopyFileToRight(CCmdUI* pCmdUI, eMenuType menuType);
	void ModifyPopup(CMenu * pPopup, int nStringResource, int nMenuId, LPCTSTR szPath);
	void DoUpdateCtxtDirDelLeft(CCmdUI* pCmdUI);
	void DoUpdateCtxtDirDelRight(CCmdUI* pCmdUI);
	void DoUpdateCtxtDirDelBoth(CCmdUI* pCmdUI);
	void DoUpdateOpenLeft(CCmdUI* pCmdUI);
	void DoUpdateOpenRight(CCmdUI* pCmdUI);
	POSITION GetItemKeyFromData(DWORD dw);
	DIFFITEM GetDiffItem(int sel);
	int GetSingleSelectedItem() const;
};


#ifndef _DEBUG  // debug version in DirView.cpp
inline CDirDoc* CDirView::GetDocument()
   { return (CDirDoc*)m_pDocument; }
#endif


CString NumToStr(int n);


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DirView_H__16E7C721_351C_11D1_95CD_444553540000__INCLUDED_)
