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

// Few pixels of empty space around bars
static const DWORD Y_OFFSET = 5;

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
	COLORREF cr0 = CLR_NONE; // Left side color
	COLORREF cr1 = CLR_NONE; // Right side color
	COLORREF crt = CLR_NONE; // Text color
	BOOL bwh = FALSE;
	int nstart0 = -1;
	int nend0 = -1;

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
		const double nBeginY = (nstart0) * hTotal / nbLines + Y_OFFSET;
		const double nEndY = (blockHeight + nstart0) * hTotal / nbLines + Y_OFFSET;

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
				const double nBeginY0 = (apparent0) * hTotal / nbLines + Y_OFFSET;
				const double nEndY0 = (blockHeight + apparent0) * hTotal / nbLines + Y_OFFSET;
				const double nBeginY1 = (apparent1) * hTotal / nbLines + Y_OFFSET;
				const double nEndY1 = (blockHeight + apparent1) * hTotal / nbLines + Y_OFFSET;
			
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
				const double nBeginY0 = (apparent0) * hTotal / nbLines + Y_OFFSET;
				const double nEndY0 = (blockHeight + apparent0) * hTotal / nbLines + Y_OFFSET;
				const double nBeginY1 = (apparent1) * hTotal / nbLines + Y_OFFSET;
				const double nEndY1 = (blockHeight + apparent1) * hTotal / nbLines + Y_OFFSET;
			
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
	const int nbLines = min(pDoc->GetLineCount(TRUE), pDoc->GetLineCount(FALSE));
	
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
	if (cr==CLR_NONE)
	{
		CPen* oldObj = (CPen*)pDC->SelectStockObject(BLACK_PEN);
		pDC->Rectangle(r);
		pDC->SelectObject(oldObj);
	}
	// colored rectangle
	else
	{
		CBrush brush(cr);
		//pDC->FillRect(r,&brush);
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

	const int w = rc.Width() / 4;
	const int x = (rc.Width() - 2 * w) / 3;

	bool leftside = (point.x >= x && point.x < x+w);
	bool rightside = (point.x >= 2 * x + w && point.x < 2 * x + 2 * w);
	if (!leftside && !rightside)
		return FALSE;

	const int line = GetLineFromYPos(point.y, rc);

	m_view0->GoToLine(line, false);
	m_view1->GoToLine(line, false);

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

	const int nLine = GetLineFromYPos(pt.y, rc);
	CString strItem;
	CString strNum;
	
	// If cursor over bar, format string with linenumber, else disable item
	if (nLine > -1)
		strNum.Format(_T("%d"), nLine);
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
		m_view0->GoToLine(nLine, false);
		m_view1->GoToLine(nLine, false);
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

/// Calculate line in file from given YCoord in locationpane
int CLocationView::GetLineFromYPos(int nYCoord, CRect rc)
{
	const int nbLines = min(m_view0->GetLineCount(), m_view1->GetLineCount());
	int line = -1;

	if ((nYCoord > Y_OFFSET) && (nYCoord < (rc.Height() - Y_OFFSET)))
		line = ((double)nbLines / (rc.Height() - Y_OFFSET * 2)) * nYCoord;
	return line;
}
