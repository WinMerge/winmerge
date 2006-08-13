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
#include "DirCmpReport.h"
#include "DirCompProgressDlg.h"
#include "CompareStatisticsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Location for folder compare specific help to open.
 */
static TCHAR DirViewHelpLocation[] = _T("::/htmlhelp/CompareDirs.html");

/**
 * @brief Limit (in seconds) to signal compare is ready for user.
 * If compare takes longer than this value (in seconds) we inform
 * user about it. Current implementation uses MessageBeep(IDOK).
 */
const int TimeToSignalCompare = 3;

/////////////////////////////////////////////////////////////////////////////
// CDirView

enum { COLUMN_REORDER=99 };

IMPLEMENT_DYNCREATE(CDirView, CListView)

CDirView::CDirView()
: m_numcols(-1)
, m_dispcols(-1)
, m_pList(NULL)
, m_nHiddenItems(0)
, m_pCmpProgressDlg(NULL)
, m_compareStart(0)
{
	m_dwDefaultStyle &= ~LVS_TYPEMASK;
	// Show selection all the time, so user can see current item even when
	// focus is elsewhere (ie, on file edit window)
	m_dwDefaultStyle |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_EDITLABELS;

	m_bEscCloses = GetOptionsMgr()->GetBool(OPT_CLOSE_WITH_ESC);
}

CDirView::~CDirView()
{
	delete m_pCmpProgressDlg;
}

BEGIN_MESSAGE_MAP(CDirView, CListView)
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
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateSelectAll)
	ON_COMMAND_RANGE(ID_PREDIFF_MANUAL, ID_PREDIFF_AUTO, OnPluginPredifferMode)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PREDIFF_MANUAL, ID_PREDIFF_AUTO, OnUpdatePluginPredifferMode)
	ON_COMMAND(ID_DIR_COPY_PATHNAMES_LEFT, OnCopyLeftPathnames)
	ON_COMMAND(ID_DIR_COPY_PATHNAMES_RIGHT, OnCopyRightPathnames)
	ON_COMMAND(ID_DIR_COPY_PATHNAMES_BOTH, OnCopyBothPathnames)
	ON_COMMAND(ID_DIR_COPY_FILENAMES, OnCopyFilenames)
	ON_UPDATE_COMMAND_UI(ID_DIR_COPY_FILENAMES, OnUpdateCopyFilenames)
	ON_COMMAND(ID_DIR_ITEM_RENAME, OnItemRename)
	ON_UPDATE_COMMAND_UI(ID_DIR_ITEM_RENAME, OnUpdateItemRename)
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
	ON_COMMAND(ID_VIEW_DIR_STATISTICS, OnViewCompareStatistics)
	ON_COMMAND(ID_FILE_ENCODING, OnFileEncoding)
	ON_UPDATE_COMMAND_UI(ID_FILE_ENCODING, OnUpdateFileEncoding)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
END_MESSAGE_MAP()

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
	const int iconCX = 16;
	const int iconCY = 16;
	CListView::OnInitialUpdate();
	m_pList = &GetListCtrl();
	GetDocument()->SetDirView(this);

	// Load user-selected font
	if (GetOptionsMgr()->GetBool(OPT_FONT_DIRCMP_USECUSTOM))
	{
		m_font.CreateFontIndirect(&GetMainFrame()->m_lfDir);
		SetFont(&m_font, TRUE);
	}

	// Replace standard header with sort header
	if (HWND hWnd = ListView_GetHeader(m_pList->m_hWnd))
		m_ctlSortHeader.SubclassWindow(hWnd);

	// Load the icons used for the list view (to reflect diff status)
	VERIFY (m_imageList.Create (iconCX, iconCY, ILC_COLOR32 | ILC_MASK, 15, 1));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_LFILE)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_RFILE)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_NOTEQUALFILE)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_EQUALFILE)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_UNKNOWN)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_EQUALBINARY)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_BINARYDIFF)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_LFOLDER)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_RFOLDER)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_FILESKIP)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_FOLDERSKIP)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_FOLDER)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_COMPARE_ERROR)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_FOLDERUP)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_FOLDERUP_DISABLE)));
	VERIFY(-1 != m_imageList.Add(AfxGetApp()->LoadIcon(IDI_COMPARE_ABORTED)));
	m_pList->SetImageList (&m_imageList, LVSIL_SMALL);

	// Restore column orders as they had them last time they ran
	LoadColumnOrders();

	// Display column headers (in appropriate order)
	ReloadColumns();

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
#define DIFFIMG_DIRUP      13
#define DIFFIMG_DIRUP_DISABLE 14
#define DIFFIMG_ABORT      15

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

/**
 * @brief Get default folder compare status image.
 */
int CDirView::GetDefaultColImage() const
{
	return DIFFIMG_ERROR;
}

/**
 * @brief Called before compare is started.
 * CDirDoc calls this function before new compare is started, so this
 * is good place to setup GUI for compare.
 * @param [in] pCompareStats Pointer to class having current compare stats.
 */
void CDirView::StartCompare(CompareStats *pCompareStats)
{
	if (m_pCmpProgressDlg == NULL)
		m_pCmpProgressDlg = new DirCompProgressDlg();

	if (!::IsWindow(m_pCmpProgressDlg->GetSafeHwnd()))
		m_pCmpProgressDlg->Create(IDD_DIRCOMP_PROGRESS, this);

	m_pCmpProgressDlg->ShowWindow(SW_SHOW);
	m_pCmpProgressDlg->SetCompareStat(pCompareStats);
	m_pCmpProgressDlg->SetDirDoc(GetDocument());
	m_pCmpProgressDlg->StartUpdating();

	m_compareStart = clock();
}

/**
 * @brief Called when folder compare row is double-clicked with mouse.
 * Selected item is opened to folder or file compare.
 */
void CDirView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_OPENING_SELECTION));
	OpenSelection();
	CListView::OnLButtonDblClk(nFlags, point);
}

/**
 * @brief Load or reload the columns (headers) of the list view
 */
void CDirView::ReloadColumns()
{
	LoadColumnHeaderItems();

	UpdateColumnNames();
	SetColumnWidths();
	SetColAlignments();
}

/**
 * @brief Redisplay folder compare view.
 * This function clears folder compare view and then adds
 * items from current compare to it.
 */
void CDirView::Redisplay()
{
	CDirDoc *pDoc = GetDocument();
	const CDiffContext &ctxt = pDoc->GetDiffContext();

	UINT cnt = 0;
	// Disable redrawing while adding new items
	SetRedraw(FALSE);

	DeleteAllDisplayItems();

	// If non-recursive compare, add special item(s)
	if (!pDoc->GetRecursive() ||
		pDoc->AllowUpwardDirectory(CString(), CString()) == CDirDoc::AllowUpwardDirectory::ParentIsTempPath)
	{
		cnt += AddSpecialItems();
	}

	int alldiffs = 0;
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

		if (pDoc->IsShowable(di))
		{
			AddNewItem(cnt, curdiffpos, I_IMAGECALLBACK);
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
	int nTotal = 0; // total #items (includes files & directories, either side)
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

/** @brief Convert number to string, with commas
between digits in locale-appropriate manner */
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
	int oldSortColumn = GetOptionsMgr()->GetInt(OPT_DIRVIEW_SORT_COLUMN);
	int sortcol = m_invcolorder[pNMListView->iSubItem];
	if (sortcol == oldSortColumn)
	{
		// Swap direction
		bool bSortAscending = GetOptionsMgr()->GetBool(OPT_DIRVIEW_SORT_ASCENDING);
		GetOptionsMgr()->SaveOption(OPT_DIRVIEW_SORT_ASCENDING, !bSortAscending);
	}
	else
	{
		GetOptionsMgr()->SaveOption(OPT_DIRVIEW_SORT_COLUMN, sortcol);
		// most columns start off ascending, but not dates
		bool bSortAscending = IsDefaultSortAscending(sortcol);
		GetOptionsMgr()->SaveOption(OPT_DIRVIEW_SORT_ASCENDING, bSortAscending);
	}

	SortColumnsAppropriately();
	*pResult = 0;
}

void CDirView::SortColumnsAppropriately()
{
	int sortCol = GetOptionsMgr()->GetInt(OPT_DIRVIEW_SORT_COLUMN);
	if (sortCol == -1)
		return;

	bool bSortAscending = GetOptionsMgr()->GetBool(OPT_DIRVIEW_SORT_ASCENDING);
	m_ctlSortHeader.SetSortImage(ColLogToPhys(sortCol), bSortAscending);
	//sort using static CompareFunc comparison function
	CompareState cs(this, sortCol, bSortAscending);
	GetListCtrl().SortItems(cs.CompareFunc, reinterpret_cast<DWORD>(&cs));
}

/// Do any last minute work as view closes
void CDirView::OnDestroy() 
{
	DeleteAllDisplayItems();

	ValidateColumnOrdering();
	SaveColumnOrders();
	SaveColumnWidths();

	CListView::OnDestroy();
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
	CListView::OnChar(nChar, nRepCnt, nFlags);
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
		GetMainFrame()->DoFileOpen(leftParent, rightParent,
			FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, pDoc->GetRecursive(), pDoc);
		// fall through (no break!)
	case CDirDoc::AllowUpwardDirectory::No:
		break;
	default:
		AfxMessageBox(IDS_INVALID_DIRECTORY, MB_ICONSTOP);
		break;
	}
}

/**
 * @brief Get one or two selected items
 *
 * Returns false if 0 or more than 2 items selecte
 */
bool CDirView::GetSelectedItems(int * sel1, int * sel2)
{
	*sel2 = -1;
	*sel1 = m_pList->GetNextItem(-1, LVNI_SELECTED);
	if (*sel1 == -1)
		return false;
	*sel2 = m_pList->GetNextItem(*sel1, LVNI_SELECTED);
	if (*sel2 == -1)
		return true;
	int extra = m_pList->GetNextItem(*sel2, LVNI_SELECTED);
	return (extra ==-1);
}

/**
 * @brief Return true if this unpacker handles binary files
 */
static bool
IsBinaryUnpacker(PackingInfo * infoUnpacker)
{
	if (!infoUnpacker)
		return false;
	if (!_tcsstr(infoUnpacker->pluginName, _T("BinaryFile")))
		return false;
	return true;
}

/**
 * @brief Open selected files or directories.
 *
 * Opens selected files to file compare. If comparing
 * directories non-recursively, then subfolders and parent
 * folder are opened too.
 *
 * This handles the case that one item is selected
 * and the case that two items are selected (one on each side)
 */
void CDirView::OpenSelection(PackingInfo * infoUnpacker /*= NULL*/)
{
	CDirDoc * pDoc = GetDocument();


	// First, figure out what was selected (store into pos1 & pos2)

	POSITION pos1 = NULL, pos2 = NULL;
	int sel1=-1, sel2=-1;
	if (!GetSelectedItems(&sel1, &sel2))
	{
		// Must have 1 or 2 items selected
		// Not valid action
		return;
	}

	pos1 = GetItemKey(sel1);
	ASSERT(pos1);
	if (sel2 != -1)
		pos2 = GetItemKey(sel2);

	// Now handle the various cases of what was selected

	if (pos1 == (POSITION) SPECIAL_ITEM_POS)
	{
		if (!pos2)
		{
			// Browse to parent folder(s) selected
			// SPECIAL_ITEM_POS is position for
			// special items, but there is currenly 
			// only one (parent folder)
			OpenParentDirectory();
			return;
		}
		else
		{
			// Parent directory & something else selected
			// Not valid action
			return;
		}
	}

	// Common variables which both code paths below are responsible for setting
	CString pathLeft, pathRight;
	DIFFITEM *di1=NULL, *di2=NULL; // left & right items (di1==di2 if single selection)
	bool isdir=false; // set if we're comparing directories

	if (pos2)
	{
		// Two items selected, get their info
		di1 = &pDoc->GetDiffRefByKey(pos1);
		di2 = &pDoc->GetDiffRefByKey(pos2);

		// Check for binary & side compatibility & file/dir compatibility
		if (!AreItemsOpenable(*di1, *di2))
		{
			return;
		}
		// Ensure that di1 is on left (swap if needed)
		if (di1->isSideRight() || (di1->isSideBoth() && di2->isSideLeft()))
		{
			DIFFITEM * temp = di1;
			di1 = di2;
			di2 = temp;
			int num = sel1;
			sel1 = sel2;
			sel2 = num;
		}
		// Fill in pathLeft & pathRight
		CString temp;
		GetItemFileNames(sel1, pathLeft, temp);
		GetItemFileNames(sel2, temp, pathRight);

		if (di1->isDirectory())
		{
			isdir=true;
			if (GetPairComparability(pathLeft, pathRight) != IS_EXISTING_DIR)
			{
				AfxMessageBox(IDS_INVALID_DIRECTORY, MB_ICONSTOP);
				return;
			} 
		}
	}
	else
	{
		// Only one item selected, so perform diff on its sides

		di1 = &pDoc->GetDiffRefByKey(pos1);
		di2 = di1;

		GetItemFileNames(sel1, pathLeft, pathRight);

		if (di1->isDirectory())
			isdir = true;

		if (di1->isDirectory() && (di1->isSideLeft() == di1->isSideRight()))
		{
			if (pDoc->GetRecursive())
			{
				AfxMessageBox(IDS_FILEISDIR, MB_ICONINFORMATION);
				return;
			}
			else
			{
				// Open subfolders if non-recursive compare
				// Don't add folders to MRU
				if (GetPairComparability(pathLeft, pathRight) != IS_EXISTING_DIR)
				{
					AfxMessageBox(IDS_INVALID_DIRECTORY, MB_ICONSTOP);
					return;
				}
				// Fall through and compare directories
			}
		}
		else if (di1->isSideLeft())
		{
			// Open left-only item to editor if its not a folder or binary
			if (isdir)
				AfxMessageBox(IDS_FOLDERUNIQUE, MB_ICONINFORMATION);
			else if (di1->isBin())
				AfxMessageBox(IDS_CANNOT_OPEN_BINARYFILE, MB_ICONSTOP);
			else
				DoOpenWithEditor(SIDE_LEFT);
			return;
		}
		else if (di1->isSideRight())
		{
			// Open right-only item to editor if its not a folder or binary
			if (isdir)
				AfxMessageBox(IDS_FOLDERUNIQUE, MB_ICONINFORMATION);
			else if (di1->isBin())
				AfxMessageBox(IDS_CANNOT_OPEN_BINARYFILE, MB_ICONSTOP);
			else
				DoOpenWithEditor(SIDE_RIGHT);
			return;
		}
		// Fall through and compare files (which may be archives)
	}

	// Now pathLeft, pathRight, di1, di2, and isdir are all set
	// We have two items to compare, no matter whether same or different underlying DirView item

	if (isdir)
	{
		// Open subfolders
		// Don't add folders to MRU
		GetMainFrame()->DoFileOpen(pathLeft, pathRight, FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, pDoc->GetRecursive(), pDoc);
	}
	else if (HasZipSupport() && ArchiveGuessFormat(pathLeft) && ArchiveGuessFormat(pathRight))
	{
		// Open archives, not adding paths to MRU
		GetMainFrame()->DoFileOpen(pathLeft, pathRight, FFILEOPEN_NOMRU, FFILEOPEN_NOMRU, pDoc->GetRecursive(), pDoc);
	}
	else
	{
		// Regular file case

		// Binary attributes are set after files are unpacked
		// so after plugins such as the MS-Office plugins have had a chance to make them textual
		// We haven't done unpacking yet in this diff, but if a binary flag is already set, 
		// then it was set in a previous diff after unpacking, so we trust it

		if (di1->isBin() || di2->isBin())
		{
			if (!IsBinaryUnpacker(infoUnpacker))
			{
				AfxMessageBox(IDS_FILEBINARY, MB_ICONSTOP);
				return;
			}
		}

		// Close open documents first (ask to save unsaved data)
		if (!GetOptionsMgr()->GetBool(OPT_MULTIDOC_MERGEDOCS))
		{
			if (!pDoc->CloseMergeDocs())
				return;
		}

		// Open identical and different files
		BOOL bLeftRO = pDoc->GetReadOnly(TRUE);
		BOOL bRightRO = pDoc->GetReadOnly(FALSE);

		FileLocation filelocLeft(pathLeft);
		filelocLeft.encoding = di1->left.encoding;

		FileLocation filelocRight(pathRight);
		filelocRight.encoding = di2->right.encoding;

		int rtn = GetMainFrame()->ShowMergeDoc(pDoc, filelocLeft, filelocRight,
			bLeftRO, bRightRO, infoUnpacker);
		if (rtn == OPENRESULTS_FAILED_BINARY)
		{
			if (di1 == di2)
			{
				di1->setBin();
				GetDocument()->ReloadItemStatus(sel1, FALSE, FALSE);
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

/**
 * Given index in list control, get its associated DIFFITEM data
 */
DIFFITEM CDirView::GetDiffItem(int sel) const
{
	// make a copy of a reference, and return copy
	DIFFITEM di = GetDiffItemConstRef(sel);
	return di;
}

/**
 * Given index in list control, get modifiable reference to its DIFFITEM data
 */
DIFFITEM & CDirView::GetDiffItemRef(int sel)
{
	POSITION diffpos = GetItemKey(sel);
	
	// If it is special item, return empty DIFFITEM
	if (diffpos == (POSITION) SPECIAL_ITEM_POS)
	{
		// TODO: It would be better if there were individual items
		// for whatever these special items are
		// because here we have to hope client does not modify this
		// static (shared) item
		static DIFFITEM emptyItem = DIFFITEM::MakeEmptyDiffItem();
		return emptyItem;
	}
	return GetDocument()->GetDiffRefByKey(diffpos);
}

/**
 * Given index in list control, get modifiable reference to its DIFFITEM data
 */
const DIFFITEM & CDirView::GetDiffItemConstRef(int sel) const
{
	CDirView * pDirView = const_cast<CDirView *>(this);
	return pDirView->GetDiffItemRef(sel);

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
	int sel1=-1, sel2=-1;
	if (!GetSelectedItems(&sel1, &sel2))
	{
		// 0 items or more than 2 items seleted
		pCmdUI->Enable(FALSE);
		return;
	}
	if (sel2 == -1)
	{
		// One item selected
		const DIFFITEM& di = GetDiffItem(sel1);
		if (!IsItemOpenable(di))
		{
			pCmdUI->Enable(FALSE);
			return;
		}
	}
	else
	{
		// Two items selected
		const DIFFITEM& di1 = GetDiffItem(sel1);
		const DIFFITEM& di2 = GetDiffItem(sel2);
		if (!AreItemsOpenable(di1, di2))
		{
			pCmdUI->Enable(FALSE);
			return;
		}
	}
	pCmdUI->Enable(TRUE);
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

/**
 * @brief Update dirview context menu "Copy Filenames" item
 */
void CDirView::DoUpdateCopyFilenames(CCmdUI* pCmdUI)
{
	int sel =- 1;
	int count = 0;
	while ((sel = m_pList->GetNextItem(sel, LVNI_SELECTED)) != -1)
	{
		const DIFFITEM& di = GetDiffItem(sel);
		if (di.diffcode != 0 && !di.isDirectory())
			++count;
	}
	pCmdUI->Enable(count > 0);
}

/**
 * @brief Return count of selected items in folder compare.
 */
UINT CDirView::GetSelectedCount() const
{
	return m_pList->GetSelectedCount();
}

/**
 * @brief Return index of first selected item in folder compare.
 */
int CDirView::GetFirstSelectedInd()
{
	return m_pList->GetNextItem(-1, LVNI_SELECTED);
}

/**
 * @brief Get index of next selected item in folder compare.
 * @param [in,out] ind
 * - IN current index, for which next index is searched
 * - OUT new index of found item
 * @return DIFFITEM in found index.
 */
DIFFITEM CDirView::GetNextSelectedInd(int &ind)
{
	DIFFITEM di;
	int sel =- 1;

	sel = m_pList->GetNextItem(ind, LVNI_SELECTED);
	di = GetDiffItem(ind);
	ind = sel;
	
	return di;
}

/**
 * @brief Return DIFFITEM from given index.
 * @param [in] ind Index from where DIFFITEM is wanted.
 * @return DIFFITEM in given index.
 */
DIFFITEM CDirView::GetItemAt(int ind)
{
	ASSERT(ind != -1); // Trap programmer errors in debug
	return GetDiffItem(ind);
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
			MoveFocus(currentInd, i, selCount);
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
			MoveFocus(currentInd, i, selCount);
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
			MoveFocus(currentInd, i, selCount);
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
			MoveFocus(currentInd, i, selCount);
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

/**
 * @brief Move focus to specified item (and selection if multiple items not selected)
 *
 * Moves the focus from item [currentInd] to item [i]
 * Additionally, if there are not multiple items selected, 
 *  deselects item [currentInd] and selects item [i]
 */
void CDirView::MoveFocus(int currentInd, int i, int selCount)
{
	if (selCount <= 1)
	{
		// Not multiple items selected, so bring selection with us
		m_pList->SetItemState(currentInd, 0, LVIS_SELECTED);
		m_pList->SetItemState(currentInd, 0, LVIS_FOCUSED);
		m_pList->SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}

	// Move focus to specified item
	// (this automatically defocuses old item)
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
	return (CDirFrame *)CListView::GetParentFrame();
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
			// ESC doesn't close window when user is renaming an item.
			if (TRUE == IsLabelEdit())
			{
				m_bUserCancelEdit = TRUE;
			}
			else if (m_bEscCloses)
			{
				AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_CLOSE);
				return FALSE;
			}
		}
		// Check if we got 'DEL pressed' -message
		if (pMsg->wParam == VK_DELETE)
		{
			// DEL doesn't delete a file when user is renaming an item.
			if (!IsLabelEdit())
			{
				AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_MERGE_DELETE);
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
	return CListView::PreTranslateMessage(pMsg);
}

void CDirView::OnUpdateRefresh(CCmdUI* pCmdUI)
{
	UINT threadState = GetDocument()->m_diffThread.GetThreadState();
	pCmdUI->Enable(threadState != THREAD_COMPARING);
}

/**
 * @brief Called when compare thread asks UI update.
 * @note Currently thread asks update after compare is ready
 * or aborted.
 */
LRESULT CDirView::OnUpdateUIMessage(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	CDirDoc * pDoc = GetDocument();
	ASSERT(pDoc);

	// Close and destroy the dialog after compare
	m_pCmpProgressDlg->CloseDialog();
	delete m_pCmpProgressDlg;
	m_pCmpProgressDlg = NULL;

	pDoc->CompareReady();
	Redisplay();
	
	if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST))
		OnFirstdiff();
	else
		MoveFocus(0, 0, 0);

	// If compare took more than TimeToSignalCompare seconds, notify user
	clock_t compareEnd = clock();
	if (compareEnd - m_compareStart > TimeToSignalCompare * CLOCKS_PER_SEC)
		MessageBeep(IDOK);
	GetMainFrame()->StartFlashing();

	return 0; // return value unused
}

BOOL CDirView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR * hdr = reinterpret_cast<NMHDR *>(lParam);
	if (hdr->code == HDN_ENDDRAG)
		return OnHeaderEndDrag((LPNMHEADER)hdr, pResult);
	if (hdr->code == HDN_BEGINDRAG)
		return OnHeaderBeginDrag((LPNMHEADER)hdr, pResult);
	
	return CListView::OnNotify(wParam, lParam, pResult);
}

BOOL CDirView::OnChildNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (uMsg == WM_NOTIFY)
	{
		NMHDR *pNMHDR = (NMHDR *)lParam;
		switch (pNMHDR->code)
		{
		case LVN_GETDISPINFO:
			ReflectGetdispinfo((NMLVDISPINFO *)lParam);
			return TRUE;
		case LVN_GETINFOTIPW:
		case LVN_GETINFOTIPA:
			return TRUE;
		}
	}
	return CListView::OnChildNotify(uMsg, wParam, lParam, pResult);
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

	CListView::OnTimer(nIDEvent);
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
 * @brief Fill string list with current dirview column registry key names
 */
void CDirView::GetCurrentColRegKeys(CStringArray & colKeys)
{
	int nphyscols = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int col=0; col<nphyscols; ++col)
	{
		int logcol = ColPhysToLog(col);
		colKeys.Add(GetColRegValueNameBase(logcol));
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

	// Make list of registry keys for columns
	// (needed for XML reports)
	CStringArray colKeys;
	GetCurrentColRegKeys(colKeys);

	DirCmpReport report(colKeys);
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
	AddNewItem(0, (POSITION) SPECIAL_ITEM_POS, bEnable ? DIFFIMG_DIRUP : DIFFIMG_DIRUP_DISABLE);
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
	// While the user is renaming an item, select all the edited text.
	CEdit *pEdit = m_pList->GetEditControl();
	if (NULL != pEdit)
	{
		pEdit->SetSel(pEdit->GetWindowTextLength());
	}
	else
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
}

/**
 * @brief Update "Select All" item
 */
void CDirView::OnUpdateSelectAll(CCmdUI* pCmdUI)
{
	BOOL bEnable = (!IsLabelEdit()) || (m_pList->GetItemCount() > 0);
	pCmdUI->Enable(bEnable);
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
	m_bEscCloses = GetOptionsMgr()->GetBool(OPT_CLOSE_WITH_ESC);
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
			// If item is a folder then subfolder (relative to base folder)
			// is in filename member.
			strPaths += di.sLeftFilename;
			strPaths += _T("\r\n");
		}
	}
	PutToClipboard(strPaths, AfxGetMainWnd()->GetSafeHwnd());
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
			// If item is a folder then subfolder (relative to base folder)
			// is in filename member.
			strPaths += di.sRightFilename;
			strPaths += _T("\r\n");
		}
	}
	PutToClipboard(strPaths, AfxGetMainWnd()->GetSafeHwnd());
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
			// If item is a folder then subfolder (relative to base folder)
			// is in filename member.
			strPaths += di.sLeftFilename;
			strPaths += _T("\r\n");
		}

		if (!di.isSideLeft())
		{
			strPaths += di. getRightFilepath(pDoc->GetRightBasePath());
			strPaths += _T("\\");
			// If item is a folder then subfolder (relative to base folder)
			// is in filename member.
			strPaths += di.sRightFilename;
			strPaths += _T("\r\n");
		}
	}
	PutToClipboard(strPaths, AfxGetMainWnd()->GetSafeHwnd());
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
	PutToClipboard(strPaths, AfxGetMainWnd()->GetSafeHwnd());
}

/**
 * @brief Enable/Disable dirview Copy Filenames context menu item.
 */
void CDirView::OnUpdateCopyFilenames(CCmdUI* pCmdUI)
{
	DoUpdateCopyFilenames(pCmdUI);
}

/**
 * @brief Rename a selected item on both sides.
 *
 */
void CDirView::OnItemRename()
{
	ASSERT(1 == m_pList->GetSelectedCount());
	int nSelItem = m_pList->GetNextItem(-1, LVNI_SELECTED);
	ASSERT(-1 != nSelItem);
	m_pList->EditLabel(nSelItem);
}

/**
 * @brief Enable/Disable dirview Rename context menu item.
 *
 */
void CDirView::OnUpdateItemRename(CCmdUI* pCmdUI)
{
	BOOL bEnabled = (1 == m_pList->GetSelectedCount());
	pCmdUI->Enable(bEnabled && !IsItemSelectedSpecial());
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
 * @brief Called before user start to item label edit.
 *
 * Disable label edit if initiated from a user double-click.
 */
afx_msg void CDirView::OnBeginLabelEdit(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	*pResult = IsItemSelectedSpecial();

	// If label edit is allowed.
	if (FALSE == *pResult)
	{
		CEdit *pEdit = m_pList->GetEditControl();
		ASSERT(NULL != pEdit);

		CString sText;
		pEdit->GetWindowText(sText);
		ASSERT(!sText.IsEmpty());

		// Keep only left file name (separated by '|'). This form occurs
		// when two files exists with same name but not in same case.
		int nPos = sText.Find('|');
		if (-1 != nPos)
		{
			sText = sText.Left(nPos);
			pEdit->SetWindowText(sText);
		}

		m_bUserCancelEdit = FALSE;
	}
}

/**
 * @brief Called when user done with item label edit.
 *
 */
afx_msg void CDirView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = FALSE;

	// We can't use the normal condition of pszText==NULL to know if the
	// user cancels editing when file names had different case (e.g.
	// "file.txt|FILE.txt"). The edit text was changed to "file.txt" and
	// if the user accept it as the new file name, pszText is NULL.

	if (TRUE != m_bUserCancelEdit)
	{
		CEdit *pEdit = m_pList->GetEditControl();
		ASSERT(NULL != pEdit);

		CString sText;
		pEdit->GetWindowText(sText);

		if (!sText.IsEmpty())
		{
			*pResult = DoItemRename(sText);
		}
	}
}

/**
 * @brief Called when item is marked for rescan.
 * This function marks selected items for rescan and rescans them.
 */
void CDirView::OnMarkedRescan()
{
	UINT items = MarkSelectedForRescan();
	if (items > 0)
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

void CDirView::OnViewCompareStatistics()
{
	CompareStatisticsDlg dlg;
	dlg.SetCompareStats(GetDocument()->GetCompareStats());
	dlg.DoModal();
}

/**
 * @brief Display file encoding dialog & handle user's actions
 */
void CDirView::OnFileEncoding()
{
	DoFileEncodingDialog();
}

/**
 * @brief Update "File Encoding" item
 */
void CDirView::OnUpdateFileEncoding(CCmdUI* pCmdUI) 
{
	DoUpdateFileEncodingDialog(pCmdUI);
}

/** @brief Open help from mainframe when user presses F1*/
void CDirView::OnHelp()
{
	GetMainFrame()->ShowHelp(DirViewHelpLocation);
}

/**
 * @brief TRUE while user is editing a file name.
 */
BOOL CDirView::IsLabelEdit()
{
	return (NULL != m_pList->GetEditControl());
}

/**
 * @brief TRUE if selected item is a "special item".
 */
BOOL CDirView::IsItemSelectedSpecial()
{
	int nSelItem = m_pList->GetNextItem(-1, LVNI_SELECTED);
	ASSERT(-1 != nSelItem);
	return ((POSITION)SPECIAL_ITEM_POS == GetItemKey(nSelItem));
}

/**
 * @brief Allow edit "Paste" when renaming an item.
 */
void CDirView::OnEditCopy() 
{
	CEdit *pEdit = m_pList->GetEditControl();
	if (NULL != pEdit)
	{
		pEdit->Copy();
	}
}

/**
 * @brief Allow edit "Cut" when renaming an item.
 */
void CDirView::OnEditCut() 
{
	CEdit *pEdit = m_pList->GetEditControl();
	if (NULL != pEdit)
	{
		pEdit->Cut();
	}
}

/**
* @brief Allow edit "Paste" when renaming an item.
 */
void CDirView::OnEditPaste() 
{
	CEdit *pEdit = m_pList->GetEditControl();
	if (NULL != pEdit)
	{
		pEdit->Paste();
	}
}

/**
 * @brief Allow edit "Undo" when renaming an item.
 */
void CDirView::OnEditUndo() 
{
	CEdit *pEdit = m_pList->GetEditControl();
	if (NULL != pEdit)
	{
		pEdit->Undo();
	}
}

/**
 * @brief Update the tool bar's "Undo" icon. It should be enabled when
 * renaming an item and undo is possible.
 */
void CDirView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CEdit *pEdit = m_pList->GetEditControl();
	if (NULL != pEdit)
	{
		pCmdUI->Enable(pEdit->CanUndo());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
