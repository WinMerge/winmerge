/** 
 * @file  CommandBar.h
 *
 * @brief Declaration file for CCommandBar
 *
 */
#pragma once

#include <afxwin.h>
#include <afxext.h>
#include <vector>

class CCommandBar : public CToolBar
{
	DECLARE_DYNAMIC(CCommandBar)
public:
	constexpr static int FIRST_MENUID = 10000;

	CCommandBar();

	virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP, UINT nID = AFX_IDW_TOOLBAR);
	bool AttachMenu(CMenu* pMenu);
	void OnCommandBarMenuItem(UINT nID);
	void OnUpdateCommandBarMenuItem(CCmdUI* pCmdUI);
	BOOL PreTranslateMessage(MSG* pMsg);

protected:
	//{{AFX_MSG(CCommandBar)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
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
	void LoseFocus();

	HMENU m_hMenu;
	bool m_bActive;
	bool m_bMouseTracking;
	bool m_bShowKeyboardCues;
	int m_nMDIButtonDown;
	HWND m_hwndOldFocus;
	int m_nCurrentHotItem;
	UINT m_nCurrentMenuItemFlags;
	HMENU m_hCurrentPopupMenu;
	static HHOOK m_hHook;
	static CCommandBar* m_pThis;
};