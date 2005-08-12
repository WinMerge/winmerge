/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  MergeEditView.cpp
 *
 * @brief Implementation of the CMergeEditView class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "BCMenu.h"
#include "merge.h"
#include "LocationView.h"
#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "MergeDoc.h"
#include "MainFrm.h"
#include "WaitStatusCursor.h"
#include "MergeEditStatus.h"
#include "FileTransform.h"
#include "Plugins.h"
#include "lwdisp.h"
#include "WMGotoDlg.h"
#include "OptionsDef.h"
#include "SyntaxColors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Timer ID and timeout for delayed rescan
 */
const UINT IDT_RESCAN = 2;
const UINT RESCAN_TIMEOUT = 1000;

/////////////////////////////////////////////////////////////////////////////
// CMergeEditView

IMPLEMENT_DYNCREATE(CMergeEditView, CCrystalEditViewEx)

CMergeEditView::CMergeEditView()
{
	m_pLocationView = NULL;
	m_bIsLeft = FALSE;
	m_nModifications = 0;
	m_piMergeEditStatus = 0;
	SetParser(&m_xParser);
	m_bAutomaticRescan = FALSE;
	fTimerWaitingForIdle = 0;
	m_bCloseWithEsc = mf->m_options.GetBool(OPT_CLOSE_WITH_ESC);

	m_bSyntaxHighlight = mf->m_options.GetBool(OPT_SYNTAX_HIGHLIGHT);
	m_bWordDiffHighlight = mf->m_options.GetBool(OPT_WORDDIFF_HIGHLIGHT);
	m_cachedColors.clrDiff = mf->m_options.GetInt(OPT_CLR_DIFF);
	m_cachedColors.clrSelDiff = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF);
	m_cachedColors.clrDiffDeleted = mf->m_options.GetInt(OPT_CLR_DIFF_DELETED);
	m_cachedColors.clrSelDiffDeleted = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF_DELETED);
	m_cachedColors.clrDiffText = mf->m_options.GetInt(OPT_CLR_DIFF_TEXT);
	m_cachedColors.clrSelDiffText = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF_TEXT);
	m_cachedColors.clrTrivial = mf->m_options.GetInt(OPT_CLR_TRIVIAL_DIFF);
	m_cachedColors.clrTrivialDeleted = mf->m_options.GetInt(OPT_CLR_TRIVIAL_DIFF_DELETED);
	m_cachedColors.clrTrivialText = mf->m_options.GetInt(OPT_CLR_TRIVIAL_DIFF_TEXT);
	m_cachedColors.clrMoved = mf->m_options.GetInt(OPT_CLR_MOVEDBLOCK);
	m_cachedColors.clrMovedDeleted = mf->m_options.GetInt(OPT_CLR_MOVEDBLOCK_DELETED);
	m_cachedColors.clrMovedText = mf->m_options.GetInt(OPT_CLR_MOVEDBLOCK_TEXT);
	m_cachedColors.clrSelMoved = mf->m_options.GetInt(OPT_CLR_SELECTED_MOVEDBLOCK);
	m_cachedColors.clrSelMovedDeleted = mf->m_options.GetInt(OPT_CLR_SELECTED_MOVEDBLOCK_DELETED);
	m_cachedColors.clrSelMovedText = mf->m_options.GetInt(OPT_CLR_SELECTED_MOVEDBLOCK_TEXT);
	m_cachedColors.clrWordDiff = mf->m_options.GetInt(OPT_CLR_WORDDIFF);
	m_cachedColors.clrSelWordDiff = mf->m_options.GetInt(OPT_CLR_SELECTED_WORDDIFF);
	m_cachedColors.clrWordDiffText = mf->m_options.GetInt(OPT_CLR_WORDDIFF_TEXT);
	m_cachedColors.clrSelWordDiffText = mf->m_options.GetInt(OPT_CLR_SELECTED_WORDDIFF_TEXT);
}

CMergeEditView::~CMergeEditView()
{
}


BEGIN_MESSAGE_MAP(CMergeEditView, CCrystalEditViewEx)
	//{{AFX_MSG_MAP(CMergeEditView)
	ON_COMMAND(ID_CURDIFF, OnCurdiff)
	ON_UPDATE_COMMAND_UI(ID_CURDIFF, OnUpdateCurdiff)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_FIRSTDIFF, OnFirstdiff)
	ON_UPDATE_COMMAND_UI(ID_FIRSTDIFF, OnUpdateFirstdiff)
	ON_COMMAND(ID_LASTDIFF, OnLastdiff)
	ON_UPDATE_COMMAND_UI(ID_LASTDIFF, OnUpdateLastdiff)
	ON_COMMAND(ID_NEXTDIFF, OnNextdiff)
	ON_UPDATE_COMMAND_UI(ID_NEXTDIFF, OnUpdateNextdiff)
	ON_COMMAND(ID_PREVDIFF, OnPrevdiff)
	ON_UPDATE_COMMAND_UI(ID_PREVDIFF, OnUpdatePrevdiff)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_ALL_LEFT, OnAllLeft)
	ON_UPDATE_COMMAND_UI(ID_ALL_LEFT, OnUpdateAllLeft)
	ON_COMMAND(ID_ALL_RIGHT, OnAllRight)
	ON_UPDATE_COMMAND_UI(ID_ALL_RIGHT, OnUpdateAllRight)
	ON_COMMAND(ID_L2R, OnL2r)
	ON_UPDATE_COMMAND_UI(ID_L2R, OnUpdateL2r)
	ON_COMMAND(ID_R2L, OnR2l)
	ON_UPDATE_COMMAND_UI(ID_R2L, OnUpdateR2l)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_LEFT, OnUpdateFileSaveLeft)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_RIGHT, OnUpdateFileSaveRight)
	ON_COMMAND(ID_REFRESH, OnRefresh)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_SHOWLINEWORDDIFF, OnShowlineworddiff)
	ON_COMMAND(ID_SHOWLINECHARDIFF, OnShowlinechardiff)
	ON_UPDATE_COMMAND_UI(ID_SHOWLINEWORDDIFF, OnUpdateShowlineworddiff)
	ON_UPDATE_COMMAND_UI(ID_SHOWLINECHARDIFF, OnUpdateShowlinechardiff)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACE, OnUpdateEditReplace)
	ON_COMMAND(ID_FILE_LEFT_READONLY, OnLeftReadOnly)
	ON_UPDATE_COMMAND_UI(ID_FILE_LEFT_READONLY, OnUpdateLeftReadOnly)
	ON_COMMAND(ID_FILE_RIGHT_READONLY, OnRightReadOnly)
	ON_UPDATE_COMMAND_UI(ID_FILE_RIGHT_READONLY, OnUpdateRightReadOnly)
	ON_UPDATE_COMMAND_UI(ID_STATUS_LEFTFILE_RO, OnUpdateStatusLeftRO)
	ON_UPDATE_COMMAND_UI(ID_STATUS_RIGHTFILE_RO, OnUpdateStatusRightRO)
	ON_COMMAND_RANGE(ID_EOL_TO_DOS, ID_EOL_TO_MAC, OnConvertEolTo)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EOL_TO_DOS, ID_EOL_TO_MAC, OnUpdateConvertEolTo)
	ON_UPDATE_COMMAND_UI(ID_STATUS_LEFTFILE_EOL, OnUpdateStatusLeftEOL)
	ON_UPDATE_COMMAND_UI(ID_STATUS_RIGHTFILE_EOL, OnUpdateStatusRightEOL)
	ON_COMMAND(ID_L2RNEXT, OnL2RNext)
	ON_UPDATE_COMMAND_UI(ID_L2RNEXT, OnUpdateL2RNext)
	ON_COMMAND(ID_R2LNEXT, OnR2LNext)
	ON_UPDATE_COMMAND_UI(ID_R2LNEXT, OnUpdateR2LNext)
	ON_COMMAND(ID_WINDOW_CHANGE_PANE, OnChangePane)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CHANGE_PANE, OnUpdateChangePane)
	ON_COMMAND(ID_EDIT_WMGOTO, OnWMGoto)
	ON_UPDATE_COMMAND_UI(ID_EDIT_WMGOTO, OnUpdateWMGoto)
	ON_COMMAND_RANGE(ID_SCRIPT_FIRST, ID_SCRIPT_LAST, OnScripts)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SCRIPT_FIRST, ID_SCRIPT_LAST, OnUpdateScripts)
	ON_COMMAND(ID_NO_PREDIFFER, OnNoPrediffer)
	ON_UPDATE_COMMAND_UI(ID_NO_PREDIFFER, OnUpdatePrediffer)
	ON_COMMAND_RANGE(ID_PREDIFFERS_FIRST, ID_PREDIFFERS_LAST, OnPrediffer)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PREDIFFERS_FIRST, ID_PREDIFFERS_LAST, OnUpdatePrediffer)
	ON_COMMAND(ID_FILE_MERGINGMODE, OnMergingMode)
	ON_UPDATE_COMMAND_UI(ID_FILE_MERGINGMODE, OnUpdateMergingMode)
	ON_UPDATE_COMMAND_UI(ID_STATUS_MERGINGMODE, OnUpdateMergingStatus)
	ON_COMMAND(ID_FILE_CLOSE, OnWindowClose)
	ON_WM_VSCROLL ()
	ON_COMMAND(ID_EDIT_COPY_LINENUMBERS, OnEditCopyLineNumbers)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_LINENUMBERS, OnUpdateEditCopyLinenumbers)
	ON_COMMAND(ID_VIEW_LINEDIFFS, OnViewLineDiffs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINEDIFFS, OnUpdateViewLineDiffs)
	ON_COMMAND(ID_FILE_OPEN_REGISTERED, OnOpenFile)
	ON_COMMAND(ID_FILE_OPEN_WITHEDITOR, OnOpenFileWithEditor)
	ON_COMMAND(ID_FILE_OPEN_WITH, OnOpenFileWith)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMergeEditView diagnostics

#ifdef _DEBUG
CMergeDoc* CMergeEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMergeDoc)));
	return (CMergeDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMergeEditView message handlers

/**
 * @brief Return text buffer for file in view
 */
CCrystalTextBuffer *CMergeEditView::LocateTextBuffer()
{
	if (m_bIsLeft)
		return &GetDocument()->m_ltBuf;
	return &GetDocument()->m_rtBuf;
}

/**
 * @brief Scroll to line
 * @todo Unused function?
 */
void CMergeEditView::DoScroll(UINT code, UINT pos, BOOL bDoScroll)
{
	TRACE(_T("Scroll %s: pos=%d\n"), m_bIsLeft? _T("left"):_T("right"), pos);
	if (bDoScroll && (code == SB_THUMBPOSITION || code == SB_THUMBTRACK))
	{
		ScrollToLine(pos);
	}
	CCrystalEditViewEx::OnVScroll(code, pos, NULL);
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMergeEditView::UpdateResources()
{
}

BOOL CMergeEditView::PrimeListWithFile()
{
	// Set the tab size now, just in case the options change...
	// We don't update it at the end of OnOptions,
	// we can update it safely now
	SetTabSize(mf->m_options.GetInt(OPT_TAB_SIZE));

	return TRUE;
}
/**
 * @brief Return text from line given
 */
CString CMergeEditView::GetLineText(int idx)
{
	return GetLineChars(idx);
}

/**
 * @brief Return text from selection
 */
CString CMergeEditView::GetSelectedText()
{
	CPoint ptStart, ptEnd;
	CString strText;
	GetSelection(ptStart, ptEnd);
	if (ptStart != ptEnd)
		GetTextWithoutEmptys(ptStart.y, ptStart.x, ptEnd.y, ptEnd.x, strText);
	return strText;
}

/**
 * @brief Get diffs inside selection.
 * @param [out] firstDiff First diff inside selection
 * @param [out] lastDiff Last diff inside selection
 * @todo This shouldn't be called when there is no diffs, so replace
 * first 'if' with ASSERT()?
 */
void CMergeEditView::GetFullySelectedDiffs(int & firstDiff, int & lastDiff)
{
	firstDiff = 0;
	lastDiff = -1;

	CMergeDoc *pd = GetDocument();
	const int nDiffs = pd->m_diffList.GetSize();
	if (nDiffs == 0)
		return;

	int firstLine, lastLine;
	GetFullySelectedLines(firstLine, lastLine);
	if (lastLine < firstLine)
		return;

	for (UINT i = 0; i < nDiffs; i++)
	{
		DIFFRANGE curDiff;
		VERIFY(pd->m_diffList.GetDiff(i, curDiff));
		if ((int)curDiff.dbegin0 >= firstLine)
		{
			firstDiff = i;
			break;
		}
	}
	if (i == nDiffs)
		return;

	lastDiff = nDiffs - 1;
	for (i = firstDiff; i < nDiffs; i++)
	{
		DIFFRANGE curDiff;
		VERIFY(pd->m_diffList.GetDiff(i, curDiff));
		if ((int)curDiff.dend0 > lastLine)
		{
				lastDiff = i-1;
				break;
		}
	}
}

void CMergeEditView::AddMod()
{

}

void CMergeEditView::ResetMod()
{

}

void CMergeEditView::OnInitialUpdate()
{
	CCrystalEditViewEx::OnInitialUpdate();
	SetFont(dynamic_cast<CMainFrame*>(AfxGetMainWnd())->m_lfDiff);
}

void CMergeEditView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	CCrystalEditViewEx::OnActivateView(bActivate, pActivateView, pDeactiveView);

	CMergeDoc* pDoc = GetDocument();
	pDoc->UpdateHeaderActivity(m_bIsLeft, bActivate);
	mf->UpdatePrediffersMenu();
}

int CMergeEditView::GetAdditionalTextBlocks (int nLineIndex, TEXTBLOCK *pBuf)
{
	DWORD dwLineFlags = GetLineFlags(nLineIndex);
	if ((dwLineFlags & LF_DIFF) != LF_DIFF || (dwLineFlags & LF_MOVED) == LF_MOVED)
		return 0;

	if (!m_bWordDiffHighlight)
		return 0;

	int nLineLength = GetLineLength(nLineIndex);
	wdiffarray worddiffs;
	GetDocument()->GetWordDiffArray(nLineIndex, &worddiffs);
	if (worddiffs.GetSize() == 0 || (worddiffs[0].end[0] == -1 && worddiffs[0].end[1] + 1 == nLineLength) || (worddiffs[0].end[1] == -1 && worddiffs[0].end[0] + 1 == nLineLength))
		return 0;

	BOOL lineInCurrentDiff = IsLineInCurrentDiff(nLineIndex);
	int nWordDiffs = worddiffs.GetSize();

	pBuf[0].m_nCharPos = 0;
	pBuf[0].m_nColorIndex = COLORINDEX_NONE;
	pBuf[0].m_nBgColorIndex = COLORINDEX_NONE;
	for (int i = 0; i < nWordDiffs; i++)
	{
		if (m_bIsLeft)
		{
			pBuf[1 + i * 2].m_nCharPos = worddiffs[i].start[0];
			pBuf[2 + i * 2].m_nCharPos = worddiffs[i].end[0] + 1;
		}
		else
		{
			pBuf[1 + i * 2].m_nCharPos = worddiffs[i].start[1];
			pBuf[2 + i * 2].m_nCharPos = worddiffs[i].end[1] + 1;
		}
		if (lineInCurrentDiff)
		{
			pBuf[1 + i * 2].m_nColorIndex = COLORINDEX_HIGHLIGHTTEXT1 | COLORINDEX_APPLYFORCE;
			pBuf[1 + i * 2].m_nBgColorIndex = COLORINDEX_HIGHLIGHTBKGND1 | COLORINDEX_APPLYFORCE;
		}
		else
		{
			pBuf[1 + i * 2].m_nColorIndex = COLORINDEX_HIGHLIGHTTEXT2 | COLORINDEX_APPLYFORCE;
			pBuf[1 + i * 2].m_nBgColorIndex = COLORINDEX_HIGHLIGHTBKGND2 | COLORINDEX_APPLYFORCE;
		}
		pBuf[2 + i * 2].m_nColorIndex = COLORINDEX_NONE;
		pBuf[2 + i * 2].m_nBgColorIndex = COLORINDEX_NONE;
	}
	return nWordDiffs * 2 + 1;
}

COLORREF CMergeEditView::GetColor(int nColorIndex)
{
	switch (nColorIndex & ~COLORINDEX_APPLYFORCE)
	{
	case COLORINDEX_HIGHLIGHTBKGND1:
		return m_cachedColors.clrSelWordDiff;
	case COLORINDEX_HIGHLIGHTTEXT1:
		return m_cachedColors.clrSelWordDiffText;
	case COLORINDEX_HIGHLIGHTBKGND2:
		return m_cachedColors.clrWordDiff;
	case COLORINDEX_HIGHLIGHTTEXT2:
		return m_cachedColors.clrWordDiffText;
	default:
		return CCrystalTextView::GetColor(nColorIndex);
	}
}

/**
 * @brief Determine text and background color for line
 * @param [in] nLineIndex Index of line in view (NOT line in file)
 * @param [out] crBkgnd Backround color for line
 * @param [out] crText Text color for line
 */
void CMergeEditView::GetLineColors(int nLineIndex, COLORREF & crBkgnd,
                                COLORREF & crText, BOOL & bDrawWhitespace)
{
	DWORD ignoreFlags = 0;
	GetLineColors2(nLineIndex, ignoreFlags, crBkgnd, crText, bDrawWhitespace);
}

/**
 * @brief Determine text and background color for line
 * @param [in] nLineIndex Index of line in view (NOT line in file)
 * @param [in] ignoreFlags Flags that caller wishes ignored
 * @param [out] crBkgnd Backround color for line
 * @param [out] crText Text color for line
 *
 * This version allows caller to suppress particular flags
 */
void CMergeEditView::GetLineColors2(int nLineIndex, DWORD ignoreFlags, COLORREF & crBkgnd,
                                COLORREF & crText, BOOL & bDrawWhitespace)
{
	DWORD dwLineFlags = GetLineFlags(nLineIndex);

	if (dwLineFlags & ignoreFlags)
		dwLineFlags &= (~ignoreFlags);

	// Line inside diff
	if (dwLineFlags & LF_WINMERGE_FLAGS)
	{
		crText = m_cachedColors.clrDiffText;
		bDrawWhitespace = TRUE;
		BOOL lineInCurrentDiff = IsLineInCurrentDiff(nLineIndex);

		if (dwLineFlags & LF_DIFF)
		{
			if (lineInCurrentDiff)
			{
				if (dwLineFlags & LF_MOVED)
				{
					if (dwLineFlags & LF_GHOST)
						crBkgnd = m_cachedColors.clrSelMovedDeleted;
					else
						crBkgnd = m_cachedColors.clrSelMoved;
					crText = m_cachedColors.clrSelMovedText;
				}
				else
				{
					crBkgnd = m_cachedColors.clrSelDiff;
					crText = m_cachedColors.clrSelDiffText;
				}
			
			}
			else
			{
				if (dwLineFlags & LF_MOVED)
				{
					if (dwLineFlags & LF_GHOST)
						crBkgnd = m_cachedColors.clrMovedDeleted;
					else
						crBkgnd = m_cachedColors.clrMoved;
					crText = m_cachedColors.clrMovedText;
				}
				else
				{
					crBkgnd = m_cachedColors.clrDiff;
					crText = m_cachedColors.clrDiffText;
				}
			}
			return;
		}
		else if (dwLineFlags & LF_TRIVIAL)
		{
			// trivial diff can not be selected
			if (dwLineFlags & LF_GHOST)
				// ghost lines in trivial diff has their own color
				crBkgnd = m_cachedColors.clrTrivialDeleted;
			else
				crBkgnd = m_cachedColors.clrTrivial;
			return;
		}
		else if (dwLineFlags & LF_GHOST)
		{
			if (lineInCurrentDiff)
				crBkgnd = m_cachedColors.clrSelDiffDeleted;
			else
				crBkgnd = m_cachedColors.clrDiffDeleted;
			return;
		}
	}
	else
	{
		// Line not inside diff,
		if (!m_bSyntaxHighlight)
		{
			// If no syntax hilighting, get windows default colors
			crBkgnd = GetSysColor (COLOR_WINDOW);
			crText = GetSysColor (COLOR_WINDOWTEXT);
			bDrawWhitespace = FALSE;
		}
		else
			// Syntax highlighting, get colors from CrystalEditor
			CCrystalEditViewEx::GetLineColors(nLineIndex, crBkgnd,
				crText, bDrawWhitespace);
	}
}

/**
 * @brief Sync other pane position
 */
void CMergeEditView::UpdateSiblingScrollPos (BOOL bHorz)
{
	CSplitterWnd *pSplitterWnd = GetParentSplitter (this, FALSE);
	if (pSplitterWnd != NULL)
	{
		//  See CSplitterWnd::IdFromRowCol() implementation for details
		int nCurrentRow = (GetDlgCtrlID () - AFX_IDW_PANE_FIRST) / 16;
		int nCurrentCol = (GetDlgCtrlID () - AFX_IDW_PANE_FIRST) % 16;
		ASSERT (nCurrentRow >= 0 && nCurrentRow < pSplitterWnd->GetRowCount ());
		ASSERT (nCurrentCol >= 0 && nCurrentCol < pSplitterWnd->GetColumnCount ());

		// limit the TopLine : must be smaller than GetLineCount for all the panels
		int newTopLine = m_nTopLine;
		int nRows = pSplitterWnd->GetRowCount ();
		int nCols = pSplitterWnd->GetColumnCount ();
		for (int nRow = 0; nRow < nRows; nRow++)
		{
			for (int nCol = 0; nCol < nCols; nCol++)
			{
				CMergeEditView *pSiblingView = static_cast<CMergeEditView*>(GetSiblingView (nRow, nCol));
				if (pSiblingView != NULL)
					if (pSiblingView->GetLineCount() <= newTopLine)
						newTopLine = pSiblingView->GetLineCount()-1;
			}
		}
		if (m_nTopLine != newTopLine)
			ScrollToLine(newTopLine);

		for (nRow = 0; nRow < nRows; nRow++)
		{
			for (int nCol = 0; nCol < nCols; nCol++)
			{
				if (!(nRow == nCurrentRow && nCol == nCurrentCol))  //  We don't need to update ourselves
				{
					CMergeEditView *pSiblingView = static_cast<CMergeEditView*>(GetSiblingView (nRow, nCol));
					if (pSiblingView != NULL)
						pSiblingView->OnUpdateSibling (this, bHorz);
				}
			}
		}
	}
}

/**
 * @brief Update other panes
 */
void CMergeEditView::OnUpdateSibling (CCrystalTextView * pUpdateSource, BOOL bHorz)
{
	if (pUpdateSource != this)
	{
		ASSERT (pUpdateSource != NULL);
		ASSERT_KINDOF (CCrystalTextView, pUpdateSource);
		CMergeEditView *pSrcView = static_cast<CMergeEditView*>(pUpdateSource);
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

/**
 * @brief Selects diff by number and syncs other file
 * @param [in] nDiff Diff to select, must be >= 0
 * @param [in] bScroll Scroll diff to view
 * @param [in] bSelectText Select diff text
 * @sa CMergeEditView::ShowDiff()
 * @sa CMergeDoc::SetCurrentDiff()
 * @todo Parameter bSelectText is never used?
 */
void CMergeEditView::SelectDiff(int nDiff, BOOL bScroll /*=TRUE*/, BOOL bSelectText /*=TRUE*/)
{
	CMergeDoc *pd = GetDocument();

	// Check that nDiff is valid
	if (nDiff < 0)
		_RPTF1(_CRT_ERROR, "Diffnumber negative (%d)", nDiff);
	if (nDiff >= pd->m_diffList.GetSize())
		_RPTF2(_CRT_ERROR, "Selected diff > diffcount (%d >= %d)",
			nDiff, pd->m_diffList.GetSize());

	SelectNone();
	pd->SetCurrentDiff(nDiff);
	ShowDiff(bScroll, bSelectText);
	pd->UpdateAllViews(this);
	UpdateSiblingScrollPos(FALSE);

	// notify either side, as it will notify the other one
	pd->GetLeftDetailView()->OnDisplayDiff(nDiff);
}

/**
 * @brief Called when user selects "Current Difference".
 * Goes to active diff. If no active diff, selects diff under cursor
 * @sa CMergeEditView::SelectDiff()
 * @sa CMergeDoc::GetCurrentDiff()
 * @sa CMergeDoc::LineToDiff()
 */
void CMergeEditView::OnCurdiff()
{
	CMergeDoc *pd = GetDocument();

	// If no diffs, nothing to select
	if (pd->m_diffList.GetSize() == 0)
		return;

	// GetCurrentDiff() returns -1 if no diff selected
	int nDiff = pd->GetCurrentDiff();
	if (nDiff != -1)
	{
		// Scroll to the first line of the currently selected diff
		SelectDiff(nDiff, TRUE, FALSE);
	}
	else
	{
		// If cursor is inside diff, select that diff
		CPoint pos = GetCursorPos();
		nDiff = pd->m_diffList.LineToDiff(pos.y);
		if (nDiff != -1)
			SelectDiff(nDiff, TRUE, FALSE);
	}
}

/**
 * @brief Called when "Current diff" item is updated
 */
void CMergeEditView::OnUpdateCurdiff(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	CPoint pos = GetCursorPos();
	int nCurrentDiff = pd->GetCurrentDiff();
	if (nCurrentDiff == -1)
	{
		if (pd->m_diffList.LineToDiff(pos.y) == -1)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(TRUE);
}

/**
 * @brief Copy selected text to clipboard
 */
void CMergeEditView::OnEditCopy()
{
	CMergeDoc * pDoc = GetDocument();
	CPoint ptSelStart, ptSelEnd;
	GetSelection(ptSelStart, ptSelEnd);

	// Nothing selected
	if (ptSelStart == ptSelEnd)
		return;

	CString text;

	CMergeDoc::CDiffTextBuffer * buffer
		= m_bIsLeft ? &pDoc->m_ltBuf : &pDoc->m_rtBuf;

	buffer->GetTextWithoutEmptys(ptSelStart.y, ptSelStart.x,
		ptSelEnd.y, ptSelEnd.x, text);

	PutToClipboard(text);
}

/**
 * @brief Called when "Copy" item is updated
 */
void CMergeEditView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	CCrystalEditViewEx::OnUpdateEditCopy(pCmdUI);
}

/**
 * @brief Cut current selection to clipboard
 */
void CMergeEditView::OnEditCut()
{
	if (IsReadOnly(m_bIsLeft))
		return;

	CPoint ptSelStart, ptSelEnd;
	CMergeDoc * pDoc = GetDocument();
	GetSelection(ptSelStart, ptSelEnd);

	// Nothing selected
	if (ptSelStart == ptSelEnd)
		return;

	CString text;
	if (m_bIsLeft)
		pDoc->m_ltBuf.GetTextWithoutEmptys(ptSelStart.y, ptSelStart.x,
			ptSelEnd.y, ptSelEnd.x, text);
	else
		pDoc->m_rtBuf.GetTextWithoutEmptys(ptSelStart.y, ptSelStart.x,
			ptSelEnd.y, ptSelEnd.x, text);

	PutToClipboard(text);

	CPoint ptCursorPos = ptSelStart;
	ASSERT_VALIDTEXTPOS(ptCursorPos);
	SetAnchor(ptCursorPos);
	SetSelection(ptCursorPos, ptCursorPos);
	SetCursorPos(ptCursorPos);
	EnsureVisible(ptCursorPos);

	if (m_bIsLeft)
		pDoc->m_ltBuf.DeleteText(this, ptSelStart.y, ptSelStart.x, ptSelEnd.y,
			ptSelEnd.x, CE_ACTION_CUT);
	else
		pDoc->m_rtBuf.DeleteText(this, ptSelStart.y, ptSelStart.x, ptSelEnd.y,
			ptSelEnd.x, CE_ACTION_CUT);

	m_pTextBuffer->SetModified(TRUE);
}

/**
 * @brief Called when "Cut" item is updated
 */
void CMergeEditView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	if (!IsReadOnly(m_bIsLeft))
		CCrystalEditViewEx::OnUpdateEditCut(pCmdUI);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Paste text from clipboard
 */
void CMergeEditView::OnEditPaste()
{
	if (IsReadOnly(m_bIsLeft))
		return;

	CCrystalEditViewEx::Paste();
	m_pTextBuffer->SetModified(TRUE);
}

/**
 * @brief Called when "Paste" item is updated
 */
void CMergeEditView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	if (!IsReadOnly(m_bIsLeft))
		CCrystalEditViewEx::OnUpdateEditPaste(pCmdUI);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Undo last action
 */
void CMergeEditView::OnEditUndo()
{
	CMergeDoc* pDoc = GetDocument();
	CMergeEditView *tgt = *(pDoc->curUndo-1);
	if(tgt==this)
	{
		if (IsReadOnly(m_bIsLeft))
			return;

		GetParentFrame()->SetActiveView(this, TRUE);
		if(CCrystalEditViewEx::DoEditUndo())
		{
			--pDoc->curUndo;
			pDoc->UpdateHeaderPath(m_bIsLeft);
			pDoc->FlushAndRescan();

			int nAction;
			m_pTextBuffer->GetRedoActionCode(nAction);
			if (nAction == CE_ACTION_MERGE)
				// select the diff so we may just merge it again
				OnCurdiff();
		}
	}
	else
	{
		tgt->SendMessage(WM_COMMAND, ID_EDIT_UNDO);
	}
}

/**
 * @brief Called when "Undo" item is updated
 */
void CMergeEditView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CMergeDoc* pDoc = GetDocument();
	if (pDoc->curUndo!=pDoc->undoTgt.begin())
	{
		CMergeEditView *tgt = *(pDoc->curUndo-1);
		pCmdUI->Enable( !IsReadOnly(tgt->m_bIsLeft));
	}
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Go to first diff
 *
 * Called when user selects "First Difference"
 * @sa CMergeEditView::SelectDiff()
 */
void CMergeEditView::OnFirstdiff()
{
	CMergeDoc *pd = GetDocument();
	if (pd->m_diffList.GetSize() > 0)
		SelectDiff(0, TRUE, FALSE);
}

/**
 * @brief Update "First diff" UI items
 */
void CMergeEditView::OnUpdateFirstdiff(CCmdUI* pCmdUI)
{
	OnUpdatePrevdiff(pCmdUI);
}

/**
 * @brief Go to last diff
 */
void CMergeEditView::OnLastdiff()
{
	CMergeDoc *pd = GetDocument();
	if (pd->m_diffList.GetSize() > 0)
		SelectDiff(pd->m_diffList.GetSize() - 1, TRUE, FALSE);
}

/**
 * @brief Update "Last diff" UI items
 */
void CMergeEditView::OnUpdateLastdiff(CCmdUI* pCmdUI)
{
	OnUpdateNextdiff(pCmdUI);
}

/**
 * @brief Go to next diff
 * @note If no diff selected, next diff below cursor
 * is selected.
 */
void CMergeEditView::OnNextdiff()
{
	CMergeDoc *pd = GetDocument();
	int cnt = pd->m_ltBuf.GetLineCount();
	if (cnt <= 0)
		return;

	// Returns -1 if no diff selected
	int curDiff = pd->GetCurrentDiff();
	if (curDiff != -1)
	{
		// We're on a diff
		// Find out if there is a following significant diff
		int nextDiff = curDiff;
		if (curDiff < pd->m_diffList.GetSize() - 1)
		{
			nextDiff = pd->m_diffList.NextSignificantDiff(curDiff);
			if (nextDiff == -1)
				nextDiff = curDiff;
		}
		// nextDiff is the next one if there is one, else it is the one we're on
		SelectDiff(nextDiff, TRUE, FALSE);
	}
	else
	{
		// we're not on a diff, so figure out which one to select
		int line = GetCursorPos().y;
		if (!IsValidTextPosY(CPoint(0, line)))
			line = m_nTopLine;
		curDiff = pd->m_diffList.NextSignificantDiffFromLine(line);
		if (curDiff >= 0)
			SelectDiff(curDiff, TRUE, FALSE);
	}
}

/**
 * @brief Update "Next diff" UI items
 */
void CMergeEditView::OnUpdateNextdiff(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	const DIFFRANGE * dfi = pd->m_diffList.LastSignificantDiffRange();

	if (!dfi)
	{
		// There aren't any significant differences
		pCmdUI->Enable(FALSE);
	}
	else
	{
		// Enable if the beginning of the last significant difference is after caret
		CPoint pos = GetCursorPos();
		pCmdUI->Enable(pos.y < (long)dfi->dbegin0);
	}
}

/**
 * @brief Goes to previous diff and selects it.
 *
 * Called when user selects "Previous Difference".
 * @note If no diff is selected, previous diff above cursor
 * is selected.
 * @sa CMergeEditView::SelectDiff()
 */
void CMergeEditView::OnPrevdiff()
{
	CMergeDoc *pd = GetDocument();
	int cnt = pd->m_ltBuf.GetLineCount();
	if (cnt <= 0)
		return;

	// GetCurrentDiff() returns -1 if no diff selected
	int curDiff = pd->GetCurrentDiff();

	if (curDiff != -1)
	{
		// We're on a diff
		// Find out if there is a preceding significant diff
		int prevDiff = curDiff;
		if (curDiff > 0)
		{
			prevDiff = pd->m_diffList.PrevSignificantDiff(curDiff);
			if (prevDiff == -1)
				prevDiff = curDiff;
		}
		// prevDiff is the preceding one if there is one, else it is the one we're on
		SelectDiff(prevDiff, TRUE, FALSE);
	}
	else
	{
		// we're not on a diff, so figure out which one to select
		int line = GetCursorPos().y;
		if (!IsValidTextPosY(CPoint(0, line)))
			line = m_nTopLine;
		curDiff = pd->m_diffList.PrevSignificantDiffFromLine(line);
		if (curDiff >= 0)
			SelectDiff(curDiff, TRUE, FALSE);
	}
}

/**
 * @brief Update "Previous diff" UI items
 */
void CMergeEditView::OnUpdatePrevdiff(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	const DIFFRANGE * dfi = pd->m_diffList.FirstSignificantDiffRange();

	if (!dfi)
	{
		// There aren't any significant differences
		pCmdUI->Enable(FALSE);
	}
	else
	{
		// Enable if the end of the first significant difference is before caret
		CPoint pos = GetCursorPos();
		pCmdUI->Enable(pos.y > (long)dfi->dend0);
	}
}

/**
 * @brief Clear selection
 */
void CMergeEditView::SelectNone()
{
	SetSelection (GetCursorPos(), GetCursorPos());
	UpdateCaret();
}

/**
 * @brief Check if line is inside currently selected diff
 * @param [in] nLine 0-based linenumber in view
 * @sa CMergeDoc::GetCurrentDiff()
 * @sa CMergeDoc::LineInDiff()
 */
BOOL CMergeEditView::IsLineInCurrentDiff(int nLine)
{
	// Check validity of nLine
#ifdef _DEBUG
	if (nLine < 0)
		_RPTF1(_CRT_ERROR, "Linenumber is negative (%d)!", nLine);
	int nLineCount = LocateTextBuffer()->GetLineCount();
	if (nLine >= nLineCount)
		_RPTF2(_CRT_ERROR, "Linenumber > linecount (%d>%d)!", nLine, nLineCount);
#endif

	CMergeDoc *pd = GetDocument();
	int curDiff = pd->GetCurrentDiff();
	if (curDiff == -1)
		return FALSE;
	return pd->m_diffList.LineInDiff(nLine, curDiff);
}

/**
 * @brief Called when mouse left-button double-clicked
 *
 * Double-clicking mouse inside diff selects that diff
 */
void CMergeEditView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMergeDoc *pd = GetDocument();
	CPoint pos = GetCursorPos();

	int diff = pd->m_diffList.LineToDiff(pos.y);
	if (diff != -1)
		SelectDiff(diff, FALSE, FALSE);

	CCrystalEditViewEx::OnLButtonDblClk(nFlags, point);
}

/**
 * @brief Called when mouse left button is released.
 *
 * If button is released outside diffs, current diff
 * is deselected.
 */
void CMergeEditView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CMergeDoc *pd = GetDocument();
	CCrystalEditViewEx::OnLButtonUp(nFlags, point);

	// If we have a selected diff, deselect it
	int nCurrentDiff = pd->GetCurrentDiff();
	if (nCurrentDiff != -1)
	{
		CPoint pos = GetCursorPos();
		if (!IsLineInCurrentDiff(pos.y))
		{
			pd->SetCurrentDiff(-1);
			Invalidate();
			pd->UpdateAllViews(this);
		}
	}
}

/**
 * @brief Finds longest line (needed for scrolling etc).
 * @sa CCrystalTextView::GetMaxLineLength()
 */
void CMergeEditView::UpdateLineLengths()
{
	GetMaxLineLength();
}

/**
 * @brief Copy diff from left pane to right pane
 *
 * Difference is copied from left to right when
 * - difference is selected
 * - difference is inside selection (allows merging multiple differences).
 *
 * If there is selected diff outside selection, we copy selected
 * difference only.
 */
void CMergeEditView::OnL2r()
{
	// Check that right side is not readonly
	if (IsReadOnly(FALSE))
		return;

	CMergeDoc *pDoc = GetDocument();
	int currentDiff = pDoc->GetCurrentDiff();
	int firstDiff, lastDiff;
	GetFullySelectedDiffs(firstDiff, lastDiff);

	if (lastDiff >= firstDiff)
	{
		WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYL2R));
		if (currentDiff != -1)
			pDoc->ListCopy(true, !!m_bIsLeft);
		else
			pDoc->CopyMultipleList(true, !!m_bIsLeft, firstDiff, lastDiff);
	}
	else if (currentDiff != -1)
	{
		WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYL2R));
		pDoc->ListCopy(true, !!m_bIsLeft);
	}
}

/**
 * @brief Called when "Copy to left" item is updated
 */
void CMergeEditView::OnUpdateL2r(CCmdUI* pCmdUI)
{
	// Check that right side is not readonly
	if (!IsReadOnly(FALSE))
	{
		int firstDiff, lastDiff;
		GetFullySelectedDiffs(firstDiff, lastDiff);

		// If one or more diffs inside selection OR
		// there is an active diff
		if (lastDiff >= firstDiff)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
	}
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Copy diff from right pane to left pane
 *
 * Difference is copied from left to right when
 * - difference is selected
 * - difference is inside selection (allows merging multiple differences).
 *
 * If there is selected diff outside selection, we copy selected
 * difference only.
 */
void CMergeEditView::OnR2l()
{
	// Check that left side is not readonly
	if (IsReadOnly(TRUE))
		return;

	CMergeDoc *pDoc = GetDocument();
	int currentDiff = pDoc->GetCurrentDiff();
	int firstDiff, lastDiff;
	GetFullySelectedDiffs(firstDiff, lastDiff);

	if (lastDiff >= firstDiff)
	{
		WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYR2L));
		if (currentDiff != -1)
			pDoc->ListCopy(false, !!m_bIsLeft);
		else
			pDoc->CopyMultipleList(false, !!m_bIsLeft, firstDiff, lastDiff);
	}
	else if (currentDiff != -1)
	{
		WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYR2L));
		pDoc->ListCopy(false, !!m_bIsLeft);
	}
}

/**
 * @brief Called when "Copy to right" item is updated
 */
void CMergeEditView::OnUpdateR2l(CCmdUI* pCmdUI)
{
	// Check that left side is not readonly
	if (!IsReadOnly(TRUE))
	{
		int firstDiff, lastDiff;
		GetFullySelectedDiffs(firstDiff, lastDiff);

		// If one or more diffs inside selection OR
		// there is an active diff
		if (lastDiff >= firstDiff)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
	}
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Copy all diffs from right pane to left pane
 */
void CMergeEditView::OnAllLeft()
{
	// Check that left side is not readonly
	if (IsReadOnly(TRUE))
		return;
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYALL2L));

	GetDocument()->CopyAllList(false, !!m_bIsLeft);
}

/**
 * @brief Called when "Copy all to left" item is updated
 */
void CMergeEditView::OnUpdateAllLeft(CCmdUI* pCmdUI)
{
	// Check that left side is not readonly
	if (!IsReadOnly(TRUE))
		pCmdUI->Enable(GetDocument()->m_diffList.GetSize() != 0);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Copy all diffs from left pane to right pane
 */
void CMergeEditView::OnAllRight()
{
	// Check that right side is not readonly
	if (IsReadOnly(FALSE))
		return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYALL2R));

	GetDocument()->CopyAllList(true, !!m_bIsLeft);
}

/**
 * @brief Called when "Copy all to right" item is updated
 */
void CMergeEditView::OnUpdateAllRight(CCmdUI* pCmdUI)
{
	// Check that right side is not readonly
	if (!IsReadOnly(FALSE))
		pCmdUI->Enable(GetDocument()->m_diffList.GetSize() != 0);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief This function is called before other edit events.
 * @param [in] nAction Edit operation to do
 * @param [in] pszText Text to insert, delete etc
 * @sa CCrystalEditView::OnEditOperation()
 * @todo More edit-events for rescan delaying?
 */
void CMergeEditView::OnEditOperation(int nAction, LPCTSTR pszText)
{
	if (IsReadOnly(m_bIsLeft))
	{
		// We must not arrive here, and assert helps detect troubles
		ASSERT(0);
		return;
	}

	CMergeDoc* pDoc = GetDocument();
	pDoc->SetEditedAfterRescan(m_bIsLeft);

	// simple hook for multiplex undo operations
	// deleted by jtuc 2003-06-28
	// now AddUndoRecords does it (so we don't create entry for OnEditOperation with no Undo data in m_pTextBuffer)
	/*if(dynamic_cast<CMergeDoc::CDiffTextBuffer*>(m_pTextBuffer)->curUndoGroup())
	{
		pDoc->undoTgt.erase(pDoc->curUndo, pDoc->undoTgt.end());
		pDoc->undoTgt.push_back(this);
		pDoc->curUndo = pDoc->undoTgt.end();
	}*/

	// perform original function
	CCrystalEditViewEx::OnEditOperation(nAction, pszText);

	// augment with additional operations

	// Change header to inform about changed doc
	pDoc->UpdateHeaderPath(m_bIsLeft);

	// If automatic rescan enabled, rescan after edit events
	if (m_bAutomaticRescan)
	{
		// keep document up to date
		// (Re)start timer to rescan only when user edits text
		// If timer starting fails, rescan immediately
		if (nAction == CE_ACTION_TYPING ||
			nAction == CE_ACTION_REPLACE ||
			nAction == CE_ACTION_BACKSPACE ||
			nAction == CE_ACTION_INDENT ||
			nAction == CE_ACTION_PASTE ||
			nAction == CE_ACTION_DELSEL ||
			nAction == CE_ACTION_DELETE ||
			nAction == CE_ACTION_CUT)
		{
			if (!SetTimer(IDT_RESCAN, RESCAN_TIMEOUT, NULL))
				pDoc->FlushAndRescan();
		}
		else
			pDoc->FlushAndRescan();
	}
}

/**
 * @brief Redo last action
 */
void CMergeEditView::OnEditRedo()
{
	CMergeDoc* pDoc = GetDocument();
	CMergeEditView *tgt = *(pDoc->curUndo);
	if(tgt==this)
	{
		if (IsReadOnly(m_bIsLeft))
			return;

		GetParentFrame()->SetActiveView(this, TRUE);
		if(CCrystalEditViewEx::DoEditRedo())
		{
			++pDoc->curUndo;
			pDoc->UpdateHeaderPath(m_bIsLeft);
			pDoc->FlushAndRescan();
		}
	}
	else
	{
		tgt->SendMessage(WM_COMMAND, ID_EDIT_REDO);
	}
}

/**
 * @brief Called when "Redo" item is updated
 */
void CMergeEditView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	CMergeDoc* pDoc = GetDocument();
	if (pDoc->curUndo!=pDoc->undoTgt.end())
	{
		CMergeEditView *tgt = *(pDoc->curUndo);
		pCmdUI->Enable( !IsReadOnly(tgt->m_bIsLeft));
	}
	else
		pCmdUI->Enable(FALSE);
}

void CMergeEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CCrystalEditViewEx::OnUpdate(pSender, lHint, pHint);
}

/**
 * @brief Scrolls to current diff and/or selects diff text
 * @param [in] bScroll If TRUE scroll diff to view
 * @param [in] bSelectText If TRUE select diff text
 * @note If bScroll and bSelectText are FALSE, this does nothing!
 * @todo This shouldn't be called when no diff is selected, so
 * somebody could try to ASSERT(nDiff > -1)...
 */
void CMergeEditView::ShowDiff(BOOL bScroll, BOOL bSelectText)
{
	CMergeDoc *pd = GetDocument();
	CMergeEditView *pCurrentView = NULL;
	CMergeEditView *pOtherView = NULL;
	const int nDiff = pd->GetCurrentDiff();

	// Try to trap some errors
	if (nDiff >= pd->m_diffList.GetSize())
		_RPTF2(_CRT_ERROR, "Selected diff > diffcount (%d > %d)!",
			nDiff, pd->m_diffList.GetSize());

	if (m_bIsLeft)
	{
		pCurrentView = pd->GetLeftView();
		pOtherView = pd->GetRightView();
	}
	else
	{
		pCurrentView = pd->GetRightView();
		pOtherView = pd->GetLeftView();
	}

	if (nDiff >= 0 && nDiff < pd->m_diffList.GetSize())
	{
		CPoint ptStart, ptEnd;
		DIFFRANGE curDiff;
		pd->m_diffList.GetDiff(nDiff, curDiff);

		ptStart.x = 0;
		ptStart.y = curDiff.dbegin0;
		ptEnd.x = 0;
		ptEnd.y = curDiff.dend0;

		if (bScroll)
		{
			// If diff first line outside current view - context OR
			// if diff last line outside current view - context OR
			// if diff is bigger than screen
			if ((ptStart.y < m_nTopLine + CONTEXT_LINES_ABOVE) ||
				(ptEnd.y >= m_nTopLine + GetScreenLines() - CONTEXT_LINES_BELOW) ||
				(ptEnd.y - ptStart.y) >= GetScreenLines())
			{
				int line = ptStart.y - CONTEXT_LINES_ABOVE;
				if (line < 0)
					line = 0;

				pCurrentView->ScrollToLine(line);
				pOtherView->ScrollToLine(line);
			}
			pCurrentView->SetCursorPos(ptStart);
			pOtherView->SetCursorPos(ptStart);
			pCurrentView->SetAnchor(ptStart);
			pOtherView->SetAnchor(ptStart);
			pCurrentView->SetSelection(ptStart, ptStart);
			pOtherView->SetSelection(ptStart, ptStart);
		}

		if (bSelectText)
		{
			ptEnd.x = GetLineLength(ptEnd.y);
			SetSelection(ptStart, ptEnd);
			UpdateCaret();
		}
		else
			Invalidate();
	}
}


void CMergeEditView::OnTimer(UINT nIDEvent)
{
	// Maybe we want theApp::OnIdle to proceed before processing a timer message
	// ...but for this the queue must be empty
	// The timer message is a low priority message but the queue is maybe not yet empty
	// So we set a flag, wait for OnIdle to proceed, then come back here...
	// We come back here with a IDLE_TIMER OnTimer message (send with SendMessage
	// not with SetTimer so there is no delay)

	// IDT_RESCAN was posted because the app wanted to do a flushAndRescan with some delay

	// IDLE_TIMER is the false timer used to come back here after OnIdle
	// fTimerWaitingForIdle is a bool to store the commands waiting for idle
	// (one normal timer = one flag = one command)

	if (nIDEvent == IDT_RESCAN)
	{
		KillTimer(IDT_RESCAN);
		fTimerWaitingForIdle |= FLAG_RESCAN_WAITS_FOR_IDLE;
		// notify the app to come back after OnIdle
		theApp.SetNeedIdleTimer();
	}

	if (nIDEvent == IDLE_TIMER)
	{
		// not a real timer, just come back after OnIdle
		// look to flags to know what to do
		if (fTimerWaitingForIdle & FLAG_RESCAN_WAITS_FOR_IDLE)
			GetDocument()->RescanIfNeeded(RESCAN_TIMEOUT/1000);
		fTimerWaitingForIdle = 0;
	}

	CCrystalEditViewEx::OnTimer(nIDEvent);
}

/**
 * @brief Returns if buffer is read-only
 * @note This has no any relation to file being read-only!
 */
BOOL CMergeEditView::IsReadOnly(BOOL bLeft)
{
	CCrystalTextBuffer *pBuf = NULL;

	if (bLeft)
		pBuf = &GetDocument()->m_ltBuf;
	else
		pBuf = &GetDocument()->m_rtBuf;

	return pBuf->GetReadOnly();
}

/**
 * @brief Called when "Save left (as...)" item is updated
 */
void CMergeEditView::OnUpdateFileSaveLeft(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();

	if (!IsReadOnly(TRUE) && pd->m_ltBuf.IsModified())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Called when "Save right (as...)" item is updated
 */
void CMergeEditView::OnUpdateFileSaveRight(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();

	if (!IsReadOnly(FALSE) && pd->m_rtBuf.IsModified())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Refresh display using text-buffers
 * @note This DOES NOT reload files!
 */
void CMergeEditView::OnRefresh()
{
	CMergeDoc *pd = GetDocument();
	ASSERT(pd);
	pd->FlushAndRescan(TRUE);
}

/**
 * @brief Enable/Disable automatic rescanning
 */
BOOL CMergeEditView::EnableRescan(BOOL bEnable)
{
	BOOL bOldValue = m_bAutomaticRescan;
	m_bAutomaticRescan = bEnable;
	return bOldValue;
}

/**
 * @brief Handle some keys when in merging mode
 */
BOOL CMergeEditView::MergeModeKeyDown(MSG* pMsg)
{
	BOOL bHandled = FALSE;

	// Allow default text selection when SHIFT pressed
	if (::GetAsyncKeyState(VK_SHIFT))
		return FALSE;

	// Allow default editor functions when CTRL pressed
	if (::GetAsyncKeyState(VK_CONTROL))
		return FALSE;

	// If we are in merging mode (merge with cursor keys)
	// handle some keys here
	switch (pMsg->wParam)
	{
	case VK_LEFT:
		OnR2l();
		bHandled = TRUE;
		break;

	case VK_RIGHT:
		OnL2r();
		bHandled = TRUE;
		break;

	case VK_UP:
		OnPrevdiff();
		bHandled = TRUE;
		break;

	case VK_DOWN:
		OnNextdiff();
		bHandled = TRUE;
		break;
	}

	if (bHandled)
		return TRUE;

	return FALSE;
}

/**
 * @brief Called before messages are translated.
 *
 * Checks if ESC key was pressed, saves and closes doc.
 * Also if in merge mode traps cursor keys.
 */
BOOL CMergeEditView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		BOOL bHandled = FALSE;
		
		// If we are in merging mode (merge with cursor keys)
		// handle some keys here
		if (GetDocument()->GetMergingMode())
		{
			bHandled = MergeModeKeyDown(pMsg);
			if (bHandled)
				return FALSE;
		}

		// Close window if user has allowed it from options
		if (pMsg->wParam == VK_ESCAPE && m_bCloseWithEsc)
		{
			GetParentFrame()->PostMessage(WM_CLOSE, 0, 0);
			return FALSE;
		}
	}
	return CCrystalEditViewEx::PreTranslateMessage(pMsg);
}

/**
 * @brief Called when "Save" item is updated
 */
void CMergeEditView::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();

	if (pd->m_ltBuf.IsModified() || pd->m_rtBuf.IsModified())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Enable/disable left buffer read-only
 */
void CMergeEditView::OnLeftReadOnly()
{
	CMergeDoc *pd = GetDocument();
	BOOL bReadOnly = pd->m_ltBuf.GetReadOnly();
	pd->m_ltBuf.SetReadOnly(!bReadOnly);
}

/**
 * @brief Called when "Left read-only" item is updated
 */
void CMergeEditView::OnUpdateLeftReadOnly(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	BOOL bReadOnly = pd->m_ltBuf.GetReadOnly();
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(bReadOnly);
}

/**
 * @brief Enable/disable right buffer read-only
 */
void CMergeEditView::OnRightReadOnly()
{
	CMergeDoc *pd = GetDocument();
	BOOL bReadOnly = pd->m_rtBuf.GetReadOnly();
	pd->m_rtBuf.SetReadOnly(!bReadOnly);
}

/**
 * @brief Called when "Left read-only" item is updated
 */
void CMergeEditView::OnUpdateRightReadOnly(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	BOOL bReadOnly = pd->m_rtBuf.GetReadOnly();
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(bReadOnly);
}

/// Store interface we use to display status line info
void CMergeEditView::SetStatusInterface(IMergeEditStatus * piMergeEditStatus)
{
	ASSERT(!m_piMergeEditStatus);
	m_piMergeEditStatus = piMergeEditStatus;
}

/**
 * @brief Update statusbar info, Override from CCrystalTextView
 * @note we tab-expand column, but we don't tab-expand char count,
 * since we want to show how many chars there are and tab is just one
 * character although it expands to several spaces.
 */
void CMergeEditView::
OnUpdateCaret()
{
	if (m_piMergeEditStatus && IsTextBufferInitialized())
	{
		CPoint cursorPos = GetCursorPos();
		int nScreenLine = cursorPos.y;
		int nRealLine = ComputeRealLine(nScreenLine);
		CString sLine;
		int chars = -1;
		CString sEol;
		int column = -1;
		int columns = -1;
		int curChar = -1;

		// Is this a ghost line ?
		if (m_pTextBuffer->GetLineFlags(nScreenLine) & LF_GHOST)
		{
			// Ghost lines display eg "Line 12-13"
			sLine.Format(_T("%d-%d"), nRealLine, nRealLine+1);
			sEol = _T("hidden");
		}
		else
		{
			// Regular lines display eg "Line 13 Characters: 25 EOL: CRLF"
			sLine.Format(_T("%d"), nRealLine+1);
			curChar = cursorPos.x + 1;
			chars = GetLineLength(nScreenLine);
			column = CalculateActualOffset(nScreenLine, cursorPos.x) + 1;
			columns = CalculateActualOffset(nScreenLine, chars) + 1;
			chars++;
			if (mf->m_options.GetBool(OPT_ALLOW_MIXED_EOL))
				sEol = GetTextBufferEol(nScreenLine);
			else
				sEol = _T("hidden");
		}
		m_piMergeEditStatus->SetLineInfo(sLine, column, columns,
			curChar, chars, sEol);

		if (m_pLocationView)
		{
			m_pLocationView->UpdateVisiblePos(m_nTopLine,
				m_nTopLine + GetScreenLines());
		}
	}
}

/// Highlight difference in current line
void CMergeEditView::OnShowlineworddiff()
{
	// Pass this to the document, to compare this file to other
	GetDocument()->Showlinediff(this, CMergeDoc::WORDDIFF);
}

/// Highlight difference in current line
void CMergeEditView::OnShowlinechardiff()
{
	// Pass this to the document, to compare this file to other
	GetDocument()->Showlinediff(this, CMergeDoc::BYTEDIFF);
}

/// Enable highlight menuitem if current line is flagged as having a difference
void CMergeEditView::OnUpdateShowlineworddiff(CCmdUI* pCmdUI)
{
	int line = GetCursorPos().y;
	BOOL enable = GetLineFlags(line) & LF_DIFF;
	pCmdUI->Enable(enable);
}

/// Enable highlight menuitem if current line is flagged as having a difference
void CMergeEditView::OnUpdateShowlinechardiff(CCmdUI* pCmdUI)
{
	OnUpdateShowlineworddiff(pCmdUI);
}

/**
 * @brief Enable/disable Replace-menuitem
 */
void CMergeEditView::OnUpdateEditReplace(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	BOOL bReadOnly = FALSE;
	if (m_bIsLeft)
		bReadOnly = pd->m_ltBuf.GetReadOnly();
	else
		bReadOnly = pd->m_rtBuf.GetReadOnly();

	pCmdUI->Enable(!bReadOnly);
}

/**
 * @brief Update left readonly statusbaritem
 */
void CMergeEditView::OnUpdateStatusLeftRO(CCmdUI* pCmdUI)
{
	BOOL bROLeft = GetDocument()->m_ltBuf.GetReadOnly();
	pCmdUI->Enable(bROLeft);
}

/**
 * @brief Update right readonly statusbaritem
 */
void CMergeEditView::OnUpdateStatusRightRO(CCmdUI* pCmdUI)
{
	BOOL bRORight = GetDocument()->m_rtBuf.GetReadOnly();
	pCmdUI->Enable(bRORight);
}

/**
 * @brief Call ::AppendMenu, and if it fails get error string into local variable
 *
 * This only provides functionality for debugging.
 */
static BOOL DoAppendMenu(HMENU hMenu, UINT uFlags, UINT_PTR uIDNewItem, LPCTSTR lpNewItem)
{
	BOOL ok = ::AppendMenu(hMenu, uFlags, uIDNewItem, lpNewItem);
	if (!ok)
	{
		int nerr = GetLastError();
		CString syserr = GetSysError(nerr);
	}
	return ok;
}

/**
 * @brief Create the dynamic submenu for scripts
 */
HMENU CMergeEditView::createScriptsSubmenu(HMENU hMenu)
{
	// get scripts list
	CStringArray functionNamesList;
	GetFreeFunctionsInScripts(functionNamesList, L"EDITOR_SCRIPT");

	// empty the menu
	int i = GetMenuItemCount(hMenu);
	while (i --)
		DeleteMenu(hMenu, 0, MF_BYPOSITION);

	if (functionNamesList.GetSize() == 0)
	{
		// no script : create a <empty> entry
		DoAppendMenu(hMenu, MF_STRING, ID_NO_EDIT_SCRIPTS, LoadResString(ID_NO_EDIT_SCRIPTS));
	}
	else
	{
		// or fill in the submenu with the scripts names
		int ID = ID_SCRIPT_FIRST;	// first ID in menu
		for (i = 0 ; i < functionNamesList.GetSize() ; i++, ID++)
			DoAppendMenu(hMenu, MF_STRING, ID, functionNamesList[i]);

		functionNamesList.RemoveAll();
	}

	if (IsWindowsScriptThere() == FALSE)
		DoAppendMenu(hMenu, MF_STRING, ID_NO_SCT_SCRIPTS, LoadResString(ID_NO_SCT_SCRIPTS));

	return hMenu;
}

/**
 * @brief Create the dynamic submenu for prediffers
 *
 * @note The plugins are grouped in (suggested) and (not suggested)
 *       The IDs follow the order of GetAvailableScripts
 *       For example :
 *				suggested 0         ID_1ST + 0 
 *				suggested 1         ID_1ST + 2 
 *				suggested 2         ID_1ST + 5 
 *				not suggested 0     ID_1ST + 1 
 *				not suggested 1     ID_1ST + 3 
 *				not suggested 2     ID_1ST + 4 
 */
HMENU CMergeEditView::createPrediffersSubmenu(HMENU hMenu)
{
	// empty the menu
	int i = GetMenuItemCount(hMenu);
	while (i --)
		DeleteMenu(hMenu, 0, MF_BYPOSITION);

	CMergeDoc *pd = GetDocument();
	ASSERT(pd);

	// title
	DoAppendMenu(hMenu, MF_STRING, ID_NO_PREDIFFER, LoadResString(ID_NO_PREDIFFER));

	// get the scriptlet files
	PluginArray * piScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"FILE_PREDIFF");
	PluginArray * piScriptArray2 = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"BUFFER_PREDIFF");

	// build the menu : first part, suggested plugins
	// title
	DoAppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	DoAppendMenu(hMenu, MF_STRING, ID_SUGGESTED_PLUGINS, LoadResString(ID_SUGGESTED_PLUGINS));

	int ID = ID_PREDIFFERS_FIRST;	// first ID in menu
	int iScript;
	for (iScript = 0 ; iScript < piScriptArray->GetSize() ; iScript++, ID ++)
	{
		PluginInfo & plugin = piScriptArray->ElementAt(iScript);
		if (plugin.TestAgainstRegList(pd->m_strBothFilenames) == FALSE)
			continue;

		DoAppendMenu(hMenu, MF_STRING, ID, plugin.name);
	}
	for (iScript = 0 ; iScript < piScriptArray2->GetSize() ; iScript++, ID ++)
	{
		PluginInfo & plugin = piScriptArray2->ElementAt(iScript);
		if (plugin.TestAgainstRegList(pd->m_strBothFilenames) == FALSE)
			continue;

		DoAppendMenu(hMenu, MF_STRING, ID, plugin.name);
	}

	// build the menu : second part, others plugins
	// title
	DoAppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	DoAppendMenu(hMenu, MF_STRING, ID_NOT_SUGGESTED_PLUGINS, LoadResString(ID_NOT_SUGGESTED_PLUGINS));

	ID = ID_PREDIFFERS_FIRST;	// first ID in menu
	for (iScript = 0 ; iScript < piScriptArray->GetSize() ; iScript++, ID ++)
	{
		PluginInfo & plugin = piScriptArray->ElementAt(iScript);
		if (plugin.TestAgainstRegList(pd->m_strBothFilenames) == TRUE)
			continue;

		DoAppendMenu(hMenu, MF_STRING, ID, plugin.name);
	}
	for (iScript = 0 ; iScript < piScriptArray2->GetSize() ; iScript++, ID ++)
	{
		PluginInfo & plugin = piScriptArray2->ElementAt(iScript);
		if (plugin.TestAgainstRegList(pd->m_strBothFilenames) == TRUE)
			continue;

		DoAppendMenu(hMenu, MF_STRING, ID, plugin.name);
	}

	// compute the m_CurrentPredifferID (to set the radio button)
	PrediffingInfo prediffer;
	pd->GetPrediffer(&prediffer);

	if (prediffer.bToBeScanned)
		m_CurrentPredifferID = 0;
	else if (prediffer.pluginName.IsEmpty())
		m_CurrentPredifferID = ID_NO_PREDIFFER;
	else
	{
		ID = ID_PREDIFFERS_FIRST;	// first ID in menu
		for (iScript = 0 ; iScript < piScriptArray->GetSize() ; iScript++, ID ++)
		{
			PluginInfo & plugin = piScriptArray->ElementAt(iScript);
			if (prediffer.pluginName == plugin.name)
				m_CurrentPredifferID = ID;

		}
		for (iScript = 0 ; iScript < piScriptArray2->GetSize() ; iScript++, ID ++)
		{
			PluginInfo & plugin = piScriptArray2->ElementAt(iScript);
			if (prediffer.pluginName == plugin.name)
				m_CurrentPredifferID = ID;
		}
	}

	return hMenu;
}

/**
 * @brief Offer a context menu built with scriptlet/ActiveX functions
 */
void CMergeEditView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// Create the menu and populate it with the available functions
	BCMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_MERGEVIEW));
	VERIFY(menu.LoadToolbar(IDR_MAINFRAME));

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
 * @brief Update left EOL mode in status bar
 */
void CMergeEditView::OnUpdateStatusLeftEOL(CCmdUI* pCmdUI)
{
	if (mf->m_options.GetBool(OPT_ALLOW_MIXED_EOL))
		pCmdUI->SetText(_T(""));
	else
		GetDocument()->GetLeftView()->OnUpdateIndicatorCRLF(pCmdUI);
}

/**
 * @brief Update right EOL mode in status bar
 */
void CMergeEditView::OnUpdateStatusRightEOL(CCmdUI* pCmdUI)
{
	if (mf->m_options.GetBool(OPT_ALLOW_MIXED_EOL))
		pCmdUI->SetText(_T(""));
	else
		GetDocument()->GetRightView()->OnUpdateIndicatorCRLF(pCmdUI);
}

/**
 * @brief Change EOL mode and unify all the lines EOL to this new mode
 */
void CMergeEditView::OnConvertEolTo(UINT nID )
{
	int nStyle = CRLF_STYLE_AUTOMATIC;;
	switch (nID)
	{
		case ID_EOL_TO_DOS:
			nStyle = CRLF_STYLE_DOS;
			break;
		case ID_EOL_TO_UNIX:
			nStyle = CRLF_STYLE_UNIX;
			break;
		case ID_EOL_TO_MAC:
			nStyle = CRLF_STYLE_MAC;
			break;
		default:
			// Catch errors
			_RPTF0(_CRT_ERROR, "Unhandled EOL type conversion!");
			break;
	}
	m_pTextBuffer->SetCRLFMode(nStyle);

	// we don't need a derived applyEOLMode for ghost lines as they have no EOL char
	if (m_pTextBuffer->applyEOLMode())
	{
		CMergeDoc *pd = GetDocument();
		ASSERT(pd);
		pd->UpdateHeaderPath(m_bIsLeft);
		pd->FlushAndRescan(TRUE);
	}
}

/**
 * @brief allow convert to entries in file submenu
 */
void CMergeEditView::OnUpdateConvertEolTo(CCmdUI* pCmdUI)
{
	int nStyle = CRLF_STYLE_AUTOMATIC;
	switch (pCmdUI->m_nID)
	{
		case ID_EOL_TO_DOS:
			nStyle = CRLF_STYLE_DOS;
			break;
		case ID_EOL_TO_UNIX:
			nStyle = CRLF_STYLE_UNIX;
			break;
		case ID_EOL_TO_MAC:
			nStyle = CRLF_STYLE_MAC;
			break;
		default:
			// Catch errors
			_RPTF0(_CRT_ERROR, "Missing menuitem handler for EOL convert menu!");
			break;
	}

	if (mf->m_options.GetBool(OPT_ALLOW_MIXED_EOL) ||
		nStyle != m_pTextBuffer->GetCRLFMode())
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Copy diff from left to right and advance to next diff
 */
void CMergeEditView::OnL2RNext()
{
	// Check that diff is selected
	if (GetDocument()->GetCurrentDiff() == -1)
		return;

	OnL2r();
	OnNextdiff();
}

/**
 * @brief Update "Copy right and advance" UI item
 */
void CMergeEditView::OnUpdateL2RNext(CCmdUI* pCmdUI)
{
	// Check that right side is not readonly
	if (!IsReadOnly(FALSE))
		pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Copy diff from right to left and advance to next diff
 */
void CMergeEditView::OnR2LNext()
{
	// Check that diff is selected
	if (GetDocument()->GetCurrentDiff() == -1)
		return;

	OnR2l();
	OnNextdiff();
}

/**
 * @brief Update "Copy left and advance" UI item
 */
void CMergeEditView::OnUpdateR2LNext(CCmdUI* pCmdUI)
{
	// Check that left side is not readonly
	if (!IsReadOnly(TRUE))
		pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Change active pane in MergeView
 */
void CMergeEditView::OnChangePane()
{
	CSplitterWnd *pSplitterWnd = GetParentSplitter(this, FALSE);
	pSplitterWnd->ActivateNext();
}

/**
 * @brief Enable "Change Pane" menuitem when mergeview is active
 */
void CMergeEditView::OnUpdateChangePane(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

/**
 * @brief Show "Go To" dialog and scroll views to line or diff.
 *
 * Before dialog is opened, current line and file is determined
 * and selected.
 * @note Conversions needed between apparent and real lines
 */
void CMergeEditView::OnWMGoto()
{
	CWMGotoDlg dlg;
	CMergeDoc *pDoc = GetDocument();
	CPoint pos = GetCursorPos();
	int nRealLine = 0;
	int nLastLine = 0;

	if (m_bIsLeft)
	{
		nRealLine = pDoc->m_ltBuf.ComputeRealLine(pos.y);
		int nLineCount = pDoc->m_ltBuf.GetLineCount();
		nLastLine = pDoc->m_ltBuf.ComputeRealLine(nLineCount - 1);
	}
	else
	{
		nRealLine = pDoc->m_rtBuf.ComputeRealLine(pos.y);
		int nLineCount = pDoc->m_rtBuf.GetLineCount();
		nLastLine = pDoc->m_rtBuf.ComputeRealLine(nLineCount - 1);
	}

	// Set active file and current line selected in dialog
	dlg.m_strParam.Format(_T("%d"), nRealLine + 1);
	dlg.m_nFile = m_bIsLeft ? 0 : 1;
	dlg.m_nGotoWhat = 0;

	if (dlg.DoModal() == IDOK)
	{
		CMergeDoc * pDoc = GetDocument();
		CMergeEditView * pCurrentView = NULL;
		CMergeEditView * pOtherView = NULL;

		// Get views
		if (dlg.m_nFile == 0)
		{
			pCurrentView = pDoc->GetLeftView();
			pOtherView = pDoc->GetRightView();
		}
		else
		{
			pOtherView = pDoc->GetLeftView();
			pCurrentView = pDoc->GetRightView();
		}

		if (dlg.m_nGotoWhat == 0)
		{
			int nRealLine = _ttoi(dlg.m_strParam) - 1;
			if (nRealLine < 0)
				nRealLine = 0;
			if (nRealLine > nLastLine)
				nRealLine = nLastLine;

			GotoLine(nRealLine, TRUE, dlg.m_nFile == 0);
		}
		else
		{
			int diff = _ttoi(dlg.m_strParam) - 1;
			if (diff < 0)
				diff = 0;
			if (diff >= pDoc->m_diffList.GetSize())
				diff = pDoc->m_diffList.GetSize();

			pCurrentView->SelectDiff(diff, TRUE, FALSE);
		}
	}
}

/**
 * @brief Enable "Go To" menuitem when mergeview is active
 */
void CMergeEditView::OnUpdateWMGoto(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

/**
 * @brief Reload cached options.
 */
void CMergeEditView::RefreshOptions()
{ 
	m_bCloseWithEsc = mf->m_options.GetBool(OPT_CLOSE_WITH_ESC);
	m_bAutomaticRescan = mf->m_options.GetBool(OPT_AUTOMATIC_RESCAN);

	if (mf->m_options.GetInt(OPT_TAB_TYPE) == 0)
		SetInsertTabs(TRUE);
	else
		SetInsertTabs(FALSE);

	m_bSyntaxHighlight = mf->m_options.GetBool(OPT_SYNTAX_HIGHLIGHT);
	m_bWordDiffHighlight = mf->m_options.GetBool(OPT_WORDDIFF_HIGHLIGHT);
	m_cachedColors.clrDiff = mf->m_options.GetInt(OPT_CLR_DIFF);
	m_cachedColors.clrSelDiff = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF);
	m_cachedColors.clrDiffDeleted = mf->m_options.GetInt(OPT_CLR_DIFF_DELETED);
	m_cachedColors.clrSelDiffDeleted = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF_DELETED);
	m_cachedColors.clrDiffText = mf->m_options.GetInt(OPT_CLR_DIFF_TEXT);
	m_cachedColors.clrSelDiffText = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF_TEXT);
	m_cachedColors.clrTrivial = mf->m_options.GetInt(OPT_CLR_TRIVIAL_DIFF);
	m_cachedColors.clrTrivialDeleted = mf->m_options.GetInt(OPT_CLR_TRIVIAL_DIFF_DELETED);
	m_cachedColors.clrTrivialText = mf->m_options.GetInt(OPT_CLR_TRIVIAL_DIFF_TEXT);
	m_cachedColors.clrMoved = mf->m_options.GetInt(OPT_CLR_MOVEDBLOCK);
	m_cachedColors.clrMovedDeleted = mf->m_options.GetInt(OPT_CLR_MOVEDBLOCK_DELETED);
	m_cachedColors.clrMovedText = mf->m_options.GetInt(OPT_CLR_MOVEDBLOCK_TEXT);
	m_cachedColors.clrSelMoved = mf->m_options.GetInt(OPT_CLR_SELECTED_MOVEDBLOCK);
	m_cachedColors.clrSelMovedDeleted = mf->m_options.GetInt(OPT_CLR_SELECTED_MOVEDBLOCK_DELETED);
	m_cachedColors.clrSelMovedText = mf->m_options.GetInt(OPT_CLR_SELECTED_MOVEDBLOCK_TEXT);
	m_cachedColors.clrWordDiff = mf->m_options.GetInt(OPT_CLR_WORDDIFF);
	m_cachedColors.clrSelWordDiff = mf->m_options.GetInt(OPT_CLR_SELECTED_WORDDIFF);
	m_cachedColors.clrWordDiffText = mf->m_options.GetInt(OPT_CLR_WORDDIFF_TEXT);
	m_cachedColors.clrSelWordDiffText = mf->m_options.GetInt(OPT_CLR_SELECTED_WORDDIFF_TEXT);
}

/**
 * @brief Called when an editor script item is updated
 */
void CMergeEditView::OnUpdateScripts(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMergeEditView::OnScripts(UINT nID )
{
	// text is CHAR if compiled without UNICODE, WCHAR with UNICODE
	CString text = GetSelectedText();

	// transform the text with a script/ActiveX function, event=EDITOR_SCRIPT
	BOOL bChanged = TextTransform_Interactive(text, L"EDITOR_SCRIPT", nID - ID_SCRIPT_FIRST);
	if (bChanged)
		// now replace the text
		ReplaceSelection(text, 0);
}

/**
 * @brief Called when an editor script item is updated
 */
void CMergeEditView::OnUpdatePrediffer(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);

	CMergeDoc *pd = GetDocument();
	ASSERT(pd);
	PrediffingInfo prediffer;
	pd->GetPrediffer(&prediffer);

	if (prediffer.bToBeScanned)
	{
		pCmdUI->SetRadio(FALSE);
		return;
	}

	// Detect when CDiffWrapper::RunFileDiff has canceled a buggy prediffer
	if (prediffer.pluginName.IsEmpty())
		m_CurrentPredifferID = ID_NO_PREDIFFER;

	pCmdUI->SetRadio(pCmdUI->m_nID == m_CurrentPredifferID);
}

void CMergeEditView::OnNoPrediffer()
{
	OnPrediffer(ID_NO_PREDIFFER);
}
void CMergeEditView::OnPrediffer(UINT nID )
{
	CMergeDoc *pd = GetDocument();
	ASSERT(pd);

	if (nID == ID_NO_PREDIFFER)
	{
		m_CurrentPredifferID = nID;
		pd->SetPrediffer(NULL);
		pd->FlushAndRescan(TRUE);
		return;
	}

	// get the scriptlet files
	PluginArray * piScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"FILE_PREDIFF");
	PluginArray * piScriptArray2 = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"BUFFER_PREDIFF");

	// build a PrediffingInfo structure fom the ID
	PrediffingInfo prediffer;
	prediffer.bToBeScanned = FALSE;

	int pluginNumber = nID - ID_PREDIFFERS_FIRST;
	if (pluginNumber < piScriptArray->GetSize())
	{
		prediffer.bWithFile = TRUE;
		PluginInfo & plugin = piScriptArray->ElementAt(pluginNumber);
		prediffer.pluginName = plugin.name;
	}
	else
	{
		pluginNumber -= piScriptArray->GetSize();
		if (pluginNumber >= piScriptArray2->GetSize())
			return;
		prediffer.bWithFile = FALSE;
		PluginInfo & plugin = piScriptArray2->ElementAt(pluginNumber);
		prediffer.pluginName = plugin.name;
	}

	// update data for the radio button
	m_CurrentPredifferID = nID;

	// update the prediffer and rescan
	pd->SetPrediffer(&prediffer);
	pd->FlushAndRescan(TRUE);
}

/**
 * @brief Switch Merging/Editing mode and update
 * buffer read-only states accordingly
 */
void CMergeEditView::OnMergingMode()
{
	CMergeDoc *pDoc = GetDocument();
	BOOL bMergingMode = pDoc->GetMergingMode();

	pDoc->SetMergingMode(!bMergingMode);
}

/**
 * @brief Update Menuitem for Merging Mode
 */
void CMergeEditView::OnUpdateMergingMode(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(GetDocument()->GetMergingMode());
}

/**
 * @brief Update MergingMode UI in statusbar
 */
void CMergeEditView::OnUpdateMergingStatus(CCmdUI *pCmdUI)
{
	CString text;
	VERIFY(text.LoadString(IDS_MERGEMODE_MERGING));
	pCmdUI->SetText(text);
	pCmdUI->Enable(GetDocument()->GetMergingMode());
}

/** 
 * @brief Goto given line.
 * @param [in] nLine Destination linenumber
 * @param [in] bRealLine if TRUE linenumber is real line, otherwise
 * it is apparent line (including deleted lines)
 * @param [in] bLeft If TRUE linenumber is for left pane
 */
void CMergeEditView::GotoLine(UINT nLine, BOOL bRealLine, BOOL bLeft)
{
 	CMergeDoc *pDoc = GetDocument();
	CMergeEditView *pLeftView = pDoc->GetLeftView();
	CMergeEditView *pRightView = pDoc->GetRightView();
	int nRealLine = nLine;
	int nApparentLine = nLine;
	int nLineCount = 0;

	// Compute apparent (shown linenumber) line
	if (bRealLine)
	{
		if (bLeft)
		{
			if (nRealLine > pDoc->m_ltBuf.GetLineCount() - 1)
				nRealLine = pDoc->m_ltBuf.GetLineCount() - 1;

			nApparentLine = pDoc->m_ltBuf.ComputeApparentLine(nRealLine);
		}
		else
		{
			if (nRealLine > pDoc->m_rtBuf.GetLineCount() - 1)
				nRealLine = pDoc->m_rtBuf.GetLineCount() - 1;

			nApparentLine = pDoc->m_rtBuf.ComputeApparentLine(nRealLine);
		}
	}
	CPoint ptPos;
	ptPos.x = 0;
	ptPos.y = nApparentLine;

	// Scroll line to center of view
	int nScrollLine = nApparentLine;
	nScrollLine -= GetScreenLines() / 2;
	if (nScrollLine < 0)
		nScrollLine = 0;
	
	pLeftView->ScrollToLine(nScrollLine);
	pRightView->ScrollToLine(nScrollLine);
	pLeftView->SetCursorPos(ptPos);
	pRightView->SetCursorPos(ptPos);
	pLeftView->SetAnchor(ptPos);
	pRightView->SetAnchor(ptPos);
}

/**
 * @brief Called when user selects Window/Close, allows user to save files.
 */
void CMergeEditView::OnWindowClose()
{
 	CMergeDoc *pDoc = GetDocument();

	// Allow user to cancel closing
	if (!pDoc->SaveHelper(TRUE))
		return;
	else
	{
		// Set modified to false so we don't ask again about saving
		pDoc->m_ltBuf.SetModified(FALSE);
		pDoc->m_rtBuf.SetModified(FALSE);
		GetParentFrame()->PostMessage(WM_CLOSE, 0, 0);
	}
	m_pLocationView = NULL;
}

/**
 * @brief When view is scrolled using scrollbars update location pane.
 */
void CMergeEditView::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	CCrystalTextView::OnVScroll (nSBCode, nPos, pScrollBar);
 
	// Note we cannot use nPos because of its 16-bit nature
	SCROLLINFO si = {0};
	si.cbSize = sizeof (si);
	si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
	VERIFY (GetScrollInfo (SB_VERT, &si));

	// Get the minimum and maximum scroll-bar	positions.
	int nMinPos =	si.nMin;
	int nMaxPos =	si.nMax;

	// Get the current position of scroll	box.
	int nCurPos =	si.nPos;

	BOOL bDisableSmooth =	TRUE;
	switch (nSBCode)
	{
	case SB_TOP:			// Scroll to top.
		nCurPos = nMinPos;
		bDisableSmooth = FALSE;
		break;

	case SB_BOTTOM:			// Scroll to bottom.
		nCurPos =	nMaxPos;
		bDisableSmooth = FALSE;
		break;

	case SB_LINEUP:			// Scroll one line up.
		if (nCurPos >	nMinPos)
			nCurPos--;
		break;

	case SB_LINEDOWN:		// Scroll one line down.
		if (nCurPos <	nMaxPos)
			nCurPos++;
		break;

	case SB_PAGEUP:			// Scroll one page up.
		nCurPos =	max(nMinPos, nCurPos - (int) si.nPage +	1);
		bDisableSmooth = FALSE;
		break;

	case SB_PAGEDOWN:		// Scroll one page down.
		nCurPos =	min(nMaxPos, nCurPos + (int) si.nPage -	1);
		bDisableSmooth = FALSE;
		break;

	case SB_THUMBPOSITION:		// Scroll to absolute position.	nPos is	the	position
		nCurPos =	si.nTrackPos;	// of the scroll box at	the	end	of the drag	operation.
		break;

	case SB_THUMBTRACK:			// Drag	scroll box to specified	position. nPos is the
		nCurPos =	si.nTrackPos;	// position	that the scroll	box	has	been dragged to.
		break;
	}

	m_pLocationView->UpdateVisiblePos(nCurPos);
}

/**
 * @brief Copy selected lines adding linenumbers.
 */
void CMergeEditView::OnEditCopyLineNumbers()
{
	CPoint ptStart;
	CPoint ptEnd;
	CString strText;
	CString strLine;
	CString strNum;
	CString strNumLine;
	UINT line = 0;
	int nNumWidth = 0;

	CMergeDoc *pDoc = GetDocument();
	GetSelection(ptStart, ptEnd);

	// Get last selected line (having widest linenumber)
	line = pDoc->m_rtBuf.ComputeRealLine(ptEnd.y);
	strNum.Format(_T("%d"), line + 1);
	nNumWidth = strNum.GetLength();
	
	for (int i = ptStart.y; i <= ptEnd.y; i++)
	{
		// We need to convert to real linenumbers
		if (m_bIsLeft)
			line = pDoc->m_ltBuf.ComputeRealLine(i);
		else
			line = pDoc->m_rtBuf.ComputeRealLine(i);

		// Insert spaces to align different width linenumbers (99, 100)
		strLine = GetLineText(i);
		strNum.Format(_T("%d"), line + 1);
		CString sSpaces(' ', nNumWidth - strNum.GetLength());
		
		strText += sSpaces;
		strNumLine.Format(_T("%d: %s"), line + 1, strLine);
		strText += strNumLine;
 	}
	PutToClipboard(strText);
}

void CMergeEditView::OnUpdateEditCopyLinenumbers(CCmdUI* pCmdUI)
{
	CCrystalEditViewEx::OnUpdateEditCopy(pCmdUI);
}

/**
 * @brief Open active file with associated application.
 *
 * First tries to open file using shell 'Edit' action, since that
 * action open scripts etc. to editor instead of running them. If
 * edit-action is not registered, 'Open' action is used.
 */
void CMergeEditView::OnOpenFile()
{
	CMergeDoc * pDoc = GetDocument();
	ASSERT(pDoc != NULL);

	CString sFileName = m_bIsLeft ? pDoc->m_filePaths.GetLeft() : pDoc->m_filePaths.GetRight();
	if (sFileName.IsEmpty())
		return;
	int rtn = (int)ShellExecute(::GetDesktopWindow(), _T("edit"), sFileName,
			0, 0, SW_SHOWNORMAL);
	if (rtn==SE_ERR_NOASSOC)
		rtn = (int)ShellExecute(::GetDesktopWindow(), _T("open"), sFileName,
			 0, 0, SW_SHOWNORMAL);
	if (rtn==SE_ERR_NOASSOC)
		OnOpenFileWith();
}

/**
 * @brief Open active file with app selection dialog
 */
void CMergeEditView::OnOpenFileWith()
{
	CMergeDoc * pDoc = GetDocument();
	ASSERT(pDoc != NULL);

	CString sFileName = m_bIsLeft ? pDoc->m_filePaths.GetLeft() : pDoc->m_filePaths.GetRight();
	if (sFileName.IsEmpty())
		return;

	CString sysdir;
	if (!GetSystemDirectory(sysdir.GetBuffer(MAX_PATH), MAX_PATH))
		return;
	sysdir.ReleaseBuffer();
	CString arg = (CString)_T("shell32.dll,OpenAs_RunDLL ") + sFileName;
	ShellExecute(::GetDesktopWindow(), 0, _T("RUNDLL32.EXE"), arg,
			sysdir, SW_SHOWNORMAL);
}

/**
 * @brief Open active file with external editor
 */
void CMergeEditView::OnOpenFileWithEditor()
{
	CMergeDoc * pDoc = GetDocument();
	ASSERT(pDoc != NULL);

	CString sFileName = m_bIsLeft ? pDoc->m_filePaths.GetLeft() : pDoc->m_filePaths.GetRight();
	if (sFileName.IsEmpty())
		return;

	mf->OpenFileToExternalEditor(sFileName);
}

/**
 * @brief Force repaint of location pane
 */
void CMergeEditView::RepaintLocationPane()
{
	if (m_pLocationView)
		m_pLocationView->Invalidate();
}

/**
 * @brief Enables/disables linediff (different color for diffs)
 */
void CMergeEditView::OnViewLineDiffs()
{
	mf->m_options.SaveOption(OPT_WORDDIFF_HIGHLIGHT, !m_bWordDiffHighlight);

	// Call CMergeDoc RefreshOptions() to refresh *both* views
	CMergeDoc *pDoc = GetDocument();
	pDoc->RefreshOptions();
	pDoc->FlushAndRescan(TRUE);
}

void CMergeEditView::OnUpdateViewLineDiffs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bWordDiffHighlight);
}
