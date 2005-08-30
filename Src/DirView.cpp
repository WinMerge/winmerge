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
#include "DirFrame.h"  // StatePane
#include "DirDoc.h"
#include "MainFrm.h"
#include "resource.h"
#include "coretools.h"
#include "WaitStatusCursor.h"
#include "dllver.h"
#include "locality.h"
#include "FileTransform.h"
#include "SelectUnpackerDlg.h"
#include "paths.h"	// GetPairComparability()
#include "7zCommon.h"
#include "OptionsDef.h"
#include "BCMenu.h"
#include "WindowStyle.h"
#include "DirCmpReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirView

enum { COLUMN_REORDER=99 };

/**
 * @brief ID for timer closing Compare Pane after delay
 */
const UINT IDT_CMPPANE_CLOSING = 1;

/**
 * @brief Delay (in milliseconds) for closing Compare Pane
 * after compare is ready. This delay is only applied if
 * automatic closing is enabled.
 */
const UINT CMPPANE_DELAY = 500;

IMPLEMENT_DYNCREATE(CDirView, CListViewEx)

CDirView::CDirView()
: m_numcols(-1)
, m_dispcols(-1)
, m_pHeaderPopup(NULL)
, m_pFont(NULL)
, m_pList(NULL)
, m_nHiddenItems(0)
{
	m_bEscCloses = mf->m_options.GetBool(OPT_CLOSE_WITH_ESC);
}

CDirView::~CDirView()
{
	m_imageList.DeleteImageList();
	delete m_pFont;
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
	ON_UPDATE_COMMAND_UI(ID_STATUS_RIGHTDIR_RO, OnUpdateStatusRightRO)
	ON_UPDATE_COMMAND_UI(ID_STATUS_LEFTDIR_RO, OnUpdateStatusLeftRO)
	ON_COMMAND(ID_FILE_LEFT_READONLY, OnLeftReadOnly)
	ON_UPDATE_COMMAND_UI(ID_FILE_LEFT_READONLY, OnUpdateLeftReadOnly)
	ON_COMMAND(ID_FILE_RIGHT_READONLY, OnRightReadOnly)
	ON_UPDATE_COMMAND_UI(ID_FILE_RIGHT_READONLY, OnUpdateRightReadOnly)
	ON_COMMAND(ID_TOOLS_CUSTOMIZECOLUMNS, OnCustomizeColumns)
	ON_COMMAND(ID_TOOLS_GENERATEREPORT, OnToolsGenerateReport)
	ON_COMMAND(ID_DIR_ZIP_LEFT, OnCtxtDirZipLeft)
	ON_COMMAND(ID_DIR_ZIP_RIGHT, OnCtxtDirZipRight)
	ON_COMMAND(ID_DIR_ZIP_BOTH, OnCtxtDirZipBoth)
	ON_COMMAND(ID_DIR_ZIP_BOTH_DIFFS_ONLY, OnCtxtDirZipBothDiffsOnly)
	ON_COMMAND(ID_VIEW_DIR_STATEPANE, OnDirStatePane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DIR_STATEPANE, OnUpdateDirStatePane)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateSelectAll)
	ON_COMMAND_RANGE(ID_PREDIFF_MANUAL, ID_PREDIFF_AUTO, OnPluginPredifferMode)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PREDIFF_MANUAL, ID_PREDIFF_AUTO, OnUpdatePluginPredifferMode)
	ON_COMMAND(ID_DIR_COPY_PATHNAMES_LEFT, OnCopyLeftPathnames)
	ON_COMMAND(ID_DIR_COPY_PATHNAMES_RIGHT, OnCopyRightPathnames)
	ON_COMMAND(ID_DIR_COPY_PATHNAMES_BOTH, OnCopyBothPathnames)
	ON_COMMAND(ID_DIR_COPY_FILENAMES, OnCopyFilenames)
	ON_COMMAND(ID_DIR_HIDE_FILENAMES, OnHideFilenames)
	ON_COMMAND(ID_DIR_MOVE_LEFT_TO_BROWSE, OnCtxtDirMoveLeftTo)
	ON_UPDATE_COMMAND_UI(ID_DIR_MOVE_LEFT_TO_BROWSE, OnUpdateCtxtDirMoveLeftTo)
	ON_COMMAND(ID_DIR_MOVE_RIGHT_TO_BROWSE, OnCtxtDirMoveRightTo)
	ON_UPDATE_COMMAND_UI(ID_DIR_MOVE_RIGHT_TO_BROWSE, OnUpdateCtxtDirMoveRightTo)
	ON_UPDATE_COMMAND_UI(ID_DIR_HIDE_FILENAMES, OnUpdateHideFilenames)
	ON_WM_SIZE()
	ON_COMMAND(ID_MERGE_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_MERGE_DELETE, OnUpdateDelete)
	ON_COMMAND(ID_DIR_RESCAN, OnMarkedRescan)
	ON_UPDATE_COMMAND_UI(ID_STATUS_DIFFNUM, OnUpdateStatusNum)
	ON_COMMAND(ID_VIEW_SHOWHIDDENITEMS, OnViewShowHiddenItems)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWHIDDENITEMS, OnUpdateViewShowHiddenItems)
	ON_COMMAND(ID_MERGE_COMPARE, OnMergeCompare)
	ON_UPDATE_COMMAND_UI(ID_MERGE_COMPARE, OnUpdateMergeCompare)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnInfoTip)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
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

CDirDoc* CDirView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDirDoc)));
	return (CDirDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CDirView message handlers

void CDirView::OnInitialUpdate() 
{
	CListViewEx::OnInitialUpdate();
	m_pList = &GetListCtrl();
	GetDocument()->SetDirView(this);

	// Load user-selected font
	CMainFrame *pMf = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	if (pMf->m_options.GetBool(OPT_FONT_DIRCMP_USECUSTOM))
	{
		if (m_pFont == NULL)
			m_pFont = new CFont;
		
		if (m_pFont != NULL)
		{
			m_pFont->CreateFontIndirect(&pMf->m_lfDir);
				SetFont(m_pFont, TRUE);
		}
	}

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
	VERIFY (bm.LoadBitmap (IDB_ERROR));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_FOLDERUP));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	VERIFY (bm.LoadBitmap (IDB_FOLDERUP_DISABLE));
	VERIFY (-1 != m_imageList.Add (&bm, RGB (255, 255, 255)));
	bm.Detach();
	m_pList->SetImageList (&m_imageList, LVSIL_SMALL);

	// Restore column orders as they had them last time they ran
	LoadColumnOrders();

	// Display column headers (in appropriate order)
	ReloadColumns();

	// Show selection all the time, so user can see current item even when
	// focus is elsewhere (ie, on file edit window)
	WindowStyle_Add(m_pList, LVS_SHOWSELALWAYS);

	// Show selection across entire row.
	// Also allow user to rearrange columns via drag&drop of headers
	// if they have a new enough common controls
	DWORD exstyle = LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP;
	// Also enable infotips if they have new enough version for our
	// custom draw code
	// LPNMLVCUSTOMDRAW->iSubItem not supported before comctl32 4.71
	if (GetDllVersion(_T("comctl32.dll")) >= PACKVERSION(4,71))
		exstyle |= LVS_EX_INFOTIP;
	m_pList->SetExtendedStyle(exstyle);

	// Disable CListViewEx's full row selection which only causes problems
	// (tooltips and custom draw do not work!)
	SetFullRowSel(FALSE);
}

// These are the offsets into the image list created in OnInitDialog
#define DIFFIMG_LUNIQUE     0
#define DIFFIMG_RUNIQUE     1
#define DIFFIMG_DIFF        2
#define DIFFIMG_SAME        3
#define DIFFIMG_UNKNOWN     4
#define DIFFIMG_BINSAME     5
#define DIFFIMG_BINDIFF     6
#define DIFFIMG_LDIRUNIQUE  7
#define DIFFIMG_RDIRUNIQUE  8
#define DIFFIMG_SKIP        9
#define DIFFIMG_DIRSKIP    10
#define DIFFIMG_DIR        11
#define DIFFIMG_ERROR      12
// TODO: Need bitmap for Abort
#define DIFFIMG_ABORT       9
#define DIFFIMG_DIRUP      13
#define DIFFIMG_DIRUP_DISABLE 14

/**
 * @brief Return image index appropriate for this row
 */
int CDirView::GetColImage(const DIFFITEM & di) const
{
	// Must return an image index into image list created above in OnInitDialog
	if (di.isResultError()) return DIFFIMG_ERROR;
	if (di.isResultAbort()) return DIFFIMG_ABORT;
	if (di.isResultFiltered())
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
	SetColAlignments();
}

void CDirView::Redisplay()
{
	ToDoDeleteThisValidateColumnOrdering();
	const CDiffContext &ctxt = GetDocument()->GetDiffContext();

	CString s,s2;
	UINT cnt=0;
	int llen = ctxt.GetNormalizedLeft().GetLength();
	int rlen = ctxt.GetNormalizedRight().GetLength();

	DeleteAllDisplayItems();

	// Disable redrawing while adding new items
	SetRedraw(FALSE);

	// If non-recursive compare, add special item(s)
	if (!GetDocument()->GetRecursive())
		cnt += AddSpecialItems();

	int alldiffs=0;
	POSITION diffpos = ctxt.GetFirstDiffPosition();
	while (diffpos)
	{
		POSITION curdiffpos = diffpos;
		DIFFITEM di = ctxt.GetNextDiffPosition(diffpos);

		// If item has hidden flag, don't add it
		if (di.customFlags1 & ViewCustomFlags::HIDDEN)
			continue;

		if (!di.isResultSame())
			++alldiffs;

		LPCTSTR p=GetDocument()->GetItemPathIfShowable(di, llen, rlen);

		if (p)
		{
			int i = AddDiffItem(cnt, di, p, curdiffpos);
			UpdateDiffItemStatus(i, di);
			cnt++;
		}
	}
	theApp.SetLastCompareResult(alldiffs);
	SortColumnsAppropriately();
	SetRedraw(TRUE);
}

/**
 * @brief User right-clicked somewhere in this view
 */
void CDirView::OnContextMenu(CWnd*, CPoint point)
{
	if (GetListCtrl().GetItemCount() == 0)
		return;
	// Make sure window is active
	GetParentFrame()->ActivateFrame();

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
 * @brief Format context menu string and disable item if it cannot be applied.
 */
static void NTAPI FormatContextMenu(BCMenu *pPopup, UINT uIDItem, int n1, int n2 = 0, int n3 = 0)
{
	CString s1, s2;
	pPopup->GetMenuText(uIDItem, s1, MF_BYCOMMAND);
	s2.FormatMessage(s1, NumToStr(n1), NumToStr(n2), NumToStr(n3));
	pPopup->SetMenuText(uIDItem, s2, MF_BYCOMMAND);
	if (n1 == 0)
	{
		pPopup->EnableMenuItem(uIDItem, MF_GRAYED);
	}
}

/**
 * @brief Toggle context menu item
 */
static void NTAPI CheckContextMenu(BCMenu *pPopup, UINT uIDItem, BOOL bCheck)
{
	if (bCheck)
		pPopup->CheckMenuItem(uIDItem, MF_CHECKED);
	else
		pPopup->CheckMenuItem(uIDItem, MF_UNCHECKED);
}

/**
 * @brief User right-clicked in listview rows
 */
void CDirView::ListContextMenu(CPoint point, int /*i*/)
{
	CDirDoc *pDoc = GetDocument();
	BCMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_DIRVIEW));
	VERIFY(menu.LoadToolbar(IDR_MAINFRAME));

	// 1st submenu of IDR_POPUP_DIRVIEW is for item popup
	BCMenu *pPopup = (BCMenu*) menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CMenu menuPluginsHolder;
	CString s;
	menuPluginsHolder.LoadMenu(IDR_POPUP_PLUGINS_SETTINGS);
	VERIFY(s.LoadString(ID_TITLE_PLUGINS_SETTINGS));
	pPopup->AppendMenu(MF_POPUP, (int)menuPluginsHolder.m_hMenu, s.GetBuffer(0));

	// set the menu items with the proper directory names
	CString sl, sr;
	GetSelectedDirNames(sl, sr);

	// TODO: It would be more efficient to set
	// all the popup items now with one traverse over selected items
	// instead of using updates, in which we make a traverse for every item
	// Perry, 2002-12-04

	//2003/12/17 Jochen:
	//-	Archive related menu items follow the above suggestion.
	//-	For disabling to work properly, the tracking frame's m_bAutoMenuEnable
	//	member has to temporarily be turned off.
	int nTotal = 0;
	int nCopyableToLeft = 0;
	int nCopyableToRight = 0;
	int nDeletableOnLeft = 0;
	int nDeletableOnRight = 0;
	int nDeletableOnBoth = 0;
	int nOpenableOnLeft = 0;
	int nOpenableOnRight = 0;
	int nOpenableOnBoth = 0;
	int nOpenableOnLeftWith = 0;
	int nOpenableOnRightWith = 0;
	int nDiffItems = 0;
	int nPredifferYes = 0;
	int nPredifferNo = 0;
	int i = -1;
	while ((i = m_pList->GetNextItem(i, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(i);
		if (di.diffcode == 0) // Invalid value, this must be special item
			continue;
		if (IsItemCopyableToLeft(di))
			++nCopyableToLeft;
		if (IsItemCopyableToRight(di))
			++nCopyableToRight;

		if (IsItemDeletableOnLeft(di))
			++nDeletableOnLeft;
		if (IsItemDeletableOnRight(di))
			++nDeletableOnRight;
		if (IsItemDeletableOnBoth(di))
			++nDeletableOnBoth;

		if (IsItemOpenableOnLeft(di))
			++nOpenableOnLeft;
		if (IsItemOpenableOnRight(di))
			++nOpenableOnRight;

		if (IsItemOpenableOnLeftWith(di))
			++nOpenableOnLeftWith;
		if (IsItemOpenableOnRightWith(di))
			++nOpenableOnRightWith;

		if (IsItemNavigableDiff(di))
			++nDiffItems;
		if (IsItemOpenableOnLeft(di) || IsItemOpenableOnRight(di))
			++nOpenableOnBoth;
		++nTotal;

		// note the prediffer flag for 'files present on both sides and not skipped'
		if (!di.isDirectory() && !di.isBin() && !di.isSideLeft() && !di.isSideRight() && !di.isResultFiltered())
		{
			CString leftPath = di.getLeftFilepath(pDoc->GetLeftBasePath()) +
					_T("\\") + di.sLeftFilename;
			CString rightPath = di.getRightFilepath(pDoc->GetRightBasePath()) +
					_T("\\") + di.sRightFilename;
			CString filteredFilenames = leftPath + "|" + rightPath;
			PackingInfo * unpacker;
			PrediffingInfo * prediffer;
			GetDocument()->FetchPluginInfos(filteredFilenames, &unpacker, &prediffer);
			if (prediffer->bToBeScanned == 1 || prediffer->pluginName.IsEmpty() == FALSE)
				nPredifferYes ++;
			else
				nPredifferNo ++;
		}
	}

	FormatContextMenu(pPopup, ID_DIR_COPY_PATHNAMES_LEFT, nOpenableOnLeft, nTotal);
	FormatContextMenu(pPopup, ID_DIR_COPY_PATHNAMES_RIGHT, nOpenableOnRight, nTotal);
	FormatContextMenu(pPopup, ID_DIR_COPY_PATHNAMES_BOTH, nOpenableOnBoth, nTotal);

	FormatContextMenu(pPopup, ID_DIR_ZIP_LEFT, nOpenableOnLeft, nTotal);
	FormatContextMenu(pPopup, ID_DIR_ZIP_RIGHT, nOpenableOnRight, nTotal);
	FormatContextMenu(pPopup, ID_DIR_ZIP_BOTH, nOpenableOnBoth, nTotal);
	FormatContextMenu(pPopup, ID_DIR_ZIP_BOTH_DIFFS_ONLY, nDiffItems, nTotal);

	CheckContextMenu(pPopup, ID_PREDIFF_AUTO, (nPredifferYes > 0));
	CheckContextMenu(pPopup, ID_PREDIFF_MANUAL, (nPredifferNo > 0));

	CFrameWnd *pFrame = GetTopLevelFrame();
	ASSERT(pFrame != NULL);
	pFrame->m_bAutoMenuEnable = FALSE;
	// invoke context menu
	// this will invoke all the OnUpdate methods to enable/disable the individual items
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		AfxGetMainWnd());
	pFrame->m_bAutoMenuEnable = TRUE;
}

/**
 * @brief User right-clicked on specified logical column
 */
void CDirView::HeaderContextMenu(CPoint point, int /*i*/)
{
	ToDoDeleteThisValidateColumnOrdering();
	BCMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_DIRVIEW));
	VERIFY(menu.LoadToolbar(IDR_MAINFRAME));

	// 2nd submenu of IDR_POPUP_DIRVIEW is for header popup
	BCMenu* pPopup = (BCMenu *)menu.GetSubMenu(1);
	ASSERT(pPopup != NULL);

	// invoke context menu
	// this will invoke all the OnUpdate methods to enable/disable the individual items
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		AfxGetMainWnd());
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
			if (di.diffcode != 0 && IsItemCopyableToLeft(di))
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
			if (di.diffcode != 0 && IsItemCopyableToRight(di))
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
 * @brief Update any resources necessary after a GUI language change
 */
void CDirView::UpdateResources()
{
	UpdateColumnNames();
	GetParentFrame()->UpdateResources();
}

/**
 * @brief User just clicked a column, so perform sort
 */
void CDirView::OnColumnClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// set sort parameters and handle ascending/descending
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*) pNMHDR;
	int oldSortColumn = mf->m_options.GetInt(OPT_DIRVIEW_SORT_COLUMN);
	int sortcol = m_invcolorder[pNMListView->iSubItem];
	if (sortcol == oldSortColumn)
	{
		// Swap direction
		bool bSortAscending = mf->m_options.GetBool(OPT_DIRVIEW_SORT_ASCENDING);
		mf->m_options.SaveOption(OPT_DIRVIEW_SORT_ASCENDING, !bSortAscending);
	}
	else
	{
		mf->m_options.SaveOption(OPT_DIRVIEW_SORT_COLUMN, sortcol);
		// most columns start off ascending, but not dates
		bool bSortAscending = IsDefaultSortAscending(sortcol);
		mf->m_options.SaveOption(OPT_DIRVIEW_SORT_ASCENDING, bSortAscending);
	}

	SortColumnsAppropriately();
	*pResult = 0;
}

void CDirView::SortColumnsAppropriately()
{
	int sortCol = mf->m_options.GetInt(OPT_DIRVIEW_SORT_COLUMN);
	if (sortCol == -1)
		return;

	bool bSortAscending = mf->m_options.GetBool(OPT_DIRVIEW_SORT_ASCENDING);
	m_ctlSortHeader.SetSortImage(ColLogToPhys(sortCol), bSortAscending);
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

/**
 * @brief Open selected item when user presses ENTER key.
 */
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
 * @brief Open parent folder if possible.
 */
void CDirView::OpenParentDirectory()
{
	CDirDoc *pDoc = GetDocument();
	CString leftParent, rightParent;
	switch (pDoc->AllowUpwardDirectory(leftParent, rightParent))
	{
	case CDirDoc::AllowUpwardDirectory::ParentIsTempPath:
		pDoc->m_pTempPathContext = pDoc->m_pTempPathContext->DeleteHead();
		// fall through (no break!)
	case CDirDoc::AllowUpwardDirectory::ParentIsRegularPath:
		mf->DoFileOpen(leftParent, rightParent,
			FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, pDoc->GetRecursive(), pDoc);
		break;
	default:
		AfxMessageBox(IDS_INVALID_DIRECTORY, MB_ICONSTOP);
		break;
	}
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
	int sel = -1;
	CDirDoc * pDoc = GetDocument();

	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		POSITION diffpos = GetItemKey(sel);

		// Browse to parent folder(s) selected, SPECIAL_ITEM_POS is position for
		// special items, but there is currenly only one (parent folder)
		if (diffpos == (POSITION) SPECIAL_ITEM_POS)
		{
			OpenParentDirectory();
			break;
		}

		DIFFITEM & di = pDoc->GetDiffRefByKey((POSITION)diffpos);

		PathContext paths;
		GetItemFileNames(sel, &paths);

		if (di.isDirectory() && (di.isSideLeft() == di.isSideRight()))
		{
			if (pDoc->GetRecursive())
				AfxMessageBox(IDS_FILEISDIR, MB_ICONINFORMATION);
			else
			{
				// Open subfolders if non-recursive compare
				// Don't add folders to MRU
				if (GetPairComparability(paths.GetLeft(), paths.GetRight()) == IS_EXISTING_DIR)
					mf->DoFileOpen(paths.GetLeft(), paths.GetRight(), FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, pDoc->GetRecursive(), pDoc);
				else
					AfxMessageBox(IDS_INVALID_DIRECTORY, MB_ICONSTOP);
			}
			break;
		}
		else if (di.isSideLeft() || di.isSideRight())
			AfxMessageBox(IDS_FILEUNIQUE, MB_ICONINFORMATION);
		else if (HasZipSupport() && ArchiveGuessFormat(paths.GetLeft()) && ArchiveGuessFormat(paths.GetRight()))
		{
			// Open archives, not adding paths to MRU
			mf->DoFileOpen(paths.GetLeft(), paths.GetRight(), FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, pDoc->GetRecursive(), pDoc);
		}
		else if (di.isBin())
			AfxMessageBox(IDS_FILEBINARY, MB_ICONSTOP);
		else
		{
			// Close open documents first (ask to save unsaved data)
			if (!mf->m_options.GetBool(OPT_MULTIDOC_MERGEDOCS))
			{
				if (!pDoc->CloseMergeDocs())
					return;
			}

			// Open identical and different files
			BOOL bLeftRO = pDoc->GetReadOnly(TRUE);
			BOOL bRightRO = pDoc->GetReadOnly(FALSE);

			int rtn = mf->ShowMergeDoc(pDoc, paths.GetLeft(), paths.GetRight(),
				bLeftRO, bRightRO,
				di.left.codepage, di.right.codepage,
				infoUnpacker);
			if (rtn == OPENRESULTS_FAILED_BINARY)
			{
				di.setBin();
				GetDocument()->ReloadItemStatus(sel, FALSE, FALSE);
			}
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
			if (di.diffcode != 0 && IsItemDeletableOnLeft(di))
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
			if (di.diffcode != 0 && IsItemDeletableOnRight(di))
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
			if (di.diffcode != 0 && IsItemDeletableOnBoth(di))
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
		if (di.diffcode != 0 && IsItemCopyableToOnLeft(di))
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
		if (di.diffcode != 0 && IsItemCopyableToOnRight(di))
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
 * @brief Enable/disable "Move | Left to..." and update item text
 */
void CDirView::DoUpdateCtxtDirMoveLeftTo(CCmdUI* pCmdUI) 
{
	int sel=-1;
	int count=0, total=0;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode != 0 && IsItemCopyableToOnLeft(di) && IsItemDeletableOnLeft(di))
			++count;
		++total;
	}
	pCmdUI->Enable(count>0);

	CString s;
	if (count == total)
		AfxFormatString1(s, IDS_MOVE_LEFT_TO, NumToStr(total));
	else
		AfxFormatString2(s, IDS_MOVE_LEFT_TO2, NumToStr(count), NumToStr(total));
	pCmdUI->SetText(s);
}

/**
 * @brief Enable/disable "Move | Right to..." and update item text
 */
void CDirView::DoUpdateCtxtDirMoveRightTo(CCmdUI* pCmdUI) 
{
	int sel=-1;
	int count=0, total=0;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode != 0 && IsItemCopyableToOnRight(di) && IsItemDeletableOnRight(di))
			++count;
		++total;
	}
	pCmdUI->Enable(count>0);

	CString s;
	if (count == total)
		AfxFormatString1(s, IDS_MOVE_RIGHT_TO, NumToStr(total));
	else
		AfxFormatString2(s, IDS_MOVE_RIGHT_TO2, NumToStr(count), NumToStr(total));
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
	if (diffpos == (POSITION) SPECIAL_ITEM_POS)
	{
		static DIFFITEM item;
		return item;
	}
	return GetDocument()->GetDiffByKey((POSITION)diffpos);
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

// Used for Open
void CDirView::DoUpdateOpen(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	int sel = GetSingleSelectedItem();
	if (sel != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (IsItemOpenable(di))
			bEnable = TRUE;
	}

	pCmdUI->Enable(bEnable);
}

// used for OpenLeft
void CDirView::DoUpdateOpenLeft(CCmdUI* pCmdUI)
{
	int sel = GetSingleSelectedItem();
	if (sel != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode == 0 || !IsItemOpenableOnLeft(di))
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
		if (di.diffcode == 0 || !IsItemOpenableOnRight(di))
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
		if (di.diffcode == 0 || !IsItemOpenableOnLeftWith(di))
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
		if (di.diffcode == 0 || !IsItemOpenableOnRightWith(di))
			sel = -1;
	}

	pCmdUI->Enable(sel>=0);
}

/**
 * @brief Update main menu "Merge | Delete" item
 */
void CDirView::DoUpdateDelete(CCmdUI* pCmdUI)
{
	// If no selected items, disable
	int count = GetSelectedCount();
	if (count == 0)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	// Enable if one deletable item is found
	int sel = -1;
	count = 0;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1 && count == 0)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode != 0 &&
			(IsItemDeletableOnLeft(di) || IsItemDeletableOnRight(di)))
		{
			++count;
		}
	}
	pCmdUI->Enable(count > 0);
}

UINT CDirView::GetSelectedCount() const
{
	return m_pList->GetSelectedCount();
}

int CDirView::GetFirstSelectedInd()
{
	return m_pList->GetNextItem(-1, LVNI_SELECTED);
}

/*DIFFITEM CDirView::GetNextSelectedInd(int &ind)
{
	DIFFITEM di;
	int sel =- 1;

	sel = m_pList->GetNextItem(ind, LVNI_SELECTED);
	di = GetDiffItem(ind);
	ind = sel;
	
	return di;
}*/

DIFFITEM CDirView::GetItemAt(int ind)
{
	if (ind != -1)
	{	
		return GetDiffItem(ind);
	}
	// 26.01.2004 jtuc: seems to be nerver reached...
	ASSERT(FALSE);
	DIFFITEM di;
	return di;
}

// Go to first diff
// If none or one item selected select found item
// This is used for scrolling to first diff too
void CDirView::OnFirstdiff()
{
	ASSERT(m_pList);
	const int count = m_pList->GetItemCount();
	BOOL found = FALSE;
	int i = 0;
	int currentInd = GetFirstSelectedInd();
	int selCount = GetSelectedCount();

	while (i < count && found == FALSE)
	{
		const DIFFITEM &di = GetItemAt(i);
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
	BOOL found = FALSE;
	const int count = m_pList->GetItemCount();
	int i = count - 1;
	int currentInd = GetFirstSelectedInd();
	int selCount = GetSelectedCount();

	while (i > -1 && found == FALSE)
	{
		const DIFFITEM &di = GetItemAt(i);
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
	const int count = m_pList->GetItemCount();
	BOOL found = FALSE;
	int i = GetFocusedItem();
	int currentInd = 0;
	int selCount = GetSelectedCount();

	currentInd = i;
	i++;

	while (i < count && found == FALSE)
	{
		const DIFFITEM &di = GetItemAt(i);
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
	BOOL found = FALSE;
	int i = GetFocusedItem();
	int currentInd = 0;
	int selCount = GetSelectedCount();

	currentInd = i;
	if (i > 0)
		i--;

	while (i > -1 && found == FALSE)
	{
		const DIFFITEM &di = GetItemAt(i);
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
	const int count = m_pList->GetItemCount();
	BOOL found = FALSE;
	int i = 0;
	int foundInd = -1;

	while (i < count && found == FALSE)
	{
		const DIFFITEM &di = GetItemAt(i);
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
	const int count = m_pList->GetItemCount();
	BOOL found = FALSE;
	int i = count - 1;
	int foundInd = -1;

	while (i > 0 && found == FALSE)
	{
		const DIFFITEM &di = GetItemAt(i);
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
	// Not a valid diffitem, one of special items (e.g "..")
	if (di.diffcode == 0)
		return false;
	if (di.isResultFiltered() || di.isResultError())
		return false;
	// Skip identical directories
	if (di.isDirectory() && !di.isSideLeft() && !di.isSideRight())
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
	// Handle special shortcuts here
	if (pMsg->message == WM_KEYDOWN)
	{
	// Check if we got 'ESC pressed' -message
		if (pMsg->wParam == VK_ESCAPE)
	{
		if (m_bEscCloses)
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_CLOSE);
			return FALSE;
		}
	}
		// Check if we got 'Backspace pressed' -message
		if (pMsg->wParam == VK_BACK)
		{
			OpenParentDirectory();
			return FALSE;
		}
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
	// Close compare pane when compare is ready
	if (mf->m_options.GetBool(OPT_AUTOCLOSE_CMPPANE))
	{
		SetTimer(IDT_CMPPANE_CLOSING, CMPPANE_DELAY, NULL);
	}

	CDirDoc * pDoc = GetDocument();
	ASSERT(pDoc);

	// Currently UI (update) message is sent after compare is ready
	pDoc->CompareReady();
	Redisplay();
	
	if (mf->m_options.GetBool(OPT_SCROLL_TO_FIRST))
		OnFirstdiff();
	else
		MoveSelection(0, 0, 0);

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
			int w = max(10, theApp.GetProfileInt(_T("DirView"), sWidthKey, DefColumnWidth));
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
		Redisplay();
	}
	else if (nIDEvent == IDT_CMPPANE_CLOSING)
	{
		KillTimer(IDT_CMPPANE_CLOSING);
		CDirFrame *pf = GetParentFrame();
		pf->ShowProcessingBar(FALSE);
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

	// Values > 0 are subitem indexes
	// 0 is filename which gets infotip automatically by LVS_EX_INFOTIP style
	if (lvhti.iSubItem > 0)
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
	OnEditColumns();
	SaveColumnOrders();
}

void CDirView::OnCtxtOpenWithUnpacker() 
{
	int sel = -1;
	sel = m_pList->GetNextItem(sel, LVNI_SELECTED);
	if (sel != -1)
	{
		// let the user choose a handler
		CSelectUnpackerDlg dlg(GetDiffItem(sel).sLeftFilename, this);
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
 * @brief Generate report from dir compare results.
 */
void CDirView::OnToolsGenerateReport() 
{
	CDirDoc *pDoc = GetDocument();
	if (!pDoc->HasDiffs())
	{
		// No items, no report
		return;
	}

	DirCmpReport report;
	report.SetList(m_pList);
	PathContext paths(pDoc->GetLeftBasePath(), pDoc->GetRightBasePath());
	report.SetRootPaths(paths);
	report.SetColumns(m_dispcols);
	CString errStr;
	if (report.GenerateReport(errStr))
	{
		if (errStr.IsEmpty())
			AfxMessageBox(IDS_REPORT_SUCCESS, MB_OK | MB_ICONINFORMATION);
		else
			ResMsgBox1(IDS_REPORT_ERROR, errStr, MB_OK | MB_ICONSTOP);
	}
}

/**
 * @brief Show directory compare statepane
 */
void CDirView::OnDirStatePane()
{
	CDirFrame *pf = GetParentFrame();
	if (!GetDocument()->HasDiffs())
		return;

	// Clear and recount item numbers
	pf->clearStatus();
	pf->UpdateStats();
	pf->ShowProcessingBar(TRUE);
}

/**
 * @brief Enable menuitem for compare statepane
 */
void CDirView::OnUpdateDirStatePane(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->HasDiffs());
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
	BOOL bEnable = TRUE;
	CString leftParent, rightParent;
	switch (pDoc->AllowUpwardDirectory(leftParent, rightParent))
	{
	case CDirDoc::AllowUpwardDirectory::No:
		bEnable = FALSE;
		// fall through
	default:
		AddParentFolderItem(bEnable);
		retVal = 1;
		// fall through
	case CDirDoc::AllowUpwardDirectory::Never:
		break;
	}
	return retVal;
}

/**
 * @brief Add "Parent folder" ("..") item to directory view
 */
void CDirView::AddParentFolderItem(BOOL bEnable)
{
	int i = AddNewItem(0);
	SetImage(i, bEnable ? DIFFIMG_DIRUP : DIFFIMG_DIRUP_DISABLE);
	SetItemKey(i, (POSITION) SPECIAL_ITEM_POS);

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = i;
	lvItem.iSubItem = 0;
	lvItem.pszText = const_cast<LPTSTR>(_T(".."));
	GetListCtrl().SetItem(&lvItem);
}

/**
 * @brief Zip selected files from left side.
 */
void CDirView::OnCtxtDirZipLeft() 
{
	if (!HasZipSupport())
	{
		AfxMessageBox(IDS_NO_ZIP_SUPPORT, MB_ICONINFORMATION);
		return;
	}

	DirItemEnumerator
	(
		this, LVNI_SELECTED
	|	DirItemEnumerator::Left
	).CompressArchive();
}

/**
 * @brief Zip selected files from right side.
 */
void CDirView::OnCtxtDirZipRight() 
{
	if (!HasZipSupport())
	{
		AfxMessageBox(IDS_NO_ZIP_SUPPORT, MB_ICONINFORMATION);
		return;
	}

	DirItemEnumerator
	(
		this, LVNI_SELECTED
	|	DirItemEnumerator::Right
	).CompressArchive();
}

/**
 * @brief Zip selected files from both sides, using original/altered format.
 */
void CDirView::OnCtxtDirZipBoth() 
{
	if (!HasZipSupport())
	{
		AfxMessageBox(IDS_NO_ZIP_SUPPORT, MB_ICONINFORMATION);
		return;
	}

	DirItemEnumerator
	(
		this, LVNI_SELECTED
	|	DirItemEnumerator::Original
	|	DirItemEnumerator::Altered
	|	DirItemEnumerator::BalanceFolders
	).CompressArchive();
}

/**
 * @brief Zip selected diffs from both sides, using original/altered format.
 */
void CDirView::OnCtxtDirZipBothDiffsOnly() 
{
	if (!HasZipSupport())
	{
		AfxMessageBox(IDS_NO_ZIP_SUPPORT, MB_ICONINFORMATION);
		return;
	}

	DirItemEnumerator
	(
		this, LVNI_SELECTED
	|	DirItemEnumerator::Original
	|	DirItemEnumerator::Altered
	|	DirItemEnumerator::BalanceFolders
	|	DirItemEnumerator::DiffsOnly
	).CompressArchive();
}


/**
 * @brief Select all visible items in dir compare
 */
void CDirView::OnSelectAll()
{
	int selCount = m_pList->GetItemCount();

	for (int i = 0; i < selCount; i++)
	{
		// Don't select special items (SPECIAL_ITEM_POS)
		POSITION diffpos = GetItemKey(i);
		if (diffpos != (POSITION) SPECIAL_ITEM_POS)
			m_pList->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
}

/**
 * @brief Update "Select All" item
 */
void CDirView::OnUpdateSelectAll(CCmdUI* pCmdUI)
{
	if (m_pList->GetItemCount() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Handle clicks in plugin context view in list
 */
void CDirView::OnPluginPredifferMode(UINT nID)
{
	int newsetting = 0;
	switch (nID)
	{
	case ID_PREDIFF_MANUAL:
		newsetting = PLUGIN_MANUAL;
		break;
	case ID_PREDIFF_AUTO:
		newsetting = PLUGIN_AUTO;
		break;
	}
	ApplyPluginPrediffSetting(newsetting);
}

/**
 * @brief Updates just before displaying plugin context view in list
 */
void CDirView::OnUpdatePluginPredifferMode(CCmdUI* pCmdUI)
{
	// 2004-04-03, Perry
	// CMainFrame::OnUpdatePluginUnpackMode handles this for global unpacking
	// and is the template to copy, but here, this is a bit tricky
	// as a group of files may be selected
	// and they may not all have the same setting
	// so I'm not trying this right now
}

/**
 * @brief Resets column widths to defaults.
 */
void CDirView::ResetColumnWidths()
{
	for (int i=0; i < m_numcols; i++)
	{
		int phy = ColLogToPhys(i);
		if (phy >= 0)
		{
			CString sWidthKey = GetColRegValueNameBase(i) + _T("_Width");
			theApp.WriteProfileInt(_T("DirView"), sWidthKey, DefColumnWidth);
		}
	}
}

/**
 * @brief Refresh cached options.
 */
void CDirView::RefreshOptions()
{
	m_bEscCloses = mf->m_options.GetBool(OPT_CLOSE_WITH_ESC);
}

/**
 * @brief Copy selected item left side paths (containing filenames) to clipboard.
 */
void CDirView::OnCopyLeftPathnames()
{
	CString strPaths;
	int sel = -1;

	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (!di.isSideRight())
		{
			strPaths += di.getLeftFilepath(GetDocument()->GetLeftBasePath());
			strPaths += _T("\\");
			if (!di.isDirectory())
				strPaths += di.sLeftFilename;
			strPaths += _T("\r\n");
		}
	}
	PutToClipboard(strPaths, mf->GetSafeHwnd());
}

/**
 * @brief Copy selected item right side paths (containing filenames) to clipboard.
 */
void CDirView::OnCopyRightPathnames()
{
	CDirDoc *pDoc = GetDocument();
	CString strPaths;
	int sel = -1;

	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (!di.isSideLeft())
		{
			strPaths += di. getRightFilepath(pDoc->GetRightBasePath());
			strPaths += _T("\\");
			if (!di.isDirectory())
				strPaths += di.sRightFilename;
			strPaths += _T("\r\n");
		}
	}
	PutToClipboard(strPaths, mf->GetSafeHwnd());
}

/**
 * @brief Copy selected item both side paths (containing filenames) to clipboard.
 */
void CDirView::OnCopyBothPathnames()
{
	CDirDoc * pDoc = GetDocument();
	CString strPaths;
	int sel = -1;

	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (!di.isSideRight())
		{
			strPaths += di.getLeftFilepath(pDoc->GetLeftBasePath());
			strPaths += _T("\\");
			if (!di.isDirectory())
				strPaths += di.sLeftFilename;
			strPaths += _T("\r\n");
		}

		if (!di.isSideLeft())
		{
			strPaths += di. getRightFilepath(pDoc->GetRightBasePath());
			strPaths += _T("\\");
			if (!di.isDirectory())
				strPaths += di.sRightFilename;
			strPaths += _T("\r\n");
		}
	}
	PutToClipboard(strPaths, mf->GetSafeHwnd());
}

/**
 * @brief Copy selected item filenames to clipboard.
 */
void CDirView::OnCopyFilenames()
{
	CString strPaths;
	int sel = -1;

	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (!di.isDirectory())
		{
			strPaths += di.sLeftFilename;
			strPaths += _T("\r\n");
		}
	}
	PutToClipboard(strPaths, mf->GetSafeHwnd());
}

/**
 * @brief hide selected item filenames (removes them from the ListView)
 */
void CDirView::OnHideFilenames()
{
	CDirDoc *pDoc = GetDocument();
	int sel = -1;
	m_pList->SetRedraw(FALSE);	// Turn off updating (better performance)
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		POSITION pos = GetItemKey(sel);
		pDoc->SetItemViewFlag(pos, ViewCustomFlags::HIDDEN, ViewCustomFlags::VISIBILITY);
		m_pList->DeleteItem(sel--);
		m_nHiddenItems++;
	}
	m_pList->SetRedraw(TRUE);	// Turn updating back on
}

/**
 * @brief update menu item
 */
void CDirView::OnUpdateHideFilenames(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pList->GetSelectedCount() != 0);
}

/// User chose (context menu) Move left to...
void CDirView::OnCtxtDirMoveLeftTo() 
{
	DoMoveLeftTo();
}

/// User chose (context menu) Move right to...
void CDirView::OnCtxtDirMoveRightTo() 
{
	DoMoveRightTo();
}

/**
 * @brief Update "Move | Right to..." item
 */
void CDirView::OnUpdateCtxtDirMoveRightTo(CCmdUI* pCmdUI) 
{
	DoUpdateCtxtDirMoveRightTo(pCmdUI);
}

/**
 * @brief Update "Move | Left to..." item
 */
void CDirView::OnUpdateCtxtDirMoveLeftTo(CCmdUI* pCmdUI) 
{
	DoUpdateCtxtDirMoveLeftTo(pCmdUI);
}

/**
 * @brief Update title after window is resized.
 */
void CDirView::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);
	GetDocument()->SetTitle(NULL);
}

/**
 * @brief Called when user selects 'Delete' from 'Merge' menu.
 */
void CDirView::OnDelete()
{
	DoDelAll();
}

/**
 * @brief Enables/disables 'Delete' item in 'Merge' menu.
 */
void CDirView::OnUpdateDelete(CCmdUI* pCmdUI)
{
	DoUpdateDelete(pCmdUI);
}

/**
 * @brief Called when item state is changed.
 *
 * Show count of selected items in statusbar.
 */
void CDirView::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// If item's selected state changed
	if ((pNMListView->uOldState & LVIS_SELECTED) !=
		(pNMListView->uNewState & LVIS_SELECTED))
	{
		CString msg;
		int items = GetSelectedCount();

		if (items == 1)
			VERIFY(msg.LoadString(IDS_STATUS_SELITEM1));
		else
		{
			TCHAR num[20] = {0};
			_itot(items, num, 10);
			AfxFormatString1(msg, IDS_STATUS_SELITEMS, num);
		}
		GetParentFrame()->SetStatus(msg);
	}
	*pResult = 0;
}

/**
 * @brief Called when item is marked for rescan.
 */
void CDirView::OnMarkedRescan()
{
	GetDocument()->SetMarkedRescan();
	MarkForRescan();
	GetDocument()->Rescan();
}

/**
 * @brief Called to update the item count in the status bar
 */
void CDirView::OnUpdateStatusNum(CCmdUI* pCmdUI) 
{
	CString s; // text to display

	int count = m_pList->GetItemCount();
	int focusItem = GetFocusedItem();

	if (focusItem == -1)
	{
		// No item has focus
		CString sCnt;
		sCnt.Format(_T("%ld"), count);
		// "Items: %1"
		AfxFormatString1(s, IDS_DIRVIEW_STATUS_FMT_NOFOCUS, sCnt);
	}
	else
	{
		// An item has focus
		CString sIdx, sCnt;
		// Don't show number to special items
		POSITION pos = GetItemKey(focusItem);
		if (pos != (POSITION) SPECIAL_ITEM_POS)
		{
			// If compare is non-recursive reduce special items count
			BOOL bRecursive = GetDocument()->GetRecursive();
			if (!bRecursive)
			{
				--focusItem;
				--count;
			}
			sIdx.Format(_T("%ld"), focusItem+1);
			sCnt.Format(_T("%ld"), count);
			// "Item %1 of %2"
			AfxFormatString2(s, IDS_DIRVIEW_STATUS_FMT_FOCUS, sIdx, sCnt);
		}
	}
	pCmdUI->SetText(s);
}

/**
 * @brief Show all hidden items.
 */
void CDirView::OnViewShowHiddenItems()
{
	GetDocument()->SetItemViewFlag(ViewCustomFlags::VISIBLE, ViewCustomFlags::VISIBILITY);
	m_nHiddenItems = 0;
	Redisplay();
}

/**
 * @brief Enable/Disable 'Show hidden items' menuitem.
 */
void CDirView::OnUpdateViewShowHiddenItems(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nHiddenItems > 0);
}

void CDirView::OnMergeCompare()
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_OPENING_SELECTION));
	OpenSelection();
}

void CDirView::OnUpdateMergeCompare(CCmdUI *pCmdUI)
{
	DoUpdateOpen(pCmdUI);
}
