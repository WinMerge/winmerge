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
 * @file  PropCodepage.h
 *
 * @brief Implementation file for CPropCodepage propertyheet
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "merge.h"
#include "PropCodepage.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropCodepage property page

CPropCodepage::CPropCodepage(COptionsMgr *optionsMgr)
 : CPropertyPage(CPropCodepage::IDD)
, m_pOptionsMgr(optionsMgr)
, m_nCodepageSystem(-1)
, m_nCustomCodepageValue(0)
, m_bDetectCodepage(FALSE)
{
}

CPropCodepage::~CPropCodepage()
{
}

void CPropCodepage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropCodepage)
	DDX_Radio(pDX, IDC_CP_SYSTEM, m_nCodepageSystem);
	DDX_Text(pDX, IDC_CUSTOM_CP_NUMBER, m_nCustomCodepageValue);
	DDX_Check(pDX, IDC_DETECT_CODEPAGE, m_bDetectCodepage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropCodepage, CPropertyPage)
	//{{AFX_MSG_MAP(CPropCodepage)
	ON_BN_CLICKED(IDC_CP_SYSTEM, OnCpSystem)
	ON_BN_CLICKED(IDC_CP_CUSTOM, OnCpCustom)
	ON_BN_CLICKED(IDC_CP_UI, OnCpUi)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void CPropCodepage::ReadOptions()
{
	m_nCodepageSystem = m_pOptionsMgr->GetInt(OPT_CP_DEFAULT_MODE);
	m_nCustomCodepageValue = m_pOptionsMgr->GetInt(OPT_CP_DEFAULT_CUSTOM);
	m_bDetectCodepage = m_pOptionsMgr->GetBool(OPT_CP_DETECT);
}

/** 
 * @brief Writes options values from UI to storage.
 */
void CPropCodepage::WriteOptions()
{
	m_pOptionsMgr->SaveOption(OPT_CP_DEFAULT_MODE, (int)m_nCodepageSystem);
	m_pOptionsMgr->SaveOption(OPT_CP_DEFAULT_CUSTOM, (int)m_nCustomCodepageValue);
	m_pOptionsMgr->SaveOption(OPT_CP_DETECT, m_bDetectCodepage == TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CPropCodepage message handlers

BOOL CPropCodepage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CEdit * pEdit = (CEdit *) GetDlgItem(IDC_CUSTOM_CP_NUMBER);

	// Enable/disable "Custom codepage" edit field
	if (IsDlgButtonChecked(IDC_CP_CUSTOM))
		pEdit->EnableWindow(TRUE);
	else
		pEdit->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropCodepage::OnCpSystem() 
{
	GetDlgItem(IDC_CUSTOM_CP_NUMBER)->EnableWindow(FALSE);	
}

void CPropCodepage::OnCpCustom() 
{
	GetDlgItem(IDC_CUSTOM_CP_NUMBER)->EnableWindow(TRUE);	
}

void CPropCodepage::OnCpUi() 
{
	GetDlgItem(IDC_CUSTOM_CP_NUMBER)->EnableWindow(FALSE);	
}
