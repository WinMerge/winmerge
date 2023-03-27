///////////////////////////////////////////////////////////////////////////
//  File:       ccrystaleditview.cpp
//  Version:    1.2.0.5
//  Created:    29-Dec-1998
//
//  Copyright:  Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Implementation of the CCrystalEditView class, a part of the Crystal Edit -
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
//
//  +   FEATURE: Auto indent
//  +   FIX: ResetView() was overriden to provide cleanup
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  19-Jul-99
//      Ferdinand Prantl:
//  +   FEATURE: support for autoindenting brackets and parentheses
//  +   FEATURE: menu options, view and window
//  +   FEATURE: SDI+MDI versions with help
//  +   FEATURE: extended registry support for saving settings
//  +   FEATURE: some other things I've forgotten ...
//  27-Jul-99
//  +   FIX: treating groups in regular expressions corrected
//  +   FIX: autocomplete corrected
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  ??-Aug-99
//      Sven Wiegand (search for "//BEGIN SW" to find my changes):
//  + FEATURE: "Go to last change" (sets the cursor on the position where
//          the user did his last edit actions
//  + FEATURE: Support for incremental search in CCrystalTextView
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  24-Oct-99
//      Sven Wiegand
//  + FEATURE: Supporting [Return] to exit incremental-search-mode
//           (see OnChar())
////////////////////////////////////////////////////////////////////////////

/**
 * @file  ccrystaleditview.cpp
 *
 * @brief Implementation of the CCrystalEditView class
 */

#include "StdAfx.h"
#include "ccrystaleditview.h"
#include "ccrystaltextbuffer.h"
#include "editcmd.h"
#include "editreg.h"
#include "dialogs/ceditreplacedlg.h"
#include "dialogs/chcondlg.h"
#include "utils/cs2cs.h"
#include "utils/string_util.h"
#include "utils/icu.hpp"

#ifndef __AFXPRIV_H__
#pragma message("Include <afxpriv.h> in your stdafx.h to avoid this message")
#include <afxpriv.h>
#endif
#ifndef __AFXOLE_H__
#pragma message("Include <afxole.h> in your stdafx.h to avoid this message")
#include <afxole.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const unsigned int MAX_TAB_LEN = 64;  // Same as in CrystalViewText.cpp

#define DRAG_BORDER_X       5
#define DRAG_BORDER_Y       5

/////////////////////////////////////////////////////////////////////////////
// CEditDropTargetImpl class declaration

class CEditDropTargetImpl : public COleDropTarget
  {
private :
    CCrystalEditView * m_pOwner;
public :
    explicit CEditDropTargetImpl (CCrystalEditView * pOwner)
      : m_pOwner(pOwner), m_pAlternateDropTarget(nullptr)
    {
    };

    virtual DROPEFFECT OnDragEnter (CWnd * pWnd, COleDataObject * pDataObject, DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave (CWnd * pWnd);
    virtual DROPEFFECT OnDragOver (CWnd * pWnd, COleDataObject * pDataObject, DWORD dwKeyState, CPoint point);
    virtual BOOL OnDrop (CWnd * pWnd, COleDataObject * pDataObject, DROPEFFECT dropEffect, CPoint point);
    virtual DROPEFFECT OnDragScroll (CWnd * pWnd, DWORD dwKeyState, CPoint point);

    IDropTarget * m_pAlternateDropTarget;
  };


/////////////////////////////////////////////////////////////////////////////
// CCrystalEditView

IMPLEMENT_DYNCREATE (CCrystalEditView, CCrystalTextView)

CCrystalEditView::CCrystalEditView ()
: m_nLastReplaceLen(0)
, m_bLastReplace(false)
, m_dwLastReplaceFlags(0)
, m_pEditReplaceDlg(nullptr)
, m_bDropPosVisible(false)
, m_bSelectionPushed(false)
, m_bAutoIndent(true)
, m_bDisableBSAtSOL(false)
, m_pDropTarget(nullptr)
, m_bMergeUndo(false)
{
}

CCrystalEditView:: ~CCrystalEditView ()
{
  delete m_pEditReplaceDlg;
}

bool CCrystalEditView::
DoSetTextType (CrystalLineParser::TextDefinition *def)
{
  m_CurSourceDef = def;
  SetAutoIndent ((def->flags & SRCOPT_AUTOINDENT) != 0);
  SetDisableBSAtSOL ((def->flags & SRCOPT_BSATBOL) == 0);
  return CCrystalTextView::DoSetTextType (def);
}

BEGIN_MESSAGE_MAP (CCrystalEditView, CCrystalTextView)
//{{AFX_MSG_MAP(CCrystalEditView)
ON_COMMAND (ID_EDIT_PASTE, OnEditPaste)
ON_UPDATE_COMMAND_UI (ID_EDIT_CUT, OnUpdateEditCut)
ON_COMMAND (ID_EDIT_CUT, OnEditCut)
ON_UPDATE_COMMAND_UI (ID_EDIT_PASTE, OnUpdateEditPaste)
ON_COMMAND (ID_EDIT_DELETE, OnEditDelete)
ON_WM_CHAR ()
ON_COMMAND (ID_EDIT_DELETE_BACK, OnEditDeleteBack)
ON_COMMAND (ID_EDIT_UNTAB, OnEditUntab)
ON_COMMAND (ID_EDIT_TAB, OnEditTab)
ON_COMMAND (ID_EDIT_SWITCH_OVRMODE, OnEditSwitchOvrmode)
ON_UPDATE_COMMAND_UI (ID_EDIT_SWITCH_OVRMODE, OnUpdateEditSwitchOvrmode)
ON_WM_CREATE ()
ON_WM_DESTROY ()
ON_COMMAND (ID_EDIT_REPLACE, OnEditReplace)
ON_UPDATE_COMMAND_UI (ID_EDIT_UNDO, OnUpdateEditUndo)
ON_COMMAND (ID_EDIT_UNDO, OnEditUndo)
ON_UPDATE_COMMAND_UI (ID_EDIT_REDO, OnUpdateEditRedo)
ON_COMMAND (ID_EDIT_REDO, OnEditRedo)
ON_UPDATE_COMMAND_UI (ID_EDIT_AUTOCOMPLETE, OnUpdateEditAutoComplete)
ON_COMMAND (ID_EDIT_AUTOCOMPLETE, OnEditAutoComplete)
ON_UPDATE_COMMAND_UI (ID_EDIT_LOWERCASE, OnUpdateEditLowerCase)
ON_COMMAND (ID_EDIT_LOWERCASE, OnEditLowerCase)
ON_UPDATE_COMMAND_UI (ID_EDIT_UPPERCASE, OnUpdateEditUpperCase)
ON_COMMAND (ID_EDIT_UPPERCASE, OnEditUpperCase)
ON_UPDATE_COMMAND_UI (ID_EDIT_SWAPCASE, OnUpdateEditSwapCase)
ON_COMMAND (ID_EDIT_SWAPCASE, OnEditSwapCase)
ON_UPDATE_COMMAND_UI (ID_EDIT_SWAPCASE, OnUpdateEditSwapCase)
ON_COMMAND (ID_EDIT_SWAPCASE, OnEditSwapCase)
ON_UPDATE_COMMAND_UI (ID_EDIT_CAPITALIZE, OnUpdateEditCapitalize)
ON_COMMAND (ID_EDIT_CAPITALIZE, OnEditCapitalize)
ON_UPDATE_COMMAND_UI (ID_EDIT_SENTENCE, OnUpdateEditSentence)
ON_COMMAND (ID_EDIT_SENTENCE, OnEditSentence)
ON_UPDATE_COMMAND_UI(ID_EDIT_GOTO_LAST_CHANGE, OnUpdateEditGotoLastChange)
ON_COMMAND(ID_EDIT_GOTO_LAST_CHANGE, OnEditGotoLastChange)
ON_COMMAND(ID_EDIT_DELETE_WORD, OnEditDeleteWord)
ON_COMMAND(ID_EDIT_DELETE_WORD_BACK, OnEditDeleteWordBack)
ON_WM_KILLFOCUS ()
//}}AFX_MSG_MAP
ON_UPDATE_COMMAND_UI (ID_EDIT_INDICATOR_READ, OnUpdateIndicatorRead)
ON_UPDATE_COMMAND_UI (ID_INDICATOR_OVR, OnUpdateIndicatorOvr)
ON_UPDATE_COMMAND_UI (ID_EDIT_INDICATOR_POSITION, OnUpdateIndicatorPosition)
ON_UPDATE_COMMAND_UI (ID_TOOLS_SPELLING, OnUpdateToolsSpelling)
ON_COMMAND (ID_TOOLS_SPELLING, OnToolsSpelling)
ON_UPDATE_COMMAND_UI (ID_TOOLS_CHARCODING, OnUpdateToolsCharCoding)
ON_COMMAND (ID_TOOLS_CHARCODING, OnToolsCharCoding)
// cursor movement commands
ON_COMMAND (ID_EDIT_CHAR_LEFT, OnCharLeft)
ON_COMMAND (ID_EDIT_EXT_CHAR_LEFT, OnExtCharLeft)
ON_COMMAND (ID_EDIT_CHAR_RIGHT, OnCharRight)
ON_COMMAND (ID_EDIT_EXT_CHAR_RIGHT, OnExtCharRight)
ON_COMMAND (ID_EDIT_WORD_LEFT, OnWordLeft)
ON_COMMAND (ID_EDIT_EXT_WORD_LEFT, OnExtWordLeft)
ON_COMMAND (ID_EDIT_WORD_RIGHT, OnWordRight)
ON_COMMAND (ID_EDIT_EXT_WORD_RIGHT, OnExtWordRight)
ON_COMMAND (ID_EDIT_LINE_UP, OnLineUp)
ON_COMMAND (ID_EDIT_EXT_LINE_UP, OnExtLineUp)
ON_COMMAND (ID_EDIT_LINE_DOWN, OnLineDown)
ON_COMMAND (ID_EDIT_EXT_LINE_DOWN, OnExtLineDown)
ON_COMMAND (ID_EDIT_SCROLL_UP, ScrollUp)
ON_COMMAND (ID_EDIT_SCROLL_DOWN, ScrollDown)
ON_COMMAND (ID_EDIT_PAGE_UP, OnPageUp)
ON_COMMAND (ID_EDIT_EXT_PAGE_UP, OnExtPageUp)
ON_COMMAND (ID_EDIT_PAGE_DOWN, OnPageDown)
ON_COMMAND (ID_EDIT_EXT_PAGE_DOWN, OnExtPageDown)
ON_COMMAND (ID_EDIT_LINE_END, OnLineEnd)
ON_COMMAND (ID_EDIT_EXT_LINE_END, OnExtLineEnd)
ON_COMMAND (ID_EDIT_HOME, OnHome)
ON_COMMAND (ID_EDIT_EXT_HOME, OnExtHome)
ON_COMMAND (ID_EDIT_TEXT_BEGIN, OnTextBegin)
ON_COMMAND (ID_EDIT_EXT_TEXT_BEGIN, OnExtTextBegin)
ON_COMMAND (ID_EDIT_TEXT_END, OnTextEnd)
ON_COMMAND (ID_EDIT_EXT_TEXT_END, OnExtTextEnd)
ON_WM_LBUTTONDOWN ()
ON_WM_RBUTTONDOWN ()
END_MESSAGE_MAP ()


/////////////////////////////////////////////////////////////////////////////
// CCrystalEditView message handlers

void CCrystalEditView::ResetView ()
{
  // m_bAutoIndent = true;
  m_bOvrMode = false;
  m_bLastReplace = false;
  CCrystalTextView::ResetView ();
}

bool CCrystalEditView::
QueryEditable ()
{
  if (m_pTextBuffer == nullptr)
    return false;
  return !m_pTextBuffer->GetReadOnly ();
}

void CCrystalEditView::
OnEditPaste ()
{
  Paste ();
}

void CCrystalEditView::
OnUpdateEditPaste (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (TextInClipboard ());
}

void CCrystalEditView::
OnUpdateEditCut (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (IsSelection ());
}

void CCrystalEditView::
OnEditCut ()
{
  Cut ();
}

bool CCrystalEditView::
DeleteCurrentSelection ()
{
  if (IsSelection ())
    {
      auto [ptSelStart, ptSelEnd] = GetSelection ();

      CEPoint ptCursorPos = ptSelStart;
      ASSERT_VALIDTEXTPOS (ptCursorPos);
      SetAnchor (ptCursorPos);
      SetSelection (ptCursorPos, ptCursorPos);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);

      // [JRT]:
      m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_DELSEL);
      return true;
    }
  return false;
}

bool CCrystalEditView::
DeleteCurrentColumnSelection (int nAction, bool bFlushUndoGroup /*= true*/, bool bUpdateCursorPosition /*= true*/)
{
  if (IsSelection ())
    {
      if (bFlushUndoGroup)
        m_pTextBuffer->BeginUndoGroup ();

      auto [ptSelStart, ptSelEnd] = GetSelection ();

      int nSelLeft, nSelRight;
      GetColumnSelection (m_ptDrawSelStart.y, nSelLeft, nSelRight);
      CEPoint ptCursorPos(nSelLeft, m_ptDrawSelStart.y);
      int nStartLine = m_ptDrawSelStart.y;
      int nEndLine = m_ptDrawSelEnd.y;

      if (GetEnableHideLines ())
        {
          for (int nLineIndex = nEndLine; nLineIndex >= nStartLine; nLineIndex--)
            {
              if (!(GetLineFlags (nLineIndex) & LF_INVISIBLE))
                {
                  int nEndLine2 = nLineIndex;
                  int nStartLine2;
                  for (nStartLine2 = nLineIndex - 1; nStartLine2 >= nStartLine; nStartLine2--)
                    {
                      if (GetLineFlags (nStartLine2) & LF_INVISIBLE)
                        break;
                    }  
                  nStartLine2++;
                  nLineIndex = nStartLine2;
                  DeleteCurrentColumnSelection2 (nStartLine2, nEndLine2, nAction);
                }
            }
        }
      else
        {
          DeleteCurrentColumnSelection2 (ptSelStart.y, ptSelEnd.y, nAction);
        }

      if (bFlushUndoGroup)
        m_pTextBuffer->FlushUndoGroup (this);

      if (bUpdateCursorPosition)
        {
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);
        }
      return true;
    }
  return false;
}

bool CCrystalEditView::
DeleteCurrentColumnSelection2 (int nStartLine, int nEndLine, int nAction)
{
  int nBufSize = 1;
  for (int L = nStartLine; L <= nEndLine; L++)
    nBufSize += GetFullLineLength (L);

  CString text;
  tchar_t* p, *pszBuf = text.GetBuffer (nBufSize);
  p = pszBuf;

  for (int I = nStartLine; I <= nEndLine; I++)
    {
      const tchar_t* pszChars = GetLineChars (I);
      int nSelLeft, nSelRight;
      GetColumnSelection (I, nSelLeft, nSelRight);
      if (nSelLeft > 0)
        {
          memcpy (p, pszChars, sizeof (tchar_t) * nSelLeft);
          p += nSelLeft;
        }
      int nLineLength = GetFullLineLength (I);
      if (nSelRight < nLineLength)
        {
          memcpy (p, pszChars + nSelRight, sizeof (tchar_t) * (nLineLength - nSelRight));
          p += nLineLength - nSelRight;
        }
    }

  p[0] = 0;
  text.ReleaseBuffer (static_cast<int>(p - pszBuf));
  text.FreeExtra ();

  if (nEndLine + 1 < GetLineCount())
    m_pTextBuffer->DeleteText (this, nStartLine, 0, nEndLine + 1, 0, nAction);
  else
    m_pTextBuffer->DeleteText (this, nStartLine, 0, nEndLine, GetLineLength (nEndLine), nAction);
  int x, y;
  m_pTextBuffer->InsertText (this, nStartLine, 0, text, text.GetLength(), y, x, nAction);

  return true;
}

bool CCrystalEditView::
InsertColumnText (int nLine, int nPos, const tchar_t* pszText, int cchText, int nAction, bool bFlushUndoGroup)
{
  if (pszText == nullptr || cchText == 0)
    return false;

  std::vector<tchar_t*> aLines;
  std::vector<uint32_t> aLineLengths;
  int nLineBegin = 0;
  for (int nTextPos = 0; nTextPos < cchText; )
    {
      tchar_t ch = 0;
      aLines.push_back ((tchar_t*)&pszText[nTextPos]);

      for (; nTextPos < cchText; nTextPos++)
        {
          ch = pszText[nTextPos];
          if (ch=='\r' || ch=='\n'/*iseol(pszText[nTextPos]*/)
            break;
        }

      aLineLengths.push_back (nTextPos - nLineBegin);

      // advance after EOL of line
      if (ch=='\r' && pszText[nTextPos + 1]=='\n'/*isdoseol(&pszText[nTextPos])*/)
        nTextPos += 2;
      else if (ch=='\r' || ch=='\n'/*iseol(pszText[nTextPos])*/)
        nTextPos++;
      nLineBegin = nTextPos;
    }

  int L;
  int nBufSize = 1;
  int nLineCount = GetLineCount ();
  ASSERT(aLineLengths.size() < INT_MAX);
  int nPasteTextLineCount = static_cast<int>(aLineLengths.size ());
  for (L = 0; L < nPasteTextLineCount; L++)
    {
      if (nLine + L < nLineCount)
        nBufSize += GetFullLineLength (nLine + L) + aLineLengths[L] + 2;
      else
        nBufSize += aLineLengths[L] + 2;
    }

  tchar_t* pszBuf = new tchar_t[nBufSize];
  tchar_t* p = pszBuf;
  int nTopBeginCharPos = CalculateActualOffset (nLine, nPos, true);
  for (L = 0; L < nPasteTextLineCount; L++)
    {
      if (nLine + L < nLineCount)
        {
          int nLineLength = GetFullLineLength (nLine + L);
          const tchar_t* pszChars = GetLineChars (nLine + L);
          int nOffset = ApproxActualOffset (nLine + L, nTopBeginCharPos);
          memcpy(p, pszChars, nOffset * sizeof(tchar_t));
          p += nOffset;
          memcpy(p, aLines[L], aLineLengths[L] * sizeof(tchar_t));
          p += aLineLengths[L];
          memcpy(p, pszChars + nOffset, (nLineLength - nOffset) * sizeof(tchar_t));
          p += nLineLength - nOffset;
        }
      else
        {
          CString sEol = m_pTextBuffer->GetDefaultEol ();
          if (p > pszBuf && p[-1] != '\r' && p[-1] != '\n')
            {
              memcpy(p, sEol, sEol.GetLength () * sizeof(tchar_t));
              p += sEol.GetLength ();
            }
          memcpy(p, aLines[L], aLineLengths[L] * sizeof(tchar_t));
          p += aLineLengths[L];
          memcpy(p, sEol, sEol.GetLength () * sizeof(tchar_t));
          p += sEol.GetLength ();
        }
    }
  p[0] = 0;

  if (bFlushUndoGroup)
    m_pTextBuffer->BeginUndoGroup ();

  if (nLine + nPasteTextLineCount + 1 < nLineCount)
    m_pTextBuffer->DeleteText (this, nLine, 0, nLine + nPasteTextLineCount, 0, nAction);
  else if (nLine != nLineCount - 1 || GetLineLength (nLineCount - 1) != 0)
    m_pTextBuffer->DeleteText (this, nLine, 0, nLineCount - 1, GetLineLength (nLineCount - 1), nAction);
  int x, y;
  m_pTextBuffer->InsertText (this, nLine, 0, pszBuf, static_cast<int>(p - pszBuf), x, y, nAction);

  if (bFlushUndoGroup)
    m_pTextBuffer->FlushUndoGroup (this);

  delete [] pszBuf;

  return true;
}

void CCrystalEditView::
Paste ()
{
  if (!QueryEditable ())
    return;
  if (m_pTextBuffer == nullptr)
    return;

  CString text;
  bool bColumnSelection;
  if (GetFromClipboard (text, bColumnSelection))
    {
      m_pTextBuffer->BeginUndoGroup ();
    
      CEPoint ptCursorPos;
      if (IsSelection ())
        {
          auto [ptSelStart, ptSelEnd] = GetSelection ();

          ptCursorPos = ptSelStart;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/

          // [JRT]:
          if (!m_bRectangularSelection)
            m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_PASTE);
          else
            DeleteCurrentColumnSelection (CE_ACTION_PASTE, false, false);
        }
      else
        {
          ptCursorPos = GetCursorPos ();
        }
      ASSERT_VALIDTEXTPOS (ptCursorPos);
      
      if (!bColumnSelection)
        {
          int x, y;
          m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, text, text.GetLength(), y, x, CE_ACTION_PASTE);  //  [JRT]
          ptCursorPos.x = x;
          ptCursorPos.y = y;
        }
      else
        InsertColumnText (ptCursorPos.y, ptCursorPos.x, text, text.GetLength(), CE_ACTION_PASTE, false);

      ASSERT_VALIDTEXTPOS (ptCursorPos);
      SetAnchor (ptCursorPos);
      SetSelection (ptCursorPos, ptCursorPos);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);

      m_pTextBuffer->FlushUndoGroup (this);
    }
}

void CCrystalEditView::
Cut ()
{
  if (!QueryEditable ())
    return;
  if (m_pTextBuffer == nullptr)
    return;
  if (!IsSelection ())
    return;

  auto [ptSelStart, ptSelEnd] = GetSelection ();
  CString text;
  if (!m_bRectangularSelection)
    GetText (ptSelStart, ptSelEnd, text);
  else
    GetTextInColumnSelection (text);
  PutToClipboard (text, text.GetLength());

  if (!m_bRectangularSelection)
    {
      CEPoint ptCursorPos = ptSelStart;
      ASSERT_VALIDTEXTPOS (ptCursorPos);
      SetAnchor (ptCursorPos);
      SetSelection (ptCursorPos, ptCursorPos);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);

      m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_CUT);  // [JRT]
    }
  else
    DeleteCurrentColumnSelection (CE_ACTION_CUT);
}

void CCrystalEditView::
OnEditDelete ()
{
  if (!QueryEditable () || m_pTextBuffer == nullptr)
    return;

  auto [ptSelStart, ptSelEnd] = GetSelection ();
  if (ptSelStart == ptSelEnd)
    {
      if (ptSelEnd.x == GetLineLength (ptSelEnd.y))
        {
          if (ptSelEnd.y == GetLineCount () - 1)
            return;
          ptSelEnd.y++;
          ptSelEnd.x = 0;
        }
      else 
        {
          auto pIterChar = ICUBreakIterator::getCharacterBreakIterator(GetLineChars(ptSelEnd.y), GetLineLength(ptSelEnd.y));
          ptSelEnd.x = pIterChar->following(ptSelEnd.x);
        }
    }

  if (!m_bRectangularSelection)
    {
      CEPoint ptCursorPos = ptSelStart;
      ASSERT_VALIDTEXTPOS (ptCursorPos);
      SetAnchor (ptCursorPos);
      SetSelection (ptCursorPos, ptCursorPos);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);

      m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_DELETE);   // [JRT]
    }
  else
    DeleteCurrentColumnSelection (CE_ACTION_DELETE);
}

void CCrystalEditView::
OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
{
  //BEGIN SW
  // check if incremental search is active before call to CCrystalTextView::OnChar()
  bool bIncrementalSearch = m_bIncrementalSearchForward || m_bIncrementalSearchBackward;
  //END Sw
  CCrystalTextView::OnChar (static_cast<wchar_t>(nChar), nRepCnt, nFlags);
  //BEGIN SW
  // if we are in incremental search mode ignore the character
  if( m_bIncrementalSearchForward || m_bIncrementalSearchBackward )
    return;
  //END SW

  // if we *were* in incremental search mode and CCrystalTextView::OnChar()
  // exited it the ignore the character (VK_RETURN)
  if( bIncrementalSearch && !m_bIncrementalSearchForward && !m_bIncrementalSearchBackward )
    return;
  //END SW

  if ((::GetAsyncKeyState (VK_LBUTTON) & 0x8000) != 0 ||
        (::GetAsyncKeyState (VK_RBUTTON) & 0x8000) != 0)
    return;

  if (nChar == VK_RETURN)
    {
      if (m_bOvrMode)
        {
          CEPoint ptCursorPos = GetCursorPos ();
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          if (ptCursorPos.y < GetLineCount () - 1)
            {
              ptCursorPos.x = 0;
              ptCursorPos.y++;

              ASSERT_VALIDTEXTPOS (ptCursorPos);
              SetSelection (ptCursorPos, ptCursorPos);
              SetAnchor (ptCursorPos);
              SetCursorPos (ptCursorPos);
              EnsureVisible (ptCursorPos);
              return;
            }
        }

      m_pTextBuffer->BeginUndoGroup(m_bMergeUndo);
      m_bMergeUndo = false;

      if (QueryEditable () && m_pTextBuffer != nullptr)
        {
          CEPoint ptCursorPos;
          if (IsSelection ())
            {
              auto [ptSelStart, ptSelEnd] = GetSelection ();
        
              ptCursorPos = ptSelStart;
              /*SetAnchor (ptCursorPos);
              SetSelection (ptCursorPos, ptCursorPos);
              SetCursorPos (ptCursorPos);
              EnsureVisible (ptCursorPos);*/
        
              // [JRT]:
              m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_TYPING);
            }
          else
            ptCursorPos = GetCursorPos ();
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          const tchar_t* pszText = m_pTextBuffer->GetDefaultEol();
          int cchText = (int)tc::tcslen(pszText);

          int x, y;
          m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, pszText, cchText, y, x, CE_ACTION_TYPING);  //  [JRT]

          ptCursorPos.x = x;
          ptCursorPos.y = y;
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetAnchor (ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);
        }

      m_pTextBuffer->FlushUndoGroup (this);
      return;
    }
  // Accept control characters other than [\t\n\r] through Alt-Numpad
  if (nChar > 31
  || GetKeyState(VK_CONTROL) >= 0 &&
      (nChar != 27 || GetKeyState(VK_ESCAPE) >= 0) &&
      nChar != 9 && nChar != 10 && nChar != 13)
    {
      if (QueryEditable () && m_pTextBuffer != nullptr)
        {
          m_pTextBuffer->BeginUndoGroup (m_bMergeUndo);
          m_bMergeUndo = true;

          auto [ptSelStart, ptSelEnd] = GetSelection ();
          CEPoint ptCursorPos;
          if (ptSelStart != ptSelEnd)
            {
              ptCursorPos = ptSelStart;
              if (IsSelection ())
                {
                  auto [ptSelStart1, ptSelEnd1] = GetSelection ();

                  /*SetAnchor (ptCursorPos);
                  SetSelection (ptCursorPos, ptCursorPos);
                  SetCursorPos (ptCursorPos);
                  EnsureVisible (ptCursorPos);*/
            
                  // [JRT]:
                  m_pTextBuffer->DeleteText (this, ptSelStart1.y, ptSelStart1.x, ptSelEnd1.y, ptSelEnd1.x, CE_ACTION_TYPING);
                }
            }
          else
            {
              ptCursorPos = GetCursorPos ();
              if (m_bOvrMode && ptCursorPos.x < GetLineLength (ptCursorPos.y))
                m_pTextBuffer->DeleteText (this, ptCursorPos.y, ptCursorPos.x, ptCursorPos.y, ptCursorPos.x + 1, CE_ACTION_TYPING);     // [JRT]

            }

          ASSERT_VALIDTEXTPOS (ptCursorPos);

          tchar_t pszText[2];
          pszText[0] = (tchar_t) nChar;
          pszText[1] = 0;

          int x, y;
          m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, pszText, 1, y, x, CE_ACTION_TYPING);    // [JRT]

          ptCursorPos.x = x;
          ptCursorPos.y = y;
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetAnchor (ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);

          m_pTextBuffer->FlushUndoGroup (this);
        }
    }
}


//
//  [JRT]: Added Support For "Disable Backspace At Start Of Line"
//
void CCrystalEditView::
OnEditDeleteBack ()
{
  //BEGIN SW
  // if we are in incremental search mode ignore backspace
  CCrystalTextView::OnEditDeleteBack();
  if( m_bIncrementalSearchForward || m_bIncrementalSearchBackward )
    return;
  //END SW

  if (IsSelection ())
    {
      OnEditDelete ();
      return;
    }

  if (!QueryEditable () || m_pTextBuffer == nullptr)
    return;

  CEPoint ptCursorPos = GetCursorPos ();
  CEPoint ptCurrentCursorPos = ptCursorPos;
  bool bDeleted = false;

  if (!(ptCursorPos.x))         // If At Start Of Line

    {
      if (!m_bDisableBSAtSOL)   // If DBSASOL Is Disabled

        {
          if (ptCursorPos.y > 0)    // If Previous Lines Available

            {
              ptCursorPos.y--;  // Decrement To Previous Line

              ptCursorPos.x = GetLineLength (
                                ptCursorPos.y);   // Set Cursor To End Of Previous Line

              bDeleted = true;  // Set Deleted Flag

            }
        }
    }
  else                          // If Caret Not At SOL

    {
      auto pIterChar = ICUBreakIterator::getCharacterBreakIterator(GetLineChars(ptCursorPos.y), GetLineLength(ptCursorPos.y));
      ptCursorPos.x = pIterChar->preceding(ptCursorPos.x);
      bDeleted = true;          // Set Deleted Flag
    }
  /*
     if (ptCursorPos.x == 0)
     {
     if (ptCursorPos.y == 0)
     return;
     ptCursorPos.y --;
     ptCursorPos.x = GetLineLength(ptCursorPos.y);
     }
     else
     ptCursorPos.x --;
   */
  ASSERT_VALIDTEXTPOS (ptCursorPos);
  SetAnchor (ptCursorPos);
  SetSelection (ptCursorPos, ptCursorPos);
  SetCursorPos (ptCursorPos);
  EnsureVisible (ptCursorPos);

  if (bDeleted)
    {
      m_pTextBuffer->DeleteText (this, ptCursorPos.y, ptCursorPos.x, ptCurrentCursorPos.y, ptCurrentCursorPos.x, CE_ACTION_BACKSPACE);  // [JRT]

    }
  return;
}

void CCrystalEditView::
OnEditTab ()
{
  if (!QueryEditable () || m_pTextBuffer == nullptr)
    return;

  bool bTabify = false;
  CEPoint ptSelStart, ptSelEnd;
  if (IsSelection ())
    {
      GetSelection (ptSelStart, ptSelEnd);
      
      // If we have more than one line selected, tabify sel lines
      if ( ptSelStart.y != ptSelEnd.y )
        {
          bTabify = true;
        }
    }

  CEPoint ptCursorPos = GetCursorPos ();
  ASSERT_VALIDTEXTPOS (ptCursorPos);

  tchar_t pszText[MAX_TAB_LEN + 1] = {0};
  // If inserting tabs, then initialize the text to a tab.
  if (m_pTextBuffer->GetInsertTabs())
    {
      pszText[0] = '\t';
      pszText[1] = '\0';
    }
  else //...otherwise, built whitespace depending on the location and tab stops
    {
      int nTabSize = GetTabSize ();
      int nChars = nTabSize - (IsSelection() ? 0 : ptCursorPos.x) % nTabSize;
      for (int i = 0; i < nChars; i++)
        pszText[i] = ' ';
      pszText[nChars] = '\0';
    }

  // Indent selected lines (multiple lines selected)
  if (bTabify)
    {
      m_pTextBuffer->BeginUndoGroup ();

      int nStartLine = ptSelStart.y;
      int nEndLine = ptSelEnd.y;
      ptSelStart.x = 0;
      if (ptSelEnd.x > 0)
        {
          if (ptSelEnd.y == GetLineCount () - 1)
            {
              ptSelEnd.x = GetLineLength (ptSelEnd.y);
            }
          else
            {
              ptSelEnd.x = 0;
              ptSelEnd.y++;
            }
        }
      else  //...otherwise, do not indent the empty line.
        nEndLine--;
      SetSelection (ptSelStart, ptSelEnd);
      SetCursorPos (ptSelEnd);
      EnsureVisible (ptSelEnd);

      //  Shift selection to right
      m_bHorzScrollBarLocked = true;
      for (int L = nStartLine; L <= nEndLine; L++)
        {
          int x, y;
          if (m_pTextBuffer->IsIndentableLine (L))
            m_pTextBuffer->InsertText (this, L, 0, pszText, (int) tc::tcslen(pszText), y, x, CE_ACTION_INDENT);  //  [JRT]
        }
      m_bHorzScrollBarLocked = false;
      RecalcHorzScrollBar ();

      m_pTextBuffer->FlushUndoGroup (this);
      return;
    }

  // Overwrite mode, replace next char with tab/spaces
  if (m_bOvrMode)
    {
      CEPoint ptCursorPos1 = GetCursorPos ();
      ASSERT_VALIDTEXTPOS (ptCursorPos1);

      int nLineLength = GetLineLength (ptCursorPos1.y);
      const tchar_t* pszLineChars = GetLineChars (ptCursorPos1.y);

      // Not end of line
      if (ptCursorPos1.x < nLineLength)
        {
          int nTabSize = GetTabSize ();
          int nChars = nTabSize - CalculateActualOffset(
              ptCursorPos1.y, ptCursorPos1.x ) % nTabSize;
          ASSERT (nChars > 0 && nChars <= nTabSize);

          while (nChars > 0)
            {
              if (ptCursorPos1.x == nLineLength)
                break;
              if (pszLineChars[ptCursorPos1.x] == _T ('\t'))
                {
                  ptCursorPos1.x++;
                  break;
                }
              ptCursorPos1.x++;
              nChars--;
            }
          ASSERT (ptCursorPos1.x <= nLineLength);
          ASSERT_VALIDTEXTPOS (ptCursorPos1);

          SetSelection (ptCursorPos1, ptCursorPos1);
          SetAnchor (ptCursorPos1);
          SetCursorPos (ptCursorPos1);
          EnsureVisible (ptCursorPos1);
          return;
        }
    }

  m_pTextBuffer->BeginUndoGroup ();

  int x, y;	// For cursor position

  // Text selected, no overwrite mode, replace sel with tab
  if (IsSelection ())
    {
      auto [ptSelStart1, ptSelEnd1] = GetSelection ();

      /*SetAnchor (ptCursorPos);
      SetSelection (ptCursorPos, ptCursorPos);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);*/

      // [JRT]:
      m_pTextBuffer->DeleteText (this, ptSelStart1.y, ptSelStart1.x, ptSelEnd1.y, ptSelEnd1.x, CE_ACTION_TYPING);
      m_pTextBuffer->InsertText( this, ptSelStart1.y, ptSelStart1.x, pszText, (int) tc::tcslen(pszText), y, x, CE_ACTION_TYPING );
    }
  // No selection, add tab
  else
    {
      m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, pszText, (int) tc::tcslen(pszText), y, x, CE_ACTION_TYPING);  //  [JRT]
    }

  ptCursorPos.x = x;
  ptCursorPos.y = y;
  ASSERT_VALIDTEXTPOS (ptCursorPos);
  SetSelection (ptCursorPos, ptCursorPos);
  SetAnchor (ptCursorPos);
  SetCursorPos (ptCursorPos);
  EnsureVisible (ptCursorPos);

  m_pTextBuffer->FlushUndoGroup (this);
}

void CCrystalEditView::
OnEditUntab ()
{
  if (!QueryEditable () || m_pTextBuffer == nullptr)
    return;

  bool bTabify = false;
  if (IsSelection ())
    {
      auto [ptSelStart, ptSelEnd] = GetSelection ();
      bTabify = ptSelStart.y != ptSelEnd.y;
    }

  if (bTabify)
    {
      m_pTextBuffer->BeginUndoGroup ();

      auto [ptSelStart, ptSelEnd] = GetSelection ();
      int nStartLine = ptSelStart.y;
      int nEndLine = ptSelEnd.y;
      ptSelStart.x = 0;
      if (ptSelEnd.x > 0)
        {
          if (ptSelEnd.y == GetLineCount () - 1)
            {
              ptSelEnd.x = GetLineLength (ptSelEnd.y);
            }
          else
            {
              ptSelEnd.x = 0;
              ptSelEnd.y++;
            }
        }
      else
        nEndLine--;
      SetSelection (ptSelStart, ptSelEnd);
      SetCursorPos (ptSelEnd);
      EnsureVisible (ptSelEnd);

      //  Shift selection to left
      m_bHorzScrollBarLocked = true;
      for (int L = nStartLine; L <= nEndLine; L++)
        {
          int nLength = GetLineLength (L);
          if (nLength > 0)
            {
              const tchar_t* pszChars = GetLineChars (L);
              int nPos = 0, nOffset = 0;
              while (nPos < nLength)
                {
                  if (pszChars[nPos] == _T (' '))
                    {
                      nPos++;
                      if (++nOffset >= GetTabSize ())
                        break;
                    }
                  else
                    {
                      if (pszChars[nPos] == _T ('\t'))
                        nPos++;
                      break;
                    }
                }

              if (nPos > 0)
                m_pTextBuffer->DeleteText (this, L, 0, L, nPos, CE_ACTION_INDENT);  // [JRT]

            }
        }
      m_bHorzScrollBarLocked = false;
      RecalcHorzScrollBar ();

      m_pTextBuffer->FlushUndoGroup (this);
    }
  else
    {
      CEPoint ptCursorPos = GetCursorPos ();
      ASSERT_VALIDTEXTPOS (ptCursorPos);
      if (ptCursorPos.x > 0)
        {
          int nTabSize = GetTabSize ();
          int nOffset = CalculateActualOffset (ptCursorPos.y, ptCursorPos.x);
          int nNewOffset = nOffset / nTabSize * nTabSize;
          if (nOffset == nNewOffset && nNewOffset > 0)
            nNewOffset -= nTabSize;
          ASSERT (nNewOffset >= 0);

          const tchar_t* pszChars = GetLineChars (ptCursorPos.y);
          int nCurrentOffset = 0;
          int I = 0;
          while (nCurrentOffset < nNewOffset)
            {
              if (pszChars[I] == _T ('\t'))
                nCurrentOffset = nCurrentOffset / nTabSize * nTabSize + nTabSize;
              else
                nCurrentOffset++;
              I++;
            }

          ASSERT (nCurrentOffset == nNewOffset);

          ptCursorPos.x = I;
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetAnchor (ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);
        }
    }
}

void CCrystalEditView::
OnUpdateIndicatorCol (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (false);
}

void CCrystalEditView::
OnUpdateIndicatorOvr (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (m_bOvrMode);
}

void CCrystalEditView::
OnUpdateIndicatorRead (CCmdUI * pCmdUI)
{
  if (m_pTextBuffer == nullptr)
    pCmdUI->Enable (false);
  else
    pCmdUI->Enable (m_pTextBuffer->GetReadOnly ());
}

void CCrystalEditView::
OnEditSwitchOvrmode ()
{
  m_bOvrMode = !m_bOvrMode;
  UpdateCaret ();
}

void CCrystalEditView::
OnUpdateEditSwitchOvrmode (CCmdUI * pCmdUI)
{
  pCmdUI->SetCheck (m_bOvrMode ? 1 : 0);
}

DROPEFFECT CEditDropTargetImpl::
OnDragEnter (CWnd * pWnd, COleDataObject * pDataObject, DWORD dwKeyState, CPoint point)
{
  CLIPFORMAT fmt = CCrystalTextView::GetClipTcharTextFormat();
  if (!pDataObject->IsDataAvailable (fmt))
    {
      if (m_pAlternateDropTarget != nullptr)
        {
          DROPEFFECT dwEffect = DROPEFFECT_NONE;
          m_pAlternateDropTarget->DragEnter(pDataObject->m_lpDataObject, dwKeyState, { point.x, point.y }, &dwEffect);
          return dwEffect;
        }
      m_pOwner->HideDropIndicator ();
      return DROPEFFECT_NONE;
    }
  m_pOwner->ShowDropIndicator (point);
  if (dwKeyState & MK_CONTROL)
    return DROPEFFECT_COPY;
  return DROPEFFECT_MOVE;
}

void CEditDropTargetImpl::
OnDragLeave (CWnd * pWnd)
{
  m_pOwner->HideDropIndicator ();
}

DROPEFFECT CEditDropTargetImpl::
OnDragOver (CWnd * pWnd, COleDataObject * pDataObject, DWORD dwKeyState, CPoint point)
{
  //
  // [JRT]
  //
  bool bDataSupported = false;

  if ((m_pOwner == nullptr) ||            // If No Owner
        (!(m_pOwner->QueryEditable ())) ||   // Or Not Editable
        (m_pOwner->GetDisableDragAndDrop ()))    // Or Drag And Drop Disabled

    {
      if (m_pAlternateDropTarget != nullptr)
        {
          DROPEFFECT dwEffect = DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK;
          m_pAlternateDropTarget->DragOver(dwKeyState, { point.x, point.y }, &dwEffect);
          return dwEffect;
        }
      m_pOwner->HideDropIndicator ();   // Hide Drop Caret

      return DROPEFFECT_NONE;   // Return DE_NONE

    }
  //  if ((pDataObject->IsDataAvailable( CF_TEXT ) ) ||       // If Text Available
  //          ( pDataObject -> IsDataAvailable( xxx ) ) ||    // Or xxx Available
  //          ( pDataObject -> IsDataAvailable( yyy ) ) )     // Or yyy Available
  CLIPFORMAT fmt = CCrystalTextView::GetClipTcharTextFormat();      // CF_TEXT or CF_UNICODETEXT
  if (pDataObject->IsDataAvailable (fmt))   // If Text Available

    {
      bDataSupported = true;    // Set Flag

    }
  if (!bDataSupported)          // If No Supported Formats Available

    {
      if (m_pAlternateDropTarget != nullptr)
        {
          DROPEFFECT dwEffect = DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK;
          m_pAlternateDropTarget->DragOver(dwKeyState, { point.x, point.y }, &dwEffect);
          return dwEffect;
        }
      m_pOwner->HideDropIndicator ();   // Hide Drop Caret

      return DROPEFFECT_NONE;   // Return DE_NONE

    }
  m_pOwner->ShowDropIndicator (point);
  if (dwKeyState & MK_CONTROL)
    return DROPEFFECT_COPY;
  return DROPEFFECT_MOVE;
}

BOOL CEditDropTargetImpl::
OnDrop (CWnd * pWnd, COleDataObject * pDataObject, DROPEFFECT dropEffect, CPoint point)
{
  //
  // [JRT]            ( m_pOwner -> GetDisableDragAndDrop() ) )       // Or Drag And Drop Disabled
  //
  bool bDataSupported = false;

  m_pOwner->HideDropIndicator ();   // Hide Drop Caret

  if ((m_pOwner == nullptr) ||            // If No Owner
        (!(m_pOwner->QueryEditable ())) ||   // Or Not Editable
        (m_pOwner->GetDisableDragAndDrop ()))    // Or Drag And Drop Disabled

    {
      if (m_pAlternateDropTarget != nullptr)
        {
          DROPEFFECT dwEffect = DROPEFFECT_NONE;
          m_pAlternateDropTarget->Drop(pDataObject->m_lpDataObject, 0, { point.x, point.y }, &dwEffect);
          return dwEffect;
        }
      return DROPEFFECT_NONE;   // Return DE_NONE

    }
  //  if( ( pDataObject -> IsDataAvailable( CF_TEXT ) ) ||    // If Text Available
  //          ( pDataObject -> IsDataAvailable( xxx ) ) ||    // Or xxx Available
  //          ( pDataObject -> IsDataAvailable( yyy ) ) )     // Or yyy Available
  CLIPFORMAT fmt = CCrystalTextView::GetClipTcharTextFormat();      // CF_TEXT or CF_UNICODETEXT
  if (pDataObject->IsDataAvailable (fmt))   // If Text Available

    {
      bDataSupported = true;    // Set Flag

    }
  if (!bDataSupported)          // If No Supported Formats Available

    {
      if (m_pAlternateDropTarget != nullptr)
        {
          DROPEFFECT dwEffect = DROPEFFECT_NONE;
          m_pAlternateDropTarget->Drop(pDataObject->m_lpDataObject, 0, { point.x, point.y }, &dwEffect);
          return dwEffect;
        }
      return DROPEFFECT_NONE;   // Return DE_NONE

    }
  return (m_pOwner->DoDropText (pDataObject, point));   // Return Result Of Drop

}

DROPEFFECT CEditDropTargetImpl::
OnDragScroll (CWnd * pWnd, DWORD dwKeyState, CPoint point)
{
  ASSERT (m_pOwner == pWnd);
  m_pOwner->DoDragScroll (point);

  if (dwKeyState & MK_CONTROL)
    return DROPEFFECT_COPY;
  return DROPEFFECT_MOVE;
}

void CCrystalEditView::
DoDragScroll (const CPoint & point)
{
  CRect rcClientRect;
  GetClientRect (rcClientRect);
  if (point.y < rcClientRect.top + DRAG_BORDER_Y)
    {
      HideDropIndicator ();
      ScrollUp ();
      UpdateWindow ();
      ShowDropIndicator (point);
      //UpdateSiblingScrollPos(false);
      return;
    }
  if (point.y >= rcClientRect.bottom - DRAG_BORDER_Y)
    {
      HideDropIndicator ();
      ScrollDown ();
      UpdateWindow ();
      ShowDropIndicator (point);
      //UpdateSiblingScrollPos(false);
      return;
    }
  if (point.x < rcClientRect.left + GetMarginWidth () + DRAG_BORDER_X)
    {
      HideDropIndicator ();
      ScrollLeft ();
      UpdateWindow ();
      ShowDropIndicator (point);
      //UpdateSiblingScrollPos(true);
      return;
    }
  if (point.x >= rcClientRect.right - DRAG_BORDER_X)
    {
      HideDropIndicator ();
      ScrollRight ();
      UpdateWindow ();
      ShowDropIndicator (point);
      //UpdateSiblingScrollPos(true);
      return;
    }
}

void CCrystalEditView::
SetAlternateDropTarget (IDropTarget *pDropTarget)
{
  ASSERT(m_pDropTarget->m_pAlternateDropTarget == nullptr);
  m_pDropTarget->m_pAlternateDropTarget = pDropTarget;
  m_pDropTarget->m_pAlternateDropTarget->AddRef();
}

bool CCrystalEditView::
DoDropText (COleDataObject * pDataObject, const CPoint & ptClient)
{
  CLIPFORMAT fmt = GetClipTcharTextFormat();      // CF_TEXT or CF_UNICODETEXT
  HGLOBAL hData = pDataObject->GetGlobalData (fmt);
  if (hData == nullptr)
    return false;

  CEPoint ptDropPos = ClientToText (ptClient);
  if (IsDraggingText () && IsInsideSelection (ptDropPos))
    {
      SetAnchor (ptDropPos);
      SetSelection (ptDropPos, ptDropPos);
      SetCursorPos (ptDropPos);
      EnsureVisible (ptDropPos);
      return false;
    }

  UINT cbData = (UINT) ::GlobalSize (hData);
  UINT cchText = cbData / sizeof(tchar_t) - 1;
  tchar_t* pszText = (tchar_t*)::GlobalLock (hData);
  if (pszText == nullptr)
    return false;

  // Open the undo group
  // When we drag from the same panel, it is already open, so do nothing
  // (we could test m_pTextBuffer->m_bUndoGroup if it were not a protected member)
  bool bGroupFlag = false;
  if (! IsDraggingText())
    {
      m_pTextBuffer->BeginUndoGroup ();
      bGroupFlag = true;
    } 

  int x, y;
  m_pTextBuffer->InsertText (this, ptDropPos.y, ptDropPos.x, pszText, cchText, y, x, CE_ACTION_DRAGDROP);  //   [JRT]

  CEPoint ptCurPos (x, y);
  ASSERT_VALIDTEXTPOS (ptCurPos);
  SetAnchor (ptDropPos);
  SetSelection (ptDropPos, ptCurPos);
  SetCursorPos (ptCurPos);
  EnsureVisible (ptCurPos);

  if (bGroupFlag)
    m_pTextBuffer->FlushUndoGroup (this);

  ::GlobalUnlock (hData);
  return true;
}

int CCrystalEditView::
OnCreate (LPCREATESTRUCT lpCreateStruct)
{
  if (CCrystalTextView::OnCreate (lpCreateStruct) == -1)
    return -1;

  ASSERT (m_pDropTarget == nullptr);
  m_pDropTarget = new CEditDropTargetImpl (this);
  if (!m_pDropTarget->Register (this))
    {
      TRACE0 ("Warning: Unable to register drop target for ccrystaleditview.\n");
      delete m_pDropTarget;
      m_pDropTarget = nullptr;
    }

  return 0;
}

void CCrystalEditView::
OnDestroy ()
{
  if (m_pDropTarget != nullptr)
    {
      m_pDropTarget->Revoke ();
      if (m_pDropTarget->m_pAlternateDropTarget)
        m_pDropTarget->m_pAlternateDropTarget->Release();
      delete m_pDropTarget;
      m_pDropTarget = nullptr;
    }

  CCrystalTextView::OnDestroy ();
}

void CCrystalEditView::
ShowDropIndicator (const CPoint & point)
{
  if (!m_bDropPosVisible)
    {
      HideCursor ();
      m_ptSavedCaretPos = GetCursorPos ();
      m_bDropPosVisible = true;
      ::CreateCaret (m_hWnd, (HBITMAP) 1, 2, GetLineHeight ());
    }
  m_ptDropPos = ClientToText (point);
  // NB: m_ptDropPos.x is index into char array, which is uncomparable to m_nOffsetChar.
  CPoint ptCaretPos = TextToClient (m_ptDropPos);
  if (ptCaretPos.x >= GetMarginWidth())
    {
      SetCaretPos (ptCaretPos);
      ShowCaret ();
    }
  else
    {
      HideCaret ();
    }
}

void CCrystalEditView::
HideDropIndicator ()
{
  if (m_bDropPosVisible)
    {
      SetCursorPos (m_ptSavedCaretPos);
      ShowCursor ();
      m_bDropPosVisible = false;
    }
}

DROPEFFECT CCrystalEditView::
GetDropEffect ()
{
  return DROPEFFECT_COPY | DROPEFFECT_MOVE;
}

void CCrystalEditView::
OnDropSource (DROPEFFECT de)
{
  if (!IsDraggingText ())
    return;

  ASSERT_VALIDTEXTPOS (m_ptDraggedTextBegin);
  ASSERT_VALIDTEXTPOS (m_ptDraggedTextEnd);

  if (de == DROPEFFECT_MOVE)
    {
      m_pTextBuffer->DeleteText (this, m_ptDraggedTextBegin.y, m_ptDraggedTextBegin.x, m_ptDraggedTextEnd.y,
                                 m_ptDraggedTextEnd.x, CE_ACTION_DRAGDROP);     // [JRT]

    }
}

void CCrystalEditView::
UpdateView (CCrystalTextView * pSource, CUpdateContext * pContext, updateview_flags_t dwFlags, int nLineIndex /*= -1*/ )
{
  CCrystalTextView::UpdateView (pSource, pContext, dwFlags, nLineIndex);

  if (m_bSelectionPushed && pContext != nullptr)
    {
      pContext->RecalcPoint (m_ptSavedSelStart);
      pContext->RecalcPoint (m_ptSavedSelEnd);
      ASSERT_VALIDTEXTPOS (m_ptSavedSelStart);
      ASSERT_VALIDTEXTPOS (m_ptSavedSelEnd);
    }
  if (m_bDropPosVisible )
    {
      pContext->RecalcPoint (m_ptSavedCaretPos);
      ASSERT_VALIDTEXTPOS (m_ptSavedCaretPos);
    }
}

void CCrystalEditView::
OnEditReplace ()
{
  if (!QueryEditable ())
    return;

  CWinApp *pApp = AfxGetApp ();
  ASSERT (pApp != nullptr);

  delete m_pEditReplaceDlg;
  m_pEditReplaceDlg = new CEditReplaceDlg(this);
  LastSearchInfos * lastSearch = m_pEditReplaceDlg->GetLastSearchInfos();

  if (m_bLastReplace)
    {
      //  Get the latest Replace parameters
      lastSearch->m_bMatchCase = (m_dwLastReplaceFlags & FIND_MATCH_CASE) != 0;
      lastSearch->m_bWholeWord = (m_dwLastReplaceFlags & FIND_WHOLE_WORD) != 0;
      lastSearch->m_bRegExp = (m_dwLastReplaceFlags & FIND_REGEXP) != 0;
      lastSearch->m_bNoWrap = (m_dwLastReplaceFlags & FIND_NO_WRAP) != 0;
      if (m_pszLastFindWhat != nullptr)
        lastSearch->m_sText = m_pszLastFindWhat;
    }
  else
    {
      DWORD dwFlags = pApp->GetProfileInt (EDITPAD_SECTION, _T("ReplaceFlags"), 0);
      lastSearch->m_bMatchCase = (dwFlags & FIND_MATCH_CASE) != 0;
      lastSearch->m_bWholeWord = (dwFlags & FIND_WHOLE_WORD) != 0;
      lastSearch->m_bRegExp = (dwFlags & FIND_REGEXP) != 0;
      lastSearch->m_bNoWrap = (dwFlags & FIND_NO_WRAP) != 0;
    }
  lastSearch->m_nDirection = 1;
  m_pEditReplaceDlg->UseLastSearch ();


  if (IsSelection ())
    {
      GetSelection (m_ptSavedSelStart, m_ptSavedSelEnd);
      m_bSelectionPushed = true;

      m_pEditReplaceDlg->SetScope(true);       //  Replace in current selection
      m_pEditReplaceDlg->m_ptCurrentPos = m_ptSavedSelStart;
      m_pEditReplaceDlg->m_bEnableScopeSelection = true;
      m_pEditReplaceDlg->m_ptBlockBegin = m_ptSavedSelStart;
      m_pEditReplaceDlg->m_ptBlockEnd = m_ptSavedSelEnd;

      // If the selection is in one line, copy text to dialog
      if (m_ptSavedSelStart.y == m_ptSavedSelEnd.y)
        GetText(m_ptSavedSelStart, m_ptSavedSelEnd, m_pEditReplaceDlg->m_sText);
    }
  else
    {
      m_pEditReplaceDlg->SetScope(false);      // Set scope when no selection
      m_pEditReplaceDlg->m_ptCurrentPos = GetCursorPos ();
      m_pEditReplaceDlg->m_bEnableScopeSelection = false;

      CEPoint ptCursorPos = GetCursorPos ();
      CEPoint ptStart = WordToLeft (ptCursorPos);
      CEPoint ptEnd = WordToRight (ptCursorPos);
      if (IsValidTextPos (ptStart) && IsValidTextPos (ptEnd) && ptStart != ptEnd)
        GetText (ptStart, ptEnd, m_pEditReplaceDlg->m_sText);
    }

  //  Execute Replace dialog
  m_pEditReplaceDlg->Create(CEditReplaceDlg::IDD, this);
  m_pEditReplaceDlg->ShowWindow(SW_SHOW);
}

void CCrystalEditView::
SaveLastSearch(LastSearchInfos *lastSearch)
{
  //  Save Replace parameters for 'F3' command
  m_bLastReplace = true;
  if (m_pszLastFindWhat != nullptr)
    free (m_pszLastFindWhat);
  m_pszLastFindWhat = tc::tcsdup (lastSearch->m_sText);
  m_dwLastReplaceFlags = 0;
  if (lastSearch->m_bMatchCase)
    m_dwLastReplaceFlags |= FIND_MATCH_CASE;
  if (lastSearch->m_bWholeWord)
    m_dwLastReplaceFlags |= FIND_WHOLE_WORD;
  if (lastSearch->m_bRegExp)
    m_dwLastReplaceFlags |= FIND_REGEXP;
  if (lastSearch->m_bNoWrap)
    m_dwLastReplaceFlags |= FIND_NO_WRAP;

  //  Restore selection
  if (m_bSelectionPushed)
    {
      SetSelection (m_ptSavedSelStart, m_ptSavedSelEnd);
      m_bSelectionPushed = false;
    }

  //  Save search parameters to registry
  VERIFY (AfxGetApp()->WriteProfileInt (EDITPAD_SECTION, _T ("ReplaceFlags"), m_dwLastReplaceFlags));
}

/**
 * @brief Replace selected text.
 * This function replaces selected text in the editor pane with given text.
 * @param [in] pszNewText The text replacing selected text.
 * @param [in] cchNewText Length of the replacing text.
 * @param [in] dwFlags Additional modifier flags:
 * - FIND_REGEXP: use the regular expression.
 * @return true if succeeded.
 */
bool CCrystalEditView::
ReplaceSelection (const tchar_t* pszNewText, size_t cchNewText, DWORD dwFlags, bool bGroupWithPrevious)
{
  if (!cchNewText)
    return DeleteCurrentSelection();
  ASSERT (pszNewText != nullptr);

  m_pTextBuffer->BeginUndoGroup(bGroupWithPrevious);

  CEPoint ptCursorPos;
  if (IsSelection ())
    {
      auto [ptSelStart, ptSelEnd] = GetSelection ();

      ptCursorPos = ptSelStart;

      m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_REPLACE);
    }
  else
    ptCursorPos = GetCursorPos ();
  ASSERT_VALIDTEXTPOS (ptCursorPos);

  int x = 0;
  int y = 0;
  if (dwFlags & FIND_REGEXP)
    {
      tchar_t* lpszNewStr = nullptr;
      if (m_pszMatched && !RxReplace(pszNewText, m_pszMatched, m_nLastFindWhatLen, m_rxmatch, &lpszNewStr, &m_nLastReplaceLen))
        {
          CString text;
          if (lpszNewStr != nullptr && m_nLastReplaceLen > 0)
            {
              tchar_t* buf = text.GetBuffer (m_nLastReplaceLen + 1);
              _tcsncpy_s (buf, m_nLastReplaceLen+1, lpszNewStr, m_nLastReplaceLen);
              text.ReleaseBuffer (m_nLastReplaceLen);
            }
          else
            text.Empty ();
          m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, text, text.GetLength(), y, x, CE_ACTION_REPLACE);  //  [JRT+FRD]
          if (lpszNewStr != nullptr)
            free(lpszNewStr);
        }
    }
  else
    {
      m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, pszNewText, cchNewText, y, x, CE_ACTION_REPLACE);  //  [JRT]
      ASSERT(cchNewText < INT_MAX);
      m_nLastReplaceLen = static_cast<int>(cchNewText);
    }

  CEPoint ptEndOfBlock = CEPoint (x, y);
  if (ptEndOfBlock.x == m_pTextBuffer->GetLineLength (ptEndOfBlock.y))
    {
      if (ptEndOfBlock.y < m_pTextBuffer->GetLineCount() - 1)
        {
          ptEndOfBlock.x = 0;
          ptEndOfBlock.y++;
        }
    }
  ASSERT_VALIDTEXTPOS (ptCursorPos);
  ASSERT_VALIDTEXTPOS (ptEndOfBlock);
  SetAnchor (ptEndOfBlock);
  SetSelection (ptCursorPos, ptEndOfBlock);
  SetCursorPos (ptEndOfBlock);

  m_pTextBuffer->FlushUndoGroup(this);

  return true;
}

void CCrystalEditView::
OnUpdateEditUndo (CCmdUI * pCmdUI)
{
  bool bCanUndo = m_pTextBuffer != nullptr && m_pTextBuffer->CanUndo ();
  pCmdUI->Enable (bCanUndo);

  //  Since we need text only for menus...
  if (pCmdUI->m_pMenu != nullptr)
    {
      //  Tune up 'resource handle'
      HINSTANCE hOldResHandle = AfxGetResourceHandle ();
      AfxSetResourceHandle (GetResourceHandle ());

      CString menu;
      if (bCanUndo)
        {
          //  Format menu item text using the provided item description
          std::basic_string<tchar_t> desc;
          m_pTextBuffer->GetUndoDescription (desc);
          menu.Format (IDS_MENU_UNDO_FORMAT, desc.c_str());
        }
      else
        {
          //  Just load default menu item text
          menu.LoadString (IDS_MENU_UNDO_DEFAULT);
        }

      //  Restore original handle
      AfxSetResourceHandle (hOldResHandle);

      //  Set menu item text
      pCmdUI->SetText (menu);
    }
}

void CCrystalEditView::
OnEditUndo ()
{
  DoEditUndo();
}

bool CCrystalEditView::
DoEditUndo ()
{
  if (m_pTextBuffer != nullptr && m_pTextBuffer->CanUndo ())
    {
      CEPoint ptCursorPos;
      if (m_pTextBuffer->Undo (this, ptCursorPos))
        {
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);
          return true;
        }
    }
  return false;
}

//  [JRT]
void CCrystalEditView::
SetDisableBSAtSOL (bool bDisableBSAtSOL)
{
  m_bDisableBSAtSOL = bDisableBSAtSOL;
}

void CCrystalEditView::
CopyProperties (CCrystalTextView* pSource)
{
  CCrystalTextView::CopyProperties(pSource);
  auto pSourceEditView = dynamic_cast<decltype(this)>(pSource);
  if (!pSourceEditView)
      return;
  m_bDisableBSAtSOL = pSourceEditView->m_bDisableBSAtSOL;
  m_bOvrMode = pSourceEditView->m_bOvrMode;
  m_bAutoIndent = pSourceEditView->m_bAutoIndent;
}

void CCrystalEditView::
OnEditRedo ()
{
  DoEditRedo();
}

bool CCrystalEditView::
DoEditRedo ()
{
  if (m_pTextBuffer != nullptr && m_pTextBuffer->CanRedo ())
    {
      CEPoint ptCursorPos;
      if (m_pTextBuffer->Redo (this, ptCursorPos))
        {
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);
          return true;
        }
    }
  return false;
}

void CCrystalEditView::
OnUpdateEditRedo (CCmdUI * pCmdUI)
{
  bool bCanRedo = m_pTextBuffer != nullptr && m_pTextBuffer->CanRedo ();
  pCmdUI->Enable (bCanRedo);

  //  Since we need text only for menus...
  if (pCmdUI->m_pMenu != nullptr)
    {
      //  Tune up 'resource handle'
      HINSTANCE hOldResHandle = AfxGetResourceHandle ();
      AfxSetResourceHandle (GetResourceHandle ());

      CString menu;
      if (bCanRedo)
        {
          //  Format menu item text using the provided item description
          std::basic_string<tchar_t> desc;
          m_pTextBuffer->GetRedoDescription (desc);
          menu.Format (IDS_MENU_REDO_FORMAT, desc.c_str());
        }
      else
        {
          //  Just load default menu item text
          menu.LoadString (IDS_MENU_REDO_DEFAULT);
        }

      //  Restore original handle
      AfxSetResourceHandle (hOldResHandle);

      //  Set menu item text
      pCmdUI->SetText (menu);
    }
}

bool
isopenbrace (tchar_t c)
{
  return c == _T ('{') || c == _T ('(') || c == _T ('[') || c == _T ('<');
}

bool
isclosebrace (tchar_t c)
{
  return c == _T ('}') || c == _T (')') || c == _T (']') || c == _T ('>');
}

bool
isopenbrace (const tchar_t* s)
{
  return s[1] == _T ('\0') && isopenbrace (*s);
}

bool
isclosebrace (const tchar_t* s)
{
  return s[1] == _T ('\0') && isclosebrace (*s);
}

int bracetype (tchar_t c);
int bracetype (const tchar_t* s);

void CCrystalEditView::
OnEditOperation (int nAction, const tchar_t* pszText, size_t cchText)
{
  if (m_bAutoIndent)
    {
      //  Analyse last action...
      if (nAction == CE_ACTION_TYPING && (
          tc::tcsncmp (pszText, _T ("\r\n"), cchText) == 0 ||
          (cchText == 1 && (*pszText == '\r' || *pszText == '\n')))
          && !m_bOvrMode && !m_pTextBuffer->GetTableEditing())
        {
          //  Enter stroke!
          CEPoint ptCursorPos = GetCursorPos ();
          ASSERT (ptCursorPos.y > 0);

          //  Take indentation from the previous line
          int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y - 1);
          const tchar_t* pszLineChars = m_pTextBuffer->GetLineChars (ptCursorPos.y - 1);
          int nPos = 0;
          while (nPos < nLength && xisspace (pszLineChars[nPos]))
            nPos++;

          if (nPos > 0)
            {
              if ((GetFlags () & SRCOPT_BRACEGNU) && isclosebrace (pszLineChars[nLength - 1]) && ptCursorPos.y > 0 && nPos && nPos == nLength - 1)
                {
                  if (pszLineChars[nPos - 1] == _T ('\t'))
                    {
                      nPos--;
                    }
                  else
                    {
                      int nTabSize = GetTabSize (),
                        nDelta = nTabSize - nPos % nTabSize;
                      if (!nDelta)
                        {
                          nDelta = nTabSize;
                        }
                      nPos -= nDelta;
                      if (nPos < 0)
                        {
                          nPos = 0;
                        }
                    }
                }
              //  Insert part of the previous line
              tchar_t *pszInsertStr;
              if ((GetFlags () & (SRCOPT_BRACEGNU|SRCOPT_BRACEANSI)) && isopenbrace (pszLineChars[nLength - 1]))
                {
                  if (m_pTextBuffer->GetInsertTabs())
                    {
                      const size_t InsertSiz = (nPos + 2);
                      pszInsertStr = static_cast<tchar_t *> (_alloca (sizeof(tchar_t) * InsertSiz));
                      _tcsncpy_s (pszInsertStr, InsertSiz, pszLineChars, nPos);
                      pszInsertStr[nPos++] = _T ('\t');
                    }
                  else
                    {
                      int nTabSize = GetTabSize ();
                      int nChars = nTabSize - nPos % nTabSize;
                      const size_t InsertSiz = (nPos + nChars + 1);
                      pszInsertStr = static_cast<tchar_t *> (_alloca (sizeof (tchar_t) * InsertSiz));
                      _tcsncpy_s (pszInsertStr, InsertSiz, pszLineChars, nPos);
                      while (nChars--)
                        {
                          pszInsertStr[nPos++] = _T (' ');
                        }
                    }
                }
              else
                {
                  const size_t InsertSiz = (nPos + 1);
                  pszInsertStr = static_cast<tchar_t *> (_alloca (sizeof (tchar_t) * InsertSiz));
                  _tcsncpy_s (pszInsertStr, InsertSiz, pszLineChars, nPos);
                }
              pszInsertStr[nPos] = 0;

              // m_pTextBuffer->BeginUndoGroup ();
              int x, y;
              m_pTextBuffer->InsertText (nullptr, ptCursorPos.y, ptCursorPos.x,
                                         pszInsertStr, nPos, y, x, CE_ACTION_AUTOINDENT);
              CEPoint pt (x, y);
              SetSelection (pt, pt);
              SetAnchor (pt);
              SetCursorPos (pt);
              EnsureVisible (pt);
              // m_pTextBuffer->FlushUndoGroup (this);
            }
          else
            {
              //  Insert part of the previous line
              if ((GetFlags () & (SRCOPT_BRACEGNU|SRCOPT_BRACEANSI)) && isopenbrace (pszLineChars[nLength - 1]))
                {
                  tchar_t *pszInsertStr;
                  if (m_pTextBuffer->GetInsertTabs())
                    {
                      pszInsertStr = (tchar_t *) _alloca (sizeof (tchar_t) * 2);
                      pszInsertStr[nPos++] = _T ('\t');
                    }
                  else
                    {
                      int nTabSize = GetTabSize ();
                      int nChars = nTabSize - nPos % nTabSize;
                      pszInsertStr = (tchar_t *) _alloca (sizeof (tchar_t) * (nChars + 1));
                      while (nChars--)
                        {
                          pszInsertStr[nPos++] = _T (' ');
                        }
                    }
                  pszInsertStr[nPos] = 0;

                  // m_pTextBuffer->BeginUndoGroup ();
                  int x, y;
                  m_pTextBuffer->InsertText (nullptr, ptCursorPos.y, ptCursorPos.x,
                                             pszInsertStr, nPos, y, x, CE_ACTION_AUTOINDENT);
                  CEPoint pt (x, y);
                  SetSelection (pt, pt);
                  SetAnchor (pt);
                  SetCursorPos (pt);
                  EnsureVisible (pt);
                  // m_pTextBuffer->FlushUndoGroup (this);
                }
            }
        }
      else if (nAction == CE_ACTION_TYPING && (GetFlags () & SRCOPT_FNBRACE) && bracetype (pszText) == 3)
        {
          //  Enter stroke!
          CEPoint ptCursorPos = GetCursorPos ();
          const tchar_t* pszChars = m_pTextBuffer->GetLineChars (ptCursorPos.y);
          if (ptCursorPos.x > 1 && xisalnum (pszChars[ptCursorPos.x - 2]))
            {
              tchar_t* pszInsertStr = (tchar_t *) _alloca (sizeof (tchar_t) * 2);
              *pszInsertStr = _T (' ');
              pszInsertStr[1] = _T ('\0');
              // m_pTextBuffer->BeginUndoGroup ();
              int x, y;
              m_pTextBuffer->InsertText (nullptr, ptCursorPos.y, ptCursorPos.x - 1,
                                         pszInsertStr, 1, y, x, CE_ACTION_AUTOINDENT);
              ptCursorPos.x = x + 1;
              ptCursorPos.y = y;
              SetSelection (ptCursorPos, ptCursorPos);
              SetAnchor (ptCursorPos);
              SetCursorPos (ptCursorPos);
              EnsureVisible (ptCursorPos);
              // m_pTextBuffer->FlushUndoGroup (this);
            }
        }
      else if (nAction == CE_ACTION_TYPING && (GetFlags () & SRCOPT_BRACEGNU) && isopenbrace (pszText))
        {
          //  Enter stroke!
          CEPoint ptCursorPos = GetCursorPos ();

          //  Take indentation from the previous line
          int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y);
          const tchar_t* pszLineChars = m_pTextBuffer->GetLineChars (ptCursorPos.y );
          int nPos = 0;
          while (nPos < nLength && xisspace (pszLineChars[nPos]))
            nPos++;
          if (nPos == nLength - 1)
            {
              tchar_t *pszInsertStr;
              if (m_pTextBuffer->GetInsertTabs())
                {
                  pszInsertStr = (tchar_t *) _alloca (sizeof (tchar_t) * 2);
                  *pszInsertStr = _T ('\t');
                  nPos = 1;
                }
              else
                {
                  int nTabSize = GetTabSize ();
                  int nChars = nTabSize - nPos % nTabSize;
                  pszInsertStr = (tchar_t *) _alloca (sizeof (tchar_t) * (nChars + 1));
                  nPos = 0;
                  while (nChars--)
                    {
                      pszInsertStr[nPos++] = _T (' ');
                    }
                }
              pszInsertStr[nPos] = 0;

              // m_pTextBuffer->BeginUndoGroup ();
              int x, y;
              m_pTextBuffer->InsertText (nullptr, ptCursorPos.y, ptCursorPos.x - 1,
                                         pszInsertStr, nPos, y, x, CE_ACTION_AUTOINDENT);
              CEPoint pt (x + 1, y);
              SetSelection (pt, pt);
              SetAnchor (pt);
              SetCursorPos (pt);
              EnsureVisible (pt);
              // m_pTextBuffer->FlushUndoGroup (this);
            }
        }
      else if (nAction == CE_ACTION_TYPING && (GetFlags () & (SRCOPT_BRACEGNU|SRCOPT_BRACEANSI)) && isclosebrace (pszText))
        {
          //  Enter stroke!
          CEPoint ptCursorPos = GetCursorPos ();

          //  Take indentation from the previous line
          int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y);
          const tchar_t* pszLineChars = m_pTextBuffer->GetLineChars (ptCursorPos.y );
          int nPos = 0;
          while (nPos < nLength && xisspace (pszLineChars[nPos]))
            nPos++;
          if (ptCursorPos.y > 0 && nPos && nPos == nLength - 1)
            {
              if (pszLineChars[nPos - 1] == _T ('\t'))
                {
                  nPos = 1;
                }
              else
                {
                  int nTabSize = GetTabSize ();
                  nPos = nTabSize - (ptCursorPos.x - 1) % nTabSize;
                  if (!nPos)
                    {
                      nPos = nTabSize;
                    }
                  if (nPos > nLength - 1)
                    {
                      nPos = nLength - 1;
                    }
                }
              // m_pTextBuffer->BeginUndoGroup ();
              m_pTextBuffer->DeleteText (nullptr, ptCursorPos.y, ptCursorPos.x - nPos - 1,
                ptCursorPos.y, ptCursorPos.x - 1, CE_ACTION_AUTOINDENT);
              ptCursorPos.x -= nPos;
              SetCursorPos (ptCursorPos);
              SetSelection (ptCursorPos, ptCursorPos);
              SetAnchor (ptCursorPos);
              EnsureVisible (ptCursorPos);
              // m_pTextBuffer->FlushUndoGroup (this);
            }
        }
    }
}

void CCrystalEditView::
OnEditAutoComplete ()
{
  CEPoint ptCursorPos = GetCursorPos ();
  int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y);
  const tchar_t* pszText = m_pTextBuffer->GetLineChars (ptCursorPos.y), *pszEnd = pszText + ptCursorPos.x;
  if (ptCursorPos.x > 0 && ptCursorPos.y > 0 && (nLength == ptCursorPos.x || !xisalnum (*pszEnd)) && xisalnum (pszEnd[-1]))
    {
      const tchar_t* pszBegin = pszEnd - 1;
      while (pszBegin > pszText && xisalnum (*pszBegin))
        pszBegin--;
      if (!xisalnum (*pszBegin))
        pszBegin++;
      nLength = static_cast<int>(pszEnd - pszBegin);
      CString sText;
      tchar_t* pszBuffer = sText.GetBuffer (nLength + 2);
      *pszBuffer = _T('<');
      _tcsncpy_s (pszBuffer + 1, nLength + 1, pszBegin, nLength);
      sText.ReleaseBuffer (nLength + 1);
      CEPoint ptTextPos;
      ptCursorPos.x -= nLength;
      bool bFound = FindText (sText, ptCursorPos, FIND_MATCH_CASE|FIND_REGEXP|FIND_DIRECTION_UP, true, &ptTextPos);
      if (!bFound)
        {
          ptCursorPos.x += nLength;
          bFound = FindText (sText, ptCursorPos, FIND_MATCH_CASE|FIND_REGEXP, true, &ptTextPos);
          ptCursorPos.x -= nLength;
        }
      if (bFound)
        {
          const int nLineLength = m_pTextBuffer->GetLineLength (ptTextPos.y);
          int nFound = nLineLength;
          pszText = m_pTextBuffer->GetLineChars (ptTextPos.y) + ptTextPos.x + m_nLastFindWhatLen;
          nFound -= ptTextPos.x + m_nLastFindWhatLen;
          pszBuffer = sText.GetBuffer (nFound + 1);
          while (nFound-- && xisalnum (*pszText))
            *pszBuffer++ = *pszText++;
          sText.ReleaseBuffer (nLineLength - (ptTextPos.x + m_nLastFindWhatLen) - nFound - 1);
          if (!sText.IsEmpty ())
            {
              m_pTextBuffer->BeginUndoGroup ();
              int x, y;
              m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x + nLength, sText, sText.GetLength(), y, x, CE_ACTION_AUTOCOMPLETE);
              ptCursorPos.x = x;
              ptCursorPos.y = y;
              SetCursorPos (ptCursorPos);
              SetSelection (ptCursorPos, ptCursorPos);
              SetAnchor (ptCursorPos);
              EnsureVisible (ptCursorPos);
              m_pTextBuffer->FlushUndoGroup (this);
            }
        }
    }
}

void CCrystalEditView::
OnUpdateEditAutoComplete (CCmdUI * pCmdUI)
{
  CEPoint ptCursorPos = GetCursorPos ();
  int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y);
  const tchar_t* pszText = m_pTextBuffer->GetLineChars (ptCursorPos.y) + ptCursorPos.x;
  pCmdUI->Enable (ptCursorPos.x > 0 && ptCursorPos.y > 0 && (nLength == ptCursorPos.x || !xisalnum (*pszText)) && xisalnum (pszText[-1]));
}

void CCrystalEditView::
OnUpdateEditLowerCase (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (IsSelection ());
}

void CCrystalEditView::
OnEditLowerCase ()
{
  if (IsSelection ())
    {
      CEPoint ptCursorPos = GetCursorPos ();
      auto [ptSelStart, ptSelEnd] = GetSelection ();
      CString text;
      GetText (ptSelStart, ptSelEnd, text);
      text.MakeLower ();

      m_pTextBuffer->BeginUndoGroup ();

      if (IsSelection ())
        {
          auto [ptSelStart1, ptSelEnd1] = GetSelection ();
    
          ptCursorPos = ptSelStart1;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/
    
          // [JRT]:
          m_pTextBuffer->DeleteText (this, ptSelStart1.y, ptSelStart1.x, ptSelEnd1.y, ptSelEnd1.x, CE_ACTION_LOWERCASE);
        }

      int x, y;
      m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, text, text.GetLength(), y, x, CE_ACTION_LOWERCASE);

      ASSERT_VALIDTEXTPOS (ptCursorPos);
      SetAnchor (ptCursorPos);
      SetSelection (ptSelStart, ptSelEnd);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);

      m_pTextBuffer->FlushUndoGroup (this);
    }
}

void CCrystalEditView::
OnUpdateEditUpperCase (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (IsSelection ());
}

void CCrystalEditView::
OnEditUpperCase ()
{
  if (IsSelection ())
    {
      CEPoint ptCursorPos = GetCursorPos ();
      auto [ptSelStart, ptSelEnd] = GetSelection ();
      CString text;
      GetText (ptSelStart, ptSelEnd, text);
      text.MakeUpper ();

      m_pTextBuffer->BeginUndoGroup ();

      if (IsSelection ())
        {
          auto [ptSelStart1, ptSelEnd1] = GetSelection ();
    
          ptCursorPos = ptSelStart1;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/
    
          // [JRT]:
          m_pTextBuffer->DeleteText (this, ptSelStart1.y, ptSelStart1.x, ptSelEnd1.y, ptSelEnd1.x, CE_ACTION_UPPERCASE);
        }

      int x, y;
      m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, text, text.GetLength(), y, x, CE_ACTION_UPPERCASE);

      ASSERT_VALIDTEXTPOS (ptCursorPos);
      SetAnchor (ptCursorPos);
      SetSelection (ptSelStart, ptSelEnd);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);

      m_pTextBuffer->FlushUndoGroup (this);
    }
}

void CCrystalEditView::
OnUpdateEditSwapCase (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (IsSelection ());
}

void CCrystalEditView::
OnEditSwapCase ()
{
  if (IsSelection ())
    {
      CEPoint ptCursorPos = GetCursorPos ();
      auto [ptSelStart, ptSelEnd] = GetSelection ();
      CString text;
      GetText (ptSelStart, ptSelEnd, text);
      int nLen = text.GetLength ();
      tchar_t* pszText = text.GetBuffer (nLen + 1);
      while (*pszText)
        *pszText++ = (tchar_t)(tc::istlower (*pszText) ? tc::totupper (*pszText) : tc::totlower (*pszText));
      text.ReleaseBuffer (nLen);

      m_pTextBuffer->BeginUndoGroup ();

      if (IsSelection ())
        {
          auto [ptSelStart1, ptSelEnd1] = GetSelection ();
    
          ptCursorPos = ptSelStart1;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/
    
          // [JRT]:
          m_pTextBuffer->DeleteText (this, ptSelStart1.y, ptSelStart1.x, ptSelEnd1.y, ptSelEnd1.x, CE_ACTION_SWAPCASE);
        }

      int x, y;
      m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, text, text.GetLength(), y, x, CE_ACTION_SWAPCASE);

      ASSERT_VALIDTEXTPOS (ptCursorPos);
      SetAnchor (ptCursorPos);
      SetSelection (ptSelStart, ptSelEnd);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);

      m_pTextBuffer->FlushUndoGroup (this);
    }
}

void CCrystalEditView::
OnUpdateEditCapitalize (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (IsSelection ());
}

void CCrystalEditView::
OnEditCapitalize ()
{
  if (IsSelection ())
    {
      CEPoint ptCursorPos = GetCursorPos ();
      auto [ptSelStart, ptSelEnd] = GetSelection ();
      CString text;
      GetText (ptSelStart, ptSelEnd, text);
      int nLen = text.GetLength ();
      tchar_t* pszText = text.GetBuffer (nLen + 1);
      bool bCapitalize = true;
      while (*pszText)
        {
          if (xisspace (*pszText))
            bCapitalize = true;
          else if (_istalpha (*pszText))
            if (bCapitalize)
              {
                *pszText = (tchar_t)tc::totupper (*pszText);
                bCapitalize = false;
              }
            else
              *pszText = (tchar_t)tc::totlower (*pszText);
          pszText++;
        }
      text.ReleaseBuffer (nLen);

      m_pTextBuffer->BeginUndoGroup ();

      if (IsSelection ())
        {
          auto [ptSelStart1, ptSelEnd1] = GetSelection ();
    
          ptCursorPos = ptSelStart1;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/
    
          // [JRT]:
          m_pTextBuffer->DeleteText (this, ptSelStart1.y, ptSelStart1.x, ptSelEnd1.y, ptSelEnd1.x, CE_ACTION_CAPITALIZE);
        }

      int x, y;
      m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, text, text.GetLength(), y, x, CE_ACTION_CAPITALIZE);

      ASSERT_VALIDTEXTPOS (ptCursorPos);
      SetAnchor (ptCursorPos);
      SetSelection (ptSelStart, ptSelEnd);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);

      m_pTextBuffer->FlushUndoGroup (this);
    }
}

void CCrystalEditView::
OnUpdateEditSentence (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (IsSelection ());
}

void CCrystalEditView::
OnEditSentence ()
{
  if (IsSelection ())
    {
      CEPoint ptCursorPos = GetCursorPos ();
      auto [ptSelStart, ptSelEnd] = GetSelection ();
      CString text;
      GetText (ptSelStart, ptSelEnd, text);
      int nLen = text.GetLength ();
      tchar_t* pszText = text.GetBuffer (nLen + 1);
      bool bCapitalize = true;
      while (*pszText)
        {
          if (!xisspace (*pszText))
            if (*pszText == _T ('.'))
              {
                if (pszText[1] && !tc::istdigit (pszText[1]))
                  bCapitalize = true;
              }
            else if (_istalpha (*pszText))
              if (bCapitalize)
                {
                  *pszText = (tchar_t)tc::totupper (*pszText);
                  bCapitalize = false;
                }
              else
                *pszText = (tchar_t)tc::totlower (*pszText);
          pszText++;
        }
      text.ReleaseBuffer (nLen);

      m_pTextBuffer->BeginUndoGroup ();

      if (IsSelection ())
        {
          auto [ptSelStart1, ptSelEnd1] = GetSelection ();
    
          ptCursorPos = ptSelStart1;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/
    
          // [JRT]:
          m_pTextBuffer->DeleteText (this, ptSelStart1.y, ptSelStart1.x, ptSelEnd1.y, ptSelEnd1.x, CE_ACTION_SENTENCIZE);
        }

      int x, y;
      m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, text, text.GetLength(), y, x, CE_ACTION_SENTENCIZE);

      ASSERT_VALIDTEXTPOS (ptCursorPos);
      SetAnchor (ptCursorPos);
      SetSelection (ptSelStart, ptSelEnd);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);

      m_pTextBuffer->FlushUndoGroup (this);
    }
}

//BEGIN SW
void CCrystalEditView::OnUpdateEditGotoLastChange( CCmdUI *pCmdUI )
{
  CEPoint	ptLastChange = m_pTextBuffer->GetLastChangePos();
  pCmdUI->Enable( ptLastChange.x > 0 && ptLastChange.y > -1 );
}

void CCrystalEditView::OnEditGotoLastChange()
{
  CEPoint	ptLastChange = m_pTextBuffer->GetLastChangePos();
  if( ptLastChange.x < 0 || ptLastChange.y < 0 )
    return;

  // goto last change
  SetSelection( ptLastChange, ptLastChange );
  SetCursorPos( ptLastChange );
  EnsureVisible( ptLastChange );
}
//END SW

int CCrystalEditView::SpellGetLine (struct SpellData_t *pdata)
{
  CCrystalEditView *pView = static_cast<CCrystalEditView*>(pdata->pUserData);
  static tchar_t szBuffer[4096];

  if (pdata->nRow < pView->GetLineCount ())
    {
      int nCount = pView->GetLineLength (pdata->nRow) + 1;
      /*if (pdata->pszBuffer)
        free (pdata->pszBuffer);
      pdata->pszBuffer = (tchar_t*) malloc (nCount + 2);*/
      pdata->pszBuffer = szBuffer;
      *pdata->pszBuffer = _T ('^');
      if (nCount > 1)
        tc::tcslcpy (pdata->pszBuffer + 1, sizeof(szBuffer)-1, pView->GetLineChars (pdata->nRow));
      else
        pdata->pszBuffer[nCount++] = _T (' ');
      pdata->pszBuffer[nCount++] = _T ('\n');
      pdata->pszBuffer[nCount] = _T ('\0');
      pdata->nRow++;
      return nCount;
    }
  pdata->pszBuffer = szBuffer;
  *pdata->pszBuffer = _T ('\0');
  return 0;
}

int CCrystalEditView::SpellNotify (int nEvent, struct SpellData_t *pdata)
{
  CCrystalEditView *pView = static_cast<CCrystalEditView*>(pdata->pUserData);
  CEPoint ptStartPos, ptEndPos;

  switch (nEvent)
    {
      case SN_FOUND:
        ptStartPos.x = pdata->nColumn - 1;
        ptStartPos.y = pdata->nRow - 1;
        ptEndPos.x = pdata->nColumn - 1 + static_cast<LONG>(tc::tcslen (pdata->pszWord));
        ptEndPos.y = pdata->nRow - 1;
        if (!pView->IsValidTextPos (ptStartPos))
          if (ptStartPos.x > 0)
            ptStartPos.x--;
          else
            ptStartPos.x = 0;
        if (!pView->IsValidTextPos (ptEndPos))
          if (ptEndPos.x > 0)
            ptEndPos.x--;
          else
            ptEndPos.x = 0;
        pView->SetAnchor (ptStartPos);
        pView->SetSelection (ptStartPos, ptEndPos);
        pView->SetCursorPos (ptStartPos);
        pView->EnsureVisible (ptStartPos);
      break;
      case SN_REPLACED:
        if (pView->IsSelection ())
          {
            int x, y;
            pView->GetSelection (ptStartPos, ptEndPos);
            pView->m_pTextBuffer->DeleteText (pView, ptStartPos.y, ptStartPos.x, ptEndPos.y, ptEndPos.x, CE_ACTION_SPELL);
            pView->m_pTextBuffer->InsertText (pView, ptStartPos.y, ptStartPos.x, pdata->pszWord, tc::tcslen(pdata->pszWord), y, x, CE_ACTION_SPELL);
            ptEndPos.x = x;
            ptEndPos.y = y;
            pView->SetAnchor (ptEndPos);
            pView->SetSelection (ptEndPos, ptEndPos);
            pView->SetCursorPos (ptEndPos);
          }
      break;
      case SN_FINISHED:
        ptStartPos = pView->GetCursorPos ();
        pView->SetAnchor (ptStartPos);
        pView->SetSelection (ptStartPos, ptStartPos);
        pView->SetCursorPos (ptStartPos);
        pView->EnsureVisible (ptStartPos);
        ::MessageBox (pdata->hParent, _T ("Spellchecking finished."), _T ("WIspell"), MB_OK|MB_ICONINFORMATION);
    }
  return 0;
}

HMODULE CCrystalEditView::hSpellDll = nullptr;
tchar_t CCrystalEditView::szWIspellPath[_MAX_PATH];
SpellData CCrystalEditView::spellData;
int (*CCrystalEditView::SpellInit) (SpellData*);
int (*CCrystalEditView::SpellCheck) (SpellData*);
int (*CCrystalEditView::SpellConfig) (SpellData*);

bool CCrystalEditView::LoadSpellDll (bool bAlert /*= true*/)
{
  if (hSpellDll != nullptr)
    return true;
  CString sPath = szWIspellPath;
  if (!sPath.IsEmpty () && sPath[sPath.GetLength () - 1] != _T('\\'))
    sPath += _T ('\\');
  sPath += _T ("wispell.dll");
  hSpellDll = LoadLibrary (sPath);
  if (hSpellDll != nullptr)
    {
      SpellInit = (int (*) (SpellData*)) GetProcAddress (hSpellDll, "SpellInit");
      SpellCheck = (int (*) (SpellData*)) GetProcAddress (hSpellDll, "SpellCheck");
      SpellConfig = (int (*) (SpellData*)) GetProcAddress (hSpellDll, "SpellConfig");
      if (SpellInit)
        SpellInit (&spellData);
      tc::tcslcpy (spellData.szIspell, szWIspellPath);
      spellData.GetLine = SpellGetLine;
      spellData.Notify = SpellNotify;
    }
  else
    {
      SpellInit = SpellCheck = SpellConfig = nullptr;
      if (bAlert)
        ::MessageBox (AfxGetMainWnd ()->GetSafeHwnd (), _T ("Error loading \"wispell.dll\"."), _T ("Error"), MB_OK|MB_ICONEXCLAMATION);
    }
  return hSpellDll != nullptr;
}

void CCrystalEditView::
OnUpdateToolsSpelling (CCmdUI * pCmdUI)
{
  CEPoint ptCursorPos = GetCursorPos ();
  int nLines = GetLineCount () - 1;
  pCmdUI->Enable (LoadSpellDll (false) && ptCursorPos.y < nLines);
}

void CCrystalEditView::
OnToolsSpelling ()
{
  CEPoint ptCursorPos = GetCursorPos ();
  if (LoadSpellDll () && ptCursorPos.y < GetLineCount ())
    {
      spellData.hParent = GetSafeHwnd ();
      spellData.nRow = ptCursorPos.y;
      spellData.pUserData = (LPVOID) (LPCVOID) this;
      spellData.pszBuffer = nullptr;
      m_pTextBuffer->BeginUndoGroup ();
      if (SpellCheck (&spellData) == IDCANCEL)
        {
          m_pTextBuffer->FlushUndoGroup (this);
          OnEditUndo ();
        }
      else
        m_pTextBuffer->FlushUndoGroup (this);
      /*if (spellData.pszBuffer)
        free (spellData.pszBuffer);*/
    }
}

void CCrystalEditView::
OnUpdateToolsCharCoding (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (IsSelection ());
}

void CCrystalEditView::
OnToolsCharCoding ()
{
  if (IsSelection ())
    {
      CWaitCursor wait;
      CEPoint ptCursorPos = GetCursorPos ();
      auto [ptSelStart, ptSelEnd] = GetSelection ();
      CString sText;
      GetText (ptSelStart, ptSelEnd, sText);
      CCharConvDlg dlg;
      dlg.m_sOriginal = sText;
      tchar_t* pszEnd = dlg.m_sOriginal.GetBuffer (dlg.m_sOriginal.GetLength () + 1);
      for (int i = 0; i < 13; i++)
        {
          pszEnd = tc::tcschr (pszEnd, _T ('\n'));
          if (pszEnd != nullptr)
            pszEnd++;
          else
            break;
        }
      if (pszEnd != nullptr)
        *pszEnd = _T ('\0');
      dlg.m_sOriginal.ReleaseBuffer ();
      if (dlg.DoModal () != IDOK)
        return;
      tchar_t* pszNew = nullptr;
      if (!iconvert_new (sText, &pszNew, dlg.m_nSource, dlg.m_nDest, dlg.m_bAlpha))
        {
          ASSERT (pszNew != nullptr);
          m_pTextBuffer->BeginUndoGroup ();

          m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_RECODE);

          int x, y;
          m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, pszNew, tc::tcslen(pszNew), y, x, CE_ACTION_RECODE);

          if (IsValidTextPos (ptCursorPos))
            ptCursorPos.x = 0;
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          SetAnchor (ptCursorPos);
          SetSelection (ptSelStart, ptSelEnd);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);

          m_pTextBuffer->FlushUndoGroup (this);
        }
      if (pszNew != nullptr)
        free (pszNew);
    }
}

void CCrystalEditView::
OnEditDeleteWord ()
{
  if (!IsSelection ())
    MoveWordRight (true);
  if (IsSelection ())
    {
      auto [ptSelStart, ptSelEnd] = GetSelection ();

      m_pTextBuffer->BeginUndoGroup ();

      m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_DELETE);

      ASSERT_VALIDTEXTPOS (ptSelStart);
      SetAnchor (ptSelStart);
      SetSelection (ptSelStart, ptSelStart);
      SetCursorPos (ptSelStart);
      EnsureVisible (ptSelStart);

      m_pTextBuffer->FlushUndoGroup (this);
    }
}

void CCrystalEditView::
OnEditDeleteWordBack ()
{
  if (!IsSelection ())
    MoveWordLeft (true);
  if (IsSelection ())
    {
      auto [ptSelStart, ptSelEnd] = GetSelection ();

      m_pTextBuffer->BeginUndoGroup ();

      m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_DELETE);

      ASSERT_VALIDTEXTPOS (ptSelStart);
      SetAnchor (ptSelStart);
      SetSelection (ptSelStart, ptSelStart);
      SetCursorPos (ptSelStart);
      EnsureVisible (ptSelStart);

      m_pTextBuffer->FlushUndoGroup (this);
    }
}

void CCrystalEditView::
OnKillFocus (CWnd * pNewWnd)
{
  m_bMergeUndo = false;
  CCrystalTextView::OnKillFocus (pNewWnd);
}

void CCrystalEditView::OnCharLeft()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnCharLeft();
}

void CCrystalEditView::OnExtCharLeft()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtCharLeft();
}

void CCrystalEditView::OnCharRight()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnCharRight();
}

void CCrystalEditView::OnExtCharRight()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtCharRight();
}

void CCrystalEditView::OnWordLeft()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnWordLeft();
}

void CCrystalEditView::OnExtWordLeft()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtWordLeft();
}

void CCrystalEditView::OnWordRight()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnWordRight();
}

void CCrystalEditView::OnExtWordRight()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtWordRight();
}

void CCrystalEditView::OnLineUp()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnLineUp();
}

void CCrystalEditView::OnExtLineUp()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtLineUp();
}

void CCrystalEditView::OnLineDown()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnLineDown();
}

void CCrystalEditView::OnExtLineDown()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtLineDown();
}

void CCrystalEditView::OnPageUp()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnPageUp();
}

void CCrystalEditView::OnExtPageUp()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtPageUp();
}

void CCrystalEditView::OnPageDown()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnPageDown();
}

void CCrystalEditView::OnExtPageDown()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtPageDown();
}

void CCrystalEditView::OnLineEnd()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnLineEnd();
}

void CCrystalEditView::OnExtLineEnd()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtLineEnd();
}

void CCrystalEditView::OnHome()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnHome();
}

void CCrystalEditView::OnExtHome()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtHome();
}

void CCrystalEditView::OnTextBegin()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnTextBegin();
}

void CCrystalEditView::OnExtTextBegin()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtTextBegin();
}

void CCrystalEditView::OnTextEnd()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnTextEnd();
}

void CCrystalEditView::OnExtTextEnd()
{
  m_bMergeUndo = false;
  CCrystalTextView::OnExtTextEnd();
}

void CCrystalEditView::OnLButtonDown(UINT nFlags, CPoint point)
{
  m_bMergeUndo = false;
  CCrystalTextView::OnLButtonDown(nFlags, point);
}

void CCrystalEditView::OnRButtonDown(UINT nFlags, CPoint point)
{
  m_bMergeUndo = false;
  CCrystalTextView::OnRButtonDown(nFlags, point);
}

////////////////////////////////////////////////////////////////////////////
