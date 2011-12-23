#ifndef __SPLASH_H__INCLUDED__
#define __SPLASH_H__INCLUDED__

/////////////////////////////////////////////////////////////////////////////
// CBigIcon window

class EDITPADC_CLASS CBigIcon : public CButton
{
// Attributes
public:
	CBitmap m_bitmap;
	CSize m_sizeBitmap;

// Operations
public:
	void SizeToContent();

// Implementation
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//{{AFX_MSG(CBigIcon)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSplash dialog

class EDITPADC_CLASS CSplashWnd : public CDialog
{
// Construction
public:
	BOOL Create(CWnd* pParent);

// Dialog Data
	//{{AFX_DATA(CSplashWnd)
	enum { IDD = IDD_SPLASH };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Implementation
protected:
	CBigIcon m_icon; // self-draw button

	// Generated message map functions
	//{{AFX_MSG(CSplashWnd)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
};

/////////////////////////////////////////////////////////////////////////////

#endif // __SPLASH_H__INCLUDED__
