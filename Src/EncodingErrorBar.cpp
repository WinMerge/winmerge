// EncodingErrorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EncodingErrorBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CEncodingErrorBar dialog


CEncodingErrorBar::CEncodingErrorBar()
{
	//{{AFX_DATA_INIT(CEncodingErrorBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEncodingErrorBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEncodingErrorBar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEncodingErrorBar, CDialogBar)
	//{{AFX_MSG_MAP(CEncodingErrorBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEncodingErrorBar message handlers

BOOL CEncodingErrorBar::Create(CWnd *pParentWnd) 
{
	if (! CTrDialogBar::Create(pParentWnd, CEncodingErrorBar::IDD, 
			CBRS_BOTTOM | CBRS_TOOLTIPS | CBRS_FLYBY, CEncodingErrorBar::IDD))
		return FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEncodingErrorBar::SetText(const String& sText)
{
	SetDlgItemText((unsigned)IDC_STATIC, sText);
}
