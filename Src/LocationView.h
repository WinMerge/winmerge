//////////////////////////////////////////////////////////////////////
/** 
 * @file  LocationView.h
 *
 * @brief Declaration of CLocationView class
 */
//
//////////////////////////////////////////////////////////////////////
#ifndef __LOCATIONVIEW_H__
#define __LOCATIONVIEW_H__


/**
 * @brief Status for display moved block
 */
enum
{
	DISPLAY_MOVED_NONE = 0,
	DISPLAY_MOVED_ALL,
	DISPLAY_MOVED_FOLLOW_DIFF,
};


/** 
 * @brief Class showing map of files.
 */
class CLocationView : public CView
{
public:
	CLocationView();
	DECLARE_DYNCREATE(CLocationView)
	void SetConnectMovedBlocks(int displayMovedBlocks);

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDiffDetailView)
	public:
	virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

	CMergeDoc* GetDocument();

protected:
	// Return end of block (diff/non-diff)
	BOOL GetNextRect(int &nLineIndex);
	void DrawRect(CDC* pDC, const CRect& r, COLORREF cr, BOOL border = FALSE);
	BOOL GotoLocation(CPoint point);

private:
	CMergeEditView* m_view0;
	CMergeEditView* m_view1;
	int m_displayMovedBlocks;

	// Generated message map functions
protected:
	//{{AFX_MSG(CLocationView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DiffView.cpp
inline CMergeDoc* CLocationView::GetDocument()
   { return (CMergeDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif //__LOCATIONVIEW_H__
