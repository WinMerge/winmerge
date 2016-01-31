/** 
 * @file  SharedFilterDlg.cpp
 *
 * @brief Dialog where user choose shared or private filter
 */

#include "stdafx.h"
#include "SharedFilterDlg.h"
#include "Merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CSharedFilterDlg dialog


/**
 * @brief A constructor.
 */
CSharedFilterDlg::CSharedFilterDlg(FilterType type, CWnd* pParent /*=NULL*/)
	: CDialog(CSharedFilterDlg::IDD, pParent), m_selectedType(type)
{
}


void CSharedFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSharedFilterDlg)
	DDX_Radio(pDX, IDC_SHARED, *(reinterpret_cast<int *>(&m_selectedType)));
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSharedFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CSharedFilterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSharedFilterDlg message handlers

/**
 * @brief Dialog initialization.
 */
BOOL CSharedFilterDlg::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
