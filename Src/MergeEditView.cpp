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
#include "merge.h"
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
	m_bIsLeft = FALSE;
	m_nModifications = 0;
	m_piMergeEditStatus = 0;
	SetParser(&m_xParser);
	m_bAutomaticRescan = FALSE;
	fTimerWaitingForIdle = 0;

	m_bSyntaxHighlight = mf->m_options.GetInt(OPT_SYNTAX_HIGHLIGHT);
	m_cachedColors.clrDiff = mf->m_options.GetInt(OPT_CLR_DIFF);
	m_cachedColors.clrSelDiff = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF);
	m_cachedColors.clrDiffDeleted = mf->m_options.GetInt(OPT_CLR_DIFF_DELETED);
	m_cachedColors.clrSelDiffDeleted = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF_DELETED);
	m_cachedColors.clrDiffText = mf->m_options.GetInt(OPT_CLR_DIFF_TEXT);
	m_cachedColors.clrSelDiffText = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF_TEXT);
	m_cachedColors.clrTrivial = mf->m_options.GetInt(OPT_CLR_TRIVIAL_DIFF);
	m_cachedColors.clrTrivialDeleted = mf->m_options.GetInt(OPT_CLR_TRIVIAL_DIFF_DELETED);

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
	ON_COMMAND(ID_SHOWLINEDIFF, OnShowlinediff)
	ON_UPDATE_COMMAND_UI(ID_SHOWLINEDIFF, OnUpdateShowlinediff)
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
	ON_COMMAND(ID_MULTIPLE_LEFT, OnMultipleLeft)
	ON_UPDATE_COMMAND_UI(ID_MULTIPLE_LEFT, OnUpdateMultipleLeft)
	ON_COMMAND(ID_MULTIPLE_RIGHT, OnMultipleRight)
	ON_UPDATE_COMMAND_UI(ID_MULTIPLE_RIGHT, OnUpdateMultipleRight)
	ON_COMMAND(ID_WINDOW_CHANGE_PANE, OnChangePane)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CHANGE_PANE, OnUpdateChangePane)
	ON_COMMAND(ID_EDIT_WMGOTO, OnWMGoto)
	ON_UPDATE_COMMAND_UI(ID_EDIT_WMGOTO, OnUpdateWMGoto)
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

/// Update any language-dependent data
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
	if (pd->m_nDiffs == 0)
		return;

	int firstLine, lastLine;
	GetFullySelectedLines(firstLine, lastLine);
	if (lastLine < firstLine)
		return;

	for (UINT i = 0; i < pd->m_nDiffs; i++)
	{
		if ((int)pd->m_diffs[i].dbegin0 >= firstLine)
		{
			firstDiff = i;
			break;
		}
	}
	if (i == pd->m_nDiffs)
		return;

	lastDiff = pd->m_nDiffs - 1;
	for (i = firstDiff; i < pd->m_nDiffs; i++)
	{
		if ((int)pd->m_diffs[i].dend0 > lastLine)
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
	DWORD dwLineFlags = GetLineFlags(nLineIndex);

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
				crBkgnd = m_cachedColors.clrSelDiff;
				crText = m_cachedColors.clrSelDiffText;
			}
			else
			{
				crBkgnd = m_cachedColors.clrDiff;
				crText = m_cachedColors.clrDiffText;
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
	if (nDiff >= (int)pd->m_nDiffs)
		_RPTF2(_CRT_ERROR, "Selected diff > diffcount (%d >= %d)",
			nDiff, (int)pd->m_nDiffs);

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

	// If no diffs, nothing to select (m_nDiffs is unsigned!)
	if (pd->m_nDiffs == 0)
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
		nDiff = pd->LineToDiff(pos.y);
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
		if (pd->LineToDiff(pos.y) == -1)
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
	if (pd->m_nDiffs > 0)
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
	if (pd->m_nDiffs>0)
		SelectDiff(pd->m_nDiffs-1, TRUE, FALSE);
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
		if (curDiff == (signed int)pd->m_nDiffs - 1)
			// We're on a last diff, so select that
			SelectDiff(curDiff, TRUE, FALSE);
		else
			// We're on a diff, so select the next one
		SelectDiff(curDiff + 1, TRUE, FALSE);
	}
	else
	{
		// we're not on a diff, so figure out which one to select
		int line = GetCursorPos().y;
		if (!IsValidTextPosY(CPoint(0, line)))
			line = m_nTopLine;
		for (UINT i = 0; i < pd->m_nDiffs; i++)
		{
			if ((int)pd->m_diffs[i].dbegin0 >= line)
			{
				curDiff = i;
				break;
			}
		}
		SelectDiff(curDiff, TRUE, FALSE);
	}
}

/**
 * @brief Update "Next diff" UI items
 */
void CMergeEditView::OnUpdateNextdiff(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	CPoint pos = GetCursorPos();
	pCmdUI->Enable(pd->m_nDiffs>0 && pos.y < (long)pd->m_diffs[pd->m_nDiffs-1].dbegin0);
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
		if (curDiff == 0)
			// We're on a first diff, select it
			SelectDiff(curDiff, TRUE, FALSE);
		else
			// We're on a diff, so select the previous one
			SelectDiff(curDiff - 1, TRUE, FALSE);
	}
	else
	{
		// we're not on a diff, so figure out which one to select
		int line = GetCursorPos().y;
		if (!IsValidTextPosY(CPoint(0, line)))
			line = m_nTopLine;
		for (int i = pd->m_nDiffs - 1; i >= 0 ; i--)
		{
			if ((int)pd->m_diffs[i].dend0 <= line)
			{
				curDiff = i;
				break;
			}
		}
		SelectDiff(curDiff, TRUE, FALSE);
	}
}

/**
 * @brief Update "Previous diff" UI items
 */
void CMergeEditView::OnUpdatePrevdiff(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	CPoint pos = GetCursorPos();
	pCmdUI->Enable(pd->m_nDiffs>0 && pos.y > (long)pd->m_diffs[0].dend0);
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
	return pd->LineInDiff(nLine, curDiff);
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

	int diff = pd->LineToDiff(pos.y);
	if (diff != -1)
		SelectDiff(diff, FALSE, FALSE);

	CCrystalEditViewEx::OnLButtonDblClk(nFlags, point);
}

/**
 * @brief Called when mouse left button is released
 */
void CMergeEditView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CMergeDoc *pd = GetDocument();
	CCrystalEditViewEx::OnLButtonUp(nFlags, point);

	CPoint pos = GetCursorPos();
	if (!IsLineInCurrentDiff(pos.y))
	{
		pd->SetCurrentDiff(-1);
		Invalidate();
		pd->UpdateAllViews(this);
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
 */
void CMergeEditView::OnL2r()
{
	// Check that right side is not readonly
	if (IsReadOnly(FALSE))
		return;
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYL2R));
	GetDocument()->ListCopy(true);
}

/**
 * @brief Called when "Copy to left" item is updated
 */
void CMergeEditView::OnUpdateL2r(CCmdUI* pCmdUI)
{
	// Check that right side is not readonly
	if (!IsReadOnly(FALSE))
		pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Copy diff from right pane to left pane
 */
void CMergeEditView::OnR2l()
{
	// Check that left side is not readonly
	if (IsReadOnly(TRUE))
		return;
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYR2L));
	GetDocument()->ListCopy(false);
}

/**
 * @brief Called when "Copy to right" item is updated
 */
void CMergeEditView::OnUpdateR2l(CCmdUI* pCmdUI)
{
	// Check that left side is not readonly
	if (!IsReadOnly(TRUE))
		pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
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

	GetDocument()->CopyAllList(false);
}

/**
 * @brief Called when "Copy all to left" item is updated
 */
void CMergeEditView::OnUpdateAllLeft(CCmdUI* pCmdUI)
{
	// Check that left side is not readonly
	if (!IsReadOnly(TRUE))
		pCmdUI->Enable(GetDocument()->m_nDiffs!=0);
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

	GetDocument()->CopyAllList(true);
}

/**
 * @brief Called when "Copy all to right" item is updated
 */
void CMergeEditView::OnUpdateAllRight(CCmdUI* pCmdUI)
{
	// Check that right side is not readonly
	if (!IsReadOnly(FALSE))
		pCmdUI->Enable(GetDocument()->m_nDiffs!=0);
	else
		pCmdUI->Enable(FALSE);
}

/**
 * @brief Copy diffs inside selection from right to left
 */
void CMergeEditView::OnMultipleLeft()
{
	if (m_bIsLeft)
	{
		// We need the right selection, go to right view
		GetDocument()->GetRightView()->OnMultipleLeft();
		return;
	}

	// Check that left side is not readonly
	if (IsReadOnly(TRUE))
		return;

	int firstDiff, lastDiff;
	GetFullySelectedDiffs(firstDiff, lastDiff);
	if (lastDiff < firstDiff)
		return;

	GetDocument()->CopyMultipleList(false, firstDiff, lastDiff);
}

/**
 * @brief Update "Copy diffs in right selection to left" item
 */
void CMergeEditView::OnUpdateMultipleLeft(CCmdUI* pCmdUI)
{
	if (m_bIsLeft)
	{
		// We need the right selection, go to right view
		GetDocument()->GetRightView()->OnUpdateMultipleLeft(pCmdUI);
		return;
	}

	// Check that left side is not readonly
	if (IsReadOnly(TRUE))
		pCmdUI->Enable(FALSE);
	else
	{
		int firstDiff, lastDiff;
		GetFullySelectedDiffs(firstDiff, lastDiff);
		pCmdUI->Enable(lastDiff >= firstDiff);
	}
}

/**
 * @brief Copy diffs inside selection from left to right
 */
void CMergeEditView::OnMultipleRight()
{
	if (!m_bIsLeft)
	{
		// We need the left selection, go to left view
		GetDocument()->GetLeftView()->OnMultipleRight();
		return;
	}

	// Check that right side is not readonly
	if (IsReadOnly(FALSE))
		return;

	int firstDiff, lastDiff;
	GetFullySelectedDiffs(firstDiff, lastDiff);
	if (lastDiff < firstDiff)
		return;

	GetDocument()->CopyMultipleList(true, firstDiff, lastDiff);
}

/**
 * @brief Update "Copy diffs in left selection to right" item
 */
void CMergeEditView::OnUpdateMultipleRight(CCmdUI* pCmdUI)
{
	if (!m_bIsLeft)
	{
		// We need the left selection, go to left view
		GetDocument()->GetLeftView()->OnUpdateMultipleRight(pCmdUI);
		return;
	}

		// Check that right side is not readonly
	if (IsReadOnly(FALSE))
		pCmdUI->Enable(FALSE);
	else
	{
		int firstDiff, lastDiff;
		GetFullySelectedDiffs(firstDiff, lastDiff);
		pCmdUI->Enable(lastDiff >= firstDiff);
	}
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
	int nDiff = pd->GetCurrentDiff();

	// Try to trap some errors
	if (nDiff >= (int)pd->m_nDiffs)
		_RPTF2(_CRT_ERROR, "Selected diff > diffcount (%d > %d)!",
			nDiff, pd->m_nDiffs);

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

	if (nDiff >= 0 && nDiff < (int)pd->m_nDiffs)
	{
		CPoint ptStart, ptEnd;
		ptStart.x = 0;
		ptStart.y = pd->m_diffs[nDiff].dbegin0;
		ptEnd.x = 0;
		ptEnd.y = pd->m_diffs[nDiff].dend0;

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

BOOL CMergeEditView::PreTranslateMessage(MSG* pMsg)
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

/// Override from CCrystalTextView
void CMergeEditView::
OnUpdateCaret()
{
	if (m_piMergeEditStatus && IsTextBufferInitialized())
	{
		CPoint cursorPos = GetCursorPos();
		int nScreenLine = cursorPos.y;
		int nRealLine = ComputeRealLine(nScreenLine);
		CString sLine;
		int chars;
		CString sEol;
		// Is this a ghost line ?
		if (m_pTextBuffer->GetLineFlags(nScreenLine) & LF_GHOST)
		{
			// Ghost lines display eg "Line 12-13"
			sLine.Format(_T("%d-%d"), nRealLine, nRealLine+1);
			chars = -1;
			sEol = _T("");
		}
		else
		{
			// Regular lines display eg "Line 13 Characters: 25 EOL: CRLF"
			sLine.Format(_T("%d"), nRealLine+1);
			chars = GetLineLength(nScreenLine);
			if (mf->m_options.GetInt(OPT_ALLOW_MIXED_EOL))
			sEol = GetTextBufferEol(nScreenLine);
			else
				sEol = _T("hidden");
		}
		m_piMergeEditStatus->SetLineInfo(sLine, cursorPos.x + 1, chars, sEol);
	}
}

/// Highlight difference in current line
void CMergeEditView::OnShowlinediff()
{
	// Pass this to the document, to compare this file to other
	GetDocument()->Showlinediff(this);
}

/// Enable highlight menuitem if current line is flagged as having a difference
void CMergeEditView::OnUpdateShowlinediff(CCmdUI* pCmdUI)
{
	int line = GetCursorPos().y;
	BOOL enable = GetLineFlags(line) & LF_DIFF;
	pCmdUI->Enable(enable);
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



typedef struct {
		struct HWND__ *	pWnd;
		CPoint point;
}				CallbackDataForContextMenu;

/**
 * @brief Callback for the context menu : display the menu, let the user choose a function
 */
int callbackForContextMenu(CStringArray * functionNamesList, void * receivedData)
{
	// create the menu and populate it with the available functions
	HMENU hMenu = ::CreatePopupMenu();

	int i;
	int ID = 101;	// first ID in menu
	for (i = 0 ; i < functionNamesList->GetSize() ; i++, ID++)
		::AppendMenu(hMenu, MF_STRING, ID, (*functionNamesList)[i]);

	::AppendMenu(hMenu, MF_SEPARATOR, (UINT) -1, 0);
	::AppendMenu(hMenu, MF_STRING, 100, _T("Unload script"));

	// wait for the user choice
	CallbackDataForContextMenu * data = (CallbackDataForContextMenu*) receivedData;
	int response = ::TrackPopupMenu(hMenu, TPM_RETURNCMD, data->point.x, data->point.y, 0, data->pWnd, 0);
	::DestroyMenu(hMenu);

	// return the chosen function, or apply "Unload script"
	if (response)
	{
		if (response == 100)
		{
			CAllThreadsScripts::GetActiveSet()->FreeScriptsForEvent(L"CONTEXT_MENU");
		}
		else
			return response-101;
	}

	// return an invalid value to cancel
	return -1;
}

/**
 * @brief Offer a context menu built with scriptlet/ActiveX functions
 */
void CMergeEditView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// text is CHAR if compiled without UNICODE, WCHAR with UNICODE
	CString text = GetSelectedText();

	// Context menu opened using keyboard has no coordinates
	if (point.x == -1 && point.y == -1)
	{
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);

		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	// prepare the tracking data for the callback
	CallbackDataForContextMenu data;
	data.point = point;
	data.pWnd = m_hWnd;

	// transform the text with a script/ActiveX function, event=USER_CONTEXT_MENU
	BOOL bChanged = TextTransform_Interactive(text, L"CONTEXT_MENU", callbackForContextMenu, &data);

	if (bChanged)
		// now replace the text
		ReplaceSelection(text, 0);
}

/**
 * @brief Update left EOL mode in status bar
 */
void CMergeEditView::OnUpdateStatusLeftEOL(CCmdUI* pCmdUI)
{
	if (mf->m_options.GetInt(OPT_ALLOW_MIXED_EOL))
		pCmdUI->SetText(_T(""));
	else
		GetDocument()->GetLeftView()->OnUpdateIndicatorCRLF(pCmdUI);
}

/**
 * @brief Update right EOL mode in status bar
 */
void CMergeEditView::OnUpdateStatusRightEOL(CCmdUI* pCmdUI)
{
	if (mf->m_options.GetInt(OPT_ALLOW_MIXED_EOL))
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

	if (mf->m_options.GetInt(OPT_ALLOW_MIXED_EOL) ||
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

	if (m_bIsLeft)
		nRealLine = pDoc->m_ltBuf.ComputeRealLine(pos.y);
	else
		nRealLine = pDoc->m_rtBuf.ComputeRealLine(pos.y);

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
			int nApparentLine = 0;
			int nLineCount = 0;

			if (nRealLine < 0)
				nRealLine = 0;

			// Compute apparent (shown linenumber) line
			if (dlg.m_nFile == 0)
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

			CPoint ptPos;
			ptPos.x = 0;
			ptPos.y = nApparentLine;

			// Scroll line to center of view
			const int offset = GetScreenLines() / 2;
			int nScrollLine = nApparentLine - offset;
			if (nScrollLine < 0)
				nScrollLine = 0;
			pCurrentView->ScrollToLine(nScrollLine);
			pOtherView->ScrollToLine(nScrollLine);
			pCurrentView->SetCursorPos(ptPos);
			pOtherView->SetCursorPos(ptPos);
			pCurrentView->SetAnchor(ptPos);
			pOtherView->SetAnchor(ptPos);
		}
		else
		{
			int diff = _ttoi(dlg.m_strParam) - 1;
			if (diff < 0)
				diff = 0;
			if (diff >= pDoc->m_nDiffs)
				diff = pDoc->m_nDiffs;

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
	m_bSyntaxHighlight = mf->m_options.GetInt(OPT_SYNTAX_HIGHLIGHT);
	m_cachedColors.clrDiff = mf->m_options.GetInt(OPT_CLR_DIFF);
	m_cachedColors.clrSelDiff = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF);
	m_cachedColors.clrDiffDeleted = mf->m_options.GetInt(OPT_CLR_DIFF_DELETED);
	m_cachedColors.clrSelDiffDeleted = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF_DELETED);
	m_cachedColors.clrDiffText = mf->m_options.GetInt(OPT_CLR_DIFF_TEXT);
	m_cachedColors.clrSelDiffText = mf->m_options.GetInt(OPT_CLR_SELECTED_DIFF_TEXT);
	m_cachedColors.clrTrivial = mf->m_options.GetInt(OPT_CLR_TRIVIAL_DIFF);
	m_cachedColors.clrTrivialDeleted = mf->m_options.GetInt(OPT_CLR_TRIVIAL_DIFF_DELETED);
}

