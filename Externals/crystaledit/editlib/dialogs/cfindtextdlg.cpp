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

#include "StdAfx.h"
#include "cfindtextdlg.h"
#include "DDXHelper.h"
#include "../ccrystaltextview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindTextDlg dialog

CFindTextDlg::CFindTextDlg (CCrystalTextView * pBuddy)
: CDialog (CFindTextDlg::IDD, nullptr)
, m_pBuddy(pBuddy)
, m_nDirection(1)
, m_bMatchCase(false)
, m_bWholeWord(false)
, m_bRegExp(false)
, m_bNoWrap(false)
, m_bNoClose(true)
, lastSearch({0})
{
  ASSERT (pBuddy != nullptr);
  Create(CFindTextDlg::IDD,pBuddy);
}

void CFindTextDlg::
DoDataExchange (CDataExchange * pDX)
{
  CDialog::DoDataExchange (pDX);
  //{{AFX_DATA_MAP(CFindTextDlg)
  DDX_Control (pDX, IDC_EDIT_FINDTEXT, m_ctlFindText);
  DDX_Control (pDX, IDC_EDIT_WHOLE_WORD, m_ctlWholeWord);
  DDX_Check (pDX, IDC_EDIT_MATCH_CASE, m_bMatchCase);
  DDX_CBStringExact (pDX, IDC_EDIT_FINDTEXT, m_sText);
  DDX_Check (pDX, IDC_EDIT_WHOLE_WORD, m_bWholeWord);
  DDX_Check (pDX, IDC_EDIT_REGEXP, m_bRegExp);
  DDX_Check (pDX, IDC_FINDDLG_DONTWRAP, m_bNoWrap);
  DDX_Check (pDX, IDC_FINDDLG_DONTCLOSE, m_bNoClose);
  if (!pDX->m_bSaveAndValidate)
    {
      m_ctlFindText.m_sGroup = _T ("FindText");
      m_ctlFindText.SetFocus ();
      UpdateControls();
    }
  //}}AFX_DATA_MAP
}

void CFindTextDlg::
UpdateRegExp ()
{
  if (m_bRegExp)
    {
      m_ctlWholeWord.EnableWindow (false);
      m_bWholeWord = false;
    }
  else
    {
      m_ctlWholeWord.EnableWindow (true);
    }
}

void CFindTextDlg::
FindText (int nDirection)
{
  if (UpdateData ())
    {
      m_ctlFindText.FillCurrent();
      m_nDirection = nDirection;
      UpdateLastSearch ();

      ASSERT (m_pBuddy != nullptr);

      if (!m_pBuddy->FindText(GetLastSearchInfos()))
        {
          CString prompt, text(m_sText);
          prompt.Format (LoadResString(IDS_EDIT_TEXT_NOT_FOUND).c_str(), (const tchar_t*)text);
          AfxMessageBox (prompt, MB_ICONINFORMATION);
        }
      else
        {
          CMemComboBox::SaveSettings();
          if (!m_bNoClose)
            {
              CDialog::OnOK ();
              m_pBuddy->SetFocus ();
            }
        }
    }
}


BEGIN_MESSAGE_MAP (CFindTextDlg, CDialog)
//{{AFX_MSG_MAP(CFindTextDlg)
ON_CBN_EDITCHANGE (IDC_EDIT_FINDTEXT, OnChangeEditText)
ON_CBN_SELCHANGE (IDC_EDIT_FINDTEXT, OnChangeSelected)
ON_BN_CLICKED (IDC_EDIT_REGEXP, OnRegExp)
ON_BN_CLICKED (IDC_EDIT_FINDPREV, OnFindPrev)
//}}AFX_MSG_MAP
END_MESSAGE_MAP ()

/////////////////////////////////////////////////////////////////////////////
// CFindTextDlg message handlers

void CFindTextDlg::OnOK ()
{
  FindText (1);
}

void CFindTextDlg::OnFindPrev ()
{
  FindText (0);
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
  LangTranslateDialog(m_hWnd);
  CDialog::OnInitDialog ();

  CMemComboBox::LoadSettings();
  
  return true;
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
  m_pBuddy->SetFocus ();
}

void CFindTextDlg::
OnRegExp ()
{
  UpdateData (true);
  UpdateRegExp ();
  UpdateData (false);
}


//
// Update controls, enabling/disabling according to what's appropriate
//
void CFindTextDlg::
UpdateControls()
{
  GetDlgItem(IDOK)->EnableWindow( !m_sText.IsEmpty() );
  GetDlgItem(IDC_EDIT_FINDPREV)->EnableWindow( !m_sText.IsEmpty() );
  
  UpdateRegExp();
}

//
// Last search functions
//
void CFindTextDlg::
SetLastSearch (const tchar_t* sText, bool bMatchCase, bool bWholeWord, bool bRegExp, int nDirection)
{
  lastSearch.m_bMatchCase = bMatchCase;
  lastSearch.m_bWholeWord = bWholeWord;
  lastSearch.m_bRegExp = bRegExp;
  lastSearch.m_nDirection = nDirection;
  lastSearch.m_sText = sText;
  lastSearch.m_bNoWrap = m_bNoWrap;
  lastSearch.m_bNoClose = m_bNoClose;
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
  m_bNoWrap = lastSearch.m_bNoWrap;
  m_bNoClose = lastSearch.m_bNoClose;
}

void CFindTextDlg::PostNcDestroy()
{
  delete this;
}
