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
#include "MyColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Default constructor.
 */
PropDirColors::PropDirColors(COptionsMgr *optionsMgr)
 : OptionsPanel(optionsMgr, PropDirColors::IDD)
 , m_bUseColors(false)
 , m_clrDirItemEqual(0)
 , m_clrDirItemEqualText(0)
 , m_clrDirItemDiff(0)
 , m_clrDirItemDiffText(0)
 , m_clrDirItemNotExistAll(0)
 , m_clrDirItemNotExistAllText(0)
 , m_clrDirItemFiltered(0)
 , m_clrDirItemFilteredText(0)
 , m_clrDirMargin(0)
 , m_cCustColors()
{
	BindOption(OPT_DIRCLR_USE_COLORS, m_bUseColors, IDC_USE_DIR_COMPARE_COLORS, DDX_Check);
	BindOption(OPT_DIRCLR_ITEM_EQUAL, m_clrDirItemEqual, IDC_DIR_ITEM_EQUAL_COLOR, DDX_ColorButton);
	BindOption(OPT_DIRCLR_ITEM_EQUAL_TEXT, m_clrDirItemEqualText, IDC_DIR_ITEM_EQUAL_TEXT_COLOR, DDX_ColorButton);
	BindOption(OPT_DIRCLR_ITEM_DIFF, m_clrDirItemDiff, IDC_DIR_ITEM_DIFF_COLOR, DDX_ColorButton);
	BindOption(OPT_DIRCLR_ITEM_DIFF_TEXT, m_clrDirItemDiffText, IDC_DIR_ITEM_DIFF_TEXT_COLOR, DDX_ColorButton);
	BindOption(OPT_DIRCLR_ITEM_NOT_EXIST_ALL, m_clrDirItemNotExistAll, IDC_DIR_ITEM_NOTEXISTALL_COLOR, DDX_ColorButton);
	BindOption(OPT_DIRCLR_ITEM_NOT_EXIST_ALL_TEXT, m_clrDirItemNotExistAllText, IDC_DIR_ITEM_NOTEXISTALL_TEXT_COLOR, DDX_ColorButton);
	BindOption(OPT_DIRCLR_ITEM_FILTERED, m_clrDirItemFiltered, IDC_DIR_ITEM_FILTERED_COLOR, DDX_ColorButton);
	BindOption(OPT_DIRCLR_ITEM_FILTERED_TEXT, m_clrDirItemFilteredText, IDC_DIR_ITEM_FILTERED_TEXT_COLOR, DDX_ColorButton);
	BindOption(OPT_DIRCLR_MARGIN, m_clrDirMargin, IDC_DIR_MARGIN_COLOR, DDX_ColorButton);
}

void PropDirColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropDirColors)
	DDX_Control(pDX, IDC_DIR_ITEM_EQUAL_COLOR, m_cDirItemEqual);
	DDX_Control(pDX, IDC_DIR_ITEM_EQUAL_TEXT_COLOR, m_cDirItemEqualText);
	DDX_Control(pDX, IDC_DIR_ITEM_DIFF_COLOR, m_cDirItemDiff);
	DDX_Control(pDX, IDC_DIR_ITEM_DIFF_TEXT_COLOR, m_cDirItemDiffText);
	DDX_Control(pDX, IDC_DIR_ITEM_NOTEXISTALL_COLOR, m_cDirItemNotExistAll);
	DDX_Control(pDX, IDC_DIR_ITEM_NOTEXISTALL_TEXT_COLOR, m_cDirItemNotExistAllText);
	DDX_Control(pDX, IDC_DIR_ITEM_FILTERED_COLOR, m_cDirItemFiltered);
	DDX_Control(pDX, IDC_DIR_ITEM_FILTERED_TEXT_COLOR, m_cDirItemFilteredText);
	DDX_Control(pDX, IDC_DIR_MARGIN_COLOR, m_cDirMargin);
	//}}AFX_DATA_MAP
	DoDataExchangeBindOptions(pDX);
}


BEGIN_MESSAGE_MAP(PropDirColors, OptionsPanel)
	//{{AFX_MSG_MAP(PropDirColors)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_DIR_ITEM_EQUAL_COLOR, IDC_DIR_MARGIN_COLOR, BrowseColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Let user browse common color dialog, and select a color
 */
void PropDirColors::BrowseColor(unsigned id)
{
	CColorButton& colorButton = (CColorButton&)*GetDlgItem(id);
	CMyColorDialog dialog(colorButton.GetColor());
	Options::CustomColors::Load(GetOptionsMgr(), m_cCustColors.data());
	dialog.m_cc.lpCustColors = m_cCustColors.data();
	
	if (dialog.DoModal() == IDOK)
		colorButton.SetColor(dialog.GetColor());
	Options::CustomColors::Save(GetOptionsMgr(), m_cCustColors.data());
}

