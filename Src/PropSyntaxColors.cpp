/** 
 * @file  PropSyntaxColors.cpp
 *
 * @brief Implementation of CPropSyntaxColors propertysheet
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "resource.h"
#include "SyntaxColors.h"
#include "PropSyntaxColors.h"
#include "Merge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CPropEditorColor dialog

IMPLEMENT_DYNAMIC(CPropSyntaxColors, CPropertyPage)

CPropSyntaxColors::CPropSyntaxColors(COptionsMgr *optionsMgr, SyntaxColors *pColors)
: CPropertyPage(CPropSyntaxColors::IDD)
, m_pOptionsMgr(optionsMgr)
, m_nKeywordsBold(0)
, m_nFunctionsBold(0)
, m_nCommentsBold(0)
, m_nNumbersBold(0)
, m_nOperatorsBold(0)
, m_nStringsBold(0)
, m_nPreProcessorBold(0)
, m_nUser1Bold(0)
, m_nUser2Bold(0)
{
	m_pTempColors = pColors;

	// Set colors for buttons, do NOT invalidate
	m_btnKeywordsText.SetColor(m_pTempColors->GetColor(COLORINDEX_KEYWORD), FALSE);
	m_btnFunctionsText.SetColor(m_pTempColors->GetColor(COLORINDEX_FUNCNAME), FALSE);
	m_btnCommentsText.SetColor(m_pTempColors->GetColor(COLORINDEX_COMMENT), FALSE);
	m_btnNumbersText.SetColor(m_pTempColors->GetColor(COLORINDEX_NUMBER), FALSE);
	m_btnOperatorsText.SetColor(m_pTempColors->GetColor(COLORINDEX_OPERATOR), FALSE);
	m_btnStringsText.SetColor(m_pTempColors->GetColor(COLORINDEX_STRING), FALSE);
	m_btnPreprocessorText.SetColor(m_pTempColors->GetColor(COLORINDEX_PREPROCESSOR), FALSE);
	m_btnUser1Text.SetColor(m_pTempColors->GetColor(COLORINDEX_USER1), FALSE);
	m_btnUser2Text.SetColor(m_pTempColors->GetColor(COLORINDEX_USER2), FALSE);

	m_nKeywordsBold = GetCheckVal(COLORINDEX_KEYWORD);
	m_nFunctionsBold = GetCheckVal(COLORINDEX_FUNCNAME);
	m_nCommentsBold = GetCheckVal(COLORINDEX_COMMENT);
	m_nNumbersBold = GetCheckVal(COLORINDEX_NUMBER);
	m_nOperatorsBold = GetCheckVal(COLORINDEX_OPERATOR);
	m_nStringsBold = GetCheckVal(COLORINDEX_STRING);
	m_nPreProcessorBold = GetCheckVal(COLORINDEX_PREPROCESSOR);
	m_nUser1Bold = GetCheckVal(COLORINDEX_USER1);
	m_nUser2Bold = GetCheckVal(COLORINDEX_USER2);
}

CPropSyntaxColors::~CPropSyntaxColors()
{
}

void CPropSyntaxColors::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCOLOR_KEYWORDS, m_btnKeywordsText);
	DDX_Control(pDX, IDC_SCOLOR_FUNCTIONS, m_btnFunctionsText);
	DDX_Control(pDX, IDC_SCOLOR_COMMENTS, m_btnCommentsText);
	DDX_Control(pDX, IDC_SCOLOR_NUMBERS, m_btnNumbersText);
	DDX_Control(pDX, IDC_SCOLOR_OPERATORS, m_btnOperatorsText);
	DDX_Control(pDX, IDC_SCOLOR_STRINGS, m_btnStringsText);
	DDX_Control(pDX, IDC_SCOLOR_PREPROCESSOR, m_btnPreprocessorText);
	DDX_Control(pDX, IDC_SCOLOR_USER1, m_btnUser1Text);
	DDX_Control(pDX, IDC_SCOLOR_USER2, m_btnUser2Text);
	DDX_Control(pDX, IDC_SCOLOR_KEYWORDS_BOLD, m_btnKeywordsBold);
	DDX_Control(pDX, IDC_SCOLOR_FUNCTIONS_BOLD, m_btnFunctionsBold);
	DDX_Control(pDX, IDC_SCOLOR_COMMENTS_BOLD, m_btnCommentsBold);
	DDX_Control(pDX, IDC_SCOLOR_NUMBERS_BOLD, m_btnNumbersBold);
	DDX_Control(pDX, IDC_SCOLOR_OPERATORS_BOLD, m_btnOperatorsBold);
	DDX_Control(pDX, IDC_SCOLOR_STRINGS_BOLD, m_btnStringsBold);
	DDX_Control(pDX, IDC_SCOLOR_PREPROCESSOR_BOLD, m_btnPreProcessorBold);
	DDX_Control(pDX, IDC_SCOLOR_USER1_BOLD, m_btnUser1Bold);
	DDX_Control(pDX, IDC_SCOLOR_USER2_BOLD, m_btnUser2Bold);
	DDX_Check(pDX, IDC_SCOLOR_KEYWORDS_BOLD, m_nKeywordsBold);
	DDX_Check(pDX, IDC_SCOLOR_FUNCTIONS_BOLD, m_nFunctionsBold);
	DDX_Check(pDX, IDC_SCOLOR_COMMENTS_BOLD, m_nCommentsBold);
	DDX_Check(pDX, IDC_SCOLOR_NUMBERS_BOLD, m_nNumbersBold);
	DDX_Check(pDX, IDC_SCOLOR_OPERATORS_BOLD, m_nOperatorsBold);
	DDX_Check(pDX, IDC_SCOLOR_STRINGS_BOLD, m_nStringsBold);
	DDX_Check(pDX, IDC_SCOLOR_PREPROCESSOR_BOLD, m_nPreProcessorBold);
	DDX_Check(pDX, IDC_SCOLOR_USER1_BOLD, m_nUser1Bold);
	DDX_Check(pDX, IDC_SCOLOR_USER2_BOLD, m_nUser2Bold);
}


BEGIN_MESSAGE_MAP(CPropSyntaxColors, CPropertyPage)
	ON_BN_CLICKED(IDC_SCOLOR_KEYWORDS, OnBnClickedEcolorKeywords)
	ON_BN_CLICKED(IDC_SCOLOR_FUNCTIONS, OnBnClickedEcolorFunctions)
	ON_BN_CLICKED(IDC_SCOLOR_COMMENTS, OnBnClickedEcolorComments)
	ON_BN_CLICKED(IDC_SCOLOR_NUMBERS, OnBnClickedEcolorNumbers)
	ON_BN_CLICKED(IDC_SCOLOR_OPERATORS, OnBnClickedEcolorOperators)
	ON_BN_CLICKED(IDC_SCOLOR_STRINGS, OnBnClickedEcolorStrings)
	ON_BN_CLICKED(IDC_SCOLOR_PREPROCESSOR, OnBnClickedEcolorPreprocessor)
	ON_BN_CLICKED(IDC_SCOLOR_USER1, OnBnClickedEcolorUser1)
	ON_BN_CLICKED(IDC_SCOLOR_USER2, OnBnClickedEcolorUser2)
	ON_BN_CLICKED(IDC_SCOLORS_BDEFAULTS, OnBnClickedEcolorsBdefaults)
	ON_BN_CLICKED(IDC_SCOLOR_KEYWORDS_BOLD, OnBnClickedEcolorKeywordsBold)
	ON_BN_CLICKED(IDC_SCOLOR_FUNCTIONS_BOLD, OnBnClickedEcolorFunctionsBold)
	ON_BN_CLICKED(IDC_SCOLOR_COMMENTS_BOLD, OnBnClickedEcolorCommentsBold)
	ON_BN_CLICKED(IDC_SCOLOR_NUMBERS_BOLD, OnBnClickedEcolorNumbersBold)
	ON_BN_CLICKED(IDC_SCOLOR_OPERATORS_BOLD, OnBnClickedEcolorOperatorsBold)
	ON_BN_CLICKED(IDC_SCOLOR_STRINGS_BOLD, OnBnClickedEcolorStringsBold)
	ON_BN_CLICKED(IDC_SCOLOR_PREPROCESSOR_BOLD, OnBnClickedEcolorPreprocessorBold)
	ON_BN_CLICKED(IDC_SCOLOR_USER1_BOLD, OnBnClickedEcolorUser1Bold)
	ON_BN_CLICKED(IDC_SCOLOR_USER2_BOLD, OnBnClickedEcolorUser2Bold)
END_MESSAGE_MAP()

// CPropSyntaxColor message handlers

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL CPropSyntaxColors::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	return CPropertyPage::OnInitDialog();
}

/** 
 * @brief Reads options values from storage to UI.
 * (Property sheet calls this before displaying all property pages)
 */
void CPropSyntaxColors::ReadOptions()
{
}

/** 
 * @brief Writes options values from UI to storage.
 * (Property sheet calls this after displaying all property pages)
 */
void CPropSyntaxColors::WriteOptions()
{
	// User can only change colors via BrowseColorAndSave,
	// which writes to m_pTempColors
	// so user's latest choices are in m_pTempColors
	// (we don't have to read them from screen)
	m_pTempColors->SaveToRegistry();
}

/** 
 * @brief Let user browse common color dialog, and select a color & save to registry
 */
void CPropSyntaxColors::BrowseColorAndSave(CColorButton & colorButton, int colorIndex)
{
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

void CPropSyntaxColors::OnBnClickedEcolorKeywords()
{
	BrowseColorAndSave(m_btnKeywordsText, COLORINDEX_KEYWORD);
}

void CPropSyntaxColors::OnBnClickedEcolorFunctions()
{
	BrowseColorAndSave(m_btnFunctionsText, COLORINDEX_FUNCNAME);
}

void CPropSyntaxColors::OnBnClickedEcolorComments()
{
	BrowseColorAndSave(m_btnCommentsText, COLORINDEX_COMMENT);
}

void CPropSyntaxColors::OnBnClickedEcolorNumbers()
{
	BrowseColorAndSave(m_btnNumbersText, COLORINDEX_NUMBER);
}

void CPropSyntaxColors::OnBnClickedEcolorOperators()
{
	BrowseColorAndSave(m_btnOperatorsText, COLORINDEX_OPERATOR);
}

void CPropSyntaxColors::OnBnClickedEcolorStrings()
{
	BrowseColorAndSave(m_btnStringsText, COLORINDEX_STRING);
}

void CPropSyntaxColors::OnBnClickedEcolorPreprocessor()
{
	BrowseColorAndSave(m_btnPreprocessorText, COLORINDEX_PREPROCESSOR);
}

void CPropSyntaxColors::OnBnClickedEcolorUser1()
{
	BrowseColorAndSave(m_btnUser1Text, COLORINDEX_USER1);
}

void CPropSyntaxColors::OnBnClickedEcolorUser2()
{
	BrowseColorAndSave(m_btnUser2Text, COLORINDEX_USER2);
}

void CPropSyntaxColors::OnBnClickedEcolorsBdefaults()
{
	m_pTempColors->SetDefaults();
	m_btnKeywordsText.SetColor(m_pTempColors->GetColor(COLORINDEX_KEYWORD));
	m_btnFunctionsText.SetColor(m_pTempColors->GetColor(COLORINDEX_FUNCNAME));
	m_btnCommentsText.SetColor(m_pTempColors->GetColor(COLORINDEX_COMMENT));
	m_btnNumbersText.SetColor(m_pTempColors->GetColor(COLORINDEX_NUMBER));
	m_btnOperatorsText.SetColor(m_pTempColors->GetColor(COLORINDEX_OPERATOR));
	m_btnStringsText.SetColor(m_pTempColors->GetColor(COLORINDEX_STRING));
	m_btnPreprocessorText.SetColor(m_pTempColors->GetColor(COLORINDEX_PREPROCESSOR));
	m_btnUser1Text.SetColor(m_pTempColors->GetColor(COLORINDEX_USER1));
	m_btnUser2Text.SetColor(m_pTempColors->GetColor(COLORINDEX_USER2));

	m_nKeywordsBold = GetCheckVal(COLORINDEX_KEYWORD);
	m_nFunctionsBold = GetCheckVal(COLORINDEX_FUNCNAME);
	m_nCommentsBold = GetCheckVal(COLORINDEX_COMMENT);
	m_nNumbersBold = GetCheckVal(COLORINDEX_NUMBER);
	m_nOperatorsBold = GetCheckVal(COLORINDEX_OPERATOR);
	m_nStringsBold = GetCheckVal(COLORINDEX_STRING);
	m_nPreProcessorBold = GetCheckVal(COLORINDEX_PREPROCESSOR);
	m_nUser1Bold = GetCheckVal(COLORINDEX_USER1);
	m_nUser2Bold = GetCheckVal(COLORINDEX_USER2);

	UpdateData(FALSE);
}

/** 
 * @brief Loads color selection dialog's custom colors from registry
 */
void CPropSyntaxColors::LoadCustomColors()
{
	SyntaxColors_Load(m_cCustColors, sizeof(m_cCustColors)/sizeof(m_cCustColors[0]));
}

/** 
 * @brief Saves color selection dialog's custom colors to registry
 */
void CPropSyntaxColors::SaveCustomColors()
{
	SyntaxColors_Save(m_cCustColors, sizeof(m_cCustColors)/sizeof(m_cCustColors[0]));
}

void CPropSyntaxColors::OnBnClickedEcolorKeywordsBold()
{
	UpdateBoldStatus(m_btnKeywordsBold, COLORINDEX_KEYWORD);
}

void CPropSyntaxColors::OnBnClickedEcolorFunctionsBold()
{
	UpdateBoldStatus(m_btnFunctionsBold, COLORINDEX_FUNCNAME);
}

void CPropSyntaxColors::OnBnClickedEcolorCommentsBold()
{
	UpdateBoldStatus(m_btnCommentsBold, COLORINDEX_COMMENT);
}

void CPropSyntaxColors::OnBnClickedEcolorNumbersBold()
{
	UpdateBoldStatus(m_btnNumbersBold, COLORINDEX_NUMBER);
}

void CPropSyntaxColors::OnBnClickedEcolorOperatorsBold()
{
	UpdateBoldStatus(m_btnOperatorsBold, COLORINDEX_OPERATOR);
}

void CPropSyntaxColors::OnBnClickedEcolorStringsBold()
{
	UpdateBoldStatus(m_btnStringsBold, COLORINDEX_STRING);
}

void CPropSyntaxColors::OnBnClickedEcolorPreprocessorBold()
{
	UpdateBoldStatus(m_btnPreProcessorBold, COLORINDEX_PREPROCESSOR);
}

void CPropSyntaxColors::OnBnClickedEcolorUser1Bold()
{
	UpdateBoldStatus(m_btnUser1Bold, COLORINDEX_USER1);
}

void CPropSyntaxColors::OnBnClickedEcolorUser2Bold()
{
	UpdateBoldStatus(m_btnUser2Bold, COLORINDEX_USER2);
}

int CPropSyntaxColors::GetCheckVal(UINT nColorIndex)
{
	if (m_pTempColors->GetBold(nColorIndex))
		return BST_CHECKED;
	else
		return BST_UNCHECKED;
}

void CPropSyntaxColors::UpdateBoldStatus(CButton &btn, UINT colorIndex)
{
	int state = btn.GetCheck();
	if (state == BST_CHECKED)
		m_pTempColors->SetBold(colorIndex, TRUE);
	else
		m_pTempColors->SetBold(colorIndex, FALSE);
}

