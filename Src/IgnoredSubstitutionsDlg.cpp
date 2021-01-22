/**
 *  @file IgnoredSubstitutionsFiltersDlg.cpp
 *
 *  @brief Implementation of Line Filter dialog
 */ 

#include "stdafx.h"
#include "TokenPairList.h"
#include "Merge.h"
#include "IgnoredSubstitutionsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/** @brief Location for file compare specific help to open. */
static TCHAR FilterHelpLocation[] = _T("::/htmlhelp/Filters.html");

/////////////////////////////////////////////////////////////////////////////
// CPropLineFilter property page

IMPLEMENT_DYNAMIC(IgnoredSubstitutionsDlg, CTrPropertyPage)

/**
 * @brief Constructor.
 */
IgnoredSubstitutionsDlg::IgnoredSubstitutionsDlg()
	: CTrPropertyPage(IgnoredSubstitutionsDlg::IDD)
	, m_pExternalRenameList(nullptr)
	, InPlaceEdit(nullptr)
{
	//{{AFX_DATA_INIT(IgnoredSubstitutionsFiltersDlg)
	m_IgnoredSubstitutionsAreEnabled = false;
	m_UseRegexpsForIgnoredSubstitutions = false;
	//}}AFX_DATA_INIT
	m_strCaption = theApp.LoadDialogCaption(m_lpszTemplateName).c_str();
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;
	m_psp.hIcon = AfxGetApp()->LoadIcon(IDI_LINEFILTER);
	m_psp.dwFlags |= PSP_USEHICON;
}

void IgnoredSubstitutionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(IgnoredSubstitutionsFiltersDlg)
	DDX_Check(pDX, IDC_IGNORED_SUSBSTITUTIONS_ARE_ENABLED, m_IgnoredSubstitutionsAreEnabled);
	DDX_Control(pDX, IDC_IGNORED_SUBSTITUTIONS_FILTER, m_VisibleFiltersList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(IgnoredSubstitutionsDlg, CTrPropertyPage)
	//{{AFX_MSG_MAP(IgnoredSubstitutionsFiltersDlg)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_BN_CLICKED(IDC_LFILTER_ADDBTN, OnBnClickedAddBtn)
	ON_BN_CLICKED(IDC_LFILTER_CLEARBTN, OnBnClickedClearBtn)
	ON_BN_CLICKED(IDC_LFILTER_REMOVEBTN, OnBnClickedRemovebtn)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_IGNORED_SUBSTITUTIONS_FILTER, OnEndLabelEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropLineFilter message handlers

/**
 * @brief Initialize the dialog.
 */
BOOL IgnoredSubstitutionsDlg::OnInitDialog()
{
	CTrPropertyPage::OnInitDialog();

	InitList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void IgnoredSubstitutionsDlg::InitList()
{
	m_VisibleFiltersList.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	//m_VisibleFiltersList.SetExtendedStyle(LVS_EX_INFOTIP | LVS_EX_GRIDLINES);

	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };

 	m_VisibleFiltersList.InsertColumn(0, _("Find what").c_str(), LVCFMT_LEFT, pointToPixel(112));
 	m_VisibleFiltersList.InsertColumn(1, _("Replace with").c_str(), LVCFMT_LEFT, pointToPixel(262));
 	m_VisibleFiltersList.InsertColumn(2, _("RegExp").c_str(), LVCFMT_LEFT, pointToPixel(72));
	m_VisibleFiltersList.SetBinaryValueColumn(2);

	if (m_pExternalRenameList)
	{
		for (int i = 0; i < (int)m_pExternalRenameList->GetCount(); i++)
		{
			const TokenPair& item = m_pExternalRenameList->GetAt(i);
			m_VisibleFiltersList.InsertItem(i, item.filterStr0.c_str());
			m_VisibleFiltersList.SetItemText(i, 1, item.filterStr1.c_str());
			m_VisibleFiltersList.SetItemText(i, 2, item.useRegExp ? _T("\u2611") : _T("\u2610"));
			m_VisibleFiltersList.SetCheck(i, item.enabled);
		}
	}
}

/**
 * @brief Open help from mainframe when user presses F1.
 */
void IgnoredSubstitutionsDlg::OnHelp()
{
	theApp.ShowHelp(FilterHelpLocation);
}

/**
 * @brief Called when Add-button is clicked.
 */
void IgnoredSubstitutionsDlg::OnBnClickedAddBtn()
{
	int num = m_VisibleFiltersList.GetItemCount();
	int ind = m_VisibleFiltersList.InsertItem(num, _("<Edit here>").c_str());
	m_VisibleFiltersList.SetItemText(num, 1, _("<Edit here>").c_str());

	if (ind >= -1)
	{
		m_VisibleFiltersList.SetItemState(ind, LVIS_SELECTED, LVIS_SELECTED);
		m_VisibleFiltersList.EnsureVisible(ind, FALSE);
		//EditSelectedFilter();
	}
}

/**
 * @brief Called when Clear-button is clicked.
 */
void IgnoredSubstitutionsDlg::OnBnClickedClearBtn()
{
	m_VisibleFiltersList.DeleteAllItems();
}

/**
 * @brief Save filters to list when exiting the dialog.
 */
void IgnoredSubstitutionsDlg::OnOK()
{
	m_pExternalRenameList->Empty();

	for (int i = 0; i < m_VisibleFiltersList.GetItemCount(); i++)
	{
		String symbolBeforeRename = m_VisibleFiltersList.GetItemText(i, 0);
		String symbolAfterRename = m_VisibleFiltersList.GetItemText(i, 1);
		bool useRegExp = m_VisibleFiltersList.GetItemText(i, 2).Compare(_T("\u2611")) == 0;
		bool enabled = !!m_VisibleFiltersList.GetCheck(i);
		if(symbolBeforeRename != _("<Edit here>") && symbolAfterRename != _("<Edit here>"))
			m_pExternalRenameList->AddFilter(symbolBeforeRename, symbolAfterRename, useRegExp, enabled);
	}

	CPropertyPage::OnClose();
	//CDialog::OnOK(); //?
}

/**
 * @brief Sets external filter list.
 * @param [in] list External filter list.
 */
void IgnoredSubstitutionsDlg::SetList(TokenPairList *list)
{
	m_pExternalRenameList = list;
}

/**
 * @brief Called when Remove button is clicked.
 */
void IgnoredSubstitutionsDlg::OnBnClickedRemovebtn()
{
	int sel = m_VisibleFiltersList.GetNextItem(-1, LVNI_SELECTED);
	if (sel != -1)
	{
		m_VisibleFiltersList.DeleteItem(sel);
	}

	int newSel = min(m_VisibleFiltersList.GetItemCount() - 1, sel);
	if (newSel >= -1)
	{
		m_VisibleFiltersList.SetItemState(newSel, LVIS_SELECTED, LVIS_SELECTED);
		bool bPartialOk = false;
		m_VisibleFiltersList.EnsureVisible(newSel, bPartialOk);
	}
}

/**
 * @brief Called when the user activates an item.
 */
// void IgnoredSubstitutionsFiltersDlg::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
// {
// 	EditSelectedFilter();
// 	*pResult = 0;
// }

/**
 * @brief Called when in-place editing has finished.
 */
void IgnoredSubstitutionsDlg::OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_VisibleFiltersList.OnEndLabelEdit(pNMHDR, pResult);
}
