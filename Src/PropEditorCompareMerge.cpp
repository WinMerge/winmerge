/** 
 * @file  PropEditorCompareMerge.cpp
 *
 * @brief Implementation of PropEditorCompareMerge propertysheet
 */

#include "stdafx.h"
#include "PropEditorCompareMerge.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to options manager for handling options.
 */
PropEditorCompareMerge::PropEditorCompareMerge(COptionsMgr *optionsMgr) 
: OptionsPanel(optionsMgr, PropEditorCompareMerge::IDD)
, m_bAutomaticRescan(false)
, m_nCopyGranularity(0)
, m_bViewLineDifferences(false)
, m_bBreakOnWords(false)
, m_nBreakType(0)
{
}

/** 
 * @brief Function handling dialog data exchange between GUI and variables.
 */
void PropEditorCompareMerge::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropEditorCompareMerge)
	DDX_Check(pDX, IDC_AUTOMRESCAN_CHECK, m_bAutomaticRescan);
	DDX_CBIndex(pDX, IDC_COPY_GRANULARITY, m_nCopyGranularity);
	DDX_Check(pDX, IDC_VIEW_LINE_DIFFERENCES, m_bViewLineDifferences);
	DDX_Radio(pDX, IDC_EDITOR_CHARLEVEL, m_bBreakOnWords);
	DDX_CBIndex(pDX, IDC_BREAK_TYPE, m_nBreakType);
	DDX_Text(pDX, IDC_BREAK_CHARS, m_breakChars);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropEditorCompareMerge, OptionsPanel)
	//{{AFX_MSG_MAP(PropEditorCompareMerge)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_BN_CLICKED(IDC_VIEW_LINE_DIFFERENCES, OnLineDiffControlClicked)
	ON_BN_CLICKED(IDC_EDITOR_CHARLEVEL, OnLineDiffControlClicked)
	ON_BN_CLICKED(IDC_EDITOR_WORDLEVEL, OnLineDiffControlClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropEditorCompareMerge::ReadOptions()
{
	m_bAutomaticRescan = GetOptionsMgr()->GetBool(OPT_AUTOMATIC_RESCAN);
	m_nCopyGranularity = GetOptionsMgr()->GetInt(OPT_COPY_GRANULARITY);
	m_bViewLineDifferences = GetOptionsMgr()->GetBool(OPT_WORDDIFF_HIGHLIGHT);
	m_bBreakOnWords = GetOptionsMgr()->GetBool(OPT_BREAK_ON_WORDS);
	m_nBreakType = GetOptionsMgr()->GetInt(OPT_BREAK_TYPE);
	m_breakChars = GetOptionsMgr()->GetString(OPT_BREAK_SEPARATORS);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropEditorCompareMerge::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_AUTOMATIC_RESCAN, m_bAutomaticRescan);
	GetOptionsMgr()->SaveOption(OPT_COPY_GRANULARITY, m_nCopyGranularity);
	GetOptionsMgr()->SaveOption(OPT_WORDDIFF_HIGHLIGHT, m_bViewLineDifferences);
	GetOptionsMgr()->SaveOption(OPT_BREAK_ON_WORDS, m_bBreakOnWords);
	GetOptionsMgr()->SaveOption(OPT_BREAK_TYPE, m_nBreakType);
	GetOptionsMgr()->SaveOption(OPT_BREAK_SEPARATORS, String(m_breakChars));
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropEditorCompareMerge::OnInitDialog() 
{
	OptionsPanel::OnInitDialog();

	LoadComboBoxStrings();
	UpdateDataToWindow();
	UpdateLineDiffControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Load strings (from resource) into combobox for break type
 */
void PropEditorCompareMerge::LoadComboBoxStrings()
{
	SetDlgItemComboBoxList(IDC_BREAK_TYPE,
		{ _("Break at whitespace"), _("Break at whitespace or punctuation") });
	SetDlgItemComboBoxList(IDC_COPY_GRANULARITY,
		{ _("Diff hunk"), _("Inline diff"), _("Line"), _("Character") });
}

/**
 * @brief Sets options to defaults
 */
void PropEditorCompareMerge::OnDefaults()
{
	m_bAutomaticRescan = GetOptionsMgr()->GetDefault<bool>(OPT_AUTOMATIC_RESCAN);
	m_nCopyGranularity = GetOptionsMgr()->GetDefault<unsigned>(OPT_COPY_GRANULARITY);
	m_bViewLineDifferences = GetOptionsMgr()->GetDefault<bool>(OPT_WORDDIFF_HIGHLIGHT);
	m_bBreakOnWords = GetOptionsMgr()->GetDefault<bool>(OPT_BREAK_ON_WORDS);
	m_nBreakType = GetOptionsMgr()->GetDefault<unsigned>(OPT_BREAK_TYPE);
	m_breakChars = GetOptionsMgr()->GetDefault<String>(OPT_BREAK_SEPARATORS);

	UpdateDataToWindow();
	UpdateLineDiffControls();
}

/**
 * @brief Handlers any clicks in any of the line differencing controls
 */
void PropEditorCompareMerge::OnLineDiffControlClicked()
{
	UpdateLineDiffControls();
}

/** 
 * @brief Update availability of line difference controls
 */
void PropEditorCompareMerge::UpdateLineDiffControls()
{
	UpdateDataFromWindow();
	// Can only choose char/word level if line differences are enabled
	EnableDlgItem(IDC_EDITOR_CHARLEVEL, m_bViewLineDifferences);
	EnableDlgItem(IDC_EDITOR_WORDLEVEL, m_bViewLineDifferences);
	// Can only choose break type if line differences are enabled & we're breaking on words
	EnableDlgItem(IDC_BREAK_TYPE, m_bViewLineDifferences);
}

