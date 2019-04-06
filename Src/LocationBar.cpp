/** 
 * @file  LocationBar.cpp
 *
 * @brief Implementation file for CLocationBar
 *
 */

#include "stdafx.h"
#include "LocationBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CLocationBar, TViewBarBase);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocationBar::CLocationBar()
: m_hwndFrame(nullptr)
{
}


CLocationBar::~CLocationBar()
{
}


BEGIN_MESSAGE_MAP(CLocationBar, TViewBarBase)
	//{{AFX_MSG_MAP(CRegBar)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocationBar message handlers

/**
* @brief Just create ourself
*
* @note The control are created in the parent frame CMergeEditFrame
*
*/
BOOL CLocationBar::Create(
	CWnd* pParentWnd,
	LPCTSTR lpszWindowName,
	DWORD dwStyle,
	UINT nID)
{
	return TViewBarBase::Create(
		lpszWindowName,
		pParentWnd,
		nID,
		dwStyle);
}


///	Create the frame window associated with the view bar.
int CLocationBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (TViewBarBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(SCBS_EDGELEFT | SCBS_EDGERIGHT | SCBS_SIZECHILD);
	return 0;
}

/**
* @note The window must always be docked after movement
* there are too much troubles if we get reparented to some minidockbar 
*
*/
void CLocationBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	TViewBarBase::OnLButtonDown(nFlags, point);
	if (m_pDockBar != nullptr)
	{
		if (!IsVertDocked())
			m_pDockContext->ToggleDocking();
	}
}

void CLocationBar::OnSize(UINT nType, int cx, int cy) 
{
	TViewBarBase::OnSize(nType, cx, cy);
}

/** 
 * @brief Informs parent frame (CMergeEditFrame) when bar is closed.
 *
 * After bar is closed parent frame saves bar states.
 */
void CLocationBar::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	TViewBarBase::OnWindowPosChanged(lpwndpos);

	if (m_hwndFrame != nullptr)
	{
		// If WINDOWPOS.flags has SWP_HIDEWINDOW flag set
		if ((lpwndpos->flags & SWP_HIDEWINDOW) != 0)
			::PostMessage(m_hwndFrame, MSG_STORE_PANESIZES, 0, 0);
	}
}

/** 
 * @brief Stores HWND of frame window (CMergeEditFrame).
 */
void CLocationBar::SetFrameHwnd(HWND hwndFrame)
{
	m_hwndFrame = hwndFrame;
}
/**
 * @brief Update any resources necessary after a GUI language change
 */
void CLocationBar::UpdateResources()
{
	String sCaption = _("Location Pane");
	SetWindowText(sCaption.c_str());
}
