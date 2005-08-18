/**
 * @file  ToolBarXPThemes.cpp
 *
 * @brief Implementation of the ToolBarXPThemes class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "ToolBarXPThemes.h"

BEGIN_MESSAGE_MAP(ToolBarXPThemes, CToolBar)
	//{{AFX_MSG_MAP(ToolBarXPThemes)
	ON_WM_NCPAINT()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/**
 * @brief Called when non-client area is to be drawn.
 */
void ToolBarXPThemes::OnNcPaint()
{
	EraseNonClient();
}

/**
 * @brief Toolbar Erase non-client area and draw gripper.
 */
void ToolBarXPThemes::EraseNonClient()
{

	// Get window DC that is clipped to the non-client area.
	CWindowDC dc(this);
	CRect rectClient;
	GetClientRect(rectClient);
	CRect rectWindow;
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.ExcludeClipRect(rectClient);

	// Draw the borders in the non-client area.
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	DrawBorders(&dc, rectWindow);

	// Erase the parts that are not drawn.
	dc.IntersectClipRect(rectWindow);
	SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);

	// Draw the gripper in the non-client area.
	DrawGripper(&dc, rectWindow);
}

/**
 * @brief Toolbar paint function.
 */
void ToolBarXPThemes::DoPaint(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	// Paint inside the client area.
	CRect rect;
	GetClientRect(rect);
	DrawBorders(pDC, rect);
	DrawGripper(pDC, rect);
}

/**
 * @brief Draws gripper.
 */
void ToolBarXPThemes::DrawGripper(CDC* pDC, const CRect& rect)
{
	pDC->FillSolidRect( &rect, ::GetSysColor(COLOR_BTNFACE)); // Fill in the background.
	CToolBar::DrawGripper(pDC,rect);
}

/**
 * @brief Dismiss right-mousebutton-up event.
 */
void ToolBarXPThemes::OnRButtonUp(UINT nFlags, CPoint point)
{
	// Empty: just dismiss righ-mouseclick for toolbar
}

/**
 * @brief Dismiss right-mousebutton-up event.
 */
void ToolBarXPThemes::OnRButtonDown(UINT nFlags, CPoint point)
{
	// Empty: just dismiss righ-mouseclick for toolbar
}
