// SplitterWndEx.cpp: implementation of the CSplitterWndEx class.
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSplitterWndEx::CSplitterWndEx()
{

}

CSplitterWndEx::~CSplitterWndEx()
{

}

void CSplitterWndEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	// maintain original synchronization functionality
	CSplitterWnd::OnHScroll(nSBCode, nPos, pScrollBar);

	// only sync if shared horizontal bars
	if(GetScrollStyle()&WS_HSCROLL == 0)
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
