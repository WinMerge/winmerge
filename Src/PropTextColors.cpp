/** 
 * @file  PropTextColors.cpp
 *
 * @brief Implementation of CPropTextColors propertysheet
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "resource.h"
#include "Merge.h"
#include "SyntaxColors.h"
#include "PropTextColors.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/** @brief Section name for settings in registry. */
static const TCHAR Section[] = _T("Custom Colors");

/////////////////////////////////////////////////////////////////////////////
// CPropTextColors dialog

/** 
 * @brief Default constructor.
 */
CPropTextColors::CPropTextColors(COptionsMgr *optionsMgr, SyntaxColors *pColors)
 : CPropertyPage(CPropTextColors::IDD)
, m_pOptionsMgr(optionsMgr)
, m_bCustomColors(FALSE)
, m_pTempColors(pColors)
{
}

CPropTextColors::~CPropTextColors()
{
}

void CPropTextColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropTextColors)
	DDX_Control(pDX, IDC_DEFAULT_STANDARD_COLORS, m_btnDefaultStandardColors);
	DDX_Check(pDX, IDC_DEFAULT_STANDARD_COLORS, m_bCustomColors);
	DDX_Control(pDX, IDC_WHITESPACE_BKGD_COLOR, m_btnWhitespaceBackground);
	DDX_Control(pDX, IDC_REGULAR_BKGD_COLOR, m_btnRegularBackground);
	DDX_Control(pDX, IDC_REGULAR_TEXT_COLOR, m_btnRegularText);
	DDX_Control(pDX, IDC_SELECTION_BKGD_COLOR, m_btnSelectionBackground);
	DDX_Control(pDX, IDC_SELECTION_TEXT_COLOR, m_btnSelectionText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropTextColors, CDialog)
	//{{AFX_MSG_MAP(CPropTextColors)
	ON_BN_CLICKED(IDC_DEFAULT_STANDARD_COLORS, OnDefaultsStandardColors)
	ON_BN_CLICKED(IDC_WHITESPACE_BKGD_COLOR, OnWhitespaceBackgroundColor)
	ON_BN_CLICKED(IDC_REGULAR_BKGD_COLOR, OnRegularBackgroundColor)
	ON_BN_CLICKED(IDC_REGULAR_TEXT_COLOR, OnRegularTextColor)
	ON_BN_CLICKED(IDC_SELECTION_BKGD_COLOR, OnSelectionBackgroundColor)
	ON_BN_CLICKED(IDC_SELECTION_TEXT_COLOR, OnSelectionTextColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Enable/Disable controls when dialog is shown.
 */
BOOL CPropTextColors::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();

	if (m_bCustomColors)
		EnableColorButtons(TRUE);
	else
		EnableColorButtons(FALSE);
    
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/** 
 * @brief Reads options values from storage to UI.
 * (Property sheet calls this before displaying all property pages)
 */
void CPropTextColors::ReadOptions()
{
	m_bCustomColors = GetOptionsMgr()->GetBool(OPT_CLR_DEFAULT_TEXT_COLORING) ? FALSE : TRUE;
	SerializeColorsToFromScreen(LOAD_COLORS);
}

/** 
 * @brief Writes options values from UI to storage.
 * (Property sheet calls this after displaying all property pages)
 */
void CPropTextColors::WriteOptions()
{
	m_pOptionsMgr->SaveOption(OPT_CLR_DEFAULT_TEXT_COLORING, m_bCustomColors == FALSE);
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
void CPropTextColors::BrowseColorAndSave(CColorButton & colorButton, int colorIndex)
{
	// Ignore user if colors are slaved to system
	if (m_btnDefaultStandardColors.GetCheck() == BST_UNCHECKED)
		return;

	COLORREF currentColor = m_pTempColors->GetColor(colorIndex);
	CColorDialog dialog(currentColor);
	LoadCustomColors();
	dialog.m_cc.lpCustColors = m_cCustColors;
	
	if (dialog.DoModal() == IDOK)
	{
		currentColor = dialog.GetColor();
		colorButton.SetColor(currentColor);
		m_pTempColors->SetColor(colorIndex, currentColor);
	}
	SaveCustomColors();
}

/** 
 * @brief User wants to change whitespace color
 */
void CPropTextColors::OnWhitespaceBackgroundColor() 
{
	BrowseColorAndSave(m_btnWhitespaceBackground, COLORINDEX_WHITESPACE);
}

/** 
 * @brief User wants to change regular background color
 */
void CPropTextColors::OnRegularBackgroundColor() 
{
	BrowseColorAndSave(m_btnRegularBackground, COLORINDEX_BKGND);
}

/** 
 * @brief User wants to change regular text color
 */
void CPropTextColors::OnRegularTextColor() 
{
	BrowseColorAndSave(m_btnRegularText, COLORINDEX_NORMALTEXT);
}

/** 
 * @brief User wants to change regular selection background color
 */
void CPropTextColors::OnSelectionBackgroundColor() 
{
	BrowseColorAndSave(m_btnSelectionBackground, COLORINDEX_SELBKGND);
}

/** 
 * @brief User wants to change regular selection text color
 */
void CPropTextColors::OnSelectionTextColor() 
{
	BrowseColorAndSave(m_btnSelectionText, COLORINDEX_SELTEXT);
}

/**
 * @brief Load all colors, Save all colors, or set all colors to default
 * @param [in] op Operation to do, one of
 *  - SET_DEFAULTS : Sets colors to defaults
 *  - LOAD_COLORS : Loads colors from registry
 * (No save operation because BrowseColorAndSave saves immediately when user chooses)
 */
void CPropTextColors::SerializeColorsToFromScreen(OPERATION op)
{
	if (op == SET_DEFAULTS)
		m_pTempColors->SetDefaults();

	SerializeColorToFromScreen(op, m_btnWhitespaceBackground, COLORINDEX_WHITESPACE);

	SerializeColorToFromScreen(op, m_btnRegularBackground, COLORINDEX_BKGND);
	SerializeColorToFromScreen(op, m_btnRegularText, COLORINDEX_NORMALTEXT);

	SerializeColorToFromScreen(op, m_btnSelectionBackground, COLORINDEX_SELBKGND);
	SerializeColorToFromScreen(op, m_btnSelectionText, COLORINDEX_SELTEXT);
}

/**
 * @brief Load color to button, Save color from button, or set button color to default
 * @param [in] op Operation to do, one of
 *  - SET_DEFAULTS : Sets colors to defaults
 *  - LOAD_COLORS : Loads colors from registry
 * (No save operation because BrowseColorAndSave saves immediately when user chooses)
 */
void CPropTextColors::SerializeColorToFromScreen(OPERATION op, CColorButton & btn, int colorIndex)
{
	
	switch (op)
	{
	case SET_DEFAULTS:
		{
		COLORREF color = m_pTempColors->GetColor(colorIndex);
		btn.SetColor(color);
		return;
		}

	case LOAD_COLORS:
		{
		COLORREF color = m_pTempColors->GetColor(colorIndex);
		// Set colors for buttons, do NOT invalidate
		btn.SetColor(color, FALSE);
		return;
		}
	}
}

/** 
 * @brief Set colors to track standard (theme) colors
 */
void CPropTextColors::OnDefaultsStandardColors()
{
	// Reset all text colors to default every time user checks defaults button
	SerializeColorsToFromScreen(SET_DEFAULTS);

	CButton * btn = (CButton *)GetDlgItem(IDC_DEFAULT_STANDARD_COLORS);
	if (btn->GetCheck() == BST_UNCHECKED)
	{
		EnableColorButtons(FALSE);
	}
	else
	{
		EnableColorButtons(TRUE);
	}
}

/** 
 * @brief Loads color selection dialog's custom colors from registry
 */
void CPropTextColors::LoadCustomColors()
{
	SyntaxColors_Load(m_cCustColors, sizeof(m_cCustColors)/sizeof(m_cCustColors[0]));
}

/** 
 * @brief Saves color selection dialog's custom colors to registry
 */
void CPropTextColors::SaveCustomColors()
{
	SyntaxColors_Save(m_cCustColors, sizeof(m_cCustColors)/sizeof(m_cCustColors[0]));
}

/** 
 * @brief Enable / disable color controls on dialog.
 * @param [in] bEnable If TRUE color controls are enabled.
 */
void CPropTextColors::EnableColorButtons(BOOL bEnable)
{
	CStatic * stc = (CStatic *) GetDlgItem(IDC_CUSTOM_COLORS_GROUP);
	stc->EnableWindow(bEnable);
	stc = (CStatic *) GetDlgItem(IDC_WHITESPACE_COLOR_LABEL);
	stc->EnableWindow(bEnable);
	stc = (CStatic *) GetDlgItem(IDC_TEXT_COLOR_LABEL);
	stc->EnableWindow(bEnable);
	stc = (CStatic *) GetDlgItem(IDC_SELECTION_COLOR_LABEL);
	stc->EnableWindow(bEnable);
	stc = (CStatic *) GetDlgItem(IDC_BACKGROUND_COLUMN_LABEL);
	stc->EnableWindow(bEnable);
	stc = (CStatic *) GetDlgItem(IDC_TEXT_COLUMN_LABEL);
	stc->EnableWindow(bEnable);
}
