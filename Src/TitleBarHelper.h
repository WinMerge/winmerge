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
	void DrawIcon(CWnd* pWnd, CDC& dc);
	void DrawButtons(CDC& dc);
	CRect GetIconRect();
	CRect GetButtonsRect();
	void OnSize(CWnd* pWnd, UINT nType, int cx, int cy);
	int HitTest(CPoint pt);

private:
	CWnd* m_pWnd;
	CSize m_size;
	int m_nType;
	int m_iconSize;
};
