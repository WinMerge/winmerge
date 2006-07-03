////////////////////////////////////////////////////////////////////////////
//  File:       ccrystaltextview2.cpp
//  Version:    1.0.0.0
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Implementation of the CCrystalTextView class, a part of Crystal Edit -
//  syntax coloring text editor.
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//	21-Feb-99
//		Paul Selormey, James R. Twine:
//	+	FEATURE: description for Undo/Redo actions
//	+	FEATURE: multiple MSVC-like bookmarks
//	+	FEATURE: 'Disable backspace at beginning of line' option
//	+	FEATURE: 'Disable drag-n-drop editing' option
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  19-Jul-99
//      Ferdinand Prantl:
//  +   FEATURE: regular expressions, go to line and things ...
//  +   FEATURE: plenty of syntax highlighting definitions
//  +   FEATURE: corrected bug in syntax highlighting C comments
//  +   FEATURE: extended registry support for saving settings
//  +   FEATURE: some other things I've forgotten ...
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ccrystaltextview2.cpp
 *
 * @brief More functions for CCrystalTextView class.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "editcmd.h"
#include "ccrystaltextview.h"
#include "ccrystaltextbuffer.h"
#include <malloc.h>

#ifndef __AFXPRIV_H__
#pragma message("Include <afxpriv.h> in your stdafx.h to avoid this message")
#include <afxpriv.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CRYSTAL_TIMER_DRAGSEL   1001


/////////////////////////////////////////////////////////////////////////////
// CCrystalTextView

void CCrystalTextView::
MoveLeft (BOOL bSelect)
{
  PrepareSelBounds ();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
    {
      m_ptCursorPos = m_ptDrawSelStart;
    }
  else
    {
      if (m_ptCursorPos.x == 0)
        {
          if (m_ptCursorPos.y > 0)
            {
              m_ptCursorPos.y--;
              m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
            }
        }
      else
        {
          m_ptCursorPos.x--;
          if (m_pTextBuffer->IsMBSTrail (m_ptCursorPos.y, m_ptCursorPos.x) &&
                // here... if its a MBSTrail, then should move one character more....
                m_ptCursorPos.x > 0)
            m_ptCursorPos.x--;
        }
    }
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MoveRight (BOOL bSelect)
{
  PrepareSelBounds ();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
    {
      m_ptCursorPos = m_ptDrawSelEnd;
    }
  else
    {
      if (m_ptCursorPos.x == GetLineLength (m_ptCursorPos.y))
        {
          if (m_ptCursorPos.y < GetLineCount () - 1)
            {
              m_ptCursorPos.y++;
              m_ptCursorPos.x = 0;
            }
        }
      else
        {
          m_ptCursorPos.x++;
          if (m_pTextBuffer->IsMBSTrail (m_ptCursorPos.y, m_ptCursorPos.x) &&
                // here... if its a MBSTrail, then should move one character more....
                m_ptCursorPos.x < GetLineLength (m_ptCursorPos.y))
            m_ptCursorPos.x++;
        }
    }
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MoveWordLeft (BOOL bSelect)
{
  PrepareSelBounds ();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
    {
      MoveLeft (bSelect);
      return;
    }

  if (m_ptCursorPos.x == 0)
    {
      if (m_ptCursorPos.y == 0)
        return;
      m_ptCursorPos.y--;
      m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
    }

  LPCTSTR pszChars = GetLineChars (m_ptCursorPos.y);
  int nPos = m_ptCursorPos.x;
  int nPrevPos;
  while (nPos > 0 && xisspace (pszChars[nPrevPos = ::CharPrev(pszChars, pszChars + nPos) - pszChars]))
    nPos = nPrevPos;

  if (nPos > 0)
    {
      int nPrevPos = ::CharPrev(pszChars, pszChars + nPos) - pszChars;
      nPos = nPrevPos;
      if (xisalnum (pszChars[nPos]))
        {
          while (nPos > 0 && (xisalnum (pszChars[nPrevPos = ::CharPrev(pszChars, pszChars + nPos) - pszChars])))
            nPos = nPrevPos;
        }
      else
        {
          while (nPos > 0 && !xisalnum (pszChars[nPrevPos = ::CharPrev(pszChars, pszChars + nPos) - pszChars])
                && !xisspace (pszChars[nPrevPos]))
            nPos = nPrevPos;
        }
    }

  m_ptCursorPos.x = nPos;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MoveWordRight (BOOL bSelect)
{
  PrepareSelBounds ();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
    {
      MoveRight (bSelect);
      return;
    }

  if (m_ptCursorPos.x == GetLineLength (m_ptCursorPos.y))
    {
      if (m_ptCursorPos.y == GetLineCount () - 1)
        return;
      m_ptCursorPos.y++;
      m_ptCursorPos.x = 0;
    }

  int nLength = GetLineLength (m_ptCursorPos.y);
  if (m_ptCursorPos.x == nLength)
    {
      MoveRight (bSelect);
      return;
    }

  LPCTSTR pszChars = GetLineChars (m_ptCursorPos.y);
  int nPos = m_ptCursorPos.x;
  if (xisalnum (pszChars[nPos]))
    {
      while (nPos < nLength && xisalnum (pszChars[nPos]))
        nPos = ::CharNext(pszChars + nPos) - pszChars;
    }
  else
    {
      while (nPos < nLength && !xisalnum (pszChars[nPos])
            && !xisspace (pszChars[nPos]))
        nPos = ::CharNext(pszChars + nPos) - pszChars;
    }

  while (nPos < nLength && xisspace (pszChars[nPos]))
    nPos = ::CharNext(pszChars + nPos) - pszChars;

  m_ptCursorPos.x = nPos;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MoveUp (BOOL bSelect)
{
  PrepareSelBounds ();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
    m_ptCursorPos = m_ptDrawSelStart;

	//BEGIN SW
	CPoint	subLinePos;
	CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, subLinePos );

	int			nSubLine = GetSubLineIndex( m_ptCursorPos.y ) + subLinePos.y;

	if( nSubLine > 0 )
	/*ORIGINAL
	if (m_ptCursorPos.y > 0)
	*///END SW
    {
      if (m_nIdealCharPos == -1)
        m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
		//BEGIN SW
		do {
			nSubLine--;
		} while (IsEmptySubLineIndex(nSubLine));
		SubLineCursorPosToTextPos( CPoint( m_nIdealCharPos, nSubLine ), m_ptCursorPos );
		/*ORIGINAL
		m_ptCursorPos.y --;
		m_ptCursorPos.x = ApproxActualOffset(m_ptCursorPos.y, m_nIdealCharPos);
		*///END SW
      if (m_ptCursorPos.x > GetLineLength (m_ptCursorPos.y))
        m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
    }
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MoveDown (BOOL bSelect)
{
  PrepareSelBounds ();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
    m_ptCursorPos = m_ptDrawSelEnd;

	//BEGIN SW
	CPoint	subLinePos;
	CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, subLinePos );

	int			nSubLine = GetSubLineIndex( m_ptCursorPos.y ) + subLinePos.y;

	if( nSubLine < GetSubLineCount() - 1 )
	/*ORIGINAL
	if (m_ptCursorPos.y < GetLineCount() - 1)
	*/
    {
      if (m_nIdealCharPos == -1)
        m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
		//BEGIN SW
		do {
			nSubLine++;
		} while (IsEmptySubLineIndex(nSubLine));
		SubLineCursorPosToTextPos( CPoint( m_nIdealCharPos, nSubLine ), m_ptCursorPos );
		/*ORIGINAL
		m_ptCursorPos.y ++;
		m_ptCursorPos.x = ApproxActualOffset(m_ptCursorPos.y, m_nIdealCharPos);
		*///END SW
      if (m_ptCursorPos.x > GetLineLength (m_ptCursorPos.y))
        m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
    }
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MoveHome (BOOL bSelect)
{
  int nLength = GetLineLength (m_ptCursorPos.y);
  LPCTSTR pszChars = GetLineChars (m_ptCursorPos.y);
	//BEGIN SW
	CPoint	pos;
	CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, pos );
	int nHomePos = SubLineHomeToCharPos( m_ptCursorPos.y, pos.y );
	int nOriginalHomePos = nHomePos;
	/*ORIGINAL
	int nHomePos = 0;
	*///END SW
  while (nHomePos < nLength && xisspace (pszChars[nHomePos]))
    nHomePos++;
  if (nHomePos == nLength || m_ptCursorPos.x == nHomePos)
		//BEGIN SW
		m_ptCursorPos.x = nOriginalHomePos;
		/*ORIGINAL
		m_ptCursorPos.x = 0;
		*///END SW
  else
    m_ptCursorPos.x = nHomePos;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MoveEnd (BOOL bSelect)
{
	//BEGIN SW
	CPoint	pos;
	CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, pos );
	m_ptCursorPos.x = SubLineEndToCharPos( m_ptCursorPos.y, pos.y );
	/*ORIGINAL
	m_ptCursorPos.x = GetLineLength(m_ptCursorPos.y);
	*///END SW
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MovePgUp (BOOL bSelect)
{
  // scrolling windows
  int nNewTopSubLine = m_nTopSubLine - GetScreenLines() + 1;
  if (nNewTopSubLine < 0)
    nNewTopSubLine = 0;
  if (m_nTopSubLine != nNewTopSubLine)
    {
      int nDummy;
      int nNewTopLine;
      GetLineBySubLine(nNewTopSubLine, nNewTopLine, nDummy);
      m_ptCursorPos.y = nNewTopLine;
      ScrollToSubLine(nNewTopSubLine);
      UpdateSiblingScrollPos(FALSE);
    }

  // setting cursor
  CPoint subLinePos;
  CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, subLinePos );

  int nSubLine = GetSubLineIndex( m_ptCursorPos.y ) + subLinePos.y;

  if ( nSubLine < 0 )
    nSubLine = 0;

  SubLineCursorPosToTextPos( CPoint( m_nIdealCharPos, nSubLine ),
    m_ptCursorPos );

  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);    //todo: no vertical scroll

  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MovePgDn (BOOL bSelect)
{
	//BEGIN SW
	// scrolling windows
	int nNewTopSubLine = m_nTopSubLine + GetScreenLines() - 1;
	int nSubLineCount = GetSubLineCount();

	if (nNewTopSubLine > nSubLineCount - 1)
		nNewTopSubLine = nSubLineCount - 1;
	if (m_nTopSubLine != nNewTopSubLine)
	{
		int nDummy;
		int nNewTopLine;
		GetLineBySubLine(nNewTopSubLine, nNewTopLine, nDummy);
		m_ptCursorPos.y = nNewTopLine;
		ScrollToSubLine(nNewTopSubLine);
        UpdateSiblingScrollPos(FALSE);
	}

	// setting cursor
	CPoint subLinePos;
	CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, subLinePos );

	int nSubLine = GetSubLineIndex( m_ptCursorPos.y ) + subLinePos.y + GetScreenLines() - 1;

	if( nSubLine > nSubLineCount - 1 )
		nSubLine = nSubLineCount - 1;

	SubLineCursorPosToTextPos( 
		CPoint( m_nIdealCharPos, nSubLine ), m_ptCursorPos );

	/*ORIGINAL
	int nNewTopLine = m_nTopLine + GetScreenLines() - 1;
	if (nNewTopLine >= GetLineCount())
		nNewTopLine = GetLineCount() - 1;
	if (m_nTopLine != nNewTopLine)
	{
		ScrollToLine(nNewTopLine);
		UpdateSiblingScrollPos(TRUE);
	}

	m_ptCursorPos.y += GetScreenLines() - 1;
	if (m_ptCursorPos.y >= GetLineCount())
		m_ptCursorPos.y = GetLineCount() - 1;
	if (m_ptCursorPos.x > GetLineLength(m_ptCursorPos.y))
		m_ptCursorPos.x = GetLineLength(m_ptCursorPos.y);
	*///END SW
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);    //todo: no vertical scroll

  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MoveCtrlHome (BOOL bSelect)
{
  m_ptCursorPos.x = 0;
  m_ptCursorPos.y = 0;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MoveCtrlEnd (BOOL bSelect)
{
  m_ptCursorPos.y = GetLineCount () - 1;
  m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
ScrollUp ()
{
  if (m_nTopLine > 0)
    {
      ScrollToLine (m_nTopLine - 1);
      UpdateSiblingScrollPos (FALSE);
    }
}

void CCrystalTextView::
ScrollDown ()
{
  if (m_nTopLine < GetLineCount () - 1)
    {
      ScrollToLine (m_nTopLine + 1);
      UpdateSiblingScrollPos (FALSE);
    }
}

void CCrystalTextView::
ScrollLeft ()
{
  if (m_nOffsetChar > 0)
    {
      ScrollToChar (m_nOffsetChar - 1);
      UpdateCaret ();
      UpdateSiblingScrollPos (TRUE);
    }
}

void CCrystalTextView::
ScrollRight ()
{
  if (m_nOffsetChar < GetMaxLineLength () - 1)
    {
      ScrollToChar (m_nOffsetChar + 1);
      UpdateCaret ();
      UpdateSiblingScrollPos (TRUE);
    }
}

CPoint CCrystalTextView::
WordToRight (CPoint pt)
{
  ASSERT_VALIDTEXTPOS (pt);
  int nLength = GetLineLength (pt.y);
  LPCTSTR pszChars = GetLineChars (pt.y);
  while (pt.x < nLength)
    {
      if (!xisalnum (pszChars[pt.x]))
        break;
      pt.x += ::CharNext (&pszChars[pt.x]) - &pszChars[pt.x];
    }
  ASSERT_VALIDTEXTPOS (pt);
  return pt;
}

CPoint CCrystalTextView::
WordToLeft (CPoint pt)
{
  ASSERT_VALIDTEXTPOS (pt);
  LPCTSTR pszChars = GetLineChars (pt.y);
  int nPrevX = pt.x;
  while (pt.x > 0)
    {
      nPrevX -= &pszChars[pt.x] - ::CharPrev (pszChars, &pszChars[pt.x]);
      if (!xisalnum (pszChars[nPrevX]))
        break;
      pt.x = nPrevX;
    }
  ASSERT_VALIDTEXTPOS (pt);
  return pt;
}

void CCrystalTextView::
SelectAll ()
{
  int nLineCount = GetLineCount ();
  m_ptCursorPos.x = GetLineLength (nLineCount - 1);
  m_ptCursorPos.y = nLineCount - 1;
  SetSelection (CPoint (0, 0), m_ptCursorPos);
  UpdateCaret ();
}

/** 
 * @brief Called when left mousebutton pressed down in editor.
 * This function handles left mousebutton down in editor.
 * @param [in] nFlags Flags indicating if virtual keys are pressed.
 * @param [in] point Point where mousebutton is pressed.
 */
void CCrystalTextView::
OnLButtonDown (UINT nFlags, CPoint point)
{
  CView::OnLButtonDown (nFlags, point);

  BOOL bShift = GetKeyState (VK_SHIFT) & 0x8000;
  BOOL bControl = GetKeyState (VK_CONTROL) & 0x8000;

  if (point.x < GetMarginWidth ())
    {
      AdjustTextPoint (point);
      if (bControl)
        {
          SelectAll ();
        }
      else
        {
          m_ptCursorPos = ClientToText (point);
          const int nSubLines = GetSubLineCount();

          // Find char pos that is the beginning of the subline clicked on
          CPoint pos;
          CharPosToPoint (m_ptCursorPos.y, m_ptCursorPos.x, pos);
          m_ptCursorPos.x = SubLineHomeToCharPos (m_ptCursorPos.y, pos.y);

          if (!bShift)
            m_ptAnchor = m_ptCursorPos;

          CPoint ptStart, ptEnd;
          CharPosToPoint (m_ptAnchor.y, m_ptAnchor.x, pos);
          ptStart.y = m_ptAnchor.y;
          const int nSublineIndex = GetSubLineIndex (ptStart.y);
          if (nSublineIndex + pos.y >= nSubLines - 1)
            {
              // Select last line to end of subline
              ptEnd.y = GetLineCount() - 1;
              ptEnd.x = SubLineEndToCharPos (ptStart.y, pos.y);
            }
          else
            {
              int nLine, nSubLine;
              GetLineBySubLine (nSublineIndex + pos.y + 1, nLine, nSubLine);
              ptEnd.y = nLine;
              ptEnd.x = SubLineHomeToCharPos (nLine, nSubLine);
            }

          m_ptCursorPos = ptEnd;
          UpdateCaret ();
          EnsureVisible (m_ptCursorPos);
          SetSelection (ptStart, ptEnd);

          SetCapture ();
          m_nDragSelTimer = SetTimer (CRYSTAL_TIMER_DRAGSEL, 100, NULL);
          ASSERT (m_nDragSelTimer != 0);
          m_bWordSelection = FALSE;
          m_bLineSelection = TRUE;
          m_bDragSelection = TRUE;
        }
    }
  else
    {
      CPoint ptText = ClientToText (point);
      PrepareSelBounds ();
      //  [JRT]:  Support For Disabling Drag and Drop...
      if ((IsInsideSelBlock (ptText)) &&    // If Inside Selection Area
            (!m_bDisableDragAndDrop))    // And D&D Not Disabled
        {
          m_bPreparingToDrag = TRUE;
        }
      else
        {
          AdjustTextPoint (point);
          m_ptCursorPos = ClientToText (point);
          if (!bShift)
            m_ptAnchor = m_ptCursorPos;

          CPoint ptStart, ptEnd;
          if (bControl)
            {
              if (m_ptCursorPos.y < m_ptAnchor.y ||
                    m_ptCursorPos.y == m_ptAnchor.y && m_ptCursorPos.x < m_ptAnchor.x)
                {
                  ptStart = WordToLeft (m_ptCursorPos);
                  ptEnd = WordToRight (m_ptAnchor);
                }
              else
                {
                  ptStart = WordToLeft (m_ptAnchor);
                  ptEnd = WordToRight (m_ptCursorPos);
                }
            }
          else
            {
              ptStart = m_ptAnchor;
              ptEnd = m_ptCursorPos;
            }

          m_ptCursorPos = ptEnd;
          UpdateCaret ();
          EnsureVisible (m_ptCursorPos);
          SetSelection (ptStart, ptEnd);

          SetCapture ();
          m_nDragSelTimer = SetTimer (CRYSTAL_TIMER_DRAGSEL, 100, NULL);
          ASSERT (m_nDragSelTimer != 0);
          m_bWordSelection = bControl;
          m_bLineSelection = FALSE;
          m_bDragSelection = TRUE;
        }
    }

  ASSERT_VALIDTEXTPOS (m_ptCursorPos);
  // we must set the ideal character position here!
  m_nIdealCharPos = CalculateActualOffset( m_ptCursorPos.y, m_ptCursorPos.x );
}

void CCrystalTextView::
OnMouseMove (UINT nFlags, CPoint point)
{
  CView::OnMouseMove (nFlags, point);

  if (m_bDragSelection)
    {
      BOOL bOnMargin = point.x < GetMarginWidth ();

      AdjustTextPoint (point);
      CPoint ptNewCursorPos = ClientToText (point);

      CPoint ptStart, ptEnd;
      if (m_bLineSelection)
        {
          if (bOnMargin)
            {
              if (ptNewCursorPos.y < m_ptAnchor.y ||
                    ptNewCursorPos.y == m_ptAnchor.y && ptNewCursorPos.x < m_ptAnchor.x)
                {
					//BEGIN SW
					CPoint	pos;
					ptEnd = m_ptAnchor;
					CharPosToPoint( ptEnd.y, ptEnd.x, pos );
					if( GetSubLineIndex( ptEnd.y ) + pos.y == GetSubLineCount() - 1 )
						ptEnd = SubLineEndToCharPos( ptEnd.y, pos.y );
					else
					{
						int	nLine, nSubLine;
						GetLineBySubLine( GetSubLineIndex( ptEnd.y ) + pos.y + 1, nLine, nSubLine );
						ptEnd.y = nLine;
						ptEnd.x = SubLineHomeToCharPos( nLine, nSubLine );
					}
					CharPosToPoint( ptNewCursorPos.y, ptNewCursorPos.x, pos );
					ptNewCursorPos.x = SubLineHomeToCharPos( ptNewCursorPos.y, pos.y );
					/*ORIGINAL
					ptEnd = m_ptAnchor;
					if (ptEnd.y == GetLineCount() - 1)
					{
						ptEnd.x = GetLineLength(ptEnd.y);
					}
					else
					{
						ptEnd.y ++;
						ptEnd.x = 0;
					}
					ptNewCursorPos.x = 0;
					*///END SW
                  m_ptCursorPos = ptNewCursorPos;
                }
              else
                {
                  ptEnd = m_ptAnchor;
					//BEGIN SW

					CPoint	pos;
					CharPosToPoint( ptEnd.y, ptEnd.x, pos );
					ptEnd.x = SubLineHomeToCharPos( ptEnd.y, pos.y );

					m_ptCursorPos = ptNewCursorPos;
					CharPosToPoint( ptNewCursorPos.y, ptNewCursorPos.x, pos );
					if( GetSubLineIndex( ptNewCursorPos.y ) + pos.y == GetSubLineCount() - 1 )
						ptNewCursorPos.x = SubLineEndToCharPos( ptNewCursorPos.y, pos.y );
					else
					{
						int	nLine, nSubLine;
						GetLineBySubLine( GetSubLineIndex( ptNewCursorPos.y ) + pos.y + 1, nLine, nSubLine );
						ptNewCursorPos.y = nLine;
						ptNewCursorPos.x = SubLineHomeToCharPos( nLine, nSubLine );
					}

					int nLine, nSubLine;
					GetLineBySubLine( GetSubLineIndex( m_ptCursorPos.y ) + pos.y, nLine, nSubLine );
					m_ptCursorPos.y = nLine;
					m_ptCursorPos.x = SubLineHomeToCharPos( nLine, nSubLine );
					/*ORIGINAL
					ptEnd.x = 0;
					m_ptCursorPos = ptNewCursorPos;
					if (ptNewCursorPos.y == GetLineCount() - 1)
					{
						ptNewCursorPos.x = GetLineLength(ptNewCursorPos.y);
					}
					else
					{
						ptNewCursorPos.y ++;
						ptNewCursorPos.x = 0;
					}
					m_ptCursorPos.x = 0;
					*///END SW
                }
              UpdateCaret ();
              SetSelection (ptNewCursorPos, ptEnd);
              return;
            }

          //  Moving to normal selection mode
          ::SetCursor (::LoadCursor (NULL, MAKEINTRESOURCE (IDC_IBEAM)));
          m_bLineSelection = m_bWordSelection = FALSE;
        }

      if (m_bWordSelection)
        {
          if (ptNewCursorPos.y < m_ptAnchor.y ||
                ptNewCursorPos.y == m_ptAnchor.y && ptNewCursorPos.x < m_ptAnchor.x)
            {
              ptStart = WordToLeft (ptNewCursorPos);
              ptEnd = WordToRight (m_ptAnchor);
            }
          else
            {
              ptStart = WordToLeft (m_ptAnchor);
              ptEnd = WordToRight (ptNewCursorPos);
            }
        }
      else
        {
          ptStart = m_ptAnchor;
          ptEnd = ptNewCursorPos;
        }

      m_ptCursorPos = ptEnd;
      UpdateCaret ();
      SetSelection (ptStart, ptEnd);
    }

  if (m_bPreparingToDrag)
    {
      m_bPreparingToDrag = FALSE;
      HGLOBAL hData = PrepareDragData ();
      if (hData != NULL)
        {
          if (m_pTextBuffer != NULL)
            m_pTextBuffer->BeginUndoGroup ();

          COleDataSource ds;
          UINT fmt = GetClipTcharTextFormat();      // CF_TEXT or CF_UNICODETEXT
          ds.CacheGlobalData (fmt, hData);
          m_bDraggingText = TRUE;
          DROPEFFECT de = ds.DoDragDrop (GetDropEffect ());
          if (de != DROPEFFECT_NONE)
            OnDropSource (de);
          m_bDraggingText = FALSE;

          if (m_pTextBuffer != NULL)
            m_pTextBuffer->FlushUndoGroup (this);
        }
    }

  ASSERT_VALIDTEXTPOS (m_ptCursorPos);
}

void CCrystalTextView::
OnLButtonUp (UINT nFlags, CPoint point)
{
  CView::OnLButtonUp (nFlags, point);

  if (m_bDragSelection)
    {
      AdjustTextPoint (point);
      CPoint ptNewCursorPos = ClientToText (point);

      CPoint ptStart, ptEnd;
      if (m_bLineSelection)
        {
          CPoint ptEnd;
          if (ptNewCursorPos.y < m_ptAnchor.y ||
                ptNewCursorPos.y == m_ptAnchor.y && ptNewCursorPos.x < m_ptAnchor.x)
            {
				//BEGIN SW
				CPoint	pos;
				ptEnd = m_ptAnchor;
				CharPosToPoint( ptEnd.y, ptEnd.x, pos );
				if( GetSubLineIndex( ptEnd.y ) + pos.y == GetSubLineCount() - 1 )
					ptEnd = SubLineEndToCharPos( ptEnd.y, pos.y );
				else
				{
					int	nLine, nSubLine;
					GetLineBySubLine( GetSubLineIndex( ptEnd.y ) + pos.y + 1, nLine, nSubLine );
					ptEnd.y = nLine;
					ptEnd.x = SubLineHomeToCharPos( nLine, nSubLine );
				}
				CharPosToPoint( ptNewCursorPos.y, ptNewCursorPos.x, pos );
				ptNewCursorPos.x = SubLineHomeToCharPos( ptNewCursorPos.y, pos.y );
				/*ORIGINAL
				ptEnd = m_ptAnchor;
				if (ptEnd.y == GetLineCount() - 1)
				{
					ptEnd.x = GetLineLength(ptEnd.y);
				}
				else
				{
					ptEnd.y ++;
					ptEnd.x = 0;
				}
				ptNewCursorPos.x = 0;
				*///END SW
              m_ptCursorPos = ptNewCursorPos;
            }
          else
            {
              ptEnd = m_ptAnchor;
				//BEGIN SW

				CPoint	pos;
				CharPosToPoint( ptEnd.y, ptEnd.x, pos );
				ptEnd.x = SubLineHomeToCharPos( ptEnd.y, pos.y );

				m_ptCursorPos = ptNewCursorPos;
				CharPosToPoint( ptNewCursorPos.y, ptNewCursorPos.x, pos );
				if( GetSubLineIndex( ptNewCursorPos.y ) + pos.y == GetSubLineCount() - 1 )
					ptNewCursorPos.x = SubLineEndToCharPos( ptNewCursorPos.y, pos.y );
				else
				{
					int	nLine, nSubLine;
					GetLineBySubLine( GetSubLineIndex( ptNewCursorPos.y ) + pos.y + 1, nLine, nSubLine );
					ptNewCursorPos.y = nLine;
					ptNewCursorPos.x = SubLineHomeToCharPos( nLine, nSubLine );
				}
				m_ptCursorPos = ptNewCursorPos;
				/*ORIGINAL
				ptEnd.x = 0;
				m_ptCursorPos = ptNewCursorPos;
				if (ptNewCursorPos.y == GetLineCount() - 1)
				{
					ptNewCursorPos.x = GetLineLength(ptNewCursorPos.y);
				}
				else
				{
					ptNewCursorPos.y ++;
					ptNewCursorPos.x = 0;
				}
              m_ptCursorPos = ptNewCursorPos;
				*///END SW
            }
          EnsureVisible (m_ptCursorPos);
          UpdateCaret ();
          SetSelection (ptNewCursorPos, ptEnd);
        }
      else
        {
          if (m_bWordSelection)
            {
              if (ptNewCursorPos.y < m_ptAnchor.y ||
                    ptNewCursorPos.y == m_ptAnchor.y && ptNewCursorPos.x < m_ptAnchor.x)
                {
                  ptStart = WordToLeft (ptNewCursorPos);
                  ptEnd = WordToRight (m_ptAnchor);
                }
              else
                {
                  ptStart = WordToLeft (m_ptAnchor);
                  ptEnd = WordToRight (ptNewCursorPos);
                }
            }
          else
            {
              ptStart = m_ptAnchor;
              ptEnd = m_ptCursorPos;
            }

          m_ptCursorPos = ptEnd;
          EnsureVisible (m_ptCursorPos);
          UpdateCaret ();
          SetSelection (ptStart, ptEnd);
        }

      ReleaseCapture ();
      KillTimer (m_nDragSelTimer);
      m_bDragSelection = FALSE;
    }

  if (m_bPreparingToDrag)
    {
      m_bPreparingToDrag = FALSE;

      AdjustTextPoint (point);
      m_ptCursorPos = ClientToText (point);
      EnsureVisible (m_ptCursorPos);
      UpdateCaret ();
      SetSelection (m_ptCursorPos, m_ptCursorPos);
    }

  ASSERT_VALIDTEXTPOS (m_ptCursorPos);
}

void CCrystalTextView::
OnTimer (UINT nIDEvent)
{
  CView::OnTimer (nIDEvent);

  if (nIDEvent == CRYSTAL_TIMER_DRAGSEL)
    {
      ASSERT (m_bDragSelection);
      CPoint pt;
      ::GetCursorPos (&pt);
      ScreenToClient (&pt);
      CRect rcClient;
      GetClientRect (&rcClient);

      BOOL bChanged = FALSE;

      //  Scroll vertically, if necessary
      int nNewTopLine = m_nTopLine;
      int nLineCount = GetLineCount ();
      if (pt.y < rcClient.top)
        {
          nNewTopLine--;
          if (pt.y < rcClient.top - GetLineHeight ())
            nNewTopLine -= 2;
        }
      else if (pt.y >= rcClient.bottom)
        {
          nNewTopLine++;
          if (pt.y >= rcClient.bottom + GetLineHeight ())
            nNewTopLine += 2;
        }

      if (nNewTopLine < 0)
        nNewTopLine = 0;
      if (nNewTopLine >= nLineCount)
        nNewTopLine = nLineCount - 1;

      if (m_nTopLine != nNewTopLine)
        {
          ScrollToLine (nNewTopLine);
          UpdateSiblingScrollPos (FALSE);
          bChanged = TRUE;
        }

      //  Scroll horizontally, if necessary
      int nNewOffsetChar = m_nOffsetChar;
      int nMaxLineLength = GetMaxLineLength ();
      if (pt.x < rcClient.left)
        nNewOffsetChar--;
      else if (pt.x >= rcClient.right)
        nNewOffsetChar++;

      if (nNewOffsetChar >= nMaxLineLength)
        nNewOffsetChar = nMaxLineLength - 1;
      if (nNewOffsetChar < 0)
        nNewOffsetChar = 0;

      if (m_nOffsetChar != nNewOffsetChar)
        {
          ScrollToChar (nNewOffsetChar);
          UpdateCaret ();
          UpdateSiblingScrollPos (TRUE);
          bChanged = TRUE;
        }

      //  Fix changes
      if (bChanged)
        {
          AdjustTextPoint (pt);
          CPoint ptNewCursorPos = ClientToText (pt);
          if (ptNewCursorPos != m_ptCursorPos)
            {
              m_ptCursorPos = ptNewCursorPos;
              UpdateCaret ();
            }
          SetSelection (m_ptAnchor, m_ptCursorPos);
        }
    }
}

/** 
 * @brief Called when mouse is double-clicked in editor.
 * This function handles mouse double-click in editor. There are many things
 * we can do, depending on where mouse is double-clicked etc:
 * - in selection area toggles bookmark
 * - in editor selects word below cursor
 * @param [in] nFlags Flags indicating if virtual keys are pressed.
 * @param [in] point Point where mouse is double-clicked.
 */
void CCrystalTextView::
OnLButtonDblClk (UINT nFlags, CPoint point)
{
  CView::OnLButtonDblClk (nFlags, point);

  if (point.x < GetMarginWidth ())
    {
      AdjustTextPoint (point);
      CPoint ptCursorPos = ClientToText (point);
      ToggleBookmark(ptCursorPos.y);
      return;
    }

  if (!m_bDragSelection)
    {
      AdjustTextPoint (point);

      m_ptCursorPos = ClientToText (point);
      m_ptAnchor = m_ptCursorPos;

      CPoint ptStart, ptEnd;
      if (m_ptCursorPos.y < m_ptAnchor.y ||
            m_ptCursorPos.y == m_ptAnchor.y && m_ptCursorPos.x < m_ptAnchor.x)
        {
          ptStart = WordToLeft (m_ptCursorPos);
          ptEnd = WordToRight (m_ptAnchor);
        }
      else
        {
          ptStart = WordToLeft (m_ptAnchor);
          ptEnd = WordToRight (m_ptCursorPos);
        }

      m_ptCursorPos = ptEnd;
      UpdateCaret ();
      EnsureVisible (m_ptCursorPos);
      SetSelection (ptStart, ptEnd);

      SetCapture ();
      m_nDragSelTimer = SetTimer (CRYSTAL_TIMER_DRAGSEL, 100, NULL);
      ASSERT (m_nDragSelTimer != 0);
      m_bWordSelection = TRUE;
      m_bLineSelection = FALSE;
      m_bDragSelection = TRUE;
    }
}

void CCrystalTextView::
OnEditCopy ()
{
  Copy ();
}

void CCrystalTextView::
OnUpdateEditCopy (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (m_ptSelStart != m_ptSelEnd);
}

void CCrystalTextView::
OnEditSelectAll ()
{
  SelectAll ();
}

void CCrystalTextView::
OnUpdateEditSelectAll (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (TRUE);
}

void CCrystalTextView::
OnRButtonDown (UINT nFlags, CPoint point)
{
  CPoint pt = point;
  AdjustTextPoint (pt);
  pt = ClientToText (pt);
  if (!IsInsideSelBlock (pt))
    {
      m_ptAnchor = m_ptCursorPos = pt;
      SetSelection (m_ptCursorPos, m_ptCursorPos);
      EnsureVisible (m_ptCursorPos);
      UpdateCaret ();
    }

  CView::OnRButtonDown (nFlags, point);
}

BOOL CCrystalTextView::
IsSelection ()
{
  return m_ptSelStart != m_ptSelEnd;
}

void CCrystalTextView::
Copy ()
{
  if (m_ptSelStart == m_ptSelEnd)
    return;

  PrepareSelBounds ();
  CString text;
  GetText (m_ptDrawSelStart, m_ptDrawSelEnd, text);
  PutToClipboard (text);
}


BOOL CCrystalTextView::
TextInClipboard ()
{
  UINT fmt = GetClipTcharTextFormat();
  return IsClipboardFormatAvailable (fmt);
}

BOOL CCrystalTextView::
PutToClipboard (LPCTSTR pszText)
{
  if (pszText == NULL || _tcslen (pszText) == 0)
    return FALSE;

  CWaitCursor wc;
  BOOL bOK = FALSE;
  if (OpenClipboard ())
    {
      EmptyClipboard ();
      HGLOBAL hData = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, (_tcslen(pszText)+1) * sizeof(TCHAR));
      if (hData != NULL)
        {
          LPTSTR pszData = (LPTSTR)::GlobalLock (hData);
          _tcscpy (pszData, pszText);
          GlobalUnlock (hData);
          UINT fmt = GetClipTcharTextFormat();
          bOK = SetClipboardData (fmt, hData) != NULL;
        }
      CloseClipboard ();
    }
  return bOK;
}

BOOL CCrystalTextView::
GetFromClipboard (CString & text)
{
  BOOL bSuccess = FALSE;
  if (OpenClipboard ())
    {
      UINT fmt = GetClipTcharTextFormat();
      HGLOBAL hData = GetClipboardData (fmt);
      if (hData != NULL)
        {
          LPTSTR pszData = (LPTSTR) GlobalLock (hData);
          if (pszData != NULL)
            {
              text = pszData;
              GlobalUnlock (hData);
              bSuccess = TRUE;
            }
        }
      CloseClipboard ();
    }
  return bSuccess;
}
