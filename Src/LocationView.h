/** 
 * @file  LocationView.h
 *
 * @brief Declaration of CLocationView class
 */
#pragma once

#include <vector>
#include <memory>

class CMergeDoc;
class CMergeEditView;

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
 * @brief A struct mapping difference lines to pixels in location pane.
 * This structure maps one difference's line numbers to pixel locations in
 * the location pane. The line numbers are "fixed" i.e. they are converted to
 * word-wrapped absolute line numbers if needed.
 */
struct DiffBlock
{
	unsigned top_line; /**< First line of the difference. */
	unsigned bottom_line; /**< Last line of the difference. */
	unsigned top_coord; /**< X-coord of diff block begin. */
	unsigned bottom_coord; /**< X-coord of diff block end. */
	unsigned diff_index; /**< Index of difference in the original diff list. */
	int op;
};

/** 
 * @brief Class showing map of files.
 * The location is a view showing two vertical bars. Each bar depicts one file
 * in the file compare. The bars show a scaled view of the files. The
 * difference areas are drawn with the same colors than in actual file compare.
 * Also visible area of files is drawn as "shaded".
 *
 * These visualizations allow user to easily see a overall picture of the files
 * in comparison. Using mouse it allows easy and fast moving in files.
 */
class CLocationView : public CView
{
public:
	CLocationView();
	~CLocationView();
	DECLARE_DYNCREATE(CLocationView)
	void SetConnectMovedBlocks(int displayMovedBlocks);
	void UpdateVisiblePos(int nTopLine = -1, int nBottomLine = -1);
	void SetFrameHwnd(HWND hwndFrame);
	void ForceRecalculate();

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocationView)
	public:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

protected:
	CMergeDoc* GetDocument();
	void DrawRect(CDC* pDC, const CRect& r, COLORREF cr, bool bSelected = false);
	bool GotoLocation(const CPoint& point, bool bRealLine = true);
	int GetLineFromYPos(int nYCoord, int bar, bool bRealLine = true);
	int IsInsideBar(const CRect& rc, const POINT& pt);
	void DrawVisibleAreaRect(CDC* pDC, int nTopLine = -1, int nBottomLine = -1);
	void DrawConnectLines(CDC* pDC);
	void DrawDiffMarker(CDC* pDC, int yCoord);
	void CalculateBars();
	void CalculateBlocks();
	void CalculateBlocksPixel(int nBlockStart, int nBlockEnd, int nBlockLength,
			int &nBeginY, int &nEndY);
	void DrawBackground(CDC* pDC);

private:
	int m_displayMovedBlocks; //*< Setting for displaying moved blocks */
	double m_pixInLines; //*< How many pixels is one line in bars */
	double m_lineInPix; //*< How many lines is one pixel?
	CRect m_bar[3]; //*< Left/middle/riggt bar */
	int m_visibleTop; //*< Top visible line for visible area indicator */
	int m_visibleBottom; //*< Bottom visible line for visible area indicator */
	int m_nSubLineCount[3]; //*< Cached subline count */
	MOVEDLINE_LIST m_movedLines; //*< List of moved block connecting lines */
	HWND m_hwndFrame; //*< Frame window handle */
	std::unique_ptr<CBitmap> m_pSavedBackgroundBitmap; //*< Saved background */
	bool m_bDrawn; //*< Is already drawn in location pane? */
	std::vector<DiffBlock> m_diffBlocks; //*< List of pre-calculated diff blocks.
	bool m_bRecalculateBlocks; //*< Recalculate diff blocks in next repaint.
	CSize m_currentSize; //*< Current size of the panel.

	// Generated message map functions
protected:
	//{{AFX_MSG(CLocationView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int  OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnVScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DiffView.cpp
inline CMergeDoc* CLocationView::GetDocument()
   { return reinterpret_cast<CMergeDoc*>(m_pDocument); }
#endif

/** 
 * @brief Stores HWND of frame window (CMergeEditFrame).
 */
inline void CLocationView::SetFrameHwnd(HWND hwndFrame)
{
	m_hwndFrame = hwndFrame;
}

