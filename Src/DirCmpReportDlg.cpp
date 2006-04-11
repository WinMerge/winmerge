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
#include "DirReportTypes.h"
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

struct ReportTypeInfo
{
	REPORT_TYPE reportType; // enum
	int idDisplay; // resource string to display
	int browseFilter; // filter to use in common file save dialog
};
static ReportTypeInfo f_types[] = {
	{ REPORT_TYPE_COMMALIST,
		IDS_REPORT_COMMALIST,
		IDS_TEXT_REPORT_FILES
	},
	{ REPORT_TYPE_TABLIST,
		IDS_REPORT_TABLIST,
		IDS_TEXT_REPORT_FILES
	},
	{ REPORT_TYPE_SIMPLEHTML,
		IDS_REPORT_SIMPLEHTML,
		IDS_HTML_REPORT_FILES
	},
	{ REPORT_TYPE_SIMPLEXML,
		IDS_REPORT_SIMPLEXML,
		IDS_XML_REPORT_FILES
	},
};

/**
 * @brief Dialog initializer function.
 */
BOOL DirCmpReportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctlReportFile.LoadState(_T("ReportFiles"));

	for (int i=0; i<sizeof(f_types)/sizeof(f_types[0]); ++i)
	{
		const ReportTypeInfo & info = f_types[i];
		int ind = m_ctlStyle.InsertString(i, LoadResString(info.idDisplay));
		m_ctlStyle.SetItemData(ind, info.reportType);

	}
	m_ctlStyle.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Browse for report file.
 */
void DirCmpReportDlg::OnBtnClickReportBrowse()
{
	UpdateData(TRUE);

	CString title = LoadResString(IDS_SAVE_AS_TITLE);
	CString folder = m_sReportFile;
	int filterid = f_types[m_ctlStyle.GetCurSel()].browseFilter;

	CString chosenFilepath;
	if (SelectFile(chosenFilepath, folder, title, filterid, FALSE))
	{
		CString name;
		SplitFilename(chosenFilepath, &folder, &name, NULL);
		m_sReportFile = chosenFilepath;
		m_ctlReportFile.SetWindowText(chosenFilepath);
	}
}

/**
 * @brief Close dialog and create a report.
 */
void DirCmpReportDlg::OnOK()
{
	UpdateData(TRUE);

	int sel = m_ctlStyle.GetCurSel();
	m_nReportType = (REPORT_TYPE)m_ctlStyle.GetItemData(sel);

	if (m_sReportFile.IsEmpty())
	{
		AfxMessageBox(IDS_MUST_SPECIFY_OUTPUT, MB_ICONSTOP);
		m_ctlReportFile.SetFocus();
		return;
	}

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
