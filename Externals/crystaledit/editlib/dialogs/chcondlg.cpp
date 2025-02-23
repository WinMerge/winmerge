///////////////////////////////////////////////////////////////////////////
//  File:    chcondlg.cpp
//  Version: 1.6.0.9
//  Updated: 17-Oct-1999
//
//  Copyright:  Ferdinand Prantl
//  E-mail:     prantl@ff.cuni.cz
//
//  Character encoding dialog
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "chcondlg.h"
#include "DDXHelper.h"
#include "../ccrystaltextview.h"
#include "../utils/cs2cs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

EDITPADC_CLASS const tchar_t* pszCodeNames[] =
  {
    _T ("ASCII (without accents)"),
    _T ("MS-Windows CP 1250"),
    _T ("MS-Windows CP 1252"),
    _T ("PC Latin I (CP 850)"),
    _T ("PC Latin II (CP 852)"),
    _T ("IBM Latin II (IBM 852)"),
    _T ("ISO-8859-1"),
    _T ("ISO-8859-2"),
    _T ("Brothers Kamenicky"),
    _T ("KOI8-CS"),
    _T ("Apple Macintosh"),
    _T ("Apple Macintosh for Central Europe"),
    _T ("Corky"),
    nullptr
  };

EDITPADC_CLASS int nCodeNames = sizeof (pszCodeNames) / sizeof (pszCodeNames[0]) - 1;

EDITPADC_CLASS void FillComboBox (CComboBox &Control, const tchar_t* *pszItems)
{
  Control.ResetContent();
  ASSERT (pszItems != nullptr);
  while (*pszItems)
    Control.AddString(*pszItems++);
}

/////////////////////////////////////////////////////////////////////////////
// CCharConvDlg dialog

CCharConvDlg::CCharConvDlg () : CDialog (CCharConvDlg::IDD, nullptr)
, m_nSource(1)
, m_nDest(1)
, m_bAlpha(false)
, m_sPreview(_T(""))
{
    //{{AFX_DATA_INIT(CCharConvDlg)
    //}}AFX_DATA_INIT
}

void CCharConvDlg::
DoDataExchange (CDataExchange * pDX)
{
  CDialog::DoDataExchange (pDX);
    //{{AFX_DATA_MAP(CCharConvDlg)
    DDX_Control(pDX, IDC_SRC, m_ctlSource);
    DDX_Control(pDX, IDC_DEST, m_ctlDest);
    DDX_CBIndex(pDX, IDC_SRC, m_nSource);
    DDX_CBIndex(pDX, IDC_DEST, m_nDest);
    DDX_Check(pDX, IDC_ALPHA_ONLY, m_bAlpha);
    DDX_Text(pDX, IDC_PREVIEW, m_sPreview);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP (CCharConvDlg, CDialog)
    //{{AFX_MSG_MAP(CCharConvDlg)
    ON_BN_CLICKED(IDPREVIEW, OnPreview)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP ()

/////////////////////////////////////////////////////////////////////////////
// CCharConvDlg message handlers

static int nSource = 1;
static int nDest = 1;

BOOL CCharConvDlg::
OnInitDialog ()
{
  CDialog::OnInitDialog ();
  FillComboBox (m_ctlSource, pszCodeNames);
  FillComboBox (m_ctlDest, pszCodeNames);
  m_nSource = nSource;
  m_nDest = nSource;
  m_sPreview = m_sOriginal;
  UpdateData (false);
  return true;
}

void CCharConvDlg::OnOK ()
{
  CDialog::OnOK ();
  nSource = m_nSource;
  nDest = m_nSource;
}

void CCharConvDlg::OnPreview() 
{
  UpdateData ();
  tchar_t* pszNew = nullptr;
  if (!iconvert_new (m_sOriginal, &pszNew, m_nSource, m_nDest, m_bAlpha))
    {
      m_sPreview = pszNew;
      UpdateData (false);
    }
  free (pszNew);
}
