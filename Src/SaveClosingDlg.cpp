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
 * @file  SaveClosingDlg.cpp
 *
 * @brief Implementation file for SaveClosingDlg dialog
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "SaveClosingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg dialog

IMPLEMENT_DYNAMIC(SaveClosingDlg, CDialog)

SaveClosingDlg::SaveClosingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SaveClosingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SaveClosingDlg)
	m_leftSave = SAVECLOSING_SAVE; // Default to save
	m_rightSave = SAVECLOSING_SAVE;
	m_bAskForLeft = FALSE;
	m_bAskForRight = FALSE;
	m_bDisableCancel = FALSE;
	//}}AFX_DATA_INIT
}

void SaveClosingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SaveClosingDlg)
	DDX_Text(pDX, IDC_SAVECLOSING_LEFTFILE, m_sLeftFile);
	DDX_Text(pDX, IDC_SAVECLOSING_RIGHTFILE, m_sRightFile);
	DDX_Radio(pDX, IDC_SAVECLOSING_SAVELEFT, m_leftSave);
	DDX_Radio(pDX, IDC_SAVECLOSING_SAVERIGHT, m_rightSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SaveClosingDlg, CDialog)
	//{{AFX_MSG_MAP(SaveClosingDlg)
	ON_BN_CLICKED(IDC_SAVECLOSING_DISCARDALL, OnDiscardAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg message handlers

BOOL SaveClosingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_SAVECLOSING_SAVELEFT)->SetFocus();
	if (!m_bAskForLeft)
	{
		// Left items disabled move focus to right side items
		GetDlgItem(IDC_SAVECLOSING_SAVERIGHT)->SetFocus();

		GetDlgItem(IDC_SAVECLOSING_LEFTFRAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAVECLOSING_LEFTFILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAVECLOSING_SAVELEFT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAVECLOSING_DISCARDLEFT)->EnableWindow(FALSE);
	}

	if (!m_bAskForRight)
	{
		GetDlgItem(IDC_SAVECLOSING_RIGHTFRAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAVECLOSING_RIGHTFILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAVECLOSING_SAVERIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAVECLOSING_DISCARDRIGHT)->EnableWindow(FALSE);
	}

	if (m_bDisableCancel)
	{
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	}

	// setup handler for resizing this dialog	
	m_constraint.InitializeCurrentSize(this);
	// configure how individual controls adjust when dialog resizes
	m_constraint.ConstrainItem(IDC_SAVECLOSING_LEFTFRAME, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SAVECLOSING_LEFTFILE, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SAVECLOSING_SAVELEFT, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SAVECLOSING_DISCARDLEFT, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SAVECLOSING_RIGHTFRAME, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SAVECLOSING_RIGHTFILE, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SAVECLOSING_SAVERIGHT, 0, 1, 0, 0); // grows right
	m_constraint.ConstrainItem(IDC_SAVECLOSING_DISCARDRIGHT, 0, 1, 0, 0); // grows right
	// IDC_SAVECLOSING_DISCARDALL doesn't move
	m_constraint.ConstrainItem(IDOK, 1, 0, 0, 0); // slides right
	m_constraint.ConstrainItem(IDCANCEL, 1, 0, 0, 0); // slides right
	m_constraint.DisallowHeightGrowth();
	m_constraint.SubclassWnd(); // install subclassing
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("SaveClosingDlg"), false); // persist size via registry


	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Select files to ask about saving.
 */
void SaveClosingDlg::DoAskFor(BOOL bLeft /*= FALSE*/, BOOL bRight /*= FALSE*/)
{
	m_bAskForLeft = bLeft;
	m_bAskForRight = bRight;
}

/** 
 * @brief Called when 'Discard All' button is selected.
 */
void SaveClosingDlg::OnDiscardAll()
{
	m_leftSave = SAVECLOSING_DISCARD;
	m_rightSave = SAVECLOSING_DISCARD;
	UpdateData(FALSE);
	OnOK();
}
