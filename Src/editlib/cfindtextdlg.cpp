////////////////////////////////////////////////////////////////////////////
//  File:       cfindtextdlg.cpp
//  Version:    1.0.0.0
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Implementation of the CFindTextDlg dialog, a part of Crystal Edit -
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
#include "cfindtextdlg.h"
#include "ccrystaltextview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindTextDlg dialog

CFindTextDlg::CFindTextDlg (CCrystalTextView * pBuddy)
: CDialog (CFindTextDlg::IDD, NULL)
, m_pBuddy(pBuddy)
, m_nDirection(1)
, m_bMatchCase(FALSE)
, m_bWholeWord(FALSE)
, m_bRegExp(FALSE)
, m_ptCurrentPos(CPoint (0, 0))
{
  //{{AFX_DATA_INIT(CFindTextDlg)
  //}}AFX_DATA_INIT
}

void CFindTextDlg::
DoDataExchange (CDataExchange * pDX)
{
  CDialog::DoDataExchange (pDX);
  //{{AFX_DATA_MAP(CFindTextDlg)
  DDX_Control (pDX, IDC_EDIT_FINDTEXT, m_ctlFindText);
  DDX_Control (pDX, IDC_EDIT_WHOLE_WORD, m_ctlWholeWord);
  DDX_Radio (pDX, IDC_EDIT_DIRECTION_UP, m_nDirection);
  DDX_Check (pDX, IDC_EDIT_MATCH_CASE, m_bMatchCase);
  DDX_CBString (pDX, IDC_EDIT_FINDTEXT, m_sText);
  DDX_Check (pDX, IDC_EDIT_WHOLE_WORD, m_bWholeWord);
  DDX_Check (pDX, IDC_EDIT_REGEXP, m_bRegExp);
  //}}AFX_DATA_MAP
}

void CFindTextDlg::
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

BEGIN_MESSAGE_MAP (CFindTextDlg, CDialog)
//{{AFX_MSG_MAP(CFindTextDlg)
ON_CBN_EDITCHANGE (IDC_EDIT_FINDTEXT, OnChangeEditText)
ON_CBN_SELCHANGE (IDC_EDIT_FINDTEXT, OnChangeSelected)
ON_BN_CLICKED (IDC_EDIT_REGEXP, OnRegExp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP ()

/////////////////////////////////////////////////////////////////////////////
// CFindTextDlg message handlers

void CFindTextDlg::OnOK ()
{
  if (UpdateData ())
    {
      UpdateLastSearch ();

      ASSERT (m_pBuddy != NULL);
      BOOL bCursorToLeft = FALSE;
      DWORD dwSearchFlags = 0;
      if (m_bMatchCase)
        dwSearchFlags |= FIND_MATCH_CASE;
      if (m_bWholeWord)
        dwSearchFlags |= FIND_WHOLE_WORD;
      if (m_bRegExp)
        dwSearchFlags |= FIND_REGEXP;
      if (m_nDirection == 0)
        {
          dwSearchFlags |= FIND_DIRECTION_UP;
          // When finding upwards put cursor to begin of selection
          bCursorToLeft = TRUE;
         }

      m_ctlFindText.SaveState(_T("Files\\FindInFile"));

      CPoint ptTextPos;
      if (!m_pBuddy->FindText (m_sText, m_ptCurrentPos, dwSearchFlags, TRUE,
          &ptTextPos))
        {
          CString prompt;
          prompt.Format (IDS_EDIT_TEXT_NOT_FOUND, m_sText);
          AfxMessageBox (prompt, MB_ICONINFORMATION);
          m_ptCurrentPos = CPoint (0, 0);
          return;
        }

      m_pBuddy->HighlightText (ptTextPos, m_pBuddy->m_nLastFindWhatLen,
          bCursorToLeft);

      CDialog::OnOK ();
    }
}

void CFindTextDlg::
OnChangeEditText ()
{
  TRACE0( "Changed find text\n" );
  UpdateData();
  UpdateControls();
}

void CFindTextDlg::
OnChangeSelected ()
{
  TRACE0( "Changed find selection\n" );
  int sel = m_ctlFindText.GetCurSel();
  if (sel != CB_ERR)
  {
    m_ctlFindText.GetLBText(sel, m_sText);
    m_ctlFindText.SetWindowText(m_sText);
  }
  UpdateControls();
}


BOOL CFindTextDlg::
OnInitDialog ()
{
  CDialog::OnInitDialog ();

  m_ctlFindText.LoadState(_T("Files\\FindInFile"));
  UpdateData (FALSE);
  UpdateControls();

  return TRUE;
}

LastSearchInfos * CFindTextDlg::
GetLastSearchInfos() 
{
  return &lastSearch;
}

void CFindTextDlg::
OnCancel ()
{
  VERIFY (UpdateData ());
  CDialog::OnCancel ();
}

void CFindTextDlg::
OnRegExp ()
{
  UpdateData (TRUE);
  UpdateRegExp ();
  UpdateData (FALSE);
}


//
// Update controls, enabling/disabling according to what's appropriate
//
void CFindTextDlg::
UpdateControls()
{
  GetDlgItem(IDOK)->EnableWindow( !m_sText.IsEmpty() );
  
  UpdateRegExp();
}

//
// Last search functions
//
void CFindTextDlg::
SetLastSearch (LPCTSTR sText, BOOL bMatchCase, BOOL bWholeWord, BOOL bRegExp, int nDirection)
{
  lastSearch.m_bMatchCase = bMatchCase;
  lastSearch.m_bWholeWord = bWholeWord;
  lastSearch.m_bRegExp = bRegExp;
  lastSearch.m_nDirection = nDirection;
  lastSearch.m_sText = sText;
}


void CFindTextDlg::
UpdateLastSearch ()
{
  SetLastSearch (m_sText, m_bMatchCase, m_bWholeWord, m_bRegExp, m_nDirection);
}

void CFindTextDlg::
UseLastSearch () 
{
  m_bMatchCase = lastSearch.m_bMatchCase;
  m_bWholeWord = lastSearch.m_bWholeWord;
  m_bRegExp = lastSearch.m_bRegExp;
  m_nDirection = lastSearch.m_nDirection;
  m_sText = lastSearch.m_sText;
}

