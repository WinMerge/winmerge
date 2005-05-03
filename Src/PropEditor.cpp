/** 
 * @file  PropEditor.cpp
 *
 * @brief Implementation of CPropEditor propertysheet
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

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
	m_bApplySyntax = FALSE;
	m_bViewLineDifferences = FALSE;
	m_bBreakOnWords = FALSE;
	m_nBreakType = 0;
	//}}AFX_DATA_INIT
}


void CPropEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropEditor)
	DDX_Check(pDX, IDC_HILITE_CHECK, m_bHiliteSyntax);
	DDX_Radio(pDX, IDC_PROP_INSERT_TABS, m_nTabType);
	DDX_Text(pDX, IDC_TAB_EDIT, m_nTabSize);
	DDV_MinMaxInt(pDX, m_nTabSize, 1, MAX_TABSIZE);
	DDX_Check(pDX, IDC_AUTOMRESCAN_CHECK, m_bAutomaticRescan);
	DDX_Check(pDX, IDC_MIXED_EOL, m_bAllowMixedEol);
	DDX_Check(pDX, IDC_UNREC_APPLYSYNTAX, m_bApplySyntax);
	DDX_Check(pDX, IDC_VIEW_LINE_DIFFERENCES, m_bViewLineDifferences);
	DDX_Check(pDX, IDC_BREAK_ON_WORDS, m_bBreakOnWords);
	DDX_CBIndex(pDX, IDC_BREAK_TYPE, m_nBreakType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropEditor, CDialog)
	//{{AFX_MSG_MAP(CPropEditor)
	ON_BN_CLICKED(IDC_HILITE_CHECK, OnSyntaxHighlight)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropEditor message handlers

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL CPropEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CPropertyPage::OnInitDialog();
	CEdit * pEdit = (CEdit *) GetDlgItem(IDC_TAB_EDIT);

	// Limit max text of tabsize to 2 chars
	if (pEdit != NULL)
		pEdit->SetLimitText(2);

	// Enable/disable "Apply to unrecognized side" checkbox
	if (IsDlgButtonChecked(IDC_HILITE_CHECK))
		GetDlgItem(IDC_UNREC_APPLYSYNTAX)->EnableWindow(TRUE);
	else
	{
		GetDlgItem(IDC_UNREC_APPLYSYNTAX)->EnableWindow(FALSE);
	}

	LoadBreakTypeStrings();
	UpdateDataToWindow();

	/// @TODO Need to implement the option controlled by this control, 2005-05-03, Perry
	GetDlgItem(IDC_BREAK_ON_WORDS)->ShowWindow(SW_HIDE);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Load strings (from resource) into combobox for break type
 */
void CPropEditor::LoadBreakTypeStrings()
{
	CComboBox * cbo = (CComboBox *)GetDlgItem(IDC_BREAK_TYPE);
	cbo->AddString(LoadResString(IDS_BREAK_ON_WHITESPACE));
	cbo->AddString(LoadResString(IDS_BREAK_ON_PUNCTUATION));
}

/** 
 * @brief Enable/Disable "Apply to other side" checkbox.
 *
 * "Apply to other side" checkbox is enabled only when syntax
 * highlight is enabled.
 */
void CPropEditor::OnSyntaxHighlight()
{
	if (IsDlgButtonChecked(IDC_HILITE_CHECK))
		GetDlgItem(IDC_UNREC_APPLYSYNTAX)->EnableWindow(TRUE);
	else
	{
		GetDlgItem(IDC_UNREC_APPLYSYNTAX)->EnableWindow(FALSE);
		CheckDlgButton(IDC_UNREC_APPLYSYNTAX, FALSE);
		m_bApplySyntax = FALSE;
	}
}
