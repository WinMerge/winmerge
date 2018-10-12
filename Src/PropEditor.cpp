/** 
 * @file  PropEditor.cpp
 *
 * @brief Implementation of PropEditor propertysheet
 */

#include "stdafx.h"
#include "PropEditor.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** @brief Maximum size for tabs in spaces. */
static const int MAX_TABSIZE = 64;

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to options manager for handling options.
 */
PropEditor::PropEditor(COptionsMgr *optionsMgr) 
: OptionsPanel(optionsMgr, PropEditor::IDD)
, m_bHiliteSyntax(false)
, m_nTabType(-1)
, m_nTabSize(0)
, m_bAutomaticRescan(false)
, m_bAllowMixedEol(false)
, m_bViewLineDifferences(false)
, m_bBreakOnWords(false)
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
	DDV_MaxChars(pDX, std::to_string(m_nTabSize).c_str(), 2);
	DDX_Check(pDX, IDC_AUTOMRESCAN_CHECK, m_bAutomaticRescan);
	DDX_Check(pDX, IDC_MIXED_EOL, m_bAllowMixedEol);
	DDX_Check(pDX, IDC_VIEW_LINE_DIFFERENCES, m_bViewLineDifferences);
	DDX_Radio(pDX, IDC_EDITOR_CHARLEVEL, m_bBreakOnWords);
	DDX_CBIndex(pDX, IDC_BREAK_TYPE, m_nBreakType);
	DDX_Text(pDX, IDC_BREAK_CHARS, m_breakChars);
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
	m_breakChars = GetOptionsMgr()->GetString(OPT_BREAK_SEPARATORS);
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
	GetOptionsMgr()->SaveOption(OPT_AUTOMATIC_RESCAN, m_bAutomaticRescan);
	GetOptionsMgr()->SaveOption(OPT_ALLOW_MIXED_EOL, m_bAllowMixedEol);
	GetOptionsMgr()->SaveOption(OPT_SYNTAX_HIGHLIGHT, m_bHiliteSyntax);
	GetOptionsMgr()->SaveOption(OPT_WORDDIFF_HIGHLIGHT, m_bViewLineDifferences);
	GetOptionsMgr()->SaveOption(OPT_BREAK_ON_WORDS, m_bBreakOnWords);
	GetOptionsMgr()->SaveOption(OPT_BREAK_TYPE, m_nBreakType);
	GetOptionsMgr()->SaveOption(OPT_BREAK_SEPARATORS, String(m_breakChars));
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropEditor::OnInitDialog() 
{
	OptionsPanel::OnInitDialog();

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
	cbo->AddString(_("Break at whitespace").c_str());
	cbo->AddString(_("Break at whitespace or punctuation").c_str());
}

/**
 * @brief Handlers any clicks in any of the line differencing controls
 */
void PropEditor::OnLineDiffControlClicked()
{
	UpdateLineDiffControls();
}

/** 
 * @brief Update availability of line difference controls
 */
void PropEditor::UpdateLineDiffControls()
{
	UpdateDataFromWindow();
	// Can only choose char/word level if line differences are enabled
	EnableDlgItem(IDC_EDITOR_CHARLEVEL, m_bViewLineDifferences);
	EnableDlgItem(IDC_EDITOR_WORDLEVEL, m_bViewLineDifferences);
	// Can only choose break type if line differences are enabled & we're breaking on words
	EnableDlgItem(IDC_BREAK_TYPE, m_bViewLineDifferences);
}

/** 
 * @brief Check tabsize value when control loses focus.
 */
void PropEditor::OnEnKillfocusTabEdit()
{
	unsigned value = GetDlgItemInt(IDC_TAB_EDIT);
	if (value < 1 || value > MAX_TABSIZE)
	{
		String msg = strutils::format_string1(
			_("Value in Tab size -field is not in range WinMerge accepts.\n\nPlease use values 1 - %1."),
			strutils::to_str(MAX_TABSIZE));
		AfxMessageBox(msg.c_str(), MB_ICONWARNING);
	}
}
