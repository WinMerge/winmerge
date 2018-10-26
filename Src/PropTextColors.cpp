/** 
 * @file  PropTextColors.cpp
 *
 * @brief Implementation of PropTextColors propertysheet
 */

#include "stdafx.h"
#include "PropTextColors.h"
#include "SyntaxColors.h"
#include "OptionsCustomColors.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** @brief Section name for settings in registry. */
static const TCHAR Section[] = _T("Custom Colors");

/** 
 * @brief Default constructor.
 */
PropTextColors::PropTextColors(COptionsMgr *optionsMgr, SyntaxColors *pColors)
 : OptionsPanel(optionsMgr, PropTextColors::IDD)
, m_bCustomColors(false)
, m_pTempColors(pColors)
, m_cCustColors()
{
}

void PropTextColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropTextColors)
	DDX_Check(pDX, IDC_DEFAULT_STANDARD_COLORS, m_bCustomColors);
	DDX_Control(pDX, IDC_WHITESPACE_BKGD_COLOR, m_btnWhitespaceBackground);
	DDX_Control(pDX, IDC_REGULAR_BKGD_COLOR, m_btnRegularBackground);
	DDX_Control(pDX, IDC_REGULAR_TEXT_COLOR, m_btnRegularText);
	DDX_Control(pDX, IDC_SELECTION_BKGD_COLOR, m_btnSelectionBackground);
	DDX_Control(pDX, IDC_SELECTION_TEXT_COLOR, m_btnSelectionText);
	DDX_Control(pDX, IDC_MARGIN_BKGD_COLOR, m_btnMarginBackground);
	//}}AFX_DATA_MAP
	EnableColorButtons(m_bCustomColors);
}


BEGIN_MESSAGE_MAP(PropTextColors, CDialog)
	//{{AFX_MSG_MAP(PropTextColors)
	ON_BN_CLICKED(IDC_DEFAULT_STANDARD_COLORS, OnDefaultsStandardColors)
	ON_BN_CLICKED(IDC_WHITESPACE_BKGD_COLOR, OnWhitespaceBackgroundColor)
	ON_BN_CLICKED(IDC_REGULAR_BKGD_COLOR, OnRegularBackgroundColor)
	ON_BN_CLICKED(IDC_REGULAR_TEXT_COLOR, OnRegularTextColor)
	ON_BN_CLICKED(IDC_SELECTION_BKGD_COLOR, OnSelectionBackgroundColor)
	ON_BN_CLICKED(IDC_SELECTION_TEXT_COLOR, OnSelectionTextColor)
	ON_BN_CLICKED(IDC_MARGIN_BKGD_COLOR, OnMarginBackgroundColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * (Property sheet calls this before displaying all property pages)
 */
void PropTextColors::ReadOptions()
{
	m_bCustomColors = GetOptionsMgr()->GetBool(OPT_CLR_DEFAULT_TEXT_COLORING) ? false : true;
	SerializeColorsToFromScreen(LOAD_COLORS);
}

/** 
 * @brief Writes options values from UI to storage.
 * (Property sheet calls this after displaying all property pages)
 */
void PropTextColors::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_CLR_DEFAULT_TEXT_COLORING, !m_bCustomColors);
	// User can only change colors via BrowseColorAndSave,
	// which writes to m_pTempColors
	// so user's latest choices are in m_pTempColors
	// (we don't have to read them from screen)

	// Also, CPropSyntaxColors writes m_pTempColors out, so we don't have to
	// We share m_pTempColors with CPropSyntaxColors
}

/** 
 * @brief Let user browse common color dialog, and select a color
 * @param [in] colorButton Button for which to change color.
 * @param [in] colorIndex Index to color table.
 */
void PropTextColors::BrowseColorAndSave(CColorButton & colorButton, int colorIndex)
{
	// Ignore user if colors are slaved to system
	if (IsDlgButtonChecked(IDC_DEFAULT_STANDARD_COLORS) == BST_UNCHECKED)
		return;

	COLORREF currentColor = m_pTempColors->GetColor(colorIndex);
	CColorDialog dialog(currentColor);
	Options::CustomColors::Load(GetOptionsMgr(), m_cCustColors.data());
	dialog.m_cc.lpCustColors = m_cCustColors.data();
	
	if (dialog.DoModal() == IDOK)
	{
		currentColor = dialog.GetColor();
		colorButton.SetColor(currentColor);
		m_pTempColors->SetColor(colorIndex, currentColor);
	}
	Options::CustomColors::Save(GetOptionsMgr(), m_cCustColors.data());
}

/** 
 * @brief User wants to change whitespace color
 */
void PropTextColors::OnWhitespaceBackgroundColor() 
{
	BrowseColorAndSave(m_btnWhitespaceBackground, COLORINDEX_WHITESPACE);
}

/** 
 * @brief User wants to change regular background color
 */
void PropTextColors::OnRegularBackgroundColor() 
{
	BrowseColorAndSave(m_btnRegularBackground, COLORINDEX_BKGND);
}

/** 
 * @brief User wants to change regular text color
 */
void PropTextColors::OnRegularTextColor() 
{
	BrowseColorAndSave(m_btnRegularText, COLORINDEX_NORMALTEXT);
}

/** 
 * @brief User wants to change regular selection background color
 */
void PropTextColors::OnSelectionBackgroundColor() 
{
	BrowseColorAndSave(m_btnSelectionBackground, COLORINDEX_SELBKGND);
}

/** 
 * @brief User wants to change regular selection text color
 */
void PropTextColors::OnSelectionTextColor() 
{
	BrowseColorAndSave(m_btnSelectionText, COLORINDEX_SELTEXT);
}

/** 
 * @brief User wants to change margin background color
 */
void PropTextColors::OnMarginBackgroundColor() 
{
	BrowseColorAndSave(m_btnMarginBackground, COLORINDEX_SELMARGIN);
}

/**
 * @brief Load all colors, Save all colors, or set all colors to default
 * @param [in] op Operation to do, one of
 *  - SET_DEFAULTS : Sets colors to defaults
 *  - LOAD_COLORS : Loads colors from registry
 * (No save operation because BrowseColorAndSave saves immediately when user chooses)
 */
void PropTextColors::SerializeColorsToFromScreen(OPERATION op)
{
	if (op == SET_DEFAULTS)
		m_pTempColors->SetDefaults();

	SerializeColorToFromScreen(op, m_btnWhitespaceBackground, COLORINDEX_WHITESPACE);

	SerializeColorToFromScreen(op, m_btnRegularBackground, COLORINDEX_BKGND);
	SerializeColorToFromScreen(op, m_btnRegularText, COLORINDEX_NORMALTEXT);

	SerializeColorToFromScreen(op, m_btnSelectionBackground, COLORINDEX_SELBKGND);
	SerializeColorToFromScreen(op, m_btnSelectionText, COLORINDEX_SELTEXT);

	SerializeColorToFromScreen(op, m_btnMarginBackground, COLORINDEX_SELMARGIN);
}

/**
 * @brief Load color to button, Save color from button, or set button color to default
 * @param [in] op Operation to do, one of
 *  - SET_DEFAULTS : Sets colors to defaults
 *  - LOAD_COLORS : Loads colors from registry
 * (No save operation because BrowseColorAndSave saves immediately when user chooses)
 */
void PropTextColors::SerializeColorToFromScreen(OPERATION op, CColorButton & btn, int colorIndex)
{
	switch (op)
	{
	case SET_DEFAULTS:
	case LOAD_COLORS:
		btn.SetColor(m_pTempColors->GetColor(colorIndex));
		break;
	}
}

/** 
 * @brief Set colors to track standard (theme) colors
 */
void PropTextColors::OnDefaultsStandardColors()
{
	// Reset all text colors to default every time user checks defaults button
	SerializeColorsToFromScreen(SET_DEFAULTS);

	UpdateData();
}

/** 
 * @brief Enable / disable color controls on dialog.
 * @param [in] bEnable If `true` color controls are enabled.
 */
void PropTextColors::EnableColorButtons(bool bEnable)
{
	EnableDlgItem(IDC_CUSTOM_COLORS_GROUP, bEnable);
	EnableDlgItem(IDC_WHITESPACE_COLOR_LABEL, bEnable);
	EnableDlgItem(IDC_TEXT_COLOR_LABEL, bEnable);
	EnableDlgItem(IDC_SELECTION_COLOR_LABEL, bEnable);
	EnableDlgItem(IDC_MARGIN_COLOR_LABEL, bEnable);
	EnableDlgItem(IDC_BACKGROUND_COLUMN_LABEL, bEnable);
	EnableDlgItem(IDC_TEXT_COLUMN_LABEL, bEnable);
}
