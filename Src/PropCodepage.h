/**
 * @file  PropCodepage.h
 *
 * @brief Declaration of CPropCodepage class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_PROPCODEPAGE_H__0DF931F8_E845_48B7_A658_3BEE6D3EAF85__INCLUDED_)
#define AFX_PROPCODEPAGE_H__0DF931F8_E845_48B7_A658_3BEE6D3EAF85__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// CPropCodepage dialog

class CPropCodepage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropCodepage)

// Construction
public:
	CPropCodepage();
	~CPropCodepage();

// Dialog Data
	//{{AFX_DATA(CPropCodepage)
	enum { IDD = IDD_PROPPAGE_CODEPAGE };
	int		m_nCodepageSystem;
	int		m_nCustomCodepageValue;
	BOOL	m_bDetectCodepage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropCodepage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropCodepage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCpSystem();
	afx_msg void OnCpCustom();
	afx_msg void OnCpUi();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPCODEPAGE_H__0DF931F8_E845_48B7_A658_3BEE6D3EAF85__INCLUDED_)
