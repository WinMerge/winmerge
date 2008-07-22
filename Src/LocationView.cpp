/////////////////////////////////////////////////////////////////////////////
//
//    WinMerge: An interactive diff/merge utility
//    Copyright (C) 1997 Dean P. Grimm
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
 * @file  LocationView.cpp
 *
 * @brief Implementation file for CLocationView
 *
 */

// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "OptionsMgr.h"
#include "MergeEditView.h"
#include "LocationView.h"
#include "MergeDoc.h"
#include "BCMenu.h"
#include "OptionsDef.h"
#include "MergeLineFlags.h"
#include "Bitmap.h"
#include "memdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** @brief Size of empty frame above and below bars (in pixels). */
static const int Y_OFFSET = 5;
/** @brief Size of y-margin for visible area indicator (in pixels). */
static const long INDICATOR_MARGIN = 2;
/** @brief Max pixels in view per line in file. */
static const double MAX_LINEPIX = 4.0;
/** @brief Top of difference marker, relative to difference start. */
static const int DIFFMARKER_TOP = 3;
/** @brief Bottom of difference marker, relative to difference start. */
static const int DIFFMARKER_BOTTOM = 3;
/** @brief Width of difference marker. */
static const int DIFFMARKER_WIDTH = 6;
/** @brief Minimum height of the visible area indicator */
static const int INDICATOR_MIN_HEIGHT = 5;

/** 
 * @brief Bars in location pane
 */
enum LOCBAR_TYPE
{
	BAR_NONE = -1,	/**< No bar in given coords */
	BAR_LEFT,		/**< Left side bar in given coords */
	BAR_RIGHT,		/**< Right side bar in given coords */
	BAR_YAREA,		/**< Y-Coord in bar area */
};

/////////////////////////////////////////////////////////////////////////////
// CMergeDiffDetailView

IMPLEMENT_DYNCREATE(CLocationView, CView)


CLocationView::CLocationView()
	: m_visibleTop(-1)
	, m_visibleBottom(-1)
//	MOVEDLINE_LIST m_movedLines; //*< List of moved block connecting lines */
	, m_hwndFrame(NULL)
	, m_pSavedBackgroundBitmap(NULL)
	, m_bDrawn(false)
	, m_bRecalculateBlocks(TRUE) // calculate for the first time
{
	// NB: set m_bIgnoreTrivials to false to see trivial diffs in the LocationView
	// There is no GUI to do this

	SetConnectMovedBlocks(GetOptionsMgr()->GetInt(OPT_CONNECT_MOVED_BLOCKS));

	m_view[MERGE_VIEW_LEFT] = NULL;
	m_view[MERGE_VIEW_RIGHT] = NULL;
}

CLocationView::~CLocationView()
{
	if (m_pSavedBackgroundBitmap)
		delete m_pSavedBackgroundBitmap;
}

BEGIN_MESSAGE_MAP(CLocationView, CView)
	//{{AFX_MSG_MAP(CLocationView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CLocationView::SetConnectMovedBlocks(int displayMovedBlocks) 
{
	if (m_displayMovedBlocks == displayMovedBlocks)
		return;

	GetOptionsMgr()->SaveOption(OPT_CONNECT_MOVED_BLOCKS, displayMovedBlocks);
	m_displayMovedBlocks = displayMovedBlocks;
	if (this->GetSafeHwnd() != NULL)
		if (IsWindowVisible())
			Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CLocationView diagnostics
#ifdef _DEBUG
CMergeDoc* CLocationView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMergeDoc)));
	return (CMergeDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLocationView message handlers

/** 
 * @brief Update view.
 */
void CLocationView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
	UNREFERENCED_PARAMETER(pSender);
	UNREFERENCED_PARAMETER(lHint);
	CMergeDoc* pDoc = GetDocument();
	m_view[MERGE_VIEW_LEFT] = pDoc->GetLeftView();
	m_view[MERGE_VIEW_RIGHT] = pDoc->GetRightView();

	// Give pointer to MergeEditView
	m_view[MERGE_VIEW_LEFT]->SetLocationView(this);
	m_view[MERGE_VIEW_RIGHT]->SetLocationView(this);

	m_bRecalculateBlocks = TRUE;
	Invalidate();
}

/** 
 * @brief Override for CMemDC to work.
 */
BOOL CLocationView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

/**
 * @brief Draw custom (non-white) background.
 * @param [in] pDC Pointer to draw context.
 */
void CLocationView::DrawBackground(CDC* pDC)
{
	// Set brush to desired background color
	CBrush backBrush(RGB(0xe8, 0xe8, 0xf4));
	
	// Save old brush
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);
	
	CRect rect;
	pDC->GetClipBox(&rect);     // Erase the area needed
	
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);

	pDC->SelectObject(pOldBrush);
}

/**
 * @brief Calculate bar coordinates and scaling factors.
 */
void CLocationView::CalculateBars()
{
	CRect rc;
	GetClientRect(rc);
	const int w = rc.Width() / 4;
	m_leftBar.left = (rc.Width() - 2 * w) / 3;
	m_leftBar.right = m_leftBar.left + w;
	m_rightBar.left = 2 * m_leftBar.left + w;
	m_rightBar.right = m_rightBar.left + w;
	const double hTotal = rc.Height() - (2 * Y_OFFSET); // Height of draw area
	const int nbLines = min(m_view[MERGE_VIEW_LEFT]->GetSubLineCount(),
			m_view[MERGE_VIEW_RIGHT]->GetSubLineCount());

	m_lineInPix = hTotal / nbLines;
	m_pixInLines = nbLines / hTotal;
	if (m_lineInPix > MAX_LINEPIX)
	{
		m_lineInPix = MAX_LINEPIX;
		m_pixInLines = 1 / MAX_LINEPIX;
	}

	m_leftBar.top = Y_OFFSET - 1;
	m_rightBar.top = Y_OFFSET - 1;
	m_leftBar.bottom = (LONG)(m_lineInPix * nbLines + Y_OFFSET + 1);
	m_rightBar.bottom = m_leftBar.bottom;
}

/**
 * @brief Calculate difference lines and coordinates.
 * This function calculates begin- and end-lines of differences when word-wrap
 * is enabled. Otherwise the value from original difflist is used. Line
 * numbers are also converted to coordinates in the window. All calculated
 * (and not ignored) differences are added to the new list.
 */
void CLocationView::CalculateBlocks()
{
	m_diffBlocks.RemoveAll();
	
	CMergeDoc *pDoc = GetDocument();
	const int nDiffs = pDoc->m_diffList.GetSize();

	int nDiff = pDoc->m_diffList.FirstSignificantDiff();
	while (nDiff != -1)
	{
		DIFFRANGE diff;
		VERIFY(pDoc->m_diffList.GetDiff(nDiff, diff));

		// Find end of diff. If first side has blank lines use other side.
		const int nLineEndDiff = (diff.blank0 > 0) ? diff.dend1 : diff.dend0;

		CMergeEditView *pView = m_view[MERGE_VIEW_LEFT];

		// Count how many line does the diff block have.
		const int nBlockStart = pView->GetSubLineIndex(diff.dbegin0);
		const int nBlockEnd = pView->GetSubLineIndex(nLineEndDiff);
		const int nBlockHeight = nBlockEnd - nBlockStart + pView->GetSubLines(nLineEndDiff);

		// Convert diff block size from lines to pixels.
		const int nBeginY = (int)(nBlockStart * m_lineInPix + Y_OFFSET);
		const int nEndY = (int)((nBlockStart + nBlockHeight) * m_lineInPix + Y_OFFSET);

		DiffBlock block;
		block.top_line = diff.dbegin0;
		block.bottom_line = nLineEndDiff;
		block.top_coord = nBeginY;
		block.bottom_coord = nEndY;
		block.diff_index = nDiff;
		m_diffBlocks.AddTail(block);

		nDiff = pDoc->m_diffList.NextSignificantDiff(nDiff);
	}
	m_bRecalculateBlocks = FALSE;
}

/** 
 * @brief Draw maps of files.
 *
 * Draws maps of differences in files. Difference list is walked and
 * every difference is drawn with same colors as in editview.
 * @note We MUST use doubles when calculating coords to avoid rounding
 * to integers. Rounding causes miscalculation of coords.
 * @sa CLocationView::DrawRect()
 */
void CLocationView::OnDraw(CDC* pDC)
{
	ASSERT(m_view[MERGE_VIEW_LEFT] != NULL);
	ASSERT(m_view[MERGE_VIEW_RIGHT] != NULL);

	if (m_view[MERGE_VIEW_LEFT] == NULL || m_view[MERGE_VIEW_RIGHT] == NULL)
		return;

	if (!m_view[MERGE_VIEW_LEFT]->IsInitialized()) return;

	CRect rc;
	GetClientRect(&rc);

	CMemDC dc(pDC, &rc);

	COLORREF cr0 = CLR_NONE; // Left side color
	COLORREF cr1 = CLR_NONE; // Right side color
	COLORREF crt = CLR_NONE; // Text color
	BOOL bwh = FALSE;

	m_movedLines.RemoveAll();

	CalculateBars();
	DrawBackground(&dc);

	// Draw bar outlines
	CPen* oldObj = (CPen*)dc.SelectStockObject(BLACK_PEN);
	dc.Rectangle(m_leftBar);
	dc.Rectangle(m_rightBar);
	dc.SelectObject(oldObj);

	// Iterate the differences list and draw differences as colored blocks.

	// Don't recalculate blocks if we earlier determined it is not needed
	// This may save lots of processing
	if (m_bRecalculateBlocks)
		CalculateBlocks();

	CMergeDoc *pDoc = GetDocument();
	int nPrevEndY = -1;
	const int nCurDiff = pDoc->GetCurrentDiff();

	POSITION pos = m_diffBlocks.GetHeadPosition();

	while (pos)
	{
		const DiffBlock &block = m_diffBlocks.GetNext(pos);
		CMergeEditView *pView = m_view[MERGE_VIEW_LEFT];
		const BOOL bInsideDiff = (nCurDiff == block.diff_index);

		if ((nPrevEndY != block.bottom_coord) || bInsideDiff)
		{
			// Draw left side block
			m_view[MERGE_VIEW_LEFT]->GetLineColors2(block.top_line, 0, cr0, crt, bwh);
			CRect r0(m_leftBar.left, block.top_coord, m_leftBar.right, block.bottom_coord);
			DrawRect(&dc, r0, cr0, bInsideDiff);

			// Draw right side block
			m_view[MERGE_VIEW_RIGHT]->GetLineColors2(block.top_line, 0, cr1, crt, bwh);
			CRect r1(m_rightBar.left, block.top_coord, m_rightBar.right, block.bottom_coord);
			DrawRect(&dc, r1, cr1, bInsideDiff);
		}
		nPrevEndY = block.bottom_coord;

		// Test if we draw a connector
		BOOL bDisplayConnectorFromLeft = FALSE;
		BOOL bDisplayConnectorFromRight = FALSE;

		switch (m_displayMovedBlocks)
		{
		case DISPLAY_MOVED_FOLLOW_DIFF:
			// display moved block only for current diff
			if (!bInsideDiff)
				break;
			// two sides may be linked to a block somewhere else
			bDisplayConnectorFromLeft = TRUE;
			bDisplayConnectorFromRight = TRUE;
			break;
		case DISPLAY_MOVED_ALL:
			// we display all moved blocks, so once direction is enough
			bDisplayConnectorFromLeft = TRUE;
			break;
		default:
			break;
		}

		if (bDisplayConnectorFromLeft)
		{
			int apparent0 = block.top_line;
			int apparent1 = pDoc->RightLineInMovedBlock(apparent0);
			const int nBlockHeight = block.bottom_line - block.top_line;
			if (apparent1 != -1)
			{
				MovedLine line;
				CPoint start;
				CPoint end;

				apparent0 = pView->GetSubLineIndex(apparent0);
				apparent1 = pView->GetSubLineIndex(apparent1);

				start.x = m_leftBar.right;
				int leftUpper = (int) (apparent0 * m_lineInPix + Y_OFFSET);
				int leftLower = (int) ((nBlockHeight + apparent0) * m_lineInPix + Y_OFFSET);
				start.y = leftUpper + (leftLower - leftUpper) / 2;
				end.x = m_rightBar.left;
				int rightUpper = (int) (apparent1 * m_lineInPix + Y_OFFSET);
				int rightLower = (int) ((nBlockHeight + apparent1) * m_lineInPix + Y_OFFSET);
				end.y = rightUpper + (rightLower - rightUpper) / 2;
				line.ptLeft = start;
				line.ptRight = end;
				m_movedLines.AddTail(line);
			}
		}

		if (bDisplayConnectorFromRight)
		{
			int apparent1 = block.top_line;
			int apparent0 = pDoc->LeftLineInMovedBlock(apparent1);
			const int nBlockHeight = block.bottom_line - block.top_line;
			if (apparent0 != -1)
			{
				MovedLine line;
				CPoint start;
				CPoint end;

				apparent0 = pView->GetSubLineIndex(apparent0);
				apparent1 = pView->GetSubLineIndex(apparent1);

				start.x = m_leftBar.right;
				int leftUpper = (int) (apparent0 * m_lineInPix + Y_OFFSET);
				int leftLower = (int) ((nBlockHeight + apparent0) * m_lineInPix + Y_OFFSET);
				start.y = leftUpper + (leftLower - leftUpper) / 2;
				end.x = m_rightBar.left;
				int rightUpper = (int) (apparent1 * m_lineInPix + Y_OFFSET);
				int rightLower = (int) ((nBlockHeight + apparent1) * m_lineInPix + Y_OFFSET);
				end.y = rightUpper + (rightLower - rightUpper) / 2;
				line.ptLeft = start;
				line.ptRight = end;
				m_movedLines.AddTail(line);
			}
		}
	}

	if (m_displayMovedBlocks != DISPLAY_MOVED_NONE)
		DrawConnectLines(&dc);

	if (m_pSavedBackgroundBitmap)
		delete m_pSavedBackgroundBitmap;
	m_pSavedBackgroundBitmap = CopyRectToBitmap(&dc, rc);

	// Since we have invalidated locationbar there is no previous
	// arearect to remove
	m_visibleTop = -1;
	m_visibleBottom = -1;
	DrawVisibleAreaRect(&dc);

	m_bDrawn = true;
}

/** 
 * @brief Draw one block of map.
 * @param [in] pDC Draw context.
 * @param [in] r Rectangle to draw.
 * @param [in] cr Color for rectangle.
 * @param [in] bSelected Is rectangle for selected difference?
 */
void CLocationView::DrawRect(CDC* pDC, const CRect& r, COLORREF cr, BOOL bSelected)
{
	// Draw only colored blocks
	if (cr != CLR_NONE && cr != GetSysColor(COLOR_WINDOW))
	{
		CBrush brush(cr);
		CRect drawRect(r);
		drawRect.DeflateRect(1, 0);

		// With long files and small difference areas rect may become 0-height.
		// Make sure all diffs are drawn at least one pixel height.
		if (drawRect.Height() < 1)
			++drawRect.bottom;
		pDC->FillSolidRect(drawRect, cr);

		if (bSelected)
		{
			DrawDiffMarker(pDC, r.top);
		}
	}
}

/**
 * @brief Capture the mouse target.
 */
void CLocationView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();

	if (!GotoLocation(point, FALSE))
		CView::OnLButtonDown(nFlags, point);
}

/**
 * @brief Release the mouse target.
 */
void CLocationView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();

	CView::OnLButtonUp(nFlags, point);
}

/**
 * @brief Process drag action on a captured mouse.
 *
 * Reposition on every dragged movement.
 * The Screen update stress will be similar to a mouse wheeling.:-)
 */
void CLocationView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetCapture() == this)
	{
		// Don't go above bars.
		point.y = max(point.y, Y_OFFSET);

		// Vertical scroll handlers are range safe, so there is no need to
		// make sure value is valid and in range.
		int nSubLine = (int) (m_pixInLines * (point.y - Y_OFFSET));
		nSubLine -= m_view[0]->GetScreenLines() / 2;
		if (nSubLine < 0)
			nSubLine = 0;

		// Just a random choose as both view share the same scroll bar.
		CWnd *pView = m_view[MERGE_VIEW_LEFT];

		SCROLLINFO si = {0};
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = nSubLine;
		pView->SetScrollInfo(SB_VERT, &si);

		// The views are child windows of a splitter windows. Splitter window
		// doesn't accept scroll bar updates not send from scroll bar control.
		// So we need to update both views.
		pView->SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, 0), NULL);
		pView = m_view[MERGE_VIEW_RIGHT];
		pView->SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, 0), NULL);
	}

	CView::OnMouseMove(nFlags, point);
}

/**
 * User left double-clicked mouse
 * @todo We can give alternative action to a double clicking. 
 */
void CLocationView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (!GotoLocation(point, FALSE))
		CView::OnLButtonDblClk(nFlags, point);
}

/**
 * @brief Scroll both views to point given.
 *
 * Scroll views to given line. There is two ways to scroll, based on
 * view lines (ghost lines counted in) or on real lines (no ghost lines).
 * In most cases view lines should be used as it avoids real line number
 * calculation and is able to scroll to all lines - real line numbers
 * cannot be used to scroll to ghost lines.
 *
 * @param [in] point Point to move to
 * @param [in] bRealLine TRUE if we want to scroll using real line num,
 * FALSE if view linenumbers are OK.
 * @return TRUE if succeeds, FALSE if point not inside bars.
 */
BOOL CLocationView::GotoLocation(const CPoint& point, BOOL bRealLine)
{
	CRect rc;
	GetClientRect(rc);

	int line = -1;
	int lineOther = -1;
	int bar = IsInsideBar(rc, point);
	if (bar == BAR_LEFT || bar == BAR_RIGHT)
	{
		line = GetLineFromYPos(point.y, bar, bRealLine);
	}
	else if (bar == BAR_YAREA)
	{
		// Outside bars, use left bar
		bar = BAR_LEFT;
		line = GetLineFromYPos(point.y, bar, FALSE);
	}
	else
		return FALSE;

	m_view[MERGE_VIEW_LEFT]->GotoLine(line, bRealLine, bar);
	if (bar == BAR_LEFT || bar == BAR_RIGHT)
		m_view[bar]->SetFocus();

	return TRUE;
}

/**
 * @brief Handle scroll events sent directly.
 *
 */
void CLocationView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	if (pScrollBar == NULL)
	{
		// Scroll did not come frome a scroll bar
		// Send it to the right view instead
 	  CMergeDoc *pDoc = GetDocument();
		pDoc->GetRightView()->SendMessage(WM_VSCROLL,
			MAKELONG(nSBCode, nPos), (LPARAM)NULL);
		return;
	}
	CView::OnVScroll (nSBCode, nPos, pScrollBar);
}

/**
 * Show context menu and handle user selection.
 */
void CLocationView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (point.x == -1 && point.y == -1)
	{
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);

		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	CRect rc;
	CPoint pt = point;
	GetClientRect(rc);
	ScreenToClient(&pt);
	BCMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_LOCATIONBAR));
	theApp.TranslateMenu(menu.m_hMenu);

	BCMenu* pPopup = (BCMenu *) menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CCmdUI cmdUI;
	cmdUI.m_pMenu = pPopup;
	cmdUI.m_nIndexMax = cmdUI.m_pMenu->GetMenuItemCount();
	for (cmdUI.m_nIndex = 0 ; cmdUI.m_nIndex < cmdUI.m_nIndexMax ; ++cmdUI.m_nIndex)
	{
		cmdUI.m_nID = cmdUI.m_pMenu->GetMenuItemID(cmdUI.m_nIndex);
		switch (cmdUI.m_nID)
		{
		case ID_DISPLAY_MOVED_NONE:
			cmdUI.SetRadio(m_displayMovedBlocks == DISPLAY_MOVED_NONE);
			break;
		case ID_DISPLAY_MOVED_ALL:
			cmdUI.SetRadio(m_displayMovedBlocks == DISPLAY_MOVED_ALL);
			break;
		case ID_DISPLAY_MOVED_FOLLOW_DIFF:
			cmdUI.SetRadio(m_displayMovedBlocks == DISPLAY_MOVED_FOLLOW_DIFF);
			break;
		}
	}

	CString strItem;
	CString strNum;
	int nLine = -1;
	int bar = IsInsideBar(rc, pt);

	// If cursor over bar, format string with linenumber, else disable item
	if (bar != BAR_NONE)
	{
		// If outside bar area use left bar
		if (bar == BAR_YAREA)
			bar = BAR_LEFT;
		nLine = GetLineFromYPos(pt.y, bar);
		strNum.Format(_T("%d"), nLine + 1); // Show linenumber not lineindex
	}
	else
		pPopup->EnableMenuItem(ID_LOCBAR_GOTODIFF, MF_GRAYED);
	LangFormatString1(strItem, ID_LOCBAR_GOTOLINE_FMT, strNum);
	pPopup->SetMenuText(ID_LOCBAR_GOTODIFF, strItem, MF_BYCOMMAND);

	// invoke context menu
	// we don't want to use the main application handlers, so we use flags TPM_NONOTIFY | TPM_RETURNCMD
	// and handle the command after TrackPopupMenu
	int command = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY  | TPM_RETURNCMD, point.x, point.y, AfxGetMainWnd());

	CMergeDoc* pDoc = GetDocument();
	switch (command)
	{
	case ID_LOCBAR_GOTODIFF:
		m_view[MERGE_VIEW_LEFT]->GotoLine(nLine, TRUE, bar);
	if (bar == BAR_LEFT || bar == BAR_RIGHT)
		m_view[bar]->SetFocus();
		break;
	case ID_EDIT_WMGOTO:
		m_view[MERGE_VIEW_LEFT]->WMGoto();
		break;
	case ID_DISPLAY_MOVED_NONE:
		SetConnectMovedBlocks(DISPLAY_MOVED_NONE);
		pDoc->SetDetectMovedBlocks(FALSE);
		break;
	case ID_DISPLAY_MOVED_ALL:
		SetConnectMovedBlocks(DISPLAY_MOVED_ALL);
		pDoc->SetDetectMovedBlocks(TRUE);
		break;
	case ID_DISPLAY_MOVED_FOLLOW_DIFF:
		SetConnectMovedBlocks(DISPLAY_MOVED_FOLLOW_DIFF);
		pDoc->SetDetectMovedBlocks(TRUE);
		break;
	}
}

/** 
 * @brief Calculates view/real line in file from given YCoord in bar.
 * @param [in] nYCoord ycoord in pane
 * @param [in] bar bar/file
 * @param [in] bRealLine TRUE if real line is returned, FALSE for view line
 * @return 0-based index of view/real line in file [0...lines-1]
 */
int CLocationView::GetLineFromYPos(int nYCoord, int bar, BOOL bRealLine)
{
	CMergeEditView *pView = (bar == BAR_LEFT) ? m_view[MERGE_VIEW_LEFT] : 
		m_view[MERGE_VIEW_RIGHT];

	int nSubLineIndex = (int) (m_pixInLines * (nYCoord - Y_OFFSET));

	// Keep sub-line index in range.
	if (nSubLineIndex < 0)
	{
		nSubLineIndex = 0;
	}
	else if (nSubLineIndex >= pView->GetSubLineCount())
	{
		nSubLineIndex = pView->GetSubLineCount() - 1;
	}

	// Find the real (not wrapped) line number from sub-line index.
	int nLine = 0;
	int nSubLine = 0;
	pView->GetLineBySubLine(nSubLineIndex, nLine, nSubLine);

	// Convert line number to line index.
	if (nLine > 0)
	{
		nLine -= 1;
	}

	// We've got a view line now
	if (bRealLine == FALSE)
		return nLine;

	// Get real line (exclude ghost lines)
	CMergeDoc* pDoc = GetDocument();
	const int nRealLine = pDoc->m_ptBuf[bar]->ComputeRealLine(nLine);
	return nRealLine;
}

/** 
 * @brief Determines if given coords are inside left/right bar.
 * @param rc [in] size of locationpane client area
 * @param pt [in] point we want to check, in client coordinates.
 * @return LOCBAR_TYPE area where point is.
 */
int CLocationView::IsInsideBar(const CRect& rc, const POINT& pt)
{
	int retVal = BAR_NONE;

	if (m_leftBar.PtInRect(pt))
		retVal = BAR_LEFT;
	else if (m_rightBar.PtInRect(pt))
		retVal = BAR_RIGHT;
	else if (pt.x >= INDICATOR_MARGIN && pt.x < (rc.Width() - INDICATOR_MARGIN) &&
		pt.y > m_leftBar.top && pt.y <= m_leftBar.bottom)
	{
		retVal = BAR_YAREA;
	}

	return retVal;
}

/** 
 * @brief Draws rect indicating visible area in file views.
 *
 * @param [in] nTopLine New topline for indicator
 * @param [in] nBottomLine New bottomline for indicator
 * @todo This function dublicates too much DrawRect() code.
 */
void CLocationView::DrawVisibleAreaRect(CDC *pClientDC, int nTopLine, int nBottomLine)
{
	CMergeDoc* pDoc = GetDocument();
	if (nTopLine == -1)
		nTopLine = pDoc->GetRightView()->GetTopSubLine();
	
	if (nBottomLine == -1)
	{
		const int nScreenLines = pDoc->GetRightView()->GetScreenLines();
		nBottomLine = nTopLine + nScreenLines;
	}

	CRect rc;
	GetClientRect(rc);
	const double hTotal = rc.Height() - (2 * Y_OFFSET); // Height of draw area
	const int nbLines = min(m_view[MERGE_VIEW_LEFT]->GetSubLineCount(),
			m_view[MERGE_VIEW_RIGHT]->GetSubLineCount());

	int nTopCoord = static_cast<int>(Y_OFFSET +
			(static_cast<double>(nTopLine * m_lineInPix)));
	int nLeftCoord = INDICATOR_MARGIN;
	int nBottomCoord = static_cast<int>(Y_OFFSET +
			(static_cast<double>(nBottomLine * m_lineInPix)));
	int nRightCoord = rc.Width() - INDICATOR_MARGIN;
	
	double xbarBottom = min(nbLines / m_pixInLines + Y_OFFSET, rc.Height() - Y_OFFSET);
	int barBottom = (int)xbarBottom;
	// Make sure bottom coord is in bar range
	nBottomCoord = min(nBottomCoord, barBottom);

	// Ensure visible area is at least minimum height
	if (nBottomCoord - nTopCoord < INDICATOR_MIN_HEIGHT)
	{
		// If area is near top of file, add additional area to bottom
		// of the bar and vice versa.
		if (nTopCoord < Y_OFFSET + 20)
			nBottomCoord += INDICATOR_MIN_HEIGHT - (nBottomCoord - nTopCoord);
		else
			nTopCoord -= INDICATOR_MIN_HEIGHT - (nBottomCoord - nTopCoord);
	}

	// Store current values for later use (to check if area changes)
	m_visibleTop = nTopCoord;
	m_visibleBottom = nBottomCoord;

	CRect rcVisibleArea(2, m_visibleTop, rc.right - 2, m_visibleBottom);
	CBitmap *pBitmap = CopyRectToBitmap(pClientDC, rcVisibleArea);
	CBitmap *pDarkenedBitmap = GetDarkenedBitmap(pClientDC, pBitmap);
	DrawBitmap(pClientDC, rcVisibleArea.left, rcVisibleArea.top, pDarkenedBitmap);
	delete pDarkenedBitmap;
	delete pBitmap;
}

/**
 * @brief Public function for updating visible area indicator.
 *
 * @param [in] nTopLine New topline for indicator
 * @param [in] nBottomLine New bottomline for indicator
 */
void CLocationView::UpdateVisiblePos(int nTopLine, int nBottomLine)
{
	if (m_bDrawn)
	{
		int nTopCoord = static_cast<int>(Y_OFFSET +
				(static_cast<double>(nTopLine * m_lineInPix)));
		int nBottomCoord = static_cast<int>(Y_OFFSET +
				(static_cast<double>(nBottomLine * m_lineInPix)));
		if (m_visibleTop != nTopCoord || m_visibleBottom != nBottomCoord)
		{
			// Visible area was changed
			CDC *pDC = GetDC();
			if (m_pSavedBackgroundBitmap)
			{
				CMemDC dc(pDC);
				// Clear previous visible rect
				DrawBitmap(&dc, 0, 0, m_pSavedBackgroundBitmap);

				DrawVisibleAreaRect(&dc, nTopLine, nBottomLine);
			}
			ReleaseDC(pDC);
		}
	}
}

/**
 * @brief Unset pointers to MergeEditView when location pane is closed.
 */
void CLocationView::OnClose()
{
	m_view[MERGE_VIEW_LEFT]->SetLocationView(NULL);
	m_view[MERGE_VIEW_RIGHT]->SetLocationView(NULL);

	CView::OnClose();
}

/** 
 * @brief Draw lines connecting moved blocks.
 */
void CLocationView::DrawConnectLines(CDC *pClientDC)
{
	CPen* oldObj = (CPen*)pClientDC->SelectStockObject(BLACK_PEN);

	POSITION pos = m_movedLines.GetHeadPosition();
	while (pos != NULL)
	{
		MovedLine line = m_movedLines.GetNext(pos);
		pClientDC->MoveTo(line.ptLeft.x, line.ptLeft.y);
		pClientDC->LineTo(line.ptRight.x, line.ptRight.y);
	}

	pClientDC->SelectObject(oldObj);
}

/** 
 * @brief Stores HWND of frame window (CChildFrame).
 */
void CLocationView::SetFrameHwnd(HWND hwndFrame)
{
	m_hwndFrame = hwndFrame;
}

/** 
 * @brief Request frame window to store sizes.
 *
 * When locationview size changes we want to save new size
 * for new windows. But we must do it through frame window.
 * @param [in] nType Type of resizing, SIZE_MAXIMIZED etc.
 * @param [in] cx New panel width.
 * @param [in] cy New panel height.
 */
void CLocationView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	// Height change needs block recalculation
	// TODO: Perhaps this should be determined from need to change bar size?
	// And we could change bar sizes more lazily, not from every one pixel change in size?
	if (cy != m_currentSize.cy)
		m_bRecalculateBlocks = TRUE;

	if (cx != m_currentSize.cx)
	{
		if (m_hwndFrame != NULL)
			::PostMessage(m_hwndFrame, MSG_STORE_PANESIZES, 0, 0);
	}

	m_currentSize.cx = cx;
	m_currentSize.cy = cy;
}

/** 
 * @brief Draw marker for top of currently selected difference.
 * This function draws marker for top of currently selected difference.
 * This marker makes it a lot easier to see where currently selected
 * difference is in location bar. Especially when selected diffence is
 * small and it is not easy to find it otherwise.
 * @param [in] pDC Pointer to draw context.
 * @param [in] yCoord Y-coord of top of difference, -1 if no difference.
 */
void CLocationView::DrawDiffMarker(CDC* pDC, int yCoord)
{
	CPoint points[3];
	points[0].x = m_leftBar.left - DIFFMARKER_WIDTH - 1;
	points[0].y = yCoord - DIFFMARKER_TOP;
	points[1].x = m_leftBar.left - 1;
	points[1].y = yCoord;
	points[2].x = m_leftBar.left - DIFFMARKER_WIDTH - 1;
	points[2].y = yCoord + DIFFMARKER_BOTTOM;

	CPen* oldObj = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush brushBlue(RGB(0x80, 0x80, 0xff));
	CBrush* pOldBrush = pDC->SelectObject(&brushBlue);

	pDC->SetPolyFillMode(WINDING);
	pDC->Polygon(points, 3);

	points[0].x = m_rightBar.right + 1 + DIFFMARKER_WIDTH;
	points[1].x = m_rightBar.right + 1;
	points[2].x = m_rightBar.right + 1 + DIFFMARKER_WIDTH;
	pDC->Polygon(points, 3);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(oldObj);
}
