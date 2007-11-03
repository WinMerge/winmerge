// OutputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "merge.h"
#include "OutputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputDlg dialog


COutputDlg::COutputDlg(const CString & sTitle, const CString & sText)
: CDialog(COutputDlg::IDD)
, m_sTitle(sTitle)
, m_sText(sText)
{
	//{{AFX_DATA_INIT(COutputDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COutputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COutputDlg, CDialog)
	//{{AFX_MSG_MAP(COutputDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputDlg message handlers

BOOL COutputDlg::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	SetWindowText(m_sTitle);
	SetDlgItemText(IDC_EDIT1, m_sText);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void OutputBox(LPCTSTR szTitle, LPCTSTR szText)
{
	COutputDlg dlg(szTitle, szText);
	dlg.DoModal();
}
