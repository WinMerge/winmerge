// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "Sample.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	ON_MESSAGE(WM_DPICHANGED_BEFOREPARENT, OnDpiChangedBeforeParent)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIChildWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	return m_wndSplitter.Create(this, 2, 2, CSize(30, 30), pContext);
}

void CChildFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	__super::OnGetMinMaxInfo(lpMMI);
	if (IsDpiChanged())
	{
		CRect rc;
		CFrameWnd* pFrameWnd = GetParentFrame();
		pFrameWnd->GetClientRect(rc);
		AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());
		lpMMI->ptMaxPosition.x = rc.left;
		lpMMI->ptMaxPosition.y = rc.top;
		lpMMI->ptMaxSize.x = rc.right - rc.left;
		lpMMI->ptMaxSize.y = rc.bottom - rc.top;
	}
}

LRESULT CChildFrame::OnDpiChangedBeforeParent(WPARAM wParam, LPARAM lParam)
{
	UpdateDpi();
	return 0;
}
