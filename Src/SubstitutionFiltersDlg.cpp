/**
 *  @file SubstitutionFiltersDlg.cpp
 *
 *  @brief Implementation of Substitution Filters dialog
 */ 

#include "stdafx.h"
#include "SubstitutionFiltersList.h"
#include "Merge.h"
#include "SubstitutionFiltersDlg.h"
#include "Constants.h"
#include <Poco/Exception.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropLineFilter property page

IMPLEMENT_DYNAMIC(SubstitutionFiltersDlg, CTrPropertyPage)

/**
 * @brief Constructor.
 */
SubstitutionFiltersDlg::SubstitutionFiltersDlg()
	: CTrPropertyPage(SubstitutionFiltersDlg::IDD)
	, m_pSubstitutionFiltersList(nullptr)
{
	//{{AFX_DATA_INIT(SubstitutionFiltersFiltersDlg)
	m_bEnabled = false;
	//}}AFX_DATA_INIT
	m_strCaption = theApp.LoadDialogCaption(m_lpszTemplateName).c_str();
	m_psp.pszTitle = m_strCaption;
	m_psp.dwFlags |= PSP_USETITLE;
	m_psp.hIcon = AfxGetApp()->LoadIcon(IDI_SUBSTITUTIONFILTER);
	m_psp.dwFlags |= PSP_USEHICON;
}

void SubstitutionFiltersDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SubstitutionFiltersFiltersDlg)
	DDX_Check(pDX, IDC_IGNORED_SUSBSTITUTIONS_ENABLED, m_bEnabled);
	DDX_Control(pDX, IDC_SUBSTITUTION_FILTERS, m_listFilters);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SubstitutionFiltersDlg, CTrPropertyPage)
	//{{AFX_MSG_MAP(SubstitutionFiltersFiltersDlg)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_BN_CLICKED(IDC_LFILTER_ADDBTN, OnBnClickedAddBtn)
	ON_BN_CLICKED(IDC_LFILTER_CLEARBTN, OnBnClickedClearBtn)
	ON_BN_CLICKED(IDC_LFILTER_REMOVEBTN, OnBnClickedRemovebtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropLineFilter message handlers

/**
 * @brief Initialize the dialog.
 */
BOOL SubstitutionFiltersDlg::OnInitDialog()
{
	m_bEnabled = m_pSubstitutionFiltersList->GetEnabled();
	CTrPropertyPage::OnInitDialog();

	InitList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

static CString RemoveMnemonic(const String& text)
{
	Poco::RegularExpression re("\\(&.\\)|&|:");
	std::string textu8 = ucr::toUTF8(text);
	re.subst(textu8, "", Poco::RegularExpression::RE_GLOBAL);
	return ucr::toTString(textu8).c_str();
}

void SubstitutionFiltersDlg::InitList()
{
	m_listFilters.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	const int lpx = CClientDC(this).GetDeviceCaps(LOGPIXELSX);
	auto pointToPixel = [lpx](int point) { return MulDiv(point, lpx, 72); };

 	m_listFilters.InsertColumn(0, RemoveMnemonic(_("Fi&nd what:")), LVCFMT_LEFT, pointToPixel(150));
 	m_listFilters.InsertColumn(1, RemoveMnemonic(_("Re&place with:")), LVCFMT_LEFT, pointToPixel(150));
 	m_listFilters.InsertColumn(2, RemoveMnemonic(_("Regular &expression")), LVCFMT_LEFT, pointToPixel(120));
 	m_listFilters.InsertColumn(3, RemoveMnemonic(_("Match &case")), LVCFMT_LEFT, pointToPixel(120));
 	m_listFilters.InsertColumn(4, RemoveMnemonic(_("Match &whole word only")), LVCFMT_LEFT, pointToPixel(120));
	m_listFilters.SetBooleanValueColumn(2);
	m_listFilters.SetBooleanValueColumn(3);
	m_listFilters.SetBooleanValueColumn(4);

	if (m_pSubstitutionFiltersList)
	{
		for (int i = 0; i < (int)m_pSubstitutionFiltersList->GetCount(); i++)
		{
			const SubstitutionFilter& item = m_pSubstitutionFiltersList->GetAt(i);
			m_listFilters.InsertItem(i, item.pattern.c_str());
			m_listFilters.SetItemText(i, 1, item.replacement.c_str());
			m_listFilters.SetItemBooleanValue(i, 2, item.useRegExp);
			m_listFilters.SetItemBooleanValue(i, 3, item.caseSensitive);
			m_listFilters.SetItemBooleanValue(i, 4, item.matchWholeWordOnly);
			m_listFilters.SetCheck(i, item.enabled);
		}
	}
}

/**
 * @brief Open help from mainframe when user presses F1.
 */
void SubstitutionFiltersDlg::OnHelp()
{
	theApp.ShowHelp(FilterHelpLocation);
}

/**
 * @brief Called when Add-button is clicked.
 */
void SubstitutionFiltersDlg::OnBnClickedAddBtn()
{
	int num = m_listFilters.GetItemCount();
	int ind = m_listFilters.InsertItem(num, _("<Edit here>").c_str());
	m_listFilters.SetItemText(num, 1, _("<Edit here>").c_str());
	m_listFilters.SetItemBooleanValue(num, 2, false);
	m_listFilters.SetItemBooleanValue(num, 3, false);
	m_listFilters.SetItemBooleanValue(num, 4, false);
	m_listFilters.SetCheck(num);

	if (ind >= -1)
	{
		m_listFilters.SetItemState(ind, LVIS_SELECTED, LVIS_SELECTED);
		m_listFilters.EnsureVisible(ind, FALSE);
	}
}

/**
 * @brief Called when Clear-button is clicked.
 */
void SubstitutionFiltersDlg::OnBnClickedClearBtn()
{
	m_listFilters.DeleteAllItems();
}

/**
 * @brief Save filters to list when exiting the dialog.
 */
BOOL SubstitutionFiltersDlg::OnApply()
{
	m_pSubstitutionFiltersList->Empty();

	for (int i = 0; i < m_listFilters.GetItemCount(); i++)
	{
		String symbolBeforeRename = m_listFilters.GetItemText(i, 0);
		String symbolAfterRename = m_listFilters.GetItemText(i, 1);
		bool useRegExp = m_listFilters.GetItemBooleanValue(i, 2);
		bool caseSensitive = m_listFilters.GetItemBooleanValue(i, 3);
		bool matchWholeWordOnly = m_listFilters.GetItemBooleanValue(i, 4);
		if (useRegExp)
			matchWholeWordOnly = false;
		bool enabled = !!m_listFilters.GetCheck(i);
		m_pSubstitutionFiltersList->Add(symbolBeforeRename, symbolAfterRename,
			useRegExp, caseSensitive, matchWholeWordOnly, enabled);
	}

	// Test
	try
	{
		m_pSubstitutionFiltersList->MakeSubstitutionList(true);
	}
	catch (Poco::RegularExpressionException& e)
	{
		AfxMessageBox(ucr::toTString(e.message()).c_str(), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	m_pSubstitutionFiltersList->SetEnabled(m_bEnabled);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("FilterStartPage"), GetParentSheet()->GetActiveIndex());
	return TRUE;
}

/**
 * @brief Sets external filter list.
 * @param [in] list External filter list.
 */
void SubstitutionFiltersDlg::SetList(SubstitutionFiltersList *list)
{
	m_pSubstitutionFiltersList = list;
}

/**
 * @brief Called when Remove button is clicked.
 */
void SubstitutionFiltersDlg::OnBnClickedRemovebtn()
{
	int sel = m_listFilters.GetNextItem(-1, LVNI_SELECTED);
	if (sel != -1)
	{
		m_listFilters.DeleteItem(sel);
	}

	int newSel = min(m_listFilters.GetItemCount() - 1, sel);
	if (newSel >= -1)
	{
		m_listFilters.SetItemState(newSel, LVIS_SELECTED, LVIS_SELECTED);
		bool bPartialOk = false;
		m_listFilters.EnsureVisible(newSel, bPartialOk);
	}
}
