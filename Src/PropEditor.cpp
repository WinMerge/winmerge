/** 
 * @file  PropEditor.cpp
 *
 * @brief Implementation of PropEditor propertysheet
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "PropEditor.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** @brief Maximum size for tabs in spaces. */
static const int MAX_TABSIZE = 64;

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to options manager for handling options.
 */
PropEditor::PropEditor(COptionsMgr *optionsMgr) 
: OptionsPanel(optionsMgr, PropEditor::IDD)
, m_bHiliteSyntax(FALSE)
, m_nTabType(-1)
, m_nTabSize(0)
, m_bAutomaticRescan(FALSE)
, m_bAllowMixedEol(FALSE)
, m_bViewLineDifferences(FALSE)
, m_bBreakOnWords(FALSE)
, m_nBreakType(0)
{
}

/** 
 * @brief Function handling dialog data exchange between GUI and variables.
 */
void PropEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropEditor)
	DDX_Check(pDX, IDC_HILITE_CHECK, m_bHiliteSyntax);
	DDX_Radio(pDX, IDC_PROP_INSERT_TABS, m_nTabType);
	DDX_Text(pDX, IDC_TAB_EDIT, m_nTabSize);
	DDX_Check(pDX, IDC_AUTOMRESCAN_CHECK, m_bAutomaticRescan);
	DDX_Check(pDX, IDC_MIXED_EOL, m_bAllowMixedEol);
	DDX_Check(pDX, IDC_VIEW_LINE_DIFFERENCES, m_bViewLineDifferences);
	DDX_Radio(pDX, IDC_EDITOR_CHARLEVEL, m_bBreakOnWords);
	DDX_CBIndex(pDX, IDC_BREAK_TYPE, m_nBreakType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropEditor, CDialog)
	//{{AFX_MSG_MAP(PropEditor)
	ON_BN_CLICKED(IDC_VIEW_LINE_DIFFERENCES, OnLineDiffControlClicked)
	ON_BN_CLICKED(IDC_EDITOR_CHARLEVEL, OnLineDiffControlClicked)
	ON_BN_CLICKED(IDC_EDITOR_WORDLEVEL, OnLineDiffControlClicked)
	ON_EN_KILLFOCUS(IDC_TAB_EDIT, OnEnKillfocusTabEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropEditor::ReadOptions()
{
	m_nTabSize = GetOptionsMgr()->GetInt(OPT_TAB_SIZE);
	m_nTabType = GetOptionsMgr()->GetInt(OPT_TAB_TYPE);
	m_bAutomaticRescan = GetOptionsMgr()->GetBool(OPT_AUTOMATIC_RESCAN);
	m_bHiliteSyntax = GetOptionsMgr()->GetBool(OPT_SYNTAX_HIGHLIGHT);
	m_bAllowMixedEol = GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL);
	m_bViewLineDifferences = GetOptionsMgr()->GetBool(OPT_WORDDIFF_HIGHLIGHT);
	m_bBreakOnWords = GetOptionsMgr()->GetBool(OPT_BREAK_ON_WORDS);
	m_nBreakType = GetOptionsMgr()->GetInt(OPT_BREAK_TYPE);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropEditor::WriteOptions()
{
	// Sanity check tabsize
	if (m_nTabSize < 1)
		m_nTabSize = 1;
	if (m_nTabSize > MAX_TABSIZE)
		m_nTabSize = MAX_TABSIZE;
	GetOptionsMgr()->SaveOption(OPT_TAB_SIZE, (int)m_nTabSize);
	GetOptionsMgr()->SaveOption(OPT_TAB_TYPE, (int)m_nTabType);
	GetOptionsMgr()->SaveOption(OPT_AUTOMATIC_RESCAN, m_bAutomaticRescan == TRUE);
	GetOptionsMgr()->SaveOption(OPT_ALLOW_MIXED_EOL, m_bAllowMixedEol == TRUE);
	GetOptionsMgr()->SaveOption(OPT_SYNTAX_HIGHLIGHT, m_bHiliteSyntax == TRUE);
	GetOptionsMgr()->SaveOption(OPT_WORDDIFF_HIGHLIGHT, !!m_bViewLineDifferences);
	GetOptionsMgr()->SaveOption(OPT_BREAK_ON_WORDS, !!m_bBreakOnWords);
	GetOptionsMgr()->SaveOption(OPT_BREAK_TYPE, m_nBreakType);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropEditor::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();

	CEdit * pEdit = (CEdit *) GetDlgItem(IDC_TAB_EDIT);

	// Limit max text of tabsize to 2 chars
	if (pEdit != NULL)
		pEdit->SetLimitText(2);

	LoadBreakTypeStrings();
	UpdateDataToWindow();
	UpdateLineDiffControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Load strings (from resource) into combobox for break type
 */
void PropEditor::LoadBreakTypeStrings()
{
	CComboBox * cbo = (CComboBox *)GetDlgItem(IDC_BREAK_TYPE);
	cbo->AddString(theApp.LoadString(IDS_BREAK_ON_WHITESPACE).c_str());
	cbo->AddString(theApp.LoadString(IDS_BREAK_ON_PUNCTUATION).c_str());
}

/**
 * @brief Handlers any clicks in any of the line differencing controls
 */
void PropEditor::OnLineDiffControlClicked()
{
	UpdateLineDiffControls();
}

/**
 * @brief Shortcut to enable or disable a control
 * @param [in] item ID of dialog control to enable/disable.
 * @param [in] enable if true control is enabled, else disabled.
 */
void PropEditor::EnableDlgItem(int item, bool enable)
{
	GetDlgItem(item)->EnableWindow(!!enable);
}

/** 
 * @brief Update availability of line difference controls
 */
void PropEditor::UpdateLineDiffControls()
{
	UpdateDataFromWindow();
	// Can only choose char/word level if line differences are enabled
	EnableDlgItem(IDC_EDITOR_CHARLEVEL, !!m_bViewLineDifferences);
	EnableDlgItem(IDC_EDITOR_WORDLEVEL, !!m_bViewLineDifferences);
	// Can only choose break type if line differences are enabled & we're breaking on words
	EnableDlgItem(IDC_BREAK_TYPE, !!m_bViewLineDifferences);
}

/** 
 * @brief Check tabsize value when control loses focus.
 */
void PropEditor::OnEnKillfocusTabEdit()
{
	CEdit * pEdit = (CEdit *)GetDlgItem(IDC_TAB_EDIT);
	CString valueAsText;
	pEdit->GetWindowText(valueAsText);
	int value = _ttoi(valueAsText);
	
	if (value < 1 || value > MAX_TABSIZE)
	{
		CString msg;
		CString num;
		num.Format(_T("%d"), MAX_TABSIZE);
		LangFormatString1(msg, IDS_OPTIONS_INVALID_TABSIZE, num);
		AfxMessageBox(msg, MB_ICONWARNING);
	}
}
