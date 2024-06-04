/** 
 * @file  DirFilterDlg.h
 *
 * @brief Declaration file for DirCmpReport Dialog.
 *
 */
#pragma once

#include "DirReportTypes.h"
#include "SuperComboBox.h"

/** 
 * @brief Folder compare dialog class.
 * This dialog (and class) showa folder-compare report's selections
 * for user. Also filename and path for report file can be chosen
 * with this dialog.
 */
class DirFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(DirFilterDlg)

public:
	explicit DirFilterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DIR_FILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBtnClickReportBrowse();
	afx_msg void OnBtnDblclickCopyClipboard();
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CSuperComboBox m_ctlReportFile; /**< Report filename control */
	CString m_sReportFile; /**< Report filename string */
	CComboBox m_ctlStyle; /**< Report type control */
	REPORT_TYPE m_nReportType; /**< Report type integer */
	bool m_bCopyToClipboard; /**< Do we copy report to clipboard? */
};
