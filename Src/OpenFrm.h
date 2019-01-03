// OpenFrm.h : interface of the COpenFrame class
//


#pragma once


class COpenFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(COpenFrame)
public:
	COpenFrame();

// Attributes
public:

// Operations
public:
	void UpdateResources();
	void SetSharedMenu(HMENU hMenu) { m_hMenuShared = hMenu; };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL DestroyWindow();
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COpenFrame();

// Generated message map functions
protected:
	//{{AFX_MSG(COpenFrame)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
