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
#include "merge.h"
#include "PropVss.h"
#include "dirtools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropVss property page

IMPLEMENT_DYNCREATE(CPropVss, CPropertyPage)

CPropVss::CPropVss() : CPropertyPage(CPropVss::IDD)
{
	//{{AFX_DATA_INIT(CPropVss)
	m_strPath = _T("");
	m_nVerSys = -1;
	//}}AFX_DATA_INIT
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
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropVss, CPropertyPage)
	//{{AFX_MSG_MAP(CPropVss)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	ON_CBN_SELENDOK(IDC_VER_SYS, OnSelendokVerSys)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropVss message handlers

BOOL ChooseFile( CString& strResult, 
				 LPCTSTR szStartPath /* = NULL */, 
				 LPCTSTR szCaption /* = "Open" */, 
				 LPCTSTR szFilter /* = "All Files (*.*)|*.*||" */, 
				 BOOL bOpenDlg /* = TRUE */) 
// displays a shell file selector
{
	CFileDialog dlg(bOpenDlg, NULL, NULL, 
				    OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | (bOpenDlg? OFN_FILEMUSTEXIST:0) , szFilter);
	dlg.m_ofn.lpstrTitle = (LPTSTR)szCaption;
	dlg.m_ofn.lpstrInitialDir = (LPTSTR)szStartPath;
	if (dlg.DoModal()==IDOK)
	{
	 	strResult = dlg.GetPathName(); 
		return TRUE;
	}
	strResult = _T("");
	return FALSE;	   
}

void CPropVss::OnBrowseButton() 
{
	CString s;
	if (ChooseFile(s))
	{
		UpdateData(TRUE);
		m_strPath = s;
		UpdateData(FALSE);
	}
	
}

BOOL CPropVss::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	UpdateData(FALSE);
	OnSelendokVerSys();
	
	return TRUE;  
}

void CPropVss::OnSelendokVerSys() 
{
	UpdateData(TRUE);
	CString tempStr((LPCSTR)(m_nVerSys==3?IDS_CC_CMD:IDS_VSS_CMD));
	m_ctlVssL1.SetWindowText(tempStr);
	m_ctlPath.EnableWindow(m_nVerSys==1 || m_nVerSys==3);
	m_ctlVssL1.EnableWindow(m_nVerSys==1 || m_nVerSys==3);
	m_ctlBrowse.EnableWindow(m_nVerSys==1 || m_nVerSys==3);
}
