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
 * @brief Max pixels in view per line in file
 */
static const double MAX_LINEPIX = 4.0;

/** 
 * @brief Bars in location pane
 */
enum
{
	BAR_NONE = 0,
	BAR_LEFT,
	BAR_RIGHT,
};

/////////////////////////////////////////////////////////////////////////////
// CMergeDiffDetailView

IMPLEMENT_DYNCREATE(CLocationView, CView)


CLocationView::CLocationView()
	: m_view0(0)
	, m_view1(0)
{
	SetConnectMovedBlocks(mf->m_options.GetInt(OPT_CONNECT_MOVED_BLOCKS));
}

BEGIN_MESSAGE_MAP(CLocationView, CView)
	//{{AFX_MSG_MAP(CLocationView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
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
	const int x = (rc.Width() - 2 * w) / 3;
	const int x2 = 2 * x + w;
	const int w2 = 2 * x + 2 * w;
	const double hTotal = rc.Height() - (2 * Y_OFFSET); // Height of draw area
	const int nbLines = min(m_view0->GetLineCount(), m_view1->GetLineCount());
	double nLineInPix = hTotal / nbLines;
	COLORREF cr0 = CLR_NONE; // Left side color
	COLORREF cr1 = CLR_NONE; // Right side color
	COLORREF crt = CLR_NONE; // Text color
	BOOL bwh = FALSE;
	int nstart0 = -1;
	int nend0 = -1;

	m_pixInLines = nbLines / hTotal;
	if (nLineInPix > MAX_LINEPIX)
	{
		nLineInPix = MAX_LINEPIX;
		m_pixInLines = 1 / MAX_LINEPIX;
	}

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
		const double nBeginY = nstart0 * nLineInPix + Y_OFFSET;
		const double nEndY = (blockHeight + nstart0) * nLineInPix + Y_OFFSET;

		// Draw left side block
		m_view0->GetLineColors(nstart0, cr0, crt, bwh);
		CRect r0(x, nBeginY, x + w, nEndY);
		DrawRect(pDC, r0, cr0, ((CMergeEditView*)m_view0)->IsLineInCurrentDiff(nstart0));
		
		// Draw right side block
		m_view1->GetLineColors(nstart0, cr1, crt, bwh);
		CRect r1(x2, nBeginY, w2, nEndY);
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
				// Draw connector between moved blocks
				const double nBeginY0 = apparent0 * nLineInPix + Y_OFFSET;
				const double nEndY0 = (blockHeight + apparent0) * nLineInPix + Y_OFFSET;
				const double nBeginY1 = apparent1 * nLineInPix + Y_OFFSET;
				const double nEndY1 = (blockHeight + apparent1) * nLineInPix + Y_OFFSET;
			
				CRect r0bis(x, nBeginY0, x + w, nEndY0);
				CRect r1bis(x2, nBeginY1, w2, nEndY1);

				CPen* oldObj = (CPen*)pDC->SelectStockObject(BLACK_PEN);
				pDC->MoveTo(r0bis.right, r0bis.CenterPoint().y);
				pDC->LineTo(r1bis.left, r1bis.CenterPoint().y);
				pDC->SelectObject(oldObj);
			}
		}

		if (bDisplayConnectorFromRight)
		{
			int apparent1 = nstart0;
			int apparent0 = pDoc->LeftLineInMovedBlock(apparent1);
			if (apparent0 != -1)
			{
				// Draw connector between moved blocks
				const double nBeginY0 = apparent0 * nLineInPix + Y_OFFSET;
				const double nEndY0 = (blockHeight + apparent0) * nLineInPix + Y_OFFSET;
				const double nBeginY1 = apparent1 * nLineInPix + Y_OFFSET;
				const double nEndY1 = (blockHeight + apparent1) * nLineInPix + Y_OFFSET;
			
				CRect r0bis(x, nBeginY0, x + w, nEndY0);
				CRect r1bis(x2, nBeginY1, w2, nEndY1);

				CPen* oldObj = (CPen*)pDC->SelectStockObject(BLACK_PEN);
				pDC->MoveTo(r0bis.right, r0bis.CenterPoint().y);
				pDC->LineTo(r1bis.left, r1bis.CenterPoint().y);
				pDC->SelectObject(oldObj);
			}
		}

		nstart0 = nend0;

	} // blocks loop 
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

	bInDiff = pDoc->GetNextDiff(nLineIndex, nextDiff);
	
	// No diffs left, return last line of file.
	if (nextDiff == -1)
	{
		nLineIndex = nbLines - 1;
		return TRUE;
	}

	DIFFRANGE di = {0};
	if (!pDoc->GetDiff(nextDiff, di))
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

/// User left double-clicked mouse
void CLocationView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (!GotoLocation(point))
		CView::OnLButtonDblClk(nFlags, point);
}

/// Move both views to point given (if in one of the file columns, else return FALSE)
BOOL CLocationView::GotoLocation(CPoint point)
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
		line = GetLineFromYPos(point.y, rc, bar);
	}
	else
		return FALSE;

	m_view0->GotoLine(line, TRUE, bar == BAR_LEFT);
	return TRUE;
}

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

	switch (m_displayMovedBlocks)
	{
	case DISPLAY_MOVED_NONE:
		pPopup->CheckMenuItem(ID_DISPLAY_MOVED_NONE, MF_CHECKED);
		break;
	case DISPLAY_MOVED_ALL:
		pPopup->CheckMenuItem(ID_DISPLAY_MOVED_ALL, MF_CHECKED);
		break;
	case DISPLAY_MOVED_FOLLOW_DIFF:
		pPopup->CheckMenuItem(ID_DISPLAY_MOVED_FOLLOW_DIFF, MF_CHECKED);
		break;
	}

	// invoke context menu
	// we don't want to use the main application handlers, so we use flags TPM_NONOTIFY | TPM_RETURNCMD
	// and handle the command after TrackPopupMenu
	int command = pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY  | TPM_RETURNCMD, point.x, point.y, AfxGetMainWnd());

	CMergeDoc* pDoc = GetDocument();
	switch (command)
	{
	case ID_LOCBAR_GOTODIFF:
		m_view0->GotoLine(nLine, TRUE, bar == BAR_LEFT);
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
 * @brief Calculates real line in file from given YCoord in bar.
 * @param nYcoord [in] ycoord in pane
 * @param rc [in] size of locationpane
 * @param bar [in] bar/file
 * @return 0-based index of real line in file [0...lines-1]
 */
int CLocationView::GetLineFromYPos(int nYCoord, CRect rc, int bar)
{
	CMergeDoc* pDoc = GetDocument();
	const int nbLines = min(m_view0->GetLineCount(), m_view1->GetLineCount());
	int line = m_pixInLines * (nYCoord - Y_OFFSET);
	int nRealLine = -1;

	line--; // Convert linenumber to lineindex
	if (line > nbLines - 1) // Just to be sure
		line = nbLines - 1;

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
 */
int CLocationView::IsInsideBar(CRect rc, POINT pt)
{
	int retVal = BAR_NONE;
	BOOL bLeftSide = FALSE;
	BOOL bRightSide = FALSE;
	const int w = rc.Width() / 4;
	const int x = (rc.Width() - 2 * w) / 3;
	const int nbLines = min(m_view0->GetLineCount(), m_view1->GetLineCount());
	// We need '1 / m_pixInLines' to get line in pixels and
	// that multiplied by linecount gives us bottom coord for bars.
	const int barBottom = min(nbLines / m_pixInLines + Y_OFFSET, rc.Height() - Y_OFFSET);

	if ((pt.y > Y_OFFSET) && (pt.y <= barBottom))
	{
		bLeftSide = (pt.x >= x && pt.x < x + w);
		bRightSide = (pt.x >= 2 * x + w && pt.x < 2 * x + 2 * w);
	}
	
	if (bLeftSide)
		retVal = BAR_LEFT;
	else if(bRightSide)
		retVal = BAR_RIGHT;

	return retVal;
}
