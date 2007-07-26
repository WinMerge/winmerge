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
// PropVss.cpp : implementation file
//

#include "stdafx.h"
#include "FileOrFolderSelect.h"
#include "MainFrm.h" // VCS_* constants
#include "PropVss.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropVss property page

CPropVss::CPropVss(COptionsMgr *optionsMgr) : CPropertyPage(CPropVss::IDD)
, m_pOptionsMgr(optionsMgr)
, m_nVerSys(-1)
{
}

CPropVss::~CPropVss()
{
}

void CPropVss::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropVss)
	DDX_Control(pDX, IDC_VSS_L1, m_ctlVssL1);
	DDX_Control(pDX, IDC_PATH_EDIT, m_ctlPath);
	DDX_Control(pDX, IDC_BROWSE_BUTTON, m_ctlBrowse);
	DDX_Text(pDX, IDC_PATH_EDIT, m_strPath);
	DDX_CBIndex(pDX, IDC_VER_SYS, m_nVerSys);
	DDX_Control(pDX, IDC_VER_SYS, m_ctlVerSys);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPropVss, CPropertyPage)
	//{{AFX_MSG_MAP(CPropVss)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	ON_CBN_SELENDOK(IDC_VER_SYS, OnSelendokVerSys)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void CPropVss::ReadOptions()
{
	m_nVerSys = m_pOptionsMgr->GetInt(OPT_VCS_SYSTEM);
	m_strPath = m_pOptionsMgr->GetString(OPT_VSS_PATH).c_str();
}

/** 
 * @brief Writes options values from UI to storage.
 */
void CPropVss::WriteOptions()
{
	m_pOptionsMgr->SaveOption(OPT_VCS_SYSTEM, (int)m_nVerSys);
	m_pOptionsMgr->SaveOption(OPT_VSS_PATH, m_strPath);
}

/////////////////////////////////////////////////////////////////////////////
// CPropVss message handlers

void CPropVss::OnBrowseButton() 
{
	CString s;
	if (SelectFile(GetSafeHwnd(), s))
	{
		m_strPath = s;
		m_ctlPath.SetWindowText(s);
	}
}

BOOL CPropVss::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	LoadVssOptionStrings();

	UpdateData(FALSE);
	OnSelendokVerSys();

	return TRUE;
}

void CPropVss::OnSelendokVerSys() 
{
	UpdateData(TRUE);
	CString tempStr((LPCTSTR)(m_nVerSys == VCS_CLEARCASE ? IDS_CC_CMD : IDS_VSS_CMD));
	m_ctlVssL1.SetWindowText(tempStr);
	m_ctlPath.EnableWindow(m_nVerSys == VCS_VSS4 || m_nVerSys == VCS_CLEARCASE);
	m_ctlVssL1.EnableWindow(m_nVerSys == VCS_VSS4 || m_nVerSys == VCS_CLEARCASE);
	m_ctlBrowse.EnableWindow(m_nVerSys == VCS_VSS4 || m_nVerSys == VCS_CLEARCASE);
}

/**
 * Load strings for supported source code control
 */
void CPropVss::LoadVssOptionStrings()
{
	/*
	Must be in order to agree with enum in MainFrm.h
	VCS_NONE = 0,
	VCS_VSS4,
	VCS_VSS5,
	VCS_CLEARCASE,
	*/

	m_ctlVerSys.AddString(LoadResString(IDS_VCS_NONE));
	m_ctlVerSys.AddString(LoadResString(IDS_VCS_VSS4));
	m_ctlVerSys.AddString(LoadResString(IDS_VCS_VSS5));
	m_ctlVerSys.AddString(LoadResString(IDS_VCS_CLEARCASE));
}
