//////////////////////////////////////////////////////////////////////
/** 
 * @file  LocationView.cpp
 *
 * @brief Implementation file for CLocationView
 *
 */
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "merge.h"
#include "MainFrm.h"
#include "MergeEditView.h"
#include "LocationView.h"
#include "MergeDoc.h"
#include "BCMenu.h"
#include "OptionsDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** 
 * @brief Size of empty frame above and below bars (in pixels)
 */
static const DWORD Y_OFFSET = 5;

/** 
 * @brief Size of y-margin for visible area indicator (in pixels)
 */
static const UINT INDICATOR_MARGIN = 2;

/** 
 * @brief Max pixels in view per line in file
 */
static const double MAX_LINEPIX = 4.0;

/** 
 * @brief Bars in location pane
 */
enum LOCBAR_TYPE
{
	BAR_NONE = 0,	/**< No bar in given coords */
	BAR_LEFT,		/**< Left side bar in given coords */
	BAR_RIGHT,		/**< Right side bar in given coords */
	BAR_YAREA,		/**< Y-Coord in bar area */
};

/////////////////////////////////////////////////////////////////////////////
// CMergeDiffDetailView

IMPLEMENT_DYNCREATE(CLocationView, CView)


CLocationView::CLocationView()
	: m_view0(0)
	, m_view1(0)
	, m_visibleTop(-1)
	, m_visibleBottom(-1)
{
	SetConnectMovedBlocks(mf->m_options.GetInt(OPT_CONNECT_MOVED_BLOCKS));
}

BEGIN_MESSAGE_MAP(CLocationView, CView)
	//{{AFX_MSG_MAP(CLocationView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CLocationView::SetConnectMovedBlocks(int displayMovedBlocks) 
{
	if (m_displayMovedBlocks == displayMovedBlocks)
		return;

	mf->m_options.SaveOption(OPT_CONNECT_MOVED_BLOCKS, displayMovedBlocks);
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
	CMergeDoc* pDoc = GetDocument();
	m_view0 = pDoc->GetLeftView();
	m_view1 = pDoc->GetRightView();

	// Give pointer to MergeEditView
	m_view0->m_pLocationView = this;
	m_view1->m_pLocationView = this;

	Invalidate();
}

/** 
 * @brief Draw maps of files.
 *
 * Draws maps of differences in files. Difference list is walked and
 * every difference is drawn with same colors than in editview.
 * @note We MUST use doubles when calculating coords to avoid rounding
 * to integers. Rounding causes miscalculation of coords.
 * @todo Use of GetNextRect() is inefficient, it reads diffs sometimes
 * twice when it first asks next diff when line is not in any diff. And
 * then reads same diff again when in diff.
 * @sa CLocationView::DrawRect()
 */
void CLocationView::OnDraw(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);

	if (m_view0 == NULL || m_view1 == NULL)
		return;

	CMergeDoc *pDoc = GetDocument();
	const int w = rc.Width() / 4;
	m_nLeftBarLeft = (rc.Width() - 2 * w) / 3;
	m_nLeftBarRight = m_nLeftBarLeft + w;
	m_nRightBarLeft = 2 * m_nLeftBarLeft + w;
	m_nRightBarRight = m_nRightBarLeft + w;
	const double hTotal = rc.Height() - (2 * Y_OFFSET); // Height of draw area
	const int nbLines = min(m_view0->GetLineCount(), m_view1->GetLineCount());
	double LineInPix = hTotal / nbLines;
	COLORREF cr0 = CLR_NONE; // Left side color
	COLORREF cr1 = CLR_NONE; // Right side color
	COLORREF crt = CLR_NONE; // Text color
	BOOL bwh = FALSE;
	int nstart0 = -1;
	int nend0 = -1;

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

	while (true)
	{
		// here nstart0 = last line before block
		BOOL ok0 = GetNextRect(nend0);
		if (!ok0)
			break;

		// here nend0 = last line of block
		int blockHeight = nend0 - nstart0;
		nstart0++;

		// here nstart0 = first line of block
		int nBeginY = (int) (nstart0 * LineInPix + Y_OFFSET);
		int nEndY = (int) ((blockHeight + nstart0) * LineInPix + Y_OFFSET);

		// Draw left side block
		m_view0->GetLineColors(nstart0, cr0, crt, bwh);
		CRect r0(m_nLeftBarLeft, nBeginY, m_nLeftBarRight, nEndY);
		DrawRect(pDC, r0, cr0, ((CMergeEditView*)m_view0)->IsLineInCurrentDiff(nstart0));
		
		// Draw right side block
		m_view1->GetLineColors(nstart0, cr1, crt, bwh);
		CRect r1(m_nRightBarLeft, nBeginY, m_nRightBarRight, nEndY);
		DrawRect(pDC, r1, cr1, ((CMergeEditView*)m_view0)->IsLineInCurrentDiff(nstart0));

		// Test if we draw a connector
		BOOL bDisplayConnectorFromLeft = FALSE;
		BOOL bDisplayConnectorFromRight = FALSE;

		switch (m_displayMovedBlocks)
		{
		case DISPLAY_MOVED_FOLLOW_DIFF:
			// display moved block only for current diff
			if (! ((CMergeEditView*)m_view0)->IsLineInCurrentDiff(nstart0))
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
			int apparent0 = nstart0;
			int apparent1 = pDoc->RightLineInMovedBlock(apparent0);
			if (apparent1 != -1)
			{
				MovedLine line;
				CPoint start;
				CPoint end;

				start.x = m_nLeftBarRight;
				int leftUpper = (int) (apparent0 * LineInPix + Y_OFFSET);
				int leftLower = (int) ((blockHeight + apparent0) * LineInPix + Y_OFFSET);
				start.y = leftUpper + (leftLower - leftUpper) / 2;
				end.x = m_nRightBarLeft;
				int rightUpper = (int) (apparent1 * LineInPix + Y_OFFSET);
				int rightLower = (int) ((blockHeight + apparent1) * LineInPix + Y_OFFSET);
				end.y = rightUpper + (rightLower - rightUpper) / 2;
				line.ptLeft = start;
				line.ptRight = end;
				m_movedLines.AddTail(line);
			}
		}

		if (bDisplayConnectorFromRight)
		{
			int apparent1 = nstart0;
			int apparent0 = pDoc->LeftLineInMovedBlock(apparent1);
			if (apparent0 != -1)
			{
				MovedLine line;
				CPoint start;
				CPoint end;

				start.x = m_nLeftBarRight;
				int leftUpper = (int) (apparent0 * LineInPix + Y_OFFSET);
				int leftLower = (int) ((blockHeight + apparent0) * LineInPix + Y_OFFSET);
				start.y = leftUpper + (leftLower - leftUpper) / 2;
				end.x = m_nRightBarLeft;
				int rightUpper = (int) (apparent1 * LineInPix + Y_OFFSET);
				int rightLower = (int) ((blockHeight + apparent1) * LineInPix + Y_OFFSET);
				end.y = rightUpper + (rightLower - rightUpper) / 2;
				line.ptLeft = start;
				line.ptRight = end;
				m_movedLines.AddTail(line);
			}
		}

		nstart0 = nend0;

	} // blocks loop 

	if (m_displayMovedBlocks != DISPLAY_MOVED_NONE)
		DrawConnectLines();
}

/** 
 * @brief Return end of block.
 * 
 * Starting from lineindex (not number!) given, finds last line in same block.
 * A block is either all the lines of a diff, or all the lines that separates
 * two diffs, or the beginning lines before the first diff, or the last lines
 * after the last diff.
 * @param nLineIndex [in,out]
 *  - [in] Lineindex where search begins
 *  - [out] Lineindex of last line in same block
 * @return TRUE if last line found.
 */
BOOL CLocationView::GetNextRect(int &nLineIndex)
{
	CMergeDoc *pDoc = GetDocument();
	BOOL bInDiff = FALSE;
	int nextDiff = -1;
	const int nbLines = min(m_view0->GetLineCount(), m_view1->GetLineCount());
	
	++nLineIndex;
	if (nLineIndex >= nbLines)
		return FALSE;

	bInDiff = pDoc->m_diffList.GetNextDiff(nLineIndex, nextDiff);
	
	// No diffs left, return last line of file.
	if (nextDiff == -1)
	{
		nLineIndex = nbLines - 1;
		return TRUE;
	}

	DIFFRANGE di = {0};
	if (!pDoc->m_diffList.GetDiff(nextDiff, di))
		return FALSE;

	// Line not in diff. Return last non-diff line.
	if (bInDiff == FALSE)
	{
		nLineIndex = di.dbegin0 - 1;
		return TRUE;
	}

	// Line is in diff. Get last line from side where all lines are present.
	if (di.op == OP_LEFTONLY || di.op == OP_RIGHTONLY || di.op == OP_DIFF)
	{
		if (di.blank0 == -1)
			nLineIndex = di.dend1;
		else
			nLineIndex = di.dend0;
	}

	return TRUE;
}

/** 
 * @brief Draw one block of map.
 */
void CLocationView::DrawRect(CDC* pDC, const CRect& r, COLORREF cr, BOOL border)
{
	if (cr == CLR_NONE || cr == GetSysColor(COLOR_WINDOW))
	{
		CPen* oldObj = (CPen*)pDC->SelectStockObject(BLACK_PEN);
		pDC->Rectangle(r);
		pDC->SelectObject(oldObj);
	}
	// colored rectangle
	else
	{
		CBrush brush(cr);
		pDC->FillSolidRect(r, cr);
		if (border)
		{
			// outter rectangle
			CRect outter = r;
			outter.InflateRect(2,2);
			// dont erase inside rect
			CBrush* oldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
			CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
			CPen* oldPen = pDC->SelectObject(&pen);
			pDC->Rectangle(outter);
			pDC->SelectObject(oldPen);
			pDC->SelectObject(oldBrush);
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
	if(GetCapture() == this)
	{
		if(GotoLocation(point, FALSE))
			return;
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
BOOL CLocationView::GotoLocation(CPoint point, BOOL bRealLine)
{
	CRect rc;
	GetClientRect(rc);

	if (m_view0 == NULL || m_view1 == NULL)
		return FALSE;

	int line = -1;
	int lineOther = -1;
	int bar = IsInsideBar(rc, point);
	if (bar == BAR_LEFT || bar == BAR_RIGHT)
	{
		line = GetLineFromYPos(point.y, rc, bar, bRealLine);
	}
	else if (bar == BAR_YAREA)
	{
		line = GetLineFromYPos(point.y, rc, bar, FALSE);
	}
	else
		return FALSE;

	m_view0->GotoLine(line, bRealLine, bar == BAR_LEFT);
	if (bar == BAR_LEFT)
		m_view0->SetFocus();
	else if (bar == BAR_RIGHT)
		m_view1->SetFocus();

	return TRUE;
}

void CLocationView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// Make sure window is active
	GetParentFrame()->ActivateFrame();

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
		nLine = GetLineFromYPos(pt.y, rc, bar);
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
		m_view0->GotoLine(nLine, TRUE, bar == BAR_LEFT);
		if (bar == BAR_LEFT)
			m_view0->SetFocus();
		else
			m_view1->SetFocus();
		break;
	case ID_EDIT_WMGOTO:
		m_view0->WMGoto();
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
 * @param [in] rc size of locationpane
 * @param [in] bar bar/file
 * @param [in] bRealLine TRUE if real line is returned, FALSE for view line
 * @return 0-based index of view/real line in file [0...lines-1]
 */
int CLocationView::GetLineFromYPos(int nYCoord, CRect rc, int bar,
	BOOL bRealLine)
{
	CMergeDoc* pDoc = GetDocument();
	const int nbLines = min(m_view0->GetLineCount(), m_view1->GetLineCount());
	int line = (int) (m_pixInLines * (nYCoord - Y_OFFSET));
	int nRealLine = -1;

	line--; // Convert linenumber to lineindex
	if (line < 0)
		line = 0;
	if (line > (nbLines - 1))
		line = nbLines - 1;

	// We've got a view line now
	if (bRealLine == FALSE)
		return line;

	// Get real line (exclude ghost lines)
	if (bar == BAR_LEFT)
	{
		nRealLine = pDoc->m_ltBuf.ComputeRealLine(line);
	}
	else if (bar == BAR_RIGHT)
	{
		nRealLine = pDoc->m_rtBuf.ComputeRealLine(line);
	}
	return nRealLine;
}

/** 
 * @brief Determines if given coords are inside left/right bar.
 * @param rc [in] size of locationpane client area
 * @param pt [in] point we want to check, in client coordinates.
 * @return LOCBAR_TYPE area where point is.
 */
int CLocationView::IsInsideBar(CRect rc, POINT pt)
{
	int retVal = BAR_NONE;
	BOOL bLeftSide = FALSE;
	BOOL bRightSide = FALSE;
	BOOL bYarea = FALSE;
	const int w = rc.Width() / 4;
	const int x = (rc.Width() - 2 * w) / 3;
	const int nbLines = min(m_view0->GetLineCount(), m_view1->GetLineCount());
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
	else if(bRightSide)
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
	const int nScreenLines = pDoc->GetRightView()->GetScreenLines();
	if (nTopLine == -1)
		nTopLine = pDoc->GetRightView()->GetTopLine();
	
	if (nBottomLine == -1)
		nBottomLine = nTopLine + nScreenLines;

	CRect rc;
	GetClientRect(rc);
	const double hTotal = rc.Height() - (2 * Y_OFFSET); // Height of draw area
	const int nbLines = min(m_view0->GetLineCount(), m_view1->GetLineCount());
	double LineInPix = hTotal / nbLines;
	if (LineInPix > MAX_LINEPIX)
		LineInPix = MAX_LINEPIX;

	int nTopCoord = (int) (Y_OFFSET + ((double)nTopLine * LineInPix));
	int nLeftCoord = INDICATOR_MARGIN;
	int nBottomCoord = (int) (Y_OFFSET + ((double)(nTopLine + nScreenLines) * LineInPix));
	int nRightCoord = rc.Width() - INDICATOR_MARGIN;
	
	// Visible area was not changed
	if (m_visibleTop == nTopCoord && m_visibleBottom == nBottomCoord)
		return;

	// Clear previous visible rect
	if (m_visibleTop != -1 && m_visibleBottom != -1)
	{
		CDC *pClientDC = GetDC();
		CRect rcVisibleArea(2, m_visibleTop, m_nLeftBarLeft - 2, m_visibleBottom);
		pClientDC->FillSolidRect(rcVisibleArea, GetSysColor(COLOR_WINDOW));
		rcVisibleArea.left = m_nLeftBarRight + 2;
		rcVisibleArea.right = m_nRightBarLeft - 2;
		pClientDC->FillSolidRect(rcVisibleArea, GetSysColor(COLOR_WINDOW));
		rcVisibleArea.left = m_nRightBarRight + 2;
		rcVisibleArea.right = rc.Width() - 2;
		pClientDC->FillSolidRect(rcVisibleArea, GetSysColor(COLOR_WINDOW));
		ReleaseDC(pClientDC);
	}

	double xbarBottom = min(nbLines / m_pixInLines + Y_OFFSET, rc.Height() - Y_OFFSET);
	int barBottom = (int)xbarBottom;
	// Make sure bottom coord is in bar range
	nBottomCoord = min(nBottomCoord, barBottom);

	// Store current values for later use (to check if area changes)
	m_visibleTop = nTopCoord;
	m_visibleBottom = nBottomCoord;

	CDC *pClientDC = GetDC();
	CRect rcVisibleArea(2, m_visibleTop, m_nLeftBarLeft - 2, m_visibleBottom);
	pClientDC->FillSolidRect(rcVisibleArea, GetSysColor(COLOR_SCROLLBAR));
	rcVisibleArea.left = m_nLeftBarRight + 2;
	rcVisibleArea.right = m_nRightBarLeft - 2;
	pClientDC->FillSolidRect(rcVisibleArea, GetSysColor(COLOR_SCROLLBAR));
	rcVisibleArea.left = m_nRightBarRight + 2;
	rcVisibleArea.right = rc.Width() - 2;
	pClientDC->FillSolidRect(rcVisibleArea, GetSysColor(COLOR_SCROLLBAR));
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
	m_view0->m_pLocationView = NULL;
	m_view1->m_pLocationView = NULL;
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
