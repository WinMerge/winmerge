/** 
 * @file  PropCompare.cpp
 *
 * @brief Implementation of CPropCompare propertysheet
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "PropCompare.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropCompare property page

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to COptionsMgr.
 */
CPropCompare::CPropCompare(COptionsMgr *optionsMgr) 
 : OptionsPanel(optionsMgr, CPropCompare::IDD)
 , m_bIgnoreCase(FALSE)
 , m_bIgnoreBlankLines(FALSE)
 , m_bIgnoreEol(TRUE)
 , m_nIgnoreWhite(-1)
 , m_bMovedBlocks(FALSE)
 , m_bMatchSimilarLines(FALSE)
 , m_bFilterCommentsLines(FALSE)
{
}

void CPropCompare::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropCompare)
	DDX_Check(pDX, IDC_IGNCASE_CHECK, m_bIgnoreCase);
	DDX_Check(pDX, IDC_IGNBLANKS_CHECK, m_bIgnoreBlankLines);
	DDX_Check(pDX, IDC_FILTERCOMMENTS_CHECK, m_bFilterCommentsLines);
	DDX_Check(pDX, IDC_EOL_SENSITIVE, m_bIgnoreEol);
	DDX_Radio(pDX, IDC_WHITESPACE, m_nIgnoreWhite);
	DDX_Check(pDX, IDC_MOVED_BLOCKS, m_bMovedBlocks);
	DDX_Check(pDX, IDC_MATCH_SIMILAR_LINES, m_bMatchSimilarLines);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropCompare, CPropertyPage)
	//{{AFX_MSG_MAP(CPropCompare)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * Property sheet calls this before displaying GUI to load values
 * into members.
 */
void CPropCompare::ReadOptions()
{
	m_nIgnoreWhite = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	m_bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	m_bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	m_bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	m_bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL) ? true : false;
	m_bMovedBlocks = GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS);
	m_bMatchSimilarLines = GetOptionsMgr()->GetBool(OPT_CMP_MATCH_SIMILAR_LINES);
}

/** 
 * @brief Writes options values from UI to storage.
 * Property sheet calls this after dialog is closed with OK button to
 * store values in member variables.
 */
void CPropCompare::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_WHITESPACE, m_nIgnoreWhite);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_BLANKLINES, m_bIgnoreBlankLines == TRUE);
	GetOptionsMgr()->SaveOption(OPT_CMP_FILTER_COMMENTLINES, m_bFilterCommentsLines == TRUE);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_EOL, m_bIgnoreEol == TRUE);
	GetOptionsMgr()->SaveOption(OPT_CMP_IGNORE_CASE, m_bIgnoreCase == TRUE);
	GetOptionsMgr()->SaveOption(OPT_CMP_MOVED_BLOCKS, m_bMovedBlocks == TRUE);
	GetOptionsMgr()->SaveOption(OPT_CMP_MATCH_SIMILAR_LINES, m_bMatchSimilarLines == TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CPropCompare message handlers

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL CPropCompare::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Sets options to defaults
 */
void CPropCompare::OnDefaults()
{
	DWORD tmp;
	GetOptionsMgr()->GetDefault(OPT_CMP_IGNORE_WHITESPACE, tmp);
	m_nIgnoreWhite = tmp;
	GetOptionsMgr()->GetDefault(OPT_CMP_IGNORE_EOL, tmp);
	m_bIgnoreEol = tmp;
	GetOptionsMgr()->GetDefault(OPT_CMP_IGNORE_BLANKLINES, tmp);
	m_bIgnoreBlankLines = tmp;
	GetOptionsMgr()->GetDefault(OPT_CMP_FILTER_COMMENTLINES, tmp);
	m_bFilterCommentsLines = tmp;
	GetOptionsMgr()->GetDefault(OPT_CMP_IGNORE_CASE, tmp);
	m_bIgnoreCase = tmp;
	GetOptionsMgr()->GetDefault(OPT_CMP_MOVED_BLOCKS, tmp);
	m_bMovedBlocks = tmp;
	GetOptionsMgr()->GetDefault(OPT_CMP_MATCH_SIMILAR_LINES, tmp);
	m_bMatchSimilarLines = tmp;
	UpdateData(FALSE);
}
