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
// ID line follows -- this is updated by SVN
// $Id: ccrystaltextview2.cpp 6457 2009-02-15 14:08:50Z kimmov $

#include "StdAfx.h"
#include "editcmd.h"
#include "ccrystaltextview.h"
#include "ccrystaltextbuffer.h"
#include "SyntaxColors.h"
#include "ccrystaltextmarkers.h"
#include <malloc.h>
#include "string_util.h"

#ifndef __AFXPRIV_H__
#pragma message("Include <afxpriv.h> in your stdafx.h to avoid this message")
#include <afxpriv.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const UINT_PTR CRYSTAL_TIMER_DRAGSEL = 1001;

static LPTSTR NTAPI EnsureCharNext(LPCTSTR current)
{
  LPTSTR next = ::CharNext(current);
  return next > current ? next : next + 1;
}

static LPTSTR NTAPI EnsureCharPrev(LPCTSTR start, LPCTSTR current)
{
  LPTSTR prev = ::CharPrev(start, current);
  return prev < current ? prev : prev - 1;
}

/////////////////////////////////////////////////////////////////////////////
// CCrystalTextView

void CCrystalTextView::
MoveLeft (bool bSelect)
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
MoveRight (bool bSelect)
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
MoveWordLeft (bool bSelect)
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
  while (nPos > 0 && xisspace (pszChars[nPrevPos = (int) (::EnsureCharPrev(pszChars, pszChars + nPos) - pszChars)]))
    nPos = nPrevPos;

  if (nPos > 0)
    {
      nPrevPos = (int) (::CharPrev(pszChars, pszChars + nPos) - pszChars);
      nPos = nPrevPos;
      if (xisalnum (pszChars[nPos]))
        {
          while (nPos > 0 && (xisalnum (pszChars[nPrevPos = (int) (::EnsureCharPrev(pszChars, pszChars + nPos) - pszChars)])))
            nPos = nPrevPos;
        }
      else
        {
          while (nPos > 0 && !xisalnum (pszChars[nPrevPos = (int) (::EnsureCharPrev(pszChars, pszChars + nPos) - pszChars)])
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
MoveWordRight (bool bSelect)
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
        nPos = (int) (::EnsureCharNext(pszChars + nPos) - pszChars);
    }
  else
    {
      while (nPos < nLength && !xisalnum (pszChars[nPos])
            && !xisspace (pszChars[nPos]))
        nPos = (int) (::EnsureCharNext(pszChars + nPos) - pszChars);
    }

  while (nPos < nLength && xisspace (pszChars[nPos]))
    nPos = (int) (::EnsureCharNext(pszChars + nPos) - pszChars);

  m_ptCursorPos.x = nPos;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MoveUp (bool bSelect)
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
MoveDown (bool bSelect)
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
        if (GetLineVisible (m_ptCursorPos.y))
          {
		    do {
			  nSubLine++;
		    } while (IsEmptySubLineIndex(nSubLine));
          }
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
MoveHome (bool bSelect)
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
MoveEnd (bool bSelect)
{
	//BEGIN SW
	CPoint	pos;
	CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, pos );
	m_ptCursorPos.x = SubLineEndToCharPos( m_ptCursorPos.y, pos.y );
	/*ORIGINAL
	m_ptCursorPos.x = GetLineLength(m_ptCursorPos.y);
	*///END SW
  m_nIdealCharPos = INT_MAX;
  EnsureVisible (m_ptCursorPos);
  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MovePgUp (bool bSelect)
{
  // scrolling windows
  int nNewTopSubLine = m_nTopSubLine - GetScreenLines() + 1;
  while (nNewTopSubLine > 0 && IsEmptySubLineIndex(nNewTopSubLine))
    nNewTopSubLine--;
  if (nNewTopSubLine < 0)
    nNewTopSubLine = 0;
  if (m_nTopSubLine != nNewTopSubLine)
    {
      ScrollToSubLine(nNewTopSubLine);
      UpdateSiblingScrollPos(false);
    }

  // setting cursor
  CPoint subLinePos;
  CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, subLinePos );

  int nSubLine = GetSubLineIndex( m_ptCursorPos.y ) + subLinePos.y - GetScreenLines() + 1;

  if (nSubLine < nNewTopSubLine || nSubLine >= nNewTopSubLine + GetScreenLines())
    nSubLine = nNewTopSubLine;

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
MovePgDn (bool bSelect)
{
	//BEGIN SW
	// scrolling windows
	int nNewTopSubLine = m_nTopSubLine + GetScreenLines() - 1;
    while (nNewTopSubLine < GetSubLineCount() && IsEmptySubLineIndex(nNewTopSubLine))
	    nNewTopSubLine++;
	int nSubLineCount = GetSubLineCount();

	if (nNewTopSubLine > nSubLineCount - 1)
		nNewTopSubLine = nSubLineCount - 1;
	if (m_nTopSubLine != nNewTopSubLine)
	{
		ScrollToSubLine(nNewTopSubLine);
        UpdateSiblingScrollPos(false);
	}

	// setting cursor
	CPoint subLinePos;
	CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, subLinePos );

	int nSubLine = GetSubLineIndex( m_ptCursorPos.y ) + subLinePos.y + GetScreenLines() - 1;

	if (nSubLine < nNewTopSubLine || nSubLine >= nNewTopSubLine + GetScreenLines())
		nSubLine = nNewTopSubLine + GetScreenLines() - 1;

	if( nSubLine > nSubLineCount - 1 )
		nSubLine = nSubLineCount - 1;

	SubLineCursorPosToTextPos( 
		CPoint( m_nIdealCharPos, nSubLine ), m_ptCursorPos );

  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);    //todo: no vertical scroll

  UpdateCaret ();
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
}

void CCrystalTextView::
MoveCtrlHome (bool bSelect)
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
MoveCtrlEnd (bool bSelect)
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
      UpdateSiblingScrollPos (false);
    }
}

void CCrystalTextView::
ScrollDown ()
{
  if (m_nTopLine < GetLineCount () - 1)
    {
      ScrollToLine (m_nTopLine + 1);
      UpdateSiblingScrollPos (false);
    }
}

void CCrystalTextView::
ScrollLeft ()
{
  if (m_nOffsetChar > 0)
    {
      ScrollToChar (m_nOffsetChar - 1);
      UpdateCaret ();
      UpdateSiblingScrollPos (true);
    }
}

void CCrystalTextView::
ScrollRight ()
{
  if (m_nOffsetChar < GetMaxLineLength (m_nTopLine, GetScreenLines()) - 1)
    {
      ScrollToChar (m_nOffsetChar + 1);
      UpdateCaret ();
      UpdateSiblingScrollPos (true);
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
      pt.x += (int) (::CharNext (&pszChars[pt.x]) - &pszChars[pt.x]);
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
      nPrevX -= (int) (&pszChars[pt.x] - ::CharPrev (pszChars, &pszChars[pt.x]));
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
  m_ptAnchor = CPoint (0, 0);
  SetSelection (m_ptAnchor, m_ptCursorPos);
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

  bool bShift = (GetKeyState (VK_SHIFT) & 0x8000) != 0;
  bool bControl = (GetKeyState (VK_CONTROL) & 0x8000) != 0;
  bool bAlt = (GetKeyState (VK_MENU) & 0x8000) != 0;

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
          ptStart.x = 0;
          ptStart.y = m_ptAnchor.y;
          const int nSublineIndex = GetSubLineIndex (ptStart.y);
          if (nSublineIndex + pos.y >= nSubLines - 1)
            {
              // Select last line to end of subline
              ptEnd.y = GetLineCount() - 1;
              ptEnd.x = SubLineEndToCharPos (ptEnd.y, pos.y);
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
          m_nDragSelTimer = SetTimer (CRYSTAL_TIMER_DRAGSEL, 100, nullptr);
          ASSERT (m_nDragSelTimer != 0);
          m_bColumnSelection = false;
          m_bWordSelection = false;
          m_bLineSelection = true;
          m_bDragSelection = true;
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
          m_bPreparingToDrag = true;
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

		  m_ptAnchor = ptStart;
          m_ptCursorPos = ptEnd;
          UpdateCaret ();
          EnsureVisible (m_ptCursorPos);
          SetSelection (ptStart, ptEnd);

          SetCapture ();
          m_nDragSelTimer = SetTimer (CRYSTAL_TIMER_DRAGSEL, 100, nullptr);
          ASSERT (m_nDragSelTimer != 0);
          m_bColumnSelection = bAlt;
          m_bWordSelection = bControl;
          m_bLineSelection = false;
          m_bDragSelection = true;
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
      AdjustTextPoint (point);
      CPoint ptNewCursorPos = ClientToText (point);

      CPoint ptStart, ptEnd;
      if (m_bLineSelection)
        {
          if (ptNewCursorPos.y < m_ptAnchor.y ||
                ptNewCursorPos.y == m_ptAnchor.y && ptNewCursorPos.x < m_ptAnchor.x)
            {
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
              m_ptCursorPos = ptNewCursorPos;
            }
          else
            {
              ptEnd = m_ptAnchor;

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
            }
          UpdateCaret ();
          SetSelection (ptNewCursorPos, ptEnd);
          return;
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
      m_bPreparingToDrag = false;
      HGLOBAL hData = PrepareDragData ();
      if (hData != nullptr)
        {
          if (m_pTextBuffer != nullptr)
            m_pTextBuffer->BeginUndoGroup ();

          COleDataSource ds;
          CLIPFORMAT fmt = GetClipTcharTextFormat();      // CF_TEXT or CF_UNICODETEXT
          ds.CacheGlobalData (fmt, hData);
          m_bDraggingText = true;
          DROPEFFECT de = ds.DoDragDrop (GetDropEffect ());
          if (de != DROPEFFECT_NONE)
            OnDropSource (de);
          m_bDraggingText = false;

          if (m_pTextBuffer != nullptr)
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

	  CPoint ptStart;
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
            }
          EnsureVisible (m_ptCursorPos);
          UpdateCaret ();
          SetSelection (ptNewCursorPos, ptEnd);
        }
      else
        {
		  CPoint ptEnd;
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

          m_ptAnchor = ptStart;
          m_ptCursorPos = ptEnd;
          EnsureVisible (m_ptCursorPos);
          UpdateCaret ();
          SetSelection (ptStart, ptEnd);
        }

      m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
      ReleaseCapture ();
      KillTimer (m_nDragSelTimer);
      m_bDragSelection = false;
    }

  if (m_bPreparingToDrag)
    {
      m_bPreparingToDrag = false;

      AdjustTextPoint (point);
      m_ptCursorPos = ClientToText (point);
      EnsureVisible (m_ptCursorPos);
      UpdateCaret ();
      SetSelection (m_ptCursorPos, m_ptCursorPos);
    }

  ASSERT_VALIDTEXTPOS (m_ptCursorPos);
}

void CCrystalTextView::
OnTimer (UINT_PTR nIDEvent)
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

      bool bChanged = false;

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
          UpdateSiblingScrollPos (false);
          bChanged = true;
        }

      //  Scroll horizontally, if necessary
      int nNewOffsetChar = m_nOffsetChar;
      int nMaxLineLength = GetMaxLineLength (m_nTopLine, GetScreenLines());
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
          UpdateSiblingScrollPos (true);
          bChanged = true;
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

	  m_ptAnchor = ptStart;
      m_ptCursorPos = ptEnd;
      UpdateCaret ();
      EnsureVisible (m_ptCursorPos);
      SetSelection (ptStart, ptEnd);
      SetCapture ();
      m_nDragSelTimer = SetTimer (CRYSTAL_TIMER_DRAGSEL, 100, nullptr);
      ASSERT (m_nDragSelTimer != 0);
      m_bColumnSelection = false;
      m_bWordSelection = true;
      m_bLineSelection = false;
      m_bDragSelection = true;
    }
}

void CCrystalTextView::
OnLButtonTrippleClk (UINT nFlags, CPoint point)
{
  if (point.x < GetMarginWidth ())
    return;
  // simulate a click on margin to select whole line
  point.x = 0;
  OnLButtonDown (nFlags, point);
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
OnRButtonDown (UINT nFlags, CPoint point)
{
  CPoint pt = point;
  AdjustTextPoint (pt);
  pt = ClientToText (pt);

  // If there is selection, dont' clear it
  if (!IsSelection() || !IsInsideSelBlock(pt))
    {
      m_ptAnchor = m_ptCursorPos = pt;
      SetSelection (m_ptCursorPos, m_ptCursorPos);
      EnsureVisible (m_ptCursorPos);
      UpdateCaret ();
    }

  CView::OnRButtonDown (nFlags, point);
}

bool CCrystalTextView::
IsSelection ()
{
#if _MSC_VER < 1910		// VS2015 (and earlier?) generates a "performance" warning
  // NOTE:  Comparing two `CPoint` values yields a BOOL result; therefore this funny code
  return (m_ptSelStart != m_ptSelEnd) ? true : false;
#else
  return (m_ptSelStart != m_ptSelEnd);
#endif
}

void CCrystalTextView::
Copy ()
{
  if (m_ptSelStart == m_ptSelEnd)
    return;

  PrepareSelBounds ();
  CString text;
  if (!m_bColumnSelection)
    GetText (m_ptDrawSelStart, m_ptDrawSelEnd, text);
  else
    GetTextInColumnSelection (text);
  PutToClipboard (text, text.GetLength(), m_bColumnSelection);
}


bool CCrystalTextView::
TextInClipboard ()
{
  CLIPFORMAT fmt = GetClipTcharTextFormat();
  return !!IsClipboardFormatAvailable (fmt);
}

bool CCrystalTextView::
PutToClipboard (LPCTSTR pszText, int cchText, bool bColumnSelection)
{
  if (pszText == nullptr || cchText == 0)
    return false;

  CWaitCursor wc;
  bool bOK = false;
  if (OpenClipboard ())
    {
      EmptyClipboard ();
      SIZE_T cbData = (cchText + 1) * sizeof(TCHAR);
      HGLOBAL hData = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, cbData);
      if (hData != nullptr)
        {
          SIZE_T dwSize = GlobalSize(hData);
          LPTSTR pszData = (LPTSTR)::GlobalLock (hData);
          if (pszData != nullptr)
            {
              memcpy (pszData, pszText, cbData);
              if (dwSize > cbData)
                  memset(reinterpret_cast<char *>(pszData) + cbData, 0, dwSize - cbData);
              GlobalUnlock (hData);
              CLIPFORMAT fmt = GetClipTcharTextFormat();
              bOK = SetClipboardData (fmt, hData) != nullptr;
              if (bOK)
                {
                  if (bColumnSelection)
                    SetClipboardData (RegisterClipboardFormat (_T("MSDEVColumnSelect")), nullptr);
                  if (dwSize == cbData)
                    SetClipboardData (RegisterClipboardFormat (_T("WinMergeClipboard")), nullptr);
                }
            }
        }
      CloseClipboard ();
    }
  return bOK;
}

bool CCrystalTextView::
GetFromClipboard (CString & text, bool & bColumnSelection)
{
  bColumnSelection = false;
  bool bSuccess = false;
  if (OpenClipboard ())
    {
      CLIPFORMAT fmt = GetClipTcharTextFormat();
      HGLOBAL hData = GetClipboardData (fmt);
      if (hData != nullptr)
        {
          LPTSTR pszData = (LPTSTR) GlobalLock (hData);
          if (pszData != nullptr)
            {
              UINT cbData = (UINT) GlobalSize (hData);
              // in case we get an odd length for unicodes
              int cchText = ((cbData + sizeof(TCHAR)/sizeof(wchar_t)) / sizeof(TCHAR)) - 1;
              if (cchText >= 0)
                memcpy(text.GetBufferSetLength(cchText), pszData, cbData);
              GlobalUnlock (hData);
              bSuccess = true;
              if (IsClipboardFormatAvailable (RegisterClipboardFormat (_T("MSDEVColumnSelect"))))
                bColumnSelection = true;
              // If in doubt, assume zero-terminated string
              if (!IsClipboardFormatAvailable (RegisterClipboardFormat (_T("WinMergeClipboard"))))
                text.ReleaseBuffer();
            }
        }
      CloseClipboard ();
    }
  return bSuccess;
}
