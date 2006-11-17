//////////////////////////////////////////////////////////////////////
/** 
 * @file  SplitterWndEx.cpp
 *
 * @brief Implementation file for CSplitterWndEx
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SplitterWndEx.h"
#include "MergeEditView.h"  // For printing - MasterPrint()

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CSplitterWndEx, CSplitterWnd)
	//{{AFX_MSG_MAP(CSplitterWndEx)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CSplitterWndEx, CSplitterWnd)

CSplitterWndEx::CSplitterWndEx()
{
	m_bBarLocked = FALSE;
	m_bResizePanes = FALSE;
}

CSplitterWndEx::~CSplitterWndEx()
{

}

int CSplitterWndEx::HitTest(CPoint pt) const
{
	if (m_bBarLocked)
		return 0;
	return CSplitterWnd::HitTest(pt);
}

void CSplitterWndEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	// maintain original synchronization functionality (all panes above the scrollbar)
	CSplitterWnd::OnHScroll(nSBCode, nPos, pScrollBar);

	// only sync if shared horizontal bars
	if((GetScrollStyle()&WS_HSCROLL) == 0)
		return;

	// enhance with proportional horizontal scroll synchronization
	ASSERT(pScrollBar != NULL);
	int curCol = ::GetDlgCtrlID(pScrollBar->m_hWnd) - AFX_IDW_HSCROLL_FIRST;
	ASSERT(curCol >= 0 && curCol < m_nMaxCols);

	ASSERT(m_nRows > 0);
	const int oldLimit = pScrollBar->GetScrollLimit();

	// broadcast to all panes (other horizontal scrollbars and other panes)
	for (int col = 0; col < m_nCols; col++)
	{
		// for current column, already handled in base OnHScroll
		if(col==curCol)
			continue;

		CScrollBar* curBar = GetPane(0, col)->GetScrollBarCtrl(SB_HORZ);
		int newPos = min(pScrollBar->GetScrollPos(), curBar->GetScrollLimit());

		// Set the scrollbar info using SetScrollInfo(), limited to 2.000.000.000 characters,
		// better than the 32.768 characters (signed short) of SendMessage(WM_HSCROLL,...) 
		SCROLLINFO si;
		si.nPos = newPos;
		si.nTrackPos = newPos;
		si.fMask = SIF_POS | SIF_TRACKPOS;
		curBar->SetScrollInfo(&si, FALSE);

		// iterate through all rows
		for (int row = 0; row < m_nRows; row++)
		{
			// repaint all rows
			GetPane(row, col)->SendMessage(WM_HSCROLL,
				MAKELONG(SB_THUMBPOSITION, newPos), (LPARAM)curBar->m_hWnd);
		}
	}

}


void CSplitterWndEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	// maintain original synchronization functionality (all panes left from the scrollbar)
	CSplitterWnd::OnVScroll(nSBCode, nPos, pScrollBar);

	// only sync if shared vertical bars
	if((GetScrollStyle()&WS_VSCROLL) == 0)
		return;

	// enhance with proportional vertical scroll synchronization
	ASSERT(pScrollBar != NULL);
	int curRow = ::GetDlgCtrlID(pScrollBar->m_hWnd) - AFX_IDW_VSCROLL_FIRST;
	ASSERT(curRow >= 0 && curRow < m_nMaxRows);

	ASSERT(m_nCols > 0);
	const int oldLimit = pScrollBar->GetScrollLimit();

	// broadcast to all panes (other vertical scrollbars and other panes)
	for (int row = 0; row < m_nRows; row++)
	{
		// for current row, already handled in base OnHScroll
		if(row==curRow)
			continue;

		CScrollBar* curBar = GetPane(0, curRow)->GetScrollBarCtrl(SB_VERT);
		double temp = ((double) pScrollBar->GetScrollPos()) * curBar->GetScrollLimit() + oldLimit/2;
		int newPos = (int) (temp/oldLimit);

		// Set the scrollbar info using SetScrollInfo(), limited to 2.000.000.000 characters,
		// better than the 32.768 characters (signed short) of SendMessage(WM_HSCROLL,...) 
		SCROLLINFO si;
		si.nPos = newPos;
		si.nTrackPos = newPos;
		si.fMask = SIF_POS | SIF_TRACKPOS;
		curBar->SetScrollInfo(&si, FALSE);

		// iterate through all columns
		for (int col = 0; col < m_nCols; col++)
		{
			// repaint all columns
			GetPane(row, col)->SendMessage(WM_VSCROLL,
				MAKELONG(SB_THUMBPOSITION, newPos), (LPARAM)curBar->m_hWnd);
		}
	}

}

void CSplitterWndEx::EqualizeRows() 
{
	if (m_nRows < 2)
		return;

	int i;
	int sum = 0;
	int hmin;
	for (i = 0 ; i < m_nRows ; i++)
	{
		int h;
		GetRowInfo(i, h, hmin);
		sum += h;
	}
	int hEqual = sum/m_nRows;
	for (i = 0 ; i < m_nRows-1 ; i++)
	{
		SetRowInfo(i, hEqual, hmin);
		sum -= hEqual;
	}
	SetRowInfo(i, sum, hmin);

	RecalcLayout();
}

void CSplitterWndEx::EqualizeCols() 
{
	if (m_nCols < 2)
		return;

	int i;
	int sum = 0;
	int hmin;

	for (i = 0 ; i < m_nCols ; i++)
	{
		int v;
		GetColumnInfo(i, v, hmin);
		sum += v;
	}

	// Sum is negative if WinMerge started minimized.
	if (sum > 0)
	{
		int vEqual = sum/m_nCols;
		for (i = 0 ; i < m_nCols-1 ; i++)
		{
			SetColumnInfo(i, vEqual, hmin);
			sum -= vEqual;
		}
		SetColumnInfo(i, sum, hmin);

		RecalcLayout();
	}
}





void CSplitterWndEx::OnSize(UINT nType, int cx, int cy) 
{
	CSplitterWnd::OnSize(nType, cx, cy);

	// and resize the panes 
	if (m_bResizePanes)
	{
		EqualizeCols();
		EqualizeRows();
	}

}

void CSplitterWndEx::MasterPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CRect rDraw = pInfo->m_rectDraw;
	CSize sz = rDraw.Size();
	int midX = sz.cx / 2;
	
	// print left pane	
	pInfo->m_rectDraw.right	= midX;
	CMergeEditView* pLeftPane = (CMergeEditView*)GetPane(0,0);
	pLeftPane->SlavePrint(pDC,pInfo);

	// print right pane
	pInfo->m_rectDraw = rDraw;
	pInfo->m_rectDraw.left = midX;

	CMergeEditView* pRightPane = (CMergeEditView*)GetPane(0,1);
	pRightPane->SlavePrint(pDC,pInfo);
}
