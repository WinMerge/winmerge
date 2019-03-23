/** 
 * @file  DirCmpReportDlg.h
 *
 * @brief Declaration file for DirCmpReport Dialog.
 *
 */
#pragma once

#include "DirReportTypes.h"
#include "TrDialogs.h"
#include "SuperComboBox.h"

/** 
 * @brief Folder compare dialog class.
 * This dialog (and class) shows folder-compare report's selections
 * for user. Also filename and path for report file can be chosen
 * with this dialog.
 */
class DirCmpReportDlg : public CTrDialog
{
	DECLARE_DYNAMIC(DirCmpReportDlg)

public:
	explicit DirCmpReportDlg(CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DIRCMP_REPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void LoadSettings();

	// Generated message map functions
	//{{AFX_MSG(DirCmpReportDlg)
	afx_msg void OnBtnClickReportBrowse();
	afx_msg void OnBtnDblclickCopyClipboard();
	afx_msg void OnCbnSelchangeReportStylecombo();
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	//}}AFX_MSG

	//{{AFX_DATA(DirCmpReportDlg)
	CSuperComboBox m_ctlReportFile; /**< Report filename control */
	String m_sReportFile; /**< Report filename string */
	CComboBox m_ctlStyle; /**< Report type control */
	REPORT_TYPE m_nReportType; /**< Report type integer */
	bool m_bCopyToClipboard; /**< Do we copy report to clipboard? */
	bool m_bIncludeFileCmpReport; /**< Do we include file compare report in folder compare report? */
	//}}AFX_DATA
};
