/** 
 * @file  PropColors.cpp
 *
 * @brief Implementation of PropMergeColors propertysheet
 */

#include "stdafx.h"
#include "PropColors.h"
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
PropMergeColors::PropMergeColors(COptionsMgr *optionsMgr)
 : OptionsPanel(optionsMgr, PropMergeColors::IDD)
 , m_cCustColors()
{
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

	UpdateTextColorButtonsState();
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropMergeColors, CDialog)
	//{{AFX_MSG_MAP(PropMergeColors)
	ON_BN_CLICKED(IDC_DIFFERENCE_COLOR, OnDifferenceColor)
	ON_BN_CLICKED(IDC_DIFFERENCE_DELETED_COLOR, OnDifferenceDeletedColor)
	ON_BN_CLICKED(IDC_SEL_DIFFERENCE_DELETED_COLOR, OnSelDifferenceDeletedColor)
	ON_BN_CLICKED(IDC_SEL_DIFFERENCE_COLOR, OnSelDifferenceColor)
	ON_BN_CLICKED(IDC_DIFFERENCE_USE_TEXT_COLOR, (OnUseTextColor<IDC_DIFFERENCE_USE_TEXT_COLOR, IDC_DIFFERENCE_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_DIFFERENCE_TEXT_COLOR, OnDifferenceTextColor)
	ON_BN_CLICKED(IDC_SEL_DIFFERENCE_USE_TEXT_COLOR, (OnUseTextColor<IDC_SEL_DIFFERENCE_USE_TEXT_COLOR, IDC_SEL_DIFFERENCE_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_SEL_DIFFERENCE_TEXT_COLOR, OnSelDifferenceTextColor)
	ON_BN_CLICKED(IDC_TRIVIAL_DIFF_COLOR, OnTrivialDiffColor)
	ON_BN_CLICKED(IDC_TRIVIAL_DIFF_DELETED_COLOR, OnTrivialDiffDeletedColor)
	ON_BN_CLICKED(IDC_COLORDEFAULTS_BTN, OnDefaults)
	ON_BN_CLICKED(IDC_COLORSCHEME_GITHUBBITBUCKET, OnColorSchemeButton<COLORSCHEME_GITHUBBITBUCKET>)
	ON_BN_CLICKED(IDC_TRIVIAL_DIFF_USE_TEXT_COLOR, (OnUseTextColor<IDC_TRIVIAL_DIFF_USE_TEXT_COLOR, IDC_TRIVIAL_DIFF_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_TRIVIAL_DIFF_TEXT_COLOR, OnTrivialDiffTextColor)
	ON_BN_CLICKED(IDC_MOVEDBLOCK_COLOR, OnMovedColor)
	ON_BN_CLICKED(IDC_MOVEDBLOCK_DELETED_COLOR, OnMovedDeletedColor)
	ON_BN_CLICKED(IDC_MOVEDBLOCK_USE_TEXT_COLOR, (OnUseTextColor<IDC_MOVEDBLOCK_USE_TEXT_COLOR, IDC_MOVEDBLOCK_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_MOVEDBLOCK_TEXT_COLOR, OnMovedTextColor)
	ON_BN_CLICKED(IDC_SEL_MOVEDBLOCK_COLOR, OnSelMovedColor)
	ON_BN_CLICKED(IDC_SEL_MOVEDBLOCK_DELETED_COLOR, OnSelMovedDeletedColor)
	ON_BN_CLICKED(IDC_SEL_MOVEDBLOCK_USE_TEXT_COLOR, (OnUseTextColor<IDC_SEL_MOVEDBLOCK_USE_TEXT_COLOR, IDC_SEL_MOVEDBLOCK_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_SEL_MOVEDBLOCK_TEXT_COLOR, OnSelMovedTextColor)
	ON_BN_CLICKED(IDC_SNP_COLOR, OnSNPColor)
	ON_BN_CLICKED(IDC_SNP_DELETED_COLOR, OnSNPDeletedColor)
	ON_BN_CLICKED(IDC_SNP_USE_TEXT_COLOR, (OnUseTextColor<IDC_SNP_USE_TEXT_COLOR, IDC_SNP_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_SNP_TEXT_COLOR, OnSNPTextColor)
	ON_BN_CLICKED(IDC_SEL_SNP_COLOR, OnSelSNPColor)
	ON_BN_CLICKED(IDC_SEL_SNP_DELETED_COLOR, OnSelSNPDeletedColor)
	ON_BN_CLICKED(IDC_SEL_SNP_USE_TEXT_COLOR, (OnUseTextColor<IDC_SEL_SNP_USE_TEXT_COLOR, IDC_SEL_SNP_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_SEL_SNP_TEXT_COLOR, OnSelSNPTextColor)
	ON_BN_CLICKED(IDC_WORDDIFF_COLOR, OnWordDiffColor)
	ON_BN_CLICKED(IDC_WORDDIFF_DELETED_COLOR, OnWordDiffDeletedColor)
	ON_BN_CLICKED(IDC_WORDDIFF_USE_TEXT_COLOR, (OnUseTextColor<IDC_WORDDIFF_USE_TEXT_COLOR, IDC_WORDDIFF_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_WORDDIFF_TEXT_COLOR, OnWordDiffTextColor)
	ON_BN_CLICKED(IDC_SEL_WORDDIFF_COLOR, OnSelWordDiffColor)
	ON_BN_CLICKED(IDC_SEL_WORDDIFF_DELETED_COLOR, OnSelWordDiffDeletedColor)
	ON_BN_CLICKED(IDC_SEL_WORDDIFF_USE_TEXT_COLOR, (OnUseTextColor<IDC_SEL_WORDDIFF_USE_TEXT_COLOR, IDC_SEL_WORDDIFF_TEXT_COLOR>))
	ON_BN_CLICKED(IDC_SEL_WORDDIFF_TEXT_COLOR, OnSelWordDiffTextColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * (Property sheet calls this before displaying all property pages)
 */
void PropMergeColors::ReadOptions()
{
	SerializeColors(READ_OPTIONS);
}

/** 
 * @brief Writes options values from UI to storage.
 * (Property sheet calls this after displaying all property pages)
 */
void PropMergeColors::WriteOptions()
{
	SerializeColors(WRITE_OPTIONS);
}

/** 
 * @brief Let user browse common color dialog, and select a color
 */
void PropMergeColors::BrowseColor(CColorButton & colorButton)
{
	CColorDialog dialog(colorButton.GetColor());
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

/** 
 * @brief User wants to change difference color
 */
void PropMergeColors::OnDifferenceColor() 
{
	BrowseColor(m_cDiff);
}

/** 
 * @brief User wants to change selected difference color
 */
void PropMergeColors::OnSelDifferenceColor() 
{
	BrowseColor(m_cSelDiff);
}

/** 
 * @brief User wants to change deleted difference color
 */
void PropMergeColors::OnDifferenceDeletedColor() 
{
	BrowseColor(m_cDiffDeleted);
}

/** 
 * @brief User wants to change selected & deleted difference color
 */
void PropMergeColors::OnSelDifferenceDeletedColor() 
{
	BrowseColor(m_cSelDiffDeleted);
}

/** 
 * @brief User wants to change difference text color
 */
void PropMergeColors::OnDifferenceTextColor() 
{
	BrowseColor(m_cDiffText);
}

/** 
 * @brief User wants to change selected difference text color
 */
void PropMergeColors::OnSelDifferenceTextColor() 
{
	BrowseColor(m_cSelDiffText);
}

/** 
 * @brief User wants to change trivial difference color
 */
void PropMergeColors::OnTrivialDiffColor() 
{
	BrowseColor(m_cTrivial);
}

/** 
 * @brief User wants to change deleted trivial difference color
 */
void PropMergeColors::OnTrivialDiffDeletedColor() 
{
	BrowseColor(m_cTrivialDeleted);
}

void PropMergeColors::OnTrivialDiffTextColor()
{
	BrowseColor(m_cTrivialText);
}

void PropMergeColors::OnMovedColor()
{
	BrowseColor(m_cMoved);
}

void PropMergeColors::OnMovedDeletedColor()
{
	BrowseColor(m_cMovedDeleted);
}

void PropMergeColors::OnMovedTextColor()
{
	BrowseColor(m_cMovedText);
}

void PropMergeColors::OnSelMovedColor()
{
	BrowseColor(m_cSelMoved);
}

void PropMergeColors::OnSelMovedDeletedColor()
{
	BrowseColor(m_cSelMovedDeleted);
}

void PropMergeColors::OnSelMovedTextColor()
{
	BrowseColor(m_cSelMovedText);
}

void PropMergeColors::OnSNPColor()
{
	BrowseColor(m_cSNP);
}

void PropMergeColors::OnSNPDeletedColor()
{
	BrowseColor(m_cSNPDeleted);
}

void PropMergeColors::OnSNPTextColor()
{
	BrowseColor(m_cSNPText);
}

void PropMergeColors::OnSelSNPColor()
{
	BrowseColor(m_cSelSNP);
}

void PropMergeColors::OnSelSNPDeletedColor()
{
	BrowseColor(m_cSelSNPDeleted);
}

void PropMergeColors::OnSelSNPTextColor()
{
	BrowseColor(m_cSelSNPText);
}

/** 
 * @brief User wants to change word difference color
 */
void PropMergeColors::OnWordDiffColor() 
{
	BrowseColor(m_cWordDiff);
}
/** 
 * @brief User wants to change word difference deleted color
 */
void PropMergeColors::OnWordDiffDeletedColor() 
{
	BrowseColor(m_cWordDiffDeleted);
}

/** 
 * @brief User wants to change selected word difference color
 */
void PropMergeColors::OnSelWordDiffColor() 
{
	BrowseColor(m_cSelWordDiff);
}

/** 
 * @brief User wants to change word difference deleted color
 */
void PropMergeColors::OnSelWordDiffDeletedColor()
{
	BrowseColor(m_cSelWordDiffDeleted);
}

/** 
 * @brief User wants to change word difference text color
 */
void PropMergeColors::OnWordDiffTextColor() 
{
	BrowseColor(m_cWordDiffText);
}

/** 
 * @brief User wants to change selected word difference text color
 */
void PropMergeColors::OnSelWordDiffTextColor() 
{
	BrowseColor(m_cSelWordDiffText);
}

template<int checkbox_id, int colorbutton_id>
void PropMergeColors::OnUseTextColor()
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

void PropMergeColors::SerializeColors(OPERATION op)
{
	SerializeColor(op, m_cDiff, OPT_CLR_DIFF);
	SerializeColor(op, m_cDiffDeleted, OPT_CLR_DIFF_DELETED);
	SerializeColor(op, m_cSelDiff, OPT_CLR_SELECTED_DIFF);

	SerializeColor(op, m_cDiffText, OPT_CLR_DIFF_TEXT);
	SerializeColor(op, m_cSelDiffDeleted, OPT_CLR_SELECTED_DIFF_DELETED);
	SerializeColor(op, m_cSelDiffText, OPT_CLR_SELECTED_DIFF_TEXT);

	SerializeColor(op, m_cTrivial, OPT_CLR_TRIVIAL_DIFF);
	SerializeColor(op, m_cTrivialDeleted, OPT_CLR_TRIVIAL_DIFF_DELETED);
	SerializeColor(op, m_cTrivialText, OPT_CLR_TRIVIAL_DIFF_TEXT);
	
	SerializeColor(op, m_cMoved, OPT_CLR_MOVEDBLOCK);
	SerializeColor(op, m_cMovedDeleted, OPT_CLR_MOVEDBLOCK_DELETED);
	SerializeColor(op, m_cMovedText, OPT_CLR_MOVEDBLOCK_TEXT);
	
	SerializeColor(op, m_cSNP, OPT_CLR_SNP);
	SerializeColor(op, m_cSNPDeleted, OPT_CLR_SNP_DELETED);
	SerializeColor(op, m_cSNPText, OPT_CLR_SNP_TEXT);

	SerializeColor(op, m_cSelMoved, OPT_CLR_SELECTED_MOVEDBLOCK);
	SerializeColor(op, m_cSelMovedDeleted, OPT_CLR_SELECTED_MOVEDBLOCK_DELETED);
	SerializeColor(op, m_cSelMovedText, OPT_CLR_SELECTED_MOVEDBLOCK_TEXT);
	
	SerializeColor(op, m_cSelSNP, OPT_CLR_SELECTED_SNP);
	SerializeColor(op, m_cSelSNPDeleted, OPT_CLR_SELECTED_SNP_DELETED);
	SerializeColor(op, m_cSelSNPText, OPT_CLR_SELECTED_SNP_TEXT);

	SerializeColor(op, m_cWordDiff, OPT_CLR_WORDDIFF);
	SerializeColor(op, m_cWordDiffDeleted, OPT_CLR_WORDDIFF_DELETED);
	SerializeColor(op, m_cWordDiffText, OPT_CLR_WORDDIFF_TEXT);

	SerializeColor(op, m_cSelWordDiff, OPT_CLR_SELECTED_WORDDIFF);
	SerializeColor(op, m_cSelWordDiffDeleted, OPT_CLR_SELECTED_WORDDIFF_DELETED);
	SerializeColor(op, m_cSelWordDiffText, OPT_CLR_SELECTED_WORDDIFF_TEXT);
}

void PropMergeColors::SerializeColor(OPERATION op, CColorButton & btn, const String& optionName)
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

void PropMergeColors::SetColorScheme(int scheme)
{
	struct ColorScheme {
		CColorButton *button;
		COLORREF color;
	};
	
	ColorScheme github_bitbucket[] = {
		{&m_cDiff,               RGB(221,255,221)},
		{&m_cDiffDeleted,        RGB(224,224,255)},
		{&m_cDiffText,           COLOR_NONE},
		{&m_cSelDiff,            RGB(255,221,221)},
		{&m_cSelDiffDeleted,     RGB(240, 192, 192)},
		{&m_cSelDiffText,        COLOR_NONE},
		{&m_cTrivial,            RGB(251,242,191)},
		{&m_cTrivialDeleted,     RGB(233,233,233)},
		{&m_cTrivialText,        COLOR_NONE},
		{&m_cMoved,              RGB(240,216,192)},
		{&m_cMovedDeleted,       RGB(192, 192, 192)},
		{&m_cMovedText,          COLOR_NONE},
		{&m_cSelMoved,           RGB(248,112,78)},
		{&m_cSelMovedDeleted,    RGB(252, 181, 163)},
		{&m_cSelMovedText,       COLOR_NONE},
		{&m_cSNP,                RGB(251,250,223)},
		{&m_cSNPDeleted,         RGB(233, 233, 233)},
		{&m_cSNPText,            COLOR_NONE},
		{&m_cSelSNP,             RGB(239,183,180)},
		{&m_cSelSNPDeleted,      RGB(240, 224, 224)},
		{&m_cSelSNPText,         COLOR_NONE},
		{&m_cWordDiff,           RGB(170,255,170)},
		{&m_cWordDiffDeleted,    RGB(160,230,160)},
		{&m_cWordDiffText,       COLOR_NONE},
		{&m_cSelWordDiff,        RGB(255,170,170)},
		{&m_cSelWordDiffDeleted, RGB(230,150,140)},
		{&m_cSelWordDiffText,    COLOR_NONE},
	};

	if (scheme == COLORSCHEME_GITHUBBITBUCKET)
	{
		for (int i = 0; i < sizeof(github_bitbucket)/sizeof(github_bitbucket[0]); ++i)
			github_bitbucket[i].button->SetColor(github_bitbucket[i].color);
	}

	UpdateTextColorButtonsState();
}

template<int scheme>
void PropMergeColors::OnColorSchemeButton()
{
	SetColorScheme(scheme);
}

/** 
 * @brief Resets colors to defaults
 */
void PropMergeColors::OnDefaults()
{
	SerializeColors(SET_DEFAULTS);
	UpdateTextColorButtonsState();
}
