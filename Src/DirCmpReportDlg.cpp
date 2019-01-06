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
#include "OptionsMgr.h"
#include "OptionsDef.h"

IMPLEMENT_DYNAMIC(DirCmpReportDlg, CTrDialog)

/**
 * @brief Constructor.
 */
DirCmpReportDlg::DirCmpReportDlg(CWnd* pParent /*= nullptr*/)
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

void DirCmpReportDlg::LoadSettings()
{
	m_nReportType = static_cast<REPORT_TYPE>(GetOptionsMgr()->GetInt(OPT_REPORTFILES_REPORTTYPE));
	m_bCopyToClipboard = GetOptionsMgr()->GetBool(OPT_REPORTFILES_COPYTOCLIPBOARD);
	m_bIncludeFileCmpReport = GetOptionsMgr()->GetBool(OPT_REPORTFILES_INCLUDEFILECMPREPORT);
}

/**
 * @brief Dialog initializer function.
 */
BOOL DirCmpReportDlg::OnInitDialog()
{
	CTrDialog::OnInitDialog();

	LoadSettings();

	m_ctlReportFile.LoadState(_T("ReportFiles"));

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
	CString cstrReportFile;
	m_ctlReportFile.GetWindowText(cstrReportFile);
	m_sReportFile = cstrReportFile;

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
	if (SelectFile(GetSafeHwnd(), chosenFilepath, false, folder.c_str(), _T(""), filter))
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
		if (paths::DoesPathExist(m_sReportFile) == paths::IS_EXISTING_FILE)
		{
			int overWrite = LangMessageBox(IDS_REPORT_FILEOVERWRITE,
					MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN,
					IDS_DIFF_FILEOVERWRITE);
			if (overWrite == IDNO)
				return;
		}
	}

	m_ctlReportFile.SaveState(_T("ReportFiles"));
	GetOptionsMgr()->SaveOption(OPT_REPORTFILES_REPORTTYPE, static_cast<int>(m_nReportType));
	GetOptionsMgr()->SaveOption(OPT_REPORTFILES_COPYTOCLIPBOARD, m_bCopyToClipboard);
	GetOptionsMgr()->SaveOption(OPT_REPORTFILES_INCLUDEFILECMPREPORT, m_bIncludeFileCmpReport);

	CTrDialog::OnOK();
}
