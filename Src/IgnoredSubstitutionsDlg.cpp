/**
 *  @file IgnoredSubstitutionsDlg.cpp
 *
 *  @brief Implementation of Ignored Substitutions dialog
 */ 

#include "stdafx.h"
#include "IgnoredSubstitutionsList.h"
#include "Merge.h"
#include "IgnoredSubstitutionsDlg.h"
#include <Poco/Exception.h>

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
	, m_pIgnoredSubstitutionsList(nullptr)
{
	//{{AFX_DATA_INIT(IgnoredSubstitutionsFiltersDlg)
	m_bEnabled = false;
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
	DDX_Check(pDX, IDC_IGNORED_SUSBSTITUTIONS_ARE_ENABLED, m_bEnabled);
	DDX_Control(pDX, IDC_IGNORED_SUBSTITUTIONS_FILTER, m_listFilters);
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

static CString BooleanValueToCheckBox(bool b)
{
	return b ? _T("\u2611") : _T("\u2610");
}

static CString RemoveMnemonic(String text)
{
	Poco::RegularExpression re("\\(&.\\)|&|:");
	std::string textu8 = ucr::toUTF8(text);
	re.subst(textu8, "", Poco::RegularExpression::RE_GLOBAL);
	return ucr::toTString(textu8).c_str();
}

void IgnoredSubstitutionsDlg::InitList()
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

	if (m_pIgnoredSubstitutionsList)
	{
		for (int i = 0; i < (int)m_pIgnoredSubstitutionsList->GetCount(); i++)
		{
			const IgnoredSubstitution& item = m_pIgnoredSubstitutionsList->GetAt(i);
			m_listFilters.InsertItem(i, item.pattern.c_str());
			m_listFilters.SetItemText(i, 1, item.replacement.c_str());
			m_listFilters.SetItemText(i, 2, BooleanValueToCheckBox(item.useRegExp));
			m_listFilters.SetItemText(i, 3, BooleanValueToCheckBox(item.caseSensitive));
			m_listFilters.SetItemText(i, 4, BooleanValueToCheckBox(item.matchWholeWordOnly));
			m_listFilters.SetCheck(i, item.enabled);
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
	int num = m_listFilters.GetItemCount();
	int ind = m_listFilters.InsertItem(num, _("<Edit here>").c_str());
	m_listFilters.SetItemText(num, 1, _("<Edit here>").c_str());
	m_listFilters.SetItemText(num, 2, BooleanValueToCheckBox(false));
	m_listFilters.SetItemText(num, 3, BooleanValueToCheckBox(false));
	m_listFilters.SetItemText(num, 4, BooleanValueToCheckBox(false));
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
void IgnoredSubstitutionsDlg::OnBnClickedClearBtn()
{
	m_listFilters.DeleteAllItems();
}

/**
 * @brief Save filters to list when exiting the dialog.
 */
BOOL IgnoredSubstitutionsDlg::OnApply()
{
	m_pIgnoredSubstitutionsList->Empty();

	for (int i = 0; i < m_listFilters.GetItemCount(); i++)
	{
		String symbolBeforeRename = m_listFilters.GetItemText(i, 0);
		String symbolAfterRename = m_listFilters.GetItemText(i, 1);
		bool useRegExp = m_listFilters.GetItemText(i, 2).Compare(_T("\u2611")) == 0;
		bool caseSensitive = m_listFilters.GetItemText(i, 3).Compare(_T("\u2611")) == 0;
		bool matchWholeWordOnly = m_listFilters.GetItemText(i, 4).Compare(_T("\u2611")) == 0;
		if (useRegExp)
			matchWholeWordOnly = false;
		bool enabled = !!m_listFilters.GetCheck(i);
		m_pIgnoredSubstitutionsList->Add(symbolBeforeRename, symbolAfterRename,
			useRegExp, caseSensitive, matchWholeWordOnly, enabled);
	}

	// Test
	try
	{
		m_pIgnoredSubstitutionsList->MakeSubstitutionList(true);
	}
	catch (Poco::RegularExpressionException& e)
	{
		AfxMessageBox(ucr::toTString(e.message()).c_str(), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("FilterStartPage"), GetParentSheet()->GetActiveIndex());
	return TRUE;
}

/**
 * @brief Sets external filter list.
 * @param [in] list External filter list.
 */
void IgnoredSubstitutionsDlg::SetList(IgnoredSubstitutionsList *list)
{
	m_pIgnoredSubstitutionsList = list;
}

/**
 * @brief Called when Remove button is clicked.
 */
void IgnoredSubstitutionsDlg::OnBnClickedRemovebtn()
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

/**
 * @brief Called when in-place editing has finished.
 */
void IgnoredSubstitutionsDlg::OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_listFilters.OnEndLabelEdit(pNMHDR, pResult);
}
