/**
 * @file FiltersPropertySheet.cpp
 * @brief Implementation file for CFiltersPropertySheet
 */
#include <stdafx.h>
#include "FiltersPropertySheet.h"

constexpr UINT_PTR IDT_LOADPOSITION = 1;

/////////////////////////////////////////////////////////////////////////////
// CFiltersPropertySheet dialog

IMPLEMENT_DYNAMIC(CFiltersPropertySheet, CPropertySheet)

CFiltersPropertySheet::CFiltersPropertySheet() : CPropertySheet()
{
	EnableDynamicLayout();
}

CFiltersPropertySheet::CFiltersPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	EnableDynamicLayout();
}

CFiltersPropertySheet::CFiltersPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage) : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	EnableDynamicLayout();
}

BEGIN_MESSAGE_MAP(CFiltersPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CFiltersPropertySheet)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CFiltersPropertySheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	I18n::TranslateDialog(m_hWnd);

	m_constraint.InitializeCurrentSize(this);
	m_constraint.SubclassWnd(); // install subclassing

	SetTimer(IDT_LOADPOSITION, 1, nullptr);

	return bResult;
}

void CFiltersPropertySheet::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == IDT_LOADPOSITION)
	{
		KillTimer(nIDEvent);
		// persist size via registry
		m_constraint.LoadPosition(_T("ResizeableDialogs"), _T("FiltersPropertySheet"), false);

		CenterWindow();
	}
}
