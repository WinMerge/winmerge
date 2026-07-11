/** 
 * @file  FileCmpReportDlg.h
 *
 * @brief Declaration file for FileCmpReport Dialog.
 *
 */
#pragma once

#include "TrDialogs.h"
#include "SuperComboBox.h"
#include "WindowListCtrl.h"

struct IMergeDoc;

/** 
 * @brief File compare dialog class.
 * This dialog (and class) shows file-compare report's selections
 * for user. Also filename and path for report file can be chosen
 * with this dialog.
 */
class FileCmpReportDlg : public CTrDialog
{
	DECLARE_DYNAMIC(FileCmpReportDlg)

public:
	using WindowItem = CWindowListCtrl::WindowItem;

	struct Options
	{
		String reportFile;
		bool includeAllImagePages = false;
		bool copyToClipboard = false;
		std::vector<uintptr_t> selectedData;
	};

	explicit FileCmpReportDlg(CWnd* pParent = nullptr);

// Dialog Data
	enum { IDD = IDD_FILECMP_REPORT };

	void SetWindows(const std::vector<WindowItem>& windows) { m_windows = windows; }
	void SetReportFile(const String& path) { m_options.reportFile = path; }
	const Options& GetOptions() const { return m_options; }

protected:
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(FileCmpReportDlg)
	afx_msg void OnBtnClickReportBrowse();
	afx_msg void OnBtnDblclickCopyClipboard();
	virtual void OnOK() override;
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP();

private:
	void LoadSettings();
	void CollectOptions();

private:
	std::vector<WindowItem> m_windows;
	Options				 m_options;

	//{{AFX_DATA(FileCmpReportDlg)
	CWindowListCtrl m_list;
	CSuperComboBox m_ctlReportFile;
	//}}AFX_DATA
};
