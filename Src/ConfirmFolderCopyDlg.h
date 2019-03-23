/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
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
