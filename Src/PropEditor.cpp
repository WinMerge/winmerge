// PropEditor.cpp : implementation file
//

#include "stdafx.h"
#include "merge.h"
#include "PropEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int MAX_TABSIZE = 64;

/////////////////////////////////////////////////////////////////////////////
// CPropEditor dialog

IMPLEMENT_DYNCREATE(CPropEditor, CPropertyPage)

CPropEditor::CPropEditor() : CPropertyPage(CPropEditor::IDD)
{
	//{{AFX_DATA_INIT(CPropEditor)
	m_bHiliteSyntax = FALSE;
	m_nTabType = -1;
	m_nTabSize = 0;
	m_bAutomaticRescan = FALSE;
	m_bAllowMixedEol = FALSE;
	//}}AFX_DATA_INIT
}


void CPropEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropEditor)
	DDX_Check(pDX, IDC_HILITE_CHECK, m_bHiliteSyntax);
	DDX_Radio(pDX, IDC_PROP_INSERT_TABS, m_nTabType);
	DDX_Text(pDX, IDC_TAB_EDIT, m_nTabSize);
	DDX_Check(pDX, IDC_AUTOMRESCAN_CHECK, m_bAutomaticRescan);
	DDX_Check(pDX, IDC_MIXED_EOL, m_bAllowMixedEol);
	DDV_MinMaxInt(pDX, m_nTabSize, 1, MAX_TABSIZE);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropEditor, CDialog)
	//{{AFX_MSG_MAP(CPropEditor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropEditor message handlers

BOOL CPropEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CPropertyPage::OnInitDialog();
	CEdit * pEdit = (CEdit *) GetDlgItem(IDC_TAB_EDIT);

	// Limit max text of tabsize to 2 chars
	if (pEdit != NULL)
		pEdit->SetLimitText(2);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

