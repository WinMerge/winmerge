// MergeEditView.cpp : implementation file
//

#include "stdafx.h"
#include "merge.h"
#include "MergeEditView.h"
#include "MergeDoc.h"
#include "MainFrm.h"
#include "WaitStatusCursor.h"
#include "MergeEditStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Timer ID and timeout for delayed rescan
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

void CMergeEditView::UpdateResources()
{

}

BOOL CMergeEditView::PrimeListWithFile()
{
	int nResumeTopLine = GetScrollPos(SB_VERT)+1;

	SetWordWrapping(FALSE);
	ResetView();
	RecalcVertScrollBar();
	SetTabSize(mf->m_nTabSize);
	GoToLine(nResumeTopLine, FALSE);

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

void CMergeEditView::GetLineColors(int nLineIndex, COLORREF & crBkgnd,
                                COLORREF & crText, BOOL & bDrawWhitespace)
{
	DWORD dwLineFlags = GetLineFlags(nLineIndex);
	
	// Line inside diff
	if (dwLineFlags & LF_WINMERGE_FLAGS)
	{
		// Black text inside diffs
		crText = RGB(0, 0, 0);
		bDrawWhitespace = TRUE;
		BOOL lineInCurrentDiff = IsLineInCurrentDiff(nLineIndex);

		if (dwLineFlags & LF_DIFF)
		{
			if (lineInCurrentDiff)
				crBkgnd = theApp.GetSelDiffColor();
			else
				crBkgnd = theApp.GetDiffColor();
			return;
		}
		else if (dwLineFlags & LF_DELETED)
		{
			if (lineInCurrentDiff)
				crBkgnd = theApp.GetSelDiffColor();
			else
				crBkgnd = RGB(192, 192, 192);
			return;
		}
		else if (dwLineFlags & LF_LEFT_ONLY)
		{
			if (lineInCurrentDiff)
				crBkgnd = theApp.GetSelDiffColor();
			else if (m_bIsLeft)
				crBkgnd = theApp.GetDiffColor();
			else
				crBkgnd = RGB(192, 192, 192);
			return;
		}
		else if (dwLineFlags & LF_RIGHT_ONLY)
		{
			if (lineInCurrentDiff)
				crBkgnd = theApp.GetSelDiffColor();
			else if (m_bIsLeft)
				crBkgnd = RGB(192, 192, 192);
			else
				crBkgnd = theApp.GetDiffColor();
			return;
		}
	}
	else
	{
		// If no syntax hilighting
		if (!theApp.m_bHiliteSyntax)
		{
			crBkgnd = RGB(255, 255, 255);
			crText = RGB(0, 0, 0);
			bDrawWhitespace = FALSE;
		}
		else
			// Line not inside diff, get colors from CrystalEditor
			CCrystalEditViewEx::GetLineColors(nLineIndex, crBkgnd,
				crText, bDrawWhitespace);
	}
}

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

		int nRows = pSplitterWnd->GetRowCount ();
		int nCols = pSplitterWnd->GetColumnCount ();
		for (int nRow = 0; nRow < nRows; nRow++)
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

void CMergeEditView::SelectDiff(int nDiff, BOOL bScroll /*=TRUE*/, BOOL bSelectText /*=TRUE*/)
{
	CMergeDoc *pd = GetDocument();
	SelectNone();
	pd->SetCurrentDiff(nDiff);
	ShowDiff(bScroll, bSelectText);
	pd->UpdateAllViews(this);
	UpdateSiblingScrollPos(FALSE);
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
		ptSelEnd.y, ptSelEnd.x, text, m_bIsLeft); 

	PutToClipboard(text);
}

void CMergeEditView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	CCrystalEditViewEx::OnUpdateEditCopy(pCmdUI);
}

void CMergeEditView::OnEditCut()
{
	CPoint ptSelStart, ptSelEnd;
	CMergeDoc * pDoc = GetDocument();
	GetSelection(ptSelStart, ptSelEnd);

	if ( ptSelStart == ptSelEnd )
		return;

	CString text;
	if (m_bIsLeft)
		pDoc->m_ltBuf.GetTextWithoutEmptys(ptSelStart.y, ptSelStart.x,
			ptSelEnd.y, ptSelEnd.x, text, m_bIsLeft);
	else
		pDoc->m_rtBuf.GetTextWithoutEmptys(ptSelStart.y, ptSelStart.x,
			ptSelEnd.y, ptSelEnd.x, text, m_bIsLeft);

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
	CCrystalEditViewEx::OnUpdateEditCut(pCmdUI);
}

void CMergeEditView::OnEditPaste()
{
	CCrystalEditViewEx::Paste();
	m_pTextBuffer->SetModified(TRUE);
}

void CMergeEditView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	CCrystalEditViewEx::OnUpdateEditPaste(pCmdUI);
}

void CMergeEditView::OnEditUndo()
{
	CMergeDoc* pDoc = GetDocument();
	CMergeEditView *tgt = *(pDoc->curUndo-1);
	if(tgt==this)
	{
		CCrystalEditViewEx::OnEditUndo();
		--pDoc->curUndo;
		pDoc->FlushAndRescan();
	}
	else
	{
		tgt->SendMessage(WM_COMMAND, ID_EDIT_UNDO);
	}
}

void CMergeEditView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CMergeDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc->curUndo!=pDoc->undoTgt.begin());
}

void CMergeEditView::OnFirstdiff()
{
	CMergeDoc *pd = GetDocument();
	if (pd->m_nDiffs > 0)
	{
		// scroll to the first line of the first diff, with some context thrown in
		int line = max(0, pd->m_diffs[0].dbegin0-CONTEXT_LINES);
		ScrollToLine(line);
		UpdateSiblingScrollPos(FALSE);

		// select the diff
		SelectDiff(0, TRUE, FALSE);
	}
}

void CMergeEditView::OnUpdateFirstdiff(CCmdUI* pCmdUI)
{
	OnUpdatePrevdiff(pCmdUI);
}

void CMergeEditView::OnLastdiff()
{
	CMergeDoc *pd = GetDocument();
	if (pd->m_nDiffs>0)
		SelectDiff(pd->m_nDiffs-1, TRUE, FALSE);
}

void CMergeEditView::OnUpdateLastdiff(CCmdUI* pCmdUI)
{
	OnUpdateNextdiff(pCmdUI);
}

void CMergeEditView::OnNextdiff()
{
	CMergeDoc *pd = GetDocument();
	int cnt = pd->m_ltBuf.GetLineCount();
	if (cnt <= 0)
		return;

	int curDiff = pd->GetCurrentDiff();
	if (curDiff != -1)
	{
		// we're on a diff, so just select the next one
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

void CMergeEditView::OnUpdateNextdiff(CCmdUI* pCmdUI)
{
	CMergeDoc *pd = GetDocument();
	CPoint pos = GetCursorPos();
	pCmdUI->Enable(pd->m_nDiffs>0 && pos.y < (long)pd->m_diffs[pd->m_nDiffs-1].dbegin0);
}

void CMergeEditView::OnPrevdiff()
{
	CMergeDoc *pd = GetDocument();
	int cnt = pd->m_ltBuf.GetLineCount();
	if (cnt <= 0)
		return;

	int curDiff = pd->GetCurrentDiff();

	if (curDiff != -1)
	{
		// we're on a diff, so just select the next one
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
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYL2R));
	GetDocument()->ListCopy(true);
}

void CMergeEditView::OnUpdateL2r(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
}

void CMergeEditView::OnR2l()
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYR2L));
	GetDocument()->ListCopy(false);
}

void CMergeEditView::OnUpdateR2l(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
}

void CMergeEditView::OnAllLeft()
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYALL2L));

	CMergeDoc* pDoc = GetDocument();
	// Unfortunately difftools is not designed for this kind
	// of use and sometimes all differences cannot be merged
	// in one run.
	while (pDoc->m_nDiffs > 0)
	{
		// copy from bottom up is more efficient
		for(int i = pDoc->m_nDiffs-1; i>=0; --i)
		{
			pDoc->SetCurrentDiff(i);
			pDoc->ListCopy(false);
		}
	}
}

void CMergeEditView::OnUpdateAllLeft(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->m_nDiffs!=0);
}

void CMergeEditView::OnAllRight()
{
	WaitStatusCursor waitstatus(LoadResString(IDS_STATUS_COPYALL2R));

	CMergeDoc* pDoc = GetDocument();
	// Unfortunately difftools is not designed for this kind
	// of use and sometimes all differences cannot be merged
	// in one run.
	while (pDoc->m_nDiffs > 0)
	{
		// copy from bottom up is more efficient
		for(int i = pDoc->m_nDiffs-1; i>=0; --i)
		{
			pDoc->SetCurrentDiff(i);
			pDoc->ListCopy(true);
		}
	}
}

void CMergeEditView::OnUpdateAllRight(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->m_nDiffs!=0);
}

void CMergeEditView::OnEditOperation(int nAction, LPCTSTR pszText)
{
	CMergeDoc* pDoc = GetDocument();

	// simple hook for multiplex undo operations
	if(dynamic_cast<CMergeDoc::CDiffTextBuffer*>(m_pTextBuffer)->curUndoGroup())
	{
		pDoc->undoTgt.erase(pDoc->curUndo, pDoc->undoTgt.end());
		pDoc->undoTgt.push_back(this);
		pDoc->curUndo = pDoc->undoTgt.end();
	}

	// perform original function
	CCrystalEditViewEx::OnEditOperation(nAction, pszText);

	// augment with additional operations

	// clear left only or right only flags
	// editing in 'blank' areas should be considered
	CPoint ptCursorPos = GetCursorPos ();
	m_pTextBuffer->SetLineFlag(ptCursorPos.y, LF_WINMERGE_FLAGS, FALSE, FALSE, FALSE);

	// If automatic rescan enabled, rescan after edit events
	if (m_bAutomaticRescan)
	{
		// keep document up to date
		// (Re)start timer to rescan only when user edits text
		// If timer starting fails, rescan immediately
		if (nAction == CE_ACTION_TYPING ||
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
		CCrystalEditViewEx::OnEditRedo();
		++pDoc->curUndo;
		pDoc->FlushAndRescan();
	}
	else
	{
		tgt->SendMessage(WM_COMMAND, ID_EDIT_REDO);
	}
}

void CMergeEditView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	CMergeDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc->curUndo!=pDoc->undoTgt.end());
}

void CMergeEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CCrystalEditViewEx::OnUpdate(pSender, lHint, pHint);
	// ShowDiff(FALSE, FALSE);
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
	// We get two kinds of timers
	// IDT_RESCAN means it has been a while since the user last made a change
	// so we'd like to do a rescan as soon as feasible
	// IDLE_TIMER means the application is idling, so now it is feasible to rescan
	// If we get the idle timer, we pass the buck to the document (ie, it may
	// still skip the rescan if it just did one recently).

	if (nIDEvent == IDT_RESCAN)
	{
		KillTimer(IDT_RESCAN);
		GetDocument()->SetNeedRescan();
		theApp.SetNeedIdleTimer();
	}
	else if (nIDEvent == IDLE_TIMER)
	{
		GetDocument()->RescanIfNeeded();
	}

	CCrystalEditViewEx::OnTimer(nIDEvent);
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
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_CLOSE);
		return FALSE;
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
		int chars = GetLineLength(nScreenLine);
		CString sEol = GetTextBufferEol(nScreenLine);
		m_piMergeEditStatus->SetLineInfo(nRealLine+1, chars, sEol);
	}
}
