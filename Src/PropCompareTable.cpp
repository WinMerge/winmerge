/** 
 * @file  PropCompareTable.cpp
 *
 * @brief Implementation of PropCompareTable propertysheet
 */

#include "stdafx.h"
#include "PropCompareTable.h"
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
, m_sDSVDelimiterChar(_T(","))
, m_sQuoteChar(_T("\""))
{
}

void PropCompareTable::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropCompareTable)
	DDX_Text(pDX, IDC_COMPARETABLE_CSV_PATTERNS, m_sCSVFilePatterns);
	DDX_Text(pDX, IDC_COMPARETABLE_TSV_PATTERNS, m_sTSVFilePatterns);
	DDX_Text(pDX, IDC_COMPARETABLE_DSV_PATTERNS, m_sDSVFilePatterns);
	DDX_Text(pDX, IDC_COMPARETABLE_DSV_DELIM_CHAR, m_sDSVDelimiterChar);
	DDX_Check(pDX, IDC_COMPARETABLE_ALLOWNEWLINE, m_bAllowNewlinesInQuotes);
	DDX_Text(pDX, IDC_COMPARETABLE_QUOTE_CHAR, m_sQuoteChar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropCompareTable, CPropertyPage)
	//{{AFX_MSG_MAP(PropCompareTable)
	ON_BN_CLICKED(IDC_COMPARETABLE_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 * Property sheet calls this before displaying GUI to load values
 * into members.
 */
void PropCompareTable::ReadOptions()
{
	m_sCSVFilePatterns = GetOptionsMgr()->GetString(OPT_CMP_CSV_FILEPATTERNS);
	m_sTSVFilePatterns = GetOptionsMgr()->GetString(OPT_CMP_TSV_FILEPATTERNS);
	m_sDSVFilePatterns = GetOptionsMgr()->GetString(OPT_CMP_DSV_FILEPATTERNS);
	m_sDSVDelimiterChar = GetOptionsMgr()->GetString(OPT_CMP_DSV_DELIM_CHAR).substr(0, 1);
	m_bAllowNewlinesInQuotes = GetOptionsMgr()->GetBool(OPT_CMP_TBL_ALLOW_NEWLINES_IN_QUOTES);
	m_sQuoteChar = GetOptionsMgr()->GetString(OPT_CMP_TBL_QUOTE_CHAR).substr(0, 1);
}

/** 
 * @brief Writes options values from UI to storage.
 * Property sheet calls this after dialog is closed with OK button to
 * store values in member variables.
 */
void PropCompareTable::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_CMP_CSV_FILEPATTERNS, m_sCSVFilePatterns);
	GetOptionsMgr()->SaveOption(OPT_CMP_TSV_FILEPATTERNS, m_sTSVFilePatterns);
	GetOptionsMgr()->SaveOption(OPT_CMP_DSV_FILEPATTERNS, m_sDSVFilePatterns);
	GetOptionsMgr()->SaveOption(OPT_CMP_DSV_DELIM_CHAR, m_sDSVDelimiterChar.substr(0, 1));
	GetOptionsMgr()->SaveOption(OPT_CMP_TBL_ALLOW_NEWLINES_IN_QUOTES, m_bAllowNewlinesInQuotes);
	GetOptionsMgr()->SaveOption(OPT_CMP_TBL_QUOTE_CHAR, m_sQuoteChar.substr(0, 1));
}

/** 
 * @brief Sets options to defaults
 */
void PropCompareTable::OnDefaults()
{
	m_sCSVFilePatterns = GetOptionsMgr()->GetDefault<String>(OPT_CMP_CSV_FILEPATTERNS);
	m_sTSVFilePatterns = GetOptionsMgr()->GetDefault<String>(OPT_CMP_TSV_FILEPATTERNS);
	m_sDSVFilePatterns = GetOptionsMgr()->GetDefault<String>(OPT_CMP_DSV_FILEPATTERNS);
	m_sDSVDelimiterChar = GetOptionsMgr()->GetDefault<String>(OPT_CMP_DSV_DELIM_CHAR);
	m_bAllowNewlinesInQuotes = GetOptionsMgr()->GetDefault<bool>(OPT_CMP_TBL_ALLOW_NEWLINES_IN_QUOTES);
	m_sQuoteChar = GetOptionsMgr()->GetDefault<String>(OPT_CMP_TBL_QUOTE_CHAR);
	UpdateData(FALSE);
}
