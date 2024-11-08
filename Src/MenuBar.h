/** 
 * @file  MenuBar.h
 *
 * @brief Declaration file for CMenuBar
 *
 */
#pragma once

#include <afxwin.h>
#include <afxext.h>
#include <vector>

class CMenuBar : public CToolBar
{
	DECLARE_DYNAMIC(CMenuBar)
public:
	constexpr static int FIRST_MENUID = 10000;
	constexpr static UINT UWM_SHOWPOPUPMENU = WM_APP + 1;
	constexpr static UINT MENUBAR_TIMER_ID = 100;

	CMenuBar();

	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP, UINT nID = AFX_IDW_MENUBAR);
	bool AttachMenu(CMenu* pMenu);
	void OnMenuBarMenuItem(UINT nID);
	void OnUpdateMenuBarMenuItem(CCmdUI* pCmdUI);
	BOOL PreTranslateMessage(MSG* pMsg);
	void SetAlwaysVisible(bool visible) { m_bAlwaysVisible = visible; }

protected:
	//{{AFX_MSG(CMenuBar)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnShowPopupMenu(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	static LRESULT CALLBACK MsgFilterProc(int code, WPARAM wp, LPARAM lp);
	void DrawMDIButtons(HDC hDC);
	int GetMDIButtonIndexFromPoint(CPoint pt) const;
	CRect GetMDIButtonsRect() const;
	CRect GetMDIButtonRect(int nItem) const;
	void ShowKeyboardCues(bool show);
	void LoseFocus();
	void Show(bool visible);

	HMENU m_hMenu;
	bool m_bActive;
	bool m_bAlwaysVisible;
	bool m_bMouseTracking;
	bool m_bShowKeyboardCues;
	int m_nMDIButtonDown;
	int m_nMDIButtonHot;
	HWND m_hwndOldFocus;
	int m_nCurrentHotItem;
	UINT m_nCurrentMenuItemFlags;
	HMENU m_hCurrentPopupMenu;
	CPoint m_ptCurrentCursor;
	static HHOOK m_hHook;
	static CMenuBar* m_pThis;
};