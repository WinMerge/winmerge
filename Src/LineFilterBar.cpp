/** 
 * @file  LineFilterBar.cpp
 *
 * @brief Implementation of CLineFilterBar dialog bar
 */

#include "stdafx.h"
#include "LineFilterBar.h"
#include "LineFilterHelper.h"
#include "FilterErrorMessages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Constructor.
 * @param [in] pParent Parent window for progress dialog.
 */
CLineFilterBar::CLineFilterBar()
	: m_pLineFilterHelper(new LineFilterHelper())
{
}

CLineFilterBar::~CLineFilterBar()
{
}

void CLineFilterBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_CBStringExact(pDX, IDC_FILTERFILE_MASK, m_sFilter);
	DDX_Control(pDX, IDC_FILTERFILE_MASK, m_ctlFilter);
}

BEGIN_MESSAGE_MAP(CLineFilterBar, CTrDialogBar)
	ON_CBN_KILLFOCUS(IDC_FILTERFILE_MASK, OnKillFocusFilter)
	ON_CBN_EDITCHANGE(IDC_FILTERFILE_MASK, OnEditChangeFilter)
	ON_CBN_SELCHANGE(IDC_FILTERFILE_MASK, OnEditChangeFilter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialize the dialog.
 * Center the dialog to main window.
 * @return TRUE (see the comment inside function).
 */
BOOL CLineFilterBar::Create(CWnd* pParentWnd)
{
	if (! CTrDialogBar::Create(pParentWnd, CLineFilterBar::IDD, 
			CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, CLineFilterBar::IDD))
		return FALSE; 

	UpdateData(FALSE);

	m_ctlFilter.LoadState(_T("Files\\DisplayLine"));

	COMBOBOXINFO cbi{ sizeof(COMBOBOXINFO) };
	GetComboBoxInfo(m_ctlFilter.m_hWnd, &cbi);
	m_ctlFilterEdit.SubclassWindow(cbi.hwndItem);
	m_ctlFilterEdit.m_validator = [this](const CString& text, CString& error) -> bool
		{
			m_pLineFilterHelper->SetStringOrExpression((const tchar_t*)text);
			error = FormatFilterErrorSummary(m_pLineFilterHelper->GetFilterExpression()).c_str();
			return m_pLineFilterHelper->GetFilterExpression().errorCode == 0;
		};
	m_ctlFilterEdit.Validate();
	m_ctlFilterEdit.SetCueBanner(strutils::format_string1(_("e.g. %1"), _T("le:Line contains \"text\"")).c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLineFilterBar::OnKillFocusFilter()
{
	UpdateData(TRUE);
}

void CLineFilterBar::OnEditChangeFilter()
{
	m_ctlFilterEdit.OnEnChange();
}

void CLineFilterBar::SaveFilterText()
{
	UpdateData(TRUE);
	if (m_sFilter.empty())
		return;
	m_ctlFilter.SaveState(_T("Files\\DisplayLine"));
	m_ctlFilter.LoadState(_T("Files\\DisplayLine"));
}

void CLineFilterBar::ShowFilterMenu()
{
	UpdateData(TRUE);
	CRect rc;
	GetDlgItem(IDC_FILTERFILE_MASK_MENU)->GetWindowRect(&rc);
	const std::optional<String> filter = m_menu.ShowMenu(m_sFilter, rc.left, rc.bottom, this);
	if (filter.has_value())
	{
		m_sFilter = *filter;
		UpdateData(FALSE);
		m_ctlFilterEdit.OnEnChange();
	}
}
