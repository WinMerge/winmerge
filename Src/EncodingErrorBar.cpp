// EncodingErrorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EncodingErrorBar.h"
#include "Merge.h"

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
	if (! CDialogBar::Create(pParentWnd, CEncodingErrorBar::IDD, 
			CBRS_BOTTOM | CBRS_TOOLTIPS | CBRS_FLYBY, CEncodingErrorBar::IDD))
		return FALSE;

	theApp.TranslateDialog(m_hWnd);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEncodingErrorBar::SetText(const CString& sText)
{
	SetDlgItemText(IDC_STATIC, sText);
}
