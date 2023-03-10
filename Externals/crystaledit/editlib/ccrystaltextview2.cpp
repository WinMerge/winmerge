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
//  21-Feb-99
//      Paul Selormey, James R. Twine:
//  +   FEATURE: description for Undo/Redo actions
//  +   FEATURE: multiple MSVC-like bookmarks
//  +   FEATURE: 'Disable backspace at beginning of line' option
//  +   FEATURE: 'Disable drag-n-drop editing' option
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

#include "StdAfx.h"
#include "ccrystaltextview.h"
#include "ccrystaltextbuffer.h"
#include "ccrystaltextmarkers.h"
#include "editcmd.h"
#include "SyntaxColors.h"
#include <malloc.h>
#include "utils/string_util.h"
#include "utils/icu.hpp"

#ifndef __AFXPRIV_H__
#pragma message("Include <afxpriv.h> in your stdafx.h to avoid this message")
#include <afxpriv.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const UINT_PTR CRYSTAL_TIMER_DRAGSEL = 1001;
static const UINT_PTR CRYSTAL_RECALC_VSCROLLBAR = 1002;
static const UINT_PTR CRYSTAL_RECALC_HSCROLLBAR = 1003;

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
          auto pIterChar = ICUBreakIterator::getCharacterBreakIterator(GetLineChars(m_ptCursorPos.y), GetLineLength(m_ptCursorPos.y));
          m_ptCursorPos.x = pIterChar->preceding(m_ptCursorPos.x);
        }
    }
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
}

void CCrystalTextView::
MoveRight (bool bSelect)
{
  int nLineLength = GetLineLength(m_ptCursorPos.y);
  PrepareSelBounds ();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
    {
      m_ptCursorPos = m_ptDrawSelEnd;
    }
  else
    {
      if (m_ptCursorPos.x == nLineLength)
        {
          if (m_ptCursorPos.y < GetLineCount () - 1)
            {
              m_ptCursorPos.y++;
              m_ptCursorPos.x = 0;
            }
        }
      else
        {
          auto pIterChar = ICUBreakIterator::getCharacterBreakIterator(GetLineChars(m_ptCursorPos.y), nLineLength);
          m_ptCursorPos.x = pIterChar->following(m_ptCursorPos.x);
        }
    }
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
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
      MoveLeft (bSelect);
      return;
    }

  if (m_ptCursorPos.x > 0)
    {
      const tchar_t *pszChars = GetLineChars(m_ptCursorPos.y);
      auto pIterWord = ICUBreakIterator::getWordBreakIterator(reinterpret_cast<const UChar *>(pszChars), GetLineLength(m_ptCursorPos.y));
      int nPos = pIterWord->preceding(m_ptCursorPos.x);
      if (xisspace(pszChars[nPos]))
        nPos = pIterWord->preceding(nPos);
      m_ptCursorPos.x = nPos;
    }

  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
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

  int nLength = GetLineLength (m_ptCursorPos.y);
  if (m_ptCursorPos.x == nLength)
    {
      MoveRight (bSelect);
      return;
    }

  const tchar_t *pszChars = GetLineChars(m_ptCursorPos.y);
  auto pIterWord = ICUBreakIterator::getWordBreakIterator(reinterpret_cast<const UChar *>(pszChars), nLength);
  int nPos = pIterWord->following(m_ptCursorPos.x);
  while (nPos < nLength && xisspace(pszChars[nPos]))
    ++nPos;
  m_ptCursorPos.x = nPos;

  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
}

void CCrystalTextView::
MoveUp (bool bSelect)
{
  PrepareSelBounds ();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
    m_ptCursorPos = m_ptDrawSelStart;

  //BEGIN SW
  CEPoint  subLinePos;
  CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, subLinePos );

  int         nSubLine = GetSubLineIndex( m_ptCursorPos.y ) + subLinePos.y;

  if( nSubLine > 0 )
    /*ORIGINAL
    if (m_ptCursorPos.y > 0)
    *///END SW
    {
      if (m_nIdealCharPos == -1)
        m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
      //BEGIN SW
      do
        {
          nSubLine--;
        }
      while (IsEmptySubLineIndex(nSubLine));
      SubLineCursorPosToTextPos( CEPoint( m_nIdealCharPos, nSubLine ), m_ptCursorPos );
      /*ORIGINAL
      m_ptCursorPos.y --;
      m_ptCursorPos.x = ApproxActualOffset(m_ptCursorPos.y, m_nIdealCharPos);
      *///END SW
      if (m_ptCursorPos.x > GetLineLength (m_ptCursorPos.y))
        m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
    }
  EnsureVisible (m_ptCursorPos);
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
}

void CCrystalTextView::
MoveDown (bool bSelect)
{
  PrepareSelBounds ();
  if (m_ptDrawSelStart != m_ptDrawSelEnd && !bSelect)
    m_ptCursorPos = m_ptDrawSelEnd;

  //BEGIN SW
  CEPoint	subLinePos;
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
          do
            {
              nSubLine++;
            }
          while (IsEmptySubLineIndex(nSubLine));
        }
      SubLineCursorPosToTextPos( CEPoint( m_nIdealCharPos, nSubLine ), m_ptCursorPos );
      /*ORIGINAL
      m_ptCursorPos.y ++;
      m_ptCursorPos.x = ApproxActualOffset(m_ptCursorPos.y, m_nIdealCharPos);
      *///END SW
      if (m_ptCursorPos.x > GetLineLength (m_ptCursorPos.y))
        m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
    }
  EnsureVisible (m_ptCursorPos);
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
}

void CCrystalTextView::
MoveHome (bool bSelect)
{
  int nLength = GetLineLength (m_ptCursorPos.y);
  const tchar_t* pszChars = GetLineChars (m_ptCursorPos.y);
  //BEGIN SW
  CEPoint	pos;
  CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, pos );
  int nHomePos = SubLineHomeToCharPos( m_ptCursorPos.y, pos.y );
  int nOriginalHomePos = nHomePos;
  /*ORIGINAL
  int nHomePos = 0;
  *///END SW
  while (nHomePos < nLength && xisspace (pszChars[nHomePos]))
    nHomePos++;
  if (nHomePos == nLength || m_ptCursorPos.x == nHomePos)
    {
      //BEGIN SW
      m_ptCursorPos.x = nOriginalHomePos;
      /*ORIGINAL
      m_ptCursorPos.x = 0;
      *///END SW
    }
  else
    m_ptCursorPos.x = nHomePos;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
}

void CCrystalTextView::
MoveEnd (bool bSelect)
{
  //BEGIN SW
  CEPoint	pos;
  CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, pos );
  m_ptCursorPos.x = SubLineEndToCharPos( m_ptCursorPos.y, pos.y );
  /*ORIGINAL
  m_ptCursorPos.x = GetLineLength(m_ptCursorPos.y);
  *///END SW
  m_nIdealCharPos = INT_MAX;
  EnsureVisible (m_ptCursorPos);
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
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
  CEPoint subLinePos;
  CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, subLinePos );

  int nSubLine = GetSubLineIndex( m_ptCursorPos.y ) + subLinePos.y - GetScreenLines() + 1;

  if (nSubLine < nNewTopSubLine || nSubLine >= nNewTopSubLine + GetScreenLines())
    nSubLine = nNewTopSubLine;

  if ( nSubLine < 0 )
    nSubLine = 0;

  SubLineCursorPosToTextPos( CEPoint( m_nIdealCharPos, nSubLine ),
    m_ptCursorPos );

  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);    //todo: no vertical scroll

  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
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
  CEPoint subLinePos;
  CharPosToPoint( m_ptCursorPos.y, m_ptCursorPos.x, subLinePos );

  int nSubLine = GetSubLineIndex( m_ptCursorPos.y ) + subLinePos.y + GetScreenLines() - 1;

  if (nSubLine < nNewTopSubLine || nSubLine >= nNewTopSubLine + GetScreenLines())
    nSubLine = nNewTopSubLine + GetScreenLines() - 1;

  if( nSubLine > nSubLineCount - 1 )
    nSubLine = nSubLineCount - 1;

  SubLineCursorPosToTextPos( 
    CEPoint( m_nIdealCharPos, nSubLine ), m_ptCursorPos );

  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);    //todo: no vertical scroll

  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
}

void CCrystalTextView::
MoveCtrlHome (bool bSelect)
{
  m_ptCursorPos.x = 0;
  m_ptCursorPos.y = 0;
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
}

void CCrystalTextView::
MoveCtrlEnd (bool bSelect)
{
  m_ptCursorPos.y = GetLineCount () - 1;
  m_ptCursorPos.x = GetLineLength (m_ptCursorPos.y);
  m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
  EnsureVisible (m_ptCursorPos);
  if (!bSelect)
    m_ptAnchor = m_ptCursorPos;
  SetSelection (m_ptAnchor, m_ptCursorPos);
  UpdateCaret ();
}

void CCrystalTextView::
ScrollUp ()
{
  if (m_nTopLine > 0)
    {
      ScrollToLine (m_nTopLine - 1);
      UpdateCaret ();
      UpdateSiblingScrollPos (false);
    }
}

void CCrystalTextView::
ScrollDown ()
{
  if (m_nTopLine < GetLineCount () - 1)
    {
      ScrollToLine (m_nTopLine + 1);
      UpdateCaret ();
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
  if (CoverLength(m_nTopLine, GetScreenLines(), m_nOffsetChar))
    {
      ScrollToChar (m_nOffsetChar + 1);
      UpdateCaret ();
      UpdateSiblingScrollPos (true);
    }
}

CEPoint CCrystalTextView::
WordToRight (CEPoint pt)
{
  ASSERT_VALIDTEXTPOS (pt);
  int nLength = GetLineLength (pt.y);
  if (pt.x < nLength)
    {
      auto pIterWord = ICUBreakIterator::getWordBreakIterator(reinterpret_cast<const UChar *>(GetLineChars(pt.y)), nLength);
      pt.x = pIterWord->following(pt.x);
    }
  ASSERT_VALIDTEXTPOS (pt);
  return pt;
}

CEPoint CCrystalTextView::
WordToLeft (CEPoint pt)
{
  ASSERT_VALIDTEXTPOS (pt);
  if (pt.x > 0)
    {
      auto pIterWord = ICUBreakIterator::getWordBreakIterator(reinterpret_cast<const UChar *>(GetLineChars(pt.y)), GetLineLength(pt.y));
      pt.x = pIterWord->following(pt.x);
      pt.x = pIterWord->preceding(pt.x);
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
  m_ptAnchor = CEPoint (0, 0);
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

  auto startDragging = [this]()
    {
      SetCapture ();
      m_nDragSelTimer = SetTimer (CRYSTAL_TIMER_DRAGSEL, 100, nullptr);
      ASSERT (m_nDragSelTimer != 0);
      m_bDragSelection = true;
    };

  m_nColumnResizing = -1;

  if (point.y < GetTopMarginHeight ())
    {
      const int nColumn = ClientToColumnResizing (point.x);
      if (nColumn >= 0)
        {
          m_nColumnResizing = nColumn;
        }
      else
        {
          int x = ClientToIdealTextPos (point.x);
          CEPoint ptStart, ptEnd;
          const int nLineCount = GetLineCount ();
          for (int y = 0; y < nLineCount; ++y)
            {
              if (x < CalculateActualOffset (y, GetLineLength (y)))
                {
                  ptStart.x = ApproxActualOffset (y, x);
                  ptStart.y = y;
                  break;
                }
            }
          ptEnd.y = nLineCount - 1;
          for (int y = nLineCount - 1; y >= 0; --y)
            {
              if (x < CalculateActualOffset (y, GetLineLength (y)))
                {
                  ptEnd.x = ApproxActualOffset (y, x);
                  ptEnd.y = y;
                  break;
                }
            }
    
          m_bWordSelection = false;
          m_bLineSelection = false;
          m_bRectangularSelection = true;
          m_bColumnSelection = true;
    
          SetSelection (ptStart, ptEnd);
          m_ptAnchor = ptStart;
          m_nIdealCharPos = x;
        }

      startDragging();

      return;
    }
  else if (point.x < GetMarginWidth ())
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
          CEPoint pos;
          CharPosToPoint (m_ptCursorPos.y, m_ptCursorPos.x, pos);
          m_ptCursorPos.x = SubLineHomeToCharPos (m_ptCursorPos.y, pos.y);

          if (!bShift)
            m_ptAnchor = m_ptCursorPos;

          CEPoint ptStart, ptEnd;
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
          EnsureVisible (m_ptCursorPos);
          SetSelection (ptStart, ptEnd);
          UpdateCaret ();

          startDragging();

          m_bColumnSelection = false;
          m_bRectangularSelection = false;
          m_bWordSelection = false;
          m_bLineSelection = true;
        }
    }
  else
    {
      CEPoint ptText = ClientToText (point);
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

          CEPoint ptStart, ptEnd;
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
          EnsureVisible (m_ptCursorPos);
          SetSelection (ptStart, ptEnd);
          UpdateCaret ();

          startDragging();

          m_bColumnSelection = false;
          m_bRectangularSelection = bAlt;
          m_bWordSelection = bControl;
          m_bLineSelection = false;
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

      if (m_nColumnResizing >= 0)
      {
          int columnleft = ColumnToClient (m_nColumnResizing);
          int nWidth = (point.x - columnleft) / GetCharWidth();
          if (nWidth <= 0)
            nWidth = 1;
          m_pTextBuffer->SetColumnWidth (m_nColumnResizing, nWidth);
          int nOffsetChar = m_nOffsetChar;
          m_pTextBuffer->InvalidateColumns ();
          m_nOffsetChar = nOffsetChar;
          return;
      }

      CEPoint ptNewCursorPos = ClientToText (point);
      if (m_bColumnSelection)
        {
          int x = ClientToIdealTextPos (point.x);
          ptNewCursorPos.x = ApproxActualOffset (m_ptSelEnd.y, x);
          ptNewCursorPos.y = m_ptSelEnd.y;
          SetSelection (m_ptAnchor, ptNewCursorPos);
          m_nIdealCharPos = x;
          return;
        }

      CEPoint ptStart, ptEnd;
      if (m_bLineSelection)
        {
          if (ptNewCursorPos.y < m_ptAnchor.y ||
                ptNewCursorPos.y == m_ptAnchor.y && ptNewCursorPos.x < m_ptAnchor.x)
            {
              CEPoint  pos;
              ptEnd = m_ptAnchor;
              CharPosToPoint( ptEnd.y, ptEnd.x, pos );
              if( GetSubLineIndex( ptEnd.y ) + pos.y == GetSubLineCount() - 1 )
                ptEnd.x = SubLineEndToCharPos( ptEnd.y, pos.y );
              else
                {
                  int nLine, nSubLine;
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

              CEPoint  pos;
              CharPosToPoint( ptEnd.y, ptEnd.x, pos );
              ptEnd.x = SubLineHomeToCharPos( ptEnd.y, pos.y );

              m_ptCursorPos = ptNewCursorPos;
              CharPosToPoint( ptNewCursorPos.y, ptNewCursorPos.x, pos );
              if( GetSubLineIndex( ptNewCursorPos.y ) + pos.y == GetSubLineCount() - 1 )
                ptNewCursorPos.x = SubLineEndToCharPos( ptNewCursorPos.y, pos.y );
              else
                {
                  int nLine, nSubLine;
                  GetLineBySubLine( GetSubLineIndex( ptNewCursorPos.y ) + pos.y + 1, nLine, nSubLine );
                  ptNewCursorPos.y = nLine;
                  ptNewCursorPos.x = SubLineHomeToCharPos( nLine, nSubLine );
                }
              m_ptCursorPos = ptNewCursorPos;
            }
          EnsureVisible(m_ptCursorPos);
          SetSelection (ptNewCursorPos, ptEnd);
          UpdateCaret ();
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
      EnsureVisible(m_ptCursorPos);
      SetSelection (ptStart, ptEnd);
      UpdateCaret ();
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
      if (m_nColumnResizing >= 0)
        {
          m_ptAnchor = m_ptSelStart;
          m_nIdealCharPos = CalculateActualOffset (m_ptCursorPos.y, m_ptCursorPos.x);
        }
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
      SetSelection (m_ptCursorPos, m_ptCursorPos);
      UpdateCaret ();
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
          UpdateCaret ();
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
      if (bChanged && !m_nColumnResizing)
        {
          AdjustTextPoint (pt);
          CEPoint ptNewCursorPos = ClientToText (pt);
          if (ptNewCursorPos != m_ptCursorPos)
            {
              m_ptCursorPos = ptNewCursorPos;
              UpdateCaret ();
            }
          SetSelection (m_ptAnchor, m_ptCursorPos);
        }
    }
  else if (nIDEvent == CRYSTAL_RECALC_VSCROLLBAR)
    {
      KillTimer (CRYSTAL_RECALC_VSCROLLBAR);
      RecalcVertScrollBar ();
    }
  else if (nIDEvent == CRYSTAL_RECALC_HSCROLLBAR)
    {
      KillTimer (CRYSTAL_RECALC_HSCROLLBAR);
      RecalcHorzScrollBar ();
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

  if (point.y < GetTopMarginHeight ())
    {
      int nColumnResizing = ClientToColumnResizing (point.x);
      AutoFitColumn (nColumnResizing);
      return;
    }

  if (point.x < GetMarginWidth ())
    {
      AdjustTextPoint (point);
      CEPoint ptCursorPos = ClientToText (point);
      ToggleBookmark(ptCursorPos.y);
      return;
    }

  if (!m_bDragSelection)
    {
      AdjustTextPoint (point);

      m_ptCursorPos = ClientToText (point);
      m_ptAnchor = m_ptCursorPos;

      CEPoint ptStart, ptEnd;
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
      EnsureVisible (m_ptCursorPos);
      SetSelection (ptStart, ptEnd);
      UpdateCaret ();
      SetCapture ();
      m_nDragSelTimer = SetTimer (CRYSTAL_TIMER_DRAGSEL, 100, nullptr);
      ASSERT (m_nDragSelTimer != 0);
      m_bRectangularSelection = false;
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
  CPoint point2 = point;
  AdjustTextPoint (point2);
  CEPoint pt = ClientToText (point2);

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
IsSelection () const
{
#if _MSC_VER < 1910		// VS2015 (and earlier?) generates a "performance" warning
  // NOTE:  Comparing two `CEPoint` values yields a BOOL result; therefore this funny code
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
  if (!m_bRectangularSelection)
    GetText (m_ptDrawSelStart, m_ptDrawSelEnd, text);
  else
    GetTextInColumnSelection (text);
  PutToClipboard (text, text.GetLength(), m_bRectangularSelection);
}


bool CCrystalTextView::
TextInClipboard ()
{
  CLIPFORMAT fmt = GetClipTcharTextFormat();
  return !!IsClipboardFormatAvailable (fmt);
}

bool CCrystalTextView::
PutToClipboard (const tchar_t* pszText, int cchText, bool bColumnSelection)
{
  if (pszText == nullptr || cchText == 0)
    return false;

  CWaitCursor wc;
  bool bOK = false;
  if (OpenClipboard ())
    {
      EmptyClipboard ();
      SIZE_T cbData = (cchText + 1) * sizeof(tchar_t);
      HGLOBAL hData = GlobalAlloc (GMEM_MOVEABLE | GMEM_DDESHARE, cbData);
      if (hData != nullptr)
        {
          SIZE_T dwSize = GlobalSize(hData);
          tchar_t* pszData = (tchar_t*)::GlobalLock (hData);
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
          tchar_t* pszData = (tchar_t*) GlobalLock (hData);
          if (pszData != nullptr)
            {
              UINT cbData = (UINT) GlobalSize (hData);
              // in case we get an odd length for unicodes
              int cchText = ((cbData + sizeof(tchar_t)/sizeof(wchar_t)) / sizeof(tchar_t)) - 1;
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

void CCrystalTextView::
InvalidateVertScrollBar ()
{
  SetTimer(CRYSTAL_RECALC_VSCROLLBAR, 1, nullptr);
}

void CCrystalTextView::
InvalidateHorzScrollBar ()
{
  SetTimer(CRYSTAL_RECALC_HSCROLLBAR, 1, nullptr);
}
