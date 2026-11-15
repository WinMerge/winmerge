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
#include <set>

#define SBPS_CLICKABLE 0x10000000
#define SBPS_AUTOWIDTH 0x20000000

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
	void SetPaneGroupCount(int nGroups);
	void SetPaneGroup(int nIndex, int nGroup);
	void SetPaneGroupWidth(int nGroup, int width);
	void BeginPaneLayoutUpdate();
	void EndPaneLayoutUpdate();
	void SetPaneInfo(int nIndex, UINT nID, UINT nStyle, int cxWidth);
	bool HasAutoPaneTextChanged();
	BOOL SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate = TRUE);

protected:
	void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler) override;
	CPoint GetClientCursorPos() const;
	int GetIndexFromPoint(const CPoint& pt) const;
	static COLORREF LightenColor(COLORREF color, double amount);
	void DrawPaneIcon(CDC& dc, int nIndex, const CRect& rcPart, CRect& rcText) const;

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void UpdateAutoPaneWidths();
	DECLARE_MESSAGE_MAP()

	bool m_bMouseTracking;
	int m_nTrackingPane;

private:
	struct PaneGroup
	{
		int width;
		std::vector<int> panes;
	};
	int MeasurePaneWidth(CDC& dc, CStatusBarCtrl& ctrl, int nIndex, int textPadding) const;
	std::vector<int> BuildPaneEdges(CDC& dc, CStatusBarCtrl& ctrl, int count, int available, int textPadding) const;
	void ApplyPaneEdges(CStatusBarCtrl& ctrl, std::vector<int> edges);
	std::map<int, std::vector<SubPaneButton>> m_subPaneButtons;
	int m_nHotSubButton = -1;
	std::map<int, HICON> m_paneIcons;
	std::set<int> m_autoWidthPanes;
	std::map<int, CString> m_autoPaneTextCache;
	std::map<int, int> m_paneGroups;
	std::vector<int> m_groupWidths;
	int m_paneLayoutUpdateDepth = 0;
	bool m_paneLayoutDirty = false;
};
