/** 
 * @file  PropMarkerColors.cpp
 *
 * @brief Implementation of PropMarkerColors propertysheet
 */

#include "stdafx.h"
#include "PropMarkerColors.h"
#include "SyntaxColors.h"
#include "OptionsCustomColors.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Default constructor.
 */
PropMarkerColors::PropMarkerColors(COptionsMgr *optionsMgr, SyntaxColors *pColors)
 : OptionsPanel(optionsMgr, PropMarkerColors::IDD)
, m_pTempColors(pColors)
, m_cCustColors()
{
}

void PropMarkerColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropMarkerColors)
	DDX_Control(pDX, IDC_MARKER0_BKGD_COLOR, m_btnMarkerColors[0]);
	DDX_Control(pDX, IDC_MARKER1_BKGD_COLOR, m_btnMarkerColors[1]);
	DDX_Control(pDX, IDC_MARKER2_BKGD_COLOR, m_btnMarkerColors[2]);
	DDX_Control(pDX, IDC_MARKER3_BKGD_COLOR, m_btnMarkerColors[3]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropMarkerColors, CDialog)
	//{{AFX_MSG_MAP(PropMarkerColors)
	ON_COMMAND_RANGE(IDC_MARKER0_BKGD_COLOR, IDC_MARKER3_BKGD_COLOR, OnMarkerColors)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * (Property sheet calls this before displaying all property pages)
 */
void PropMarkerColors::ReadOptions()
{
	SerializeColorsToFromScreen(LOAD_COLORS);
}

/** 
 * @brief Writes options values from UI to storage.
 * (Property sheet calls this after displaying all property pages)
 */
void PropMarkerColors::WriteOptions()
{
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
void PropMarkerColors::BrowseColorAndSave(CColorButton & colorButton, int colorIndex)
{
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
void PropMarkerColors::OnMarkerColors(UINT nID) 
{
	BrowseColorAndSave(m_btnMarkerColors[nID - IDC_MARKER0_BKGD_COLOR], COLORINDEX_MARKERBKGND0 + nID - IDC_MARKER0_BKGD_COLOR);
}

/**
 * @brief Load all colors, Save all colors, or set all colors to default
 * @param [in] op Operation to do, one of
 *  - SET_DEFAULTS : Sets colors to defaults
 *  - LOAD_COLORS : Loads colors from registry
 * (No save operation because BrowseColorAndSave saves immediately when user chooses)
 */
void PropMarkerColors::SerializeColorsToFromScreen(OPERATION op)
{
	if (op == SET_DEFAULTS)
		m_pTempColors->SetDefaults();

	for (int i = 0; i < 4; ++i)
		SerializeColorToFromScreen(op, m_btnMarkerColors[i], COLORINDEX_MARKERBKGND0 + i);
}

/**
 * @brief Load color to button, Save color from button, or set button color to default
 * @param [in] op Operation to do, one of
 *  - SET_DEFAULTS : Sets colors to defaults
 *  - LOAD_COLORS : Loads colors from registry
 * (No save operation because BrowseColorAndSave saves immediately when user chooses)
 */
void PropMarkerColors::SerializeColorToFromScreen(OPERATION op, CColorButton & btn, int colorIndex)
{
	switch (op)
	{
	case SET_DEFAULTS:
	case LOAD_COLORS:
		btn.SetColor(m_pTempColors->GetColor(colorIndex));
		break;
	}
}
