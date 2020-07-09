/** 
 * @file  OpenTableDlg.cpp
 *
 * @brief Implementation of the dialog used to select table properties
 */

#include "stdafx.h"
#include "OpenTableDlg.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenTableDlg dialog


COpenTableDlg::COpenTableDlg(CWnd* pParent /*= nullptr*/)
: CTrDialog(COpenTableDlg::IDD, pParent)
, m_nFileType(0)
, m_sDelimiterChar(_T(","))
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
	DDX_Text(pDX, IDC_COMPARETABLE_DSV_DELIM_CHAR, m_sDelimiterChar);
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
		m_sDelimiterChar = _T(",");
	else if (m_nFileType == 1/*TSV*/)
		m_sDelimiterChar = _T("\t");
}

void COpenTableDlg::OnClickFileType(UINT nID)
{
	EnableDlgItem(IDC_COMPARETABLE_DSV_DELIM_CHAR, nID == IDC_COMPARETABLE_DSV);
}
