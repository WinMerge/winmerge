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
, m_bAllowMixedEol(false)
, m_nRenderingMode(0)
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
	DDX_Check(pDX, IDC_MIXED_EOL, m_bAllowMixedEol);
	// m_bCopyFullLine currently is only a hidden option
	//  > it is used here in PropEditor.cpp, because otherwise it doesn't get saved to the registry
	DDX_CBIndex(pDX, IDC_RENDERING_MODE, m_nRenderingMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropEditor, OptionsPanel)
	//{{AFX_MSG_MAP(PropEditor)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
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
	m_bHiliteSyntax = GetOptionsMgr()->GetBool(OPT_SYNTAX_HIGHLIGHT);
	m_bAllowMixedEol = GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL);
	m_nRenderingMode = GetOptionsMgr()->GetInt(OPT_RENDERING_MODE) + 1;
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
	GetOptionsMgr()->SaveOption(OPT_ALLOW_MIXED_EOL, m_bAllowMixedEol);
	GetOptionsMgr()->SaveOption(OPT_SYNTAX_HIGHLIGHT, m_bHiliteSyntax);
	GetOptionsMgr()->SaveOption(OPT_RENDERING_MODE, m_nRenderingMode - 1);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropEditor::OnInitDialog() 
{
	OptionsPanel::OnInitDialog();

	LoadComboBoxStrings();
	UpdateDataToWindow();
#ifndef _WIN64
	EnableDlgItem(IDC_RENDERING_MODE, false);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Load strings (from resource) into combobox for break type
 */
void PropEditor::LoadComboBoxStrings()
{
	SetDlgItemComboBoxList(IDC_RENDERING_MODE,
		{ _("GDI"), _("DirectWrite Default"), _("DirectWrite Aliased"), _("DirectWrite GDI Classic"), _("DirectWrite GDI Natural"), _("DirectWrite Natural"), _("DirectWrite Natural Symmetric") });
}

/**
 * @brief Sets options to defaults
 */
void PropEditor::OnDefaults()
{
	m_nTabSize = GetOptionsMgr()->GetDefault<unsigned>(OPT_TAB_SIZE);
	m_nTabType = GetOptionsMgr()->GetDefault<unsigned>(OPT_TAB_TYPE);
	m_bHiliteSyntax = GetOptionsMgr()->GetDefault<bool>(OPT_SYNTAX_HIGHLIGHT);
	m_bAllowMixedEol = GetOptionsMgr()->GetDefault<bool>(OPT_ALLOW_MIXED_EOL);
	m_nRenderingMode = GetOptionsMgr()->GetDefault<unsigned>(OPT_RENDERING_MODE) + 1;
	UpdateData(FALSE);
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
			_("Tab size value is out of range. Please use 1 - %1."),
			strutils::to_str(MAX_TABSIZE));
		AfxMessageBox(msg.c_str(), MB_ICONWARNING);
	}
}
