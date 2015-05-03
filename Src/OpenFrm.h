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

protected:
	CRect m_rcNormalView;   // rectangle for normal (MFC) view pos

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
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COpenFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(COpenFrame)
	afx_msg void OnPaint(); // override required to paint rectangles.
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
