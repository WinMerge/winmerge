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
 * @file  VssPrompt.cpp
 *
 * @brief Code for CVssPrompt class
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "Merge.h"
#include "VssPrompt.h"
#include "RegKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVssPrompt dialog


/**
 * @brief Default constructor.
 * @param [in] pParent Pointer to parent component.
 */
CVssPrompt::CVssPrompt(CWnd* pParent /*=NULL*/)
	: CDialog(CVssPrompt::IDD, pParent)
	, m_strSelectedDatabase(_T(""))
	, m_bMultiCheckouts(FALSE)
	, m_bVCProjSync(FALSE)
{
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
	DDX_CBString(pDX, IDC_DATABASE_LIST, m_strSelectedDatabase);
	DDX_Control(pDX, IDC_DATABASE_LIST, m_ctlDBCombo);
	DDX_Check(pDX, IDC_MULTI_CHECKOUT, m_bMultiCheckouts);
	DDX_Control(pDX, IDC_MULTI_CHECKOUT, m_ctlMultiCheckouts);
	DDX_Check(pDX, IDC_VCPROJ_SYNC, m_bVCProjSync);
}


BEGIN_MESSAGE_MAP(CVssPrompt, CDialog)
	//{{AFX_MSG_MAP(CVssPrompt)
	ON_BN_CLICKED(IDC_SAVE_AS, OnSaveas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVssPrompt message handlers

/**
 * @brief Initialize the dialog.
 * @return TRUE, unless focus is modified.
 */
BOOL CVssPrompt::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	m_ctlProject.LoadState(_T("Vss"));

	int i = 0;
	int j = 0;
	TCHAR cName[MAX_PATH] = {0};
	TCHAR cString[MAX_PATH] = {0};
	DWORD cssize = MAX_PATH;
	DWORD csize = MAX_PATH;
	CRegKeyEx reg;

	// Open key containing VSS databases
	if (!reg.QueryRegMachine(_T("SOFTWARE\\Microsoft\\SourceSafe\\Databases")))
	{
		CString msg;
		VERIFY(msg.LoadString(IDS_VSS_NODATABASES));
		AfxMessageBox(msg, MB_ICONERROR);
		return FALSE;
	}

	HKEY hreg = reg.GetKey();
	LONG retval = ERROR_SUCCESS;
	while (retval == ERROR_SUCCESS || retval == ERROR_MORE_DATA)
	{
		if (_tcslen(cString) > 0 && _tcslen(cName) > 0)
		{
			m_ctlDBCombo.InsertString(j, (LPCTSTR)cString);
			j++;
		}
		retval = RegEnumValue(hreg, i, (LPTSTR)&cName, &csize, NULL, 
				NULL, (LPBYTE)&cString, &cssize);
		cssize = MAX_PATH;
		csize = MAX_PATH;
		i++;
	}
	return TRUE;
}

/**
 * @brief Close dialog with OK-button.
 */
void CVssPrompt::OnOK()
{
	UpdateData(TRUE);
	if (m_strProject.IsEmpty())
	{
		LangMessageBox(IDS_NOPROJECT,MB_ICONSTOP);
		m_ctlProject.SetFocus();
		return;
	}

	m_ctlProject.SaveState(_T("Vss"));
	CDialog::OnOK();
}

/**
 * @brief Close dialog with Save As -button.
 */
void CVssPrompt::OnSaveas()
{
	EndDialog(IDC_SAVE_AS);
}
