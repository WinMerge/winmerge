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
 * @file  ConfirmFolderCopyDlg.cpp
 *
 * @brief Implementation file for ConfirmFolderCopyDlg dialog
 */

#include "stdafx.h"
#include "ConfirmFolderCopyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ConfirmFolderCopyDlg dialog

IMPLEMENT_DYNAMIC(ConfirmFolderCopyDlg, CTrDialog)
ConfirmFolderCopyDlg::ConfirmFolderCopyDlg(CWnd* pParent /*= nullptr*/)
	: CTrDialog(ConfirmFolderCopyDlg::IDD, pParent)
{
}

ConfirmFolderCopyDlg::~ConfirmFolderCopyDlg()
{
}

void ConfirmFolderCopyDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ConfirmFolderCopyDlg)
	DDX_Text(pDX, IDC_FLDCONFIRM_FROM_TEXT, m_fromText);
	DDX_Text(pDX, IDC_FLDCONFIRM_TO_TEXT, m_toText);
	DDX_Text(pDX, IDC_FLDCONFIRM_FROM_PATH, m_fromPath);
	DDX_Text(pDX, IDC_FLDCONFIRM_TO_PATH, m_toPath);
	DDX_Text(pDX, IDC_FLDCONFIRM_QUERY, m_question);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ConfirmFolderCopyDlg, CTrDialog)
	ON_BN_CLICKED(IDNO, OnBnClickedNo)
	ON_BN_CLICKED(IDYES, OnBnClickedYes)
END_MESSAGE_MAP()


// ConfirmFolderCopyDlg message handlers

/**
 * @brief Handler for WM_INITDIALOG; conventional location to initialize
 * controls. At this point dialog and control windows exist.
 */
BOOL ConfirmFolderCopyDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();

	// Load warning icon
	// TODO: we can have per-action icons?
	HICON icon = AfxGetApp()->LoadStandardIcon(IDI_EXCLAMATION);
	SendDlgItemMessage(IDC_FLDCONFIRM_ICON, STM_SETICON, (WPARAM)icon, 0L);

	if (!m_caption.empty())
		SetWindowText(m_caption.c_str());

	// setup handler for resizing this dialog	
	m_constraint.InitializeCurrentSize(this);
	m_constraint.DisallowHeightGrowth();
	m_constraint.SubclassWnd(); // install subclassing
	// persist size via registry
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("FolderCopyConfirmDlg"), false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Close dialog when No button is clicked.
 */
void ConfirmFolderCopyDlg::OnBnClickedNo()
{
	EndDialog(IDNO);
}

/**
 * @brief Close dialog when Yes button is clicked.
 */
void ConfirmFolderCopyDlg::OnBnClickedYes()
{
	EndDialog(IDYES);
}
