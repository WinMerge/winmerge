/** 
 * @file  MDITabBar.h
 *
 * @brief Declaration file for MDITabBar
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef MDITABBAR_H
#define MDITABBAR_H

/**
 * @brief Class for Tab bar.
 */
class CMDITabBar : public CControlBar
{
	DECLARE_DYNAMIC(CMDITabBar)

private:
	BOOL m_bInSelchange;
	CMDIFrameWnd *m_pMainFrame;

public:
	CMDITabBar() : m_bInSelchange(FALSE), m_pMainFrame(NULL) {}
	virtual ~CMDITabBar() {}
	BOOL Create(CMDIFrameWnd* pParentWnd);
	void UpdateTabs();

	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler) {}
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

	int GetItemCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TCM_GETITEMCOUNT, 0, 0L); }
	BOOL GetItem(int nItem, TCITEM* pTabCtrlItem) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_GETITEM, nItem, (LPARAM)pTabCtrlItem); }
	BOOL SetItem(int nItem, TCITEM* pTabCtrlItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_SETITEM, nItem, (LPARAM)pTabCtrlItem); }
	BOOL InsertItem(int nItem, TCITEM* pTabCtrlItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_INSERTITEM, nItem, (LPARAM)pTabCtrlItem); }
	BOOL DeleteItem(int nItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TCM_DELETEITEM, nItem, 0L); }
	int GetCurSel() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TCM_GETCURSEL, 0, 0L); }
	int SetCurSel(int nItem)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TCM_SETCURSEL, nItem, 0L); }
	int HitTest(TCHITTESTINFO* pHitTestInfo) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, TCM_HITTEST, 0, (LPARAM) pHitTestInfo); }

protected:
	//{{AFX_MSG(CMDITabBar)
	afx_msg void OnPaint();
	afx_msg BOOL OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // MDITABBAR_H
