/** 
 * @file  DirFilterBar.cpp
 *
 * @brief Implementation of CDirFilterBar dialog bar
 */

#include "stdafx.h"
#include "DirFilterBar.h"
#include "FileFilterHelper.h"
#include "FilterErrorMessages.h"
#include "Merge.h"
#include "Constants.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Constructor.
 * @param [in] pParent Parent window for progress dialog.
 */
CDirFilterBar::CDirFilterBar()
	: m_pFileFilterHelper(new FileFilterHelper())
{
}

CDirFilterBar::~CDirFilterBar()
{
}

void CDirFilterBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_CBStringExact(pDX, IDC_FILTERFILE_MASK, m_sFilter);
	DDX_Control(pDX, IDC_FILTERFILE_MASK, m_ctlFilter);
}

BEGIN_MESSAGE_MAP(CDirFilterBar, CTrDialogBar)
	ON_COMMAND(ID_HELP, OnHelp)
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
BOOL CDirFilterBar::Create(CWnd* pParentWnd)
{
	if (! CTrDialogBar::Create(pParentWnd, CDirFilterBar::IDD, 
			CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY, CDirFilterBar::IDD))
		return FALSE; 

	UpdateData(FALSE);

	m_ctlFilter.LoadState(_T("Files\\DisplayExt"));

	COMBOBOXINFO cbi{ sizeof(COMBOBOXINFO) };
	GetComboBoxInfo(m_ctlFilter.m_hWnd, &cbi);
	m_ctlFilterEdit.SubclassWindow(cbi.hwndItem);
	m_ctlFilterEdit.m_validator = [this](const CString& text, CString& error) -> bool
		{
			m_pFileFilterHelper->SetMaskOrExpression((const tchar_t*)text);
			const bool bError = !m_pFileFilterHelper->GetErrorList().empty();
			if (bError)
			{
				for (const auto* errorItem : m_pFileFilterHelper->GetErrorList())
					error += (FormatFilterErrorSummary(*errorItem) + _T("\r\n")).c_str();
			}
			return !bError;
		};
	m_ctlFilterEdit.Validate();
	m_ctlFilterEdit.SetCueBanner(
		strutils::format_string1(_("e.g. %1"), _T("*.txt|fe:Size > 100KB")).c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDirFilterBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F1)
	{
		OnHelp();
		return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}

void CDirFilterBar::OnHelp()
{
	CMergeApp::ShowHelp(DisplayFolderFilterHelpLocation);
}

void CDirFilterBar::OnKillFocusFilter()
{
	UpdateData(TRUE);
}

void CDirFilterBar::OnEditChangeFilter()
{
	m_ctlFilterEdit.SetApplied(false);
	m_ctlFilterEdit.OnEnChange();
}

void CDirFilterBar::SaveFilterText()
{
	UpdateData(TRUE);
	if (m_sFilter.empty())
		return;
	m_ctlFilter.SaveState(_T("Files\\DisplayExt"));
	m_ctlFilter.LoadState(_T("Files\\DisplayExt"));
}

void CDirFilterBar::ShowFilterMenu()
{
	UpdateData(TRUE);
	CRect rc;
	GetDlgItem(IDC_FILTERFILE_MASK_MENU)->GetWindowRect(&rc);
	const std::optional<String> filter = m_menu.ShowMenu(m_sFilter, rc.left, rc.bottom, this);
	if (filter.has_value())
	{
		m_sFilter = *filter;
		UpdateData(FALSE);
		m_ctlFilterEdit.SetApplied(false);
		m_ctlFilterEdit.OnEnChange();
		m_ctlFilterEdit.SetFocus();
	}
}

