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
 * @file  CCPromptDlg.cpp
 *
 * @brief Implementation file for ClearCase dialog
 */

#include "stdafx.h"
#include "CCPromptDlg.h"
#include "Merge.h"
#include "DDXHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief A dialog for ClearCase checkout/checkin.
 */
class CCCPromptDlg::Impl : public CDialog
{
// Construction
public:
	CCCPromptDlg::Impl(CCCPromptDlg *p, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCCPromptDlg::Impl)
	enum { IDD = IDD_CLEARCASE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCCPromptDlg::Impl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CCCPromptDlg::Impl)
	afx_msg void OnSaveas();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CCCPromptDlg *m_p;
};

CCCPromptDlg::Impl::Impl(CCCPromptDlg *p, CWnd* pParent /*=NULL*/)
	: CDialog(CCCPromptDlg::Impl::IDD, pParent)
	, m_p(p)
{
}

void CCCPromptDlg::Impl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCCPromptDlg::Impl)
	DDX_Text(pDX, IDC_COMMENTS, m_p->m_comments);
	DDX_Check(pDX, IDC_MULTI_CHECKOUT, m_p->m_bMultiCheckouts);
	DDX_Check(pDX, IDC_CHECKIN, m_p->m_bCheckin);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCCPromptDlg::Impl, CDialog)
	//{{AFX_MSG_MAP(CCCPromptDlg::Impl)
	ON_BN_CLICKED(IDC_SAVE_AS, OnSaveas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/**
 * @brief Handler for WM_INITDIALOG; conventional location to initialize
 * controls. At this point dialog and control windows exist.
 */
BOOL CCCPromptDlg::Impl::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCCPromptDlg::Impl::OnSaveas() 
{
	EndDialog(IDC_SAVE_AS);
}

CCCPromptDlg::CCCPromptDlg()
	: m_pimpl(new CCCPromptDlg::Impl(this))
	, m_bMultiCheckouts(false)
	, m_bCheckin(false)
{}
CCCPromptDlg::~CCCPromptDlg() {}
int CCCPromptDlg::DoModal() { return static_cast<int>(m_pimpl->DoModal()); }

