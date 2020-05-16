// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  ConfirmFolderCopyDlg.h
 *
 * @brief Declaration file for ConfirmFolderCopyDlg dialog
 */
#pragma once

#include "TrDialogs.h"
#include "CMoveConstraint.h"
#include "UnicodeString.h"

/**
 * @brief Dialog for item copy confirmation.
 */
class ConfirmFolderCopyDlg : public CTrDialog
{
	DECLARE_DYNAMIC(ConfirmFolderCopyDlg)

public:
	explicit ConfirmFolderCopyDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ConfirmFolderCopyDlg();

// Dialog Data
	enum { IDD = IDD_CONFIRM_COPY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog() override;

	DECLARE_MESSAGE_MAP()
public:
	
	String m_question;
	String m_fromText;
	String m_fromPath;
	String m_toText;
	String m_toPath;
	String m_caption;
	afx_msg void OnBnClickedNo();
	afx_msg void OnBnClickedYes();

private:
	prdlg::CMoveConstraint m_constraint; //*< Resizes dialog controls when dialog resized */
};
