////////////////////////////////////////////////////////////////////////////
//  File:       ceditreplacedlg.cpp
//  Version:    1.0.0.0
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Implementation of the CEditReplaceDlg dialog, a part of Crystal Edit -
//  syntax coloring text editor.
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  19-Jul-99
//      Ferdinand Prantl:
//  +   FEATURE: regular expressions, go to line and things ...
//  +   FEATURE: some other things I've forgotten ...
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////
/**
 *  @file ceditreplacedlg.cpp
 *
 *  @brief Implementation of Replace-dialog.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "resource.h"
#include "ceditreplacedlg.h"
#include "ccrystaleditview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditReplaceDlg dialog


CEditReplaceDlg::CEditReplaceDlg (CCrystalEditView * pBuddy)
: CDialog (CEditReplaceDlg::IDD, NULL)
, m_pBuddy(pBuddy)
, m_bMatchCase(FALSE)
, m_bWholeWord(FALSE)
, m_bRegExp(FALSE)
, m_nScope(-1)
, m_bDontWrap(FALSE)
, m_bEnableScopeSelection(TRUE)
{
  ASSERT (pBuddy != NULL);
}

void CEditReplaceDlg::
UpdateRegExp ()
{
  if (m_bRegExp)
    {
      m_ctlWholeWord.EnableWindow (FALSE);
      m_bWholeWord = FALSE;
    }
  else
    {
      m_ctlWholeWord.EnableWindow (TRUE);
    }
}

void CEditReplaceDlg::
DoDataExchange (CDataExchange * pDX)
{
  CDialog::DoDataExchange (pDX);
  //{{AFX_DATA_MAP(CEditReplaceDlg)
  DDX_Control (pDX, IDC_EDIT_FINDTEXT, m_ctlFindText);
  DDX_Control (pDX, IDC_EDIT_REPLACE_WITH, m_ctlReplText);
  DDX_Control (pDX, IDC_EDIT_WHOLE_WORD, m_ctlWholeWord);
  DDX_Check (pDX, IDC_EDIT_MATCH_CASE, m_bMatchCase);
  DDX_Check (pDX, IDC_EDIT_WHOLE_WORD, m_bWholeWord);
  DDX_Check (pDX, IDC_EDIT_REGEXP, m_bRegExp);
  DDX_CBString (pDX, IDC_EDIT_FINDTEXT, m_sText);
  DDX_CBString (pDX, IDC_EDIT_REPLACE_WITH, m_sNewText);
  DDX_Radio (pDX, IDC_EDIT_SCOPE_SELECTION, m_nScope);
  DDX_Check (pDX, IDC_EDIT_SCOPE_DONT_WRAP, m_bDontWrap);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP (CEditReplaceDlg, CDialog)
//{{AFX_MSG_MAP(CEditReplaceDlg)
ON_CBN_EDITCHANGE (IDC_EDIT_FINDTEXT, OnChangeEditText)
ON_CBN_SELCHANGE (IDC_EDIT_FINDTEXT, OnChangeSelected)
ON_BN_CLICKED (IDC_EDIT_REPLACE, OnEditReplace)
ON_BN_CLICKED (IDC_EDIT_REPLACE_ALL, OnEditReplaceAll)
ON_BN_CLICKED (IDC_EDIT_SKIP, OnEditSkip)
ON_BN_CLICKED (IDC_EDIT_REGEXP, OnRegExp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP ()

BOOL CEditReplaceDlg::PreTranslateMessage(MSG* pMsg)
{
  if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
      CButton *pSkip = (CButton*) GetDlgItem (IDC_EDIT_SKIP);
      if (pSkip->GetButtonStyle () & BS_DEFPUSHBUTTON)
        {
          OnEditSkip ();
        }
      else
        {
          OnEditReplace ();
        }
      return TRUE;
    }
  return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CEditReplaceDlg message handlers

void CEditReplaceDlg::OnChangeEditText ()
{
  UpdateData();
  UpdateControls();
}
void CEditReplaceDlg::OnChangeSelected ()
{
  int sel = m_ctlFindText.GetCurSel();
  if (sel != CB_ERR)
  {
    m_ctlFindText.GetLBText(sel, m_sText);
    m_ctlFindText.SetWindowText(m_sText);
  }
  UpdateControls();
}

void CEditReplaceDlg::
OnCancel ()
{
  VERIFY (UpdateData ());
  CDialog::OnCancel ();
}

BOOL CEditReplaceDlg::
OnInitDialog ()
{
  CDialog::OnInitDialog ();

  m_ctlFindText.LoadState(_T("Files\\ReplaceInFile"));
  m_ctlReplText.LoadState(_T("Files\\ReplaceWithInFile"));
  GetDlgItem (IDC_EDIT_REPLACE_WITH)->GetWindowText (m_sNewText);
  UpdateData (FALSE);

  UpdateControls();
  GetDlgItem (IDC_EDIT_SCOPE_SELECTION)->EnableWindow (m_bEnableScopeSelection);
  m_bFound = FALSE;

  return TRUE;
}

LastSearchInfos * CEditReplaceDlg::
GetLastSearchInfos() 
{
  return &lastSearch;
}

BOOL CEditReplaceDlg::
DoHighlightText ( BOOL bNotifyIfNotFound )
{
  ASSERT (m_pBuddy != NULL);
  DWORD dwSearchFlags = 0;
  if (m_bMatchCase)
    dwSearchFlags |= FIND_MATCH_CASE;
  if (m_bWholeWord)
    dwSearchFlags |= FIND_WHOLE_WORD;
  if (m_bRegExp)
    dwSearchFlags |= FIND_REGEXP;

  BOOL bFound;
  if (m_nScope == 0)
    {
      //  Searching selection only
      bFound = m_pBuddy->FindTextInBlock (m_sText, m_ptFoundAt, m_ptBlockBegin, m_ptBlockEnd,
                                          dwSearchFlags, FALSE, &m_ptFoundAt);
    }
  else if (m_bDontWrap)
    {
      //  Searching whole text, no wrap
      bFound = m_pBuddy->FindText (m_sText, m_ptFoundAt, dwSearchFlags, FALSE, &m_ptFoundAt);
    }
  else
    {
      //  Searching whole text, wrap
      bFound = m_pBuddy->FindText (m_sText, m_ptFoundAt, dwSearchFlags, TRUE, &m_ptFoundAt);
    }

  if (!bFound)
    {
      if ( bNotifyIfNotFound ) 
      {
        CString prompt;
        prompt.Format (IDS_EDIT_TEXT_NOT_FOUND, m_sText);
        AfxMessageBox (prompt, MB_ICONINFORMATION);
      }
      if (m_nScope == 0)
        m_ptCurrentPos = m_ptBlockBegin;

      return FALSE;
    }

  m_pBuddy->HighlightText (m_ptFoundAt, m_pBuddy->m_nLastFindWhatLen);
  return TRUE;
}

BOOL CEditReplaceDlg::
DoReplaceText (LPCTSTR /*pszNewText*/, DWORD dwSearchFlags)
{
  ASSERT (m_pBuddy != NULL);
  // m_pBuddy->m_nLastFindWhatLen

  BOOL bFound;
  if (m_nScope == 0)
    {
      //  Searching selection only
      bFound = m_pBuddy->FindTextInBlock (m_sText, m_ptFoundAt, m_ptBlockBegin, m_ptBlockEnd,
                                          dwSearchFlags, FALSE, &m_ptFoundAt);
    }
  else if (m_bDontWrap)
    {
      //  Searching whole text, no wrap
      bFound = m_pBuddy->FindText (m_sText, m_ptFoundAt, dwSearchFlags, FALSE, &m_ptFoundAt);
    }
  else
    {
      //  Searching whole text, wrap
      bFound = m_pBuddy->FindText (m_sText, m_ptFoundAt, dwSearchFlags, TRUE, &m_ptFoundAt);
    }

  if (!bFound)
    {
      CString prompt;
      prompt.Format (IDS_EDIT_TEXT_NOT_FOUND, m_sText);
      AfxMessageBox (prompt, MB_ICONINFORMATION);
      if (m_nScope == 0)
        m_ptCurrentPos = m_ptBlockBegin;
      return FALSE;
    }

  m_pBuddy->HighlightText (m_ptFoundAt, m_pBuddy->m_nLastFindWhatLen);
  return TRUE;
}

void CEditReplaceDlg::
OnEditSkip ()
{
  if (!UpdateData ())
    return;
  
  m_ctlFindText.SaveState(_T("Files\\ReplaceInFile"));
  m_ctlReplText.SaveState(_T("Files\\ReplaceWithInFile"));
  UpdateLastSearch ();

  CButton *pSkip = (CButton*) GetDlgItem (IDC_EDIT_SKIP);
  CButton *pRepl = (CButton*) GetDlgItem (IDC_EDIT_REPLACE);

  if (!m_bFound)
    {
      m_ptFoundAt = m_ptCurrentPos;
      m_bFound = DoHighlightText ( TRUE );
      if (m_bFound)
        {
          pSkip->SetButtonStyle (pSkip->GetButtonStyle () & ~BS_DEFPUSHBUTTON);
          pRepl->SetButtonStyle (pRepl->GetButtonStyle () | BS_DEFPUSHBUTTON);
          // pRepl->SetFocus ();
        }
      else
        {
          pRepl->SetButtonStyle (pRepl->GetButtonStyle () & ~BS_DEFPUSHBUTTON);
          pSkip->SetButtonStyle (pSkip->GetButtonStyle () | BS_DEFPUSHBUTTON);
          // pSkip->SetFocus ();
        }
      return;
    }

  if (!m_pBuddy->m_nLastFindWhatLen)
    if (m_ptFoundAt.y + 1 < m_pBuddy->GetLineCount ())
      {
        m_ptFoundAt.x = 0;
        m_ptFoundAt.y++;
      }
    else
      {
        m_bFound = FALSE;
        return;
      }
  else
    m_ptFoundAt.x += 1;
  m_bFound = DoHighlightText ( TRUE );
  if (m_bFound)
    {
      pSkip->SetButtonStyle (pSkip->GetButtonStyle () & ~BS_DEFPUSHBUTTON);
      pRepl->SetButtonStyle (pRepl->GetButtonStyle () | BS_DEFPUSHBUTTON);
      // pRepl->SetFocus ();
    }
  else
    {
      pRepl->SetButtonStyle (pRepl->GetButtonStyle () & ~BS_DEFPUSHBUTTON);
      pSkip->SetButtonStyle (pSkip->GetButtonStyle () | BS_DEFPUSHBUTTON);
      // pSkip->SetFocus ();
    }
}

void CEditReplaceDlg::
OnEditReplace ()
{
  if (!UpdateData ())
    return;

  m_ctlFindText.SaveState(_T("Files\\ReplaceInFile"));
  m_ctlReplText.SaveState(_T("Files\\ReplaceWithInFile"));
  UpdateLastSearch ();

  if (!m_bFound)
    {
      m_ptFoundAt = m_ptCurrentPos;
      m_bFound = DoHighlightText ( TRUE );
      CButton *pSkip = (CButton*) GetDlgItem (IDC_EDIT_SKIP);
      CButton *pRepl = (CButton*) GetDlgItem (IDC_EDIT_REPLACE);
      if (m_bFound)
        {
          pSkip->SetButtonStyle (pSkip->GetButtonStyle () & ~BS_DEFPUSHBUTTON);
          pRepl->SetButtonStyle (pRepl->GetButtonStyle () | BS_DEFPUSHBUTTON);
          // pRepl->SetFocus ();
        }
      else
        {
          pRepl->SetButtonStyle (pRepl->GetButtonStyle () & ~BS_DEFPUSHBUTTON);
          pSkip->SetButtonStyle (pSkip->GetButtonStyle () | BS_DEFPUSHBUTTON);
          // pSkip->SetFocus ();
        }
      return;
    }
  DWORD dwSearchFlags = 0;
  if (m_bMatchCase)
    dwSearchFlags |= FIND_MATCH_CASE;
  if (m_bWholeWord)
    dwSearchFlags |= FIND_WHOLE_WORD;
  if (m_bRegExp)
    dwSearchFlags |= FIND_REGEXP;

  //  We have highlighted text
  VERIFY (m_pBuddy->ReplaceSelection (m_sNewText, dwSearchFlags));

  //  Manually recalculate points
  if (m_bEnableScopeSelection)
    {
      if (m_ptBlockBegin.y == m_ptFoundAt.y && m_ptBlockBegin.x > m_ptFoundAt.x)
        {
          m_ptBlockBegin.x -= m_pBuddy->m_nLastFindWhatLen;
          m_ptBlockBegin.x += m_pBuddy->m_nLastReplaceLen;
        }
      if (m_ptBlockEnd.y == m_ptFoundAt.y && m_ptBlockEnd.x > m_ptFoundAt.x)
        {
          m_ptBlockEnd.x -= m_pBuddy->m_nLastFindWhatLen;
          m_ptBlockEnd.x += m_pBuddy->m_nLastReplaceLen;
        }
    }
  if (!m_pBuddy->m_nLastFindWhatLen)
    if (m_ptFoundAt.y + 1 < m_pBuddy->GetLineCount ())
      {
        m_ptFoundAt.x = 0;
        m_ptFoundAt.y++;
      }
    else
      {
        m_bFound = FALSE;
        return;
      }
  else
    {
      m_ptFoundAt.x += m_pBuddy->m_nLastReplaceLen;
      m_ptFoundAt = m_pBuddy->GetCursorPos ();
    }
  m_bFound = DoHighlightText ( TRUE );
}

void CEditReplaceDlg::
OnEditReplaceAll ()
{
  if (!UpdateData ())
    return;

  m_ctlFindText.SaveState(_T("Files\\ReplaceInFile"));
  m_ctlReplText.SaveState(_T("Files\\ReplaceWithInFile"));
  UpdateLastSearch ();

  int nNumReplaced = 0;
  BOOL bWrapped = FALSE;
  CWaitCursor waitCursor;


  if (!m_bFound)
    {
      m_ptFoundAt = m_ptCurrentPos;
      m_bFound = DoHighlightText ( FALSE );
    }

  CPoint m_ptFirstFound = m_ptFoundAt;

  while (m_bFound)
    {
      DWORD dwSearchFlags = 0;
      if (m_bMatchCase)
        dwSearchFlags |= FIND_MATCH_CASE;
      if (m_bWholeWord)
        dwSearchFlags |= FIND_WHOLE_WORD;
      if (m_bRegExp)
        dwSearchFlags |= FIND_REGEXP;
    
      //  We have highlighted text
      VERIFY (m_pBuddy->ReplaceSelection (m_sNewText, dwSearchFlags));

      //  Manually recalculate points
      if (m_bEnableScopeSelection)
        {
          if (m_ptBlockBegin.y == m_ptFoundAt.y && m_ptBlockBegin.x > m_ptFoundAt.x)
            {
              m_ptBlockBegin.x -= m_pBuddy->m_nLastFindWhatLen;
              m_ptBlockBegin.x += m_pBuddy->m_nLastReplaceLen;
            }
          if (m_ptBlockEnd.y == m_ptFoundAt.y && m_ptBlockEnd.x > m_ptFoundAt.x)
            {
              m_ptBlockEnd.x -= m_pBuddy->m_nLastFindWhatLen;
              m_ptBlockEnd.x += m_pBuddy->m_nLastReplaceLen;
            }
        }
      // recalculate m_ptFirstFound
      if (m_ptFirstFound.y == m_ptFoundAt.y && m_ptFirstFound.x > m_ptFoundAt.x)
        {
          m_ptFirstFound.x -= m_pBuddy->m_nLastFindWhatLen;
          m_ptFirstFound.x += m_pBuddy->m_nLastReplaceLen;
        }

      // calculate the end of the current replacement
      CPoint m_ptCurrentReplacedEnd;
      m_ptCurrentReplacedEnd.y = m_ptFoundAt.y;
      m_ptCurrentReplacedEnd.x = m_ptFoundAt.x + m_pBuddy->m_nLastReplaceLen;

      // m_ptFoundAt.x has two meanings:
      // (1) One is the position of the word that was found.
      // (2) The other is next position to search.
      // The code below calculates the latter.
      if (!m_pBuddy->m_nLastFindWhatLen)
        if (m_ptFoundAt.y + 1 < m_pBuddy->GetLineCount ())
          {
            m_ptFoundAt.x = 0;
            m_ptFoundAt.y++;
          }
        else
          {
            m_bFound = FALSE;
            break;
          }
      else
        {
          m_ptFoundAt.x += m_pBuddy->m_nLastReplaceLen;
          m_ptFoundAt = m_pBuddy->GetCursorPos ();
        }
      nNumReplaced++;

      // find the next instance
      m_bFound = DoHighlightText ( FALSE );

      // detect if we just wrapped at end of file
      if (m_ptFoundAt.y < m_ptCurrentReplacedEnd.y || (m_ptFoundAt.y == m_ptCurrentReplacedEnd.y && m_ptFoundAt.x < m_ptCurrentReplacedEnd.x))
        bWrapped = TRUE;

      // after wrapping, stop at m_ptFirstFound
      // so we don't replace twice when replacement string includes replaced string 
      // (like replace "here" with "there")
      if (bWrapped)
        if (m_ptFoundAt.y > m_ptFirstFound.y || (m_ptFoundAt.y == m_ptFirstFound.y && m_ptFoundAt.x >= m_ptFirstFound.x))
          break;
    }

  // Let user know how many strings were replaced
  CString strMessage;
  CString strNumber;
  strNumber.Format( _T("%d"), nNumReplaced );
  AfxFormatString1(strMessage, IDS_NUM_REPLACED, strNumber);

  AfxMessageBox( strMessage, MB_ICONINFORMATION|MB_DONT_DISPLAY_AGAIN, IDS_NUM_REPLACED);
}

void CEditReplaceDlg::
OnRegExp ()
{
  UpdateData (TRUE);
  UpdateRegExp ();
  UpdateData (FALSE);
}

void CEditReplaceDlg::
UpdateControls()
{
  GetDlgItem(IDC_EDIT_SKIP)->EnableWindow( !m_sText.IsEmpty() );
  GetDlgItem(IDC_EDIT_REPLACE)->EnableWindow( !m_sText.IsEmpty() );
  GetDlgItem(IDC_EDIT_REPLACE_ALL)->EnableWindow( !m_sText.IsEmpty() );
  
  UpdateRegExp();
}


//
// Last search functions
//
void CEditReplaceDlg::
SetLastSearch (LPCTSTR sText, BOOL bMatchCase, BOOL bWholeWord, BOOL bRegExp, int nScope)
{
  lastSearch.m_bMatchCase = bMatchCase;
  lastSearch.m_bWholeWord = bWholeWord;
  lastSearch.m_bRegExp = bRegExp;
  lastSearch.m_sText = sText;
  lastSearch.m_bNoWrap = m_bDontWrap;
}


void CEditReplaceDlg::
UpdateLastSearch ()
{
  SetLastSearch (m_sText, m_bMatchCase, m_bWholeWord, m_bRegExp, m_nScope);
}

void CEditReplaceDlg::
UseLastSearch () 
{
  m_bMatchCase = lastSearch.m_bMatchCase;
  m_bWholeWord = lastSearch.m_bWholeWord;
  m_bRegExp = lastSearch.m_bRegExp;
  m_sText = lastSearch.m_sText;
  m_bDontWrap = lastSearch.m_bNoWrap;
}

void CEditReplaceDlg::
SetScope(BOOL bWithSelection)
{
  if (bWithSelection)
    m_nScope = 0;
  else
    m_nScope = 1;
}
