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

#include "stdafx.h"
#include "ceditreplacedlg.h"
#include "ccrystaleditview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditReplaceDlg dialog


CEditReplaceDlg::CEditReplaceDlg (CCrystalEditView * pBuddy):CDialog (CEditReplaceDlg::IDD, NULL)
{
  ASSERT (pBuddy != NULL);
  m_pBuddy = pBuddy;
  //{{AFX_DATA_INIT(CEditReplaceDlg)
  m_bMatchCase = FALSE;
  m_bWholeWord = FALSE;
  m_bRegExp = FALSE;
  m_sText = _T ("");
  m_sNewText = _T ("");
  m_nScope = -1;
  //}}AFX_DATA_INIT
  m_bEnableScopeSelection = TRUE;
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
  DDX_Control (pDX, IDC_EDIT_TEXT, m_ctlFindText);
  DDX_Control (pDX, IDC_EDIT_REPLACE_WITH, m_ctlReplText);
  DDX_Control (pDX, IDC_EDIT_WHOLE_WORD, m_ctlWholeWord);
  DDX_Check (pDX, IDC_EDIT_MATCH_CASE, m_bMatchCase);
  DDX_Check (pDX, IDC_EDIT_WHOLE_WORD, m_bWholeWord);
  DDX_Check (pDX, IDC_EDIT_REGEXP, m_bRegExp);
  DDX_CBString (pDX, IDC_EDIT_TEXT, m_sText);
  DDX_CBString (pDX, IDC_EDIT_REPLACE_WITH, m_sNewText);
  DDX_Radio (pDX, IDC_EDIT_SCOPE_SELECTION, m_nScope);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP (CEditReplaceDlg, CDialog)
//{{AFX_MSG_MAP(CEditReplaceDlg)
ON_CBN_EDITCHANGE (IDC_EDIT_TEXT, OnChangeEditText)
ON_CBN_SELCHANGE (IDC_EDIT_TEXT, OnChangeEditText)
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
  GetDlgItem (IDC_EDIT_SKIP)->EnableWindow (!m_sText.IsEmpty ());
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

  m_ctlReplText.m_sGroup = _T ("ReplaceText");
  m_ctlReplText.OnSetfocus ();
  GetDlgItem (IDC_EDIT_REPLACE_WITH)->GetWindowText (m_sNewText);
  UpdateData (FALSE);
  m_ctlFindText.m_sGroup = _T ("FindText");
  m_ctlFindText.OnSetfocus ();
  GetDlgItem (IDC_EDIT_SKIP)->EnableWindow (!m_sText.IsEmpty ());
  GetDlgItem (IDC_EDIT_SCOPE_SELECTION)->EnableWindow (m_bEnableScopeSelection);
  m_bFound = FALSE;
  UpdateRegExp ();

  return TRUE;
}

BOOL CEditReplaceDlg::
DoHighlightText ()
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
  else
    {
      //  Searching whole text
      bFound = m_pBuddy->FindText (m_sText, m_ptFoundAt, dwSearchFlags, FALSE, &m_ptFoundAt);
    }

  if (!bFound)
    {
      CString prompt;
      prompt.Format (IDS_EDIT_TEXT_NOT_FOUND, m_sText);
      AfxMessageBox (prompt);
      m_ptCurrentPos = m_nScope == 0 ? m_ptBlockBegin : CPoint (0, 0);
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
  else
    {
      //  Searching whole text
      bFound = m_pBuddy->FindText (m_sText, m_ptFoundAt, dwSearchFlags, FALSE, &m_ptFoundAt);
    }

  if (!bFound)
    {
      CString prompt;
      prompt.Format (IDS_EDIT_TEXT_NOT_FOUND, m_sText);
      AfxMessageBox (prompt);
      m_ptCurrentPos = m_nScope == 0 ? m_ptBlockBegin : CPoint (0, 0);
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
  CButton *pSkip = (CButton*) GetDlgItem (IDC_EDIT_SKIP);
  CButton *pRepl = (CButton*) GetDlgItem (IDC_EDIT_REPLACE);

  if (!m_bFound)
    {
      m_ptFoundAt = m_ptCurrentPos;
      m_bFound = DoHighlightText ();
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
  m_bFound = DoHighlightText ();
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

  if (!m_bFound)
    {
      m_ptFoundAt = m_ptCurrentPos;
      m_bFound = DoHighlightText ();
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
  m_bFound = DoHighlightText ();
}

void CEditReplaceDlg::
OnEditReplaceAll ()
{
  if (!UpdateData ())
    return;

  CWaitCursor waitCursor;
  if (!m_bFound)
    {
      m_ptFoundAt = m_ptCurrentPos;
      m_bFound = DoHighlightText ();
    }

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
      m_bFound = DoHighlightText ();
    }
}

void CEditReplaceDlg::
OnRegExp ()
{
  UpdateData (TRUE);
  UpdateRegExp ();
  UpdateData (FALSE);
}
