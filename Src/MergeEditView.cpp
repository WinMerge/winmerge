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
#include <shlwapi.h>
#include "merge.h"
#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "MergeDoc.h"
#include "MainFrm.h"
#include "WaitStatusCursor.h"
#include "MergeEditStatus.h"
#include "lwdisp.h"

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMergeEditView diagnostics

#ifdef _DEBUG
void CMergeEditView::AssertValid() const
{
	CCrystalEditViewEx::AssertValid();
}

void CMergeEditView::Dump(CDumpContext& dc) const
{
	CCrystalEditViewEx::Dump(dc);
}
CMergeDoc* CMergeEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMergeDoc)));
	return (CMergeDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMergeEditView message handlers
CCrystalTextBuffer *CMergeEditView::LocateTextBuffer ()
{
	if (m_bIsLeft)
		return &GetDocument()->m_ltBuf;
	return &GetDocument()->m_rtBuf;
}

void CMergeEditView::DoScroll(UINT code, UINT pos, BOOL bDoScroll)
{
	TRACE(_T("Scroll %s: pos=%d\n"), m_bIsLeft? _T("left"):_T("right"), pos);
	if (bDoScroll
		&& (code == SB_THUMBPOSITION
			|| code == SB_THUMBTRACK))
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
	int nResumeTopLine = m_nTopLine;

	SetWordWrapping(FALSE);
	ResetView();
	RecalcVertScrollBar();
	SetTabSize(mf->m_nTabSize);

	ScrollToLine(nResumeTopLine);

	return TRUE;
}

CString CMergeEditView::GetLineText(int idx)
{
	return GetLineChars(idx);
}

CString CMergeEditView::GetSelectedText()
{
	CPoint ptStart, ptEnd;
	CString strText;
	GetSelection(ptStart, ptEnd);
	GetText(ptStart, ptEnd, strText);
	return strText;
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
	// TODO: Add your specialized code here and/or call the base class

	CCrystalEditViewEx::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

/**
 * @brief Determine text and backgdound color for line
 */
void CMergeEditView::GetLineColors(int nLineIndex, COLORREF & crBkgnd,
                                COLORREF & crText, BOOL & bDrawWhitespace)
{
	DWORD dwLineFlags = GetLineFlags(nLineIndex);
	
	// Line inside diff
	if (dwLineFlags & LF_WINMERGE_FLAGS)
	{
		crText = theApp.GetDiffTextColor();
		bDrawWhitespace = TRUE;
		BOOL lineInCurrentDiff = IsLineInCurrentDiff(nLineIndex);

		if (dwLineFlags & LF_DIFF)
		{
			if (lineInCurrentDiff)
			{
				crBkgnd = theApp.GetSelDiffColor();
				crText = theApp.GetSelDiffTextColor();
			}
			else
			{
				crBkgnd = theApp.GetDiffColor();
				crText = theApp.GetDiffTextColor();
			}
			return;
		}
		else if (dwLineFlags & LF_GHOST)
		{
			if (lineInCurrentDiff)
				crBkgnd = theApp.GetSelDiffDeletedColor();
			else
				crBkgnd = theApp.GetDiffDeletedColor();
			return;
		}

	}
	else
	{
		// Line not inside diff, 
		if (!theApp.m_bHiliteSyntax)
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
 * @brief Select diff by number
 */
void CMergeEditView::SelectDiff(int nDiff, BOOL bScroll /*=TRUE*/, BOOL bSelectText /*=TRUE*/)
{
	CMergeDoc *pd = GetDocument();
	SelectNone();
	pd->SetCurrentDiff(nDiff);
	ShowDiff(bScroll, bSelectText);
	pd->UpdateAllViews(this);
	UpdateSiblingScrollPos(FALSE);

	// notify either side, as it will notify the other one
	pd->GetLeftDetailView()->OnDisplayDiff(nDiff);
}

void CMergeEditView::OnCurdiff()
{
	CMergeDoc *pd = GetDocument();
	if (pd->m_nDiffs > 0)
	{
		// get the diff location
		int nDiff = pd->GetCurrentDiff();
		if (nDiff != -1)
		{
			// scroll to the first line of the first diff, with some context thrown in
			SelectDiff(nDiff, TRUE, FALSE);
		}
		else
		{
			CPoint pos = GetCursorPos();
			nDiff = pd->LineToDiff(pos.y);
			if (nDiff != -1)
				SelectDiff(nDiff, TRUE, FALSE);
		}
	}
}

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

void CMergeEditView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	CCrystalEditViewEx::OnUpdateEditCopy(pCmdUI);
}

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

void CMergeEditView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	if (!IsReadOnly(m_bIsLeft))
		CCrystalEditViewEx::OnUpdateEditCut(pCmdUI);
	else
		pCmdUI->Enable(FALSE);
}

void CMergeEditView::OnEditPaste()
{
	if (IsReadOnly(m_bIsLeft))
		return;

	CCrystalEditViewEx::Paste();
	m_pTextBuffer->SetModified(TRUE);
}

void CMergeEditView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	if (!IsReadOnly(m_bIsLeft))
		CCrystalEditViewEx::OnUpdateEditPaste(pCmdUI);
	else
		pCmdUI->Enable(FALSE);
}

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
			pDoc->FlushAndRescan();
		}
	}
	else
	{
		tgt->SendMessage(WM_COMMAND, ID_EDIT_UNDO);
	}
}

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
 */
void CMergeEditView::OnFirstdiff()
{
	CMergeDoc *pd = GetDocument();
	if (pd->m_nDiffs > 0)
	{
		// scroll to the first line of the first diff, with some context thrown in
		int line = pd->m_diffs[0].dbegin0-CONTEXT_LINES;
		if (line < 0)
			line = 0;
		ScrollToLine(line);
		UpdateSiblingScrollPos(FALSE);

		// select the diff
		SelectDiff(0, TRUE, FALSE);
	}
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
		if (curDiff == pd->m_nDiffs - 1)
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
 * @brief Go to previous diff
 * @note If no diff selected, previous diff above cursor
 * is selected. 
 */
void CMergeEditView::OnPrevdiff()
{
	CMergeDoc *pd = GetDocument();
	int cnt = pd->m_ltBuf.GetLineCount();
	if (cnt <= 0)
		return;

	// Returns -1 if no diff selected
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

void CMergeEditView::SelectNone()
{
	SetSelection (GetCursorPos(), GetCursorPos());
	UpdateCaret();
}

/// Check if line is inside currently selected diff
BOOL CMergeEditView::IsLineInCurrentDiff(int nLine)
{
	CMergeDoc *pd = GetDocument();
	int cur = pd->GetCurrentDiff();
	if (cur==-1)
		return FALSE;
	return (nLine >= (int)pd->m_diffs[cur].dbegin0 && nLine <= (int)pd->m_diffs[cur].dend0);
}

void CMergeEditView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CMergeDoc *pd = GetDocument();
	CPoint pos = GetCursorPos();

	int diff = pd->LineToDiff(pos.y);
	if (diff!=-1)
	{
		SelectDiff(diff, FALSE, FALSE);
//		mf->m_pLeft->Invalidate();
//		mf->m_pRight->Invalidate();
	}

	CCrystalEditViewEx::OnLButtonDblClk(nFlags, point);
}

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

void CMergeEditView::UpdateLineLengths()
{
	//m_nMaxLineLength=-1;
	GetMaxLineLength();
}

void CMergeEditView::OnL2r()
{
	// Check that right side is not readonly
	if (IsReadOnly(FALSE))
		return;
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYL2R));
	GetDocument()->ListCopy(true);
}

void CMergeEditView::OnUpdateL2r(CCmdUI* pCmdUI)
{
	// Check that right side is not readonly
	if (!IsReadOnly(FALSE))
		pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
	else
		pCmdUI->Enable(FALSE);
}

void CMergeEditView::OnR2l()
{
	// Check that left side is not readonly
	if (IsReadOnly(TRUE))
		return;
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYR2L));
	GetDocument()->ListCopy(false);
}

void CMergeEditView::OnUpdateR2l(CCmdUI* pCmdUI)
{
	// Check that left side is not readonly
	if (!IsReadOnly(TRUE))
		pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
	else
		pCmdUI->Enable(FALSE);
}

void CMergeEditView::OnAllLeft()
{
	// Check that left side is not readonly
	if (IsReadOnly(TRUE))
		return;
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYALL2L));

	GetDocument()->CopyAllList(false);
}

void CMergeEditView::OnUpdateAllLeft(CCmdUI* pCmdUI)
{
	// Check that left side is not readonly
	if (!IsReadOnly(TRUE))
		pCmdUI->Enable(GetDocument()->m_nDiffs!=0);
	else
		pCmdUI->Enable(FALSE);
}

void CMergeEditView::OnAllRight()
{
	// Check that right side is not readonly
	if (IsReadOnly(FALSE))
		return;

	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYALL2R));

	GetDocument()->CopyAllList(true);
}

void CMergeEditView::OnUpdateAllRight(CCmdUI* pCmdUI)
{
	// Check that right side is not readonly
	if (!IsReadOnly(FALSE))
		pCmdUI->Enable(GetDocument()->m_nDiffs!=0);
	else
		pCmdUI->Enable(FALSE);
}

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

	// If automatic rescan enabled, rescan after edit events
	if (m_bAutomaticRescan)
	{
		// keep document up to date
		// (Re)start timer to rescan only when user edits text
		// If timer starting fails, rescan immediately
		if (nAction == CE_ACTION_TYPING ||
			nAction == CE_ACTION_REPLACE ||
			nAction == CE_ACTION_BACKSPACE ||
			nAction == CE_ACTION_INDENT)
		{
			if (!SetTimer(IDT_RESCAN, RESCAN_TIMEOUT, NULL))
				pDoc->FlushAndRescan();
		}
		else
			pDoc->FlushAndRescan();
	}
}

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
			pDoc->FlushAndRescan();
		}
	}
	else
	{
		tgt->SendMessage(WM_COMMAND, ID_EDIT_REDO);
	}
}

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

void CMergeEditView::ShowDiff(BOOL bScroll, BOOL bSelectText)
{
	CMergeDoc *pd = GetDocument();
	int nDiff = pd->GetCurrentDiff();
	if (nDiff >= 0 && nDiff < (int)pd->m_nDiffs)
	{
		CPoint ptStart, ptEnd;
		ptStart.x = 0;
		ptStart.y = pd->m_diffs[nDiff].dbegin0;

		if (bScroll)
		{
			int line = ptStart.y - CONTEXT_LINES;
			if (line < 0)
				line = 0;
			ScrollToLine(line);
			SetCursorPos(ptStart);
		}

		if (bSelectText)
		{
			ptEnd.y = pd->m_diffs[nDiff].dend0;
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

BOOL CMergeEditView::IsReadOnly(BOOL bLeft)
{
	CCrystalTextBuffer *pBuf = NULL;

	if (bLeft)
		pBuf = &GetDocument()->m_ltBuf;
	else
		pBuf = &GetDocument()->m_rtBuf;

	return pBuf->GetReadOnly();
}

void CMergeEditView::OnRefresh()
{
	CMergeDoc *pd = GetDocument();
	ASSERT(pd);
	pd->FlushAndRescan(TRUE);
}

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
		// Ask about saving unsaved document
		CMergeDoc *pd = GetDocument();
		if (pd->SaveHelper())
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

void CMergeEditView::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	
	if (pd->m_ltBuf.IsModified() || pd->m_rtBuf.IsModified())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMergeEditView::OnLeftReadOnly()
{
	CMergeDoc *pd = GetDocument();
	BOOL bReadOnly = pd->m_ltBuf.GetReadOnly();
	pd->m_ltBuf.SetReadOnly(!bReadOnly);
}

void CMergeEditView::OnUpdateLeftReadOnly(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	BOOL bReadOnly = pd->m_ltBuf.GetReadOnly();
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(bReadOnly);
}

void CMergeEditView::OnRightReadOnly()
{
	CMergeDoc *pd = GetDocument();
	BOOL bReadOnly = pd->m_rtBuf.GetReadOnly();
	pd->m_rtBuf.SetReadOnly(!bReadOnly);
}

void CMergeEditView::OnUpdateRightReadOnly(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	BOOL bReadOnly = pd->m_rtBuf.GetReadOnly();
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(bReadOnly);
}

// Store our interface we use to display status line info
void CMergeEditView::SetStatusInterface(IMergeEditStatus * piMergeEditStatus)
{
	ASSERT(!m_piMergeEditStatus);
	m_piMergeEditStatus = piMergeEditStatus;
}

// Override from CCrystalTextView
void CMergeEditView::
OnUpdateCaret()
{
	if (m_piMergeEditStatus && IsTextBufferInitialized())
	{
		int nScreenLine = GetCursorPos().y;
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
			if (mf->m_bAllowMixedEol)
			sEol = GetTextBufferEol(nScreenLine);
			else
				sEol = _T("hidden");
		}
		m_piMergeEditStatus->SetLineInfo(sLine, chars, sEol);
	}
}

// Highlight difference in current line
void CMergeEditView::OnShowlinediff() 
{
	// Pass this to the document, to compare this file to other
	GetDocument()->Showlinediff(this);
}

// Enable highlight menuitem if current line is flagged as having a difference
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


static UINT NTAPI PopulateMenu(HMENU hMenu, UINT uID, LPDISPATCH piDispatch)
{
	if (piDispatch)
	{
		ITypeInfo *piTypeInfo;
		if SUCCEEDED(piDispatch->GetTypeInfo(0, 0, &piTypeInfo))
		{
			TYPEATTR *pTypeAttr;
			if SUCCEEDED(piTypeInfo->GetTypeAttr(&pTypeAttr))
			{
				UINT iMaxFunc = pTypeAttr->cFuncs - 1;
				for (UINT iFunc = 0 ; iFunc <= iMaxFunc ; ++iFunc)
				{
					UINT iFuncDesc = iMaxFunc - iFunc;
					FUNCDESC *pFuncDesc;
					if SUCCEEDED(piTypeInfo->GetFuncDesc(iFuncDesc, &pFuncDesc))
					{
						// exclude properties
						// exclude IDispatch inherited methods
						if (pFuncDesc->invkind & INVOKE_FUNC && !(pFuncDesc->wFuncFlags & 1))
						{
							BSTR bstrName;
							UINT cNames;
							if SUCCEEDED(piTypeInfo->GetNames(pFuncDesc->memid,
								&bstrName, 1, &cNames))
							{
								PCH pchName = B2A(bstrName);
								AppendMenu(hMenu, MF_STRING, uID++, pchName);
								SysFreeString(bstrName);
							}
						}
						piTypeInfo->ReleaseFuncDesc(pFuncDesc);
					}
				}
				piTypeInfo->ReleaseTypeAttr(pTypeAttr);
			}
			piTypeInfo->Release();
		}
	}
	return uID;
}

/*
 * @brief Offer a context menu build with scriptlet functions
 *
 * @note The scriptlet file is "contextmenu.sct" in WinMerge directory.
 * The path is always ANSI even if UNICODE is defined.
 */
void CMergeEditView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	static LPDISPATCH piScript = 0;

	if (piScript == 0)
	{
		char path[MAX_PATH];
		::GetModuleFileNameA(0, path, sizeof path);
		::PathRemoveFileSpecA(path);
		::PathAppendA(path, "contextmenu.sct");
		piScript = ::CreateDispatchBySource(path, "Scriptlet");
	}

	if (piScript == 0)
		return;

	HMENU hMenu = ::CreatePopupMenu();
	::PopulateMenu(hMenu, 101, piScript);
	::AppendMenu(hMenu, MF_SEPARATOR, -1, 0);
	::AppendMenu(hMenu, MF_STRING, 100, "Unload script");

	if (short response = ::TrackPopupMenu(hMenu, TPM_RETURNCMD, point.x, point.y, 0, m_hWnd, 0))
	{
		if (response == 100)
		{
			if (piScript)
			{
				piScript->Release();
				piScript = 0;
			}
		}
		else
		{
			// text is CHAR if compiled without UNICODE, WCHAR with UNICODE
			CString text = GetSelectedText();

			// variable to receive the method's return value:
			// derived from VARIANT, so it is always WCHAR
			LWRet ret;
			// as GetMenuStringW() does not work with Win9x,
			// first GetMenuStringA(), then MultiByteToWideChar():
			CHAR cName[260];
			::GetMenuStringA(hMenu, response, cName, DIMOF(cName), MF_BYCOMMAND);
			WCHAR wcName[260];
			::MultiByteToWideChar(CP_ACP, 0, cName, -1, wcName, DIMOF(wcName));
			// invoke method by name:
			::invokeW(piScript, &ret, wcName, opFxn[1], LWArgT(text));
			// when UNICODE is not defined, 
			// V_BSTR performs the conversion BSTR (wide char pointer) to lpchar
			text = V_BSTR(&ret);

			// now replace the text
			ReplaceSelection(text, 0);
		}
	}
	::DestroyMenu(hMenu);
}

/**
 * @brief Update left EOL mode in status bar
 */
void CMergeEditView::OnUpdateStatusLeftEOL(CCmdUI* pCmdUI)
{
	if (mf->m_bAllowMixedEol)
		pCmdUI->SetText(_T(""));
	else
		GetDocument()->GetLeftView()->OnUpdateIndicatorCRLF(pCmdUI);
}

/**
 * @brief Update right EOL mode in status bar
 */
void CMergeEditView::OnUpdateStatusRightEOL(CCmdUI* pCmdUI)
{
	if (mf->m_bAllowMixedEol)
		pCmdUI->SetText(_T(""));
	else
		GetDocument()->GetRightView()->OnUpdateIndicatorCRLF(pCmdUI);
}

/**
 * @brief Change EOL mode and unify all the lines EOL to this new mode
 */
void CMergeEditView::OnConvertEolTo(UINT nID ) 
{
	int nStyle;
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
	}
	m_pTextBuffer->SetCRLFMode(nStyle);

	// we don't need a derived applyEOLMode for ghost lines as they have no EOL char
	if (m_pTextBuffer->applyEOLMode())
	{
		CMergeDoc *pd = GetDocument();
		ASSERT(pd);
		pd->FlushAndRescan(TRUE);
	}
}

/**
 * @brief allow convert to entries in file submenu
 */
void CMergeEditView::OnUpdateConvertEolTo(CCmdUI* pCmdUI) 
{
	int nStyle;
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
	}

	if (mf->m_bAllowMixedEol || nStyle != m_pTextBuffer->GetCRLFMode())
	pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

