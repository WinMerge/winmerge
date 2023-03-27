/** 
 * @file  OpenTableDlg.cpp
 *
 * @brief Implementation of the dialog used to select table properties
 */

#include "stdafx.h"
#include "OpenTableDlg.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenTableDlg dialog


COpenTableDlg::COpenTableDlg(CWnd* pParent /*= nullptr*/)
: CTrDialog(COpenTableDlg::IDD, pParent)
, m_nFileType(0)
, m_sCSVDelimiterChar(GetOptionsMgr()->GetString(OPT_CMP_CSV_DELIM_CHAR))
, m_sDSVDelimiterChar(GetOptionsMgr()->GetString(OPT_CMP_DSV_DELIM_CHAR))
, m_sQuoteChar(_T("\""))
, m_bAllowNewlinesInQuotes(true)
{
	//{{AFX_DATA_INIT(COpenTableDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void COpenTableDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenTableDlg)
	DDX_Radio(pDX, IDC_COMPARETABLE_CSV, m_nFileType);
	DDX_Text(pDX, IDC_COMPARETABLE_CSV_DELIM_CHAR, m_sCSVDelimiterChar);
	DDX_Text(pDX, IDC_COMPARETABLE_DSV_DELIM_CHAR, m_sDSVDelimiterChar);
	DDX_Check(pDX, IDC_COMPARETABLE_ALLOWNEWLINE, m_bAllowNewlinesInQuotes);
	DDX_Text(pDX, IDC_COMPARETABLE_QUOTE_CHAR, m_sQuoteChar);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COpenTableDlg, CTrDialog)
	//{{AFX_MSG_MAP(COpenTableDlg)
	ON_COMMAND_RANGE(IDC_COMPARETABLE_CSV, IDC_COMPARETABLE_DSV, OnClickFileType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenTableDlg message handlers

void COpenTableDlg::OnOK()
{
	CDialog::OnOK();
	if (m_nFileType == 0/*CSV*/)
	{
		m_sDelimiterChar = m_sCSVDelimiterChar;
		GetOptionsMgr()->SaveOption(OPT_CMP_CSV_DELIM_CHAR, m_sCSVDelimiterChar);
	}
	else if (m_nFileType == 1/*TSV*/)
	{
		m_sDelimiterChar = _T("\t");
	}
	else
	{
		m_sDelimiterChar = m_sDSVDelimiterChar;
		GetOptionsMgr()->SaveOption(OPT_CMP_DSV_DELIM_CHAR, m_sDSVDelimiterChar);
	}
}

void COpenTableDlg::OnClickFileType(UINT nID)
{
	EnableDlgItem(IDC_COMPARETABLE_CSV_DELIM_CHAR, nID == IDC_COMPARETABLE_CSV);
	EnableDlgItem(IDC_COMPARETABLE_DSV_DELIM_CHAR, nID == IDC_COMPARETABLE_DSV);
}
