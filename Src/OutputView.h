// OutputView.h : interface of the COutputView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CCrystalTextView.h"

class COutputView : public CCrystalTextView
{
protected: // create from serialization only
	COutputView();
	DECLARE_DYNCREATE(COutputView)
	CCrystalParser m_xParser;

	// Attributes
public:
	virtual CCrystalTextBuffer* LocateTextBuffer() override;

protected:

	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(COutputView)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual void OnInitialUpdate() override;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(COutputView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in OutputView.cpp
inline COutputDoc* COutputView::GetDocument()
{
	return (COutputDoc*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
