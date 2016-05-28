/** 
 * @file  DirCmpReportDlg.cpp
 *
 * @brief Implementation file for DirCmpReport dialog
 *
 */

#include "stdafx.h"
#include "DirCmpReportDlg.h"
#include "Coretools.h"
#include "DirReportTypes.h"
#include "paths.h"
#include "FileOrFolderSelect.h"

IMPLEMENT_DYNAMIC(DirCmpReportDlg, CTrDialog)

/**
 * @brief Constructor.
 */
DirCmpReportDlg::DirCmpReportDlg(CWnd* pParent /*=NULL*/)
	: CTrDialog(DirCmpReportDlg::IDD, pParent)
	, m_bCopyToClipboard(false)
	, m_bIncludeFileCmpReport(false)
	, m_nReportType(REPORT_TYPE_COMMALIST)
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
	CTrDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REPORT_FILE, m_ctlReportFile);
	DDX_Control(pDX, IDC_REPORT_STYLECOMBO, m_ctlStyle);
	DDX_Text(pDX, IDC_REPORT_FILE, m_sReportFile);
	DDX_Check(pDX, IDC_REPORT_COPYCLIPBOARD, m_bCopyToClipboard);
	DDX_Check(pDX, IDC_REPORT_INCLUDEFILECMPREPORT, m_bIncludeFileCmpReport);
}

BEGIN_MESSAGE_MAP(DirCmpReportDlg, CTrDialog)
	ON_BN_CLICKED(IDC_REPORT_BROWSEFILE, OnBtnClickReportBrowse)
	ON_BN_DOUBLECLICKED(IDC_REPORT_COPYCLIPBOARD, OnBtnDblclickCopyClipboard)
	ON_CBN_SELCHANGE(IDC_REPORT_STYLECOMBO, OnCbnSelchangeReportStylecombo)
END_MESSAGE_MAP()

/**
 * @brief Definition for structure containing report types.
 * This struct is used to form a report types list. This list
 * can be then used to initialize the GUI for reports.
 */
struct ReportTypeInfo
{
	REPORT_TYPE reportType; /**< Report-type ID */
	const char *idDisplay; /**< Resource-string ID (shown in file-selection dialog) */
	const char *browseFilter; /**< File-extension filter (resource-string ID) */
};

/**
 * @brief List of report types.
 * This list is used to initialize the GUI.
 */
static ReportTypeInfo f_types[] = {
	{ REPORT_TYPE_COMMALIST,
		"Comma-separated list",
		"Text Files (*.csv;*.asc;*.rpt;*.txt)|*.csv;*.asc;*.rpt;*.txt|All Files (*.*)|*.*||"
	},
	{ REPORT_TYPE_TABLIST,
		"Tab-separated list",
		"Text Files (*.csv;*.asc;*.rpt;*.txt)|*.csv;*.asc;*.rpt;*.txt|All Files (*.*)|*.*||"
	},
	{ REPORT_TYPE_SIMPLEHTML,
		"Simple HTML",
		"HTML Files (*.htm,*.html)|*.htm;*.html|All Files (*.*)|*.*||"
	},
	{ REPORT_TYPE_SIMPLEXML,
		"Simple XML",
		"XML Files (*.xml)|*.xml|All Files (*.*)|*.*||"
	},
};

/**
 * @brief Dialog initializer function.
 */
BOOL DirCmpReportDlg::OnInitDialog()
{
	CTrDialog::OnInitDialog();

	m_ctlReportFile.LoadState(_T("ReportFiles"));
	m_nReportType = static_cast<REPORT_TYPE>(AfxGetApp()->GetProfileInt(_T("ReportFiles"), _T("ReportType"), 0));
	m_bCopyToClipboard = !!AfxGetApp()->GetProfileInt(_T("ReportFiles"), _T("CopoyToClipboard"), false);
	m_bIncludeFileCmpReport = !!AfxGetApp()->GetProfileInt(_T("ReportFiles"), _T("IncludeFileCmpReport"), false);

	for (int i = 0; i < sizeof(f_types) / sizeof(f_types[0]); ++i)
	{
		const ReportTypeInfo & info = f_types[i];
		int ind = m_ctlStyle.InsertString(i, tr(info.idDisplay).c_str());
		m_ctlStyle.SetItemData(ind, info.reportType);
		if (info.reportType == m_nReportType)
			m_ctlStyle.SetCurSel(m_nReportType);
	}
	if (m_ctlStyle.GetCurSel() < 0)
		m_ctlStyle.SetCurSel(0);

	OnCbnSelchangeReportStylecombo();

	// Set selected path to variable so file selection dialog shows
	// correct filename and path.
	m_ctlReportFile.GetWindowText(PopString(m_sReportFile));

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Browse for report file.
 */
void DirCmpReportDlg::OnBtnClickReportBrowse()
{
	UpdateData(TRUE);

	String folder = m_sReportFile;
	String filter = tr(f_types[m_ctlStyle.GetCurSel()].browseFilter);

	String chosenFilepath;
	if (SelectFile(GetSafeHwnd(), chosenFilepath, folder.c_str(), _("Save As"),
			filter, FALSE))
	{
		m_sReportFile = chosenFilepath;
		m_ctlReportFile.SetWindowText(chosenFilepath.c_str());
	}
}

/**
 * @brief Erase report file name.
 */
void DirCmpReportDlg::OnBtnDblclickCopyClipboard()
{
	m_ctlReportFile.SetWindowText(_T(""));
}

void DirCmpReportDlg::OnCbnSelchangeReportStylecombo()
{
	EnableDlgItem(IDC_REPORT_INCLUDEFILECMPREPORT,
		m_ctlStyle.GetItemData(m_ctlStyle.GetCurSel()) == REPORT_TYPE_SIMPLEHTML);
}

/**
 * @brief Close dialog.
 */
void DirCmpReportDlg::OnOK()
{
	UpdateData(TRUE);

	int sel = m_ctlStyle.GetCurSel();
	m_nReportType = (REPORT_TYPE)m_ctlStyle.GetItemData(sel);

	if (m_sReportFile.empty() && !m_bCopyToClipboard)
	{
		LangMessageBox(IDS_MUST_SPECIFY_OUTPUT, MB_ICONSTOP);
		m_ctlReportFile.SetFocus();
		return;
	}

	if (!m_sReportFile.empty())
	{
		if (paths_DoesPathExist(m_sReportFile) == IS_EXISTING_FILE)
		{
			int overWrite = LangMessageBox(IDS_REPORT_FILEOVERWRITE,
					MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN,
					IDS_DIFF_FILEOVERWRITE);
			if (overWrite == IDNO)
				return;
		}
	}

	m_ctlReportFile.SaveState(_T("ReportFiles"));
	AfxGetApp()->WriteProfileInt(_T("ReportFiles"), _T("ReportType"), m_nReportType);
	AfxGetApp()->WriteProfileInt(_T("ReportFiles"), _T("CopoyToClipboard"), m_bCopyToClipboard);
	AfxGetApp()->WriteProfileInt(_T("ReportFiles"), _T("IncludeFileCmpReport"), m_bIncludeFileCmpReport);

	CTrDialog::OnOK();
}
