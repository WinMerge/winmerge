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
	using Item = CWindowListCtrl::Item;

	struct Options
	{
		String reportFile;
		bool includeAllImagePages = false;
		bool copyToClipboard = false;
		bool openReportFile = false;
		std::vector<uintptr_t> selectedData;
	};

	explicit FileCmpReportDlg(CWnd* pParent = nullptr);

// Dialog Data
	enum { IDD = IDD_FILECMP_REPORT };

	void SetItems(const std::vector<Item>& items) { m_items = items; }
	void SetReportFile(const String& path) { m_options.reportFile = path; }
	const Options& GetOptions() const { return m_options; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(FileCmpReportDlg)
	afx_msg void OnBtnClickReportBrowse();
	afx_msg void OnBtnDblclickCopyClipboard();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP();

private:
	void LoadSettings();
	void CollectOptions();

private:
	std::vector<Item>	 m_items;
	Options				 m_options;

	//{{AFX_DATA(FileCmpReportDlg)
	CWindowListCtrl m_list;
	CSuperComboBox m_ctlReportFile;
	//}}AFX_DATA
};
