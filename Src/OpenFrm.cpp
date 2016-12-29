// ChildFrm.cpp : implementation of the COpenFrame class
//
#include "stdafx.h"
#include "OpenFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenFrame

IMPLEMENT_DYNCREATE(COpenFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(COpenFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// COpenFrame construction/destruction

COpenFrame::COpenFrame()
{
	// TODO: add member initialization code here
}

COpenFrame::~COpenFrame()
{
}


BOOL COpenFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

////////////////
// Recalculate the layout of the view.
// Move the view to the center of the window.
// Paul DiLascia described how to center CFormView in MSJ (Oct. 1996)
// http://www.microsoft.com/msj/archive/S1F64.aspx
//
void COpenFrame::RecalcLayout(BOOL bNotify)
{
	// First, let MFC recalculate the layout as per the 
	// normal thing. MFC will figure out where to place 
	// the view, taking into account which status bars 
	// are displayed, etc.
	//
	CFrameWnd::RecalcLayout(bNotify);   

	CScrollView* pView = (CScrollView*)GetActiveView();
	if (pView) {
		// Move form view to center of main 
		// window if it's smaller.
		//
		CRect rcNormalView;
		pView->GetWindowRect(&m_rcNormalView);
		ScreenToClient(&m_rcNormalView);    // view rect MFC would use
		CSize sz = pView->GetTotalSize();   // length, width
		CRect rc = m_rcNormalView;
		if (rc.Width() > sz.cx)                   // if window > form:
			rc.left += (rc.Width() -sz.cx) >> 1;   // move over half the extra
		else
			rc.left = rc.Width() - sz.cx;
		if (rc.Height() > sz.cy)                  // ditto for height
			rc.top  += (rc.Height()-sz.cy) >> 1;   // ...
		else
			rc.top = rc.Height() - sz.cy;

		// This actually moves the view window
		pView->SetWindowPos(NULL, rc.left, rc.top, sz.cx, sz.cy,
			SWP_NOACTIVATE|SWP_NOZORDER);
	}
}

//////////////////
// Moving the view/form to the center of the main
// frame window will leave extra white space in upper 
// left corner. To fix this, need to paint two rectangles.
// Paul DiLascia described how to center CFormView in MSJ (Oct. 1996)
// http://www.microsoft.com/msj/archive/S1F64.aspx
//
void COpenFrame::OnPaint() 
{
   CView* pView = GetActiveView();
   if (pView) {
      CPaintDC dc(this);

      // Easier to use HBRUSH than create CBrush here.
      // Note: be sure to use COLOR_3DFACE to get the
      // right logical color, in case user has customized it.
      //
      HBRUSH hOldBrush = (HBRUSH)::SelectObject(dc.m_hDC, 
         GetSysColorBrush(COLOR_APPWORKSPACE));

	  CRect rc, rcFrame;
	  GetClientRect(&rcFrame);
	  rc = rcFrame;

      CRect rcView;                    // actual view pos (we moved it)
      pView->GetWindowRect(&rcView);   // ...
      ScreenToClient(&rcView);         // ...

      // paint horz rectangle along top edge
      rc.bottom = rcView.top;
      dc.PatBlt(rc.left,rc.top,rc.Width(),rc.Height(),PATCOPY);

      // paint vert rectangle along left side
      rc.bottom = rcView.bottom;
      rc.right  = rcView.left;
      dc.PatBlt(rc.left,rc.top,rc.Width(),rc.Height(),PATCOPY);

      // paint horz rectangle along right edge
      rc.left = rcView.left + rcView.Width();
      rc.right = rcFrame.right;
      rc.bottom = rcFrame.bottom;
      dc.PatBlt(rc.left,rc.top,rc.Width(),rc.Height(),PATCOPY);

      // paint horz rectangle along bottom edge
      rc.left = rcFrame.left;
      rc.right = rcFrame.right;
      rc.top = rcView.top + rcView.Height();
      rc.bottom = rcFrame.bottom;
      dc.PatBlt(rc.left,rc.top,rc.Width(),rc.Height(),PATCOPY);

      ::SelectObject(dc.m_hDC, hOldBrush);
   }
   CMDIChildWnd::OnPaint();
}

void COpenFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
	// [Fix for MFC 8.0 MDI Maximizing Child Window bug on Vista]
	// https://groups.google.com/forum/#!topic/microsoft.public.vc.mfc/iajCdW5DzTM
#if _MFC_VER >= 0x0800
	lpMMI->ptMaxTrackSize.x = max(lpMMI->ptMaxTrackSize.x, lpMMI->ptMaxSize.x);
	lpMMI->ptMaxTrackSize.y = max(lpMMI->ptMaxTrackSize.y, lpMMI->ptMaxSize.y);
#endif
}

void COpenFrame::ActivateFrame(int nCmdShow) 
{
	if (!GetMDIFrame()->MDIGetActive() && AfxGetApp()->GetProfileInt(_T("Settings"), _T("ActiveFrameMax"), TRUE))
	{
		nCmdShow = SW_SHOWMAXIMIZED;
	}
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void COpenFrame::UpdateResources()
{
}

/**
 * @brief Save the window's position, free related resources, and destroy the window
 */
BOOL COpenFrame::DestroyWindow() 
{
	// If we are active, save the restored/maximized state
	// If we are not, do nothing and let the active frame do the job.
 	if (GetParentFrame()->GetActiveFrame() == this)
	{
		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(&wp);
		AfxGetApp()->WriteProfileInt(_T("Settings"), _T("ActiveFrameMax"), (wp.showCmd == SW_MAXIMIZE));
	}

	return CMDIChildWnd::DestroyWindow();
}

// COpenFrame message handlers
