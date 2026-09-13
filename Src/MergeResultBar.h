/**
 * @file  MergeResultBar.h
 *
 * @brief Declaration of CMergeResultBar class
 */
#pragma once

#include "scbarcf.h"

// You must #define this for viewbar to compile properly
#define TViewBarBase CSizingControlBarCF

/**
 * @brief Dockable bar hosting the kdiff3-style merge result view.
 */
class CMergeResultBar : public TViewBarBase
{
	DECLARE_DYNAMIC(CMergeResultBar);
public:
	CMergeResultBar();
	virtual ~CMergeResultBar();
	virtual BOOL Create(
		CWnd* pParentWnd,			// mandatory
		LPCTSTR lpszWindowName = nullptr,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_PANE_FIRST);

	void SetFrameHwnd(HWND hwndFrame);
	void UpdateResources();

	//{{AFX_MSG(CMergeResultBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HWND m_hwndFrame; /**< Frame window handle */
};
