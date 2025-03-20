//////////////////////////////////////////////////////////////////////
/** 
 * @file  OutputBar.h
 *
 * @brief Declaration of COutputBar class
 */
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "scbarcf.h"


// You must #define this for viewbar to compile properly
#define TViewBarBase CSizingControlBarCF

/**
 * @brief Class for location bar.
 */
class COutputBar : public TViewBarBase
{
	DECLARE_DYNAMIC(COutputBar);
public:
	COutputBar();
	virtual ~COutputBar();
	virtual BOOL Create(
		CWnd* pParentWnd,			// mandatory
		LPCTSTR lpszWindowName = nullptr,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_PANE_FIRST);

	void SetFrameHwnd(HWND hwndFrame);
	void UpdateResources();
protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputBar)
public:
	//}}AFX_VIRTUAL


	//{{AFX_MSG(COutputBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HWND m_hwndFrame; //*< Frame window handle */

};
