/** 
 * @file  MDITabBar.h
 *
 * @brief Declaration file for MDITabBar
 *
 */
#pragma once

#include "TitleBarHelper.h"

class CMyTabCtrl : public CTabCtrl
{
	enum { MDITABBAR_MINTITLELENGTH = 8, MDITABBAR_MAXTITLELENGTH = 64 };

	DECLARE_DYNAMIC(CMyTabCtrl)
public:
	CMyTabCtrl()
		: m_bMouseTracking(false)
		, m_bAutoMaxWidth(true)
		, m_pMainFrame(nullptr)
		, m_bCloseButtonDown(false)
		, m_nDraggingTabItemIndex(-1)
		, m_bInSelchange(false)
		, m_nTooltipTabItemIndex(-1)
		, m_bOnTitleBar(false)
		, m_bActive(false)
	{}

protected:
	bool m_bInSelchange;
	bool  m_bAutoMaxWidth;
	bool m_bMouseTracking;
	bool m_bCloseButtonDown;
	bool m_bOnTitleBar;
	bool m_bActive;
	CRect m_rcCurrentCloseButtom;
	int   m_nDraggingTabItemIndex;
	int   m_nTooltipTabItemIndex;	/**< Index of the tab displaying tooltip */
	CMDIFrameWnd *m_pMainFrame;
	CToolTipCtrl m_tooltips;		/**< Tooltip for the tab */

public:
	BOOL Create(CMDIFrameWnd* pMainFrame, CWnd* pParent);
	bool GetAutoMaxWidth() const { return m_bAutoMaxWidth; }
	void SetAutoMaxWidth(bool bAutoMaxWidth) { m_bAutoMaxWidth = bAutoMaxWidth; }
	void UpdateTabs();
	void SetOnTitleBar(bool onTitleBar) { m_bOnTitleBar = onTitleBar; }
	bool GetActive() const { return m_bActive; }
	void SetActive(bool bActive);
	void ActivateTab(int nTabIndex);
	COLORREF GetBackColor() const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTabCtrl)
	//}}AFX_VIRTUAL
// Implementation
public:
	virtual ~CMyTabCtrl() {}

// Generated message map functions
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//{{AFX_MSG(CMyTabCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CRect GetCloseButtonRect(int nItem);
	int GetItemIndexFromPoint(CPoint pt) const;
	void SwapTabs(int nIndexA, int nIndexB);
	int GetMaxTitleLength() const;
	void UpdateToolTips(int index);
};

/**
 * @brief Class for Tab bar.
 */
class CMDITabBar : public CControlBar
{
	DECLARE_DYNAMIC(CMDITabBar)

private:

	bool m_bOnTitleBar;
	CMyTabCtrl m_tabCtrl;
	CFont m_font;
	CTitleBarHelper m_titleBar;

public:
	CMDITabBar()
		: m_bOnTitleBar(true)
	{}
	virtual ~CMDITabBar() {}
	BOOL Update(bool bOnTitleBar, bool bMaxmized);
	void UpdateActive(bool bActive);
	BOOL Create(CMDIFrameWnd* pParentWnd);
	void UpdateTabs() { m_tabCtrl.UpdateTabs(); }
	bool GetAutoMaxWidth() const { return m_tabCtrl.GetAutoMaxWidth(); }
	void SetAutoMaxWidth(bool bAutoMaxWidth) { m_tabCtrl.SetAutoMaxWidth(bAutoMaxWidth); }
	int GetItemCount() const { return m_tabCtrl.GetItemCount(); }
	int GetItemIndexFromPoint(CPoint point, bool bRelatively) const;
	bool ForwardMouseEventToTabCtrlIfNeeded(CPoint& point, UINT message);

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler) {}
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

protected:

	//{{AFX_MSG(CMDITabBar)
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcMouseLeave();
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
};
