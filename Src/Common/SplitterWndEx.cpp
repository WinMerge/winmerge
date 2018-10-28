//////////////////////////////////////////////////////////////////////
/** 
 * @file  SplitterWndEx.cpp
 *
 * @brief Implementation file for CSplitterWndEx
 *
 */
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <vector>
#include "SplitterWndEx.h"

#ifdef _DEBUG
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
	m_bBarLocked = false;
	m_bResizePanes = false;
	m_bAutoResizePanes = false;
	m_bHideBorders = false;
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

CScrollBar* CSplitterWndEx::GetScrollBarCtrl(CWnd* pWnd, int nBar) const
{
	UINT nID = pWnd->GetDlgCtrlID();
	//IdFromRowCol(row, col);
	if (nID < AFX_IDW_PANE_FIRST || nID > AFX_IDW_PANE_LAST)
		return nullptr;            // not a standard pane ID

	// appropriate PANE id - look for sibling (splitter, or just frame)
	UINT nIDScroll;
	if (nBar == SB_HORZ)
		nIDScroll = AFX_IDW_HSCROLL_FIRST + (nID - AFX_IDW_PANE_FIRST) % 16;
	else
		nIDScroll = AFX_IDW_VSCROLL_FIRST + (nID - AFX_IDW_PANE_FIRST) / 16;

	// return shared scroll bars that are immediate children of splitter
	return (CScrollBar*)GetDlgItem(nIDScroll);
}

void CSplitterWndEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
  // Ignore scroll events sent directly to the splitter (i.e. not from a
  // scroll bar)
  if (pScrollBar == nullptr)
    return;
	// maintain original synchronization functionality (all panes above the scrollbar)
	CSplitterWnd::OnHScroll(nSBCode, nPos, pScrollBar);

	// only sync if shared horizontal bars
	if((GetScrollStyle()&WS_HSCROLL) == 0)
		return;

	// enhance with proportional horizontal scroll synchronization
	ASSERT(pScrollBar != nullptr);
	int curCol = ::GetDlgCtrlID(pScrollBar->m_hWnd) - AFX_IDW_HSCROLL_FIRST;
	ASSERT(curCol >= 0 && curCol < m_nMaxCols);

	ASSERT(m_nRows > 0);

	// broadcast to all panes (other horizontal scrollbars and other panes)
	for (int col = 0; col < m_nCols; col++)
	{
		// for current column, already handled in base OnHScroll
		if(col==curCol)
			continue;

		CScrollBar* curBar = GetScrollBarCtrl(GetPane(0, col), SB_HORZ);
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
  // Ignore scroll events sent directly to the splitter (i.e. not from a
  // scroll bar)
  if (pScrollBar == nullptr)
    return;

	// only sync if shared vertical bars
	if((GetScrollStyle()&WS_VSCROLL) == 0)
		return;

	// maintain original synchronization functionality (all panes left from the scrollbar)
	CSplitterWnd::OnVScroll(nSBCode, nPos, pScrollBar);

	// enhance with proportional vertical scroll synchronization
	ASSERT(pScrollBar != nullptr);
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

		CScrollBar* curBar = GetScrollBarCtrl(GetPane(row, 0), SB_VERT);
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
	int hmin = 0;
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
		CSplitterWnd::RecalcLayout();
	}
}

void CSplitterWndEx::EqualizeCols() 
{
	if (m_nCols < 2)
		return;

	int i;
	int sum = 0;
	int hmin = 0;

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
		CSplitterWnd::RecalcLayout();
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
			EqualizeCols();
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
		if (m_nCols == 2 && m_bAutoResizePanes)
		{
			RecalcLayout();
			EqualizeRows();
		}
		else
		{
			EqualizeCols();
			EqualizeRows();
		}
	}

}

void CSplitterWndEx::FlipSplit()
{
	int nRows = m_nCols, nCols = m_nRows;
	std::vector<CWnd *> pColPanes(nCols);
	std::vector<CWnd *> pRowPanes(nRows);

	bool bHasVScroll = !!m_bHasHScroll;
	bool bHasHScroll = !!m_bHasVScroll;

	CScrollBar *pBar;
	int pane;
	for (pane = 1; pane < nRows; pane++)
	{
		pRowPanes[pane] = GetDlgItem(IdFromRowCol( 0, pane ));
		pBar = pRowPanes[pane]->GetScrollBarCtrl(SB_HORZ);
		if (pBar != nullptr)
			pBar->ShowWindow(SW_HIDE);
		pBar = pRowPanes[pane]->GetScrollBarCtrl(SB_VERT);
		if (pBar != nullptr)
			pBar->ShowWindow(SW_HIDE);
	}
	for (pane = 1; pane < nCols; pane++)
	{
		pColPanes[pane] = GetDlgItem(IdFromRowCol( pane, 0 ));
		pBar = pColPanes[pane]->GetScrollBarCtrl(SB_HORZ);
		if (pBar != nullptr)
			pBar->ShowWindow(SW_HIDE);
		pBar = pColPanes[pane]->GetScrollBarCtrl(SB_VERT);
		if (pBar != nullptr)
			pBar->ShowWindow(SW_HIDE);
	}

	m_nMaxCols = m_nCols = nCols;
	m_nMaxRows = m_nRows = nRows;

	CRowColInfo* pTmp = m_pColInfo;
	m_pColInfo = m_pRowInfo;
	m_pRowInfo = pTmp;

	for (pane = 1; pane < nRows; pane++)
		pRowPanes[pane]->SetDlgCtrlID( IdFromRowCol( pane, 0 ));
	for (pane = 1; pane < nCols; pane++)
		pColPanes[pane]->SetDlgCtrlID( IdFromRowCol( 0, pane ));

	SetScrollStyle(0);
	SetScrollStyle(
		(bHasVScroll ? WS_VSCROLL : 0) | 
		(bHasHScroll ? WS_HSCROLL : 0));

	RecalcLayout();

	EqualizeCols();
	EqualizeRows();

}

static COLORREF GetIntermediateColor(COLORREF a, COLORREF b)
{
	const int R = (GetRValue(a) - GetRValue(b)) / 2 + GetRValue(b);
	const int G = (GetGValue(a) - GetGValue(b)) / 2 + GetGValue(b);
	const int B = (GetBValue(a) - GetBValue(b)) / 2 + GetBValue(b);
	return RGB(R, G, B);
}

void CSplitterWndEx::OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg)
{
	CRect rect = rectArg;
	if (nType == splitBorder && pDC != nullptr && !m_bHideBorders)
	{
		COLORREF clrShadow  = GetSysColor(COLOR_BTNSHADOW);
		COLORREF clrFace    = GetSysColor(COLOR_BTNFACE);
		COLORREF clrShadow2 = GetIntermediateColor(clrFace, clrShadow);
		COLORREF clrShadow3 = GetIntermediateColor(clrFace, clrShadow2);
		COLORREF clrShadow4 = GetIntermediateColor(clrFace, clrShadow3);
		COLORREF clrShadow5 = GetIntermediateColor(clrFace, clrShadow4);
		pDC->Draw3dRect(rect, clrShadow5, clrShadow4);
		rect.InflateRect(-1, -1);
		pDC->Draw3dRect(rect, clrShadow3, clrShadow2);
		return;
	}
	return CSplitterWnd::OnDrawSplitter(pDC, nType, rectArg);
}
