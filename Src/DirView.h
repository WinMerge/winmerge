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
 *  @file DirView.h
 *
 *  @brief Declaration of class CDirView
 */ 
//
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_DirView_H__16E7C721_351C_11D1_95CD_444553540000__INCLUDED_)
#define AFX_DirView_H__16E7C721_351C_11D1_95CD_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// TODO delete
/*
#define DV_NAME     0
#define DV_PATH     1
#define DV_STATUS   2
#define DV_LTIME    3
#define DV_RTIME    4
#define DV_EXT      5
*/


/////////////////////////////////////////////////////////////////////////////
// CDirView view
#include <afxcview.h>
#include "listvwex.h"
#include "SortHeaderCtrl.h"


struct DIFFITEM;

class CDiffContext;

typedef enum { eMain, eContext } eMenuType;

class CDirDoc;
class CDirFrame;

class PackingInfo;

/** View displaying results of a diff, one row per file */
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
	const CDiffContext * GetDiffContext() const;

// Operations
public:
	CDirFrame * GetParentFrame();


	void UpdateResources();
	void LoadColumnHeaderItems();
	POSITION GetItemKey(int idx) const;
	void SetItemKey(int idx, POSITION diffpos);
	int GetItemIndex(DWORD key);
	// for populating list
	void DeleteAllDisplayItems();
	void SetColumnWidths();

	void SortColumnsAppropriately();

	UINT GetSelectedCount() const;
	int GetFirstSelectedInd();
	DIFFITEM GetNextSelectedInd(int &ind);
	DIFFITEM GetItemAt(int ind);
	void GotoFirstDiff() { OnFirstdiff(); };

// Implementation types
private:
	typedef enum { ACT_COPY=1, ACT_DEL_LEFT, ACT_DEL_RIGHT, ACT_DEL_BOTH } ACT_TYPE;
	struct action { CString src; CString dest; BOOL dirflag; int idx; int code;};
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
	BOOL IsItemCopyableToLeft(const DIFFITEM & di);
	BOOL IsItemCopyableToRight(const DIFFITEM & di);
	BOOL IsItemDeletableOnLeft(const DIFFITEM & di);
	BOOL IsItemDeletableOnRight(const DIFFITEM & di);
	BOOL IsItemDeletableOnBoth(const DIFFITEM & di);
	BOOL IsItemOpenableOnLeft(const DIFFITEM & di);
	BOOL IsItemOpenableOnRight(const DIFFITEM & di);
	BOOL IsItemOpenableOnLeftWith(const DIFFITEM & di);
	BOOL IsItemOpenableOnRightWith(const DIFFITEM & di);
	void DoCopyLeftToRight();
	void DoCopyRightToLeft();
	void DoDelLeft();
	void DoDelRight();
	void DoDelBoth();
	void DoOpen(SIDE_TYPE stype);
	void DoOpenWith(SIDE_TYPE stype);
	void DoOpenWithEditor(SIDE_TYPE stype);
	void ConfirmAndPerformActions(ActionList & actions);
	BOOL ConfirmActionList(const ActionList & actions);
	void PerformActionList(ActionList & actions);
	void PerformAndRemoveTopAction(ActionList & actions);
// End DirActions.cpp

// Implementation in DirViewCols.cpp
public:
	void UpdateColumnNames();
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	int AddDiffItem(int index, const DIFFITEM & di, LPCTSTR szPath, POSITION curdiffpos);
	void UpdateDiffItemStatus(UINT nIdx, const DIFFITEM & di);
	void ToDoDeleteThisValidateColumnOrdering() { ValidateColumnOrdering(); }
private:
	void InitiateSort();
	void NameColumn(int id, int subitem);
	int AddNewItem(int i);
	void SetSubitem(int item, int phy, LPCTSTR sz);
	bool IsDefaultSortAscending(int col) const;
	int ColPhysToLog(int i) const { return m_invcolorder[i]; }
	int ColLogToPhys(int i) const { return m_colorder[i]; } /**< -1 if not displayed */
	CString GetColDisplayName(int col) const;
	int GetColLogCount() const;
	void LoadColumnOrders();
	void ValidateColumnOrdering();
	void ClearColumnOrders();
	void ResetColumnOrdering();
	void MoveColumn(int psrc, int pdest);
	CString GetColRegValueNameBase(int col) const;
	int GetColDefaultOrder(int col) const;
// End DirViewCols.cpp

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDirView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	int GetFocusedItem();
	int GetFirstDifferentItem();
	int GetLastDifferentItem();
	int GetColImage(const DIFFITEM & di) const;
	int GetDefaultColImage() const;

// Implementation data
protected:
	CString GenerateReport();
	CSortHeaderCtrl m_ctlSortHeader;
	CImageList m_imageList;
	bool m_bSortAscending;  /** < current column sort is ascending ? */
	int m_sortColumn;  /**< index of column currently used for sorting */
	CListCtrl * m_pList;
	int m_numcols;
	int m_dispcols;
	CArray<int, int> m_colorder; /**< colorder[logical#]=physical# */
	CArray<int, int> m_invcolorder; /**< invcolorder[physical]=logical# */
	CPoint m_ptLastMousePos;
	CMenu * m_pHeaderPopup;
	
	// Generated message map functions
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	//{{AFX_MSG(CDirView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDirCopyRightToLeft();
	afx_msg void OnCtxtDirCopyRightToLeft();
	afx_msg void OnUpdateDirCopyRightToLeft(CCmdUI* pCmdUI);
	afx_msg void OnDirCopyLeftToRight();
	afx_msg void OnCtxtDirCopyLeftToRight();
	afx_msg void OnUpdateDirCopyLeftToRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxtDirCopyRightToLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxtDirCopyLeftToRight(CCmdUI* pCmdUI);
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
	afx_msg void OnCtxtDirOpenRightWithEditor();
	afx_msg void OnUpdateCtxtDirOpenRightWithEditor(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirOpenLeftWithEditor();
	afx_msg void OnUpdateCtxtDirOpenLeftWithEditor(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFirstdiff();
	afx_msg void OnUpdateFirstdiff(CCmdUI* pCmdUI);
	afx_msg void OnLastdiff();
	afx_msg void OnUpdateLastdiff(CCmdUI* pCmdUI);
	afx_msg void OnNextdiff();
	afx_msg void OnUpdateNextdiff(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiff();
	afx_msg void OnUpdatePrevdiff(CCmdUI* pCmdUI);
	afx_msg void OnCurdiff();
	afx_msg void OnUpdateCurdiff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSave(CCmdUI* pCmdUI);
	afx_msg LRESULT OnUpdateUIMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRefresh();
	afx_msg void OnUpdateRefresh(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEditColumns();
	afx_msg void OnLeftReadOnly();
	afx_msg void OnUpdateLeftReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnRightReadOnly();
	afx_msg void OnUpdateRightReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusLeftRO(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusRightRO(CCmdUI* pCmdUI);
	afx_msg void OnCustomizeColumns();
	afx_msg void OnCtxtOpenWithUnpacker();
	afx_msg void OnUpdateCtxtOpenWithUnpacker(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	BOOL OnHeaderBeginDrag(LPNMHEADER hdr, LRESULT* pResult);
	BOOL OnHeaderEndDrag(LPNMHEADER hdr, LRESULT* pResult);
	afx_msg void OnInfoTip(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);

private:
	void OpenSelection(PackingInfo * infoUnpacker = NULL);
	void DoUpdateDirCopyRightToLeft(CCmdUI* pCmdUI, eMenuType menuType);
	void DoUpdateDirCopyLeftToRight(CCmdUI* pCmdUI, eMenuType menuType);
	void ModifyPopup(CMenu * pPopup, int nStringResource, int nMenuId, LPCTSTR szPath);
	void DoUpdateCtxtDirDelLeft(CCmdUI* pCmdUI);
	void DoUpdateCtxtDirDelRight(CCmdUI* pCmdUI);
	void DoUpdateCtxtDirDelBoth(CCmdUI* pCmdUI);
	void DoUpdateOpenLeft(CCmdUI* pCmdUI);
	void DoUpdateOpenRight(CCmdUI* pCmdUI);
	void DoUpdateOpenLeftWith(CCmdUI* pCmdUI);
	void DoUpdateOpenRightWith(CCmdUI* pCmdUI);
	POSITION GetItemKeyFromData(DWORD dw) const;
	DIFFITEM GetDiffItem(int sel);
	int GetSingleSelectedItem() const;
	bool IsItemNavigableDiff(const DIFFITEM & di) const;
	void MoveSelection(int currentInd, int i, int selCount);
	void SaveColumnWidths();
	void SaveColumnOrders();
	void FixReordering();
	void HeaderContextMenu(CPoint point, int i);
	void ListContextMenu(CPoint point, int i);
	void ReloadColumns();
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
