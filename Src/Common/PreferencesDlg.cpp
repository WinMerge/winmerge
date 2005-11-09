/** 
 * @file PreferencesDlg.cpp
 *
 * @brief Implementation file for CPreferencesDlg
 *
 * @note This code originates from AbstractSpoon / TodoList
 * (http://www.abstractspoon.com/) but is modified to use in
 * WinMerge.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "resource.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "SyntaxColors.h"
#include "PreferencesDlg.h"
#include "MainFrm.h"

#include "winclasses.h"
#include "wclassdefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog

const TCHAR PATHDELIM = '>';

CPreferencesDlg::CPreferencesDlg(COptionsMgr *regOptions, SyntaxColors *colors,
		UINT nMenuID, CWnd* pParent)   // standard constructor
	: CDialog(IDD_PREFERENCES, pParent), m_pOptionsMgr(regOptions),
	m_pageGeneral(regOptions), m_pageCompare(regOptions),
	m_pageColors(regOptions), m_pSyntaxColors(colors),
	m_pageSyntaxColors(colors), m_pageArchive(regOptions),
	m_pageCodepage(regOptions), m_pageEditor(regOptions),
	m_pageSystem(regOptions), m_pageVss(regOptions)
{
	UNREFERENCED_PARAMETER(nMenuID);
}

CPreferencesDlg::~CPreferencesDlg()
{
}

void CPreferencesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesDlg)
	DDX_Control(pDX, IDC_TREEOPT_PAGES, m_tcPages);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPreferencesDlg, CDialog)
	//{{AFX_MSG_MAP(CPreferencesDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TREEOPT_HELP, OnHelpButton)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREEOPT_PAGES, OnSelchangedPages)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg message handlers

BOOL CPreferencesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_tcPages.SetIndent(0);

	// Second parameter is 'path', page's parent page(s) and caption.
	// '>' is used as path separator.
	// For example "General" creates top-level "General" page
	// and "General>Colors" creates "Colors" sub-page for "General"
	AddPage(&m_pageGeneral, IDS_OPTIONSPG_GENERAL);
	AddPage(&m_pageCompare, IDS_OPTIONSPG_COMPARE);
	AddPage(&m_pageEditor, IDS_OPTIONSPG_EDITOR);
	AddPage(&m_pageColors, IDS_OPTIONSPG_COLORS);
	AddPage(&m_pageSyntaxColors, IDS_OPTIONSPG_SYNTAXCOLORS);
	AddPage(&m_pageArchive, IDS_OPTIONSPG_ARCHIVE);
	AddPage(&m_pageSystem, IDS_OPTIONSPG_SYSTEM);
	AddPage(&m_pageVss, IDS_OPTIONSPG_VERSIONCONTROL);
	AddPage(&m_pageCodepage, IDS_OPTIONSPG_CODEPAGE);

	ReadOptions();
	
	CRect rPPHost;
	GetDlgItem(IDC_TREEOPT_HOSTFRAME)->GetWindowRect(rPPHost);
	ScreenToClient(rPPHost);

	if (m_pphost.Create(rPPHost, this))
		SetActivePage(AfxGetApp()->GetProfileInt(_T("Settings"), _T("OptStartPage"), 0));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesDlg::OnOK()
{
	CDialog::OnOK();
	m_pphost.OnOK();

	SaveOptions();
}

void CPreferencesDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("OptStartPage"), m_pphost.GetActiveIndex());
}

void CPreferencesDlg::OnHelpButton() 
{
	CMainFrame *pMf = dynamic_cast<CMainFrame*>(AfxGetMainWnd());
	pMf->ShowHelp();
}

void CPreferencesDlg::AddPage(CPropertyPage* pPage, UINT nResourceID)
{
	CString sPath;
	VERIFY(sPath.LoadString(nResourceID));
	AddPage(pPage, sPath);
}

void CPreferencesDlg::AddPage(CPropertyPage* pPage, LPCTSTR szPath)
{
	CString sPath(szPath);

	if (m_pphost.AddPage(pPage))
	{
		HTREEITEM htiParent = TVI_ROOT; // default
		int nFind = sPath.Find(PATHDELIM);

		while (nFind != -1)
		{
			CString sParent = sPath.Left(nFind);
			sPath = sPath.Mid(nFind + 1);

			// see if parent already exists
			HTREEITEM htiParentParent = htiParent;
			htiParent = m_tcPages.GetChildItem(htiParentParent);

			while (htiParent)
			{
				if (sParent.CompareNoCase(m_tcPages.GetItemText(htiParent)) == 0)
					break;

				htiParent = m_tcPages.GetNextItem(htiParent, TVGN_NEXT);
			}

			if (!htiParent)
				htiParent = m_tcPages.InsertItem(sParent, htiParentParent);

			nFind = sPath.Find(PATHDELIM);
		}

		HTREEITEM hti = m_tcPages.InsertItem(sPath, htiParent); // whatever's left
		m_tcPages.EnsureVisible(hti);

		// map both ways
		m_tcPages.SetItemData(hti, (DWORD)pPage);
		m_mapPP2HTI[(void*)pPage] = (void*)hti;
	}
}

void CPreferencesDlg::OnSelchangedPages(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UNREFERENCED_PARAMETER(pNMHDR);
	HTREEITEM htiSel = m_tcPages.GetSelectedItem();

	while (m_tcPages.ItemHasChildren(htiSel))
		htiSel = m_tcPages.GetChildItem(htiSel);

	CPropertyPage* pPage = (CPropertyPage*)m_tcPages.GetItemData(htiSel);
	ASSERT (pPage);

	if (pPage)
	{
		m_pphost.SetActivePage(pPage, FALSE);

		// update caption
		CString sCaption;
		AfxFormatString1(sCaption, IDS_OPTIONS_TITLE, GetItemPath(htiSel));
		SetWindowText(sCaption);
	}

	m_tcPages.SetFocus();
	
	*pResult = 0;
}

void CPreferencesDlg::SetActivePage(int nPage)
{
	m_pphost.SetActivePage(nPage, FALSE);

	// synchronize tree
	CPropertyPage* pPage = m_pphost.GetActivePage();
	HTREEITEM hti = NULL;

	if (m_mapPP2HTI.Lookup(pPage, (void*&)hti) && hti)
		m_tcPages.SelectItem(hti);
}

CString CPreferencesDlg::GetItemPath(HTREEITEM hti)
{
	CString sPath = m_tcPages.GetItemText(hti);

	while (hti = m_tcPages.GetParentItem(hti))
		sPath = m_tcPages.GetItemText(hti) + _T(" > ") + sPath;

	return sPath;
}

void CPreferencesDlg::ReadOptions()
{
	m_pageGeneral.ReadOptions();
	m_pageColors.ReadOptions();
	m_pageSystem.ReadOptions();
	m_pageCompare.ReadOptions();
	m_pageEditor.ReadOptions();
	m_pageCodepage.ReadOptions();
	m_pageVss.ReadOptions();
	m_pageArchive.ReadOptions();
}

void CPreferencesDlg::SaveOptions()
{
	m_pageGeneral.WriteOptions();
	m_pageSystem.WriteOptions();
	m_pageCompare.WriteOptions();
	m_pageEditor.WriteOptions();
	m_pageColors.WriteOptions();
	m_pageCodepage.WriteOptions();
	m_pageVss.WriteOptions();	

	m_pSyntaxColors->Clone(m_pageSyntaxColors.m_pTempColors);
	m_pSyntaxColors->SaveToRegistry();

	m_pageArchive.WriteOptions();
}

void CPreferencesDlg::SetSyntaxColors(SyntaxColors *pColors)
{
	m_pSyntaxColors = pColors;
}
