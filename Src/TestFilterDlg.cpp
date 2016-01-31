/** 
 * @file  TestFilterDlg.cpp
 *
 * @brief Dialog for testing file filters
 */

#include "stdafx.h"
#include "TestFilterDlg.h"
#include "Merge.h"
#include "resource.h"
#include "UnicodeString.h"
#include "FileFilterMgr.h"
#include "DDXHelper.h"

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
: CDialog(CTestFilterDlg::IDD, pParent)
, m_pFileFilter(pFileFilter)
, m_pFileFilterMgr(pFilterMgr)
{
}

void CTestFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestFilterDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestFilterDlg, CDialog)
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
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	GetDlgItem(IDC_TEST_TEXT)->SetFocus();

	String name = m_pFileFilterMgr->GetFilterName(m_pFileFilter);
	SetDlgItemText(IDC_HEADER_FILTER_NAME, name.c_str());
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** @brief User pressed Text button. */
void CTestFilterDlg::OnTestBtn() 
{
	String text;
	GetDlgItemText(IDC_TEST_TEXT, PopString(text));

	bool passed = CheckText(text);

	String result = (passed ? _T("passed") : _T("failed"));
	text += _T(": ") + result;

	AppendResult(text);
}

/** @brief user pressed <enter> key. */
void CTestFilterDlg::OnOK()
{
   CWnd *pWnd = GetFocus(); 
   ASSERT (pWnd); 
   if (IDCANCEL == pWnd->GetDlgCtrlID()) 
   { 
       CDialog::OnCancel(); 
   }
   else
   {
	   OnTestBtn();
   }
}

/**
 * @brief Test text against filter.
 * @param [in] text Text to test.
 * @return true if text passes the filter, FALSE otherwise.
 */
bool CTestFilterDlg::CheckText(String text) const
{
	CButton * IsDirButton = (CButton *)GetDlgItem(IDC_IS_DIRECTORY);
	bool isDir = (IsDirButton->GetCheck() == BST_CHECKED);
	if (isDir)
	{
		// Convert any forward slashes to canonical Windows-style backslashes
		string_replace(text, _T("/"), _T("\\"));
		return m_pFileFilterMgr->TestDirNameAgainstFilter(m_pFileFilter, text);
	}
	else
	{
		return m_pFileFilterMgr->TestFileNameAgainstFilter(m_pFileFilter, text);
	}
}

/**
 * @brief Add text to end of edit control.
 * @param [in] edit Edit contror into which the text is added.
 * @param [in] text Text to add to edit control.
 */
void AppendToEditBox(CEdit & edit, const String& text)
{
	int len = edit.GetWindowTextLength();
	edit.SetSel(len, len);
	edit.ReplaceSel(text.c_str());
}

/**
 * @brief Add new result to end of result edit box.
 * @param [in] result Result text to add.
 */
void CTestFilterDlg::AppendResult(String result)
{
	CEdit * edit = (CEdit *)GetDlgItem(IDC_RESULTS);
	if (edit->GetWindowTextLength()>0)
		result = _T("\r\n") + result;
	AppendToEditBox(*edit, result);
}
