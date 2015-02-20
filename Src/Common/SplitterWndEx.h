/** 
 * @file  SplitterWndEx.h
 *
 * @brief Declaration of CSplitterWndEx class
 */
#pragma once

class CSplitterWndEx : public CSplitterWnd  
{
	DECLARE_DYNCREATE(CSplitterWndEx)

public:
	CSplitterWndEx();
	virtual ~CSplitterWndEx();
	void LockBar(BOOL bState=TRUE){m_bBarLocked=bState;};
	void ResizablePanes(BOOL bState=TRUE){m_bResizePanes=bState;};
	void AutoResizePanes(BOOL bState=TRUE){m_bAutoResizePanes=bState;};
	void FlipSplit();

	int HitTest(CPoint pt) const;

	void EqualizeRows(); 
	void EqualizeCols(); 

	virtual void RecalcLayout();
	CScrollBar* GetScrollBarCtrl(CWnd* pWnd, int nBar) const;

private:
	BOOL m_bBarLocked;
	BOOL m_bResizePanes;
	BOOL m_bAutoResizePanes;

protected:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);
	DECLARE_MESSAGE_MAP()
};
