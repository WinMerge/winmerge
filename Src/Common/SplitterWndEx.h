/** 
 * @file  SplitterWndEx.h
 *
 * @brief Declaration of CSplitterWndEx class
 */
#pragma once

#include <vector>

class CSplitterWndEx : public CSplitterWnd  
{
	DECLARE_DYNCREATE(CSplitterWndEx)

public:
	CSplitterWndEx();
	virtual ~CSplitterWndEx();
	void LockBar(bool bState=true){m_bBarLocked=bState;};
	void ResizablePanes(bool bState=true){m_bResizePanes=bState;};
	void AutoResizePanes(bool bState=true){m_bAutoResizePanes=bState;};
	void HideBorders(bool bHide)
	{
		m_cxBorder = m_cyBorder = bHide ? 0 : 2;
		m_bHideBorders = bHide;
	}
	void FlipSplit();

	int HitTest(CPoint pt) const;

	void EqualizeRows(); 
	void EqualizeCols(); 

	virtual void RecalcLayout() override;
	virtual void TrackRowSize(int y, int row) override;
	virtual void TrackColumnSize(int x, int col) override;
	virtual CWnd* GetActivePane(int* pRow = NULL, int* pCol = NULL) override;

	CScrollBar* GetScrollBarCtrl(CWnd* pWnd, int nBar) const;

private:
	bool m_bBarLocked;
	bool m_bResizePanes;
	bool m_bAutoResizePanes;
	bool m_bHideBorders;
	std::vector<int> m_rowRatios;
	std::vector<int> m_colRatios;

	BOOL CreateScrollBarCtrl(DWORD dwStyle, UINT nID) override;

protected:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rect);
	DECLARE_MESSAGE_MAP()
};
