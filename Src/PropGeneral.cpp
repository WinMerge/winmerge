/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  PropGeneral.h
 *
 * @brief Implementation file for CPropGeneral propertyheet
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "PropGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropGeneral property page

IMPLEMENT_DYNCREATE(CPropGeneral, CPropertyPage)

/** 
 * @brief Constructor initialising members.
 */
CPropGeneral::CPropGeneral() : CPropertyPage(CPropGeneral::IDD)
{
	//{{AFX_DATA_INIT(CPropGeneral)
	m_bBackup = FALSE;
	m_bScroll = FALSE;
	m_bDisableSplash = FALSE;
	m_bAutoCloseCmpPane = FALSE;
	m_bCloseWindowWithEsc = TRUE;
	//}}AFX_DATA_INIT
}

CPropGeneral::~CPropGeneral()
{
}


BOOL CPropGeneral::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPropGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropGeneral)
	DDX_Check(pDX, IDC_BACKUP_CHECK, m_bBackup);
	DDX_Check(pDX, IDC_SCROLL_CHECK, m_bScroll);
	DDX_Check(pDX, IDC_DISABLE_SPLASH, m_bDisableSplash);
	DDX_Check(pDX, IDC_CMPPANE_AUTOCLOSE, m_bAutoCloseCmpPane);
	DDX_Check(pDX, IDC_ESC_CLOSES_WINDOW, m_bCloseWindowWithEsc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CPropGeneral)
	ON_BN_CLICKED(IDC_RESET_ALL_MESSAGE_BOXES, OnResetAllMessageBoxes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropGeneral message handlers

/** 
 * @brief Called when user wants to see all messageboxes again.
 */
void CPropGeneral::OnResetAllMessageBoxes()
{
	CMessageBoxDialog::ResetMessageBoxes();
	AfxMessageBox(IDS_MESSAGE_BOX_ARE_RESET, MB_ICONINFORMATION);
}
