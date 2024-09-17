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
	CTitleBarHelper() {}
	void DrawIcon(CWnd* pWnd, CDC& dc, int leftMarginWidth);
	void DrawButtons(CDC& dc);
	CRect GetIconRect();
	CRect GetButtonsRect();
	void OnSize(CWnd* pWnd, bool maximized, int cx, int cy);
	int HitTest(CPoint pt);

private:
	int PointToPixel(float point);

	CWnd* m_pWnd;
	CSize m_size;
	bool m_maximized;
	int m_iconSize;
	int m_dpi;
};
