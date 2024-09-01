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
	void DrawIcon(CDC& dc);
	void DrawButtons(CDC& dc);
	CRect GetIconRect();
	CRect GetButtonsRect();
	void Update(CSize size, bool maxmized);
	int HitTest(CPoint pt);

private:
	CSize m_size;
	int m_iconSize;
};
