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
#include "MergeEditView.h"
#include "LocationView.h"
#include "MergeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeDiffDetailView

IMPLEMENT_DYNCREATE(CLocationView, CView)


CLocationView::CLocationView()
	: m_view0(0)
	, m_view1(0)
{
}

BEGIN_MESSAGE_MAP(CLocationView, CView)
	//{{AFX_MSG_MAP(CLocationView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


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
 *
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

	const int w = rc.Width() / 4;
	const int x = (rc.Width() - 2 * w) / 3;
	const int nbLines = m_view0->GetLineCount();
	COLORREF cr0 = CLR_NONE; // Left side color
	COLORREF cr1 = CLR_NONE; // Right side color
	COLORREF crt = CLR_NONE; // Text color
	BOOL bwh = FALSE;
	int nstart0 = 0;
	int nend0 = 0;
	
	while (true)
	{
		BOOL ok0 = GetNextRect(nend0);
		if (!ok0)
			break;

		// Draw left side block
		m_view0->GetLineColors(nstart0 + 1, cr0, crt, bwh);
		CRect r0(x, nstart0 * rc.Height() / nbLines,
				x + w, nend0 * rc.Height() / nbLines);
		DrawRect(pDC, r0, cr0, ((CMergeEditView*)m_view0)->IsLineInCurrentDiff(nstart0 + 1));
		
		// Draw right side block
		m_view1->GetLineColors(nstart0 + 1, cr1, crt, bwh);
		CRect r1(2 * x + w, nstart0 * rc.Height() / nbLines,
				2 * x + 2 * w, nend0 * rc.Height() / nbLines);
		DrawRect(pDC, r1, cr1, ((CMergeEditView*)m_view0)->IsLineInCurrentDiff(nstart0 + 1));
		nstart0 = nend0;

		// Connected line
		if (ok0 && (cr0 == CLR_NONE) && (cr1 == CLR_NONE))
		{
			CPen* oldObj = (CPen*)pDC->SelectStockObject(BLACK_PEN);
			pDC->MoveTo(r0.right, r0.CenterPoint().y);
			pDC->LineTo(r1.left, r1.CenterPoint().y);
			pDC->SelectObject(oldObj);
		}
	}
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
	int nbLines = pDoc->GetLineCount(TRUE);
	
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
