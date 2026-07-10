/** 
 * @file  FileCmpReportDlg.cpp
 *
 * @brief Implementation file for FileCmpReport dialog
 *
 */

#include "stdafx.h"
#include "FileCmpReportDlg.h"
#include "FileOrFolderSelect.h"

IMPLEMENT_DYNAMIC(FileCmpReportDlg, CTrDialog)

FileCmpReportDlg::FileCmpReportDlg(CWnd* pParent)
	: CTrDialog(IDD_FILECMP_REPORT, pParent)
{}

BEGIN_MESSAGE_MAP(FileCmpReportDlg, CTrDialog)
	ON_BN_CLICKED(IDC_REPORT_BROWSEFILE, &FileCmpReportDlg::OnBtnClickReportBrowse)
	ON_BN_CLICKED(IDC_REPORT_COPYCLIPBOARD, &FileCmpReportDlg::OnBnClickedCopyClipboard)
END_MESSAGE_MAP()

void FileCmpReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_REPORT_WINDOW_LIST, m_list);
	DDX_Control(pDX, IDC_REPORT_FILE, m_ctlReportFile);
	DDX_Check(pDX, IDC_REPORT_INCLUDE_ALL_IMAGE_PAGES, m_options.includeAllImagePages);
	DDX_CBString(pDX, IDC_REPORT_FILE, m_options.reportFile);
}

BOOL FileCmpReportDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_ctlReportFile.LoadState(_T("ReportFiles"));
	CRect rc;
	m_list.GetClientRect(&rc);
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(0, _T("Title"), LVCFMT_LEFT, rc.Width());
	m_imageList.Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 | ILC_MASK, 4, 4);
	m_list.SetImageList(&m_imageList, LVSIL_SMALL);

	for (const auto& windowItem: m_windows)
	{
		HWND hwndFrame = windowItem.pFrame->GetSafeHwnd();
		HICON hIcon = (HICON)::SendMessage(hwndFrame, WM_GETICON, ICON_SMALL2, 0);
		if (hIcon == nullptr)
			hIcon = (HICON)::GetClassLongPtr(hwndFrame, GCLP_HICONSM);
		int image = m_imageList.Add(hIcon);
		CString title;
		windowItem.pFrame->GetWindowText(title);
		LVITEM item = {};
		item.mask = LVIF_TEXT | LVIF_IMAGE;
		item.iItem = m_list.GetItemCount();
		item.iImage = image;
		item.pszText = title.GetBuffer();
		int index = m_list.InsertItem(&item);
		m_list.SetCheck(index, windowItem.checked);
	}

	if (!m_options.reportFile.empty())
		m_ctlReportFile.SetWindowText(m_options.reportFile.c_str());

	return TRUE;
}

void FileCmpReportDlg::CollectOptions()
{
	UpdateData(TRUE);

	m_options.selectedData.clear();

	for (int i = 0; i < (int)m_windows.size(); ++i)
	{
		if (m_list.GetCheck(i))
			m_options.selectedData.push_back(m_windows[i].data);
	}
}

void FileCmpReportDlg::OnOK()
{
	CollectOptions();
	m_ctlReportFile.SaveState(_T("ReportFiles"));
	__super::OnOK();
}

void FileCmpReportDlg::OnBnClickedCopyClipboard()
{
	CollectOptions();
	EndDialog(IDC_REPORT_COPYCLIPBOARD);
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

