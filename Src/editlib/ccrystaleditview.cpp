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
//	??-Aug-99
//		Sven Wiegand (search for "//BEGIN SW" to find my changes):
//	+ FEATURE: "Go to last change" (sets the cursor on the position where
//			the user did his last edit actions
//	+ FEATURE: Support for incremental search in CCrystalTextView
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//	24-Oct-99
//		Sven Wiegand
//	+ FEATURE: Supporting [Return] to exit incremental-search-mode
//		     (see OnChar())
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editcmd.h"
#include "editreg.h"
#include "ccrystaleditview.h"
#include "ccrystaltextbuffer.h"
#include "ceditreplacedlg.h"
#include "registry.h"
#include "cs2cs.h"
#include "chcondlg.h"

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
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DRAG_BORDER_X       5
#define DRAG_BORDER_Y       5

/////////////////////////////////////////////////////////////////////////////
// CEditDropTargetImpl class declaration

class CEditDropTargetImpl : public COleDropTarget
  {
private :
    CCrystalEditView * m_pOwner;
public :
    CEditDropTargetImpl (CCrystalEditView * pOwner)
    {
      m_pOwner = pOwner;
    };

    virtual DROPEFFECT OnDragEnter (CWnd * pWnd, COleDataObject * pDataObject, DWORD dwKeyState, CPoint point);
    virtual void OnDragLeave (CWnd * pWnd);
    virtual DROPEFFECT OnDragOver (CWnd * pWnd, COleDataObject * pDataObject, DWORD dwKeyState, CPoint point);
    virtual BOOL OnDrop (CWnd * pWnd, COleDataObject * pDataObject, DROPEFFECT dropEffect, CPoint point);
    virtual DROPEFFECT OnDragScroll (CWnd * pWnd, DWORD dwKeyState, CPoint point);
  };


#pragma warning ( disable : 4100 )
/////////////////////////////////////////////////////////////////////////////
// CCrystalEditView

IMPLEMENT_DYNCREATE (CCrystalEditView, CCrystalTextView)

CCrystalEditView::CCrystalEditView ()
{
  AFX_ZERO_INIT_OBJECT (CCrystalTextView);
  m_bInsertTabs = TRUE;
  m_bAutoIndent = TRUE;
  m_mapExpand = new CMap<CString, LPCTSTR, CString, LPCTSTR> (10);
}

CCrystalEditView:: ~ CCrystalEditView ()
{
  delete m_mapExpand;
}

bool CCrystalEditView::
DoSetTextType (TextDefinition *def)
{
  m_CurSourceDef = def;
  SetInsertTabs ((def->flags & SRCOPT_INSERTTABS) != FALSE);
  SetAutoIndent ((def->flags & SRCOPT_AUTOINDENT) != FALSE);
  SetDisableBSAtSOL ((def->flags & SRCOPT_BSATBOL) == FALSE);
  m_mapExpand->RemoveAll ();
  CReg reg;
  CString sKey = REG_EDITPAD _T("\\");
  sKey += def->name;
  sKey += _T ("\\Expand");
  if (reg.Open (HKEY_CURRENT_USER, sKey, KEY_READ))
    {
      LPCTSTR pszValue;
      RegVal regVal;
      if (reg.FindFirstValue (pszValue, &regVal))
        {
          CString sData;
          do
            if (RegValGetString (&regVal, sData))
              m_mapExpand->SetAt (pszValue, sData);
          while (reg.FindNextValue (pszValue, &regVal));
        }
      reg.FindClose ();
    }
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
ON_UPDATE_COMMAND_UI (ID_EDIT_AUTOEXPAND, OnUpdateEditAutoExpand)
ON_COMMAND (ID_EDIT_AUTOEXPAND, OnEditAutoExpand)
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
//}}AFX_MSG_MAP
ON_UPDATE_COMMAND_UI (ID_EDIT_INDICATOR_READ, OnUpdateIndicatorRead)
ON_UPDATE_COMMAND_UI (ID_INDICATOR_OVR, OnUpdateIndicatorOvr)
ON_UPDATE_COMMAND_UI (ID_EDIT_INDICATOR_POSITION, OnUpdateIndicatorPosition)
ON_UPDATE_COMMAND_UI (ID_TOOLS_SPELLING, OnUpdateToolsSpelling)
ON_COMMAND (ID_TOOLS_SPELLING, OnToolsSpelling)
ON_UPDATE_COMMAND_UI (ID_TOOLS_CHARCODING, OnUpdateToolsCharCoding)
ON_COMMAND (ID_TOOLS_CHARCODING, OnToolsCharCoding)
END_MESSAGE_MAP ()


/////////////////////////////////////////////////////////////////////////////
// CCrystalEditView message handlers

void CCrystalEditView::ResetView ()
{
  // m_bAutoIndent = TRUE;
  m_bOvrMode = FALSE;
  m_bLastReplace = FALSE;
  CCrystalTextView::ResetView ();
}

BOOL CCrystalEditView::
QueryEditable ()
{
  if (m_pTextBuffer == NULL)
    return FALSE;
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

BOOL CCrystalEditView::
DeleteCurrentSelection ()
{
  if (IsSelection ())
    {
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);

      CPoint ptCursorPos = ptSelStart;
      ASSERT_VALIDTEXTPOS (ptCursorPos);
      SetAnchor (ptCursorPos);
      SetSelection (ptCursorPos, ptCursorPos);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);

      // [JRT]:
      m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_DELSEL);
      return TRUE;
    }
  return FALSE;
}

void CCrystalEditView::
Paste ()
{
  if (!QueryEditable ())
    return;
  if (m_pTextBuffer == NULL)
    return;

  CString text;
  if (GetFromClipboard (text))
    {
      m_pTextBuffer->BeginUndoGroup ();
    
      CPoint ptCursorPos;
      if (IsSelection ())
        {
          CPoint ptSelStart, ptSelEnd;
          GetSelection (ptSelStart, ptSelEnd);
    
          ptCursorPos = ptSelStart;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/

		  // WINMERGE: clear the winmerge flags so the file saves correctly
		  for (int i=ptSelStart.y; i <= ptSelEnd.y; i++)
			 m_pTextBuffer->SetLineFlag(i, LF_WINMERGE_FLAGS, FALSE, FALSE, FALSE);
    
          // [JRT]:
          m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_PASTE);
        }
      else
	  {
        ptCursorPos = GetCursorPos ();
		  
		// WINMERGE: clear the winmerge flags so the file saves correctly
		m_pTextBuffer->SetLineFlag(ptCursorPos.y, LF_WINMERGE_FLAGS, FALSE, FALSE, FALSE);
	  }
      ASSERT_VALIDTEXTPOS (ptCursorPos);
      
      int x, y;
      m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, text, y, x, CE_ACTION_PASTE);  //  [JRT]

      ptCursorPos.x = x;
      ptCursorPos.y = y;
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
  if (m_pTextBuffer == NULL)
    return;
  if (!IsSelection ())
    return;

  CPoint ptSelStart, ptSelEnd;
  GetSelection (ptSelStart, ptSelEnd);
  CString text;
  GetText (ptSelStart, ptSelEnd, text);
  PutToClipboard (text);

  CPoint ptCursorPos = ptSelStart;
  ASSERT_VALIDTEXTPOS (ptCursorPos);
  SetAnchor (ptCursorPos);
  SetSelection (ptCursorPos, ptCursorPos);
  SetCursorPos (ptCursorPos);
  EnsureVisible (ptCursorPos);

  m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_CUT);  // [JRT]

}

void CCrystalEditView::
OnEditDelete ()
{
  if (!QueryEditable () || m_pTextBuffer == NULL)
    return;

  CPoint ptSelStart, ptSelEnd;
  GetSelection (ptSelStart, ptSelEnd);
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
        ptSelEnd.x++;
    }

  CPoint ptCursorPos = ptSelStart;
  ASSERT_VALIDTEXTPOS (ptCursorPos);
  SetAnchor (ptCursorPos);
  SetSelection (ptCursorPos, ptCursorPos);
  SetCursorPos (ptCursorPos);
  EnsureVisible (ptCursorPos);

  m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_DELETE);   // [JRT]

}

void CCrystalEditView::
OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
{
  //BEGIN SW
  // check if incremental search is active before call to CCrystalTextView::OnChar()
  BOOL bIncrementalSearch = m_bIncrementalSearchForward || m_bIncrementalSearchBackward;
  //END Sw
  CCrystalTextView::OnChar (nChar, nRepCnt, nFlags);
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
          CPoint ptCursorPos = GetCursorPos ();
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

      m_pTextBuffer->BeginUndoGroup ();

      if (QueryEditable () && m_pTextBuffer != NULL)
        {
          CPoint ptCursorPos;
          if (IsSelection ())
            {
              CPoint ptSelStart, ptSelEnd;
              GetSelection (ptSelStart, ptSelEnd);
        
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
          const static TCHAR pszText[3] = _T ("\r\n");

          int x, y;
          m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, pszText, y, x, CE_ACTION_TYPING);  //  [JRT]

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

  if (nChar > 31)
    {
      if (QueryEditable () && m_pTextBuffer != NULL)
        {
          m_pTextBuffer->BeginUndoGroup (nChar != _T (' '));

          CPoint ptSelStart, ptSelEnd;
          GetSelection (ptSelStart, ptSelEnd);
          CPoint ptCursorPos;
          if (ptSelStart != ptSelEnd)
            {
              ptCursorPos = ptSelStart;
              if (IsSelection ())
                {
                  CPoint ptSelStart, ptSelEnd;
                  GetSelection (ptSelStart, ptSelEnd);
            
                  /*SetAnchor (ptCursorPos);
                  SetSelection (ptCursorPos, ptCursorPos);
                  SetCursorPos (ptCursorPos);
                  EnsureVisible (ptCursorPos);*/
            
                  // [JRT]:
                  m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_TYPING);
                }
            }
          else
            {
              ptCursorPos = GetCursorPos ();
              if (m_bOvrMode && ptCursorPos.x < GetLineLength (ptCursorPos.y))
                m_pTextBuffer->DeleteText (this, ptCursorPos.y, ptCursorPos.x, ptCursorPos.y, ptCursorPos.x + 1, CE_ACTION_TYPING);     // [JRT]

            }

          ASSERT_VALIDTEXTPOS (ptCursorPos);

          TCHAR pszText[2];
          pszText[0] = (TCHAR) nChar;
          pszText[1] = 0;

          int x, y;
          m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, pszText, y, x, CE_ACTION_TYPING);    // [JRT]

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

  if (!QueryEditable () || m_pTextBuffer == NULL)
    return;

  CPoint ptCursorPos = GetCursorPos ();
  CPoint ptCurrentCursorPos = ptCursorPos;
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
      ptCursorPos.x--;          // Decrement Position

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
  if (!QueryEditable () || m_pTextBuffer == NULL)
    return;

  BOOL bTabify = FALSE;
  CPoint ptSelStart, ptSelEnd;
  if (IsSelection ())
    {
      GetSelection (ptSelStart, ptSelEnd);
      bTabify = ptSelStart.y != ptSelEnd.y;
    }

  CPoint ptCursorPos = GetCursorPos ();
  ASSERT_VALIDTEXTPOS (ptCursorPos);

  static TCHAR pszText[32];
  if (m_bInsertTabs)
    {
      *pszText = _T ('\t');
      pszText[1] = _T ('\0');
    }
  else
    {
      int nTabSize = GetTabSize ();
      int nChars = nTabSize - ptCursorPos.x % nTabSize;
      memset(pszText, _T(' '), nChars);
      pszText[nChars] = _T ('\0');
    }

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
      else
        nEndLine--;
      SetSelection (ptSelStart, ptSelEnd);
      SetCursorPos (ptSelEnd);
      EnsureVisible (ptSelEnd);

      //  Shift selection to right
      m_bHorzScrollBarLocked = TRUE;
      for (int L = nStartLine; L <= nEndLine; L++)
        {
          int x, y;
          m_pTextBuffer->InsertText (this, L, 0, pszText, y, x, CE_ACTION_INDENT);  //  [JRT]

        }
      m_bHorzScrollBarLocked = FALSE;
      RecalcHorzScrollBar ();

      m_pTextBuffer->FlushUndoGroup (this);
      return;
    }

  if (m_bOvrMode)
    {
      CPoint ptCursorPos = GetCursorPos ();
      ASSERT_VALIDTEXTPOS (ptCursorPos);

      int nLineLength = GetLineLength (ptCursorPos.y);
      LPCTSTR pszLineChars = GetLineChars (ptCursorPos.y);
      if (ptCursorPos.x < nLineLength)
        {
          int nTabSize = GetTabSize ();
          int nChars = nTabSize - CalculateActualOffset (ptCursorPos.y, ptCursorPos.x) % nTabSize;
          ASSERT (nChars > 0 && nChars <= nTabSize);

          while (nChars > 0)
            {
              if (ptCursorPos.x == nLineLength)
                break;
              if (pszLineChars[ptCursorPos.x] == _T ('\t'))
                {
                  ptCursorPos.x++;
                  break;
                }
              ptCursorPos.x++;
              nChars--;
            }
          ASSERT (ptCursorPos.x <= nLineLength);
          ASSERT_VALIDTEXTPOS (ptCursorPos);

          SetSelection (ptCursorPos, ptCursorPos);
          SetAnchor (ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);
          return;
        }
    }

  m_pTextBuffer->BeginUndoGroup ();

  if (IsSelection ())
    {
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);

      /*SetAnchor (ptCursorPos);
      SetSelection (ptCursorPos, ptCursorPos);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);*/

      // [JRT]:
      m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_TYPING);
    }

  int x, y;
  m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, pszText, y, x, CE_ACTION_TYPING);  //  [JRT]

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
  if (!QueryEditable () || m_pTextBuffer == NULL)
    return;

  BOOL bTabify = FALSE;
  CPoint ptSelStart, ptSelEnd;
  if (IsSelection ())
    {
      GetSelection (ptSelStart, ptSelEnd);
      bTabify = ptSelStart.y != ptSelEnd.y;
    }

  if (bTabify)
    {
      m_pTextBuffer->BeginUndoGroup ();

      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);
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
      m_bHorzScrollBarLocked = TRUE;
      for (int L = nStartLine; L <= nEndLine; L++)
        {
          int nLength = GetLineLength (L);
          if (nLength > 0)
            {
              LPCTSTR pszChars = GetLineChars (L);
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
      m_bHorzScrollBarLocked = FALSE;
      RecalcHorzScrollBar ();

      m_pTextBuffer->FlushUndoGroup (this);
    }
  else
    {
      CPoint ptCursorPos = GetCursorPos ();
      ASSERT_VALIDTEXTPOS (ptCursorPos);
      if (ptCursorPos.x > 0)
        {
          int nTabSize = GetTabSize ();
          int nOffset = CalculateActualOffset (ptCursorPos.y, ptCursorPos.x);
          int nNewOffset = nOffset / nTabSize * nTabSize;
          if (nOffset == nNewOffset && nNewOffset > 0)
            nNewOffset -= nTabSize;
          ASSERT (nNewOffset >= 0);

          LPCTSTR pszChars = GetLineChars (ptCursorPos.y);
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
  pCmdUI->Enable (FALSE);
}

void CCrystalEditView::
OnUpdateIndicatorOvr (CCmdUI * pCmdUI)
{
  pCmdUI->Enable (m_bOvrMode);
}

void CCrystalEditView::
OnUpdateIndicatorRead (CCmdUI * pCmdUI)
{
  if (m_pTextBuffer == NULL)
    pCmdUI->Enable (FALSE);
  else
    pCmdUI->Enable (m_pTextBuffer->GetReadOnly ());
}

void CCrystalEditView::
OnEditSwitchOvrmode ()
{
  m_bOvrMode = !m_bOvrMode;
}

void CCrystalEditView::
OnUpdateEditSwitchOvrmode (CCmdUI * pCmdUI)
{
  pCmdUI->SetCheck (m_bOvrMode ? 1 : 0);
}

DROPEFFECT CEditDropTargetImpl::
OnDragEnter (CWnd * pWnd, COleDataObject * pDataObject, DWORD dwKeyState, CPoint point)
{
  if (!pDataObject->IsDataAvailable (CF_TEXT))
    {
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
  /*
     if (! pDataObject->IsDataAvailable(CF_TEXT))
     {
     m_pOwner->HideDropIndicator();
     return DROPEFFECT_NONE;
     }
   */
  //
  // [JRT]
  //
  bool bDataSupported = false;

  if ((!m_pOwner) ||            // If No Owner
        (!(m_pOwner->QueryEditable ())) ||   // Or Not Editable
        (m_pOwner->GetDisableDragAndDrop ()))    // Or Drag And Drop Disabled

    {
      m_pOwner->HideDropIndicator ();   // Hide Drop Caret

      return DROPEFFECT_NONE;   // Return DE_NONE

    }
  //  if ((pDataObject->IsDataAvailable( CF_TEXT ) ) ||       // If Text Available
  //          ( pDataObject -> IsDataAvailable( xxx ) ) ||    // Or xxx Available
  //          ( pDataObject -> IsDataAvailable( yyy ) ) )     // Or yyy Available
  if (pDataObject->IsDataAvailable (CF_TEXT))   // If Text Available

    {
      bDataSupported = true;    // Set Flag

    }
  if (!bDataSupported)          // If No Supported Formats Available

    {
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

  if ((!m_pOwner) ||            // If No Owner
        (!(m_pOwner->QueryEditable ())) ||   // Or Not Editable
        (m_pOwner->GetDisableDragAndDrop ()))    // Or Drag And Drop Disabled

    {
      return DROPEFFECT_NONE;   // Return DE_NONE

    }
  //  if( ( pDataObject -> IsDataAvailable( CF_TEXT ) ) ||    // If Text Available
  //          ( pDataObject -> IsDataAvailable( xxx ) ) ||    // Or xxx Available
  //          ( pDataObject -> IsDataAvailable( yyy ) ) )     // Or yyy Available
  if (pDataObject->IsDataAvailable (CF_TEXT))   // If Text Available

    {
      bDataSupported = true;    // Set Flag

    }
  if (!bDataSupported)          // If No Supported Formats Available

    {
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
	  //UpdateSiblingScrollPos(FALSE);
      return;
    }
  if (point.y >= rcClientRect.bottom - DRAG_BORDER_Y)
    {
      HideDropIndicator ();
      ScrollDown ();
      UpdateWindow ();
      ShowDropIndicator (point);
	  //UpdateSiblingScrollPos(FALSE);
      return;
    }
  if (point.x < rcClientRect.left + GetMarginWidth () + DRAG_BORDER_X)
    {
      HideDropIndicator ();
      ScrollLeft ();
      UpdateWindow ();
      ShowDropIndicator (point);
	  //UpdateSiblingScrollPos(TRUE);
      return;
    }
  if (point.x >= rcClientRect.right - DRAG_BORDER_X)
    {
      HideDropIndicator ();
      ScrollRight ();
      UpdateWindow ();
      ShowDropIndicator (point);
	  //UpdateSiblingScrollPos(TRUE);
      return;
    }
}

BOOL CCrystalEditView::
DoDropText (COleDataObject * pDataObject, const CPoint & ptClient)
{
  HGLOBAL hData = pDataObject->GetGlobalData (CF_TEXT);
  if (hData == NULL)
    return FALSE;

  CPoint ptDropPos = ClientToText (ptClient);
  if (IsDraggingText () && IsInsideSelection (ptDropPos))
    {
      SetAnchor (ptDropPos);
      SetSelection (ptDropPos, ptDropPos);
      SetCursorPos (ptDropPos);
      EnsureVisible (ptDropPos);
      return FALSE;
    }

  LPTSTR pszText = (LPTSTR)::GlobalLock (hData);
  if (pszText == NULL)
    return FALSE;

  int x, y;
  m_pTextBuffer->InsertText (this, ptDropPos.y, ptDropPos.x, pszText, y, x, CE_ACTION_DRAGDROP);  //   [JRT]

  CPoint ptCurPos (x, y);
  ASSERT_VALIDTEXTPOS (ptCurPos);
  SetAnchor (ptDropPos);
  SetSelection (ptDropPos, ptCurPos);
  SetCursorPos (ptCurPos);
  EnsureVisible (ptCurPos);

  ::GlobalUnlock (hData);
  return TRUE;
}

int CCrystalEditView::
OnCreate (LPCREATESTRUCT lpCreateStruct)
{
  if (CCrystalTextView::OnCreate (lpCreateStruct) == -1)
    return -1;

  ASSERT (m_pDropTarget == NULL);
  m_pDropTarget = new CEditDropTargetImpl (this);
  if (!m_pDropTarget->Register (this))
    {
      TRACE0 ("Warning: Unable to register drop target for ccrystaleditview.\n");
      delete m_pDropTarget;
      m_pDropTarget = NULL;
    }

  return 0;
}

void CCrystalEditView::
OnDestroy ()
{
  if (m_pDropTarget != NULL)
    {
      m_pDropTarget->Revoke ();
      delete m_pDropTarget;
      m_pDropTarget = NULL;
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
      m_bDropPosVisible = TRUE;
      ::CreateCaret (m_hWnd, (HBITMAP) 1, 2, GetLineHeight ());
    }
  m_ptDropPos = ClientToText (point);
  if (m_ptDropPos.x >= m_nOffsetChar)
    {
      SetCaretPos (TextToClient (m_ptDropPos));
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
      m_bDropPosVisible = FALSE;
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
UpdateView (CCrystalTextView * pSource, CUpdateContext * pContext, DWORD dwFlags, int nLineIndex /*= -1*/ )
{
  CCrystalTextView::UpdateView (pSource, pContext, dwFlags, nLineIndex);

  if (m_bSelectionPushed && pContext != NULL)
    {
      pContext->RecalcPoint (m_ptSavedSelStart);
      pContext->RecalcPoint (m_ptSavedSelEnd);
      ASSERT_VALIDTEXTPOS (m_ptSavedSelStart);
      ASSERT_VALIDTEXTPOS (m_ptSavedSelEnd);
    }
}

void CCrystalEditView::
OnEditReplace ()
{
  if (!QueryEditable ())
    return;

  CWinApp *pApp = AfxGetApp ();
  ASSERT (pApp != NULL);

  CEditReplaceDlg dlg (this);

  if (m_bLastReplace)
    {
      //  Get the latest Replace parameters
      dlg.m_bMatchCase = (m_dwLastReplaceFlags & FIND_MATCH_CASE) != 0;
      dlg.m_bWholeWord = (m_dwLastReplaceFlags & FIND_WHOLE_WORD) != 0;
      dlg.m_bRegExp = (m_dwLastReplaceFlags & FIND_REGEXP) != 0;
      if (m_pszLastFindWhat != NULL)
        dlg.m_sText = m_pszLastFindWhat;
    }
  else
    {
      DWORD dwFlags;
      if (!RegLoadNumber (HKEY_CURRENT_USER, REG_EDITPAD, _T ("ReplaceFlags"), &dwFlags))
        dwFlags = 0;
      dlg.m_bMatchCase = (dwFlags & FIND_MATCH_CASE) != 0;
      dlg.m_bWholeWord = (dwFlags & FIND_WHOLE_WORD) != 0;
      dlg.m_bRegExp = (dwFlags & FIND_REGEXP) != 0;
      // dlg.m_sText = pApp->GetProfileString (REG_REPLACE_SUBKEY, REG_FIND_WHAT, _T (""));
      // dlg.m_sNewText = pApp->GetProfileString (REG_REPLACE_SUBKEY, REG_REPLACE_WITH, _T (""));
    }

  if (IsSelection ())
    {
      GetSelection (m_ptSavedSelStart, m_ptSavedSelEnd);
      m_bSelectionPushed = TRUE;

      dlg.m_nScope = 0;         //  Replace in current selection

      dlg.m_ptCurrentPos = m_ptSavedSelStart;
      dlg.m_bEnableScopeSelection = TRUE;
      dlg.m_ptBlockBegin = m_ptSavedSelStart;
      dlg.m_ptBlockEnd = m_ptSavedSelEnd;
    }
  else
    {
      dlg.m_nScope = 1;         //  Replace in whole text

      dlg.m_ptCurrentPos = GetCursorPos ();
      dlg.m_bEnableScopeSelection = FALSE;

      CPoint ptCursorPos = GetCursorPos (), ptStart = WordToLeft (ptCursorPos), ptEnd = WordToRight (ptCursorPos);
      if (IsValidTextPos (ptStart) && IsValidTextPos (ptEnd) && ptStart != ptEnd)
        GetText (ptStart, ptEnd, dlg.m_sText);
    }

  //  Execute Replace dialog
  // m_bShowInactiveSelection = TRUE; // FP: removed because I like it
  dlg.DoModal ();
  // m_bShowInactiveSelection = FALSE; // FP: removed because I like it

  //  Save Replace parameters for 'F3' command
  m_bLastReplace = TRUE;
  if (m_pszLastFindWhat != NULL)
    free (m_pszLastFindWhat);
  m_pszLastFindWhat = _tcsdup (dlg.m_sText);
  m_dwLastReplaceFlags = 0;
  if (dlg.m_bMatchCase)
    m_dwLastReplaceFlags |= FIND_MATCH_CASE;
  if (dlg.m_bWholeWord)
    m_dwLastReplaceFlags |= FIND_WHOLE_WORD;
  if (dlg.m_bRegExp)
    m_dwLastReplaceFlags |= FIND_REGEXP;

  //  Restore selection
  if (m_bSelectionPushed)
    {
      SetSelection (m_ptSavedSelStart, m_ptSavedSelEnd);
      m_bSelectionPushed = FALSE;
    }

  //  Save search parameters to registry
  VERIFY (RegSaveNumber (HKEY_CURRENT_USER, REG_EDITPAD, _T ("ReplaceFlags"), m_dwLastReplaceFlags));
  // pApp->WriteProfileString (REG_REPLACE_SUBKEY, REG_FIND_WHAT, dlg.m_sText);
  // pApp->WriteProfileString (REG_REPLACE_SUBKEY, REG_REPLACE_WITH, dlg.m_sNewText);
}

BOOL CCrystalEditView::
ReplaceSelection (LPCTSTR pszNewText, DWORD dwFlags)
{
  if (!pszNewText)
    pszNewText = _T ("");
  /*ASSERT (pszNewText != NULL);
  if (!IsSelection ())
    return FALSE;*/

  CPoint ptCursorPos;
  if (IsSelection ())
    {
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);

      ptCursorPos = ptSelStart;
      /*SetAnchor (ptCursorPos);
      SetSelection (ptCursorPos, ptCursorPos);
      SetCursorPos (ptCursorPos);
      EnsureVisible (ptCursorPos);*/

      // [JRT]:
      m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_REPLACE, FALSE);
    }
  else
    ptCursorPos = GetCursorPos ();
  ASSERT_VALIDTEXTPOS (ptCursorPos);

  int x, y;
  if (dwFlags & FIND_REGEXP)
    {
      LPTSTR lpszNewStr;
      if (m_pszMatched && !RxReplace(pszNewText, m_pszMatched, m_nLastFindWhatLen, m_rxmatch, &lpszNewStr, &m_nLastReplaceLen))
        {
          CString text;
          if (lpszNewStr && m_nLastReplaceLen > 0)
            {
              LPTSTR buf = text.GetBuffer (m_nLastReplaceLen + 1);
              _tcsncpy (buf, lpszNewStr, m_nLastReplaceLen + 1);
              buf [m_nLastReplaceLen] = _T('\0');
              text.ReleaseBuffer ();
            }
          else
            text.Empty ();
          m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, text, y, x, CE_ACTION_REPLACE, FALSE);  //  [JRT+FRD]
          if (lpszNewStr)
            free(lpszNewStr);
        }
    }
  else
    {
      m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, pszNewText, y, x, CE_ACTION_REPLACE, FALSE);  //  [JRT]
      m_nLastReplaceLen = _tcslen (pszNewText);
    }
  CPoint ptEndOfBlock = CPoint (x, y);
  ASSERT_VALIDTEXTPOS (ptCursorPos);
  ASSERT_VALIDTEXTPOS (ptEndOfBlock);
  SetAnchor (ptEndOfBlock);
  SetSelection (ptCursorPos, ptEndOfBlock);
  SetCursorPos (ptEndOfBlock);
  //EnsureVisible (ptEndOfBlock);
  return TRUE;
}

void CCrystalEditView::
OnUpdateEditUndo (CCmdUI * pCmdUI)
{
  BOOL bCanUndo = m_pTextBuffer != NULL && m_pTextBuffer->CanUndo ();
  pCmdUI->Enable (bCanUndo);

  //  Since we need text only for menus...
  if (pCmdUI->m_pMenu != NULL)
    {
      //  Tune up 'resource handle'
      HINSTANCE hOldResHandle = AfxGetResourceHandle ();
      AfxSetResourceHandle (GetResourceHandle ());

      CString menu;
      if (bCanUndo)
        {
          //  Format menu item text using the provided item description
          CString desc;
          m_pTextBuffer->GetUndoDescription (desc);
          menu.Format (IDS_MENU_UNDO_FORMAT, desc);
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
  if (m_pTextBuffer != NULL && m_pTextBuffer->CanUndo ())
    {
      CPoint ptCursorPos;
      if (m_pTextBuffer->Undo (ptCursorPos))
        {
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);
        }
    }
}

//  [JRT]
void CCrystalEditView::
SetDisableBSAtSOL (BOOL bDisableBSAtSOL)
{
  m_bDisableBSAtSOL = bDisableBSAtSOL;
}

void CCrystalEditView::
OnEditRedo ()
{
  if (m_pTextBuffer != NULL && m_pTextBuffer->CanRedo ())
    {
      CPoint ptCursorPos;
      if (m_pTextBuffer->Redo (ptCursorPos))
        {
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);
        }
    }
}

void CCrystalEditView::
OnUpdateEditRedo (CCmdUI * pCmdUI)
{
  BOOL bCanRedo = m_pTextBuffer != NULL && m_pTextBuffer->CanRedo ();
  pCmdUI->Enable (bCanRedo);

  //  Since we need text only for menus...
  if (pCmdUI->m_pMenu != NULL)
    {
      //  Tune up 'resource handle'
      HINSTANCE hOldResHandle = AfxGetResourceHandle ();
      AfxSetResourceHandle (GetResourceHandle ());

      CString menu;
      if (bCanRedo)
        {
          //  Format menu item text using the provided item description
          CString desc;
          m_pTextBuffer->GetRedoDescription (desc);
          menu.Format (IDS_MENU_REDO_FORMAT, desc);
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
isopenbrace (TCHAR c)
{
  return c == _T ('{') || c == _T ('(') || c == _T ('[') || c == _T ('<');
}

bool
isclosebrace (TCHAR c)
{
  return c == _T ('}') || c == _T ('}') || c == _T (']') || c == _T ('>');
}

bool
isopenbrace (LPCTSTR s)
{
  return s[1] == _T ('\0') && isopenbrace (*s);
}

bool
isclosebrace (LPCTSTR s)
{
  return s[1] == _T ('\0') && isclosebrace (*s);
}

int
bracetype (TCHAR c)
{
  static LPCTSTR braces = _T("{}()[]<>");
  LPCTSTR pos = _tcschr (braces, c);
  return pos ? pos - braces + 1 : 0;
}

int
bracetype (LPCTSTR s)
{
  if (s[1])
    return 0;
  return bracetype (*s);
}

void CCrystalEditView::
OnEditOperation (int nAction, LPCTSTR pszText)
{
  if (m_bAutoIndent)
    {
      //  Analyse last action...
      if (nAction == CE_ACTION_TYPING && _tcscmp (pszText, _T ("\r\n")) == 0 && !m_bOvrMode)
        {
          //  Enter stroke!
          CPoint ptCursorPos = GetCursorPos ();
          ASSERT (ptCursorPos.y > 0);

          //  Take indentation from the previos line
          int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y - 1);
          LPCTSTR pszLineChars = m_pTextBuffer->GetLineChars (ptCursorPos.y - 1);
          int nPos = 0;
          while (nPos < nLength && isspace (pszLineChars[nPos]))
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
              //  Insert part of the previos line
              TCHAR *pszInsertStr;
              if ((GetFlags () & (SRCOPT_BRACEGNU|SRCOPT_BRACEANSI)) && isopenbrace (pszLineChars[nLength - 1]))
                {
                  if (m_bInsertTabs)
                    {
                      pszInsertStr = (TCHAR *) _alloca (sizeof (TCHAR) * (nPos + 2));
                      _tcsncpy (pszInsertStr, pszLineChars, nPos);
                      pszInsertStr[nPos++] = _T ('\t');
                    }
                  else
                    {
                      int nTabSize = GetTabSize ();
                      int nChars = nTabSize - nPos % nTabSize;
                      pszInsertStr = (TCHAR *) _alloca (sizeof (TCHAR) * (nPos + nChars));
                      _tcsncpy (pszInsertStr, pszLineChars, nPos);
                      while (nChars--)
                        {
                          pszInsertStr[nPos++] = _T (' ');
                        }
                    }
                }
              else
                {
                  pszInsertStr = (TCHAR *) _alloca (sizeof (TCHAR) * (nPos + 1));
                  _tcsncpy (pszInsertStr, pszLineChars, nPos);
                }
              pszInsertStr[nPos] = 0;

              // m_pTextBuffer->BeginUndoGroup ();
              int x, y;
              m_pTextBuffer->InsertText (NULL, ptCursorPos.y, ptCursorPos.x,
                                         pszInsertStr, y, x, CE_ACTION_AUTOINDENT);
              CPoint pt (x, y);
              SetCursorPos (pt);
              SetSelection (pt, pt);
              SetAnchor (pt);
              EnsureVisible (pt);
              // m_pTextBuffer->FlushUndoGroup (this);
            }
          else
            {
              //  Insert part of the previos line
              TCHAR *pszInsertStr;
              if ((GetFlags () & (SRCOPT_BRACEGNU|SRCOPT_BRACEANSI)) && isopenbrace (pszLineChars[nLength - 1]))
                {
                  if (m_bInsertTabs)
                    {
                      pszInsertStr = (TCHAR *) _alloca (sizeof (TCHAR) * 2);
                      pszInsertStr[nPos++] = _T ('\t');
                    }
                  else
                    {
                      int nTabSize = GetTabSize ();
                      int nChars = nTabSize - nPos % nTabSize;
                      pszInsertStr = (TCHAR *) _alloca (sizeof (TCHAR) * (nChars + 1));
                      while (nChars--)
                        {
                          pszInsertStr[nPos++] = _T (' ');
                        }
                    }
                  pszInsertStr[nPos] = 0;

                  // m_pTextBuffer->BeginUndoGroup ();
                  int x, y;
                  m_pTextBuffer->InsertText (NULL, ptCursorPos.y, ptCursorPos.x,
                                             pszInsertStr, y, x, CE_ACTION_AUTOINDENT);
                  CPoint pt (x, y);
                  SetCursorPos (pt);
                  SetSelection (pt, pt);
                  SetAnchor (pt);
                  EnsureVisible (pt);
                  // m_pTextBuffer->FlushUndoGroup (this);
                }
            }
        }
      else if (nAction == CE_ACTION_TYPING && (GetFlags () & SRCOPT_FNBRACE) && bracetype (pszText) == 3)
        {
          //  Enter stroke!
          CPoint ptCursorPos = GetCursorPos ();
          LPCTSTR pszChars = m_pTextBuffer->GetLineChars (ptCursorPos.y);
          if (ptCursorPos.x > 1 && xisalnum (pszChars[ptCursorPos.x - 2]))
            {
              LPTSTR pszInsertStr = (TCHAR *) _alloca (sizeof (TCHAR) * 2);
              *pszInsertStr = _T (' ');
              pszInsertStr[1] = _T ('\0');
              // m_pTextBuffer->BeginUndoGroup ();
              int x, y;
              m_pTextBuffer->InsertText (NULL, ptCursorPos.y, ptCursorPos.x - 1,
                                         pszInsertStr, y, x, CE_ACTION_AUTOINDENT);
              ptCursorPos.x = x + 1;
              ptCursorPos.y = y;
              SetCursorPos (ptCursorPos);
              SetSelection (ptCursorPos, ptCursorPos);
              SetAnchor (ptCursorPos);
              EnsureVisible (ptCursorPos);
              // m_pTextBuffer->FlushUndoGroup (this);
            }
        }
      else if (nAction == CE_ACTION_TYPING && (GetFlags () & SRCOPT_BRACEGNU) && isopenbrace (pszText))
        {
          //  Enter stroke!
          CPoint ptCursorPos = GetCursorPos ();

          //  Take indentation from the previos line
          int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y);
          LPCTSTR pszLineChars = m_pTextBuffer->GetLineChars (ptCursorPos.y );
          int nPos = 0;
          while (nPos < nLength && isspace (pszLineChars[nPos]))
            nPos++;
          if (nPos == nLength - 1)
            {
              TCHAR *pszInsertStr;
              if (m_bInsertTabs)
                {
                  pszInsertStr = (TCHAR *) _alloca (sizeof (TCHAR) * 2);
                  *pszInsertStr = _T ('\t');
                  nPos = 1;
                }
              else
                {
                  int nTabSize = GetTabSize ();
                  int nChars = nTabSize - nPos % nTabSize;
                  pszInsertStr = (TCHAR *) _alloca (sizeof (TCHAR) * (nChars + 1));
                  nPos = 0;
                  while (nChars--)
                    {
                      pszInsertStr[nPos++] = _T (' ');
                    }
                }
              pszInsertStr[nPos] = 0;

              // m_pTextBuffer->BeginUndoGroup ();
              int x, y;
              m_pTextBuffer->InsertText (NULL, ptCursorPos.y, ptCursorPos.x - 1,
                                         pszInsertStr, y, x, CE_ACTION_AUTOINDENT);
              CPoint pt (x + 1, y);
              SetCursorPos (pt);
              SetSelection (pt, pt);
              SetAnchor (pt);
              EnsureVisible (pt);
              // m_pTextBuffer->FlushUndoGroup (this);
            }
        }
      else if (nAction == CE_ACTION_TYPING && (GetFlags () & (SRCOPT_BRACEGNU|SRCOPT_BRACEANSI)) && isclosebrace (pszText))
        {
          //  Enter stroke!
          CPoint ptCursorPos = GetCursorPos ();

          //  Take indentation from the previos line
          int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y);
          LPCTSTR pszLineChars = m_pTextBuffer->GetLineChars (ptCursorPos.y );
          int nPos = 0;
          while (nPos < nLength && isspace (pszLineChars[nPos]))
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
              m_pTextBuffer->DeleteText (NULL, ptCursorPos.y, ptCursorPos.x - nPos - 1,
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
  CPoint ptCursorPos = GetCursorPos ();
  int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y);
  LPCTSTR pszText = m_pTextBuffer->GetLineChars (ptCursorPos.y), pszEnd = pszText + ptCursorPos.x;
  if (ptCursorPos.x > 0 && ptCursorPos.y > 0 && (nLength == ptCursorPos.x || !xisalnum (*pszEnd)) && xisalnum (pszEnd[-1]))
    {
      LPCTSTR pszBegin = pszEnd - 1;
      while (pszBegin > pszText && xisalnum (*pszBegin))
        pszBegin--;
      if (!xisalnum (*pszBegin))
        pszBegin++;
      nLength = pszEnd - pszBegin;
      CString sText;
      LPTSTR pszBuffer = sText.GetBuffer (nLength + 2);
      *pszBuffer = _T('<');
      _tcsncpy (pszBuffer + 1, pszBegin, nLength);
      pszBuffer[nLength + 1] = _T('\0');
      sText.ReleaseBuffer ();
      CPoint ptTextPos;
      ptCursorPos.x -= nLength;
      BOOL bFound = FindText (sText, ptCursorPos, FIND_MATCH_CASE|FIND_REGEXP|FIND_DIRECTION_UP, TRUE, &ptTextPos);
      if (!bFound)
        {
          ptCursorPos.x += nLength;
          bFound = FindText (sText, ptCursorPos, FIND_MATCH_CASE|FIND_REGEXP, TRUE, &ptTextPos);
          ptCursorPos.x -= nLength;
        }
      if (bFound)
        {
          int nFound = m_pTextBuffer->GetLineLength (ptTextPos.y);
          pszText = m_pTextBuffer->GetLineChars (ptTextPos.y) + ptTextPos.x + m_nLastFindWhatLen;
          nFound -= ptTextPos.x + m_nLastFindWhatLen;
          pszBuffer = sText.GetBuffer (nFound + 1);
          while (nFound-- && xisalnum (*pszText))
            *pszBuffer++ = *pszText++;
          *pszBuffer = _T('\0');
          sText.ReleaseBuffer ();
          if (!sText.IsEmpty ())
            {
              m_pTextBuffer->BeginUndoGroup ();
              int x, y;
              m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x + nLength, sText, y, x, CE_ACTION_AUTOCOMPLETE);
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
  CPoint ptCursorPos = GetCursorPos ();
  int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y);
  LPCTSTR pszText = m_pTextBuffer->GetLineChars (ptCursorPos.y) + ptCursorPos.x;
  pCmdUI->Enable (ptCursorPos.x > 0 && ptCursorPos.y > 0 && (nLength == ptCursorPos.x || !xisalnum (*pszText)) && xisalnum (pszText[-1]));
}

void CCrystalEditView::
OnEditAutoExpand ()
{
  CPoint ptCursorPos = GetCursorPos ();
  int nLength = m_pTextBuffer->GetLineLength (ptCursorPos.y);
  LPCTSTR pszText = m_pTextBuffer->GetLineChars (ptCursorPos.y), pszEnd = pszText + ptCursorPos.x;
  if (ptCursorPos.x > 0 && ptCursorPos.y > 0 && (nLength == ptCursorPos.x || !xisalnum (*pszEnd)) && xisalnum (pszEnd[-1]))
    {
      LPCTSTR pszBegin = pszEnd - 1;
      while (pszBegin > pszText && xisalnum (*pszBegin))
        pszBegin--;
      if (!xisalnum (*pszBegin))
        pszBegin++;
      nLength = pszEnd - pszBegin;
      CString sText, sExpand;
      LPTSTR pszBuffer = sText.GetBuffer (nLength + 1);
      _tcsncpy (pszBuffer, pszBegin, nLength);
      pszBuffer[nLength] = _T('\0');
      sText.ReleaseBuffer ();
      CPoint ptTextPos;
      ptCursorPos.x -= nLength;
      BOOL bFound = m_mapExpand->Lookup (sText, sExpand);
      if (bFound && !sExpand.IsEmpty ())
        {
          m_pTextBuffer->BeginUndoGroup ();
          int x, y;
          m_pTextBuffer->DeleteText (this, ptCursorPos.y, ptCursorPos.x, ptCursorPos.y, ptCursorPos.x + nLength, CE_ACTION_AUTOEXPAND);
          LPTSTR pszExpand = sExpand.GetBuffer (sExpand.GetLength () + 1);
          LPTSTR pszSlash = _tcschr (pszExpand, _T ('\\'));
          if (!pszSlash)
            {
              m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, pszExpand, y, x, CE_ACTION_AUTOEXPAND);
              ptCursorPos.x = x;
              ptCursorPos.y = y;
              ASSERT_VALIDTEXTPOS (ptCursorPos);
              SetCursorPos (ptCursorPos);
              SetSelection (ptCursorPos, ptCursorPos);
              SetAnchor (ptCursorPos);
            }
          else
            {
              *pszSlash++ = _T ('\0');
              for(;;)
                {
                  m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, pszExpand, y, x, CE_ACTION_AUTOEXPAND);
                  ptCursorPos.x = x;
                  ptCursorPos.y = y;
                  ASSERT_VALIDTEXTPOS (ptCursorPos);
                  SetCursorPos (ptCursorPos);
                  SetSelection (ptCursorPos, ptCursorPos);
                  SetAnchor (ptCursorPos);
                  OnEditOperation (CE_ACTION_TYPING, pszExpand);
                  ptCursorPos = GetCursorPos ();
                  if (!pszSlash)
                    break;
                  switch (*pszSlash)
                    {
                      case _T ('n'):
                        {
                          const static TCHAR szText[3] = _T ("\r\n");
                          m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, szText, y, x, CE_ACTION_AUTOEXPAND);  //  [JRT]
                          ptCursorPos.x = x;
                          ptCursorPos.y = y;
                          ASSERT_VALIDTEXTPOS (ptCursorPos);
                          SetSelection (ptCursorPos, ptCursorPos);
                          SetAnchor (ptCursorPos);
                          SetCursorPos (ptCursorPos);
                          OnEditOperation (CE_ACTION_TYPING, szText);
                        }
                        break;
                      case _T ('u'):
                        MoveUp (FALSE);
                        break;
                      case _T ('d'):
                        MoveDown (FALSE);
                        break;
                      case _T ('l'):
                        MoveLeft (FALSE);
                        break;
                      case _T ('r'):
                        MoveRight (FALSE);
                        break;
                      case _T ('h'):
                        MoveHome (FALSE);
                        break;
                      case _T ('f'):
                        MoveEnd (FALSE);
                        break;
                      case _T ('b'):
                        {
                          CPoint ptSelStart = ptCursorPos;
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
                              ptCursorPos.x--;          // Decrement Position

                              bDeleted = true;          // Set Deleted Flag

                            }
                          ASSERT_VALIDTEXTPOS (ptCursorPos);
                          SetAnchor (ptCursorPos);
                          SetSelection (ptCursorPos, ptCursorPos);
                          SetCursorPos (ptCursorPos);

                          if (bDeleted)
                              m_pTextBuffer->DeleteText (this, ptCursorPos.y, ptCursorPos.x, ptSelStart.y, ptSelStart.x, CE_ACTION_AUTOEXPAND);  // [JRT]
                        }
                        break;
                      case _T ('e'):
                        {
                          CPoint ptSelEnd = ptCursorPos;
                          if (ptSelEnd.x == GetLineLength (ptSelEnd.y))
                            {
                              if (ptSelEnd.y == GetLineCount () - 1)
                                break;
                              ptSelEnd.y++;
                              ptSelEnd.x = 0;
                            }
                          else
                            ptSelEnd.x++;
                          m_pTextBuffer->DeleteText (this, ptCursorPos.y, ptCursorPos.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_AUTOEXPAND);   // [JRT]
                        }
                        break;
                      case _T ('t'):
                        {
                          static TCHAR szText[32];
                          if (m_bInsertTabs)
                            {
                              *szText = _T ('\t');
                              szText[1] = _T ('\0');
                            }
                          else
                            {
                              int nTabSize = GetTabSize ();
                              int nChars = nTabSize - ptCursorPos.x % nTabSize;
                              memset(szText, _T(' '), nChars);
                              szText[nChars] = _T ('\0');
                            }
                          m_pTextBuffer->InsertText (this, ptCursorPos.y, ptCursorPos.x, szText, y, x, CE_ACTION_AUTOEXPAND);  //  [JRT]
                          ptCursorPos.x = x;
                          ptCursorPos.y = y;
                          ASSERT_VALIDTEXTPOS (ptCursorPos);
                          SetSelection (ptCursorPos, ptCursorPos);
                          SetAnchor (ptCursorPos);
                          SetCursorPos (ptCursorPos);
                        }
                    }
                  ptCursorPos = GetCursorPos ();
                  pszExpand = pszSlash + 1;
                  pszSlash = _tcschr (pszExpand, _T ('\\'));
                  if (pszSlash)
                    *pszSlash++ = _T ('\0');
                }
            }
          sExpand.ReleaseBuffer ();
          EnsureVisible (ptCursorPos);
          m_pTextBuffer->FlushUndoGroup (this);
        }
    }
}

void CCrystalEditView::
OnUpdateEditAutoExpand (CCmdUI * pCmdUI)
{
  if (m_mapExpand->IsEmpty ())
    pCmdUI->Enable (FALSE);
  else
    OnUpdateEditAutoComplete (pCmdUI);
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
      CPoint ptCursorPos = GetCursorPos ();
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);
      CString text;
      GetText (ptSelStart, ptSelEnd, text);
      text.MakeLower ();

      m_pTextBuffer->BeginUndoGroup ();

      if (IsSelection ())
        {
          CPoint ptSelStart, ptSelEnd;
          GetSelection (ptSelStart, ptSelEnd);
    
          ptCursorPos = ptSelStart;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/
    
          // [JRT]:
          m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_LOWERCASE);
        }

      int x, y;
      m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, text, y, x, CE_ACTION_LOWERCASE);

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
      CPoint ptCursorPos = GetCursorPos ();
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);
      CString text;
      GetText (ptSelStart, ptSelEnd, text);
      text.MakeUpper ();

      m_pTextBuffer->BeginUndoGroup ();

      if (IsSelection ())
        {
          CPoint ptSelStart, ptSelEnd;
          GetSelection (ptSelStart, ptSelEnd);
    
          ptCursorPos = ptSelStart;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/
    
          // [JRT]:
          m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_UPPERCASE);
        }

      int x, y;
      m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, text, y, x, CE_ACTION_UPPERCASE);

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
      CPoint ptCursorPos = GetCursorPos ();
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);
      CString text;
      GetText (ptSelStart, ptSelEnd, text);
      int nLen = text.GetLength ();
      LPTSTR pszText = text.GetBuffer (nLen + 1);
      while (*pszText)
        *pszText++ = (TCHAR)(_istlower (*pszText) ? _totupper (*pszText) : _totlower (*pszText));
      text.ReleaseBuffer ();

      m_pTextBuffer->BeginUndoGroup ();

      if (IsSelection ())
        {
          CPoint ptSelStart, ptSelEnd;
          GetSelection (ptSelStart, ptSelEnd);
    
          ptCursorPos = ptSelStart;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/
    
          // [JRT]:
          m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_SWAPCASE);
        }

      int x, y;
      m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, text, y, x, CE_ACTION_SWAPCASE);

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
      CPoint ptCursorPos = GetCursorPos ();
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);
      CString text;
      GetText (ptSelStart, ptSelEnd, text);
      int nLen = text.GetLength ();
      LPTSTR pszText = text.GetBuffer (nLen + 1);
      bool bCapitalize = true;
      while (*pszText)
        {
          if (_istspace (*pszText))
            bCapitalize = true;
          else if (_istalpha (*pszText))
            if (bCapitalize)
              {
                *pszText = (TCHAR)_totupper (*pszText);
                bCapitalize = false;
              }
            else
              *pszText = (TCHAR)_totlower (*pszText);
          pszText++;
        }
      text.ReleaseBuffer ();

      m_pTextBuffer->BeginUndoGroup ();

      if (IsSelection ())
        {
          CPoint ptSelStart, ptSelEnd;
          GetSelection (ptSelStart, ptSelEnd);
    
          ptCursorPos = ptSelStart;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/
    
          // [JRT]:
          m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_CAPITALIZE);
        }

      int x, y;
      m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, text, y, x, CE_ACTION_CAPITALIZE);

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
      CPoint ptCursorPos = GetCursorPos ();
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);
      CString text;
      GetText (ptSelStart, ptSelEnd, text);
      int nLen = text.GetLength ();
      LPTSTR pszText = text.GetBuffer (nLen + 1);
      bool bCapitalize = true;
      while (*pszText)
        {
          if (!_istspace (*pszText))
            if (*pszText == _T ('.'))
              {
                if (pszText[1] && !_istdigit (pszText[1]))
                  bCapitalize = true;
              }
            else if (_istalpha (*pszText))
              if (bCapitalize)
                {
                  *pszText = (TCHAR)_totupper (*pszText);
                  bCapitalize = false;
                }
              else
                *pszText = (TCHAR)_totlower (*pszText);
          pszText++;
        }
      text.ReleaseBuffer ();

      m_pTextBuffer->BeginUndoGroup ();

      if (IsSelection ())
        {
          CPoint ptSelStart, ptSelEnd;
          GetSelection (ptSelStart, ptSelEnd);
    
          ptCursorPos = ptSelStart;
          /*SetAnchor (ptCursorPos);
          SetSelection (ptCursorPos, ptCursorPos);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);*/
    
          // [JRT]:
          m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_SENTENCIZE);
        }

      int x, y;
      m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, text, y, x, CE_ACTION_SENTENCIZE);

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
	CPoint	ptLastChange = m_pTextBuffer->GetLastChangePos();
	pCmdUI->Enable( ptLastChange.x > 0 && ptLastChange.y > -1 );
}

void CCrystalEditView::OnEditGotoLastChange()
{
	CPoint	ptLastChange = m_pTextBuffer->GetLastChangePos();
	if( ptLastChange.x < 0 || ptLastChange.y < 0 )
		return;

	// goto last change
	SetCursorPos( ptLastChange );
	SetSelection( ptLastChange, ptLastChange );
	EnsureVisible( ptLastChange );
}
//END SW

int CCrystalEditView::SpellGetLine (struct SpellData_t *pdata)
{
  int nCount;
  CCrystalEditView *pView = (CCrystalEditView*) pdata->pUserData;
  static TCHAR szBuffer[4096];

  if (pdata->nRow < pView->GetLineCount ())
    {
      nCount = pView->GetLineLength (pdata->nRow) + 1;
      /*if (pdata->pszBuffer)
        free (pdata->pszBuffer);
      pdata->pszBuffer = (LPTSTR) malloc (nCount + 2);*/
      pdata->pszBuffer = szBuffer;
      *pdata->pszBuffer = _T ('^');
      if (nCount > 1)
        _tcscpy (pdata->pszBuffer + 1, pView->GetLineChars (pdata->nRow));
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
  CCrystalEditView *pView = (CCrystalEditView*) pdata->pUserData;
  CPoint ptStartPos, ptEndPos;
  int x, y;

  switch (nEvent)
    {
      case SN_FOUND:
        ptStartPos.x = pdata->nColumn - 1;
        ptStartPos.y = pdata->nRow - 1;
        ptEndPos.x = pdata->nColumn - 1 + _tcslen (pdata->pszWord);
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
            pView->GetSelection (ptStartPos, ptEndPos);
            pView->m_pTextBuffer->DeleteText (pView, ptStartPos.y, ptStartPos.x, ptEndPos.y, ptEndPos.x, CE_ACTION_SPELL);
            pView->m_pTextBuffer->InsertText (pView, ptStartPos.y, ptStartPos.x, pdata->pszWord, y, x, CE_ACTION_SPELL);
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

HMODULE CCrystalEditView::hSpellDll = NULL;
TCHAR CCrystalEditView::szWIspellPath[_MAX_PATH];
SpellData CCrystalEditView::spellData;
int (*CCrystalEditView::SpellInit) (SpellData*);
int (*CCrystalEditView::SpellCheck) (SpellData*);
int (*CCrystalEditView::SpellConfig) (SpellData*);

BOOL CCrystalEditView::LoadSpellDll (BOOL bAlert /*= TRUE*/)
{
  if (hSpellDll)
    return TRUE;
  CString sPath = szWIspellPath;
  if (!sPath.IsEmpty () && sPath[sPath.GetLength () - 1] != _T('\\'))
    sPath += _T ('\\');
  sPath += _T ("wispell.dll");
  hSpellDll = LoadLibrary (sPath);
  if (hSpellDll)
    {
      SpellInit = (int (*) (SpellData*)) GetProcAddress (hSpellDll, "SpellInit");
      SpellCheck = (int (*) (SpellData*)) GetProcAddress (hSpellDll, "SpellCheck");
      SpellConfig = (int (*) (SpellData*)) GetProcAddress (hSpellDll, "SpellConfig");
      if (SpellInit)
        SpellInit (&spellData);
      _tcscpy (spellData.szIspell, szWIspellPath);
      spellData.GetLine = SpellGetLine;
      spellData.Notify = SpellNotify;
    }
  else
    {
      SpellInit = SpellCheck = SpellConfig = NULL;
      if (bAlert)
        ::MessageBox (AfxGetMainWnd ()->GetSafeHwnd (), _T ("Error loading \"wispell.dll\"."), _T ("Error"), MB_OK|MB_ICONEXCLAMATION);
    }
  return hSpellDll != NULL;
}

void CCrystalEditView::
OnUpdateToolsSpelling (CCmdUI * pCmdUI)
{
  CPoint ptCursorPos = GetCursorPos ();
  int nLines = GetLineCount () - 1;
  pCmdUI->Enable (LoadSpellDll (FALSE) && ptCursorPos.y < nLines);
}

void CCrystalEditView::
OnToolsSpelling ()
{
  CPoint ptCursorPos = GetCursorPos ();
  if (LoadSpellDll () && ptCursorPos.y < GetLineCount ())
    {
      spellData.hParent = GetSafeHwnd ();
      spellData.nRow = ptCursorPos.y;
      spellData.pUserData = (LPVOID) (LPCVOID) this;
      spellData.pszBuffer = NULL;
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

long str_pos (LPCTSTR whole, LPCTSTR piece);

void CCrystalEditView::
OnToolsCharCoding ()
{
  if (IsSelection ())
    {
      CWaitCursor wait;
      CPoint ptCursorPos = GetCursorPos ();
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);
      CString sText;
      GetText (ptSelStart, ptSelEnd, sText);
      CCharConvDlg dlg;
      dlg.m_sOriginal = sText;
      LPTSTR pszEnd = dlg.m_sOriginal.GetBuffer (dlg.m_sOriginal.GetLength () + 1);
      for (int i = 0; i < 13; i++)
        {
          pszEnd = _tcschr (pszEnd, _T ('\n'));
          if (pszEnd)
            pszEnd++;
          else
            break;
        }
      if (pszEnd)
        *pszEnd = _T ('\0');
      dlg.m_sOriginal.ReleaseBuffer ();
      if (dlg.DoModal () != IDOK)
        return;
      LPTSTR pszNew;
      if (!iconvert_new (sText, &pszNew, dlg.m_nSource, dlg.m_nDest, dlg.m_bAlpha != FALSE))
        {
          ASSERT (pszNew);
          m_pTextBuffer->BeginUndoGroup ();

          m_pTextBuffer->DeleteText (this, ptSelStart.y, ptSelStart.x, ptSelEnd.y, ptSelEnd.x, CE_ACTION_RECODE);

          int x, y;
          m_pTextBuffer->InsertText (this, ptSelStart.y, ptSelStart.x, pszNew, y, x, CE_ACTION_RECODE);

          if (IsValidTextPos (ptCursorPos))
            ptCursorPos.x = 0;
          ASSERT_VALIDTEXTPOS (ptCursorPos);
          SetAnchor (ptCursorPos);
          SetSelection (ptSelStart, ptSelEnd);
          SetCursorPos (ptCursorPos);
          EnsureVisible (ptCursorPos);

          m_pTextBuffer->FlushUndoGroup (this);
        }
      if (pszNew)
        free (pszNew);
    }
}

void CCrystalEditView::
OnEditDeleteWord ()
{
  if (!IsSelection ())
    MoveWordRight (TRUE);
  if (IsSelection ())
    {
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);

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
    MoveWordLeft (TRUE);
  if (IsSelection ())
    {
      CPoint ptSelStart, ptSelEnd;
      GetSelection (ptSelStart, ptSelEnd);

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

////////////////////////////////////////////////////////////////////////////
#pragma warning ( default : 4100 )
