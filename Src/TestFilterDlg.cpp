/** 
 * @file  TestFilterDlg.cpp
 *
 * @brief Dialog for testing file filters
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "resource.h"
#include "TestFilterDlg.h"
#include "FileFilterMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestFilterDlg dialog


CTestFilterDlg::CTestFilterDlg(CWnd* pParent, FileFilter * pFileFilter, FileFilterMgr *pFilterMgr)
: CDialog(CTestFilterDlg::IDD, pParent)
, m_pFileFilter(pFileFilter)
, m_pFileFilterMgr(pFilterMgr)
{
	//{{AFX_DATA_INIT(CTestFilterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
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

BOOL CTestFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_TEST_TEXT)->SetFocus();

	CString name = m_pFileFilterMgr->GetFilterName(m_pFileFilter);
	SetDlgItemText(IDC_HEADER_FILTER_NAME, name);
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** @brief User pressed Text button */
void CTestFilterDlg::OnTestBtn() 
{
	CString text;
	GetDlgItemText(IDC_TEST_TEXT, text);

	BOOL passed = CheckText(text);

	CString result = (passed ? _T("passed") : _T("failed"));
	text += _T(": ") + result;

	AppendResult(text);
}

/** @brief user pressed <enter> key */
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

/** @brief Return true if specified text passes our filter */
BOOL CTestFilterDlg::CheckText(CString text) const
{
	CButton * IsDirButton = (CButton *)GetDlgItem(IDC_IS_DIRECTORY);
	bool isDir = (IsDirButton->GetCheck() == BST_CHECKED);
	if (isDir)
	{
		// Convert any forward slashes to canonical Windows-style backslashes
		text.Replace('/', '\\');
		return m_pFileFilterMgr->TestDirNameAgainstFilter(m_pFileFilter, text);
	}
	else
	{
		return m_pFileFilterMgr->TestFileNameAgainstFilter(m_pFileFilter, text);
	}

}

/** @brief Add text to end of edit control */
void AppendToEditBox(CEdit & edit, LPCTSTR text)
{
	int len = edit.GetWindowTextLength();
	edit.SetSel(len, len);
	edit.ReplaceSel(text);
}

/** @brief Add new result to end of result edit box */
void CTestFilterDlg::AppendResult(CString result)
{
	CEdit * edit = (CEdit *)GetDlgItem(IDC_RESULTS);
	if (edit->GetWindowTextLength()>0)
		result = (CString)_T("\r\n") + result;
	AppendToEditBox(*edit, result);
}
