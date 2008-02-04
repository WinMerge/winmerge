/** 
 * @file  SharedFilterDlg.cpp
 *
 * @brief Dialog where user choose shared or private filter
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "SharedFilterDlg.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSharedFilterDlg dialog


/**
 * @brief A constructor.
 */
CSharedFilterDlg::CSharedFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSharedFilterDlg::IDD, pParent)
{
}


void CSharedFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSharedFilterDlg)
	DDX_Control(pDX, IDC_SHARED, m_SharedButton);
	DDX_Control(pDX, IDC_PRIVATE, m_PrivateButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSharedFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CSharedFilterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSharedFilterDlg message handlers

/**
 * @brief Dialog initialization.
 */
BOOL CSharedFilterDlg::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	if (GetOptionsMgr()->GetBool(OPT_FILEFILTER_SHARED))
		m_SharedButton.SetCheck(BST_CHECKED);
	else
		m_PrivateButton.SetCheck(BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Called when user closes the dialog with OK button.
 */
void CSharedFilterDlg::OnOK()
{
	BOOL bShared = (m_SharedButton.GetCheck() == BST_CHECKED);
	GetOptionsMgr()->SaveOption(OPT_FILEFILTER_SHARED, bShared);
	if (bShared)
		m_ChosenFolder = m_SharedFolder;
	else
		m_ChosenFolder = m_PrivateFolder;

	CDialog::OnOK();
}

/**
 * @brief Show user a selection dialog for shared/private filter creation.
 * @param [in] parent Parent window pointer.
 * @param [in] SharedFolder Folder for shared filters.
 * @param [in] PrivateFolder Folder for private filters.
 * @return Selected filter folder (shared or private).
 */
String CSharedFilterDlg::PromptForNewFilter(CWnd * Parent,
		const String &SharedFolder, const String &PrivateFolder)
{
	CSharedFilterDlg dlg(Parent);
	dlg.m_SharedFolder = SharedFolder;
	dlg.m_PrivateFolder = PrivateFolder;
	if (dlg.DoModal() != IDOK)
		return _T("");
	return dlg.m_ChosenFolder;
}
