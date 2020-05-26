// SPDX-License-Identifier: GPL-2.0-or-later
/**
 * @file  CompareStatisticsDlg.h
 *
 * @brief Declaration file for CompareStatisticsDlg dialog
 */
#pragma once

#include "TrDialogs.h"

class CompareStats;

/**
 * @brief A dialog showing folder compare statistics.
 * This dialog shows statistics about current folder compare. It shows amounts
 * if items in different compare status categories.
 */
class CompareStatisticsDlg : public CTrDialog
{
	DECLARE_DYNAMIC(CompareStatisticsDlg)

public:
	explicit CompareStatisticsDlg(const CompareStats * pStats, CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CompareStatisticsDlg)
	enum { IDD = IDD_COMPARE_STATISTICS };
	//}}AFX_DATA

protected:
	// Generated message map functions
	//{{AFX_MSG(CompareStatisticsDlg)
	afx_msg BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation data
private:
	const CompareStats * m_pCompareStats; /**< Compare statistics structure. */
};
