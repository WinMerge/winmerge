/** 
 * @file  MatchInsideDlg.cpp
 *
 * @brief Implementation of CMatchInsideDlg class.
 */

#include "StdAfx.h"
#include "MatchInsideDlg.h"
#include "LineFilterHelper.h"
#include "LineFilterHelperMenu.h"
#include "FilterErrorMessages.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CMatchInsideDlg dialog

/**
 * @brief Default constructor.
 */
CMatchInsideDlg::CMatchInsideDlg(CWnd* pParent /*=nullptr*/)
	: CTrDialog(CMatchInsideDlg::IDD, pParent)
	, m_pLineFilterHelper1(new LineFilterHelper())
	, m_pLineFilterHelper2(new LineFilterHelper())
{
}

/**
 * @brief Constructor with initial expressions.
 */
CMatchInsideDlg::CMatchInsideDlg(const String& filter1, const String& filter2, CWnd* pParent /*=nullptr*/)
	: CTrDialog(CMatchInsideDlg::IDD, pParent)
	, m_sFilter1(filter1)
	, m_sFilter2(filter2)
	, m_pLineFilterHelper1(new LineFilterHelper())
	, m_pLineFilterHelper2(new LineFilterHelper())
{
}

/**
 * @brief Data exchange.
 */
void CMatchInsideDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMatchInsideDlg)
	DDX_CBStringExact(pDX, IDC_MATCHINSIDE_EXPR1, m_sFilter1);
	DDX_CBStringExact(pDX, IDC_MATCHINSIDE_EXPR2, m_sFilter2);
	DDX_Control(pDX, IDC_MATCHINSIDE_EXPR1, m_ctlFilter1);
	DDX_Control(pDX, IDC_MATCHINSIDE_EXPR2, m_ctlFilter2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMatchInsideDlg, CTrDialog)
	//{{AFX_MSG_MAP(CMatchInsideDlg)
	ON_BN_CLICKED(IDC_MATCHINSIDE_EXPR1_BUTTON, OnFilter1Button)
	ON_BN_CLICKED(IDC_MATCHINSIDE_EXPR2_BUTTON, OnFilter2Button)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMatchInsideDlg message handlers

/**
 * @brief Setup filter validation for a combo box.
 */
void CMatchInsideDlg::SetupFilterValidation(CValidatingEdit& edit, CSuperComboBox& combo,
											std::unique_ptr<LineFilterHelper>& helper)
{
	combo.LoadState(_T("Files\\DisplayLine"));

	COMBOBOXINFO cbi{ sizeof(COMBOBOXINFO) };
	GetComboBoxInfo(combo.m_hWnd, &cbi);
	edit.SubclassWindow(cbi.hwndItem);
	edit.m_validator = [&helper](const CString& text, CString& error) -> bool
		{
			helper->SetStringOrExpression((const tchar_t*)text);
			error = FormatFilterErrorSummary(helper->GetFilterExpression()).c_str();
			return helper->GetFilterExpression().errorCode == 0;
		};
	edit.Validate();
	edit.SetCueBanner(
		strutils::format_string1(_("e.g. %1"), _T("ERROR / le:Line contains \"ERROR\"")).c_str());
}

/**
 * @brief Initialize the dialog.
 */
BOOL CMatchInsideDlg::OnInitDialog()
{
	CTrDialog::OnInitDialog();

	SetupFilterValidation(m_ctlFilterEdit1, m_ctlFilter1, m_pLineFilterHelper1);
	SetupFilterValidation(m_ctlFilterEdit2, m_ctlFilter2, m_pLineFilterHelper2);

	// If initial values were provided in constructor, restore them
	// (LoadState() may have overwritten them with history)
	if (!m_sFilter1.empty())
	{
		m_ctlFilter1.SetWindowText(m_sFilter1.c_str());
		m_ctlFilterEdit1.Validate();
	}
	if (!m_sFilter2.empty())
	{
		m_ctlFilter2.SetWindowText(m_sFilter2.c_str());
		m_ctlFilterEdit2.Validate();
	}

	return TRUE;
}

/**
 * @brief Handle filter button click.
 */
void CMatchInsideDlg::OnFilterButton(int buttonId, String& filter)
{
	UpdateData(TRUE);

	CLineFilterHelperMenu menu;
	CWnd* pButton = GetDlgItem(buttonId);
	if (pButton)
	{
		CRect rect;
		pButton->GetWindowRect(&rect);
		std::optional<String> result = menu.ShowMenu(filter, rect.left, rect.bottom, this);
		if (result.has_value())
		{
			filter = result.value();
			UpdateData(FALSE);
		}
	}
}

/**
 * @brief Handle the "..." button for expression 1.
 */
void CMatchInsideDlg::OnFilter1Button()
{
	OnFilterButton(IDC_MATCHINSIDE_EXPR1_BUTTON, m_sFilter1);
}

/**
 * @brief Handle the "..." button for expression 2.
 */
void CMatchInsideDlg::OnFilter2Button()
{
	OnFilterButton(IDC_MATCHINSIDE_EXPR2_BUTTON, m_sFilter2);
}
