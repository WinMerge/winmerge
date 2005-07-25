/** 
 * @file  DirCmpReportDlg.cpp
 *
 * @brief Implementation file for DirCmpReport dialog
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$
//

#include "stdafx.h"
#include "Merge.h"
#include "Coretools.h"
#include "DirCmpReportDlg.h"
#include "DirCmpReport.h"
#include "paths.h"

IMPLEMENT_DYNAMIC(DirCmpReportDlg, CDialog)

/**
 * @brief Constructor.
 */
DirCmpReportDlg::DirCmpReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DirCmpReportDlg::IDD, pParent)
{
}

DirCmpReportDlg::~DirCmpReportDlg()
{
}

void DirCmpReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REPORT_FILE, m_ctlReportFile);
	DDX_Control(pDX, IDC_REPORT_STYLECOMBO, m_ctlStyle);
	DDX_Text(pDX, IDC_REPORT_FILE, m_sReportFile);

}


BEGIN_MESSAGE_MAP(DirCmpReportDlg, CDialog)
	ON_BN_CLICKED(IDC_REPORT_BROWSEFILE, OnBtnClickReportBrowse)
END_MESSAGE_MAP()


/**
 * @brief Dialog initializer function.
 */
BOOL DirCmpReportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctlReportFile.LoadState(_T("ReportFiles"));

	CString str;
	VERIFY(str.LoadString(IDS_REPORT_COMMALIST));
	int ind = m_ctlStyle.InsertString(0, str);
	m_ctlStyle.SetItemData(ind, DirCmpReport::REPORT_COMMALIST);
	m_ctlStyle.SelectString(0, str);
	VERIFY(str.LoadString(IDS_REPORT_TABLIST));
	ind = m_ctlStyle.InsertString(1, str);
	m_ctlStyle.SetItemData(ind, DirCmpReport::REPORT_TABLIST);
	VERIFY(str.LoadString(IDS_REPORT_SIMPLEHTML));
	ind = m_ctlStyle.InsertString(2, str);
	m_ctlStyle.SetItemData(ind, DirCmpReport::REPORT_SIMPLEHTML);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Browse for report file.
 */
void DirCmpReportDlg::OnBtnClickReportBrowse()
{
	CString s;
	CString folder;
	CString name;
	CString title;

	UpdateData(TRUE);
	VERIFY(title.LoadString(IDS_SAVE_AS_TITLE));
	folder = m_sReportFile;
	if (SelectFile(s, folder, title, NULL, FALSE))
	{
		SplitFilename(s, &folder, &name, NULL);
		m_sReportFile = s;
		m_ctlReportFile.SetWindowText(s);
	}
}

/**
 * @brief Close dialog and create a report.
 */
void DirCmpReportDlg::OnOK()
{
	UpdateData(TRUE);

	int sel = m_ctlStyle.GetCurSel();
	m_nReportType = m_ctlStyle.GetItemData(sel);

	if (paths_DoesPathExist(m_sReportFile) == IS_EXISTING_FILE)
	{
		int overWrite = AfxMessageBox(IDS_REPORT_FILEOVERWRITE,
				MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN,
				IDS_DIFF_FILEOVERWRITE);
		if (overWrite == IDNO)
			return;
	}

	m_ctlReportFile.SaveState(_T("ReportFiles"));

	CDialog::OnOK();
}
