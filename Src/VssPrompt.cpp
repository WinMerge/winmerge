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
// VssPrompt.cpp : implementation file
//

#include "stdafx.h"
#include "merge.h"
#include "VssPrompt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVssPrompt dialog


CVssPrompt::CVssPrompt(CWnd* pParent /*=NULL*/)
	: CDialog(CVssPrompt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVssPrompt)
	m_strProject = _T("");
	m_strUser = _T("");
	m_strPassword = _T("");
	m_strMessage = _T("");
	//}}AFX_DATA_INIT
}


void CVssPrompt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVssPrompt)
	DDX_Control(pDX, IDC_PROJECT_COMBO, m_ctlProject);
	DDX_CBString(pDX, IDC_PROJECT_COMBO, m_strProject);
	DDX_Text(pDX, IDC_USER, m_strUser);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVssPrompt, CDialog)
	//{{AFX_MSG_MAP(CVssPrompt)
	ON_BN_CLICKED(IDSAVEAS, OnSaveas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVssPrompt message handlers

BOOL CVssPrompt::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ctlProject.LoadState(_T("Vss"));
	
	return TRUE;  
}

void CVssPrompt::OnOK() 
{
	UpdateData(TRUE);
	if (m_strProject.IsEmpty())
	{
		AfxMessageBox(IDS_NOPROJECT,MB_ICONSTOP);
		m_ctlProject.SetFocus();
		return;
	}

	m_ctlProject.SaveState(_T("Vss"));
	
	CDialog::OnOK();
}

void CVssPrompt::OnSaveas() 
{
	EndDialog(IDSAVEAS);
}
