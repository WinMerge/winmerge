//////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeDiffDetailView.h
 *
 * @brief Declaration of CMergeDiffDetailView class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$
//
//////////////////////////////////////////////////////////////////////
#ifndef __MERGEDIFFDETAILVIEW_H__
#define __MERGEDIFFDETAILVIEW_H__


//#define CONTEXT_LINES   5


/////////////////////////////////////////////////////////////////////////////
// CMergeDiffDetailView view
#ifndef __EDTLIB_H
#include "edtlib.h"
#endif



/**
* @note This class must not be used in a vertical scrollable splitter
* as we want to scroll only in the current diff, but the vertical
* scrollbar would be for the whole buffer.
* There are three virtual functions : ScrollToSubLine/EnsureVisible/SetSelection
* to be sure that the top line and the cursor/selection pos remains in the
* current diff.
*
*/
class CMergeDiffDetailView : public CCrystalTextView
{
protected:
	CMergeDiffDetailView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMergeDiffDetailView)

// Attributes
public:
	BOOL m_bIsLeft;
protected:
	int lineBegin, diffLength, displayLength;


// Operations
public:
	virtual CCrystalTextBuffer *LocateTextBuffer ();
	void DoScroll(UINT code, UINT pos, BOOL bDoScroll);
	CMergeDoc* GetDocument();
	void UpdateResources();
	BOOL IsModified() { return FALSE; }
	BOOL PrimeListWithFile();
	int ComputeInitialHeight(); 
	void SetDisplayHeight(int h); 

	virtual void EnsureVisible (CPoint pt);
	virtual void SetSelection (const CPoint & ptStart, const CPoint & ptEnd);

	void SelectArea(const CPoint & ptStart, const CPoint & ptEnd) { SetSelection(ptStart, ptEnd); } // make public
	virtual void OnDisplayDiff(int nDiff=0);

protected:
	BOOL EnsureInDiff(CPoint & pt);
  virtual void ScrollToSubLine (int nNewTopLine, BOOL bNoSmoothScroll = FALSE, BOOL bTrackScrollBar = TRUE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDiffDetailView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMergeDiffDetailView();
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
	//{{AFX_MSG(CMergeDiffDetailView)
	afx_msg void OnRefresh();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowlinediff();
	afx_msg void OnUpdateShowlinediff(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DiffView.cpp
inline CMergeDoc* CMergeDiffDetailView::GetDocument()
   { return (CMergeDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif //__MERGEDIFFDETAILVIEW_H__
