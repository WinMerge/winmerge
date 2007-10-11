//////////////////////////////////////////////////////////////////////
/** 
 * @file  SplitterWndEx.h
 *
 * @brief Declaration of CSplitterWndEx class
 */
// ID line follows -- this is updated by SVN
// $Id$
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITTERWNDEX_H__68D1F449_015D_4575_9094_FA5D8C98BE63__INCLUDED_)
#define AFX_SPLITTERWNDEX_H__68D1F449_015D_4575_9094_FA5D8C98BE63__INCLUDED_


class CSplitterWndEx : public CSplitterWnd  
{
	DECLARE_DYNCREATE(CSplitterWndEx)

public:
	CSplitterWndEx();
	virtual ~CSplitterWndEx();
	void LockBar(BOOL bState=TRUE){m_bBarLocked=bState;};
	void ResizablePanes(BOOL bState=TRUE){m_bResizePanes=bState;};
	void AutoResizePanes(BOOL bState=TRUE){m_bAutoResizePanes=bState;};

	int HitTest(CPoint pt) const;

	void EqualizeRows(); 
	void EqualizeCols(); 

	virtual void RecalcLayout();

private:
	BOOL m_bBarLocked;
	BOOL m_bResizePanes;
	BOOL m_bAutoResizePanes;

protected:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SPLITTERWNDEX_H__68D1F449_015D_4575_9094_FA5D8C98BE63__INCLUDED_)
