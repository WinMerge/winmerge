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
#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "MergeDoc.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "OptionsDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NROWS_INIT	4

#define MY_IDW_PANE_FIRST    AFX_IDW_PANE_FIRST  

/////////////////////////////////////////////////////////////////////////////
// CMergeDiffDetailView

IMPLEMENT_DYNCREATE(CMergeDiffDetailView, CCrystalTextView)

CMergeDiffDetailView::CMergeDiffDetailView()
{
	m_bIsLeft = FALSE;
}

CMergeDiffDetailView::~CMergeDiffDetailView()
{
}


BEGIN_MESSAGE_MAP(CMergeDiffDetailView, CCrystalTextView)
	//{{AFX_MSG_MAP(CMergeDiffDetailView)
	ON_WM_SIZE()
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_COMMAND(ID_SHOWLINEDIFF, OnShowlinediff)
	ON_UPDATE_COMMAND_UI(ID_SHOWLINEDIFF, OnUpdateShowlinediff)
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
CCrystalTextBuffer *CMergeDiffDetailView::LocateTextBuffer ()
{
	if (m_bIsLeft)
		return &GetDocument()->m_ltBuf;
	return &GetDocument()->m_rtBuf;
}

void CMergeDiffDetailView::DoScroll(UINT code, UINT pos, BOOL bDoScroll)
{
	TRACE(_T("Scroll %s: pos=%d\n"), m_bIsLeft? _T("left"):_T("right"), pos);
	if (bDoScroll
		&& (code == SB_THUMBPOSITION
			|| code == SB_THUMBTRACK))
	{
		ScrollToLine(pos);
	}
	CCrystalTextView::OnVScroll(code, pos, NULL);
}

/// Update any language-dependent data
void CMergeDiffDetailView::UpdateResources()
{
}

BOOL CMergeDiffDetailView::PrimeListWithFile()
{
	// Set the tab size now, just in case the options change...
	// We don't update it at the end of OnOptions, 
	// we can update it safely now
	SetTabSize(mf->m_options.GetInt(OPT_TAB_SIZE));

	return TRUE;
}

int CMergeDiffDetailView::ComputeInitialHeight() 
{
	return GetLineHeight() * NROWS_INIT;
}
void CMergeDiffDetailView::SetDisplayHeight(int h) 
{
	displayLength = (h + GetLineHeight()/10) / GetLineHeight();
}

void CMergeDiffDetailView::OnSize(UINT nType, int cx, int cy) 
{
	CCrystalTextView::OnSize(nType, cx, cy);
	CRect rc;
	GetWindowRect(rc);
	SetDisplayHeight(rc.Height());
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

	lineBegin = 0;
	lineEnd = -1;
	diffLength = 0;
	displayLength = NROWS_INIT;
}


/// virtual, avoid coloring the whole diff with diff color 
void CMergeDiffDetailView::GetLineColors(int nLineIndex, COLORREF & crBkgnd,
                                COLORREF & crText, BOOL & bDrawWhitespace)
{
	DWORD dwLineFlags = GetLineFlags(nLineIndex);
	
	// Line with WinMerge flag, 
	// Lines with only the LF_DIFF/LF_TRIVIAL flags are not colored with Winmerge colors
	if (dwLineFlags & (LF_WINMERGE_FLAGS & ~LF_DIFF & ~LF_TRIVIAL))
	{
		crText = mf->m_options.GetInt(OPT_CLR_DIFF);
		bDrawWhitespace = TRUE;

		if (dwLineFlags & LF_GHOST)
		{
			crBkgnd = mf->m_options.GetInt(OPT_CLR_DIFF_DELETED);
		}

	}
	else
	{
		// If no syntax hilighting
		if (!mf->m_options.GetInt(OPT_SYNTAX_HIGHLIGHT))
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
	if (nLineIndex < lineBegin || nLineIndex > lineEnd)
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
	if (nDiff < 0 || nDiff >= (signed int)pd->m_nDiffs)
	{
		newlineBegin = 0;
		newlineEnd = -1;
	}
	else
	{
		newlineBegin = pd->m_diffs[nDiff].dbegin0;
		ASSERT (newlineBegin >= 0);
		newlineEnd = pd->m_diffs[nDiff].dend0;
	}

	if (newlineBegin == lineBegin && newlineEnd == lineEnd)
		return;
	lineBegin = newlineBegin;
	lineEnd = newlineEnd;
	diffLength = lineEnd - lineBegin + 1;

	// scroll to the first line of the first diff
	ScrollToLine(lineBegin);

	// tell the others views about this diff (no need to call UpdateSiblingScrollPos)
	CSplitterWnd *pSplitterWnd = GetParentSplitter (this, FALSE);
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

/**
 * @brief Adjust the point to remain in the displayed diff
 *
 * @return Tells if the point has been changed
 */
BOOL CMergeDiffDetailView::EnsureInDiff(CPoint & pt)
{
	// first get the degenerate case out of the way
	// no diff ?
	if (diffLength == 0)
	{
		if (pt.y == lineBegin && pt.x == 0)
			return FALSE;
		pt.y = lineBegin;
		pt.x = 0;
		return TRUE;
	}

	// not above diff
	if (pt.y < lineBegin)
	{
		pt.y = lineBegin;
		pt.x = 0;
		return TRUE;
	}
	// not below diff
	if (pt.y > lineEnd)
	{
		pt.y = lineEnd;
		pt.x = GetLineLength(pt.y);
		return TRUE;
	}
	return FALSE;
}



/// virtual, ensure we remain in diff
void CMergeDiffDetailView::ScrollToSubLine (int nNewTopLine, BOOL bNoSmoothScroll /*= FALSE*/, BOOL bTrackScrollBar /*= TRUE*/)
{
	if (diffLength <= displayLength)
		nNewTopLine = lineBegin;
	else
	{
		if (nNewTopLine < lineBegin)
			nNewTopLine = lineBegin;
		if (nNewTopLine + displayLength - 1 > lineEnd)
			nNewTopLine = lineEnd - displayLength + 1;
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
* @brief Same purpose as the one as in MergeDiffView.cpp
*
* @note Nearly the same code also
*
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
		for (int nRow = 0; nRow < nRows; nRow++)
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
			nNewTopLine = lineBegin = 0;
			lineEnd = -1;
			diffLength = 0;

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
*
* @note Code is the same except we cast to a pointer to a CMergeDiffDetailView
*
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

			// This ASSERT is wrong: panes have different files and
			// different linecounts
			// ASSERT (pSrcView->m_nTopLine < GetLineCount ());
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

			// This ASSERT is wrong: panes have different files and
			// different linelengths
			// ASSERT (pSrcView->m_nOffsetChar < GetMaxLineLength ());
			if (pSrcView->m_nOffsetChar != m_nOffsetChar)
			{
				ScrollToChar (pSrcView->m_nOffsetChar, TRUE, FALSE);
				UpdateCaret ();
				RecalcHorzScrollBar(TRUE);
			}
		}
	}
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
		if (pd->SaveHelper(TRUE))
		{
			// Set modified status to false so that we are not asking
			// about saving again
			pd->m_ltBuf.SetModified(FALSE);
			pd->m_rtBuf.SetModified(FALSE);
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_CLOSE);
			return FALSE;
		}
		else
			return TRUE;
	}
	return CCrystalTextView::PreTranslateMessage(pMsg);
}

/// Highlight difference in current line
void CMergeDiffDetailView::OnShowlinediff() 
{
	// Pass this to the document, to compare this file to other
	GetDocument()->Showlinediff(this);
}

/// Enable highlight menuitem if current line is flagged as having a difference
void CMergeDiffDetailView::OnUpdateShowlinediff(CCmdUI* pCmdUI) 
{
	int line = GetCursorPos().y;
	BOOL enable = GetLineFlags(line) & LF_DIFF;
	pCmdUI->Enable(enable);
}

void CMergeDiffDetailView::PushCursors()
{
	// push lineBegin and the cursor
	m_lineBeginPushed = lineBegin;
	m_ptCursorPosPushed = m_ptCursorPos;
	// and top line positions
	m_nTopLinePushed = m_nTopLine;
}

void CMergeDiffDetailView::PopCursors()
{
	lineBegin = m_lineBeginPushed;
	lineEnd = lineBegin + diffLength - 1;

	m_ptCursorPos = m_ptCursorPosPushed;

	if (lineBegin >= GetLineCount())
	{
		// even the first line is invalid, stop displaying the diff
		lineBegin = m_nTopLine = m_nTopSubLine = 0;
		lineEnd = -1;
		diffLength = 0;
	}
	else
		{
		// just check that all positions all valid
		lineEnd = min(lineEnd, GetLineCount()-1);
		diffLength = lineEnd - lineBegin + 1;
		m_ptCursorPos.y = min(m_ptCursorPos.y, GetLineCount()-1);
		m_ptCursorPos.x = min(m_ptCursorPos.x, GetLineLength(m_ptCursorPos.y));
	}

		// restore the scrolling position
	if (m_nTopLinePushed >= GetLineCount())
		m_nTopLinePushed = GetLineCount()-1;
	ScrollToLine(m_nTopLinePushed);

	// other positions are set to (0,0) during ResetView
}
