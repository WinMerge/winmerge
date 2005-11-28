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
enum DISPLAY_MOVED_BLOCKS
{
	DISPLAY_MOVED_NONE = 0,
	DISPLAY_MOVED_ALL,
	DISPLAY_MOVED_FOLLOW_DIFF,
};

/**
 * @brief Endpoints of line connecting moved blocks
 */
struct MovedLine
{
	CPoint ptLeft;
	CPoint ptRight;
};

typedef CList<MovedLine, MovedLine&> MOVEDLINE_LIST;

/** 
 * @brief Class showing map of files.
 */
class CLocationView : public CView
{
public:
	CLocationView();
	DECLARE_DYNCREATE(CLocationView)
	void SetConnectMovedBlocks(int displayMovedBlocks);
	void UpdateVisiblePos(int nTopLine = -1, int nBottomLine = -1);

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDiffDetailView)
	public:
	virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC);
	afx_msg void OnClose();
	//}}AFX_VIRTUAL

	CMergeDoc* GetDocument();

protected:
	// Return end of block (diff/non-diff)
	BOOL GetNextRect(int &nLineIndex);
	void DrawRect(CDC* pDC, const CRect& r, COLORREF cr, BOOL border = FALSE);
	BOOL GotoLocation(CPoint point, BOOL bRealLine = TRUE);
	int GetLineFromYPos(int nYCoord, CRect rc, int bar, BOOL bRealLine = TRUE);
	int IsInsideBar(CRect rc, POINT pt);
	void DrawVisibleAreaRect(int nTopLine = -1, int nBottomLine = -1);
	void DrawConnectLines();

private:
	CMergeEditView* m_view[2];
	int m_displayMovedBlocks;
	double m_pixInLines; //*< How many pixels is one line in bars */
	UINT m_nLeftBarLeft; //*< Left edge of left-side bar */
	UINT m_nLeftBarRight; //*< Right edge of left-side bar */
	UINT m_nRightBarLeft; //*< Left edge of right-side bar */
	UINT m_nRightBarRight; //*< Right edge of right-side bar */
	int m_visibleTop; //*< Top visible line for visible area indicator */
	int m_visibleBottom; //*< Bottom visible line for visible area indicator */
	MOVEDLINE_LIST m_movedLines; //*< List of moved block connecting lines */
	bool m_bIgnoreTrivials; //*< Whether to paint trivial blocks */

	// Generated message map functions
protected:
	//{{AFX_MSG(CLocationView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
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
