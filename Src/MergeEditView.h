#if !defined(AFX_MERGEEDITVIEW_H__0CE31CFD_4BEE_4378_ADB4_B7C9F50A9F53__INCLUDED_)
#define AFX_MERGEEDITVIEW_H__0CE31CFD_4BEE_4378_ADB4_B7C9F50A9F53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MergeEditView.h : header file
//

#define CONTEXT_LINES   5

static LPCTSTR crlfs[] =
  {
    _T ("\x0d\x0a"), //  DOS/Windows style
    _T ("\x0a"),     //  UNIX style
    _T ("\x0d")      //  Macintosh style
  };

/////////////////////////////////////////////////////////////////////////////
// CMergeEditView view
#ifndef __EDTLIB_H
#include "edtlib.h"
#endif

class CMergeEditView : public CCrystalEditViewEx
{
protected:
	CMergeEditView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMergeEditView)
    CCrystalParser m_xParser;

// Attributes
public:
	BOOL m_bIsLeft;
	LONG m_nModifications;

protected:
	// Controls if rescan is done after edit events
	BOOL m_bAutomaticRescan;

// Operations
public:
	BOOL EnableRescan(BOOL bEnable);
	void ShowDiff(BOOL bScroll, BOOL bSelectText);
	virtual void OnEditOperation(int nAction, LPCTSTR pszText);
	void UpdateLineLengths();
	BOOL IsLineInCurrentDiff(int nLine);
	void SelectNone();
	void SelectDiff(int nDiff, BOOL bScroll =TRUE, BOOL bSelectText =TRUE);
    virtual CCrystalTextBuffer *LocateTextBuffer ();
	void ResetMod();
	void AddMod();
	CString GetSelectedText();
	CString GetLineText(int idx);
	void DoScroll(UINT code, UINT pos, BOOL bDoScroll);
	CMergeDoc* GetDocument();
	void UpdateResources();
	BOOL IsModified() { return (LocateTextBuffer()->IsModified()); }
	BOOL PrimeListWithFile();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeEditView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMergeEditView();
    virtual void GetLineColors (int nLineIndex, COLORREF & crBkgnd,
                                COLORREF & crText, BOOL & bDrawWhitespace);
	virtual void UpdateSiblingScrollPos (BOOL bHorz);
	virtual void OnUpdateSibling (CCrystalTextView * pUpdateSource, BOOL bHorz);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMergeEditView)
	afx_msg void OnCurdiff();
	afx_msg void OnUpdateCurdiff(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnFirstdiff();
	afx_msg void OnUpdateFirstdiff(CCmdUI* pCmdUI);
	afx_msg void OnLastdiff();
	afx_msg void OnUpdateLastdiff(CCmdUI* pCmdUI);
	afx_msg void OnNextdiff();
	afx_msg void OnUpdateNextdiff(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiff();
	afx_msg void OnUpdatePrevdiff(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnAllLeft();
	afx_msg void OnUpdateAllLeft(CCmdUI* pCmdUI);
	afx_msg void OnAllRight();
	afx_msg void OnUpdateAllRight(CCmdUI* pCmdUI);
	afx_msg void OnL2r();
	afx_msg void OnUpdateL2r(CCmdUI* pCmdUI);
	afx_msg void OnR2l();
	afx_msg void OnUpdateR2l(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DiffView.cpp
inline CMergeDoc* CMergeEditView::GetDocument()
   { return (CMergeDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEEDITVIEW_H__0CE31CFD_4BEE_4378_ADB4_B7C9F50A9F53__INCLUDED_)
