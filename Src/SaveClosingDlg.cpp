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

#include "stdafx.h"
#include "SaveClosingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg dialog

IMPLEMENT_DYNAMIC(SaveClosingDlg, CTrDialog)

/**
 * @brief Constructor.
 * @param [in] pParent Dialog's parent window.
 */
SaveClosingDlg::SaveClosingDlg(CWnd* pParent /*= nullptr*/)
 : CTrDialog(SaveClosingDlg::IDD, pParent)
 , m_leftSave(SAVECLOSING_SAVE)
 , m_middleSave(SAVECLOSING_SAVE)
 , m_rightSave(SAVECLOSING_SAVE)
 , m_bAskForLeft(false)
 , m_bAskForMiddle(false)
 , m_bAskForRight(false)
 , m_bDisableCancel(false)
{
}

void SaveClosingDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SaveClosingDlg)
	DDX_Text(pDX, IDC_SAVECLOSING_LEFTFILE, m_sLeftFile);
	DDX_Text(pDX, IDC_SAVECLOSING_MIDDLEFILE, m_sMiddleFile);
	DDX_Text(pDX, IDC_SAVECLOSING_RIGHTFILE, m_sRightFile);
	DDX_Radio(pDX, IDC_SAVECLOSING_SAVELEFT, m_leftSave);
	DDX_Radio(pDX, IDC_SAVECLOSING_SAVEMIDDLE, m_middleSave);
	DDX_Radio(pDX, IDC_SAVECLOSING_SAVERIGHT, m_rightSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SaveClosingDlg, CTrDialog)
	//{{AFX_MSG_MAP(SaveClosingDlg)
	ON_BN_CLICKED(IDC_SAVECLOSING_DISCARDALL, OnDiscardAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SaveClosingDlg message handlers

/**
 * @brief Initialize dialog.
 * @return Always FALSE.
 */
BOOL SaveClosingDlg::OnInitDialog() 
{
	CTrDialog::OnInitDialog();

	SetDlgItemFocus(IDC_SAVECLOSING_SAVELEFT);
	if (!m_bAskForLeft)
	{
		// Left items disabled move focus to middle side items
		SetDlgItemFocus(IDC_SAVECLOSING_SAVEMIDDLE);

		EnableDlgItem(IDC_SAVECLOSING_LEFTFRAME, false);
		EnableDlgItem(IDC_SAVECLOSING_LEFTFILE, false);
		EnableDlgItem(IDC_SAVECLOSING_SAVELEFT, false);
		EnableDlgItem(IDC_SAVECLOSING_DISCARDLEFT, false);
	}

	if (!m_bAskForMiddle)
	{
		// Middle items disabled move focus to right side items
		if (!m_bAskForLeft)
			SetDlgItemFocus(IDC_SAVECLOSING_SAVERIGHT);

		EnableDlgItem(IDC_SAVECLOSING_MIDDLEFRAME, false);
		EnableDlgItem(IDC_SAVECLOSING_MIDDLEFILE, false);
		EnableDlgItem(IDC_SAVECLOSING_SAVEMIDDLE, false);
		EnableDlgItem(IDC_SAVECLOSING_DISCARDMIDDLE, false);
	}
	
	if (!m_bAskForRight)
	{
		EnableDlgItem(IDC_SAVECLOSING_RIGHTFRAME, false);
		EnableDlgItem(IDC_SAVECLOSING_RIGHTFILE, false);
		EnableDlgItem(IDC_SAVECLOSING_SAVERIGHT, false);
		EnableDlgItem(IDC_SAVECLOSING_DISCARDRIGHT, false);
	}

	if (m_bDisableCancel)
	{
		EnableDlgItem(IDCANCEL, false);
	}

	// setup handler for resizing this dialog	
	m_constraint.InitializeCurrentSize(this);
	m_constraint.DisallowHeightGrowth();
	m_constraint.SubclassWnd(); // install subclassing
	m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("SaveClosingDlg"), false); // persist size via registry

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Select files to ask about saving.
 * @param [in] bLeft Do we ask about left-side file?
 * @param [in] bRight Do we ask about right-side file?
 */
void SaveClosingDlg::DoAskFor(bool bLeft /*= false*/, bool bMiddle /*= false*/, bool bRight /*= false*/)
{
	m_bAskForLeft = bLeft;
	m_bAskForMiddle = bMiddle;
	m_bAskForRight = bRight;
}

/** 
 * @brief Called when 'Discard All' button is selected.
 */
void SaveClosingDlg::OnDiscardAll()
{
	m_leftSave = SAVECLOSING_DISCARD;
	m_middleSave = SAVECLOSING_DISCARD;
	m_rightSave = SAVECLOSING_DISCARD;
	UpdateData(FALSE);
	OnOK();
}
