// EncodingErrorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Merge.h"
#include "resource.h"
#include "EncodingErrorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEncodingErrorDlg dialog


CEncodingErrorDlg::CEncodingErrorDlg(const CString & sText)
: CDialog(CEncodingErrorDlg::IDD)
, m_sText(sText)
{
	//{{AFX_DATA_INIT(CEncodingErrorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEncodingErrorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEncodingErrorDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEncodingErrorDlg, CDialog)
	//{{AFX_MSG_MAP(CEncodingErrorDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_FILEENCODING, OnBnClickedEncoding)
	ON_BN_CLICKED(IDC_PLUGIN, OnBnClickedPlugin)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEncodingErrorDlg message handlers

BOOL CEncodingErrorDlg::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_STATIC, m_sText);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CEncodingErrorDlg::OnBnClickedEncoding()
{
	EndDialog(IDC_FILEENCODING);
}

void CEncodingErrorDlg::OnBnClickedPlugin()
{
	EndDialog(IDC_PLUGIN);
}
