/** 
 * @file  TestFilterDlg.cpp
 *
 * @brief Dialog for testing file filters
 */

#include "stdafx.h"
#include "TestFilterDlg.h"
#include "resource.h"
#include "UnicodeString.h"
#include "FileFilterMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Constructor.
 * @param [in] pParent Parent window.
 * @param [in] pFileFilter File filter to test.
 * @param [in] pFilterMgr File filter manager.
 */
CTestFilterDlg::CTestFilterDlg(CWnd* pParent, FileFilter * pFileFilter, FileFilterMgr *pFilterMgr)
: CTrDialog(CTestFilterDlg::IDD, pParent)
, m_pFileFilter(pFileFilter)
, m_pFileFilterMgr(pFilterMgr)
{
}

void CTestFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestFilterDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestFilterDlg, CTrDialog)
	//{{AFX_MSG_MAP(CTestFilterDlg)
	ON_BN_CLICKED(IDC_TEST_BTN, OnTestBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestFilterDlg message handlers

/**
 * @brief Initialize the dialog.
 * @return FALSE always.
 */
BOOL CTestFilterDlg::OnInitDialog()
{
	CTrDialog::OnInitDialog();

	SetDlgItemFocus(IDC_TEST_TEXT);

	String name = m_pFileFilterMgr->GetFilterName(m_pFileFilter);
	SetDlgItemText(IDC_HEADER_FILTER_NAME, name);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** @brief User pressed Text button. */
void CTestFilterDlg::OnTestBtn() 
{
	String text;
	GetDlgItemText(IDC_TEST_TEXT, text);

	bool passed = CheckText(text);

	String result = (passed ? _T("passed") : _T("failed"));
	text += _T(": ") + result;

	AppendResult(text);
}

/**
 * @brief Test text against filter.
 * @param [in] text Text to test.
 * @return true if text passes the filter, `false` otherwise.
 */
bool CTestFilterDlg::CheckText(String text) const
{
	bool isDir = (IsDlgButtonChecked(IDC_IS_DIRECTORY) == 1);
	if (isDir)
	{
		// Convert any forward slashes to canonical Windows-style backslashes
		strutils::replace(text, _T("/"), _T("\\"));
		return m_pFileFilterMgr->TestDirNameAgainstFilter(m_pFileFilter, text);
	}
	else
	{
		return m_pFileFilterMgr->TestFileNameAgainstFilter(m_pFileFilter, text);
	}
}

/**
 * @brief Add new result to end of result edit box.
 * @param [in] result Result text to add.
 */
void CTestFilterDlg::AppendResult(const String& result)
{
	String text;
	GetDlgItemText(IDC_RESULTS, text);
	if (text.length() > 0)
		text += _T("\r\n") + result;
	else
		text = result;
	SetDlgItemText(IDC_RESULTS, text);
	SendDlgItemMessage(IDC_RESULTS, WM_VSCROLL, SB_BOTTOM, 0L);
}
