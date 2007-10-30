//////////////////////////////////////////////////////////////////////
/** 
 * @file  SplitterWndEx.cpp
 *
 * @brief Implementation file for CSplitterWndEx
 *
 */
// ID line follows -- this is updated by SVN
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SplitterWndEx.h"

#ifdef COMPILE_MULTIMON_STUBS
#undef COMPILE_MULTIMON_STUBS
#endif
#include <multimon.h>

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
	m_bAutoResizePanes = FALSE;
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
	if (sum > 0)
	{
		int hEqual = sum/m_nRows;
		for (i = 0 ; i < m_nRows-1 ; i++)
		{
			SetRowInfo(i, hEqual, hmin);
			sum -= hEqual;
		}
		SetRowInfo(i, sum, hmin);

		RecalcLayout();
	}
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

void CSplitterWndEx::RecalcLayout()
{
	if (m_nCols == 2 && m_bAutoResizePanes)
	{
		// If WinMerge spans multiple monitors, split the panes on the monitor split.
		CRect vSplitterWndRect;
		GetWindowRect(vSplitterWndRect);
		HMONITOR hLeftMonitor = MonitorFromPoint(vSplitterWndRect.TopLeft(), MONITOR_DEFAULTTONEAREST);
		HMONITOR hRightMonitor = MonitorFromPoint(CPoint(vSplitterWndRect.right, vSplitterWndRect.top), MONITOR_DEFAULTTONEAREST);

		bool bSplitPanesInHalf = true;
		if (hLeftMonitor != hRightMonitor)
		{
			MONITORINFO info;
			info.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(hLeftMonitor, &info);

			int iDesiredWidthOfLeftPane = info.rcMonitor.right - vSplitterWndRect.left;
			int iDesiredWidthOfRightPane = vSplitterWndRect.right - info.rcMonitor.right;

			// Edge case - don't split if either pane would be less than 100 pixels.
			if (iDesiredWidthOfLeftPane > 100 && iDesiredWidthOfRightPane > 100)
			{
				bSplitPanesInHalf = false;
				SetColumnInfo(0, iDesiredWidthOfLeftPane, 0);
				SetColumnInfo(1, iDesiredWidthOfRightPane, 0);
			}
		}
		
		// If we don't want to split panes across monitors, just split them in half.
		// We want to do this so that if the window used to be split across monitors,
		// but now occupies only a single monitor, then the panes are updated correctly.
		if (bSplitPanesInHalf)
		{
			CRect vSplitterWndRect;
			GetWindowRect(vSplitterWndRect);
			SetColumnInfo(0, vSplitterWndRect.Width() / 2, 0);
			SetColumnInfo(1, vSplitterWndRect.Width() / 2, 0);
		}
	}

	CSplitterWnd::RecalcLayout();
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
