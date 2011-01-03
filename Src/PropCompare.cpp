/** 
 * @file  PropCompare.cpp
 *
 * @brief Implementation of PropCompare propertysheet
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "PropCompare.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PropCompare property page

/** 
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to COptionsMgr.
 */
PropCompare::PropCompare(COptionsMgr *optionsMgr) : CPropertyPage(PropCompare::IDD)
 , m_pOptionsMgr(optionsMgr)
 , m_compareMethod(-1)
 , m_bIgnoreCase(FALSE)
 , m_bIgnoreBlankLines(FALSE)
 , m_bIgnoreEol(TRUE)
 , m_nIgnoreWhite(-1)
 , m_bMovedBlocks(FALSE)
 , m_bMatchSimilarLines(FALSE)
 , m_bStopAfterFirst(FALSE)
 , m_bFilterCommentsLines(FALSE)
, m_bIgnoreSmallTimeDiff(FALSE)
{
}

void PropCompare::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompare)
	DDX_CBIndex(pDX, IDC_COMPAREMETHODCOMBO, m_compareMethod);
	DDX_Check(pDX, IDC_IGNCASE_CHECK, m_bIgnoreCase);
	DDX_Check(pDX, IDC_IGNBLANKS_CHECK, m_bIgnoreBlankLines);
	DDX_Check(pDX, IDC_FILTERCOMMENTS_CHECK, m_bFilterCommentsLines);
	DDX_Check(pDX, IDC_EOL_SENSITIVE, m_bIgnoreEol);
	DDX_Radio(pDX, IDC_WHITESPACE, m_nIgnoreWhite);
	DDX_Check(pDX, IDC_MOVED_BLOCKS, m_bMovedBlocks);
	DDX_Check(pDX, IDC_MATCH_SIMILAR_LINES, m_bMatchSimilarLines);
	DDX_Check(pDX, IDC_COMPARE_STOPFIRST, m_bStopAfterFirst);
	DDX_Check(pDX, IDC_IGNORE_SMALLTIMEDIFF, m_bIgnoreSmallTimeDiff);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropCompare, CPropertyPage)
	//{{AFX_MSG_MAP(PropCompare)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMPAREMETHODCOMBO, OnCbnSelchangeComparemethodcombo)
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * Property sheet calls this before displaying GUI to load values
 * into members.
 */
void PropCompare::ReadOptions()
{
	m_nIgnoreWhite = m_pOptionsMgr->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	m_bIgnoreBlankLines = m_pOptionsMgr->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	m_bFilterCommentsLines = m_pOptionsMgr->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	m_bIgnoreCase = m_pOptionsMgr->GetBool(OPT_CMP_IGNORE_CASE);
	m_bIgnoreEol = m_pOptionsMgr->GetBool(OPT_CMP_IGNORE_EOL) ? true : false;
	m_bMovedBlocks = m_pOptionsMgr->GetBool(OPT_CMP_MOVED_BLOCKS);
	m_bMatchSimilarLines = m_pOptionsMgr->GetBool(OPT_CMP_MATCH_SIMILAR_LINES);
	m_compareMethod = m_pOptionsMgr->GetInt(OPT_CMP_METHOD);
	m_bStopAfterFirst = m_pOptionsMgr->GetBool(OPT_CMP_STOP_AFTER_FIRST);
	m_bIgnoreSmallTimeDiff = m_pOptionsMgr->GetBool(OPT_IGNORE_SMALL_FILETIME);
}

/** 
 * @brief Writes options values from UI to storage.
 * Property sheet calls this after dialog is closed with OK button to
 * store values in member variables.
 */
void PropCompare::WriteOptions()
{
	m_pOptionsMgr->SaveOption(OPT_CMP_IGNORE_WHITESPACE, m_nIgnoreWhite);
	m_pOptionsMgr->SaveOption(OPT_CMP_IGNORE_BLANKLINES, m_bIgnoreBlankLines == TRUE);
	m_pOptionsMgr->SaveOption(OPT_CMP_FILTER_COMMENTLINES, m_bFilterCommentsLines == TRUE);
	m_pOptionsMgr->SaveOption(OPT_CMP_IGNORE_EOL, m_bIgnoreEol == TRUE);
	m_pOptionsMgr->SaveOption(OPT_CMP_IGNORE_CASE, m_bIgnoreCase == TRUE);
	m_pOptionsMgr->SaveOption(OPT_CMP_METHOD, (int)m_compareMethod);
	m_pOptionsMgr->SaveOption(OPT_CMP_MOVED_BLOCKS, m_bMovedBlocks == TRUE);
	m_pOptionsMgr->SaveOption(OPT_CMP_MATCH_SIMILAR_LINES, m_bMatchSimilarLines == TRUE);
	m_pOptionsMgr->SaveOption(OPT_CMP_STOP_AFTER_FIRST, m_bStopAfterFirst == TRUE);
	m_pOptionsMgr->SaveOption(OPT_IGNORE_SMALL_FILETIME, m_bIgnoreSmallTimeDiff == TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// PropCompare message handlers

/** 
 * @brief Called before propertysheet is drawn.
 */
BOOL PropCompare::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();
	CComboBox * combo = (CComboBox*) GetDlgItem(IDC_COMPAREMETHODCOMBO);

	String item = theApp.LoadString(IDS_COMPMETHOD_FULL_CONTENTS);
	combo->AddString(item.c_str());
	item = theApp.LoadString(IDS_COMPMETHOD_QUICK_CONTENTS);
	combo->AddString(item.c_str());
	item = theApp.LoadString(IDS_COMPMETHOD_MODDATE);
	combo->AddString(item.c_str());
	item = theApp.LoadString(IDS_COMPMETHOD_DATESIZE);
	combo->AddString(item.c_str());
	item = theApp.LoadString(IDS_COMPMETHOD_SIZE);
	combo->AddString(item.c_str());
	combo->SetCurSel(m_compareMethod);

	CButton * pBtn = (CButton*) GetDlgItem(IDC_COMPARE_STOPFIRST);
	if (m_compareMethod == 1)
		pBtn->EnableWindow(TRUE);
	else
		pBtn->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Sets options to defaults
 */
void PropCompare::OnDefaults()
{
	DWORD tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_METHOD, tmp);
	m_compareMethod = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_IGNORE_WHITESPACE, tmp);
	m_nIgnoreWhite = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_IGNORE_EOL, tmp);
	m_bIgnoreEol = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_IGNORE_BLANKLINES, tmp);
	m_bIgnoreBlankLines = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_FILTER_COMMENTLINES, tmp);
	m_bFilterCommentsLines = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_IGNORE_CASE, tmp);
	m_bIgnoreCase = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_MOVED_BLOCKS, tmp);
	m_bMovedBlocks = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_MATCH_SIMILAR_LINES, tmp);
	m_bMatchSimilarLines = tmp;
	m_pOptionsMgr->GetDefault(OPT_CMP_STOP_AFTER_FIRST, tmp);
	m_bStopAfterFirst = tmp;
	UpdateData(FALSE);
}

/** 
 * @brief Called when compare method dropdown selection is changed.
 * Enables / disables "Stop compare after first difference" checkbox.
 * That checkbox is valid only for quick contents compare method.
 */
void PropCompare::OnCbnSelchangeComparemethodcombo()
{
	CComboBox * pCombo = (CComboBox*) GetDlgItem(IDC_COMPAREMETHODCOMBO);
	CButton * pBtn = (CButton*) GetDlgItem(IDC_COMPARE_STOPFIRST);
	if (pCombo->GetCurSel() == 1)
		pBtn->EnableWindow(TRUE);
	else
		pBtn->EnableWindow(FALSE);
}
