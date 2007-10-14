/** 
 * @file  SharedFilterDlg.cpp
 *
 * @brief Dialog where user choose shared or private filter
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "SharedFilterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSharedFilterDlg dialog


CSharedFilterDlg::CSharedFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSharedFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSharedFilterDlg)
	//}}AFX_DATA_INIT
}


void CSharedFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSharedFilterDlg)
	DDX_Control(pDX, IDC_SHARED, m_SharedButton);
	DDX_Control(pDX, IDC_PRIVATE, m_PrivateButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSharedFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CSharedFilterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSharedFilterDlg message handlers

BOOL CSharedFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (AfxGetApp()->GetProfileInt(_T("Filters"), _T("Shared"), 0))
		m_SharedButton.SetCheck(BST_CHECKED);
	else
		m_PrivateButton.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSharedFilterDlg::OnOK() 
{
	BOOL bShared = (m_SharedButton.GetCheck() == BST_CHECKED);
	AfxGetApp()->WriteProfileInt(_T("Filters"), _T("Shared"), bShared);
	if (bShared)
		m_ChosenFolder = m_SharedFolder;
	else
		m_ChosenFolder = m_PrivateFolder;

	CDialog::OnOK();
}

String
CSharedFilterDlg::PromptForNewFilter(CWnd * Parent, String SharedFolder, String PrivateFolder)
{
	CSharedFilterDlg dlg(Parent);
	dlg.m_SharedFolder = SharedFolder;
	dlg.m_PrivateFolder = PrivateFolder;
	if (dlg.DoModal() != IDOK)
		return _T("");
	return dlg.m_ChosenFolder;
}

