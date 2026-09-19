/**
 * @file  MergeResultContainer.cpp
 *
 * @brief Implementation of CMergeResultContainer class
 */

#include "stdafx.h"
#include "MergeResultContainer.h"
#include "MergeDoc.h"
#include "MergeResultStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMergeResultContainer, CWnd)

BEGIN_MESSAGE_MAP(CMergeResultContainer, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CMergeResultContainer::CMergeResultContainer()
	: m_pDoc(nullptr)
{
}

CMergeResultContainer::~CMergeResultContainer()
{
	// Destroy the window if it still exists
	if (IsWindow(m_hWnd))
	{
		DestroyWindow();
	}
}

BOOL CMergeResultContainer::Create(CWnd* pParentWnd, CMergeDoc* pDoc)
{
	m_pDoc = pDoc;

	// Register and create the window class
	CString wndclass = ::AfxRegisterWndClass(CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW),
		::GetSysColorBrush(COLOR_BTNFACE), 0);

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	if (!CWnd::Create(wndclass, _T("MergeResultContainer"), dwStyle,
		CRect(0, 0, 0, 0), pParentWnd, 0))
		return FALSE;

	return TRUE;
}

int CMergeResultContainer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the status bar first
	if (!m_statusBar.Create(this))
	{
		TRACE0("Failed to create merge result status bar\n");
		return -1;
	}

	// The result view will be created by the frame (MergeEditFrm)
	// via standard MFC document/view mechanism
	// We just prepare the space for it here

	return 0;
}

void CMergeResultContainer::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (!IsWindow(m_statusBar.m_hWnd))
		return;

	// Calculate layout: status bar at bottom
	int nStatusBarHeight = MulDiv(STATUS_BAR_HEIGHT, GetSystemMetrics(SM_CXSMICON), 16);
	if (cy < nStatusBarHeight)
		nStatusBarHeight = cy / 2;

	// Position the result view - look for first child and position it
	CWnd* pView = GetWindow(GW_CHILD);
	while (pView != nullptr && pView->m_hWnd == m_statusBar.m_hWnd)
		pView = pView->GetWindow(GW_HWNDNEXT);

	if (pView != nullptr)
	{
		pView->MoveWindow(0, 0, cx, cy - nStatusBarHeight);
	}

	// Position the status bar at the bottom
	m_statusBar.MoveWindow(0, cy - nStatusBarHeight, cx, nStatusBarHeight);
}

void CMergeResultContainer::UpdateConflictInfo(const String& sOutputPath, int nConflicts, int nUnresolved, int nWhiteSpaceOnly)
{
	if (IsWindow(m_statusBar.m_hWnd))
	{
		m_statusBar.SetPath(sOutputPath);
		m_statusBar.SetConflictInfo(nConflicts, nUnresolved, nWhiteSpaceOnly);
	}
}

void CMergeResultContainer::UpdateResources()
{
	if (IsWindow(m_statusBar.m_hWnd))
	{
		m_statusBar.UpdateResources();
	}
}
