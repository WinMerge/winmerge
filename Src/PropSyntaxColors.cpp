/** 
 * @file  PropSyntaxColors.cpp
 *
 * @brief Implementation of PropSyntaxColors propertysheet
 */

#include "stdafx.h"
#include "PropSyntaxColors.h"
#include "SyntaxColors.h"
#include "OptionsSyntaxColors.h"
#include "OptionsCustomColors.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const unsigned ColorIndices[] = {
	COLORINDEX_KEYWORD, COLORINDEX_FUNCNAME, COLORINDEX_COMMENT,
	COLORINDEX_NUMBER, COLORINDEX_OPERATOR, COLORINDEX_STRING,
	COLORINDEX_PREPROCESSOR, COLORINDEX_USER1, COLORINDEX_USER2
};

IMPLEMENT_DYNAMIC(PropSyntaxColors, CPropertyPage)

PropSyntaxColors::PropSyntaxColors(COptionsMgr *optionsMgr, SyntaxColors *pColors)
: OptionsPanel(optionsMgr, PropSyntaxColors::IDD)
, m_nBolds()
, m_pTempColors(pColors)
, m_cCustColors()
{
}

void PropSyntaxColors::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCOLOR_KEYWORDS, m_colorButtons[COLORINDEX_KEYWORD]);
	DDX_Control(pDX, IDC_SCOLOR_FUNCTIONS, m_colorButtons[COLORINDEX_FUNCNAME]);
	DDX_Control(pDX, IDC_SCOLOR_COMMENTS, m_colorButtons[COLORINDEX_COMMENT]);
	DDX_Control(pDX, IDC_SCOLOR_NUMBERS, m_colorButtons[COLORINDEX_NUMBER]);
	DDX_Control(pDX, IDC_SCOLOR_OPERATORS, m_colorButtons[COLORINDEX_OPERATOR]);
	DDX_Control(pDX, IDC_SCOLOR_STRINGS, m_colorButtons[COLORINDEX_STRING]);
	DDX_Control(pDX, IDC_SCOLOR_PREPROCESSOR, m_colorButtons[COLORINDEX_PREPROCESSOR]);
	DDX_Control(pDX, IDC_SCOLOR_USER1, m_colorButtons[COLORINDEX_USER1]);
	DDX_Control(pDX, IDC_SCOLOR_USER2, m_colorButtons[COLORINDEX_USER2]);
	DDX_Check(pDX, IDC_SCOLOR_KEYWORDS_BOLD, m_nBolds[COLORINDEX_KEYWORD]);
	DDX_Check(pDX, IDC_SCOLOR_FUNCTIONS_BOLD, m_nBolds[COLORINDEX_FUNCNAME]);
	DDX_Check(pDX, IDC_SCOLOR_COMMENTS_BOLD, m_nBolds[COLORINDEX_COMMENT]);
	DDX_Check(pDX, IDC_SCOLOR_NUMBERS_BOLD, m_nBolds[COLORINDEX_NUMBER]);
	DDX_Check(pDX, IDC_SCOLOR_OPERATORS_BOLD, m_nBolds[COLORINDEX_OPERATOR]);
	DDX_Check(pDX, IDC_SCOLOR_STRINGS_BOLD, m_nBolds[COLORINDEX_STRING]);
	DDX_Check(pDX, IDC_SCOLOR_PREPROCESSOR_BOLD, m_nBolds[COLORINDEX_PREPROCESSOR]);
	DDX_Check(pDX, IDC_SCOLOR_USER1_BOLD, m_nBolds[COLORINDEX_USER1]);
	DDX_Check(pDX, IDC_SCOLOR_USER2_BOLD, m_nBolds[COLORINDEX_USER2]);
}


BEGIN_MESSAGE_MAP(PropSyntaxColors, CPropertyPage)
	ON_BN_CLICKED(IDC_SCOLOR_KEYWORDS, OnBnClickedEcolor<COLORINDEX_KEYWORD>)
	ON_BN_CLICKED(IDC_SCOLOR_FUNCTIONS, OnBnClickedEcolor<COLORINDEX_FUNCNAME>)
	ON_BN_CLICKED(IDC_SCOLOR_COMMENTS, OnBnClickedEcolor<COLORINDEX_COMMENT>)
	ON_BN_CLICKED(IDC_SCOLOR_NUMBERS, OnBnClickedEcolor<COLORINDEX_NUMBER>)
	ON_BN_CLICKED(IDC_SCOLOR_OPERATORS, OnBnClickedEcolor<COLORINDEX_OPERATOR>)
	ON_BN_CLICKED(IDC_SCOLOR_STRINGS, OnBnClickedEcolor<COLORINDEX_STRING>)
	ON_BN_CLICKED(IDC_SCOLOR_PREPROCESSOR, OnBnClickedEcolor<COLORINDEX_PREPROCESSOR>)
	ON_BN_CLICKED(IDC_SCOLOR_USER1, OnBnClickedEcolor<COLORINDEX_USER1>)
	ON_BN_CLICKED(IDC_SCOLOR_USER2, OnBnClickedEcolor<COLORINDEX_USER2>)
	ON_BN_CLICKED(IDC_SCOLORS_BDEFAULTS, OnBnClickedEcolorsBdefaults)
	ON_BN_CLICKED(IDC_SCOLOR_KEYWORDS_BOLD, (OnBnClickedBoldButton<IDC_SCOLOR_KEYWORDS_BOLD, COLORINDEX_KEYWORD>))
	ON_BN_CLICKED(IDC_SCOLOR_FUNCTIONS_BOLD, (OnBnClickedBoldButton<IDC_SCOLOR_FUNCTIONS_BOLD, COLORINDEX_FUNCNAME>))
	ON_BN_CLICKED(IDC_SCOLOR_COMMENTS_BOLD, (OnBnClickedBoldButton<IDC_SCOLOR_COMMENTS_BOLD, COLORINDEX_COMMENT>))
	ON_BN_CLICKED(IDC_SCOLOR_NUMBERS_BOLD, (OnBnClickedBoldButton<IDC_SCOLOR_NUMBERS_BOLD, COLORINDEX_NUMBER>))
	ON_BN_CLICKED(IDC_SCOLOR_OPERATORS_BOLD, (OnBnClickedBoldButton<IDC_SCOLOR_OPERATORS_BOLD, COLORINDEX_OPERATOR>))
	ON_BN_CLICKED(IDC_SCOLOR_STRINGS_BOLD, (OnBnClickedBoldButton<IDC_SCOLOR_STRINGS_BOLD, COLORINDEX_STRING>))
	ON_BN_CLICKED(IDC_SCOLOR_PREPROCESSOR_BOLD, (OnBnClickedBoldButton<IDC_SCOLOR_PREPROCESSOR_BOLD, COLORINDEX_PREPROCESSOR>))
	ON_BN_CLICKED(IDC_SCOLOR_USER1_BOLD, (OnBnClickedBoldButton<IDC_SCOLOR_USER1_BOLD, COLORINDEX_USER1>))
	ON_BN_CLICKED(IDC_SCOLOR_USER2_BOLD, (OnBnClickedBoldButton<IDC_SCOLOR_USER2_BOLD, COLORINDEX_USER2>))
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * (Property sheet calls this before displaying all property pages)
 */
void PropSyntaxColors::ReadOptions()
{
	// Set colors for buttons
	for (auto&& colorIndex : ColorIndices)
	{
		m_colorButtons[colorIndex].SetColor(m_pTempColors->GetColor(colorIndex));
		m_nBolds[colorIndex] = GetCheckVal(colorIndex);
	}
}

/** 
 * @brief Writes options values from UI to storage.
 * (Property sheet calls this after displaying all property pages)
 */
void PropSyntaxColors::WriteOptions()
{
	// User can only change colors via BrowseColorAndSave,
	// which writes to m_pTempColors
	// so user's latest choices are in m_pTempColors
	// (we don't have to read them from screen)
	Options::SyntaxColors::Save(GetOptionsMgr(), m_pTempColors);
}

/** 
 * @brief Let user browse common color dialog, and select a color & save to registry
 */
void PropSyntaxColors::BrowseColorAndSave(unsigned colorIndex)
{
	COLORREF currentColor = m_pTempColors->GetColor(colorIndex);
	CColorDialog dialog(currentColor);
	Options::CustomColors::Load(GetOptionsMgr(), m_cCustColors.data());
	dialog.m_cc.lpCustColors = m_cCustColors.data();
	
	if (dialog.DoModal() == IDOK)
	{
		currentColor = dialog.GetColor();
		m_colorButtons[colorIndex].SetColor(currentColor);
		m_pTempColors->SetColor(colorIndex, currentColor);
	}
	Options::CustomColors::Save(GetOptionsMgr(), m_cCustColors.data());
}

template <unsigned colorIndex>
void PropSyntaxColors::OnBnClickedEcolor()
{
	BrowseColorAndSave(colorIndex);
}

void PropSyntaxColors::OnBnClickedEcolorsBdefaults()
{
	m_pTempColors->SetDefaults();
	for (auto&& colorIndex : ColorIndices)
	{
		m_colorButtons[colorIndex].SetColor(m_pTempColors->GetColor(colorIndex));
		m_nBolds[colorIndex] = GetCheckVal(colorIndex);
	}

	UpdateData(FALSE);
}

template <unsigned ctlId, unsigned colorIndex>
void PropSyntaxColors::OnBnClickedBoldButton()
{
	m_pTempColors->SetBold(colorIndex, IsDlgButtonChecked(ctlId) == BST_CHECKED);
}

int PropSyntaxColors::GetCheckVal(UINT nColorIndex)
{
	if (m_pTempColors->GetBold(nColorIndex))
		return BST_CHECKED;
	else
		return BST_UNCHECKED;
}
