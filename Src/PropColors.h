#if !defined(AFX_PROPCOLORS_H__9C0A0F92_CDD6_40FE_BEFC_101B1E22FFDD__INCLUDED_)
#define AFX_PROPCOLORS_H__9C0A0F92_CDD6_40FE_BEFC_101B1E22FFDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropColors.h : header file
//

#include "ColorButton.h"

const int CustomColorsAmount = 16;

/////////////////////////////////////////////////////////////////////////////
// CPropColors dialog

class CPropColors : public CPropertyPage
{
// Construction
public:

	CPropColors(COLORREF clrDiff, COLORREF clrSelDiff, COLORREF clrDiffDeleted, COLORREF clrSelDiffDeleted, COLORREF clrDiffText, COLORREF clrSelDiffText, COLORREF clrTrivial, COLORREF clrTrivialDeleted);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPropColors)
	enum { IDD = IDD_PROPPAGE_COLORS };
	CColorButton	m_cTrivialDeleted;
	CColorButton	m_cTrivial;
	CColorButton	m_cSelDiffText;
	CColorButton	m_cDiffText;
	CColorButton	m_cSelDiffDeleted;
	CColorButton	m_cDiffDeleted;
	CColorButton	m_cSelDiff;
	CColorButton	m_cDiff;
	//}}AFX_DATA


	COLORREF	m_clrDiff;
	COLORREF	m_clrSelDiff;
	COLORREF	m_clrDiffDeleted;
	COLORREF	m_clrSelDiffDeleted;
	COLORREF	m_clrDiffText;
	COLORREF	m_clrSelDiffText;
	COLORREF	m_clrTrivial;
	COLORREF	m_clrTrivialDeleted;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropColors)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void LoadCustomColors();
	void SaveCustomColors();

	COLORREF m_cCustColors[CustomColorsAmount];

	// Generated message map functions
	//{{AFX_MSG(CPropColors)
	afx_msg void OnDifferenceColor();
	afx_msg void OnDifferenceDeletedColor();
	afx_msg void OnSelDifferenceDeletedColor();
	afx_msg void OnSelDifferenceColor();
	afx_msg void OnDifferenceTextColor();
	afx_msg void OnSelDifferenceTextColor();
	afx_msg void OnTrivialDiffColor();
	afx_msg void OnTrivialDiffDeletedColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPCOLORS_H__9C0A0F92_CDD6_40FE_BEFC_101B1E22FFDD__INCLUDED_)
