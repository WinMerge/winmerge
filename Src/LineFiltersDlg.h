/** 
 * @file  LineFiltersDlg.h
 *
 * @brief Declaration file for Line Filter dialog
 *
 */
#pragma once

#include "TrDialogs.h"

class LineFiltersList;

/**
 * @brief A dialog for editing and selecting used line filters.
 * This dialog allows user to add, edit and remove line filters. Currently
 * active filters are selected by enabling their checkbox.
 */
class LineFiltersDlg : public CTrPropertyPage
{
	DECLARE_DYNAMIC(LineFiltersDlg)

// Construction
public:
	LineFiltersDlg();

	void SetList(LineFiltersList * list);

// Dialog Data
	//{{AFX_DATA(LineFiltersDlg)
	enum { IDD = IDD_PROPPAGE_FILTER };
	bool	m_bIgnoreRegExp;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(LineFiltersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(LineFiltersDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnHelp();
	virtual void OnOK() override;
	afx_msg void OnBnClickedLfilterAddBtn();
	afx_msg void OnBnClickedLfilterEditbtn();
	afx_msg void OnBnClickedLfilterRemovebtn();
	afx_msg void OnLvnItemActivateLfilterList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeyDownLfilterList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEndLabelEditLfilterList(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void InitList();
	int AddRow(const String& filter = nullptr, bool enabled = false);
	void EditSelectedFilter();

private:
	CListCtrl m_filtersList; /**< List control having filter strings */

	LineFiltersList * m_pList; /**< Helper list for getting/setting filters. */
};
