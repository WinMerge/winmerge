// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/**
 * @file  BasicFlatStatusBar.cpp
 *
 * @brief Implementation of the CBasicFlatStatusBar class
 */

#include "StdAfx.h"
#include "BasicFlatStatusBar.h"
#include "RoundedRectWithShadow.h"

IMPLEMENT_DYNAMIC(CBasicFlatStatusBar, CStatusBar)

BEGIN_MESSAGE_MAP(CBasicFlatStatusBar, CStatusBar)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CBasicFlatStatusBar::CBasicFlatStatusBar() : m_bMouseTracking(false), m_nTrackingPane(-1)
{
}

CPoint CBasicFlatStatusBar::GetClientCursorPos() const
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	return pt;
}

int CBasicFlatStatusBar::GetIndexFromPoint(const CPoint& pt) const
{
	CStatusBarCtrl& ctrl = GetStatusBarCtrl();
	int parts[32];
	const int nParts = ctrl.GetParts(32, parts);
	for (int i = 0; i < nParts; i++)
	{
		CRect rcPart;
		ctrl.GetRect(i, &rcPart);
		if (PtInRect(rcPart, pt))
			return i;
	}
	return -1;
}

COLORREF CBasicFlatStatusBar::LightenColor(COLORREF color, double amount)
{
	BYTE red = GetRValue(color);
	BYTE green = GetGValue(color);
	BYTE blue = GetBValue(color);
	red = static_cast<BYTE>(red + (255 - red) * amount);
	green = static_cast<BYTE>(green + (255 - green) * amount);
	blue = static_cast<BYTE>(blue + (255 - blue) * amount);
	return RGB(red, green, blue);
}

HICON CBasicFlatStatusBar::GetPaneIcon(int nIndex) const
{
	return m_paneIcons.count(nIndex) ? m_paneIcons.at(nIndex) : nullptr;
}

void CBasicFlatStatusBar::SetPaneIcon(int nIndex, HICON hIcon)
{
	if (hIcon)
		m_paneIcons[nIndex] = hIcon;
	else
		m_paneIcons.erase(nIndex);
	Invalidate();
	UpdateAutoPaneWidths();
}

void CBasicFlatStatusBar::SetPaneInfo(int nIndex, UINT nID, UINT nStyle, int cxWidth)
{
	const bool autoWidth = (nStyle & SBPS_AUTOWIDTH) != 0 && (nStyle & SBPS_STRETCH) == 0;
	nStyle &= ~SBPS_AUTOWIDTH;
	CStatusBar::SetPaneInfo(nIndex, nID, nStyle, cxWidth);
	if (autoWidth)
	{
		m_autoWidthPanes.insert(nIndex);
		m_autoPaneTextCache[nIndex] = GetStatusBarCtrl().GetText(nIndex);
	}
	else
	{
		m_autoWidthPanes.erase(static_cast<int>(nIndex));
		m_autoPaneTextCache.erase(static_cast<int>(nIndex));
	}
	UpdateAutoPaneWidths();
}

void CBasicFlatStatusBar::BeginPaneLayoutUpdate()
{
	++m_paneLayoutUpdateDepth;
}

void CBasicFlatStatusBar::EndPaneLayoutUpdate()
{
	if (m_paneLayoutUpdateDepth <= 0)
		return;
	if (--m_paneLayoutUpdateDepth == 0 && m_paneLayoutDirty)
	{
		m_paneLayoutDirty = false;
		UpdateAutoPaneWidths();
	}
}

void CBasicFlatStatusBar::SetPaneGroupCount(int nGroups)
{
	m_groupWidths.assign((std::max)(0, nGroups), 0);
	UpdateAutoPaneWidths();
}

void CBasicFlatStatusBar::SetPaneGroup(int nIndex, int nGroup)
{
	if (nGroup < 0 || nGroup >= static_cast<int>(m_groupWidths.size()))
		m_paneGroups.erase(nIndex);
	else
		m_paneGroups[nIndex] = nGroup;
	UpdateAutoPaneWidths();
}

void CBasicFlatStatusBar::SetPaneGroupWidth(int nGroup, int width)
{
	if (nGroup < 0 || nGroup >= static_cast<int>(m_groupWidths.size()))
		return;
	m_groupWidths[nGroup] = (std::max)(0, width);
	UpdateAutoPaneWidths();
}

BOOL CBasicFlatStatusBar::SetPaneText(int nIndex, LPCTSTR lpszNewText, BOOL bUpdate)
{
	const CString previousText = CStatusBar::GetPaneText(nIndex);
	const BOOL result = CStatusBar::SetPaneText(nIndex, lpszNewText, bUpdate);
	if (result && m_autoWidthPanes.count(nIndex) && previousText != lpszNewText)
	{
		m_autoPaneTextCache[nIndex] = lpszNewText;
		UpdateAutoPaneWidths();
	}
	return result;
}

bool CBasicFlatStatusBar::HasAutoPaneTextChanged()
{
	CStatusBarCtrl& ctrl = GetStatusBarCtrl();
	bool changed = false;
	for (const int pane : m_autoWidthPanes)
	{
		const CString text = ctrl.GetText(pane);
		auto cached = m_autoPaneTextCache.find(pane);
		if (cached == m_autoPaneTextCache.end() || cached->second != text)
		{
			m_autoPaneTextCache[pane] = text;
			changed = true;
		}
	}
	return changed;
}

void CBasicFlatStatusBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CStatusBar::OnUpdateCmdUI(pTarget, bDisableIfNoHndler);
	if (HasAutoPaneTextChanged())
		UpdateAutoPaneWidths();
}

void CBasicFlatStatusBar::OnSize(UINT nType, int cx, int cy)
{
	CStatusBar::OnSize(nType, cx, cy);
	UpdateAutoPaneWidths();
}

int CBasicFlatStatusBar::MeasurePaneWidth(CDC& dc, CStatusBarCtrl& ctrl, int nIndex, int textPadding) const
{
	CString text = ctrl.GetText(nIndex);
	if (text.Find(_T('\t')) >= 0)
	{
		text.Trim();
		text.Replace(_T("\t"), _T("  "));
	}
	int width = static_cast<int>(dc.GetTextExtent(text, text.GetLength()).cx) + textPadding;

	const int iconSize = GetSystemMetrics(SM_CXSMICON);
	const int iconWidth = m_paneIcons.count(nIndex) ? iconSize + MulDiv(4, iconSize, 16) : 0;
	width += iconWidth;

	auto buttons = m_subPaneButtons.find(nIndex);
	if (nIndex == m_nTrackingPane && buttons != m_subPaneButtons.end() && !buttons->second.empty())
	{
		int maxButtonTextWidth = 0;
		for (const auto& button : buttons->second)
		{
			const CString buttonText(button.text.c_str());
			maxButtonTextWidth = (std::max)(maxButtonTextWidth,
				static_cast<int>(dc.GetTextExtent(buttonText, buttonText.GetLength()).cx));
		}
		const int buttonsWidth = iconWidth + static_cast<int>(buttons->second.size())
			* (maxButtonTextWidth + textPadding);
		width = (std::max)(width, buttonsWidth);
	}
	return width;
}

std::vector<int> CBasicFlatStatusBar::BuildPaneEdges(CDC& dc, CStatusBarCtrl& ctrl,
	int count, int available, int textPadding) const
{
	const bool grouped = !m_groupWidths.empty() && !m_paneGroups.empty();
	std::vector<PaneGroup> groups(grouped ? m_groupWidths.size() : 1);
	for (size_t i = 0; i < groups.size(); ++i)
		groups[i].width = grouped ? m_groupWidths[i] : available;
	for (int pane = 0; pane < count; ++pane)
	{
		int groupIndex = 0;
		if (grouped)
		{
			auto group = m_paneGroups.find(pane);
			if (group == m_paneGroups.end())
				continue;
			groupIndex = group->second;
		}
		if (groupIndex >= 0 && groupIndex < static_cast<int>(groups.size()))
			groups[groupIndex].panes.push_back(pane);
	}
	std::vector<int> edges(count);
	int edge = 0;
	for (const PaneGroup& group : groups)
	{
		if (group.panes.empty())
			continue;
		std::vector<int> paneWidths(group.panes.size());
		std::vector<int> minWidths(group.panes.size());
		int fillPane = -1;
		int desiredTotal = 0;
		for (size_t p = 0; p < group.panes.size(); ++p)
		{
			const int pane = group.panes[p];
			UINT id = 0, style = 0;
			int baseWidth = 0;
			GetPaneInfo(pane, id, style, baseWidth);
			const bool autoWidth = m_autoWidthPanes.count(pane) != 0;
			const bool stretch = (style & SBPS_STRETCH) != 0;
			const bool fill = stretch || (grouped && !autoWidth);
			if (fill)
				fillPane = static_cast<int>(p);

			int wanted = baseWidth;
			if (autoWidth || (!grouped && !stretch && baseWidth > 0))
				wanted = (std::max)(wanted, MeasurePaneWidth(dc, ctrl, pane, textPadding));
			minWidths[p] = fill ? 0 : (autoWidth ? baseWidth : wanted);
			paneWidths[p] = wanted;
			desiredTotal += wanted;
		}

		const int groupWidth = (std::min)(group.width, (std::max)(0, available - edge));
		if (desiredTotal < groupWidth && fillPane >= 0)
		{
			paneWidths[fillPane] += groupWidth - desiredTotal;
			desiredTotal = groupWidth;
		}
		if (desiredTotal > groupWidth)
		{
			int excess = desiredTotal - groupWidth;
			// Reduce content-driven growth before shrinking configured minima.
			for (size_t p = 0; p < group.panes.size() && excess > 0; ++p)
			{
				const int reduction = (std::min)((std::max)(0, paneWidths[p] - minWidths[p]), excess);
				paneWidths[p] -= reduction;
				excess -= reduction;
			}
			// When minima do not fit, shrink auto-width panes first.
			for (size_t p = 0; p < group.panes.size() && excess > 0; ++p)
			{
				if (!m_autoWidthPanes.count(group.panes[p]))
					continue;
				const int reduction = (std::min)(paneWidths[p], excess);
				paneWidths[p] -= reduction;
				excess -= reduction;
			}
			for (size_t p = 0; p < group.panes.size() && excess > 0; ++p)
			{
				if (m_autoWidthPanes.count(group.panes[p]))
					continue;
				const int reduction = (std::min)(paneWidths[p], excess);
				paneWidths[p] -= reduction;
				excess -= reduction;
			}
		}

		int localEdge = edge;
		for (size_t p = 0; p < group.panes.size(); ++p)
		{
			localEdge += paneWidths[p];
			edges[group.panes[p]] = localEdge;
		}
		edge += groupWidth;
		edges[group.panes.back()] = edge;
	}
	if (count > 0)
		edges.back() = available;
	return edges;
}

void CBasicFlatStatusBar::ApplyPaneEdges(CStatusBarCtrl& ctrl, std::vector<int> edges)
{
	if (edges.empty())
		return;
	std::vector<int> currentParts(edges.size());
	// Avoid resetting the native parts when their boundaries are unchanged.
	if (ctrl.GetParts(static_cast<int>(edges.size()), currentParts.data()) != static_cast<int>(edges.size())
		|| currentParts != edges)
	{
		ctrl.SetParts(static_cast<int>(edges.size()), edges.data());
		Invalidate(FALSE);
	}
}

void CBasicFlatStatusBar::UpdateAutoPaneWidths()
{
	if (m_paneLayoutUpdateDepth > 0)
	{
		m_paneLayoutDirty = true;
		return;
	}
	if (!::IsWindow(m_hWnd) || (m_autoWidthPanes.empty() && m_paneGroups.empty()))
		return;
	CStatusBarCtrl& ctrl = GetStatusBarCtrl();
	const int count = ctrl.GetParts(0, nullptr);
	if (count <= 0)
		return;
	CRect client;
	GetClientRect(&client);
	CClientDC measureDC(this);
	CFont* font = GetFont();
	CFont* oldFont = font ? measureDC.SelectObject(font) : nullptr;
	const int textPadding = MulDiv(12, measureDC.GetDeviceCaps(LOGPIXELSX), 72);
	std::vector<int> edges = BuildPaneEdges(measureDC, ctrl, count, client.Width(), textPadding);
	if (oldFont)
		measureDC.SelectObject(oldFont);
	ApplyPaneEdges(ctrl, std::move(edges));
}

void CBasicFlatStatusBar::SetSubPaneButtons(int nIndex, std::vector<SubPaneButton> buttons)
{
	if (buttons.empty())
		m_subPaneButtons.erase(nIndex);
	else
		m_subPaneButtons[nIndex] = std::move(buttons);
	if (nIndex == m_nTrackingPane)
		UpdateAutoPaneWidths();
}

void CBasicFlatStatusBar::GetSubPaneButtonRects(int nIndex, std::vector<CRect>& rects) const
{
	rects.clear();
	auto it = m_subPaneButtons.find(nIndex);
	if (it == m_subPaneButtons.end() || it->second.empty())
		return;

	CRect rcPart;
	GetStatusBarCtrl().GetRect(nIndex, &rcPart);

	auto itIcon = m_paneIcons.find(nIndex);
	if (itIcon != m_paneIcons.end() && itIcon->second)
	{
		const int iconSize = GetSystemMetrics(SM_CXSMICON);
		const int iconWidth = iconSize + MulDiv(4, iconSize, 16);

		// Reserve space for the pane icon.
		rcPart.left += iconWidth;
	}

	const int n = static_cast<int>(it->second.size());
	const int w = rcPart.Width() / n;
	int x = rcPart.left;
	for (int i = 0; i < n; ++i)
	{
		int right = (i == n - 1) ? rcPart.right : x + w;
		rects.emplace_back(x, rcPart.top, right, rcPart.bottom);
		x = right;
	}
}

int CBasicFlatStatusBar::HitTestSubPaneButton(int nIndex, const CPoint& pt) const
{
	std::vector<CRect> rects;
	GetSubPaneButtonRects(nIndex, rects);
	for (size_t i = 0; i < rects.size(); ++i)
		if (rects[i].PtInRect(pt))
			return static_cast<int>(i);
	return -1;
}

void CBasicFlatStatusBar::DrawPaneIcon(CDC& dc, int nIndex, const CRect& rcPart, CRect& rcText) const
{
	auto itIcon = m_paneIcons.find(nIndex);
	if (itIcon != m_paneIcons.end() && itIcon->second)
	{
		const int iconSize = GetSystemMetrics(SM_CXSMICON);
		const int iconY = rcPart.top + (rcPart.Height() - iconSize) / 2;
		::DrawIconEx(dc.m_hDC, rcText.left, iconY, itIcon->second,
			iconSize, iconSize, 0, nullptr, DI_NORMAL);
		rcText.left += iconSize + MulDiv(4, iconSize, 16);
	}
}

void CBasicFlatStatusBar::OnPaint()
{
	const COLORREF clr3DFace = GetSysColor(COLOR_3DFACE);
	const COLORREF clr3DFaceLight = LightenColor(clr3DFace, 0.5);

	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);

	CStatusBarCtrl& ctrl = GetStatusBarCtrl();
	int parts[32];
	const int nParts = ctrl.GetParts(32, parts);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap* pOldBmp = memDC.SelectObject(&bmp);

	memDC.FillSolidRect(&rect, clr3DFace);
	memDC.SetTextColor(GetSysColor(COLOR_BTNTEXT));
	memDC.SetBkMode(TRANSPARENT);

	CFont* pFont = GetFont();
	CFont* pOldFont = pFont ? memDC.SelectObject(pFont) : nullptr;
	const int radius = MulDiv(3, memDC.GetDeviceCaps(LOGPIXELSY), 72);

	for (int i = 0; i < nParts; i++)
	{
		const unsigned style = GetPaneStyle(i);
		CRect rcPart;
		ctrl.GetRect(i, &rcPart);

		auto it = m_subPaneButtons.find(i);
		const bool bShowSubButtons = (i == m_nTrackingPane) && it != m_subPaneButtons.end() && !it->second.empty();

		// Calculate SubPane button rectangles once and reuse them
		// for both background and text drawing.
		std::vector<CRect> subPaneRects;
		if (bShowSubButtons)
			GetSubPaneButtonRects(i, subPaneRects);

		// Draw pane/subpane background.
		if (bShowSubButtons)
		{
			for (size_t b = 0; b < subPaneRects.size(); ++b)
			{
				if (m_nHotSubButton == static_cast<int>(b))
				{
					DrawRoundedRect(memDC.m_hDC, subPaneRects[b].left, subPaneRects[b].top,
						subPaneRects[b].Width(), subPaneRects[b].Height(), radius, clr3DFaceLight, clr3DFace);
				}
			}
		}
		else if (m_bMouseTracking && (style & SBPS_CLICKABLE) != 0 && i == m_nTrackingPane)
		{
			DrawRoundedRect(memDC.m_hDC, rcPart.left, rcPart.top, rcPart.Width(), rcPart.Height(), radius, clr3DFaceLight, clr3DFace);
		}

		if ((style & SBPS_DISABLED) != 0)
			continue;

		// Draw one icon per pane.
		CRect rcText = rcPart;
		rcText.left += radius;
		DrawPaneIcon(memDC, i, rcPart, rcText);

		if (bShowSubButtons)
		{
			for (size_t b = 0; b < subPaneRects.size(); ++b)
			{
				CRect rcButtonText = subPaneRects[b];
				rcButtonText.left += radius;

				memDC.DrawText(it->second[b].text.c_str(), rcButtonText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
		}
		else
		{
			CString text = ctrl.GetText(i);
			if (text.Find('\t') >= 0)
			{
				text.Trim();
				text.Replace(_T("\t"), _T("  "));
			}
			memDC.DrawText(text, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}
	}

	if (pOldFont)
		memDC.SelectObject(pOldFont);

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBmp);
}

BOOL CBasicFlatStatusBar::OnEraseBkgnd(CDC* pDC)
{
	UNREFERENCED_PARAMETER(pDC);
	return TRUE;
}

void CBasicFlatStatusBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bMouseTracking)
	{
		TRACKMOUSEEVENT tme = { sizeof TRACKMOUSEEVENT, TME_LEAVE, m_hWnd };
		TrackMouseEvent(&tme);
		m_bMouseTracking = true;
	}
	int i = GetIndexFromPoint(GetClientCursorPos());

	int nNewHotSubButton = -1;
	if (i >= 0 && m_subPaneButtons.count(i))
		nNewHotSubButton = HitTestSubPaneButton(i, point);

	const int previousTrackingPane = m_nTrackingPane;
	for (int pane : {i, previousTrackingPane})
	{
		if (pane >= 0 && ((GetPaneStyle(pane) & SBPS_CLICKABLE) != 0 || m_subPaneButtons.count(pane)))
		{
			CRect rcPart;
			GetStatusBarCtrl().GetRect(pane, &rcPart);
			InvalidateRect(&rcPart, false);
		}
	}
	m_nHotSubButton = nNewHotSubButton;
	m_nTrackingPane = i;
	if (m_nTrackingPane != previousTrackingPane)
		UpdateAutoPaneWidths();
}

void CBasicFlatStatusBar::OnMouseLeave()
{
	TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_CANCEL, m_hWnd };
	TrackMouseEvent(&tme);
	m_bMouseTracking = false;
	const int previousTrackingPane = m_nTrackingPane;
	if (m_nTrackingPane >= 0 && (GetPaneStyle(m_nTrackingPane) & SBPS_CLICKABLE) != 0)
	{
		CRect rcPart;
		GetStatusBarCtrl().GetRect(m_nTrackingPane, &rcPart);
		InvalidateRect(&rcPart, false);
	}
	m_nTrackingPane = -1;
	if (previousTrackingPane >= 0)
		UpdateAutoPaneWidths();
}

BOOL CBasicFlatStatusBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	const int i = GetIndexFromPoint(GetClientCursorPos());
	LPCTSTR icon = (i >= 0 && (GetPaneStyle(i) & SBPS_CLICKABLE) != 0) ? IDC_HAND : IDC_ARROW;
	::SetCursor(::LoadCursor(nullptr, icon));
	return TRUE;
}
