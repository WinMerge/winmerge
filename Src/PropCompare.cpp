/** 
 * @file  PropCompare.cpp
 *
 * @brief Implementation of PropCompare propertysheet
 */

#include "stdafx.h"
#include "PropCompare.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to COptionsMgr.
 */
PropCompare::PropCompare(COptionsMgr *optionsMgr) 
 : OptionsPanel(optionsMgr, PropCompare::IDD)
 , m_bIgnoreCase(false)
 , m_bIgnoreNumbers(false)
 , m_bIgnoreBlankLines(false)
 , m_bIgnoreEol(true)
 , m_bIgnoreCodepage(true)
 , m_bIgnoreMissingTrailingEol(true)
 , m_bIgnoreLineBreaks(true)
 , m_nIgnoreWhite(-1)
 , m_bMovedBlocks(false)
 , m_bAlignSimilarLines(false)
 , m_bFilterCommentsLines(false)
 , m_nDiffAlgorithm(0)
 , m_bIndentHeuristic(true)
 , m_bCompleteBlankOutIgnoredChanges(false)
{
	BindOption(OPT_CMP_IGNORE_WHITESPACE, m_nIgnoreWhite, IDC_WHITESPACE, DDX_Radio);
	BindOption(OPT_CMP_IGNORE_BLANKLINES, m_bIgnoreBlankLines, IDC_IGNBLANKS_CHECK, DDX_Check);
	BindOption(OPT_CMP_FILTER_COMMENTLINES, m_bFilterCommentsLines, IDC_FILTERCOMMENTS_CHECK, DDX_Check);
	BindOption(OPT_CMP_IGNORE_CASE, m_bIgnoreCase, IDC_IGNCASE_CHECK, DDX_Check);
	BindOption(OPT_CMP_IGNORE_NUMBERS, m_bIgnoreNumbers, IDC_IGNORE_NUMBERS, DDX_Check);
	BindOption(OPT_CMP_IGNORE_EOL, m_bIgnoreEol, IDC_EOL_SENSITIVE, DDX_Check);
	BindOption(OPT_CMP_IGNORE_CODEPAGE, m_bIgnoreCodepage, IDC_CP_SENSITIVE, DDX_Check);
	BindOption(OPT_CMP_IGNORE_MISSING_TRAILING_EOL, m_bIgnoreMissingTrailingEol, IDC_IGNEOFEOL_CHECK, DDX_Check);
	BindOption(OPT_CMP_IGNORE_LINE_BREAKS, m_bIgnoreLineBreaks, IDC_IGNLBRKS_CHECK, DDX_Check);
	BindOption(OPT_CMP_MOVED_BLOCKS, m_bMovedBlocks, IDC_MOVED_BLOCKS, DDX_Check);
	BindOption(OPT_CMP_ALIGN_SIMILAR_LINES, m_bAlignSimilarLines, IDC_ALIGN_SIMILAR_LINES, DDX_Check);
	BindOption(OPT_CMP_DIFF_ALGORITHM, m_nDiffAlgorithm, IDC_DIFF_ALGORITHM, DDX_CBIndex);
	BindOption(OPT_CMP_INDENT_HEURISTIC, m_bIndentHeuristic, IDC_INDENT_HEURISTIC, DDX_Check);
	BindOption(OPT_CMP_COMPLETELY_BLANK_OUT_IGNORED_CHANGES, m_bCompleteBlankOutIgnoredChanges, IDC_COMPLETELY_BLANK_OUT_IGNORED_DIFFERENCES, DDX_Check);
}

void PropCompare::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompare)
	//}}AFX_DATA_MAP
	DoDataExchangeBindOptions(pDX);
	UpdateControls();
}


BEGIN_MESSAGE_MAP(PropCompare, OptionsPanel)
	//{{AFX_MSG_MAP(PropCompare)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_CBN_SELCHANGE(IDC_DIFF_ALGORITHM, OnCbnSelchangeDiffAlgorithm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropCompare::OnInitDialog()
{
	SetDlgItemComboBoxList(IDC_DIFF_ALGORITHM,
		{ _("default"), _("minimal"), _("patience"), _("histogram"), _("none") });

	OptionsPanel::OnInitDialog();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void PropCompare::OnCbnSelchangeDiffAlgorithm()
{
	UpdateControls();
}

void PropCompare::UpdateControls()
{
	CComboBox * pCombo = (CComboBox*)GetDlgItem(IDC_DIFF_ALGORITHM);
	int cursel = pCombo->GetCurSel();
	EnableDlgItem(IDC_INDENT_HEURISTIC, cursel != 0/*default*/ && cursel != 4/*none*/);
}
