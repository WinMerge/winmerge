//////////////////////////////////////////////////////////////////////
/** 
 * @file  SplitterWndEx.h
 *
 * @brief Declaration of CSplitterWndEx class
 */
// RCS ID line follows -- this is updated by CVS
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

	int HitTest(CPoint pt) const;

	void EqualizeRows(); 
	void EqualizeCols(); 

private:
	BOOL m_bBarLocked;
	BOOL m_bResizePanes;

protected:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SPLITTERWNDEX_H__68D1F449_015D_4575_9094_FA5D8C98BE63__INCLUDED_)
