// MergeEditView.cpp : implementation file
//

#include "stdafx.h"
#include "merge.h"
#include "MergeEditView.h"
#include "MergeDoc.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMergeEditView

IMPLEMENT_DYNCREATE(CMergeEditView, CCrystalEditViewEx)

CMergeEditView::CMergeEditView()
{
	m_bIsLeft=FALSE;
	m_nModifications=0;
  SetParser (&m_xParser);
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



BOOL CMergeEditView::SaveHelper()
{
	BOOL result = TRUE;
	CMergeDoc *pd = GetDocument();
	CString s;

	AfxFormatString1(s, IDS_SAVE_FMT, pd->m_strLeftFile); 
	if (mf->m_pLeft
		&& mf->m_pLeft->IsModified())
	{
		switch(AfxMessageBox(s, MB_YESNOCANCEL|MB_ICONQUESTION))
		{
		case IDYES:
			if (pd->DoSave(pd->m_strLeftFile, mf->m_pLeft, TRUE))
				mf->m_pLeft->ResetMod();
			else
				result=FALSE;
			break;
		case IDNO:
			break;
		default:  // IDCANCEL
			result=FALSE;
			break;
		}
	}

	AfxFormatString1(s, IDS_SAVE_FMT, pd->m_strRightFile); 
	if (mf->m_pRight
		&& mf->m_pRight->IsModified())
	{
		switch(AfxMessageBox(s, MB_YESNOCANCEL|MB_ICONQUESTION))
		{
		case IDYES:
			if (pd->DoSave(pd->m_strRightFile, mf->m_pRight, FALSE))
				mf->m_pRight->ResetMod();
			else
				result=FALSE;
			break;
		case IDNO:
			break;
		default:  // IDCANCEL
			result=FALSE;
			break;
		}
	}
	return result;
}

void CMergeEditView::UpdateResources()
{

}

BOOL CMergeEditView::PrimeListWithFile(LPCTSTR szFilename)
{
	CMergeDoc *pd = GetDocument();
	CStdioFile file;
	UINT extras=0;   // extra lines added to view

	// get the text buffer for the current view and initialize it
	CMergeDoc::CDiffTextBuffer *pBuf = static_cast<CMergeDoc::CDiffTextBuffer*>(LocateTextBuffer());

	CString blankline(_T(""));
	int blanklen=blankline.GetLength();

	// walk the diff stack and flag the line codes
	pd->SetCurrentDiff(-1);
	for (int nDiff=0; nDiff < static_cast<int>(pd->m_nDiffs); ++nDiff)
	{
		DIFFRANGE &curDiff = pd->m_diffs[nDiff];

		// handle left-only for the left view
		switch (curDiff.op)
		{
		case OP_LEFTONLY:
			if (m_bIsLeft)
			{
				// just flag the lines
				curDiff.dbegin0 = curDiff.begin0+extras;
				curDiff.dend0 = curDiff.end0+extras;
				curDiff.blank0 = -1;
				for (UINT i=curDiff.dbegin0; i <= curDiff.dend0; i++)
				{
					pBuf->SetLineFlag(i, LF_LEFT_ONLY, TRUE, FALSE, FALSE);
				}
			}
			else
			{
				// need to insert blanks to compensate for diff on other side
				curDiff.dbegin1 = curDiff.begin1+extras;
				curDiff.dend1 = curDiff.dbegin1+(curDiff.end0-curDiff.begin0);
				curDiff.blank1 = curDiff.dbegin1;
				for (UINT i=curDiff.dbegin1; i <= curDiff.dend1; i++)
				{
					pBuf->InsertLine(blankline, blanklen, i);
					pBuf->SetLineFlag(i, LF_LEFT_ONLY, TRUE, FALSE, FALSE);
					extras++;
				}
			}
			break;
		case OP_RIGHTONLY:
			if (!m_bIsLeft)
			{
				// just flag the lines
				curDiff.dbegin1 = curDiff.begin1+extras;
				curDiff.dend1 = curDiff.end1+extras;
				curDiff.blank1 = -1;
				for (UINT i=curDiff.dbegin1; i <= curDiff.dend1; i++)
				{
					pBuf->SetLineFlag(i, LF_RIGHT_ONLY, TRUE, FALSE, FALSE);
				}
			}
			else
			{
				// need to insert blanks to compensate for diff on other side
				curDiff.dbegin0 = curDiff.begin0+extras;
				curDiff.dend0 = curDiff.dbegin0+(curDiff.end1-curDiff.begin1);
				curDiff.blank0 = curDiff.dbegin0;
				for (UINT i=curDiff.dbegin0; i <= curDiff.dend0; i++)
				{
					pBuf->InsertLine(blankline, blanklen, i);
					pBuf->SetLineFlag(i, LF_RIGHT_ONLY, TRUE, FALSE, FALSE);
					extras++;
				}
			}
			break;
		case OP_DIFF:
			{
				if (m_bIsLeft)
				{
					// just flag the lines
					curDiff.dbegin0 = curDiff.begin0+extras;
					curDiff.dend0 = curDiff.end0+extras;
					for (UINT i=curDiff.dbegin0; i <= curDiff.dend0; i++)
					{
						pBuf->SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
					}

					// insert blanks if needed
					int blanks = (curDiff.end1-curDiff.begin1)-(curDiff.end0-curDiff.begin0);
					if (blanks>0)
					{
						curDiff.blank0 = curDiff.dend0+1;
						curDiff.blank1 = -1;
						for (int b=0; b < blanks; b++)
						{
							int idx = curDiff.blank0+b;
							pBuf->InsertLine(blankline, blanklen, idx);
							pBuf->SetLineFlag(idx, LF_RIGHT_ONLY, TRUE, FALSE, FALSE);
							curDiff.dend0++;
							extras++;
						}
					}
				}
				else
				{
					// just flag the lines
					curDiff.dbegin1 = curDiff.begin1+extras;
					curDiff.dend1 = curDiff.end1+extras;
					for (UINT i=curDiff.dbegin1; i <= curDiff.dend1; i++)
					{
						pBuf->SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
					}

					// insert blanks if needed
					int blanks = (curDiff.end0-curDiff.begin0)-(curDiff.end1-curDiff.begin1);
					if (blanks>0)
					{
						curDiff.blank1 = curDiff.dend1+1;
						curDiff.blank0 = -1;
						for (int b=0; b < blanks; b++)
						{
							int idx = curDiff.blank1+b;
							pBuf->InsertLine(blankline, blanklen, idx);
							pBuf->SetLineFlag(idx, LF_LEFT_ONLY, TRUE, FALSE, FALSE);
							curDiff.dend1++;
							extras++;
						}
					}
				}

			}
			break;
		}
	}


	pBuf->SetReadOnly(FALSE);
	SetWordWrapping(FALSE);
	ResetView();

	return TRUE;
}

CString CMergeEditView::GetLineText(int idx)
{
	return CString(GetLineChars(idx));
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
	GetDocument()->m_pView = this;
}

void CMergeEditView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CCrystalEditViewEx::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CMergeEditView::GetLineColors (int nLineIndex, COLORREF & crBkgnd,
                                COLORREF & crText, BOOL & bDrawWhitespace)
{
  if (mf->m_bHiliteSyntax)
	CCrystalEditViewEx::GetLineColors(nLineIndex, crBkgnd, crText, bDrawWhitespace);
  DWORD dwLineFlags = GetLineFlags (nLineIndex);
  if (dwLineFlags & LF_DIFF)
  {
	  if (IsLineInCurrentDiff(nLineIndex))
		crBkgnd = RGB(255,0,0);
	  else
		crBkgnd = RGB(255,255,92);
      crText = RGB(0,0,0);
	  bDrawWhitespace = TRUE;
      return;
  }
  else if (dwLineFlags & LF_DELETED)
  {
      if (IsLineInCurrentDiff(nLineIndex))
		crBkgnd = RGB(255,0,0);
	  else
		crBkgnd = RGB(192,192,192);
      crText = RGB(0,0,0);
	  bDrawWhitespace = TRUE;
      return;
  }
  else if (dwLineFlags & LF_LEFT_ONLY)
  {
	  if (IsLineInCurrentDiff(nLineIndex))
		crBkgnd = RGB(255,0,0);
	  else if (m_bIsLeft)
		crBkgnd = RGB(255,255,92);
	  else
		crBkgnd = RGB(192,192,192);
      crText = RGB(0,0,0);
	  bDrawWhitespace = TRUE;
      return;
  }
  else if (dwLineFlags & LF_RIGHT_ONLY)
  {
	  if (IsLineInCurrentDiff(nLineIndex))
		crBkgnd = RGB(255,0,0);
	  else if (m_bIsLeft)
		crBkgnd = RGB(192,192,192);
	  else
		crBkgnd = RGB(255,255,92);
      crText = RGB(0,0,0);
	  bDrawWhitespace = TRUE;
      return;
  }  
  else if (!mf->m_bHiliteSyntax)
  {
	  crBkgnd = RGB(255,255,255);
	  crText = RGB(0,0,0);
	  bDrawWhitespace = FALSE;
	  return;
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
          ASSERT (pSrcView->m_nTopLine < GetLineCount ());
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
          ASSERT (pSrcView->m_nOffsetChar < GetMaxLineLength ());
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
	mf->m_pLeft->SelectNone();
	mf->m_pRight->SelectNone();
	pd->SetCurrentDiff(nDiff);
	if (nDiff >= 0
		&& nDiff < (int)pd->m_nDiffs)
	{
		CPoint ptStart, ptEnd;
		ptStart.x = 0;
		ptStart.y = pd->m_diffs[nDiff].dbegin0;

		if (bScroll)
		{		
			int line = ptStart.y-CONTEXT_LINES;
			if (line<0)
				line=0;
			mf->m_pLeft->ScrollToLine(line);
			mf->m_pRight->ScrollToLine(line);
			SetCursorPos(ptStart);
		}

		if (bSelectText)
		{
			ptEnd.y = pd->m_diffs[nDiff].dend0;
			ptEnd.x = mf->m_pLeft->GetLineLength(ptEnd.y);
			mf->m_pLeft->SetSelection(ptStart, ptEnd);

			ptEnd.x = mf->m_pRight->GetLineLength(ptEnd.y);
			mf->m_pRight->SetSelection(ptStart, ptEnd);
			UpdateCaret();
		}
		else
		{
			mf->m_pLeft->Invalidate();
			mf->m_pRight->Invalidate();
		}

	}
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
			PushCursor();
			SelectDiff(nDiff, TRUE, FALSE);
			PopCursor();
		}
	}
}

void CMergeEditView::OnUpdateCurdiff(CCmdUI* pCmdUI) 
{
	CMergeDoc *pd = GetDocument();
	pCmdUI->Enable(pd!=NULL && pd->GetCurrentDiff()!=-1);
}

void CMergeEditView::OnEditCopy() 
{
	CCrystalEditViewEx::Copy();
}

void CMergeEditView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	CCrystalEditViewEx::OnUpdateEditCopy(pCmdUI);
}

void CMergeEditView::OnEditCut() 
{
	CCrystalEditViewEx::Cut();
	GetDocument()->FlushAndRescan();
	m_pTextBuffer->SetModified(TRUE);
}

void CMergeEditView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	CCrystalEditViewEx::OnUpdateEditCut(pCmdUI);
}

void CMergeEditView::OnEditPaste() 
{
	CCrystalEditViewEx::Paste();
	GetDocument()->FlushAndRescan();
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
		mf->m_pLeft->ScrollToLine(line);
		mf->m_pRight->ScrollToLine(line);

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

	if (curDiff+1 >= (int)pd->m_nDiffs)
	{
		// we're on the last diff already, so remove the selection
		mf->m_pLeft->SelectNone();
		mf->m_pRight->SelectNone();
	}
	else if (curDiff!=-1)
	{
		// we're on a diff, so just select the next one
		SelectDiff(curDiff+1, TRUE, FALSE);
	}
	else
	{
		// we're not on a diff, so figure out which one to select
		int line = GetCursorPos().y;
		if (!IsValidTextPosY(CPoint(0,line)))
			line = m_nTopLine;
		for (UINT i=0; i < pd->m_nDiffs; i++)
		{
			if ((int)pd->m_diffs[i].dbegin0 >= line)
			{
				curDiff=i;
				SelectDiff(i, TRUE, FALSE);
				break;
			}		
		}
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

	if (curDiff==0)
	{
		// we're on the first diff already, so remove the selection
		mf->m_pLeft->SelectNone();
		mf->m_pRight->SelectNone();
	}
	else if (curDiff!=-1)
	{
		// we're on a diff, so just select the next one
		SelectDiff(curDiff-1, TRUE, FALSE);
	}
	else
	{
		// we're not on a diff, so figure out which one to select
		int line = GetCursorPos().y;
		if (!IsValidTextPosY(CPoint(0,line)))
			line = m_nTopLine;
		for (int i=pd->m_nDiffs-1; i >= 0 ; i--)
		{
			if ((int)pd->m_diffs[i].dend0 <= line)
			{
				curDiff=i;
				SelectDiff(i, TRUE, FALSE);
				break;
			}		
		}
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
		mf->m_pLeft->Invalidate();
		mf->m_pRight->Invalidate();
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
		mf->m_pLeft->Invalidate();
		mf->m_pRight->Invalidate();
	}
}

void CMergeEditView::UpdateLineLengths()
{
	//m_nMaxLineLength=-1;
	GetMaxLineLength();
}

void CMergeEditView::OnL2r() 
{
	GetDocument()->ListCopy(true);
}

void CMergeEditView::OnUpdateL2r(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
}

void CMergeEditView::OnR2l() 
{
	GetDocument()->ListCopy(false);
}

void CMergeEditView::OnUpdateR2l(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->GetCurrentDiff()!=-1);
}

void CMergeEditView::OnAllLeft() 
{
	CMergeDoc* pDoc = GetDocument();
	// copy from bottom up is more efficient
	for(int i = pDoc->m_nDiffs-1; i>=0; --i)
	{
		pDoc->SetCurrentDiff(i);
		pDoc->ListCopy(false);
	}
}

void CMergeEditView::OnUpdateAllLeft(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->m_nDiffs!=0);
}

void CMergeEditView::OnAllRight() 
{
	CMergeDoc* pDoc = GetDocument();
	// copy from bottom up is more efficient
	for(int i = pDoc->m_nDiffs-1; i>=0; --i)
	{
		pDoc->SetCurrentDiff(i);
		pDoc->ListCopy(true);
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
	if(dynamic_cast<CMergeDoc::CDiffTextBuffer*>(m_pTextBuffer)->lastUndoGroup())
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

	// keep document up to date
	pDoc->FlushAndRescan();
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
