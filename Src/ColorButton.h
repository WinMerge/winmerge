#if !defined(AFX_COLORBUTTON_H__75623C05_2D2D_4FBE_B819_661F49DEAE94__INCLUDED_)
#define AFX_COLORBUTTON_H__75623C05_2D2D_4FBE_B819_661F49DEAE94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorButton window

class CColorButton : public CButton
{
	COLORREF	m_clrFill;

// Construction
public:
	CColorButton( COLORREF clrFill );

// Attributes
public:

// Operations
public:

	void	SetColor( COLORREF clrFill );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBUTTON_H__75623C05_2D2D_4FBE_B819_661F49DEAE94__INCLUDED_)
