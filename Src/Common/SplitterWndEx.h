// SplitterWndEx.h: interface for the CSplitterWndEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLITTERWNDEX_H__68D1F449_015D_4575_9094_FA5D8C98BE63__INCLUDED_)
#define AFX_SPLITTERWNDEX_H__68D1F449_015D_4575_9094_FA5D8C98BE63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSplitterWndEx : public CSplitterWnd  
{
public:
	CSplitterWndEx();
	virtual ~CSplitterWndEx();
protected:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SPLITTERWNDEX_H__68D1F449_015D_4575_9094_FA5D8C98BE63__INCLUDED_)
