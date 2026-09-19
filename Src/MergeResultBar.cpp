/**
 * @file  MergeResultBar.cpp
 *
 * @brief Implementation file for CMergeResultBar
 *
 */

#include "stdafx.h"
#include "MergeResultBar.h"
#include "MergeResultContainer.h"
#include "MergeResultView.h"
#include "MergeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMergeResultBar, TViewBarBase);

CMergeResultBar::CMergeResultBar()
: m_hwndFrame(nullptr)
, m_pCreateContext(nullptr)
, m_pDoc(nullptr)
, m_pContainer(nullptr)
{
}

CMergeResultBar::~CMergeResultBar()
{
}

BEGIN_MESSAGE_MAP(CMergeResultBar, TViewBarBase)
	//{{AFX_MSG_MAP(CMergeResultBar)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/**
* @brief Just create ourself
*
* @note The controls are created in the parent frame CMergeEditFrame
*
*/
BOOL CMergeResultBar::Create(
	CWnd* pParentWnd,
	LPCTSTR lpszWindowName /*= nullptr*/,
	DWORD dwStyle /*= WS_CHILD | WS_VISIBLE | CBRS_TOP*/,
	UINT nID /*= AFX_IDW_PANE_FIRST*/,
	CCreateContext* pContext /*= nullptr*/,
	CMergeDoc* pDoc /*= nullptr*/)
{
	// Store context and document before calling TViewBarBase::Create
	// This ensures they're available in OnCreate()
	m_pCreateContext = pContext;
	m_pDoc = pDoc;

	return TViewBarBase::Create(
		lpszWindowName,
		pParentWnd,
		nID,
		dwStyle);
}

int CMergeResultBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (TViewBarBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(SCBS_EDGETOP | SCBS_EDGEBOTTOM | SCBS_SIZECHILD);

	// Create the container that holds both the result view and status bar
	m_pContainer.reset(new CMergeResultContainer());
	if (!m_pContainer->Create(this, m_pDoc))
	{
		TRACE0("Failed to create merge result container\n");
		m_pContainer.reset();
		return -1;
	}

	// Create the result view inside the container
	if (m_pCreateContext != nullptr && m_pDoc != nullptr)
	{
		CMergeResultView* pView = new CMergeResultView();
		DWORD dwStyle = AFX_WS_DEFAULT_VIEW & ~WS_BORDER;
		if (!pView->Create(nullptr, nullptr, dwStyle, CRect(0,0,1,1), m_pContainer.get(), 154, m_pCreateContext))
		{
			TRACE0("Failed to create CMergeResultView\n");
			delete pView;
			return -1;
		}
		m_pDoc->SetMergeResultView(pView);
	}

	return 0;
}

/**
* @note The window must always be docked after movement
* there are too much troubles if we get reparented to some minidockbar
*
*/
void CMergeResultBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	TViewBarBase::OnLButtonDown(nFlags, point);
	if (m_pDockBar != nullptr)
	{
		if (!IsHorzDocked())
			m_pDockContext->ToggleDocking();
	}
}

/**
 * @brief Informs parent frame (CMergeEditFrame) when bar is closed.
 *
 * After bar is closed parent frame saves bar states.
 */
void CMergeResultBar::OnWindowPosChanged(WINDOWPOS* lpwndpos)
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
void CMergeResultBar::SetFrameHwnd(HWND hwndFrame)
{
	m_hwndFrame = hwndFrame;
}

/**
 * @brief Stores create context for MFC view creation.
 */
void CMergeResultBar::SetCreateContext(CCreateContext* pContext)
{
	m_pCreateContext = pContext;
}

/**
 * @brief Stores reference to merge document.
 */
void CMergeResultBar::SetMergeDoc(CMergeDoc* pDoc)
{
	m_pDoc = pDoc;
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMergeResultBar::UpdateResources()
{
	String sCaption = _("Merge Result Pane");
	SetWindowText(sCaption.c_str());
	if (m_pContainer != nullptr)
	{
		m_pContainer->UpdateResources();
	}
}

/**
 * @brief Update conflict statistics information in status bar
 */
void CMergeResultBar::UpdateConflictInfo(const String& sOutputPath, int nConflicts, int nUnresolved, int nWhiteSpaceOnly)
{
	if (m_pContainer != nullptr)
	{
		m_pContainer->UpdateConflictInfo(sOutputPath, nConflicts, nUnresolved, nWhiteSpaceOnly);
	}
}


