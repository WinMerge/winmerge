/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
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
 * @file  DirView.cpp
 *
 * @brief Main implementation file for CDirView
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "Merge.h"
#include "DirView.h"
#include "DirDoc.h"
#include "MainFrm.h"
#include "resource.h"
#include "coretools.h"
#include "WaitStatusCursor.h"
#include "dllver.h"
#include "locality.h"
#include "FileTransform.h"
#include "SelectUnpackerDlg.h"
#include "paths.h"	// paths_GetParentPath()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirView

enum { COLUMN_REORDER=99 };


IMPLEMENT_DYNCREATE(CDirView, CListViewEx)

CDirView::CDirView()
: m_numcols(-1)
, m_dispcols(-1)
, m_bSortAscending(true)
, m_pHeaderPopup(NULL)
{
	m_pList = NULL;
}

CDirView::~CDirView()
{
	m_imageList.DeleteImageList();
}


BEGIN_MESSAGE_MAP(CDirView, CListViewEx)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CDirView)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_R2L, OnDirCopyRightToLeft)
	ON_UPDATE_COMMAND_UI(ID_R2L, OnUpdateDirCopyRightToLeft)
	ON_COMMAND(ID_DIR_COPY_RIGHT_TO_LEFT, OnCtxtDirCopyRightToLeft)
	ON_UPDATE_COMMAND_UI(ID_DIR_COPY_RIGHT_TO_LEFT, OnUpdateCtxtDirCopyRightToLeft)
	ON_COMMAND(ID_L2R, OnDirCopyLeftToRight)
	ON_UPDATE_COMMAND_UI(ID_L2R, OnUpdateDirCopyLeftToRight)
	ON_COMMAND(ID_DIR_COPY_LEFT_TO_RIGHT, OnCtxtDirCopyLeftToRight)
	ON_UPDATE_COMMAND_UI(ID_DIR_COPY_LEFT_TO_RIGHT, OnUpdateCtxtDirCopyLeftToRight)
	ON_COMMAND(ID_DIR_DEL_LEFT, OnCtxtDirDelLeft)
	ON_UPDATE_COMMAND_UI(ID_DIR_DEL_LEFT, OnUpdateCtxtDirDelLeft)
	ON_COMMAND(ID_DIR_DEL_RIGHT, OnCtxtDirDelRight)
	ON_UPDATE_COMMAND_UI(ID_DIR_DEL_RIGHT, OnUpdateCtxtDirDelRight)
	ON_COMMAND(ID_DIR_DEL_BOTH, OnCtxtDirDelBoth)
	ON_UPDATE_COMMAND_UI(ID_DIR_DEL_BOTH, OnUpdateCtxtDirDelBoth)
	ON_COMMAND(ID_DIR_OPEN_LEFT, OnCtxtDirOpenLeft)
	ON_UPDATE_COMMAND_UI(ID_DIR_OPEN_LEFT, OnUpdateCtxtDirOpenLeft)
	ON_COMMAND(ID_DIR_OPEN_LEFT_WITH, OnCtxtDirOpenLeftWith)
	ON_UPDATE_COMMAND_UI(ID_DIR_OPEN_LEFT_WITH, OnUpdateCtxtDirOpenLeftWith)
	ON_COMMAND(ID_DIR_OPEN_RIGHT, OnCtxtDirOpenRight)
	ON_UPDATE_COMMAND_UI(ID_DIR_OPEN_RIGHT, OnUpdateCtxtDirOpenRight)
	ON_COMMAND(ID_DIR_OPEN_RIGHT_WITH, OnCtxtDirOpenRightWith)
	ON_UPDATE_COMMAND_UI(ID_DIR_OPEN_RIGHT_WITH, OnUpdateCtxtDirOpenRightWith)
	ON_COMMAND(ID_POPUP_OPEN_WITH_UNPACKER, OnCtxtOpenWithUnpacker)
	ON_UPDATE_COMMAND_UI(ID_POPUP_OPEN_WITH_UNPACKER, OnUpdateCtxtOpenWithUnpacker)
	ON_COMMAND(ID_DIR_OPEN_RIGHT_WITHEDITOR, OnCtxtDirOpenRightWithEditor)
	ON_UPDATE_COMMAND_UI(ID_DIR_OPEN_RIGHT_WITHEDITOR, OnUpdateCtxtDirOpenRightWithEditor)
	ON_COMMAND(ID_DIR_OPEN_LEFT_WITHEDITOR, OnCtxtDirOpenLeftWithEditor)
	ON_UPDATE_COMMAND_UI(ID_DIR_OPEN_LEFT_WITHEDITOR, OnUpdateCtxtDirOpenLeftWithEditor)
	ON_COMMAND(ID_DIR_COPY_LEFT_TO_BROWSE, OnCtxtDirCopyLeftTo)
	ON_COMMAND(ID_DIR_COPY_RIGHT_TO_BROWSE, OnCtxtDirCopyRightTo)
	ON_UPDATE_COMMAND_UI(ID_DIR_COPY_LEFT_TO_BROWSE, OnUpdateCtxtDirCopyLeftTo)
	ON_UPDATE_COMMAND_UI(ID_DIR_COPY_RIGHT_TO_BROWSE, OnUpdateCtxtDirCopyRightTo)
	ON_WM_DESTROY()
	ON_WM_CHAR()
	ON_COMMAND(ID_FIRSTDIFF, OnFirstdiff)
	ON_UPDATE_COMMAND_UI(ID_FIRSTDIFF, OnUpdateFirstdiff)
	ON_COMMAND(ID_LASTDIFF, OnLastdiff)
	ON_UPDATE_COMMAND_UI(ID_LASTDIFF, OnUpdateLastdiff)
	ON_COMMAND(ID_NEXTDIFF, OnNextdiff)
	ON_UPDATE_COMMAND_UI(ID_NEXTDIFF, OnUpdateNextdiff)
	ON_COMMAND(ID_PREVDIFF, OnPrevdiff)
	ON_UPDATE_COMMAND_UI(ID_PREVDIFF, OnUpdatePrevdiff)
	ON_COMMAND(ID_CURDIFF, OnCurdiff)
	ON_UPDATE_COMMAND_UI(ID_CURDIFF, OnUpdateCurdiff)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateSave)
	ON_MESSAGE(MSG_UI_UPDATE, OnUpdateUIMessage)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_UPDATE_COMMAND_UI(ID_REFRESH, OnUpdateRefresh)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_EDIT_COLUMNS, OnEditColumns)
	ON_UPDATE_COMMAND_UI(ID_STATUS_RIGHTDIR_RO, OnUpdateStatusRightRO)
	ON_UPDATE_COMMAND_UI(ID_STATUS_LEFTDIR_RO, OnUpdateStatusLeftRO)
	ON_COMMAND(ID_FILE_LEFT_READONLY, OnLeftReadOnly)
	ON_UPDATE_COMMAND_UI(ID_FILE_LEFT_READONLY, OnUpdateLeftReadOnly)
	ON_COMMAND(ID_FILE_RIGHT_READONLY, OnRightReadOnly)
	ON_UPDATE_COMMAND_UI(ID_FILE_RIGHT_READONLY, OnUpdateRightReadOnly)
	ON_COMMAND(ID_TOOLS_CUSTOMIZECOLUMNS, OnCustomizeColumns)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnInfoTip)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirView drawing

void CDirView::OnDraw(CDC* /*pDC*/)
{
	// This is a CListView, so it is wrapped around a Windows common control
	// which does the drawing
}

/////////////////////////////////////////////////////////////////////////////
// CDirView diagnostics

#ifdef _DEBUG
void CDirView::AssertValid() const
{
	CListViewEx::AssertValid();
}

void CDirView::Dump(CDumpContext& dc) const
{
	CListViewEx::Dump(dc);
}

CDirDoc* CDirView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDirDoc)));
	return (CDirDoc*)m_pDocument;
}
#endif //_DEBUG

CDiffContext * CDirView::GetDiffContext()
{
	return GetDocument()->m_pCtxt;
}

const CDiffContext * CDirView::GetDiffContext() const
{
	return GetDocument()->m_pCtxt;
}

/////////////////////////////////////////////////////////////////////////////
// CDirView message handlers

void CDirView::OnInitialUpdate() 
{
	CListViewEx::OnInitialUpdate();
	m_sortColumn = -1;	// start up in no sorted order.
	m_pList = &GetListCtrl();
	GetDocument()->SetDirView(this);

	// Replace standard header with sort header
	if (HWND hWnd = ListView_GetHeader(m_pList->m_hWnd))
		m_ctlSortHeader.SubclassWindow(hWnd);

	// Load the bitmaps used for the list view (to reflect diff status)
	CBitmap bm;
	VERIFY (m_imageList.Create (16, 16, ILC_MASK, 0, 1));
	VERIFY (bm.LoadBitmap (IDB_LFILE));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_RFILE));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_NOTEQUAL));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_EQUAL));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_UNKNOWN));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_BINARYSAME));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_BINARYDIFF));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_LFOLDER));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_RFOLDER));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_FILESKIP));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_FOLDERSKIP));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_FOLDER));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	m_pList->SetImageList (&m_imageList, LVSIL_SMALL);

	// Restore column orders as they had them last time they ran
	LoadColumnOrders();

	// Display column headers (in appropriate order)
	ReloadColumns();

	// Show selection across entire row.
	// Also allow user to rearrange columns via drag&drop of headers
	// if they have a new enough common controls
	DWORD newstyle = LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP;
	// Also enable infotips if they have new enough version for our
	// custom draw code
	// LPNMLVCUSTOMDRAW->iSubItem not supported before comctl32 4.71
	if (GetDllVersion(_T("comctl32.dll")) >= PACKVERSION(4,71))
		newstyle |= LVS_EX_INFOTIP;
	m_pList->SetExtendedStyle(newstyle);

	// Disable CListViewEx's full row selection which only causes problems
	// (tooltips and custom draw do not work!)
	SetFullRowSel(FALSE);
}

// These are the offsets into the image list created in OnInitDialog
#define DIFFIMG_LUNIQUE     0
#define DIFFIMG_RUNIQUE     1
#define DIFFIMG_DIFF        2
#define DIFFIMG_SAME        3
#define DIFFIMG_ERROR       4
#define DIFFIMG_BINSAME     5
#define DIFFIMG_BINDIFF     6
#define DIFFIMG_LDIRUNIQUE  7
#define DIFFIMG_RDIRUNIQUE  8
#define DIFFIMG_SKIP        9
#define DIFFIMG_DIRSKIP    10
#define DIFFIMG_DIR        11

/**
 * @brief Return image index appropriate for this row
 */
int CDirView::GetColImage(const DIFFITEM & di) const
{
	// Must return an image index into image list created above in OnInitDialog
	if (di.isResultError()) return DIFFIMG_ERROR;
	if (di.isResultSkipped())
		return (di.isDirectory() ? DIFFIMG_DIRSKIP : DIFFIMG_SKIP);
	if (di.isSideLeft())
		return (di.isDirectory() ? DIFFIMG_LDIRUNIQUE : DIFFIMG_LUNIQUE);
	if (di.isSideRight())
		return (di.isDirectory() ? DIFFIMG_RDIRUNIQUE : DIFFIMG_RUNIQUE);
	if (di.isResultSame())
		return (di.isBin() ? DIFFIMG_BINSAME : DIFFIMG_SAME);
	if (di.isDirectory())
		return DIFFIMG_DIR;
	// diff
	return (di.isBin() ? DIFFIMG_BINDIFF : DIFFIMG_DIFF);
}
int CDirView::GetDefaultColImage() const
{
	return DIFFIMG_ERROR;
}


void CDirView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_OPENING_SELECTION));
	OpenSelection();
	CListViewEx::OnLButtonDblClk(nFlags, point);
}

/**
 * @brief Load or reload the columns (headers) of the list view
 */
void CDirView::ReloadColumns()
{
	LoadColumnHeaderItems();
	ToDoDeleteThisValidateColumnOrdering();

	ToDoDeleteThisValidateColumnOrdering();
	UpdateColumnNames();
	ToDoDeleteThisValidateColumnOrdering();
	SetColumnWidths();
	ToDoDeleteThisValidateColumnOrdering();
}

/**
 * @brief User right-clicked somewhere in this view
 */
void CDirView::OnContextMenu(CWnd*, CPoint point)
{

	if (!GetListCtrl().GetItemCount())
		return;
	int i=0;
	if (point.x == -1 && point.y == -1)
	{
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);

		point = rect.TopLeft();
		point.Offset(5, 5);
	} else {
		// Check if user right-clicked on header
		// convert screen coordinates to client coordinates of listview
		CPoint insidePt = point;
		GetListCtrl().ScreenToClient(&insidePt);
		// TODO: correct for hscroll ?
		// Ask header control if click was on one of its header items
		HDHITTESTINFO hhti;
		memset(&hhti, 0, sizeof(hhti));
		hhti.pt = insidePt;
		int col = GetListCtrl().GetHeaderCtrl()->SendMessage(HDM_HITTEST, 0, (LPARAM)&hhti);
		if (col >= 0)
		{
			// Presumably hhti.flags & HHT_ONHEADER is true
			HeaderContextMenu(point, ColPhysToLog(col));
			return;
		}
		// bail out if point is not in any row
		LVHITTESTINFO lhti;
		memset(&lhti, 0, sizeof(lhti));
		insidePt = point;
		ScreenToClient(&insidePt);
		lhti.pt = insidePt;
		i = GetListCtrl().HitTest(insidePt);
		TRACE(_T("i=%d\n"), i);
		if (i<0)
			return;
	}

	ListContextMenu(point, i);
}

/**
 * @brief Return nearest ancestor which is not a child window
 */
static CWnd * GetNonChildAncestor(CWnd * w)
{
	CWnd* parent = w;
	while (parent->GetStyle() & WS_CHILD)
		parent = parent->GetParent();
	return parent;
}

/**
 * @brief User right-clicked in listview rows
 */
void CDirView::ListContextMenu(CPoint point, int /*i*/)
{
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_DIRVIEW));

	// 1st submenu of IDR_POPUP_DIRVIEW is for header popup
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	// set the menu items with the proper directory names
	CString sl, sr;
	GetSelectedDirNames(sl, sr);

	// find non-child ancestor to use as menu parent
	CWnd * pWndPopupOwner = GetNonChildAncestor(this);

	// TODO: It would be more efficient to set
	// all the popup items now with one traverse over selected items
	// instead of using updates, in which we make a traverse for every item
	// Perry, 2002-12-04

	// invoke context menu
	// this will invoke all the OnUpdate methods to enable/disable the individual items
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
}

/**
 * @brief User right-clicked on specified logical column
 */
void CDirView::HeaderContextMenu(CPoint point, int /*i*/)
{
	ToDoDeleteThisValidateColumnOrdering();
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_DIRVIEW));

	// 2nd submenu of IDR_POPUP_DIRVIEW is for header popup
	CMenu* pPopup = menu.GetSubMenu(1);
	ASSERT(pPopup != NULL);

	// find non-child ancestor to use as menu parent
	CWnd * pWndPopupOwner = GetNonChildAncestor(this);

	// invoke context menu
	// this will invoke all the OnUpdate methods to enable/disable the individual items
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
	
}	

/// Make a string out of a number
// TODO: Ought to introduce commas every three digits, except this is locale-specific
// How to do this with locale sensitivity ?
CString NumToStr(int n)
{
	CString s = locality::NumToLocaleStr(n);
	return s;
}

/// Change menu item by using string resource
// (Question: Why don't we just remove it from the menu resource entirely & do an Add here ?)
void CDirView::ModifyPopup(CMenu * pPopup, int nStringResource, int nMenuId, LPCTSTR szPath)
{
	CString s;
	AfxFormatString1(s, nStringResource, szPath);
	pPopup->ModifyMenu(nMenuId, MF_BYCOMMAND|MF_STRING, nMenuId, s);
}


/**
 * @brief User chose (main menu) Copy from right to left
 */
void CDirView::OnDirCopyRightToLeft()
{
	DoCopyRightToLeft();
}
/**
 * @brief User chose (main menu) Copy from left to right
 */
void CDirView::OnDirCopyLeftToRight()
{
	DoCopyLeftToRight();
}

/// User chose (context men) Copy from right to left
void CDirView::OnCtxtDirCopyRightToLeft()
{
	DoCopyRightToLeft();
}
/// User chose (context menu) Copy from left to right
void CDirView::OnCtxtDirCopyLeftToRight()
{
	DoCopyLeftToRight();
}

/// User chose (context menu) Copy left to...
void CDirView::OnCtxtDirCopyLeftTo()
{
	DoCopyLeftTo();
}

/// User chose (context menu) Copy from right to...
void CDirView::OnCtxtDirCopyRightTo()
{
	DoCopyRightTo();
}

/// Update context menu Copy Right to Left item
void CDirView::OnUpdateCtxtDirCopyRightToLeft(CCmdUI* pCmdUI) 
{
	DoUpdateDirCopyRightToLeft(pCmdUI, eContext);
}
/// Update context menu Copy Left to Right item
void CDirView::OnUpdateCtxtDirCopyLeftToRight(CCmdUI* pCmdUI) 
{
	DoUpdateDirCopyLeftToRight(pCmdUI, eContext);
}

/// Update main menu Copy Right to Left item
void CDirView::OnUpdateDirCopyRightToLeft(CCmdUI* pCmdUI) 
{
	DoUpdateDirCopyRightToLeft(pCmdUI, eMain);
}
/// Update main menu Copy Left to Right item
void CDirView::OnUpdateDirCopyLeftToRight(CCmdUI* pCmdUI) 
{
	DoUpdateDirCopyLeftToRight(pCmdUI, eMain);
}

/// Should Copy to Left be enabled or disabled ? (both main menu & context menu use this)
void CDirView::DoUpdateDirCopyRightToLeft(CCmdUI* pCmdUI, eMenuType menuType)
{
	if (GetDocument()->GetReadOnly(TRUE))
		pCmdUI->Enable(FALSE);
	else
	{
		int sel=-1;
		int legalcount=0, selcount=0;
		while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
		{
			const DIFFITEM& di = GetDiffItem(sel);
			if (IsItemCopyableToLeft(di))
				++legalcount;
			++selcount;
		}
		pCmdUI->Enable(legalcount>0);
		if (menuType==eContext)
		{
			CString s;
			if (legalcount == selcount)
				AfxFormatString1(s, IDS_COPY_TO_LEFT, NumToStr(selcount));
			else
				AfxFormatString2(s, IDS_COPY_TO_LEFT2, NumToStr(legalcount), NumToStr(selcount));
			pCmdUI->SetText(s);
		}
	}
}

/// Should Copy to Right be enabled or disabled ? (both main menu & context menu use this)
void CDirView::DoUpdateDirCopyLeftToRight(CCmdUI* pCmdUI, eMenuType menuType)
{
	if (GetDocument()->GetReadOnly(FALSE))
		pCmdUI->Enable(FALSE);
	else
	{
		int sel=-1;
		int legalcount=0, selcount=0;
		while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
		{
			const DIFFITEM& di = GetDiffItem(sel);
			if (IsItemCopyableToRight(di))
				++legalcount;
			++selcount;
		}
		pCmdUI->Enable(legalcount>0);
		if (menuType==eContext)
		{
			CString s;
			if (legalcount == selcount)
				AfxFormatString1(s, IDS_COPY_TO_RIGHT, NumToStr(selcount));
			else
				AfxFormatString2(s, IDS_COPY_TO_RIGHT2, NumToStr(legalcount), NumToStr(selcount));
			pCmdUI->SetText(s);
		}
	}
}

/**
 * @brief Update any language-dependent data
 */
void CDirView::UpdateResources()
{
	UpdateColumnNames();
}

/**
 * @brief User just clicked a column, so perform sort
 */
void CDirView::OnColumnClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// set sort parameters and handle ascending/descending
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*) pNMHDR;
	int sortcol = m_invcolorder[pNMListView->iSubItem];
	if (sortcol==m_sortColumn)
	{
		m_bSortAscending = !m_bSortAscending;
	}
	else
	{
		m_sortColumn = sortcol;
		// most columns start off ascending, but not dates
		m_bSortAscending = IsDefaultSortAscending(m_sortColumn);
	}

	SortColumnsAppropriately();
	*pResult = 0;
}

void CDirView::SortColumnsAppropriately()
{
	if (m_sortColumn == -1) return;

	m_ctlSortHeader.SetSortImage(ColLogToPhys(m_sortColumn), m_bSortAscending);
	//sort using static CompareFunc comparison function
	GetListCtrl ().SortItems (CompareFunc, reinterpret_cast<DWORD>(this));//pNMListView->iSubItem);
}

/// Do any last minute work as view closes
void CDirView::OnDestroy() 
{
	DeleteAllDisplayItems();

	ValidateColumnOrdering();
	SaveColumnOrders();
	SaveColumnWidths();

	CListViewEx::OnDestroy();
}

void CDirView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar==VK_RETURN)
	{
		WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_OPENING_SELECTION));
		OpenSelection();
	}
	CListViewEx::OnChar(nChar, nRepCnt, nFlags);
}

/**
 * @brief Open selected files or directories.
 *
 * Opens selected files to file compare. If comparing
 * directories non-recursively, then subfolders and parent
 * folder are opened too.
 * @todo This just opens first selected item - should
 * it do something different when multiple items are selected ?
 */
void CDirView::OpenSelection(PackingInfo * infoUnpacker /*= NULL*/)
{
	int sel = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (sel != -1)
	{
		DIFFITEM di;
		POSITION diffpos = GetItemKey(sel);

		if (diffpos != (POSITION) -1)
			di = GetDiffContext()->GetDiffAt((POSITION)diffpos);

		// Browse to parent folder(s) selected, -1 is position for
		// special items, but there is currenly only one (parent folder)
		if (diffpos == (POSITION) -1)
		{
			CString left = GetDocument()->m_pCtxt->m_strNormalizedLeft;
			CString right = GetDocument()->m_pCtxt->m_strNormalizedRight;
			CString leftParent = paths_GetParentPath(left);
			CString rightParent = paths_GetParentPath(right);

			if (paths_DoesPathExist(leftParent) == IS_EXISTING_DIR &&
					paths_DoesPathExist(rightParent) == IS_EXISTING_DIR)
				mf->DoFileOpen(leftParent, rightParent,
					FFILEOPEN_NOMRU, FFILEOPEN_NOMRU);
		}		
		else if (di.isDirectory() && (di.isSideLeft() == di.isSideRight()))
		{
			CDirDoc * pDoc = GetDocument();
			if (pDoc->GetRecursive())
				AfxMessageBox(IDS_FILEISDIR, MB_ICONINFORMATION);
			else
			{
				// Open subfolders if non-recursive compare
				// Don't add folders to MRU
				CString left, right;
				GetItemFileNames(sel, left, right);
				mf->DoFileOpen(left, right, FFILEOPEN_NOMRU, FFILEOPEN_NOMRU);
			}

		}
		else if (di.isSideLeft() || di.isSideRight())
			AfxMessageBox(IDS_FILEUNIQUE, MB_ICONINFORMATION);
		else if (di.isBin())
			AfxMessageBox(IDS_FILEBINARY, MB_ICONSTOP);
		else
		{
			// Open identical and different files
			CString left, right;
			BOOL bLeftRO = GetDocument()->GetReadOnly(TRUE);
			BOOL bRightRO = GetDocument()->GetReadOnly(FALSE);

			// FileFlags can only set RO status, not clear!
			bLeftRO |= (di.left.flags.flags & FileFlags::RO);
			bRightRO |= (di.right.flags.flags & FileFlags::RO);

			GetItemFileNames(sel, left, right);
			mf->ShowMergeDoc(GetDocument(), left, right,
				bLeftRO, bRightRO,
				di.left.codepage, di.right.codepage,
				infoUnpacker);
		}
	}
}

/// User chose (context menu) delete left
void CDirView::OnCtxtDirDelLeft()
{
	DoDelLeft();
}

/// User chose (context menu) delete right
void CDirView::OnCtxtDirDelRight()
{
	DoDelRight();
}

/// User chose (context menu) delete both
void CDirView::OnCtxtDirDelBoth()
{
	DoDelBoth();
}

/// Enable/disable Delete Left menu choice on context menu
void CDirView::OnUpdateCtxtDirDelLeft(CCmdUI* pCmdUI)
{
	DoUpdateCtxtDirDelLeft(pCmdUI);
}

/// Enable/disable Delete Right menu choice on context menu
void CDirView::OnUpdateCtxtDirDelRight(CCmdUI* pCmdUI) 
{
	DoUpdateCtxtDirDelRight(pCmdUI);
}
/// Enable/disable Delete Both menu choice on context menu
void CDirView::OnUpdateCtxtDirDelBoth(CCmdUI* pCmdUI) 
{
	DoUpdateCtxtDirDelBoth(pCmdUI);
}

/// Should Delete left be enabled or disabled ?
void CDirView::DoUpdateCtxtDirDelLeft(CCmdUI* pCmdUI)
{
	if (GetDocument()->GetReadOnly(TRUE))
		pCmdUI->Enable(FALSE);
	else
	{
		int sel=-1;
		int count=0, total=0;
		while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
		{
			const DIFFITEM& di = GetDiffItem(sel);
			if (IsItemDeletableOnLeft(di))
				++count;
			++total;
		}
		pCmdUI->Enable(count>0);

		CString s;
		if (count == total)
			AfxFormatString1(s, IDS_DEL_LEFT_FMT, NumToStr(total));
		else
			AfxFormatString2(s, IDS_DEL_LEFT_FMT2, NumToStr(count), NumToStr(total));
		pCmdUI->SetText(s);
	}
}

/// Should Delete right be enabled or disabled ?
void CDirView::DoUpdateCtxtDirDelRight(CCmdUI* pCmdUI) 
{
	if (GetDocument()->GetReadOnly(FALSE))
		pCmdUI->Enable(FALSE);
	else
	{
		int sel=-1;
		int count=0, total=0;
		while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
		{
			const DIFFITEM& di = GetDiffItem(sel);
			if (IsItemDeletableOnRight(di))
				++count;
			++total;
		}
		pCmdUI->Enable(count>0);
		
		CString s;
		if (count == total)
			AfxFormatString1(s, IDS_DEL_RIGHT_FMT, NumToStr(total));
		else
			AfxFormatString2(s, IDS_DEL_RIGHT_FMT2, NumToStr(count), NumToStr(total));
		pCmdUI->SetText(s);
	}
}

/**
 * @brief Should Delete both be enabled or disabled ?
 */
void CDirView::DoUpdateCtxtDirDelBoth(CCmdUI* pCmdUI) 
{
	if (GetDocument()->GetReadOnly(TRUE) || GetDocument()->GetReadOnly(FALSE))
		pCmdUI->Enable(FALSE);
	else
	{
		int sel=-1;
		int count=0, total=0;
		while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
		{
			const DIFFITEM& di = GetDiffItem(sel);
			if (IsItemDeletableOnBoth(di))
				++count;
			++total;
		}
		pCmdUI->Enable(count>0);

		CString s;
		if (count == total)
			AfxFormatString1(s, IDS_DEL_BOTH_FMT, NumToStr(total));
		else
			AfxFormatString2(s, IDS_DEL_BOTH_FMT2, NumToStr(count), NumToStr(total));
		pCmdUI->SetText(s);
	}
}

/**
 * @brief Enable/disable "Copy | Left to..." and update item text
 */
void CDirView::DoUpdateCtxtDirCopyLeftTo(CCmdUI* pCmdUI) 
{
	int sel=-1;
	int count=0, total=0;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (IsItemCopyableToOnLeft(di))
			++count;
		++total;
	}
	pCmdUI->Enable(count>0);

	CString s;
	if (count == total)
		AfxFormatString1(s, IDS_COPY_LEFT_TO, NumToStr(total));
	else
		AfxFormatString2(s, IDS_COPY_LEFT_TO2, NumToStr(count), NumToStr(total));
	pCmdUI->SetText(s);
}

/**
 * @brief Enable/disable "Copy | Right to..." and update item text
 */
void CDirView::DoUpdateCtxtDirCopyRightTo(CCmdUI* pCmdUI) 
{
	int sel=-1;
	int count=0, total=0;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (IsItemCopyableToOnRight(di))
			++count;
		++total;
	}
	pCmdUI->Enable(count>0);

	CString s;
	if (count == total)
		AfxFormatString1(s, IDS_COPY_RIGHT_TO, NumToStr(total));
	else
		AfxFormatString2(s, IDS_COPY_RIGHT_TO2, NumToStr(count), NumToStr(total));
	pCmdUI->SetText(s);
}

/**
 * @brief Update "Copy | Right to..." item
 */
void CDirView::OnUpdateCtxtDirCopyLeftTo(CCmdUI* pCmdUI)
{
	DoUpdateCtxtDirCopyLeftTo(pCmdUI);
}

/**
 * @brief Update "Copy | Right to..." item
 */
void CDirView::OnUpdateCtxtDirCopyRightTo(CCmdUI* pCmdUI)
{
	DoUpdateCtxtDirCopyRightTo(pCmdUI);
}

/**
 * @brief Get keydata associated with item in given index
 * @param idx Item's index to list in UI
 */
POSITION CDirView::GetItemKey(int idx) const
{
	return GetItemKeyFromData(m_pList->GetItemData(idx));
}

// SetItemKey & GetItemKey encapsulate how the display list items
// are mapped to DiffItems, which in turn are DiffContext keys to the actual DIFFITEM data
POSITION CDirView::GetItemKeyFromData(DWORD dw) const
{
	return (POSITION)dw;
}
void CDirView::SetItemKey(int idx, POSITION diffpos)
{
	m_pList->SetItemData(idx, (DWORD)diffpos);
}

/**
 * Given index in list control, get its associated DIFFITEM data
 */
DIFFITEM CDirView::GetDiffItem(int sel)
{
	POSITION diffpos = GetItemKey(sel);
	
	// If it is special item, return empty DIFFITEM
	if (diffpos == (POSITION) -1)
	{
		DIFFITEM item;
		return item;
	}
	return GetDiffContext()->GetDiffAt((POSITION)diffpos);
}

void CDirView::DeleteAllDisplayItems()
{
	// item data are just positions (diffposes)
	// that is, they contain no memory needing to be freed
	m_pList->DeleteAllItems();
}

/**
 * @brief Given key, get index of item which has it stored.
 * This function searches from list in UI.
 */
int CDirView::GetItemIndex(DWORD key)
{
	LVFINDINFO findInfo;

	findInfo.flags = LVFI_PARAM;  // Search for itemdata
	findInfo.lParam = key;
	return m_pList->FindItem(&findInfo);
}

/// User chose (context menu) open left
void CDirView::OnCtxtDirOpenLeft()
{
	DoOpen(SIDE_LEFT);
}
/// User chose (context menu) open right
void CDirView::OnCtxtDirOpenRight()
{
	DoOpen(SIDE_RIGHT);
}

/// User chose (context menu) open left with
void CDirView::OnCtxtDirOpenLeftWith()
{
	DoOpenWith(SIDE_LEFT);
}

/// User chose (context menu) open right with
void CDirView::OnCtxtDirOpenRightWith()
{
	DoOpenWith(SIDE_RIGHT);
}

/// User chose (context menu) open right with editor
void CDirView::OnCtxtDirOpenRightWithEditor()
{
	DoOpenWithEditor(SIDE_RIGHT);
}

/// Update context menuitem "Open right | with editor"
void CDirView::OnUpdateCtxtDirOpenRightWithEditor(CCmdUI* pCmdUI)
{
	DoUpdateOpenRightWith(pCmdUI);
}

/// User chose (context menu) open left with editor
void CDirView::OnCtxtDirOpenLeftWithEditor()
{
	DoOpenWithEditor(SIDE_LEFT);
}

void CDirView::OnUpdateCtxtDirOpenLeftWithEditor(CCmdUI* pCmdUI)
{
	DoUpdateOpenLeftWith(pCmdUI);
}

// return selected item index, or -1 if none or multiple
int CDirView::GetSingleSelectedItem() const
{
	int sel=-1, sel2=-1;
	sel = m_pList->GetNextItem(sel, LVNI_SELECTED);
	if (sel == -1) return -1;
	sel2 = m_pList->GetNextItem(sel, LVNI_SELECTED);
	if (sel2 != -1) return -1;
	return sel;
}
// Enable/disable Open Left menu choice on context menu
void CDirView::OnUpdateCtxtDirOpenLeft(CCmdUI* pCmdUI)
{
	DoUpdateOpenLeft(pCmdUI);
}
// Enable/disable Open Right menu choice on context menu
void CDirView::OnUpdateCtxtDirOpenRight(CCmdUI* pCmdUI)
{
	DoUpdateOpenRight(pCmdUI);
}

// Enable/disable Open Left With menu choice on context menu
void CDirView::OnUpdateCtxtDirOpenLeftWith(CCmdUI* pCmdUI)
{
	DoUpdateOpenLeftWith(pCmdUI);
}
// Enable/disable Open Right With menu choice on context menu
void CDirView::OnUpdateCtxtDirOpenRightWith(CCmdUI* pCmdUI)
{
	DoUpdateOpenRightWith(pCmdUI);
}

// used for OpenLeft
void CDirView::DoUpdateOpenLeft(CCmdUI* pCmdUI)
{
	int sel = GetSingleSelectedItem();
	if (sel != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (!IsItemOpenableOnLeft(di))
			sel = -1;
	}

	pCmdUI->Enable(sel>=0);
}

// used for OpenRight
void CDirView::DoUpdateOpenRight(CCmdUI* pCmdUI)
{
	int sel = GetSingleSelectedItem();
	if (sel != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (!IsItemOpenableOnRight(di))
			sel = -1;
	}

	pCmdUI->Enable(sel>=0);
}

// used for OpenLeftWith
void CDirView::DoUpdateOpenLeftWith(CCmdUI* pCmdUI)
{
	int sel = GetSingleSelectedItem();
	if (sel != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (!IsItemOpenableOnLeftWith(di))
			sel = -1;
	}

	pCmdUI->Enable(sel>=0);
}

// used for OpenRightWith
void CDirView::DoUpdateOpenRightWith(CCmdUI* pCmdUI)
{
	int sel = GetSingleSelectedItem();
	if (sel != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (!IsItemOpenableOnRightWith(di))
			sel = -1;
	}

	pCmdUI->Enable(sel>=0);
}

UINT CDirView::GetSelectedCount() const
{
	return m_pList->GetSelectedCount();
}

int CDirView::GetFirstSelectedInd()
{
	int sel =- 1;
	sel = m_pList->GetNextItem(sel, LVNI_SELECTED);
	
	return sel;
}

DIFFITEM CDirView::GetNextSelectedInd(int &ind)
{
	DIFFITEM di;
	int sel =- 1;

	sel = m_pList->GetNextItem(ind, LVNI_SELECTED);
	di = GetDiffItem(ind);
	ind = sel;
	
	return di;
}

DIFFITEM CDirView::GetItemAt(int ind)
{
	DIFFITEM di;
	if (ind != -1)
	{	
		di = GetDiffItem(ind);
	}
	return di;
}

// Go to first diff
// If none or one item selected select found item
// This is used for scrolling to first diff too
void CDirView::OnFirstdiff()
{
	ASSERT(m_pList);
	DIFFITEM di;
	const int count = m_pList->GetItemCount();
	BOOL found = FALSE;
	int i = 0;
	int currentInd = GetFirstSelectedInd();
	int selCount = GetSelectedCount();

	while (i < count && found == FALSE)
	{
		di = GetItemAt(i);
		if (IsItemNavigableDiff(di))
		{
			MoveSelection(currentInd, i, selCount);
			found = TRUE;
		}
		i++;
	}
}

void CDirView::OnUpdateFirstdiff(CCmdUI* pCmdUI)
{
	int firstDiff = GetFirstDifferentItem();
	if (firstDiff > -1)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

// Go to last diff
// If none or one item selected select found item
void CDirView::OnLastdiff()
{
	DIFFITEM di;
	BOOL found = FALSE;
	const int count = m_pList->GetItemCount();
	int i = count - 1;
	int currentInd = GetFirstSelectedInd();
	int selCount = GetSelectedCount();

	while (i > -1 && found == FALSE)
	{
		di = GetItemAt(i);
		if (IsItemNavigableDiff(di))
		{
			MoveSelection(currentInd, i, selCount);
			found = TRUE;
		}
		i--;
	}
}

void CDirView::OnUpdateLastdiff(CCmdUI* pCmdUI)
{
	int firstDiff = GetFirstDifferentItem();
	if (firstDiff > -1)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

// Go to next diff
// If none or one item selected select found item
void CDirView::OnNextdiff()
{
	DIFFITEM di;
	const int count = m_pList->GetItemCount();
	BOOL found = FALSE;
	int i = GetFocusedItem();
	int currentInd = 0;
	int selCount = GetSelectedCount();

	currentInd = i;
	i++;

	while (i < count && found == FALSE)
	{
		di = GetItemAt(i);
		if (IsItemNavigableDiff(di))
		{
			MoveSelection(currentInd, i, selCount);
			found = TRUE;
		}
		i++;
	}
}


void CDirView::OnUpdateNextdiff(CCmdUI* pCmdUI)
{
	int focused = GetFocusedItem();
	int lastDiff = GetLastDifferentItem();

	// Check if different files were found and
	// there is different item after focused item
	if ((lastDiff > -1) && (focused < lastDiff))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

// Go to prev diff
// If none or one item selected select found item
void CDirView::OnPrevdiff()
{
	DIFFITEM di;
	BOOL found = FALSE;
	int i = GetFocusedItem();
	int currentInd = 0;
	int selCount = GetSelectedCount();

	currentInd = i;
	if (i > 0)
		i--;

	while (i > -1 && found == FALSE)
	{
		di = GetItemAt(i);
		if (IsItemNavigableDiff(di))
		{
			MoveSelection(currentInd, i, selCount);
			found = TRUE;
		}
		i--;
	}
}


void CDirView::OnUpdatePrevdiff(CCmdUI* pCmdUI)
{
	int focused = GetFocusedItem();
	int firstDiff = GetFirstDifferentItem();

	// Check if different files were found and
	// there is different item before focused item
	if ((firstDiff > -1) && (firstDiff < focused))
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CDirView::OnCurdiff()
{
	const int count = m_pList->GetItemCount();
	BOOL found = FALSE;
	int i = GetFirstSelectedInd();
	BOOL selected = FALSE;
	BOOL focused = FALSE;

	// No selection - no diff to go
	if (i == -1)
		i = count;

	while (i < count && found == FALSE)
	{
		selected = m_pList->GetItemState(i, LVIS_SELECTED);
		focused = m_pList->GetItemState(i, LVIS_FOCUSED);
			
		if (selected == LVIS_SELECTED && focused == LVIS_FOCUSED)
		{
			m_pList->EnsureVisible(i, FALSE);
			found = TRUE;
		}
		i++;
	}
}

void CDirView::OnUpdateCurdiff(CCmdUI* pCmdUI)
{
	int selection = GetFirstSelectedInd();	
	if (selection > -1)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

int CDirView::GetFocusedItem()
{
	int retInd = -1;
	int i = 0;
	const int count = m_pList->GetItemCount();
	BOOL found = FALSE;
	BOOL focused = FALSE;

	while (i < count && found == FALSE)
	{
		focused = m_pList->GetItemState(i, LVIS_FOCUSED);			
		if (focused == LVIS_FOCUSED)
		{
			retInd = i;
			found = TRUE;
		}
		i++;
	}
	return retInd;
}

int CDirView::GetFirstDifferentItem()
{
	DIFFITEM di;
	const int count = m_pList->GetItemCount();
	BOOL found = FALSE;
	int i = 0;
	int foundInd = -1;

	while (i < count && found == FALSE)
	{
		di = GetItemAt(i);
		if (IsItemNavigableDiff(di))
		{
			foundInd = i;		
			found = TRUE;
		}
		i++;
	}
	return foundInd;
}

int CDirView::GetLastDifferentItem()
{
	DIFFITEM di;
	const int count = m_pList->GetItemCount();
	BOOL found = FALSE;
	int i = count - 1;
	int foundInd = -1;

	while (i > 0 && found == FALSE)
	{
		di = GetItemAt(i);
		if (IsItemNavigableDiff(di))
		{
			foundInd = i;		
			found = TRUE;
		}
		i--;
	}
	return foundInd;
}

// When navigating differences, do we stop at this one ?
bool CDirView::IsItemNavigableDiff(const DIFFITEM & di) const
{
	if (di.isResultSkipped() || di.isResultError())
		return false;
	if (di.isDirectory())
		return false;
	if (di.isResultSame())
		return false;
	return true;
}

// move focus (& selection if only one selected) from currentInd to i
void CDirView::MoveSelection(int currentInd, int i, int selCount)
{
	if (selCount <= 1)
	{
		m_pList->SetItemState(currentInd, 0, LVIS_SELECTED);
		m_pList->SetItemState(currentInd, 0, LVIS_FOCUSED);
		m_pList->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}

	m_pList->SetItemState(i, LVIS_FOCUSED, LVIS_FOCUSED);
	m_pList->EnsureVisible(i, FALSE);
}

void CDirView::OnUpdateSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

CDirFrame * CDirView::GetParentFrame()
{
	// can't verify cast without introducing more coupling
	// (CDirView doesn't include DirFrame.h)
	return (CDirFrame *)CListViewEx::GetParentFrame();
}

void CDirView::OnRefresh()
{
	GetDocument()->Rescan();
}

BOOL CDirView::PreTranslateMessage(MSG* pMsg)
{
	// Check if we got 'ESC pressed' -message
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE)) 
	{
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_CLOSE);
		return FALSE;
	}
	return CListViewEx::PreTranslateMessage(pMsg);
}

void CDirView::OnUpdateRefresh(CCmdUI* pCmdUI)
{
	UINT threadState = GetDocument()->m_diffThread.GetThreadState();
	if (threadState == THREAD_COMPARING)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

/**
 * @brief Called when compare thread asks UI update
 * @note Currently thread asks update after compare is ready
 */
LRESULT CDirView::OnUpdateUIMessage(WPARAM wParam, LPARAM lParam)
{
	CDirDoc * pDoc = GetDocument();
	ASSERT(pDoc);

	// Currently UI (update) message is sent after compare is ready
	pDoc->CompareReady();
	pDoc->Redisplay();
	
	if (mf->m_bScrollToFirst)
		OnFirstdiff();
	return 0; // return value unused
}

BOOL CDirView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR * hdr = reinterpret_cast<NMHDR *>(lParam);
	if (hdr->code == HDN_ENDDRAG)
		return OnHeaderEndDrag((LPNMHEADER)hdr, pResult);
	if (hdr->code == HDN_BEGINDRAG)
		return OnHeaderBeginDrag((LPNMHEADER)hdr, pResult);
	
	return CListViewEx::OnNotify(wParam, lParam, pResult);
}

/**
 * @brief User is starting to drag a column header
 */
BOOL CDirView::OnHeaderBeginDrag(LPNMHEADER hdr, LRESULT* pResult)
{
	// save column widths before user reorders them
	// so we can reload them on the end drag
	SaveColumnWidths();
	return TRUE;
}

/**
 * @brief User just finished dragging a column header
 */
BOOL CDirView::OnHeaderEndDrag(LPNMHEADER hdr, LRESULT* pResult)
{
	int src = hdr->iItem;
	int dest = hdr->pitem->iOrder;
	BOOL allowDrop = TRUE;
	*pResult = !allowDrop;
	if (allowDrop && src!=dest && dest!=-1)
	{
		MoveColumn(src, dest);
	}
	return TRUE;
}

/**
 * @brief Remove any windows reordering of columns
 */
void CDirView::FixReordering()
{
	// LVCOLUMN.iOrder is present with version 4.70+
	if (GetDllVersion(_T("shlwapi.dll")) < PACKVERSION(4,70))
		return;

	LVCOLUMN lvcol;
	lvcol.mask = LVCF_ORDER;
	lvcol.fmt = 0;
	lvcol.cx = 0;
	lvcol.pszText = 0;
	lvcol.iSubItem = 0;
	for (int i=0; i<m_numcols; ++i)
	{
		lvcol.iOrder = i;
		GetListCtrl().SetColumn(i, &lvcol);
	}
}

/** @brief Add columns to display, loading width & order from registry. */
void CDirView::LoadColumnHeaderItems()
{
	bool dummyflag = false;

	CHeaderCtrl * h = m_pList->GetHeaderCtrl();
	if (h->GetItemCount())
	{
		dummyflag = true;
		while (m_pList->GetHeaderCtrl()->GetItemCount()>1)
			m_pList->DeleteColumn(1);
	}

	for (int i=0; i<m_dispcols; ++i)
	{
		LVCOLUMN lvc;
		lvc.mask = LVCF_FMT+LVCF_SUBITEM+LVCF_TEXT;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx = 0;
		lvc.pszText = _T("text");
		lvc.iSubItem = i;
		m_pList->InsertColumn(i, &lvc);
	}
	if (dummyflag)
		m_pList->DeleteColumn(1);

}

/// Update all column widths (from registry to screen)
// Necessary when user reorders columns
void CDirView::SetColumnWidths()
{
	for (int i=0; i<m_numcols; ++i)
	{
		int phy = ColLogToPhys(i);
		if (phy >= 0)
		{
			CString sWidthKey = GetColRegValueNameBase(i) + _T("_Width");
			int w = max(10, theApp.GetProfileInt(_T("DirView"), sWidthKey, 150));
			GetListCtrl().SetColumnWidth(m_colorder[i], w);
		}
	}
}

/** @brief store current column widths into registry */
void CDirView::SaveColumnWidths()
{
	for (int i=0; i < m_numcols; i++)
	{
		int phy = ColLogToPhys(i);
		if (phy >= 0)
		{
			CString sWidthKey = GetColRegValueNameBase(i) + _T("_Width");
			int w = GetListCtrl().GetColumnWidth(phy);
			theApp.WriteProfileInt(_T("DirView"), sWidthKey, w);
		}
	}
}

/** @brief Fire off a resort of the data, to take place when things stabilize. */
void CDirView::InitiateSort()
{
	PostMessage(WM_TIMER, COLUMN_REORDER);
}

void CDirView::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == COLUMN_REORDER)
	{
		// Remove the windows reordering, as we're doing it ourselves
		FixReordering();
		// Now redraw screen
		UpdateColumnNames();
		SetColumnWidths();
		GetDocument()->Redisplay();
	}
	
	CListViewEx::OnTimer(nIDEvent);
}

/// Called before infotip is shown to get infotip text
void CDirView::OnInfoTip(NMHDR * pNMHDR, LRESULT * pResult)
{
	LVHITTESTINFO lvhti = {0};
	NMLVGETINFOTIP * pInfoTip = reinterpret_cast<NMLVGETINFOTIP*>(pNMHDR);
	ASSERT(pInfoTip);

	if (GetDllVersion(_T("comctl32.dll")) < PACKVERSION(4,71))
	{
		// LPNMLVCUSTOMDRAW->iSubItem not supported before comctl32 4.71
		return;
	}

	// Get subitem under mouse cursor
	lvhti.pt = m_ptLastMousePos;
	m_pList->SubItemHitTest(&lvhti);

	// Values >= 0 are subitem indexes
	if (lvhti.iSubItem >= 0)
	{
		// Check that we are over icon or label
		if ((lvhti.flags & LVHT_ONITEMICON) || (lvhti.flags & LVHT_ONITEMLABEL))
		{
			// Set item text to tooltip
			CString strText = m_pList->GetItemText(lvhti.iItem, lvhti.iSubItem);
			_tcscpy(pInfoTip->pszText, strText);
		}
	}
}

/// Track mouse position for showing tooltips
void CDirView::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_ptLastMousePos = point;
	CListViewEx::OnMouseMove(nFlags, point);
}

/// Implement custom draw for DirView items
void CDirView::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	DIFFITEM ditem;

	if (GetDllVersion(_T("comctl32.dll")) < PACKVERSION(4,71))
	{
		// LPNMLVCUSTOMDRAW->iSubItem not supported before comctl32 4.71
		*pResult = CDRF_DODEFAULT;
		return;
	}

	switch (lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:	// Request prepaint notifications for each item.
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT: //Before an item is drawn
		// Add code here to customise whole line and return:
		*pResult = CDRF_NEWFONT;			// If no subitems customised
		//*pResult = CDRF_NOTIFYSUBITEMDRAW;	// To customise subitems
		break;

	default:
		*pResult = CDRF_DODEFAULT;
	}
}

/**
 * @brief Change left-side readonly-status
 */
void CDirView::OnLeftReadOnly()
{
	BOOL bReadOnly = GetDocument()->GetReadOnly(TRUE);
	GetDocument()->SetReadOnly(TRUE, !bReadOnly);
}

/**
 * @brief Update left-readonly menu item
 */
void CDirView::OnUpdateLeftReadOnly(CCmdUI* pCmdUI)
{
	BOOL bReadOnly = GetDocument()->GetReadOnly(TRUE);
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(bReadOnly);
}

/**
 * @brief Change right-side readonly-status
 */
void CDirView::OnRightReadOnly()
{
	BOOL bReadOnly = GetDocument()->GetReadOnly(FALSE);
	GetDocument()->SetReadOnly(FALSE, !bReadOnly);
}

/**
 * @brief Update right-side readonly menuitem
 */
void CDirView::OnUpdateRightReadOnly(CCmdUI* pCmdUI)
{
	BOOL bReadOnly = GetDocument()->GetReadOnly(FALSE);
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(bReadOnly);
}

/**
 * @brief Update left-side readonly statusbar item
 */
void CDirView::OnUpdateStatusLeftRO(CCmdUI* pCmdUI)
{
	BOOL bROLeft = GetDocument()->GetReadOnly(TRUE);
	pCmdUI->Enable(bROLeft);
}

/**
 * @brief Update right-side readonly statusbar item
 */
void CDirView::OnUpdateStatusRightRO(CCmdUI* pCmdUI)
{
	BOOL bRORight = GetDocument()->GetReadOnly(FALSE);
	pCmdUI->Enable(bRORight);
}

/**
 * @brief Open dialog to customize dirview columns
 */
void CDirView::OnCustomizeColumns()
{
	// Located in DirViewColHandler.cpp
	CDirView::OnEditColumns();
}


void CDirView::OnCtxtOpenWithUnpacker() 
{
	int sel = -1;
	sel = m_pList->GetNextItem(sel, LVNI_SELECTED);
	if (sel != -1)
	{
		// let the user choose a handler
		CSelectUnpackerDlg dlg(GetDiffItem(sel).sfilename, this);
		// create now a new infoUnpacker to initialize the manual/automatic flag
		PackingInfo infoUnpacker;
		dlg.SetInitialInfoHandler(&infoUnpacker);

		if (dlg.DoModal() == IDOK)
		{
			infoUnpacker = dlg.GetInfoHandler();
			OpenSelection(&infoUnpacker);
		}
	}

}

void CDirView::OnUpdateCtxtOpenWithUnpacker(CCmdUI* pCmdUI) 
{
	// we need one selected file, existing on both side
	if (m_pList->GetSelectedCount() != 1)
		pCmdUI->Enable(FALSE);
	else
	{
		int sel=-1;
		sel = m_pList->GetNextItem(sel, LVNI_SELECTED);
		const DIFFITEM& di = GetDiffItem(sel);
		if (IsItemDeletableOnBoth(di))
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}
/**
 * @brief Copy the generated diffed report to clipboard
 */
void CDirView::OnEditCopy() 
{
	PutToClipboard(GenerateReport(), this->m_hWnd);
}

/**
 * @brief Create a string report for the viewed diffed directory list
 * @note This function assumes longest header length is < 160.
 * @note DOS-EOL style is used for reports.
 * @todo Error handling for listcontrol access!
 */
CString CDirView::GenerateReport()
{
	//Initialize
	int nCols = m_dispcols;
	int nRows = m_pList->GetItemCount();
	bool onlySelected = (GetSelectedCount() > 0) ? true : false;
	const TCHAR cSeparator = '\t';
	CString report;
	
	// Report:Title
	if (GetDiffContext() != NULL)
		AfxFormatString2(report, IDS_DIRECTORY_REPORT_TITLE, GetDiffContext()->m_strLeft, GetDiffContext()->m_strRight);
	report += _T("\r\n"); // Use DOS-EOL style for reports

	// Report:Header
	for (int currCol = 0; currCol < nCols; currCol++)
	{
		TCHAR columnName[160]; // Assuming max col header will never be > 160
		LVCOLUMN lvc;
		lvc.mask = LVCF_TEXT;
		lvc.pszText = &columnName[0];
		lvc.cchTextMax = countof(columnName);
		if (m_pList->GetColumn(currCol, &lvc))
			report += lvc.pszText;
		report += cSeparator;
	}

	// Report:Detail. All currently displayed columns will be added
	for (int currRow = 0;currRow < nRows; currRow++)
	{
		if (!onlySelected || (onlySelected && 
			(m_pList->GetItemState(currRow, LVIS_SELECTED) & LVIS_SELECTED)))
		{
			report += _T("\r\n"); // Use DOS-EOL style for reports
			for (int currCol = 0; currCol < nCols; currCol++)
			{
				report += m_pList->GetItemText(currRow, currCol);

				// Add tab-separator, but not after last field
				if (currCol < nCols - 1)
					report += cSeparator;
			}
		}
	}
	return report;
}

/**
 * @brief Add special items for non-recursive compare
 * to directory view.
 *
 * Currently only special item is ".." for browsing to
 * parent folders.
 * @return number of items added to view
 */
int CDirView::AddSpecialItems()
{
	CDirDoc *pDoc = GetDocument();
	int retVal = 0;
	CString leftPath = pDoc->m_pCtxt->m_strNormalizedLeft;
	CString rightPath = pDoc->m_pCtxt->m_strNormalizedRight;
	CString leftParent = paths_GetParentPath(leftPath);
	CString rightParent = paths_GetParentPath(rightPath);

	if (paths_DoesPathExist(leftParent) == IS_EXISTING_DIR &&
		paths_DoesPathExist(rightParent) == IS_EXISTING_DIR)
	{
		AddParentFolderItem();
		retVal = 1;
	}
	return retVal;
}

/**
 * @brief Add "Parent folder" ("..") item to directory view
 */
void CDirView::AddParentFolderItem()
{
	int i = AddNewItem(0);
	SetImage(i, DIFFIMG_DIR);
	SetItemKey(i, (POSITION) -1);

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = i;
	lvItem.iSubItem = 0;
	lvItem.pszText = const_cast<LPTSTR>(_T(".."));
	GetListCtrl().SetItem(&lvItem);
}
