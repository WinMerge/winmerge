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
 , m_bIgnoreBlankLines(false)
 , m_bIgnoreEol(true)
 , m_bIgnoreCodepage(true)
 , m_nIgnoreWhite(-1)
 , m_bMovedBlocks(false)
 , m_bMatchSimilarLines(false)
 , m_bFilterCommentsLines(false)
 , m_nDiffAlgorithm(0)
{
}

void PropCompare::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompare)
	DDX_CBIndex(pDX, IDC_DIFF_ALGORITHM, m_nDiffAlgorithm);
	DDX_Check(pDX, IDC_IGNCASE_CHECK, m_bIgnoreCase);
	DDX_Check(pDX, IDC_IGNBLANKS_CHECK, m_bIgnoreBlankLines);
	DDX_Check(pDX, IDC_FILTERCOMMENTS_CHECK, m_bFilterCommentsLines);
	DDX_Check(pDX, IDC_CP_SENSITIVE, m_bIgnoreCodepage);
	DDX_Check(pDX, IDC_EOL_SENSITIVE, m_bIgnoreEol);
	DDX_Radio(pDX, IDC_WHITESPACE, m_nIgnoreWhite);
	DDX_Check(pDX, IDC_MOVED_BLOCKS, m_bMovedBlocks);
	DDX_Check(pDX, IDC_MATCH_SIMILAR_LINES, m_bMatchSimilarLines);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropCompare, CPropertyPage)
	//{{AFX_MSG_MAP(PropCompare)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * Property sheet calls this before displaying GUI to load values
 * into members.
 */
void PropCompare::ReadOptions()
{
	m_nIgnoreWhite = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	m_bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	m_bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	m_bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	m_bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL);
	m_bIgnoreCodepage = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CODEPAGE);
	m_bMovedBlocks = GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS);
	m_bMatchSimilarLines = GetOptionsMgr()->GetBool(OPT_CMP_MATCH_SIMILAR_LINES);
	m_nDiffAlgorithm = GetOptionsMgr()->GetInt(OPT_CMP_DIFF_ALGORITHM);
}

/** 
 * @brief Writes options values from UI to storage.
 * Property sheet calls this after dialog is closed with OK button to
 * store values in member variables.
 */
void PropCompare::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_WHITESPACE, m_nIgnoreWhite);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_BLANKLINES, m_bIgnoreBlankLines);
	GetOptionsMgr()->SaveOption(OPT_CMP_FILTER_COMMENTLINES, m_bFilterCommentsLines);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_CODEPAGE, m_bIgnoreCodepage);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_EOL, m_bIgnoreEol);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_CASE, m_bIgnoreCase);
	GetOptionsMgr()->SaveOption(OPT_CMP_MOVED_BLOCKS, m_bMovedBlocks);
	GetOptionsMgr()->SaveOption(OPT_CMP_MATCH_SIMILAR_LINES, m_bMatchSimilarLines);
	GetOptionsMgr()->SaveOption(OPT_CMP_DIFF_ALGORITHM, m_nDiffAlgorithm);
}

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropCompare::OnInitDialog()
{
	OptionsPanel::OnInitDialog();
	CComboBox * combo = (CComboBox*) GetDlgItem(IDC_DIFF_ALGORITHM);

	String item = _("default");
	combo->AddString(item.c_str());
	item = _("minimal");
	combo->AddString(item.c_str());
	item = _("patience");
	combo->AddString(item.c_str());
	item = _("histogram");
	combo->AddString(item.c_str());
	combo->SetCurSel(m_nDiffAlgorithm);

	return TRUE;  // return TRUE unless you set the focus to a control
}

/** 
 * @brief Sets options to defaults
 */
void PropCompare::OnDefaults()
{
	m_nIgnoreWhite = GetOptionsMgr()->GetDefault<unsigned>(OPT_CMP_IGNORE_WHITESPACE);
	m_bIgnoreEol = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_IGNORE_EOL);
	m_bIgnoreCodepage = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_IGNORE_CODEPAGE);
	m_bIgnoreBlankLines = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_IGNORE_BLANKLINES);
	m_bFilterCommentsLines = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_FILTER_COMMENTLINES);
	m_bIgnoreCase = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_IGNORE_CASE);
	m_bMovedBlocks = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_MOVED_BLOCKS);
	m_bMatchSimilarLines = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_MATCH_SIMILAR_LINES);
	m_nDiffAlgorithm = GetOptionsMgr()->GetDefault<unsigned>(OPT_CMP_DIFF_ALGORITHM);
	UpdateData(FALSE);
}

