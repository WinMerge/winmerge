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
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropGeneral property page

/** 
 * @brief Constructor initialising members.
 */
CPropGeneral::CPropGeneral(COptionsMgr *optionsMgr) : CPropertyPage(CPropGeneral::IDD)
, m_pOptionsMgr(optionsMgr)
, m_bBackup(FALSE)
, m_bScroll(FALSE)
, m_bDisableSplash(FALSE)
, m_bSingleInstance(FALSE)
, m_bVerifyPaths(FALSE)
, m_bCloseWindowWithEsc(TRUE)
, m_bMultipleFileCmp(FALSE)
, m_bMultipleDirCmp(FALSE)
, m_nAutoCompleteSource(0)
{
}

CPropGeneral::~CPropGeneral()
{
}

BOOL CPropGeneral::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	CComboBox *pWnd = (CComboBox*)GetDlgItem(IDC_AUTO_COMPLETE_SOURCE);
	ASSERT(NULL != pWnd);

	CString str;
	VERIFY(str.LoadString(IDS_AUTOCOMPLETE_DISABLED));
	pWnd->AddString(str);
	VERIFY(str.LoadString(IDS_AUTOCOMPLETE_FILE_SYS));
	pWnd->AddString(str);
	VERIFY(str.LoadString(IDS_AUTOCOMPLETE_MRU));
	pWnd->AddString(str);

	pWnd->SetCurSel(m_nAutoCompleteSource);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPropGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropGeneral)
	DDX_Check(pDX, IDC_BACKUP_CHECK, m_bBackup);
	DDX_Check(pDX, IDC_SCROLL_CHECK, m_bScroll);
	DDX_Check(pDX, IDC_DISABLE_SPLASH, m_bDisableSplash);
	DDX_Check(pDX, IDC_SINGLE_INSTANCE, m_bSingleInstance);
	DDX_Check(pDX, IDC_VERIFY_OPEN_PATHS, m_bVerifyPaths);
	DDX_Check(pDX, IDC_ESC_CLOSES_WINDOW, m_bCloseWindowWithEsc);
	DDX_Check(pDX, IDC_MULTIDOC_FILECMP, m_bMultipleFileCmp);
	DDX_Check(pDX, IDC_MULTIDOC_DIRCMP, m_bMultipleDirCmp);
	DDX_CBIndex(pDX, IDC_AUTO_COMPLETE_SOURCE, m_nAutoCompleteSource);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CPropGeneral)
	ON_BN_CLICKED(IDC_RESET_ALL_MESSAGE_BOXES, OnResetAllMessageBoxes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void CPropGeneral::ReadOptions()
{
	m_bBackup = m_pOptionsMgr->GetBool(OPT_CREATE_BACKUPS);
	m_bScroll = m_pOptionsMgr->GetBool(OPT_SCROLL_TO_FIRST);
	m_bDisableSplash = m_pOptionsMgr->GetBool(OPT_DISABLE_SPLASH);
	m_bSingleInstance = m_pOptionsMgr->GetBool(OPT_SINGLE_INSTANCE);
	m_bVerifyPaths = m_pOptionsMgr->GetBool(OPT_VERIFY_OPEN_PATHS);
	m_bCloseWindowWithEsc = m_pOptionsMgr->GetBool(OPT_CLOSE_WITH_ESC);
	m_bMultipleFileCmp = m_pOptionsMgr->GetBool(OPT_MULTIDOC_MERGEDOCS);
	m_bMultipleDirCmp = m_pOptionsMgr->GetBool(OPT_MULTIDOC_DIRDOCS);
	m_nAutoCompleteSource = m_pOptionsMgr->GetInt(OPT_AUTO_COMPLETE_SOURCE);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void CPropGeneral::WriteOptions()
{
	m_pOptionsMgr->SaveOption(OPT_CREATE_BACKUPS, m_bBackup == TRUE);
	m_pOptionsMgr->SaveOption(OPT_SCROLL_TO_FIRST, m_bScroll == TRUE);
	m_pOptionsMgr->SaveOption(OPT_DISABLE_SPLASH, m_bDisableSplash == TRUE);
	m_pOptionsMgr->SaveOption(OPT_SINGLE_INSTANCE, m_bSingleInstance == TRUE);
	m_pOptionsMgr->SaveOption(OPT_VERIFY_OPEN_PATHS, m_bVerifyPaths == TRUE);
	m_pOptionsMgr->SaveOption(OPT_CLOSE_WITH_ESC, m_bCloseWindowWithEsc == TRUE);
	m_pOptionsMgr->SaveOption(OPT_MULTIDOC_MERGEDOCS, m_bMultipleFileCmp == TRUE);
	m_pOptionsMgr->SaveOption(OPT_MULTIDOC_DIRDOCS, m_bMultipleDirCmp == TRUE);
	m_pOptionsMgr->SaveOption(OPT_AUTO_COMPLETE_SOURCE, m_nAutoCompleteSource);
}

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
