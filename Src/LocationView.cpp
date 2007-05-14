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

// RCS ID line follows -- this is updated by CVS
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
	, m_bIgnoreTrivials(true)
	, m_hwndFrame(NULL)
	, m_nPrevPaneWidth(0)
	, m_DiffMarkerCoord(-1)
{
	// NB: set m_bIgnoreTrivials to false to see trivial diffs in the LocationView
	// There is no GUI to do this

	SetConnectMovedBlocks(GetOptionsMgr()->GetInt(OPT_CONNECT_MOVED_BLOCKS));

	m_view[MERGE_VIEW_LEFT] = NULL;
	m_view[MERGE_VIEW_RIGHT] = NULL;
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
	m_view[MERGE_VIEW_LEFT]->SetLocationView(GetSafeHwnd(), this);
	m_view[MERGE_VIEW_RIGHT]->SetLocationView(GetSafeHwnd(), this);

	Invalidate();
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
	GetClientRect(rc);

	CMergeDoc *pDoc = GetDocument();
	const int w = rc.Width() / 4;
	m_nLeftBarLeft = (rc.Width() - 2 * w) / 3;
	m_nLeftBarRight = m_nLeftBarLeft + w;
	m_nRightBarLeft = 2 * m_nLeftBarLeft + w;
	m_nRightBarRight = m_nRightBarLeft + w;
	const double hTotal = rc.Height() - (2 * Y_OFFSET); // Height of draw area
	const int nbLines = min(m_view[MERGE_VIEW_LEFT]->GetSubLineCount(),
			m_view[MERGE_VIEW_RIGHT]->GetSubLineCount());

	double LineInPix = hTotal / nbLines;
	COLORREF cr0 = CLR_NONE; // Left side color
	COLORREF cr1 = CLR_NONE; // Right side color
	COLORREF crt = CLR_NONE; // Text color
	BOOL bwh = FALSE;

	m_pixInLines = nbLines / hTotal;
	if (LineInPix > MAX_LINEPIX)
	{
		LineInPix = MAX_LINEPIX;
		m_pixInLines = 1 / MAX_LINEPIX;
	}

	// Since we have invalidated locationbar there is no previous
	// arearect to remove
	m_visibleTop = -1;
	m_visibleBottom = -1;
	DrawVisibleAreaRect();
	m_movedLines.RemoveAll();

	// Adjust line coloring if ignoring trivials
	DWORD ignoreFlags = (m_bIgnoreTrivials ? LF_TRIVIAL : 0);

	// Draw bar outlines
	CPen* oldObj = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	const int nBottom = (int)(LineInPix * nbLines + Y_OFFSET + 1);
	pDC->Rectangle(m_nLeftBarLeft, Y_OFFSET - 1, m_nLeftBarRight, nBottom);
	pDC->Rectangle(m_nRightBarLeft, Y_OFFSET - 1, m_nRightBarRight, nBottom);
	pDC->SelectObject(oldObj);

	// Iterate the differences list and draw differences as colored blocks.
	for (int nDiff = 0; nDiff < pDoc->m_diffList.GetSize(); ++nDiff)
	{
		DIFFRANGE diff;
		VERIFY(pDoc->m_diffList.GetDiff(nDiff, diff));

		// Skip trivial differences.
		if (m_bIgnoreTrivials && diff.op == OP_TRIVIAL)
		{
			continue;
		}

		// Find end of diff. If first side has blank lines use other side.
		const int nLineEndDiff = (diff.blank0 > 0) ? diff.dend1 : diff.dend0;

		CMergeEditView *pView = m_view[MERGE_VIEW_LEFT];

		// Count how many line does the diff block have.
		const int nBlockStart = pView->GetSubLineIndex(diff.dbegin0);
		const int nBlockEnd = pView->GetSubLineIndex(nLineEndDiff);
		const int nBlockHeight = nBlockEnd - nBlockStart + pView->GetSubLines(nLineEndDiff);

		// Convert diff block size from lines to pixels.
		const int nBeginY = (int)(nBlockStart * LineInPix + Y_OFFSET);
		const int nEndY = (int)((nBlockStart + nBlockHeight) * LineInPix + Y_OFFSET);
		
		// If no selected diff, remove diff marker
		if (pDoc->GetCurrentDiff() == -1)
			DrawDiffMarker(pDC, -1);

		const BOOL bInsideDiff = pView->IsLineInCurrentDiff(diff.dbegin0);

		// Draw left side block
		m_view[MERGE_VIEW_LEFT]->GetLineColors2(diff.dbegin0, ignoreFlags, cr0, crt, bwh);
		CRect r0(m_nLeftBarLeft, nBeginY, m_nLeftBarRight, nEndY);
		DrawRect(pDC, r0, cr0, bInsideDiff);

		// Draw right side block
		m_view[MERGE_VIEW_RIGHT]->GetLineColors2(diff.dbegin0, ignoreFlags, cr1, crt, bwh);
		CRect r1(m_nRightBarLeft, nBeginY, m_nRightBarRight, nEndY);
		DrawRect(pDC, r1, cr1, bInsideDiff);

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
			int apparent0 = diff.dbegin0;
			int apparent1 = pDoc->RightLineInMovedBlock(apparent0);
			if (apparent1 != -1)
			{
				MovedLine line;
				CPoint start;
				CPoint end;

				apparent0 = pView->GetSubLineIndex(apparent0);
				apparent1 = pView->GetSubLineIndex(apparent1);

				start.x = m_nLeftBarRight;
				int leftUpper = (int) (apparent0 * LineInPix + Y_OFFSET);
				int leftLower = (int) ((nBlockHeight + apparent0) * LineInPix + Y_OFFSET);
				start.y = leftUpper + (leftLower - leftUpper) / 2;
				end.x = m_nRightBarLeft;
				int rightUpper = (int) (apparent1 * LineInPix + Y_OFFSET);
				int rightLower = (int) ((nBlockHeight + apparent1) * LineInPix + Y_OFFSET);
				end.y = rightUpper + (rightLower - rightUpper) / 2;
				line.ptLeft = start;
				line.ptRight = end;
				m_movedLines.AddTail(line);
			}
		}

		if (bDisplayConnectorFromRight)
		{
			int apparent1 = diff.dbegin0;
			int apparent0 = pDoc->LeftLineInMovedBlock(apparent1);
			if (apparent0 != -1)
			{
				MovedLine line;
				CPoint start;
				CPoint end;

				apparent0 = pView->GetSubLineIndex(apparent0);
				apparent1 = pView->GetSubLineIndex(apparent1);

				start.x = m_nLeftBarRight;
				int leftUpper = (int) (apparent0 * LineInPix + Y_OFFSET);
				int leftLower = (int) ((nBlockHeight + apparent0) * LineInPix + Y_OFFSET);
				start.y = leftUpper + (leftLower - leftUpper) / 2;
				end.x = m_nRightBarLeft;
				int rightUpper = (int) (apparent1 * LineInPix + Y_OFFSET);
				int rightLower = (int) ((nBlockHeight + apparent1) * LineInPix + Y_OFFSET);
				end.y = rightUpper + (rightLower - rightUpper) / 2;
				line.ptLeft = start;
				line.ptRight = end;
				m_movedLines.AddTail(line);
			}
		}
	}

	if (m_displayMovedBlocks != DISPLAY_MOVED_NONE)
		DrawConnectLines();
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
		const int nSubLine = (int) (m_pixInLines * (point.y - Y_OFFSET));

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
	AfxFormatString1(strItem, ID_LOCBAR_GOTOLINE_FMT, strNum);
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
	BOOL bLeftSide = FALSE;
	BOOL bRightSide = FALSE;
	BOOL bYarea = FALSE;
	const int w = rc.Width() / 4;
	const int x = (rc.Width() - 2 * w) / 3;
	const int nbLines = min(m_view[MERGE_VIEW_LEFT]->GetSubLineCount(),
			m_view[MERGE_VIEW_RIGHT]->GetSubLineCount());
	// We need '1 / m_pixInLines' to get line in pixels and
	// that multiplied by linecount gives us bottom coord for bars.
	double xbarBottom = min(nbLines / m_pixInLines + Y_OFFSET, rc.Height() - Y_OFFSET);
	int barBottom = (int)xbarBottom;

	if ((pt.y > Y_OFFSET) && (pt.y <= barBottom))
	{
		bLeftSide = (pt.x >= x && pt.x < x + w);
		bRightSide = (pt.x >= 2 * x + w && pt.x < 2 * x + 2 * w);
		bYarea = (pt.x >= INDICATOR_MARGIN &&
			pt.x < (rc.Width() - INDICATOR_MARGIN));
	}
	
	if (bLeftSide)
		retVal = BAR_LEFT;
	else if (bRightSide)
		retVal = BAR_RIGHT;
	else if (bYarea)
		retVal = BAR_YAREA;

	return retVal;
}

/** 
 * @brief Draws rect indicating visible area in file views.
 *
 * @param [in] nTopLine New topline for indicator
 * @param [in] nBottomLine New bottomline for indicator
 * @todo This function dublicates too much DrawRect() code.
 */
void CLocationView::DrawVisibleAreaRect(int nTopLine, int nBottomLine)
{
	CMergeDoc* pDoc = GetDocument();
	const DWORD areaColor = GetSysColor(COLOR_3DSHADOW);
	const DWORD bkColor = GetSysColor(COLOR_WINDOW);
	
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
	double LineInPix = hTotal / nbLines;
	if (LineInPix > MAX_LINEPIX)
		LineInPix = MAX_LINEPIX;

	int nTopCoord = static_cast<int>(Y_OFFSET +
			(static_cast<double>(nTopLine * LineInPix)));
	int nLeftCoord = INDICATOR_MARGIN;
	int nBottomCoord = static_cast<int>(Y_OFFSET +
			(static_cast<double>(nBottomLine * LineInPix)));
	int nRightCoord = rc.Width() - INDICATOR_MARGIN;
	
	// Visible area was not changed
	if (m_visibleTop == nTopCoord && m_visibleBottom == nBottomCoord)
		return;

	// Clear previous visible rect
	if (m_visibleTop != -1 && m_visibleBottom != -1)
	{
		CDC *pClientDC = GetDC();
		CRect rcVisibleArea(2, m_visibleTop, m_nLeftBarLeft - 2, m_visibleBottom);
		pClientDC->FillSolidRect(rcVisibleArea, bkColor);
		rcVisibleArea.left = m_nLeftBarRight + 2;
		rcVisibleArea.right = m_nRightBarLeft - 2;
		pClientDC->FillSolidRect(rcVisibleArea, bkColor);
		rcVisibleArea.left = m_nRightBarRight + 2;
		rcVisibleArea.right = rc.Width() - 2;
		pClientDC->FillSolidRect(rcVisibleArea, bkColor);
		if (((m_DiffMarkerCoord - DIFFMARKER_TOP >= m_visibleTop) &&
			(m_DiffMarkerCoord - DIFFMARKER_TOP <= m_visibleBottom)) ||
			((m_DiffMarkerCoord + DIFFMARKER_BOTTOM >= m_visibleTop) &&
			(m_DiffMarkerCoord + DIFFMARKER_BOTTOM <= m_visibleBottom)))
		{
			DrawDiffMarker(pClientDC, m_DiffMarkerCoord);
		}
		ReleaseDC(pClientDC);
	}

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

	CDC *pClientDC = GetDC();
	CRect rcVisibleArea(2, m_visibleTop, m_nLeftBarLeft - 2, m_visibleBottom);
	pClientDC->FillSolidRect(rcVisibleArea, areaColor);
	rcVisibleArea.left = m_nLeftBarRight + 2;
	rcVisibleArea.right = m_nRightBarLeft - 2;
	pClientDC->FillSolidRect(rcVisibleArea, areaColor);
	rcVisibleArea.left = m_nRightBarRight + 2;
	rcVisibleArea.right = rc.Width() - 2;
	pClientDC->FillSolidRect(rcVisibleArea, areaColor);

	if (((m_DiffMarkerCoord - DIFFMARKER_TOP >= m_visibleTop) &&
		(m_DiffMarkerCoord - DIFFMARKER_TOP <= m_visibleBottom)) ||
		((m_DiffMarkerCoord + DIFFMARKER_BOTTOM >= m_visibleTop) &&
		(m_DiffMarkerCoord + DIFFMARKER_BOTTOM <= m_visibleBottom)))
	{
		DrawDiffMarker(pClientDC, m_DiffMarkerCoord);
	}
	ReleaseDC(pClientDC);
}

/**
 * @brief Public function for updating visible area indicator.
 *
 * @param [in] nTopLine New topline for indicator
 * @param [in] nBottomLine New bottomline for indicator
 */
void CLocationView::UpdateVisiblePos(int nTopLine, int nBottomLine)
{
	DrawVisibleAreaRect(nTopLine, nBottomLine);
	if (m_displayMovedBlocks != DISPLAY_MOVED_NONE)
		DrawConnectLines();
}

/**
 * @brief Unset pointers to MergeEditView when location pane is closed.
 */
void CLocationView::OnClose()
{
	m_view[MERGE_VIEW_LEFT]->SetLocationView(NULL, NULL);
	m_view[MERGE_VIEW_RIGHT]->SetLocationView(NULL, NULL);

	CView::OnClose();
}

/** 
 * @brief Draw lines connecting moved blocks.
 */
void CLocationView::DrawConnectLines()
{
	CDC *pClientDC = GetDC();
	CPen* oldObj = (CPen*)pClientDC->SelectStockObject(BLACK_PEN);

	POSITION pos = m_movedLines.GetHeadPosition();
	while (pos != NULL)
	{
		MovedLine line = m_movedLines.GetNext(pos);
		pClientDC->MoveTo(line.ptLeft.x, line.ptLeft.y);
		pClientDC->LineTo(line.ptRight.x, line.ptRight.y);
	}

	pClientDC->SelectObject(oldObj);
	ReleaseDC(pClientDC);
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
 */
void CLocationView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	if (cx != m_nPrevPaneWidth)
	{
		m_nPrevPaneWidth = cx;
		if (m_hwndFrame != NULL)
			::PostMessage(m_hwndFrame, MSG_STORE_PANESIZES, 0, 0);
	}
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
	// First erase marker from current position
	if (m_DiffMarkerCoord != -1)
	{
		// If in visible area, use its background color
		COLORREF cr;
		if (m_DiffMarkerCoord > m_visibleTop && m_DiffMarkerCoord < m_visibleBottom)
			cr = GetSysColor(COLOR_3DSHADOW);
		else
			cr = GetSysColor(COLOR_WINDOW);

		CRect rect;
		rect.left = m_nLeftBarLeft - DIFFMARKER_WIDTH - 1;
		rect.top = m_DiffMarkerCoord - DIFFMARKER_TOP;
		rect.right = m_nLeftBarLeft;
		rect.bottom = m_DiffMarkerCoord + DIFFMARKER_BOTTOM + 1;
		pDC->FillSolidRect(rect, cr);

		rect.left = m_nRightBarRight;
		rect.right = m_nRightBarRight + 2 + DIFFMARKER_WIDTH;
		pDC->FillSolidRect(rect, cr);
	}

	m_DiffMarkerCoord = yCoord;

	// Then draw marker to new position
	if (yCoord != -1)
	{
		CPoint points[3];
		points[0].x = m_nLeftBarLeft - DIFFMARKER_WIDTH - 1;
		points[0].y = yCoord - DIFFMARKER_TOP;
		points[1].x = m_nLeftBarLeft - 1;
		points[1].y = yCoord;
		points[2].x = m_nLeftBarLeft - DIFFMARKER_WIDTH - 1;
		points[2].y = yCoord + DIFFMARKER_BOTTOM;

		CPen* oldObj = (CPen*)pDC->SelectStockObject(BLACK_PEN);
		CBrush brushBlack(RGB(0, 0, 0));
		CBrush* pOldBrush = pDC->SelectObject(&brushBlack);

		pDC->SetPolyFillMode(WINDING);
		pDC->Polygon(points, 3);

		points[0].x = m_nRightBarRight + 1 + DIFFMARKER_WIDTH;
		points[1].x = m_nRightBarRight + 1;
		points[2].x = m_nRightBarRight + 1 + DIFFMARKER_WIDTH;
		pDC->Polygon(points, 3);

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(oldObj);
	}
}
