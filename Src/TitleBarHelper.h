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
	~CTitleBarHelper();
	void Init(CWnd* pWnd);
	void DrawIcon(CWnd* pWnd, CDC& dc, bool active);
	void DrawButtons(CDC& dc, COLORREF textColor, COLORREF backColor);
	int GetTopMargin() const;
	int GetLeftMargin() const { return PointToPixel(m_leftMargin); }
	int GetRightMargin() const { return PointToPixel(m_rightMargin); }
	CRect GetButtonRect(int button) const;
	void SetMaximized(bool maximized) { m_maximized = maximized; }
	bool GetMaximized() const { return m_maximized; }
	void SetSize(int cx, int cy);
	static COLORREF GetBackColor(bool bActive);
	static COLORREF GetTextColor(bool bActive);
	static void ReloadAccentColor();
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
	HICON CreateGrayIcon(HICON hIcon);
	HICON LazyLoadIcon(CWnd* pWnd, bool active);

	CWnd* m_pWnd;
	CSize m_size;
	bool m_maximized;
	bool m_bMouseTracking;
	int m_nTrackingButton;
	int m_dpi;
	int m_nPressedButton;
	float m_leftMargin;
	float m_rightMargin;
	HICON m_icon;
	HICON m_icon_gray;
};
