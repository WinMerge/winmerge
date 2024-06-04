///////////////////////////////////////////////////////////////////////////
//  File:    gotodlg.cpp
//  Version: 1.1.0.4
//  Updated: 19-Jul-1998
//
//  Copyright:  Ferdinand Prantl
//  E-mail:     prantl@ff.cuni.cz
//
//  Go to line dialog
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "gotodlg.h"
#include "../ccrystaltextview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CGotoDlg dialog

CGotoDlg::CGotoDlg (CCrystalTextView * pBuddy):CDialog (CGotoDlg::IDD, nullptr)
, m_pBuddy(pBuddy)
, m_sNumber(_T(""))
{
  //{{AFX_DATA_INIT(CGotoDlg)
  //}}AFX_DATA_INIT
}

void CGotoDlg::
DoDataExchange (CDataExchange * pDX)
{
  CDialog::DoDataExchange (pDX);
  //{{AFX_DATA_MAP(CGotoDlg)
  DDX_Control (pDX, IDC_NUMBER, m_ctlNumber);
  DDX_Text (pDX, IDC_NUMBER, m_sNumber);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP (CGotoDlg, CDialog)
//{{AFX_MSG_MAP(CGotoDlg)
ON_EN_CHANGE (IDC_NUMBER, OnChangeNumber)
//}}AFX_MSG_MAP
END_MESSAGE_MAP ()

/////////////////////////////////////////////////////////////////////////////
// CGotoDlg message handlers

void CGotoDlg::OnOK ()
{
  CDialog::OnOK ();
  ASSERT (m_pBuddy != nullptr);
  m_pBuddy->GoToLine (tc::ttoi (m_sNumber), tc::tcschr (_T ("+-"), *(const tchar_t*)m_sNumber) != nullptr);
}

void CGotoDlg::
OnChangeNumber ()
{
  CString text;
  GetDlgItem (IDC_NUMBER)->GetWindowText (text);
  GetDlgItem (IDOK)->EnableWindow (!text.IsEmpty ());
}

BOOL CGotoDlg::
OnInitDialog ()
{
  CDialog::OnInitDialog ();

  GetDlgItem (IDOK)->EnableWindow (!m_sNumber.IsEmpty ());

  return true;
}
