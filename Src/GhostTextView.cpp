////////////////////////////////////////////////////////////////////////////
//  File:       GhostTextView.h
//  Version:    1.0.0.0
//  Created:    31-Jul-2003
//
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


#include "stdafx.h"
#include "GhostTextView.h"
#include "GhostTextBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE (CGhostTextView, CCrystalEditViewEx)

/** 
 * @brief Constructor, initializes members.
 */
CGhostTextView::CGhostTextView()
: m_pGhostTextBuffer(NULL)
{
}

void CGhostTextView::
ReAttachToBuffer (CCrystalTextBuffer * pBuf /*= NULL*/ )
{
	if (pBuf == NULL)
	{
		pBuf = LocateTextBuffer ();
		// ...
	}
	m_pGhostTextBuffer = dynamic_cast<CGhostTextBuffer*> (pBuf);
	CCrystalEditViewEx::ReAttachToBuffer(pBuf);
}

void CGhostTextView::
AttachToBuffer (CCrystalTextBuffer * pBuf /*= NULL*/ )
{
	if (pBuf == NULL)
	{
		pBuf = LocateTextBuffer ();
		// ...
	}
	m_pGhostTextBuffer = dynamic_cast<CGhostTextBuffer*> (pBuf);
	CCrystalEditViewEx::AttachToBuffer(pBuf);
}

void CGhostTextView::
DetachFromBuffer ()
{
	if (m_pGhostTextBuffer != NULL)
		m_pGhostTextBuffer = NULL;
	CCrystalEditViewEx::DetachFromBuffer();
}

void CGhostTextView::popPosition(SCursorPushed Ssrc, CPoint & pt)
{
	pt.x = Ssrc.x;
	pt.y = m_pGhostTextBuffer->ComputeApparentLine(Ssrc.y, Ssrc.nToFirstReal);
	// if the cursor was in a trailing ghost line, and this disappeared,
	// got at the end of the last line
	if (pt.y >= GetLineCount())
	{
		pt.y = GetLineCount()-1;
		pt.x = GetLineLength(pt.y);
	}
}

void CGhostTextView::pushPosition(SCursorPushed & Sdest, CPoint pt)
{
	Sdest.x = pt.x;
	Sdest.y = m_pGhostTextBuffer->ComputeRealLineAndGhostAdjustment(pt.y, Sdest.nToFirstReal);
}

void CGhostTextView::PopCursors ()
{
	CPoint ptCursorLast = m_ptCursorLast;
	popPosition(m_ptCursorPosPushed, ptCursorLast);

	ASSERT_VALIDTEXTPOS (ptCursorLast);
	SetCursorPos (ptCursorLast);

	popPosition(m_ptSelStartPushed, m_ptSelStart);
	ASSERT_VALIDTEXTPOS (m_ptSelStart);
	popPosition(m_ptSelEndPushed, m_ptSelEnd);
	ASSERT_VALIDTEXTPOS (m_ptSelEnd);
	popPosition(m_ptAnchorPushed, m_ptAnchor);
	ASSERT_VALIDTEXTPOS (m_ptAnchor);
	// laoran 2003/09/03
	// here is what we did before, maybe we have to do it, but test with pushed positions
	// SetSelection (ptCursorLast, ptCursorLast);
	// SetAnchor (ptCursorLast);

	if (m_bDraggingText == TRUE)
	{
		popPosition(m_ptDraggedTextBeginPushed, m_ptDraggedTextBegin);
		ASSERT_VALIDTEXTPOS(m_ptDraggedTextBegin);
		popPosition(m_ptDraggedTextEndPushed, m_ptDraggedTextEnd);
		ASSERT_VALIDTEXTPOS(m_ptDraggedTextEnd);
	}
	if (m_bDropPosVisible == TRUE)
	{
		popPosition(m_ptSavedCaretPosPushed, m_ptSavedCaretPos);
		ASSERT_VALIDTEXTPOS(m_ptSavedCaretPos);
	}
	if (m_bSelectionPushed == TRUE)
	{
		popPosition(m_ptSavedSelStartPushed, m_ptSavedSelStart);
		ASSERT_VALIDTEXTPOS(m_ptSavedSelStart);
		popPosition(m_ptSavedSelEndPushed, m_ptSavedSelEnd);
		ASSERT_VALIDTEXTPOS(m_ptSavedSelEnd);
	}

	CPoint ptLastChange;
	if (m_ptLastChangePushed.y == 0 && m_ptLastChangePushed.nToFirstReal > 0)
		ptLastChange = CPoint(-1,-1);
	else 
	{
		popPosition(m_ptLastChangePushed, ptLastChange);
		ASSERT_VALIDTEXTPOS(ptLastChange);
	}
	m_pGhostTextBuffer->RestoreLastChangePos(ptLastChange);

	// restore the scrolling position
	CPoint temp;
	popPosition(m_nTopLinePushed, temp);
	ASSERT_VALIDTEXTPOS(temp);
	ScrollToLine(temp.y);
}

void CGhostTextView::PushCursors ()
{
	pushPosition(m_ptCursorPosPushed, m_ptCursorPos);
	pushPosition(m_ptSelStartPushed, m_ptSelStart);
	pushPosition(m_ptSelEndPushed, m_ptSelEnd);
	pushPosition(m_ptAnchorPushed, m_ptAnchor);
	if (m_bDraggingText == TRUE)
	{
		pushPosition(m_ptDraggedTextBeginPushed, m_ptDraggedTextBegin);
		pushPosition(m_ptDraggedTextEndPushed, m_ptDraggedTextEnd);
	}
	if (m_bDropPosVisible == TRUE)
	{
		pushPosition(m_ptSavedCaretPosPushed, m_ptSavedCaretPos);
	}
	if (m_bSelectionPushed == TRUE)
	{
		pushPosition(m_ptSavedSelStartPushed, m_ptSavedSelStart);
		pushPosition(m_ptSavedSelEndPushed, m_ptSavedSelEnd);
	}

	pushPosition(m_ptLastChangePushed, m_pGhostTextBuffer->GetLastChangePos());

	// and top line positions
	pushPosition(m_nTopLinePushed, CPoint(0, m_nTopLine));
}




int CGhostTextView::ComputeRealLine (int nApparentLine) const
{
	if (!m_pGhostTextBuffer)
		return 0;
	return m_pGhostTextBuffer->ComputeRealLine(nApparentLine);
}

int CGhostTextView::ComputeApparentLine (int nRealLine) const
{
	return m_pGhostTextBuffer->ComputeApparentLine(nRealLine);
}

void CGhostTextView::GetTextWithoutEmptys (int nStartLine, int nStartChar, int nEndLine, int nEndChar, CString &text, int nCrlfStyle /*=CRLF_STYLE_AUTOMATIC*/ )
{
  if (m_pGhostTextBuffer != NULL)
    m_pGhostTextBuffer->GetTextWithoutEmptys (nStartLine, nStartChar, nEndLine, nEndChar, text, nCrlfStyle);
  else
    text = _T ("");
}

HGLOBAL CGhostTextView::PrepareDragData ()
{
	PrepareSelBounds ();
	if (m_ptDrawSelStart == m_ptDrawSelEnd)
		return NULL;

	CString text;
	GetTextWithoutEmptys (m_ptDrawSelStart.y, m_ptDrawSelStart.x, m_ptDrawSelEnd.y, m_ptDrawSelEnd.x, text);
	HGLOBAL hData =::GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, (_tcslen (text)+1)*sizeof(TCHAR));
	if (hData == NULL)
		return NULL;

	LPTSTR pszData = (LPTSTR)::GlobalLock (hData);
	_tcscpy (pszData, text.GetBuffer (0));
	text.ReleaseBuffer ();
	::GlobalUnlock (hData);

	m_ptDraggedTextBegin = m_ptDrawSelStart;
	m_ptDraggedTextEnd = m_ptDrawSelEnd;
	return hData;
}

/**
 * @brief Draw selection margin. 
 * @param [in] pdc         Pointer to draw context.
 * @param [in] rect        The rectangle to draw.
 * @param [in] nLineIndex  Index of line in view.
 * @param [in] nLineNumber Line number to display. if -1, it's not displayed.
 */
void CGhostTextView::DrawMargin (CDC * pdc, const CRect & rect, int nLineIndex, int nLineNumber)
{
	int nRealLineNumber;
	if (nLineIndex < 0 || GetLineFlags(nLineIndex) & LF_GHOST)
		nRealLineNumber = -1;
	else
		nRealLineNumber = ComputeRealLine(nLineIndex) + 1;
	CCrystalTextView::DrawMargin(pdc, rect, nLineIndex, nRealLineNumber);
}
