//////////////////////////////////////////////////////////////////////
/** 
 * @file  LocationBar.h
 *
 * @brief Declaration of CLocationBar class
 */
//
//////////////////////////////////////////////////////////////////////

#ifndef LOCATIONBAR_H
#define LOCATIONBAR_H


#include "sizecbar.h"
#include "scbarcf.h"


// You must #define this for viewbar to compile properly
#define TViewBarBase CSizingControlBarCF

/**
 * @brief Class for location bar.
 */
class CLocationBar : public TViewBarBase
{
	DECLARE_DYNAMIC(CLocationBar);
public:
	CLocationBar();
	virtual ~CLocationBar();
	virtual BOOL Create(
		CWnd* pParentWnd,			// mandatory
		LPCTSTR lpszWindowName = NULL,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_PANE_FIRST);

	void SetFrameHwnd(HWND hwndFrame);

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocationBar)
public:
	//}}AFX_VIRTUAL


	//{{AFX_MSG(CLocationBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HWND m_hwndFrame; //*< Frame window handle */

};

#endif // LOCATIONBAR_H
