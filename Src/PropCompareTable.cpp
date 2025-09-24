/** 
 * @file  PropCompareTable.cpp
 *
 * @brief Implementation of PropCompareTable propertysheet
 */

#include "stdafx.h"
#include "PropCompareTable.h"
#include "WildcardDropList.h"
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
PropCompareTable::PropCompareTable(COptionsMgr *optionsMgr) 
 : OptionsPanel(optionsMgr, PropCompareTable::IDD)
, m_bAllowNewlinesInQuotes(true)
, m_sCSVDelimiterChar(_T(","))
, m_sDSVDelimiterChar(_T(";"))
, m_sQuoteChar(_T("\""))
{
	auto converter = [](String v, bool write) {
		if (!write) return v;
		WildcardRemoveDuplicatePatterns(v);
		return v;
	};
	BindOption(OPT_CMP_CSV_FILEPATTERNS, m_sCSVFilePatterns, IDC_COMPARETABLE_CSV_PATTERNS, DDX_Text, converter);
	BindOption(OPT_CMP_CSV_DELIM_CHAR, m_sCSVDelimiterChar, IDC_COMPARETABLE_CSV_DELIM_CHAR, DDX_Text);
	BindOption(OPT_CMP_TSV_FILEPATTERNS, m_sTSVFilePatterns, IDC_COMPARETABLE_TSV_PATTERNS, DDX_Text, converter);
	BindOption(OPT_CMP_DSV_FILEPATTERNS, m_sDSVFilePatterns, IDC_COMPARETABLE_DSV_PATTERNS, DDX_Text, converter);
	BindOption(OPT_CMP_DSV_DELIM_CHAR, m_sDSVDelimiterChar, IDC_COMPARETABLE_DSV_DELIM_CHAR, DDX_Text);
	BindOption(OPT_CMP_TBL_ALLOW_NEWLINES_IN_QUOTES, m_bAllowNewlinesInQuotes, IDC_COMPARETABLE_ALLOWNEWLINE, DDX_Check);
	BindOption(OPT_CMP_TBL_QUOTE_CHAR, m_sQuoteChar, IDC_COMPARETABLE_QUOTE_CHAR, DDX_Text);
}

void PropCompareTable::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompareTable)
	DDX_Control(pDX, IDC_COMPARETABLE_CSV_PATTERNS, m_comboCSVPatterns);
	DDX_Control(pDX, IDC_COMPARETABLE_TSV_PATTERNS, m_comboTSVPatterns);
	DDX_Control(pDX, IDC_COMPARETABLE_DSV_PATTERNS, m_comboDSVPatterns);
	//}}AFX_DATA_MAP
	DoDataExchangeBindOptions(pDX);
}


BEGIN_MESSAGE_MAP(PropCompareTable, OptionsPanel)
	//{{AFX_MSG_MAP(PropCompareTable)
	ON_BN_CLICKED(IDC_COMPARE_DEFAULTS, OnDefaults)
	ON_CBN_DROPDOWN(IDC_COMPARETABLE_CSV_PATTERNS, OnDropDownCSVPatterns)
	ON_CBN_CLOSEUP(IDC_COMPARETABLE_CSV_PATTERNS, OnCloseUpCSVPatterns)
	ON_CBN_DROPDOWN(IDC_COMPARETABLE_TSV_PATTERNS, OnDropDownTSVPatterns)
	ON_CBN_CLOSEUP(IDC_COMPARETABLE_TSV_PATTERNS, OnCloseUpTSVPatterns)
	ON_CBN_DROPDOWN(IDC_COMPARETABLE_DSV_PATTERNS, OnDropDownDSVPatterns)
	ON_CBN_CLOSEUP(IDC_COMPARETABLE_DSV_PATTERNS, OnCloseUpDSVPatterns)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/**
 * @brief Prepares multi-selection drop list 
 */
void PropCompareTable::OnDropDownCSVPatterns()
{
	String patterns = GetOptionsMgr()->GetDefault<String>(OPT_CMP_CSV_FILEPATTERNS);
	WildcardDropList::OnDropDown(m_comboCSVPatterns, 6, patterns.c_str());
}

/**
 * @brief Finishes drop list multi-selection
 */
void PropCompareTable::OnCloseUpCSVPatterns()
{
	WildcardDropList::OnCloseUp(m_comboCSVPatterns);
}

/**
 * @brief Prepares multi-selection drop list 
 */
void PropCompareTable::OnDropDownTSVPatterns()
{
	String patterns = GetOptionsMgr()->GetDefault<String>(OPT_CMP_TSV_FILEPATTERNS);
	WildcardDropList::OnDropDown(m_comboTSVPatterns, 6, patterns.c_str());
}

/**
 * @brief Finishes drop list multi-selection
 */
void PropCompareTable::OnCloseUpTSVPatterns()
{
	WildcardDropList::OnCloseUp(m_comboTSVPatterns);
}

/**
 * @brief Prepares multi-selection drop list 
 */
void PropCompareTable::OnDropDownDSVPatterns()
{
	String patterns = GetOptionsMgr()->GetDefault<String>(OPT_CMP_DSV_FILEPATTERNS);
	WildcardDropList::OnDropDown(m_comboDSVPatterns, 6, patterns.c_str());
}

/**
 * @brief Finishes drop list multi-selection
 */
void PropCompareTable::OnCloseUpDSVPatterns()
{
	WildcardDropList::OnCloseUp(m_comboDSVPatterns);
}
