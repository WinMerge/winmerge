/** 
 * @file  FileCmpReportDlg.cpp
 *
 * @brief Implementation file for FileCmpReport dialog
 *
 */

#include "stdafx.h"
#include "FileCmpReportDlg.h"
#include "FileOrFolderSelect.h"
#include "DiffImageListUtils.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(FileCmpReportDlg, CTrDialog)

FileCmpReportDlg::FileCmpReportDlg(CWnd* pParent) : CTrDialog(IDD_FILECMP_REPORT, pParent)
{}

void FileCmpReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_REPORT_WINDOW_LIST, m_list);
	DDX_Control(pDX, IDC_REPORT_FILE, m_ctlReportFile);
	DDX_Check(pDX, IDC_REPORT_COPYCLIPBOARD, m_options.copyToClipboard);
	DDX_Check(pDX, IDC_REPORT_OPENREPORTFILE, m_options.openReportFile);
	DDX_Check(pDX, IDC_REPORT_INCLUDE_ALL_IMAGE_PAGES, m_options.includeAllImagePages);
	DDX_CBString(pDX, IDC_REPORT_FILE, m_options.reportFile);
}

BEGIN_MESSAGE_MAP(FileCmpReportDlg, CTrDialog)
	ON_BN_CLICKED(IDC_REPORT_BROWSEFILE, OnBtnClickReportBrowse)
	ON_BN_DOUBLECLICKED(IDC_REPORT_COPYCLIPBOARD, OnBtnDblclickCopyClipboard)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void FileCmpReportDlg::LoadSettings()
{
	m_options.copyToClipboard = GetOptionsMgr()->GetBool(OPT_REPORTFILES_COPYTOCLIPBOARD);
	m_options.openReportFile = GetOptionsMgr()->GetBool(OPT_REPORTFILES_OPENREPORTFILE);
	m_options.includeAllImagePages = GetOptionsMgr()->GetBool(OPT_REPORTFILES_INCLUDEALLIMAGEPAGES);
}

BOOL FileCmpReportDlg::OnInitDialog()
{
	__super::OnInitDialog();

	LoadSettings();

	m_ctlReportFile.SetFileControlStates(true);
	m_ctlReportFile.LoadState(_T("ReportFiles"));

	m_list.Initialize();
	DiffImageListUtils::InitializeDiffImageList(m_list.GetImageList());
	m_list.SetImageList(&m_list.GetImageList(), LVSIL_SMALL);

	m_list.SetItems(m_items);

	String label;
	GetDlgItemText(IDC_REPORT_INFO, label);
	strutils::replace(label, _T("\\u24D8"), _T("\u24D8"));
	SetDlgItemText(IDC_REPORT_INFO, label);

	// Set selected path to variable so file selection dialog shows
	// correct filename and path.
	CString cstrReportFile;
	m_ctlReportFile.GetWindowText(cstrReportFile);
	m_options.reportFile = cstrReportFile;

	UpdateData(FALSE);

	return TRUE;
}

HBRUSH FileCmpReportDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC && pWnd->GetDlgCtrlID() == IDC_REPORT_INFO)
	{
		pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
		pDC->SetBkMode(TRANSPARENT);
	}
	return hbr;
}

void FileCmpReportDlg::CollectOptions()
{
	UpdateData(TRUE);

	m_options.selectedData = m_list.GetCheckedData();
}

void FileCmpReportDlg::OnBtnClickReportBrowse()
{
	String folder = m_options.reportFile;
	String chosenFilepath;
	if (SelectFile(GetSafeHwnd(), chosenFilepath, false, folder.c_str(), _T(""), _("HTML Files (*.htm,*.html)|*.htm;*.html|All Files (*.*)|*.*||"), _T("htm")))
	{
		m_options.reportFile = chosenFilepath;
		UpdateData(FALSE);
	}
}

void FileCmpReportDlg::OnBtnDblclickCopyClipboard()
{
	m_ctlReportFile.SetWindowText(_T(""));
}

void FileCmpReportDlg::OnOK()
{
	CollectOptions();
	m_ctlReportFile.SaveState(_T("ReportFiles"));
	GetOptionsMgr()->SaveOption(OPT_REPORTFILES_COPYTOCLIPBOARD, m_options.copyToClipboard);
	GetOptionsMgr()->SaveOption(OPT_REPORTFILES_OPENREPORTFILE, m_options.openReportFile);
	GetOptionsMgr()->SaveOption(OPT_REPORTFILES_INCLUDEALLIMAGEPAGES, m_options.includeAllImagePages);
	__super::OnOK();
}
