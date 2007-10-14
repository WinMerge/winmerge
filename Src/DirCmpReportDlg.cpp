/** 
 * @file  DirCmpReportDlg.cpp
 *
 * @brief Implementation file for DirCmpReport dialog
 *
 */
// ID line follows -- this is updated by SVN
// $Id$
//

#include "stdafx.h"
#include "Coretools.h"
#include "DirCmpReportDlg.h"
#include "DirReportTypes.h"
#include "paths.h"
#include "FileOrFolderSelect.h"
#include "Merge.h"

IMPLEMENT_DYNAMIC(DirCmpReportDlg, CDialog)

/**
 * @brief Constructor.
 */
DirCmpReportDlg::DirCmpReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DirCmpReportDlg::IDD, pParent)
	, m_bCopyToClipboard(FALSE)
{
}

/**
 * @brief Map dialog controls to member variables.
 * This function maps dialog controls with member variables so
 * when UpdateData() is called controls and member variables
 * get updated.
 */
void DirCmpReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REPORT_FILE, m_ctlReportFile);
	DDX_Control(pDX, IDC_REPORT_STYLECOMBO, m_ctlStyle);
	DDX_Text(pDX, IDC_REPORT_FILE, m_sReportFile);
	DDX_Check(pDX, IDC_REPORT_COPYCLIPBOARD, m_bCopyToClipboard);
}

BEGIN_MESSAGE_MAP(DirCmpReportDlg, CDialog)
	ON_BN_CLICKED(IDC_REPORT_BROWSEFILE, OnBtnClickReportBrowse)
	ON_BN_DOUBLECLICKED(IDC_REPORT_COPYCLIPBOARD, OnBtnDblclickCopyClipboard)
END_MESSAGE_MAP()

/**
 * @brief Definition for structure containing report types.
 * This struct is used to form a report types list. This list
 * can be then used to initialize the GUI for reports.
 */
struct ReportTypeInfo
{
	REPORT_TYPE reportType; /**< Report-type ID */
	int idDisplay; /**< Resource-string ID (shown in file-selection dialog) */
	int browseFilter; /**< File-extension filter (resource-string ID) */
};

/**
 * @brief List of report types.
 * This list is used to initialize the GUI.
 */
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
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	m_ctlReportFile.LoadState(_T("ReportFiles"));

	for (int i = 0; i < sizeof(f_types) / sizeof(f_types[0]); ++i)
	{
		const ReportTypeInfo & info = f_types[i];
		int ind = m_ctlStyle.InsertString(i, theApp.LoadString(info.idDisplay).c_str());
		m_ctlStyle.SetItemData(ind, info.reportType);

	}
	m_ctlStyle.SetCurSel(0);
	// Set selected path to variable so file selection dialog shows
	// correct filename and path.
	m_ctlReportFile.GetWindowText(m_sReportFile);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Browse for report file.
 */
void DirCmpReportDlg::OnBtnClickReportBrowse()
{
	UpdateData(TRUE);

	CString folder = m_sReportFile;
	int filterid = f_types[m_ctlStyle.GetCurSel()].browseFilter;

	CString chosenFilepath;
	if (SelectFile(GetSafeHwnd(), chosenFilepath, folder, IDS_SAVE_AS_TITLE,
			filterid, FALSE))
	{
		m_sReportFile = chosenFilepath;
		m_ctlReportFile.SetWindowText(chosenFilepath);
	}
}

/**
 * @brief Erase report file name.
 */
void DirCmpReportDlg::OnBtnDblclickCopyClipboard()
{
	m_ctlReportFile.SetWindowText(_T(""));
}

/**
 * @brief Close dialog.
 */
void DirCmpReportDlg::OnOK()
{
	UpdateData(TRUE);

	int sel = m_ctlStyle.GetCurSel();
	m_nReportType = (REPORT_TYPE)m_ctlStyle.GetItemData(sel);

	if (m_sReportFile.IsEmpty() && !m_bCopyToClipboard)
	{
		AfxMessageBox(IDS_MUST_SPECIFY_OUTPUT, MB_ICONSTOP);
		m_ctlReportFile.SetFocus();
		return;
	}

	if (!m_sReportFile.IsEmpty())
	{
		if (paths_DoesPathExist(m_sReportFile) == IS_EXISTING_FILE)
		{
			int overWrite = AfxMessageBox(IDS_REPORT_FILEOVERWRITE,
					MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN,
					IDS_DIFF_FILEOVERWRITE);
			if (overWrite == IDNO)
				return;
		}
	}

	m_ctlReportFile.SaveState(_T("ReportFiles"));

	CDialog::OnOK();
}
