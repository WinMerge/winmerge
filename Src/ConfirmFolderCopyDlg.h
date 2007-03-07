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
 * @file  SaveClosingDlg.h
 *
 * @brief Declaration file for ConfirmFolderCopyDlg dialog
 */
// ID line follows -- this is updated by SVN
// $Id: $

/**
 * @brief Dialog for item copy confirmation.
 */
class ConfirmFolderCopyDlg : public CDialog
{
	DECLARE_DYNAMIC(ConfirmFolderCopyDlg)

public:
	ConfirmFolderCopyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ConfirmFolderCopyDlg();

// Dialog Data
	enum { IDD = IDD_CONFIRM_COPY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	
	CString m_question;
	CString m_fromText;
	CString m_fromPath;
	CString m_toText;
	CString m_toPath;
	afx_msg void OnBnClickedNo();
	afx_msg void OnBnClickedYes();
};
