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
 * @file  AboutDlg.cpp
 *
 * @brief Implementation of the About-dialog.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: AboutDlg.cpp 6380 2009-01-25 22:13:36Z kimmov $

#include "stdafx.h"
#include "AboutDlg.h"
#include "statlink.h"
#include "Merge.h"
#include "DDXHelper.h"
#include "resource.h" // IDD_ABOUTBOX

/** 
 * @brief About-dialog class.
 * 
 * Shows About-dialog bitmap and draws version number and other
 * texts into it.
 */
class CAboutDlg::Impl : public CDialog
{
public:
	CAboutDlg::Impl(CAboutDlg *p, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CAboutDlg::Impl)
	enum { IDD = IDD_ABOUTBOX };
	CStaticLink	m_ctlWWW;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg::Impl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg::Impl)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenContributors();

private:
	CAboutDlg *m_p;
};

BEGIN_MESSAGE_MAP(CAboutDlg::Impl, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg::Impl)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_OPEN_CONTRIBUTORS, OnBnClickedOpenContributors)
END_MESSAGE_MAP()

CAboutDlg::Impl::Impl(CAboutDlg *p, CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::Impl::IDD)
	, m_p(p)
{
}

void CAboutDlg::Impl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg::Impl)
	DDX_Text(pDX, IDC_COMPANY, m_p->m_info.copyright);
	DDX_Control(pDX, IDC_WWW, m_ctlWWW);
	DDX_Text(pDX, IDC_VERSION, m_p->m_info.version);
	DDX_Text(pDX, IDC_PRIVATEBUILD, m_p->m_info.private_build);
	//}}AFX_DATA_MAP
}

/** 
 * @brief Read version info from resource to dialog.
 */
BOOL CAboutDlg::Impl::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	// Load application icon
	HICON icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	if (icon != NULL) {
		CStatic * pIcon = (CStatic *) GetDlgItem(IDC_ABOUTBOX_ICON);
		pIcon->SetIcon(icon);
	}

	m_ctlWWW.m_link = m_p->m_info.website.c_str();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Show contributors list.
 * Opens Contributors.txt into notepad.
 */
void CAboutDlg::Impl::OnBnClickedOpenContributors()
{
	int tmp = 0;
	m_p->m_onclick_contributers.notify(m_p, tmp);
}

CAboutDlg::CAboutDlg() : m_pimpl(new CAboutDlg::Impl(this)) {}
CAboutDlg::~CAboutDlg() {}
int CAboutDlg::DoModal() { return static_cast<int>(m_pimpl->DoModal()); }
