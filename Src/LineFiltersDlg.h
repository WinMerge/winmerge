/** 
 * @file  PropLineFilter.h
 *
 * @brief Declaration file for Line Filter dialog
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "afxcmn.h"
#include "afxwin.h"
#if !defined(AFX_PROPFILTER_H__73E79E13_34DD_4C86_A3EC_A1044B721CCA__INCLUDED_)
#define AFX_PROPFILTER_H__73E79E13_34DD_4C86_A3EC_A1044B721CCA__INCLUDED_

class LineFiltersList;

/// Class for Line filter propertypage
class CPropLineFilter : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropLineFilter)

// Construction
public:
	CPropLineFilter();
	~CPropLineFilter();

	void SetList(LineFiltersList * list);

// Dialog Data
	//{{AFX_DATA(CPropLineFilter)
	enum { IDD = IDD_PROPPAGE_FILTER };
	BOOL	m_bIgnoreRegExp;
	//}}AFX_DATA

// Implementation

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropLineFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropLineFilter)
	virtual BOOL OnInitDialog();
	afx_msg void OnIgnoreregexp();
	afx_msg void OnHelp();
	virtual void OnOK();
	afx_msg void OnBnClickedLfilterAddBtn();
	afx_msg void OnBnClickedLfilterEditbtn();
	afx_msg void OnBnClickedLfilterRemovebtn();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedLfilterEditsave();
	afx_msg void OnLvnItemActivateLfilterList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusLfilterEditbox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void InitList();
	int AddRow(LPCTSTR filter = NULL, BOOL enabled = FALSE);
	void EditSelectedFilter();
	void SaveItem();

private:
	CListCtrl m_filtersList; /**< List control having filter strings */
	CEdit m_editRegexp; /**< Editbox for editing filter */
	CButton m_saveRegexp; /**< Button for saving edited filter */

	LineFiltersList * m_pList; /**< Helper list for getting/setting filters. */
	BOOL m_bEditing; /**< Are we editing a filter string? */
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPFILTER_H__73E79E13_34DD_4C86_A3EC_A1044B721CCA__INCLUDED_)
