/** 
 * @file  DiffViewBar.h
 *
 * @brief Declaration of CDiffViewBar class
 */
#pragma once

#include "scbarcf.h"

class CSplitterWndEx;

// You must #define this for viewbar to compile properly
#define TViewBarBase CSizingControlBarCF

class CDiffViewBar : public TViewBarBase
{
	DECLARE_DYNAMIC(CDiffViewBar);
public:
	CDiffViewBar();
	virtual ~CDiffViewBar();
	virtual BOOL Create(
		CWnd* pParentWnd,			// mandatory
		LPCTSTR lpszWindowName = nullptr,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_PANE_FIRST);

	void SetFrameHwnd(HWND hwndFrame);
	void UpdateResources();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDiffViewBar)
public:
	//}}AFX_VIRTUAL


	//{{AFX_MSG(CDiffViewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HWND m_hwndFrame; //*< Frame window handle */

};
