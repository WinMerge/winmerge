/** 
 * @file  SubstitutionFiltersDlg.h
 *
 * @brief Declaration file for Line Filter dialog
 *
 */
#pragma once

#include "TrDialogs.h"
#include "SubeditList.h"

class SubstitutionFilterFiltersList;

class SubstitutionFiltersDlg : public CTrPropertyPage
{
	DECLARE_DYNAMIC(SubstitutionFiltersDlg)

// Construction
public:
	SubstitutionFiltersDlg();

	void SetList(SubstitutionFiltersList *list);

// Dialog Data
	//{{AFX_DATA(SubstitutionFiltersDlg)
	enum { IDD = IDD_FILTERS_SUBSTITUTIONFILTERS };
	bool m_bEnabled;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SubstitutionFiltersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SubstitutionFiltersDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnHelp();
	virtual BOOL OnApply() override;
	afx_msg void OnBnClickedAddBtn();
	afx_msg void OnBnClickedClearBtn();
	afx_msg void OnBnClickedRemovebtn();
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void InitList();

private:
	CSubeditList m_listFilters; /**< List control having filter strings */

	SubstitutionFiltersList *m_pSubstitutionFiltersList;
};
