/** 
 * @file  BasicFlatStatusBar.h
 *
 * @brief Declaration file for CBasicFlatStatusBar
 *
 */
#pragma once

#include <afxwin.h>
#include <afxext.h>

#define SBPS_CLICKABLE 0x10000000

class CBasicFlatStatusBar : public CStatusBar
{
	DECLARE_DYNAMIC(CBasicFlatStatusBar)
public:
	CBasicFlatStatusBar();

protected:
	CPoint CBasicFlatStatusBar::GetClientCursorPos() const;
	int GetIndexFromPoint(const CPoint& pt) const;
	static COLORREF LightenColor(COLORREF color, double amount);

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()

	bool m_bMouseTracking;
	int m_nTrackingPane;
};