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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CSplitterWndEx, CSplitterWnd)
	//{{AFX_MSG_MAP(CSplitterWndEx)
	ON_WM_HSCROLL()
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
	// maintain original synchronization functionality
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
	// broadcast to all panes
	for (int col = 0; col < m_nCols; col++)
	{
		// for current column, already handled in base OnHScroll
		if(col==curCol)
			continue;

		CScrollBar* curBar = GetPane(0, col)->GetScrollBarCtrl(SB_HORZ);
		register int temp = pScrollBar->GetScrollPos() * curBar->GetScrollLimit() + oldLimit/2;
		int newPos = temp/oldLimit;

		// iterate through all rows
		for (int row = 0; row < m_nRows; row++)
		{
			// broadcast to all rows
			GetPane(row, col)->SendMessage(WM_HSCROLL,
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
	int vEqual = sum/m_nCols;
	for (i = 0 ; i < m_nCols-1 ; i++)
	{
		SetColumnInfo(i, vEqual, hmin);
		sum -= vEqual;
	}
	SetColumnInfo(i, sum, hmin);

	RecalcLayout();
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


