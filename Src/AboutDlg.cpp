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

#include "stdafx.h"
#include "AboutDlg.h"
#include "TrDialogs.h"
#include "Bitmap.h"
#include "resource.h" // IDD_ABOUTBOX

/** 
 * @brief About-dialog class.
 * 
 * Shows About-dialog bitmap and draws version number and other
 * texts into it.
 */
class CAboutDlg::Impl : public CTrDialog
{
public:
	explicit Impl(CAboutDlg *p, CWnd* pParent = nullptr);

// Dialog Data
	//{{AFX_DATA(CAboutDlg::Impl)
	enum { IDD = IDD_ABOUTBOX };
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
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedWWW(NMHDR *pNMHDR, LRESULT *pResult);

private:
	CAboutDlg *m_p;
	ATL::CImage m_image;
	CFont m_font;
};

BEGIN_MESSAGE_MAP(CAboutDlg::Impl, CTrDialog)
	//{{AFX_MSG_MAP(CAboutDlg::Impl)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_OPEN_CONTRIBUTORS, OnBnClickedOpenContributors)
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CLICK, IDC_WWW, OnBnClickedWWW)
END_MESSAGE_MAP()

CAboutDlg::Impl::Impl(CAboutDlg *p, CWnd* pParent /*= nullptr*/)
	: CTrDialog(CAboutDlg::Impl::IDD)
	, m_p(p)
{
}

void CAboutDlg::Impl::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg::Impl)
	DDX_Text(pDX, IDC_COMPANY, m_p->m_info.copyright);
	DDX_Text(pDX, IDC_VERSION, m_p->m_info.version);
	//}}AFX_DATA_MAP
}

/** 
 * @brief Read version info from resource to dialog.
 */
BOOL CAboutDlg::Impl::OnInitDialog() 
{
	CTrDialog::OnInitDialog();

	LoadImageFromResource(m_image, MAKEINTRESOURCE(IDR_SPLASH), _T("IMAGE"));

	m_font.CreatePointFont(10 * 10, _T("Tahoma"));

	SetDlgItemText(static_cast<unsigned>(IDC_STATIC), m_p->m_info.developers);
	GetDlgItem(IDC_STATIC)->SetFont(&m_font);
	GetDlgItem(IDC_VERSION)->SetFont(&m_font);

	String link;
	GetDlgItemText(IDC_WWW, link);
	link = _T("<a href=\"") + m_p->m_info.website + _T("\">") + link + _T("</a>");
	SetDlgItemText(IDC_WWW, link);	

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CAboutDlg::Impl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_STATIC && pWnd != GetDlgItem(IDC_WWW))
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	return CTrDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CAboutDlg::Impl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc;
	GetDlgItem(nIDCtl)->GetClientRect(&rc);
	m_image.Draw(lpDrawItemStruct->hDC, rc, Gdiplus::InterpolationModeBicubic);
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

void CAboutDlg::Impl::OnBnClickedWWW(NMHDR *pNMHDR, LRESULT *pResult)
{
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	ShellExecute(nullptr, _T("open"), pNMLink->item.szUrl, nullptr, nullptr, SW_SHOWNORMAL);
}

CAboutDlg::CAboutDlg() : m_pimpl(new CAboutDlg::Impl(this)) {}
CAboutDlg::~CAboutDlg() {}
int CAboutDlg::DoModal() { return static_cast<int>(m_pimpl->DoModal()); }
