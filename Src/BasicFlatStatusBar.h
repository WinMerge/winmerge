/** 
 * @file  BasicFlatStatusBar.h
 *
 * @brief Declaration file for CBasicFlatStatusBar
 *
 */
#pragma once

#include <afxwin.h>
#include <afxext.h>
#include <vector>
#include <map>

#define SBPS_CLICKABLE 0x10000000

struct SubPaneButton
{
	String text;
};

class CBasicFlatStatusBar : public CStatusBar
{
	DECLARE_DYNAMIC(CBasicFlatStatusBar)
public:
	CBasicFlatStatusBar();
	void SetSubPaneButtons(int nIndex, std::vector<SubPaneButton> buttons);
	void GetSubPaneButtonRects(int nIndex, std::vector<CRect>& rects) const;
	int  HitTestSubPaneButton(int nIndex, const CPoint& pt) const;
	HICON GetPaneIcon(int nIndex) const;
	void SetPaneIcon(int nIndex, HICON hIcon);

protected:
	CPoint GetClientCursorPos() const;
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

private:
	std::map<int, std::vector<SubPaneButton>> m_subPaneButtons;
	int m_nHotSubButton = -1;
	std::map<int, HICON> m_paneIcons;
};