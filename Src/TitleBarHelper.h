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
	void DrawButtons(CDC& dc, COLORREF textColor, COLORREF backColor);
	int GetLeftMargin() const { return PointToPixel(m_leftMargin); }
	int GetRightMargin() const { return PointToPixel(m_rightMargin); }
	CRect GetButtonRect(int button) const;
	void OnSize(bool maximized, int cx, int cy);
	LRESULT OnNcHitTest(CPoint pt);
	void OnNcMouseMove(UINT nHitTest, CPoint point);
	void OnNcMouseLeave();
	void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	void OnNcLButtonDown(UINT nHitTest, CPoint point);
	void OnNcLButtonUp(UINT nHitTest, CPoint point);
	void OnNcRButtonDown(UINT nHitTest, CPoint point);
	void OnNcRButtonUp(UINT nHitTest, CPoint point);

private:
	float PointToPixelF(float point) const { return point * m_dpi / 72.f; }
	int PointToPixel(float point) const { return static_cast<int>(point * m_dpi / 72.f); }

	void ShowSysMenu(CPoint point);
	COLORREF GetIntermediateColor(COLORREF a, COLORREF b, float ratio);

	CWnd* m_pWnd;
	CSize m_size;
	CRect m_rc;
	bool m_maximized;
	bool m_bMouseTracking;
	int m_nTrackingButton;
	int m_dpi;
	unsigned m_nHitTest;
	float m_leftMargin;
	float m_rightMargin;
};
