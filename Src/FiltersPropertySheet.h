/**
 * @file FiltersPropertySheet.h
 * @brief Declaration file for CFiltersPropertySheet
 */
#pragma once
#include <afxdlgs.h>
#include "CMoveConstraint.h"

class CFiltersPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CFiltersPropertySheet)

public:
	CFiltersPropertySheet();
	CFiltersPropertySheet(UINT nIDCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);
	CFiltersPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()

private:
	prdlg::CMoveConstraint m_constraint; /**< Resizes dialog controls when dialog resized */
};