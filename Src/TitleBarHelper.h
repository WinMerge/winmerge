/** 
 * @file  TitleBarHelper.h
 *
 * @brief Declaration file for CTitleBarHelper
 *
 */
#pragma once

#include <afxwin.h>
#include <afxext.h>
#include <vector>

class CTitleBarHelper {
public:
	CTitleBarHelper();
	void Init(CWnd* pWnd);
	void DrawIcon(CWnd* pWnd, CDC& dc);
	void DrawButtons(CDC& dc);
	int GetLeftMargin() const { return static_cast<int>(PointToPixel(m_leftMargin)); }
	int GetRightMargin() const { return static_cast<int>(PointToPixel(m_rightMargin)); }
	CRect GetButtonRect(int button) const;
	void OnSize(bool maximized, int cx, int cy);
	void OnNcMouseMove(UINT nHitTest, CPoint point);
	void OnNcMouseLeave();
	void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	void OnNcLButtonDown(UINT nHitTest, CPoint point);
	void OnNcLButtonUp(UINT nHitTest, CPoint point);
	int HitTest(CPoint pt);

private:
	float PointToPixel(float point) const;

	CWnd* m_pWnd;
	CSize m_size;
	CRect m_rc;
	bool m_maximized;
	bool m_bMouseTracking;
	int m_nTrackingButton;
	int m_nHitTest;
	int m_iconSize;
	int m_dpi;
	float m_leftMargin;
	float m_rightMargin;
};
