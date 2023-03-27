////////////////////////////////////////////////////////////////////////////
//  File:       GhostTextView.h
//  Version:    1.0.0.0
//  Created:    31-Jul-2003
//
/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//    SPDX-License-Identifier: GPL-2.0-or-later
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "GhostTextView.h"
#include "GhostTextBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE (CGhostTextView, CCrystalEditViewEx)

/** 
 * @brief Constructor, initializes members.
 */
CGhostTextView::CGhostTextView()
: m_pGhostTextBuffer(nullptr)
, m_ptCursorPosPushed{}
, m_ptSelStartPushed{}
, m_ptSelEndPushed{}
, m_ptAnchorPushed{}
, m_ptDraggedTextBeginPushed{}
, m_ptDraggedTextEndPushed{}
, m_ptSavedCaretPosPushed{}
, m_ptSavedSelStartPushed{}
, m_ptSavedSelEndPushed{}
, m_ptLastChangePushed{}
, m_nTopSubLinePushed(0)
, m_nOffsetCharPushed(0)
{
}

void CGhostTextView::
ReAttachToBuffer (CCrystalTextBuffer * pBuf /*= nullptr*/ )
{
	if (pBuf == nullptr)
	{
		pBuf = LocateTextBuffer ();
		// ...
	}
	m_pGhostTextBuffer = dynamic_cast<CGhostTextBuffer*> (pBuf);
	CCrystalEditViewEx::ReAttachToBuffer(pBuf);
}

void CGhostTextView::
AttachToBuffer (CCrystalTextBuffer * pBuf /*= nullptr*/ )
{
	if (pBuf == nullptr)
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
	m_pGhostTextBuffer = nullptr;
	CCrystalEditViewEx::DetachFromBuffer();
}

void CGhostTextView::popPosition(SCursorPushed Ssrc, CEPoint & pt)
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
	if (pt.y < 0)
		pt.y = 0;
}

void CGhostTextView::pushPosition(SCursorPushed & Sdest, CEPoint pt)
{
	Sdest.x = pt.x;
	if (m_pGhostTextBuffer)
		Sdest.y = m_pGhostTextBuffer->ComputeRealLineAndGhostAdjustment(pt.y, Sdest.nToFirstReal);
	else
		Sdest.y = pt.y;
}

void CGhostTextView::PopCursors ()
{
	CEPoint ptCursorLast = m_ptCursorLast;
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

	if (m_bDraggingText)
	{
		popPosition(m_ptDraggedTextBeginPushed, m_ptDraggedTextBegin);
		ASSERT_VALIDTEXTPOS(m_ptDraggedTextBegin);
		popPosition(m_ptDraggedTextEndPushed, m_ptDraggedTextEnd);
		ASSERT_VALIDTEXTPOS(m_ptDraggedTextEnd);
	}
	if (m_bDropPosVisible)
	{
		popPosition(m_ptSavedCaretPosPushed, m_ptSavedCaretPos);
		ASSERT_VALIDTEXTPOS(m_ptSavedCaretPos);
	}
	if (m_bSelectionPushed)
	{
		popPosition(m_ptSavedSelStartPushed, m_ptSavedSelStart);
		ASSERT_VALIDTEXTPOS(m_ptSavedSelStart);
		popPosition(m_ptSavedSelEndPushed, m_ptSavedSelEnd);
		ASSERT_VALIDTEXTPOS(m_ptSavedSelEnd);
	}

	CEPoint ptLastChange;
	if (m_ptLastChangePushed.y == 0 && m_ptLastChangePushed.nToFirstReal > 0)
		ptLastChange = CEPoint(-1,-1);
	else 
	{
		popPosition(m_ptLastChangePushed, ptLastChange);
		ASSERT_VALIDTEXTPOS(ptLastChange);
	}
	m_pGhostTextBuffer->RestoreLastChangePos(ptLastChange);

	// restore the scrolling position
	m_nTopSubLine = m_nTopSubLinePushed;
	if (m_nTopSubLine >= GetSubLineCount())
		m_nTopSubLine = GetSubLineCount() - 1;
	if (m_nTopSubLine < 0)
		m_nTopSubLine = 0;
	int nDummy;
	GetLineBySubLine( m_nTopSubLine, m_nTopLine, nDummy );
	const int nMaxLineLength = GetMaxLineLength(m_nTopLine, GetScreenLines());
	m_nOffsetChar = (m_nOffsetCharPushed < nMaxLineLength) ? m_nOffsetCharPushed : nMaxLineLength;
    RecalcVertScrollBar(true);
    InvalidateHorzScrollBar();
}

void CGhostTextView::PushCursors ()
{
	pushPosition(m_ptCursorPosPushed, m_ptCursorPos);
	pushPosition(m_ptSelStartPushed, m_ptSelStart);
	pushPosition(m_ptSelEndPushed, m_ptSelEnd);
	pushPosition(m_ptAnchorPushed, m_ptAnchor);
	if (m_bDraggingText)
	{
		pushPosition(m_ptDraggedTextBeginPushed, m_ptDraggedTextBegin);
		pushPosition(m_ptDraggedTextEndPushed, m_ptDraggedTextEnd);
	}
	if (m_bDropPosVisible)
	{
		pushPosition(m_ptSavedCaretPosPushed, m_ptSavedCaretPos);
	}
	if (m_bSelectionPushed)
	{
		pushPosition(m_ptSavedSelStartPushed, m_ptSavedSelStart);
		pushPosition(m_ptSavedSelEndPushed, m_ptSavedSelEnd);
	}

	pushPosition(m_ptLastChangePushed, m_pGhostTextBuffer ? m_pGhostTextBuffer->GetLastChangePos() : CEPoint{0, 0});

	// and top line positions
	m_nTopSubLinePushed = m_nTopSubLine;
	m_nOffsetCharPushed = m_nOffsetChar;
}




int CGhostTextView::ComputeRealLine (int nApparentLine) const
{
	if (m_pGhostTextBuffer == nullptr)
		return 0;
	return m_pGhostTextBuffer->ComputeRealLine(nApparentLine);
}

int CGhostTextView::ComputeApparentLine (int nRealLine) const
{
	return m_pGhostTextBuffer->ComputeApparentLine(nRealLine);
}

void CGhostTextView::GetTextWithoutEmptys (int nStartLine, int nStartChar,
		int nEndLine, int nEndChar, CString &text,
		CRLFSTYLE nCrlfStyle /*= CRLFSTYLE::AUTOMATIC*/,
		bool bExcludeInvisibleLines /*= true*/)
{
	if (m_pGhostTextBuffer != nullptr)
	{
		String sText;
		m_pGhostTextBuffer->GetTextWithoutEmptys(nStartLine, nStartChar, nEndLine, nEndChar, sText, nCrlfStyle, bExcludeInvisibleLines);
		text.SetString(sText.c_str(), static_cast<int>(sText.length())); // TODO: Use String instead of CString
	}
	else
		text.Empty();
}

void CGhostTextView::GetTextWithoutEmptysInColumnSelection (CString & text, bool bExcludeInvisibleLines /*= true*/)
{
	if (m_pGhostTextBuffer == nullptr)
	{
		text.Empty();
		return;
	}

	PrepareSelBounds ();

	CString sEol = m_pGhostTextBuffer->GetStringEol (CRLFSTYLE::DOS);

	int nBufSize = 1;
	for (int L = m_ptDrawSelStart.y; L <= m_ptDrawSelEnd.y; L++)
		nBufSize += GetLineLength (L) + sEol.GetLength ();
	tchar_t* pszBuf = text.GetBuffer (nBufSize);

	for (int I = m_ptDrawSelStart.y; I <= m_ptDrawSelEnd.y; I++)
	{
		// exclude ghost lines
		if ((GetLineFlags(I) & LF_GHOST) || (bExcludeInvisibleLines && (GetLineFlags(I) & LF_INVISIBLE)))
			continue;

		int nSelLeft, nSelRight;
		GetColumnSelection (I, nSelLeft, nSelRight);
		memcpy (pszBuf, GetLineChars (I) + nSelLeft, sizeof (tchar_t) * (nSelRight - nSelLeft));
		pszBuf += (nSelRight - nSelLeft);
		memcpy (pszBuf, sEol, sizeof (tchar_t) * sEol.GetLength ());
		pszBuf += sEol.GetLength ();
	}
	pszBuf[0] = 0;
	text.ReleaseBuffer ();
	text.FreeExtra ();
}

HGLOBAL CGhostTextView::PrepareDragData ()
{
	PrepareSelBounds ();
	if (m_ptDrawSelStart == m_ptDrawSelEnd)
		return nullptr;

	CString text;
	GetTextWithoutEmptys (m_ptDrawSelStart.y, m_ptDrawSelStart.x, m_ptDrawSelEnd.y, m_ptDrawSelEnd.x, text);
	int cchText = text.GetLength();
	SIZE_T cbData = (cchText + 1) * sizeof(tchar_t);
	HGLOBAL hData =::GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, cbData);
	if (hData == nullptr)
		return nullptr;

	tchar_t* pszData = (tchar_t*)::GlobalLock (hData);
	if (pszData != nullptr)
		memcpy (pszData, text, cbData);
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
void CGhostTextView::DrawMargin (const CRect & rect, int nLineIndex, int nLineNumber)
{
	int nRealLineNumber;
	if (nLineIndex < 0 || GetLineFlags(nLineIndex) & LF_GHOST)
		nRealLineNumber = -1;
	else
		nRealLineNumber = ComputeRealLine(nLineIndex) + 1;
	CCrystalTextView::DrawMargin(rect, nLineIndex, nRealLineNumber);
}
