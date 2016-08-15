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
 * @file  VssPromptDlg.cpp
 *
 * @brief Code for CVssPromptDlg::Impl class
 */

#include "stdafx.h"
#include "VssPromptDlg.h"
#include "SuperComboBox.h"
#include "TrDialogs.h"
#include "RegKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Class for VSS dialog
 */
class CVssPromptDlg::Impl : public CTrDialog
{
// Construction
public:
	Impl(CVssPromptDlg *p, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVssPromptDlg::Impl)
	enum { IDD = IDD_VSS };
	CSuperComboBox	m_ctlProject;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVssPromptDlg::Impl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVssPromptDlg::Impl)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSaveas();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ctlDBCombo;

private:
	CVssPromptDlg *m_p;
};

/////////////////////////////////////////////////////////////////////////////
// CVssPromptDlg::Impl dialog


/**
 * @brief Default constructor.
 * @param [in] pParent Pointer to parent component.
 */
CVssPromptDlg::Impl::Impl(CVssPromptDlg *p, CWnd* pParent /*=NULL*/)
	: CTrDialog(CVssPromptDlg::Impl::IDD, pParent)
	, m_p(p)

{
}


void CVssPromptDlg::Impl::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVssPromptDlg::Impl)
	DDX_Control(pDX, IDC_PROJECT_COMBO, m_ctlProject);
	DDX_CBString(pDX, IDC_PROJECT_COMBO, m_p->m_strProject);
	DDX_Text(pDX, IDC_USER, m_p->m_strUser);
	DDX_Text(pDX, IDC_PASSWORD, m_p->m_strPassword);
	DDX_Text(pDX, IDC_MESSAGE, m_p->m_strMessage);
	//}}AFX_DATA_MAP
	DDX_CBString(pDX, IDC_DATABASE_LIST, m_p->m_strSelectedDatabase);
	DDX_Control(pDX, IDC_DATABASE_LIST, m_ctlDBCombo);
	DDX_Check(pDX, IDC_MULTI_CHECKOUT, m_p->m_bMultiCheckouts);
	DDX_Check(pDX, IDC_VCPROJ_SYNC, m_p->m_bVCProjSync);
}


BEGIN_MESSAGE_MAP(CVssPromptDlg::Impl, CTrDialog)
	//{{AFX_MSG_MAP(CVssPromptDlg::Impl)
	ON_BN_CLICKED(IDC_SAVE_AS, OnSaveas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVssPromptDlg::Impl message handlers

/**
 * @brief Initialize the dialog.
 * @return TRUE, unless focus is modified.
 */
BOOL CVssPromptDlg::Impl::OnInitDialog()
{
	CTrDialog::OnInitDialog();

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
		if (!reg.QueryRegUser(_T("SOFTWARE\\Microsoft\\SourceSafe\\Databases")))
		{
			String msg = _("No VSS database(s) found!");
			AfxMessageBox(msg.c_str(), MB_ICONERROR);
			return FALSE;
		}
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
void CVssPromptDlg::Impl::OnOK()
{
	UpdateData(TRUE);
	if (m_p->m_strProject.empty())
	{
		LangMessageBox(IDS_NOPROJECT,MB_ICONSTOP);
		m_ctlProject.SetFocus();
		return;
	}

	m_ctlProject.SaveState(_T("Vss"));
	CTrDialog::OnOK();
}

/**
 * @brief Close dialog with Save As -button.
 */
void CVssPromptDlg::Impl::OnSaveas()
{
	EndDialog(IDC_SAVE_AS);
}


CVssPromptDlg::CVssPromptDlg()
	: m_pimpl(new CVssPromptDlg::Impl(this))
	, m_strSelectedDatabase(_T(""))
	, m_bMultiCheckouts(false)
	, m_bVCProjSync(false)
{}
CVssPromptDlg::~CVssPromptDlg() {}
int CVssPromptDlg::DoModal() { return static_cast<int>(m_pimpl->DoModal()); }
