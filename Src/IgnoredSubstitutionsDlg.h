/** 
 * @file  IgnoredSubstitutionsDlg.h
 *
 * @brief Declaration file for Line Filter dialog
 *
 */
#pragma once

#include "TrDialogs.h"
#include "SubeditList.h"

class IgnoredSubstitutionFiltersList;

class IgnoredSubstitutionsDlg : public CTrPropertyPage
{
	DECLARE_DYNAMIC(IgnoredSubstitutionsDlg)

// Construction
public:
	IgnoredSubstitutionsDlg();

	void SetList(TokenPairList *list);

// Dialog Data
	//{{AFX_DATA(IgnoredSubstitutionsDlg)
	enum { IDD = IDD_IGNORED_SUSBSTITUTIONS_DLG };
	bool m_IgnoredSubstitutionsAreEnabled;
	bool m_UseRegexpsForIgnoredSubstitutions;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(IgnoredSubstitutionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(IgnoredSubstitutionsDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnHelp();
	virtual void OnOK() override;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedAddBtn();
	afx_msg void OnBnClickedClearBtn();
	afx_msg void OnBnClickedEditbtn();
	afx_msg void OnBnClickedRemovebtn();
	afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeyDown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void InitList();

private:
	std::unique_ptr<CInPlaceEdit> InPlaceEdit;

	CSubeditList m_VisibleFiltersList; /**< List control having filter strings */

	TokenPairList *m_pExternalRenameList;
};
