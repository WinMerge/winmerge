// PropFilter.h : header file
//

#if !defined(AFX_PROPFILTER_H__73E79E13_34DD_4C86_A3EC_A1044B721CCA__INCLUDED_)
#define AFX_PROPFILTER_H__73E79E13_34DD_4C86_A3EC_A1044B721CCA__INCLUDED_

/** 
 * @file  PropFilter.h
 *
 * @brief Declaration file for Line Filter dialog
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

/// Class for Line filter propertypage
class CPropFilter : public CPropertyPage
{
	DECLARE_DYNAMIC(CPropFilter)

// Construction
public:
	CPropFilter();
	~CPropFilter();

// Dialog Data
	//{{AFX_DATA(CPropFilter)
	enum { IDD = IDD_PROPPAGE_FILTER };
	CEdit	m_cPattern;
	BOOL	m_bIgnoreRegExp;
	CString	m_sPattern;
	//}}AFX_DATA

// Implementation

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropFilter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropFilter)
	virtual BOOL OnInitDialog();
	afx_msg void OnIgnoreregexp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPFILTER_H__73E79E13_34DD_4C86_A3EC_A1044B721CCA__INCLUDED_)
