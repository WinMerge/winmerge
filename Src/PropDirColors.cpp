/** 
 * @file  PropDirColors.cpp
 *
 * @brief Implementation of PropDirColors propertysheet
 */

#include "stdafx.h"
#include "PropDirColors.h"
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
PropDirColors::PropDirColors(COptionsMgr *optionsMgr)
 : OptionsPanel(optionsMgr, PropDirColors::IDD)
 , m_bUseColors(false)
 , m_cCustColors()
{
}

void PropDirColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropDirColors)
	DDX_Check(pDX, IDC_USE_DIR_COMPARE_COLORS, m_bUseColors);
	DDX_Control(pDX, IDC_DIR_ITEM_EQUAL_COLOR, m_cDirItemEqual);
	DDX_Control(pDX, IDC_DIR_ITEM_EQUAL_TEXT_COLOR, m_cDirItemEqualText);
	DDX_Control(pDX, IDC_DIR_ITEM_DIFF_COLOR, m_cDirItemDiff);
	DDX_Control(pDX, IDC_DIR_ITEM_DIFF_TEXT_COLOR, m_cDirItemDiffText);
	DDX_Control(pDX, IDC_DIR_ITEM_NOTEXISTALL_COLOR, m_cDirItemNotExistAll);
	DDX_Control(pDX, IDC_DIR_ITEM_NOTEXISTALL_TEXT_COLOR, m_cDirItemNotExistAllText);
	DDX_Control(pDX, IDC_DIR_ITEM_FILTERED_COLOR, m_cDirItemFiltered);
	DDX_Control(pDX, IDC_DIR_ITEM_FILTERED_TEXT_COLOR, m_cDirItemFilteredText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropDirColors, CDialog)
	//{{AFX_MSG_MAP(PropDirColors)
	ON_BN_CLICKED(IDC_DIR_ITEM_EQUAL_COLOR, OnDirItemEqualColor)
	ON_BN_CLICKED(IDC_DIR_ITEM_EQUAL_TEXT_COLOR, OnDirItemEqualTextColor)
	ON_BN_CLICKED(IDC_DIR_ITEM_DIFF_COLOR, OnDirItemDiffColor)
	ON_BN_CLICKED(IDC_DIR_ITEM_DIFF_TEXT_COLOR, OnDirItemDiffTextColor)
	ON_BN_CLICKED(IDC_DIR_ITEM_NOTEXISTALL_COLOR, OnDirItemNotExistAllColor)
	ON_BN_CLICKED(IDC_DIR_ITEM_NOTEXISTALL_TEXT_COLOR, OnDirItemNotExistAllTextColor)
	ON_BN_CLICKED(IDC_DIR_ITEM_FILTERED_COLOR, OnDirItemFilteredColor)
	ON_BN_CLICKED(IDC_DIR_ITEM_FILTERED_TEXT_COLOR, OnDirItemFilteredTextColor)
	ON_BN_CLICKED(IDC_COLORDEFAULTS_BTN, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * (Property sheet calls this before displaying all property pages)
 */
void PropDirColors::ReadOptions()
{
	m_bUseColors = GetOptionsMgr()->GetBool(OPT_DIRCLR_USE_COLORS) ? true : false;
	SerializeColors(READ_OPTIONS);
}

/** 
 * @brief Writes options values from UI to storage.
 * (Property sheet calls this after displaying all property pages)
 */
void PropDirColors::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_DIRCLR_USE_COLORS, m_bUseColors);
	SerializeColors(WRITE_OPTIONS);
}

/** 
 * @brief Let user browse common color dialog, and select a color
 */
void PropDirColors::BrowseColor(CColorButton & colorButton)
{
	CColorDialog dialog(colorButton.GetColor());
	Options::CustomColors::Load(GetOptionsMgr(), m_cCustColors.data());
	dialog.m_cc.lpCustColors = m_cCustColors.data();
	
	if (dialog.DoModal() == IDOK)
		colorButton.SetColor(dialog.GetColor());
	Options::CustomColors::Save(GetOptionsMgr(), m_cCustColors.data());
}

/** 
 * @brief User wants to change equal color
 */
void PropDirColors::OnDirItemEqualColor()
{
	BrowseColor(m_cDirItemEqual);
}

/** 
 * @brief User wants to change equal text color
 */
void PropDirColors::OnDirItemEqualTextColor()
{
	BrowseColor(m_cDirItemEqualText);
}

/** 
 * @brief User wants to change diff color
 */
void PropDirColors::OnDirItemDiffColor()
{
	BrowseColor(m_cDirItemDiff);
}

/** 
 * @brief User wants to change diff text color
 */
void PropDirColors::OnDirItemDiffTextColor()
{
	BrowseColor(m_cDirItemDiffText);
}

/** 
 * @brief User wants to change not-exist-all color
 */
void PropDirColors::OnDirItemNotExistAllColor()
{
	BrowseColor(m_cDirItemNotExistAll);
}

/** 
 * @brief User wants to change not-exist-all text color
 */
void PropDirColors::OnDirItemNotExistAllTextColor()
{
	BrowseColor(m_cDirItemNotExistAllText);
}

/** 
 * @brief User wants to change filtered color
 */
void PropDirColors::OnDirItemFilteredColor()
{
	BrowseColor(m_cDirItemFiltered);
}

/**
 * @brief User wants to change filtered text color
 */
void PropDirColors::OnDirItemFilteredTextColor()
{
	BrowseColor(m_cDirItemFilteredText);
}

void PropDirColors::SerializeColors(OPERATION op)
{
	SerializeColor(op, m_cDirItemEqual, OPT_DIRCLR_ITEM_EQUAL);
	SerializeColor(op, m_cDirItemEqualText, OPT_DIRCLR_ITEM_EQUAL_TEXT);

	SerializeColor(op, m_cDirItemDiff, OPT_DIRCLR_ITEM_DIFF);
	SerializeColor(op, m_cDirItemDiffText, OPT_DIRCLR_ITEM_DIFF_TEXT);

	SerializeColor(op, m_cDirItemNotExistAll, OPT_DIRCLR_ITEM_NOT_EXIST_ALL);
	SerializeColor(op, m_cDirItemNotExistAllText, OPT_DIRCLR_ITEM_NOT_EXIST_ALL_TEXT);

	SerializeColor(op, m_cDirItemFiltered, OPT_DIRCLR_ITEM_FILTERED);
	SerializeColor(op, m_cDirItemFilteredText, OPT_DIRCLR_ITEM_FILTERED_TEXT);
}

void PropDirColors::SerializeColor(OPERATION op, CColorButton & btn, const String& optionName)
{
	switch (op)
	{
	case SET_DEFAULTS:
		btn.SetColor(GetOptionsMgr()->GetDefault<unsigned>(optionName));
		return;

	case WRITE_OPTIONS:
		GetOptionsMgr()->SaveOption(optionName, (unsigned)btn.GetColor());
		return;

	case READ_OPTIONS:
		// Set colors for buttons, do NOT invalidate
		btn.SetColor(GetOptionsMgr()->GetInt(optionName), false);
		return;
	}
}

/** 
 * @brief Resets colors to defaults
 */
void PropDirColors::OnDefaults()
{
	SerializeColors(SET_DEFAULTS);
}

