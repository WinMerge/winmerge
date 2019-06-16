// OpenFrm.h : interface of the COpenFrame class
//
#pragma once

#include "MergeFrameCommon.h"

class COpenFrame : public CMergeFrameCommon
{
	DECLARE_DYNCREATE(COpenFrame)
public:
	COpenFrame();

// Attributes
public:

// Operations
public:
	void UpdateResources();

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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
