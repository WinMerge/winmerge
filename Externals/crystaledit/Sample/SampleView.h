// SampleView.h : interface of the CSampleView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CCrystalEditView.h"
#include "CrystalEditViewEx.h"

class CSampleDoc;

class CSampleView : public CCrystalEditViewEx
{
protected: // create from serialization only
	CSampleView();
	DECLARE_DYNCREATE(CSampleView)
	CCrystalParser m_xParser;

// Attributes
public:
	CSampleDoc* GetDocument();

	virtual CCrystalTextBuffer *LocateTextBuffer() override;

protected:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	virtual void OnInitialUpdate() override;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSampleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSampleView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelMargin();
	afx_msg void OnWordWrap();
	afx_msg void OnViewWhitespace();
	afx_msg void OnUpdateViewWhitespace(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SampleView.cpp
inline CSampleDoc* CSampleView::GetDocument()
   { return (CSampleDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
