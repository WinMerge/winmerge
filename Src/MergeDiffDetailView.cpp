//////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeDiffDetailView.cpp
 *
 * @brief Implementation file for CMergeDiffDetailView
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "merge.h"
#include "MergeDiffDetailView.h"
#include "MergeDoc.h"
#include "MainFrm.h"
#include "OptionsMgr.h"
#include "ChildFrm.h"
#include "OptionsDef.h"
#include "SyntaxColors.h"
#include "MergeLineFlags.h"
#include "BCMenu.h"
#include "WaitStatusCursor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const UINT NROWS_INIT = 4;

#define MY_IDW_PANE_FIRST    AFX_IDW_PANE_FIRST  

/////////////////////////////////////////////////////////////////////////////
// CMergeDiffDetailView

IMPLEMENT_DYNCREATE(CMergeDiffDetailView, CCrystalTextView)

/**
 * @brief Constructor.
 */
CMergeDiffDetailView::CMergeDiffDetailView()
: m_nThisPane(0)
, m_lineBegin(0)
, m_lineEnd(-1)
, m_diffLength(0)
, m_displayLength(NROWS_INIT)
, m_nPrevPaneHeight(0)
, m_hwndFrame(NULL)
{
}

CMergeDiffDetailView::~CMergeDiffDetailView()
{
}


BEGIN_MESSAGE_MAP(CMergeDiffDetailView, CCrystalTextView)
	//{{AFX_MSG_MAP(CMergeDiffDetailView)
	ON_WM_SIZE()
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND(ID_SELECTLINEDIFF, OnSelectLineDiff)
	ON_UPDATE_COMMAND_UI(ID_SELECTLINEDIFF, OnUpdateSelectLineDiff)
	ON_COMMAND(ID_WINDOW_CHANGE_PANE, OnChangePane)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CHANGE_PANE, OnUpdateChangePane)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_L2R, OnL2r)
	ON_UPDATE_COMMAND_UI(ID_L2R, OnUpdateL2r)
	ON_COMMAND(ID_R2L, OnR2l)
	ON_UPDATE_COMMAND_UI(ID_R2L, OnUpdateR2l)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORDWRAP, OnUpdateViewWordWrap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMergeDiffDetailView diagnostics

#ifdef _DEBUG
CMergeDoc* CMergeDiffDetailView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMergeDoc)));
	return (CMergeDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMergeDiffDetailView message handlers

/**
 * @brief Get TextBuffer associated with view.
 * @return pointer to textbuffer associated with the view.
 */
CCrystalTextBuffer *CMergeDiffDetailView::LocateTextBuffer ()
{
	return GetDocument()->m_ptBuf[m_nThisPane];
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMergeDiffDetailView::UpdateResources()
{
}

BOOL CMergeDiffDetailView::PrimeListWithFile()
{
	// Set the tab size now, just in case the options change...
	// We don't update it at the end of OnOptions, 
	// we can update it safely now
	SetTabSize(GetOptionsMgr()->GetInt(OPT_TAB_SIZE));

	return TRUE;
}

/**
 * @brief Compute initial height of view.
 * @return Initial height set for view.
 */
int CMergeDiffDetailView::ComputeInitialHeight() 
{
	return GetLineHeight() * NROWS_INIT;
}

/**
 * @brief Set view's height.
 * @param [in] h new view height in lines
 * @todo Calculation seems suspicious...
 */
void CMergeDiffDetailView::SetDisplayHeight(int h) 
{
	m_displayLength = (h + GetLineHeight()/10) / GetLineHeight();
}

/**
 * @brief Called when window size is changed.
 * Recalculates view size and stores new size to registry.
 */
void CMergeDiffDetailView::OnSize(UINT nType, int cx, int cy) 
{
	CCrystalTextView::OnSize(nType, cx, cy);
	CRect rc;
	GetWindowRect(rc);
	SetDisplayHeight(rc.Height());

	// If height changed tell frame to store new size
	if (cx != m_nPrevPaneHeight)
	{
		m_nPrevPaneHeight = cx;
		if (m_hwndFrame != NULL)
			::PostMessage(m_hwndFrame, MSG_STORE_PANESIZES, 0, 0); 
	}
}

/// virtual, ensure we remain in diff
void CMergeDiffDetailView::EnsureVisible (CPoint pt)
{
	CPoint ptNew = pt;
	if (EnsureInDiff(ptNew))
		SetCursorPos(ptNew);
	CCrystalTextView::EnsureVisible(ptNew);
}


/// virtual, ensure we remain in diff
void CMergeDiffDetailView::SetSelection (const CPoint & ptStart, const CPoint & ptEnd)
{
	CPoint ptStartNew = ptStart;
	EnsureInDiff(ptStartNew);
	CPoint ptEndNew = ptEnd;
	EnsureInDiff(ptEndNew);
	CCrystalTextView::SetSelection(ptStartNew, ptEndNew);
}

void CMergeDiffDetailView::OnInitialUpdate()
{
	CCrystalTextView::OnInitialUpdate();
	SetFont(dynamic_cast<CMainFrame*>(AfxGetMainWnd())->m_lfDiff);

	m_lineBegin = 0;
	m_lineEnd = -1;
	m_diffLength = 0;
	m_displayLength = NROWS_INIT;
}

int CMergeDiffDetailView::GetAdditionalTextBlocks (int nLineIndex, TEXTBLOCK *pBuf)
{
	if (nLineIndex < m_lineBegin || nLineIndex > m_lineEnd)
		return 0;

	DWORD dwLineFlags = GetLineFlags(nLineIndex);
	if ((dwLineFlags & LF_DIFF) != LF_DIFF || (dwLineFlags & LF_MOVED) == LF_MOVED)
		return 0;

	if (!GetOptionsMgr()->GetBool(OPT_WORDDIFF_HIGHLIGHT))
		return 0;

	int nLineLength = GetLineLength(nLineIndex);
	wdiffarray worddiffs;
	GetDocument()->GetWordDiffArray(nLineIndex, &worddiffs);
	if (worddiffs.GetSize() == 0 || (worddiffs[0].end[0] == -1 && worddiffs[0].end[1] + 1 == nLineLength) || (worddiffs[0].end[1] == -1 && worddiffs[0].end[0] + 1 == nLineLength))
		return 0;

	int nWordDiffs = worddiffs.GetSize();

	pBuf[0].m_nCharPos = 0;
	pBuf[0].m_nColorIndex = COLORINDEX_NONE;
	pBuf[0].m_nBgColorIndex = COLORINDEX_NONE;
	for (int i = 0; i < nWordDiffs; i++)
	{
		pBuf[1 + i * 2].m_nCharPos = worddiffs[i].start[m_nThisPane];
		pBuf[2 + i * 2].m_nCharPos = worddiffs[i].end[m_nThisPane] + 1;
		pBuf[1 + i * 2].m_nColorIndex = COLORINDEX_HIGHLIGHTTEXT1 | COLORINDEX_APPLYFORCE;
		pBuf[1 + i * 2].m_nBgColorIndex = COLORINDEX_HIGHLIGHTBKGND1 | COLORINDEX_APPLYFORCE;
		pBuf[2 + i * 2].m_nColorIndex = COLORINDEX_NONE;
		pBuf[2 + i * 2].m_nBgColorIndex = COLORINDEX_NONE;
	}
	return nWordDiffs * 2 + 1;
}

COLORREF CMergeDiffDetailView::GetColor(int nColorIndex)
{
	switch (nColorIndex & ~COLORINDEX_APPLYFORCE)
	{
	case COLORINDEX_HIGHLIGHTBKGND1:
		return GetOptionsMgr()->GetInt(OPT_CLR_SELECTED_WORDDIFF);
	case COLORINDEX_HIGHLIGHTTEXT1:
		return GetOptionsMgr()->GetInt(OPT_CLR_SELECTED_WORDDIFF_TEXT);
	case COLORINDEX_HIGHLIGHTBKGND2:
		return GetOptionsMgr()->GetInt(OPT_CLR_WORDDIFF);
	case COLORINDEX_HIGHLIGHTTEXT2:
		return GetOptionsMgr()->GetInt(OPT_CLR_WORDDIFF_TEXT);
	default:
		return CCrystalTextView::GetColor(nColorIndex);
	}
}

/// virtual, avoid coloring the whole diff with diff color 
void CMergeDiffDetailView::GetLineColors(int nLineIndex, COLORREF & crBkgnd,
                                COLORREF & crText, BOOL & bDrawWhitespace)
{
	DWORD ignoreFlags = 0;
	GetLineColors2(nLineIndex, ignoreFlags, crBkgnd, crText, bDrawWhitespace);
}
/// virtual, avoid coloring the whole diff with diff color 
void CMergeDiffDetailView::GetLineColors2(int nLineIndex, DWORD ignoreFlags,
							COLORREF & crBkgnd, COLORREF & crText, BOOL & bDrawWhitespace)
{
	DWORD dwLineFlags = GetLineFlags(nLineIndex);

	if (dwLineFlags & ignoreFlags)
		dwLineFlags &= (~ignoreFlags);

	// Line with WinMerge flag, 
	// Lines with only the LF_DIFF/LF_TRIVIAL flags are not colored with Winmerge colors
	if (dwLineFlags & (LF_WINMERGE_FLAGS & ~LF_DIFF & ~LF_TRIVIAL & ~LF_MOVED))
	{
		crText = GetOptionsMgr()->GetInt(OPT_CLR_DIFF);
		bDrawWhitespace = TRUE;

		if (dwLineFlags & LF_GHOST)
		{
			crBkgnd = GetOptionsMgr()->GetInt(OPT_CLR_DIFF_DELETED);
		}

	}
	else
	{
		// If no syntax hilighting
		if (!GetOptionsMgr()->GetBool(OPT_SYNTAX_HIGHLIGHT))
		{
			crBkgnd = GetSysColor (COLOR_WINDOW);
			crText = GetSysColor (COLOR_WINDOWTEXT);
			bDrawWhitespace = FALSE;
		}
		else
			// Line not inside diff, get colors from CrystalEditor
			CCrystalTextView::GetLineColors(nLineIndex, crBkgnd,
				crText, bDrawWhitespace);
	}
	if (nLineIndex < m_lineBegin || nLineIndex > m_lineEnd)
		{
			crBkgnd = GetSysColor (COLOR_WINDOW);
			crText = GetSysColor (COLOR_WINDOW);
			bDrawWhitespace = FALSE;
	}
}

void CMergeDiffDetailView::OnDisplayDiff(int nDiff /*=0*/)
{
	int newlineBegin, newlineEnd;
	CMergeDoc *pd = GetDocument();
	if (nDiff < 0 || nDiff >= pd->m_diffList.GetSize())
	{
		newlineBegin = 0;
		newlineEnd = -1;
	}
	else
	{
		DIFFRANGE curDiff;
		VERIFY(pd->m_diffList.GetDiff(nDiff, curDiff));

		newlineBegin = curDiff.dbegin0;
		ASSERT (newlineBegin >= 0);
		newlineEnd = curDiff.dend0;
	}

	if (newlineBegin == m_lineBegin && newlineEnd == m_lineEnd)
		return;
	m_lineBegin = newlineBegin;
	m_lineEnd = newlineEnd;
	m_diffLength = m_lineEnd - m_lineBegin + 1;

	// scroll to the first line of the diff
	ScrollToLine(m_lineBegin);

	// tell the others views about this diff (no need to call UpdateSiblingScrollPos)
	CSplitterWnd *pSplitterWnd = GetParentSplitter(this, FALSE);

	// pSplitterWnd is NULL if WinMerge started minimized.
	if (pSplitterWnd != NULL)
	{
		int nRows = pSplitterWnd->GetRowCount ();
		int nCols = pSplitterWnd->GetColumnCount ();
		for (int nRow = 0; nRow < nRows; nRow++)
		{
			for (int nCol = 0; nCol < nCols; nCol++)
			{
				CMergeDiffDetailView *pSiblingView = static_cast<CMergeDiffDetailView*>(GetSiblingView (nRow, nCol));
				if (pSiblingView != NULL)
					pSiblingView->OnDisplayDiff(nDiff);
			}
		}
	}

	// update the width of the horizontal scrollbar
	RecalcHorzScrollBar();
}

/**
 * @brief Adjust the point to remain in the displayed diff
 *
 * @return Tells if the point has been changed
 */
BOOL CMergeDiffDetailView::EnsureInDiff(CPoint & pt)
{
	// first get the degenerate case out of the way
	// no diff ?
	if (m_diffLength == 0)
	{
		if (pt.y == m_lineBegin && pt.x == 0)
			return FALSE;
		pt.y = m_lineBegin;
		pt.x = 0;
		return TRUE;
	}

	// not above diff
	if (pt.y < m_lineBegin)
	{
		pt.y = m_lineBegin;
		pt.x = 0;
		return TRUE;
	}
	// not below diff
	if (pt.y > m_lineEnd)
	{
		pt.y = m_lineEnd;
		pt.x = GetLineLength(pt.y);
		return TRUE;
	}
	return FALSE;
}



/// virtual, ensure we remain in diff
void CMergeDiffDetailView::ScrollToSubLine (int nNewTopLine, BOOL bNoSmoothScroll /*= FALSE*/, BOOL bTrackScrollBar /*= TRUE*/)
{
	if (m_diffLength <= m_displayLength)
		nNewTopLine = m_lineBegin;
	else
	{
		if (nNewTopLine < m_lineBegin)
			nNewTopLine = m_lineBegin;
		if (nNewTopLine + m_displayLength - 1 > m_lineEnd)
			nNewTopLine = m_lineEnd - m_displayLength + 1;
	}
	m_nTopLine = nNewTopLine;
	
	CPoint pt = GetCursorPos();
	if (EnsureInDiff(pt))
		SetCursorPos(pt);
	
	CPoint ptSelStart, ptSelEnd;
	GetSelection (ptSelStart, ptSelEnd);
	if (EnsureInDiff(ptSelStart) || EnsureInDiff(ptSelEnd))
		SetSelection (ptSelStart, ptSelEnd);
	
	CCrystalTextView::ScrollToSubLine(nNewTopLine, bNoSmoothScroll, bTrackScrollBar);
}

/**
 * @brief Same purpose as the one as in MergeEditView.cpp
 * @note Nearly the same code also
 */
void CMergeDiffDetailView::UpdateSiblingScrollPos (BOOL bHorz)
{
	CSplitterWnd *pSplitterWnd = GetParentSplitter (this, FALSE);
	if (pSplitterWnd != NULL)
	{
		//  See CSplitterWnd::IdFromRowCol() implementation for details
		int nCurrentRow = (GetDlgCtrlID () - MY_IDW_PANE_FIRST) / 16;
		int nCurrentCol = (GetDlgCtrlID () - MY_IDW_PANE_FIRST) % 16;
		ASSERT (nCurrentRow >= 0 && nCurrentRow < pSplitterWnd->GetRowCount ());
		ASSERT (nCurrentCol >= 0 && nCurrentCol < pSplitterWnd->GetColumnCount ());

		// limit the TopLine : must be smaller than GetLineCount for all the panels
		int nNewTopLine = m_nTopLine;
		int nRows = pSplitterWnd->GetRowCount ();
		int nCols = pSplitterWnd->GetColumnCount ();
		int nRow=0;
		for (nRow = 0; nRow < nRows; nRow++)
		{
			for (int nCol = 0; nCol < nCols; nCol++)
			{
				CMergeDiffDetailView *pSiblingView = static_cast<CMergeDiffDetailView*>(GetSiblingView (nRow, nCol));
				if (pSiblingView != NULL)
					if (pSiblingView->GetLineCount() <= nNewTopLine)
						nNewTopLine = pSiblingView->GetLineCount()-1;
			}
		}

		if (m_nTopLine != nNewTopLine) 
		{
			// only modification from code in MergeEditView.cpp
			// Where are we now, are we still in a diff ? So set to no diff
			nNewTopLine = m_lineBegin = 0;
			m_lineEnd = -1;
			m_diffLength = 0;

			ScrollToLine(nNewTopLine);
		}


		for (nRow = 0; nRow < nRows; nRow++)
		{
			for (int nCol = 0; nCol < nCols; nCol++)
			{
				if (!(nRow == nCurrentRow && nCol == nCurrentCol))  //  We don't need to update ourselves
				{
					CMergeDiffDetailView *pSiblingView = static_cast<CMergeDiffDetailView*>(GetSiblingView (nRow, nCol));
					if (pSiblingView != NULL)
						pSiblingView->OnUpdateSibling (this, bHorz);
				}
			}
		}
	}
}

/**
 * @brief Same purpose as the one as in MergeDiffView.cpp
 * @note Code is the same except we cast to a pointer to a CMergeDiffDetailView
 */
void CMergeDiffDetailView::OnUpdateSibling (CCrystalTextView * pUpdateSource, BOOL bHorz)
{
	if (pUpdateSource != this)
	{
		ASSERT (pUpdateSource != NULL);
		ASSERT_KINDOF (CCrystalTextView, pUpdateSource);
		// only modification from code in MergeEditView.cpp
		CMergeDiffDetailView *pSrcView = static_cast<CMergeDiffDetailView*>(pUpdateSource);
		if (!bHorz)  // changed this so bHorz works right
		{
			ASSERT (pSrcView->m_nTopLine >= 0);
			if (pSrcView->m_nTopLine != m_nTopLine)
			{
				ScrollToLine (pSrcView->m_nTopLine, TRUE, FALSE);
				UpdateCaret ();
				RecalcVertScrollBar(TRUE);
			}
		}
		else
		{
			ASSERT (pSrcView->m_nOffsetChar >= 0);
			if (pSrcView->m_nOffsetChar != m_nOffsetChar)
			{
				ScrollToChar (pSrcView->m_nOffsetChar, TRUE, FALSE);
				UpdateCaret ();
				RecalcHorzScrollBar(TRUE);
			}
		}
	}
}

/*
 * @brief Compute the max length of the lines inside the displayed diff
 */
int CMergeDiffDetailView::GetDiffLineLength ()
{
	int nMaxLineLength = 0;

	// we can not use GetLineActualLength below nLineCount
	// diff info (and lineBegin/lineEnd) are updated only during Rescan
	// they may get invalid just after we delete some text
	int validLineEnd = m_lineEnd;
	if (m_lineEnd >= GetLineCount())
		validLineEnd = GetLineCount() - 1;

	for (int I = m_lineBegin; I <= validLineEnd; I++)
	{
		int nActualLength = GetLineActualLength (I);
		if (nMaxLineLength < nActualLength)
			nMaxLineLength = nActualLength;
	}
	return nMaxLineLength;
}


/**
 * @brief Update the horizontal scrollbar
 *
 * @note The scrollbar width is the one needed for the largest view
 * @sa ccrystaltextview::RecalcHorzScrollBar()
 */
void CMergeDiffDetailView::RecalcHorzScrollBar (BOOL bPositionOnly /*= FALSE*/ )
{
	// Again, we cannot use nPos because it's 16-bit
	SCROLLINFO si = {0};
	const int nScreenChars = GetScreenChars();

	// note : this value differs from the value in CCrystalTextView::RecalcHorzScrollBar
	int nMaxLineLen = 0;
	if (GetDocument()->GetRightDetailView())
		nMaxLineLen = GetDocument()->GetRightDetailView()->GetDiffLineLength();
	if (GetDocument()->GetLeftDetailView())
		nMaxLineLen = max(nMaxLineLen, GetDocument()->GetLeftDetailView()->GetDiffLineLength());
	
	si.cbSize = sizeof (si);
	if (bPositionOnly)
	{
		si.fMask = SIF_POS;
		si.nPos = m_nOffsetChar;
	}
	else
	{
		if (nScreenChars >= nMaxLineLen && m_nOffsetChar > 0)
		{
			m_nOffsetChar = 0;
			Invalidate ();
			UpdateCaret ();
		}
		si.fMask = SIF_DISABLENOSCROLL | SIF_PAGE | SIF_POS | SIF_RANGE;
		si.nMin = 0;
		
		// Horiz scroll limit to longest line + one screenwidth 
		si.nMax = nMaxLineLen + nScreenChars;
		si.nPage = nScreenChars;
		si.nPos = m_nOffsetChar;
	}
	VERIFY (SetScrollInfo (SB_HORZ, &si));
}

void CMergeDiffDetailView::OnRefresh()
{
	CMergeDoc *pd = GetDocument();
	ASSERT(pd);
	pd->FlushAndRescan(TRUE);
}

BOOL CMergeDiffDetailView::PreTranslateMessage(MSG* pMsg)
{
	// Check if we got 'ESC pressed' -message
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE)) 
	{
		// Ask about saving unsaved document, allow to cancel closing
		CMergeDoc *pd = GetDocument();
		if (pd->PromptAndSaveIfNeeded(TRUE))
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_CLOSE);
			return FALSE;
		}
		else
			return TRUE;
	}
	return CCrystalTextView::PreTranslateMessage(pMsg);
}

/**
 * @brief Select linedifference in the current line.
 *
 * Select line difference in current line. Selection type
 * is choosed by highlight type.
 */
void CMergeDiffDetailView::OnSelectLineDiff() 
{
	CMergeDoc::DIFFLEVEL level = CMergeDoc::BYTEDIFF;
	if (GetOptionsMgr()->GetBool(OPT_BREAK_ON_WORDS))
		level = CMergeDoc::WORDDIFF;

	// Pass this to the document, to compare this file to other
	GetDocument()->Showlinediff(this, level);
}

/// Enable select difference menuitem if current line is inside difference.
void CMergeDiffDetailView::OnUpdateSelectLineDiff(CCmdUI* pCmdUI) 
{
	int line = GetCursorPos().y;
	BOOL enable = (GetLineFlags(line) & LF_DIFF) != 0;
	pCmdUI->Enable(enable);
}

void CMergeDiffDetailView::PushCursors()
{
	// push lineBegin and the cursor
	m_lineBeginPushed = m_lineBegin;
	m_ptCursorPosPushed = m_ptCursorPos;
	// and top line positions
	m_nTopSubLinePushed = m_nTopSubLine;
}

void CMergeDiffDetailView::PopCursors()
{
	m_lineBegin = m_lineBeginPushed;
	m_lineEnd = m_lineBegin + m_diffLength - 1;

	m_ptCursorPos = m_ptCursorPosPushed;

	if (m_lineBegin >= GetLineCount())
	{
		// even the first line is invalid, stop displaying the diff
		m_lineBegin = m_nTopLine = m_nTopSubLine = 0;
		m_lineEnd = -1;
		m_diffLength = 0;
	}
	else
		{
		// just check that all positions all valid
		m_lineEnd = min(m_lineEnd, GetLineCount()-1);
		m_diffLength = m_lineEnd - m_lineBegin + 1;
		m_ptCursorPos.y = min(m_ptCursorPos.y, GetLineCount()-1);
		m_ptCursorPos.x = min(m_ptCursorPos.x, GetLineLength(m_ptCursorPos.y));
	}

		// restore the scrolling position
	m_nTopSubLine = m_nTopSubLinePushed;
	if (m_nTopSubLine >= GetSubLineCount())
		m_nTopSubLine = GetSubLineCount() - 1;
	int nDummy;
	GetLineBySubLine( m_nTopSubLine, m_nTopLine, nDummy );
    RecalcVertScrollBar(TRUE);

	// other positions are set to (0,0) during ResetView
}

/**
 * @brief Change active pane in diff pane
 */
void CMergeDiffDetailView::OnChangePane()
{
	CSplitterWnd *pSplitterWnd = GetParentSplitter(this, FALSE);
	pSplitterWnd->ActivateNext();
}

/**
 * @brief Enable "Change Pane" menuitem when diff pane is active
 */
void CMergeDiffDetailView::OnUpdateChangePane(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

/**
 * @brief Store HWND for parent frame.
 */
void CMergeDiffDetailView::SetFrameHwnd(HWND hwndFrame)
{
	m_hwndFrame = hwndFrame;
}

/**
 * @brief Offer a context menu
 */
void CMergeDiffDetailView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// Create the menu and populate it with the available functions
	BCMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_MERGEDETAILVIEW));
	VERIFY(menu.LoadToolbar(IDR_MAINFRAME));
	theApp.TranslateMenu(menu.m_hMenu);

	BCMenu *pSub = (BCMenu *)menu.GetSubMenu(0);
	ASSERT(pSub != NULL);

	// Context menu opened using keyboard has no coordinates
	if (point.x == -1 && point.y == -1)
	{
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);

		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	pSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, AfxGetMainWnd());
}

/**
 * @brief Copy diff from left pane to right pane
 */
void CMergeDiffDetailView::OnL2r()
{
	// Check that right side is not readonly
	if (IsReadOnly(1))
		return;
	
	CMergeDoc *pDoc = GetDocument();
	int currentDiff = pDoc->GetCurrentDiff();

	if (currentDiff != -1 && pDoc->m_diffList.IsDiffSignificant(currentDiff))
	{
		WaitStatusCursor waitstatus(IDS_STATUS_COPYL2R);
		pDoc->ListCopy(0, 1, currentDiff);
	}
}

/**
 * @brief Called when "Copy to left" item is updated
 */
void CMergeDiffDetailView::OnUpdateL2r(CCmdUI* pCmdUI)
{
	// Check that right side is not readonly
	if (!IsReadOnly(1))
	{
		if (GetDocument()->GetCurrentDiff() != -1)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Copy diff from right pane to left pane
 */
void CMergeDiffDetailView::OnR2l()
{
	// Check that left side is not readonly
	if (IsReadOnly(0))
		return;

	CMergeDoc *pDoc = GetDocument();
	int currentDiff = pDoc->GetCurrentDiff();

	if (currentDiff != -1 && pDoc->m_diffList.IsDiffSignificant(currentDiff))
	{
		WaitStatusCursor waitstatus(IDS_STATUS_COPYR2L);
		pDoc->ListCopy(1, 0, currentDiff);
	}
}

/**
 * @brief Called when "Copy to right" item is updated
 */
void CMergeDiffDetailView::OnUpdateR2l(CCmdUI* pCmdUI)
{
	// Check that left side is not readonly
	if (!IsReadOnly(0))
	{
		if (GetDocument()->GetCurrentDiff() != -1)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Returns if buffer is read-only
 * @note This has no any relation to file being read-only!
 */
BOOL CMergeDiffDetailView::IsReadOnly(int pane)
{
	return GetDocument()->m_ptBuf[pane]->GetReadOnly();
}

/**
 * @brief Called after document is loaded.
 * This function is called from CMergeDoc::OpenDocs() after documents are
 * loaded. So this is good place to set View's options etc.
 */
void CMergeDiffDetailView::DocumentsLoaded()
{
	SetTabSize(GetOptionsMgr()->GetInt(OPT_TAB_SIZE));
	SetViewTabs(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE));
	SetViewEols(GetOptionsMgr()->GetBool(OPT_VIEW_WHITESPACE),
			GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL));
	SetWordWrapping(FALSE);
}

void CMergeDiffDetailView::OnUpdateViewWordWrap(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(m_bWordWrap);
}
