/** 
 * @file  PropMergeColors.cpp
 *
 * @brief Implementation of PropMergeColors propertysheet
 */

#include "stdafx.h"
#include "PropMergeColors.h"
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
PropMergeColors::PropMergeColors(COptionsMgr *optionsMgr)
 : OptionsPanel(optionsMgr, PropMergeColors::IDD)
 , m_cCustColors()
 , m_clrTrivialDeleted()
 , m_clrTrivial()
 , m_clrSelDiffText()
 , m_clrDiffText()
 , m_clrSelDiffDeleted()
 , m_clrDiffDeleted()
 , m_clrSelDiff()
 , m_clrDiff()
 , m_clrTrivialText()
 , m_clrMoved()
 , m_clrMovedDeleted()
 , m_clrMovedText()
 , m_clrSelMoved()
 , m_clrSelMovedDeleted()
 , m_clrSelMovedText()
 , m_clrSNP()
 , m_clrSNPDeleted()
 , m_clrSNPText()
 , m_clrSelSNP()
 , m_clrSelSNPDeleted()
 , m_clrSelSNPText()
 , m_clrWordDiff()
 , m_clrWordDiffDeleted()
 , m_clrWordDiffText()
 , m_clrSelWordDiff()
 , m_clrSelWordDiffDeleted()
 , m_clrSelWordDiffText()
{
	BindOption(OPT_CLR_DIFF, m_clrDiff, IDC_DIFFERENCE_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_DIFF_DELETED, m_clrDiffDeleted, IDC_DIFFERENCE_DELETED_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_SELECTED_DIFF, m_clrSelDiff, IDC_SEL_DIFFERENCE_COLOR, DDX_ColorButton);

	BindOption(OPT_CLR_DIFF_TEXT, m_clrDiffText, IDC_DIFFERENCE_TEXT_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_SELECTED_DIFF_DELETED, m_clrSelDiffDeleted, IDC_SEL_DIFFERENCE_DELETED_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_SELECTED_DIFF_TEXT, m_clrSelDiffText, IDC_SEL_DIFFERENCE_TEXT_COLOR, DDX_ColorButton);

	BindOption(OPT_CLR_TRIVIAL_DIFF, m_clrTrivial, IDC_TRIVIAL_DIFF_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_TRIVIAL_DIFF_DELETED, m_clrTrivialDeleted, IDC_TRIVIAL_DIFF_DELETED_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_TRIVIAL_DIFF_TEXT, m_clrTrivialText, IDC_TRIVIAL_DIFF_TEXT_COLOR, DDX_ColorButton);
	
	BindOption(OPT_CLR_MOVEDBLOCK, m_clrMoved, IDC_MOVEDBLOCK_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_MOVEDBLOCK_DELETED, m_clrMovedDeleted, IDC_MOVEDBLOCK_DELETED_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_MOVEDBLOCK_TEXT, m_clrMovedText, IDC_MOVEDBLOCK_TEXT_COLOR, DDX_ColorButton);
	
	BindOption(OPT_CLR_SNP, m_clrSNP, IDC_SNP_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_SNP_DELETED, m_clrSNPDeleted, IDC_SNP_DELETED_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_SNP_TEXT, m_clrSNPText, IDC_SNP_TEXT_COLOR, DDX_ColorButton);

	BindOption(OPT_CLR_SELECTED_MOVEDBLOCK, m_clrSelMoved, IDC_SEL_MOVEDBLOCK_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_SELECTED_MOVEDBLOCK_DELETED, m_clrSelMovedDeleted, IDC_SEL_MOVEDBLOCK_DELETED_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_SELECTED_MOVEDBLOCK_TEXT, m_clrSelMovedText, IDC_SEL_MOVEDBLOCK_TEXT_COLOR, DDX_ColorButton);
	
	BindOption(OPT_CLR_SELECTED_SNP, m_clrSelSNP, IDC_SEL_SNP_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_SELECTED_SNP_DELETED, m_clrSelSNPDeleted, IDC_SEL_SNP_DELETED_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_SELECTED_SNP_TEXT, m_clrSelSNPText, IDC_SEL_SNP_TEXT_COLOR, DDX_ColorButton);

	BindOption(OPT_CLR_WORDDIFF, m_clrWordDiff, IDC_WORDDIFF_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_WORDDIFF_DELETED, m_clrWordDiffDeleted, IDC_WORDDIFF_DELETED_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_WORDDIFF_TEXT, m_clrWordDiffText, IDC_WORDDIFF_TEXT_COLOR, DDX_ColorButton);

	BindOption(OPT_CLR_SELECTED_WORDDIFF, m_clrSelWordDiff, IDC_SEL_WORDDIFF_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_SELECTED_WORDDIFF_DELETED, m_clrSelWordDiffDeleted, IDC_SEL_WORDDIFF_DELETED_COLOR, DDX_ColorButton);
	BindOption(OPT_CLR_SELECTED_WORDDIFF_TEXT, m_clrSelWordDiffText, IDC_SEL_WORDDIFF_TEXT_COLOR, DDX_ColorButton);
}

void PropMergeColors::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropMergeColors)
	DDX_Control(pDX, IDC_TRIVIAL_DIFF_DELETED_COLOR, m_cTrivialDeleted);
	DDX_Control(pDX, IDC_TRIVIAL_DIFF_COLOR, m_cTrivial);
	DDX_Control(pDX, IDC_SEL_DIFFERENCE_TEXT_COLOR, m_cSelDiffText);
	DDX_Control(pDX, IDC_DIFFERENCE_TEXT_COLOR, m_cDiffText);
	DDX_Control(pDX, IDC_SEL_DIFFERENCE_DELETED_COLOR, m_cSelDiffDeleted);
	DDX_Control(pDX, IDC_DIFFERENCE_DELETED_COLOR, m_cDiffDeleted);
	DDX_Control(pDX, IDC_SEL_DIFFERENCE_COLOR, m_cSelDiff);
	DDX_Control(pDX, IDC_DIFFERENCE_COLOR, m_cDiff);
	DDX_Control(pDX, IDC_TRIVIAL_DIFF_TEXT_COLOR, m_cTrivialText);
	DDX_Control(pDX, IDC_MOVEDBLOCK_COLOR, m_cMoved);
	DDX_Control(pDX, IDC_MOVEDBLOCK_DELETED_COLOR, m_cMovedDeleted);
	DDX_Control(pDX, IDC_MOVEDBLOCK_TEXT_COLOR, m_cMovedText);
	DDX_Control(pDX, IDC_SEL_MOVEDBLOCK_COLOR, m_cSelMoved);
	DDX_Control(pDX, IDC_SEL_MOVEDBLOCK_DELETED_COLOR, m_cSelMovedDeleted);
	DDX_Control(pDX, IDC_SEL_MOVEDBLOCK_TEXT_COLOR, m_cSelMovedText);
	DDX_Control(pDX, IDC_SNP_COLOR, m_cSNP);
	DDX_Control(pDX, IDC_SNP_DELETED_COLOR, m_cSNPDeleted);
	DDX_Control(pDX, IDC_SNP_TEXT_COLOR, m_cSNPText);
	DDX_Control(pDX, IDC_SEL_SNP_COLOR, m_cSelSNP);
	DDX_Control(pDX, IDC_SEL_SNP_DELETED_COLOR, m_cSelSNPDeleted);
	DDX_Control(pDX, IDC_SEL_SNP_TEXT_COLOR, m_cSelSNPText);
	DDX_Control(pDX, IDC_WORDDIFF_COLOR, m_cWordDiff);
	DDX_Control(pDX, IDC_WORDDIFF_DELETED_COLOR, m_cWordDiffDeleted);
	DDX_Control(pDX, IDC_WORDDIFF_TEXT_COLOR, m_cWordDiffText);
	DDX_Control(pDX, IDC_SEL_WORDDIFF_COLOR, m_cSelWordDiff);
	DDX_Control(pDX, IDC_SEL_WORDDIFF_DELETED_COLOR, m_cSelWordDiffDeleted);
	DDX_Control(pDX, IDC_SEL_WORDDIFF_TEXT_COLOR, m_cSelWordDiffText);
	//}}AFX_DATA_MAP
	DoDataExchangeBindOptions(pDX);
	UpdateTextColorButtonsState();
}


BEGIN_MESSAGE_MAP(PropMergeColors, OptionsPanel)
	//{{AFX_MSG_MAP(PropMergeColors)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_DIFFERENCE_COLOR, IDC_SEL_WORDDIFF_TEXT_COLOR, BrowseColor)
	ON_BN_CLICKED(IDC_DIFFERENCE_USE_TEXT_COLOR, (OnUseTextColor<IDC_DIFFERENCE_USE_TEXT_COLOR, IDC_DIFFERENCE_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_SEL_DIFFERENCE_USE_TEXT_COLOR, (OnUseTextColor<IDC_SEL_DIFFERENCE_USE_TEXT_COLOR, IDC_SEL_DIFFERENCE_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_TRIVIAL_DIFF_USE_TEXT_COLOR, (OnUseTextColor<IDC_TRIVIAL_DIFF_USE_TEXT_COLOR, IDC_TRIVIAL_DIFF_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_MOVEDBLOCK_USE_TEXT_COLOR, (OnUseTextColor<IDC_MOVEDBLOCK_USE_TEXT_COLOR, IDC_MOVEDBLOCK_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_SEL_MOVEDBLOCK_USE_TEXT_COLOR, (OnUseTextColor<IDC_SEL_MOVEDBLOCK_USE_TEXT_COLOR, IDC_SEL_MOVEDBLOCK_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_SNP_USE_TEXT_COLOR, (OnUseTextColor<IDC_SNP_USE_TEXT_COLOR, IDC_SNP_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_SEL_SNP_USE_TEXT_COLOR, (OnUseTextColor<IDC_SEL_SNP_USE_TEXT_COLOR, IDC_SEL_SNP_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_WORDDIFF_USE_TEXT_COLOR, (OnUseTextColor<IDC_WORDDIFF_USE_TEXT_COLOR, IDC_WORDDIFF_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_SEL_WORDDIFF_USE_TEXT_COLOR, (OnUseTextColor<IDC_SEL_WORDDIFF_USE_TEXT_COLOR, IDC_SEL_WORDDIFF_TEXT_COLOR>))
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Let user browse common color dialog, and select a color
 */
void PropMergeColors::BrowseColor(unsigned id)
{
	CColorButton& colorButton = *dynamic_cast<CColorButton*>(GetDlgItem(id));
	CMyColorDialog dialog(colorButton.GetColor());
	Options::CustomColors::Load(GetOptionsMgr(), m_cCustColors.data());
	dialog.m_cc.lpCustColors = m_cCustColors.data();
	
	if (dialog.DoModal() == IDOK)
		colorButton.SetColor(dialog.GetColor());
	Options::CustomColors::Save(GetOptionsMgr(), m_cCustColors.data());
}

void PropMergeColors::UpdateTextColorButtonState(int checkboxId, CColorButton &btn)
{
	CheckDlgButton(checkboxId, btn.GetColor() != COLOR_NONE ? BST_CHECKED : BST_UNCHECKED);
	btn.ShowWindow(btn.GetColor() != COLOR_NONE ? SW_SHOW : SW_HIDE);
}

void PropMergeColors::UpdateTextColorButtonsState()
{
	UpdateTextColorButtonState(IDC_DIFFERENCE_USE_TEXT_COLOR, m_cDiffText);
	UpdateTextColorButtonState(IDC_SEL_DIFFERENCE_USE_TEXT_COLOR, m_cSelDiffText);
	UpdateTextColorButtonState(IDC_TRIVIAL_DIFF_USE_TEXT_COLOR, m_cTrivialText);
	UpdateTextColorButtonState(IDC_MOVEDBLOCK_USE_TEXT_COLOR, m_cMovedText);
	UpdateTextColorButtonState(IDC_SEL_MOVEDBLOCK_USE_TEXT_COLOR, m_cSelMovedText);
	UpdateTextColorButtonState(IDC_SNP_USE_TEXT_COLOR, m_cSNPText);
	UpdateTextColorButtonState(IDC_SEL_SNP_USE_TEXT_COLOR, m_cSelSNPText);
	UpdateTextColorButtonState(IDC_WORDDIFF_USE_TEXT_COLOR, m_cWordDiffText);
	UpdateTextColorButtonState(IDC_SEL_WORDDIFF_USE_TEXT_COLOR, m_cSelWordDiffText);
}

void PropMergeColors::OnUseTextColor(int checkbox_id, int colorbutton_id)
{
	CColorButton *cButton = dynamic_cast<CColorButton *>(GetDlgItem(colorbutton_id));
	if (IsDlgButtonChecked(checkbox_id) != 0)
	{
		cButton->ShowWindow(SW_SHOW);
		cButton->SetColor(0);
	}
	else
	{
		cButton->ShowWindow(SW_HIDE);
		cButton->SetColor(COLOR_NONE);
	}
}

