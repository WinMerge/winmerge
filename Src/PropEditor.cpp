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
static const unsigned MAX_TABSIZE = 64u;

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
	BindOptionCustom(OPT_TAB_SIZE, m_nTabSize, IDC_TAB_EDIT, DDX_Text,
		+[](unsigned v) { return v; }, +[](unsigned v) { return std::clamp(v, 1u, MAX_TABSIZE); });
	BindOption(OPT_TAB_TYPE, m_nTabType, IDC_PROP_INSERT_TABS, DDX_Radio);
	BindOption(OPT_SYNTAX_HIGHLIGHT, m_bHiliteSyntax, IDC_HILITE_CHECK, DDX_Check);
	BindOption(OPT_ALLOW_MIXED_EOL, m_bAllowMixedEol, IDC_MIXED_EOL, DDX_Check);
	BindOptionCustom(OPT_RENDERING_MODE, m_nRenderingMode, IDC_RENDERING_MODE, DDX_CBIndex,
		+[](int v) { return v + 1; }, +[](int v) { return (v - 1); });
}

/** 
 * @brief Function handling dialog data exchange between GUI and variables.
 */
void PropEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropEditor)
	DDV_MaxChars(pDX, std::to_string(m_nTabSize).c_str(), 2);
	// m_bCopyFullLine currently is only a hidden option
	//  > it is used here in PropEditor.cpp, because otherwise it doesn't get saved to the registry
	//}}AFX_DATA_MAP
	DoDataExchangeBindOptions(pDX);
}


BEGIN_MESSAGE_MAP(PropEditor, OptionsPanel)
	//{{AFX_MSG_MAP(PropEditor)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_EN_KILLFOCUS(IDC_TAB_EDIT, OnEnKillfocusTabEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

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
