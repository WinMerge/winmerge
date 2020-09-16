// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Sample.h"

#include "MainFrm.h"
#include "ceditcmd.h"
#include <commoncontrols.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, DpiAware::CDpiAwareWnd<CMDIFrameWnd>)

BEGIN_MESSAGE_MAP(CMainFrame, DpiAware::CDpiAwareWnd<CMDIFrameWnd>)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	ON_MESSAGE(WM_NCCALCSIZE, OnNcCalcSize)
	ON_MESSAGE(WM_DPICHANGED, OnDpiChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,               // status line indicator 
	ID_EDIT_INDICATOR_POSITION,	 
	ID_EDIT_INDICATOR_COL,
	ID_EDIT_INDICATOR_CRLF,
	ID_INDICATOR_ENCODING,
	ID_INDICATOR_OVR,	
	ID_EDIT_INDICATOR_READ,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!LoadToolBar())
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

LRESULT CMainFrame::OnNcCalcSize(WPARAM wParam, LPARAM lParam)
{
	BOOL bCalcValidRects = wParam;
	NCCALCSIZE_PARAMS* lpncsp = (NCCALCSIZE_PARAMS*)lParam;
	RECT *lprect = (RECT *)lParam;

	LRESULT ret = Default();
	if (bCalcValidRects)
	{
//		lpncsp->rgrc->top -= 10;
		return ret;
	}
	return ret;
}

LRESULT CMainFrame::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	__super::OnDpiChanged(wParam, lParam);
	DpiAware::UpdateAfxDataSysMetrics(GetDpi());
	LoadToolBar();
	const RECT* pRect = reinterpret_cast<RECT *>(lParam);
	SetWindowPos(nullptr, pRect->left, pRect->top,
		pRect->right - pRect->left,
		pRect->bottom - pRect->top, SWP_NOZORDER | SWP_NOACTIVATE);
	Default();
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return __super::PreCreateWindow(cs);
}

BOOL CMainFrame::LoadToolBar()
{
	m_wndToolBar.LoadToolBar(IDR_MAINFRAME);
	CToolBarCtrl& toolbarCtrl = m_wndToolBar.GetToolBarCtrl();
	int cx = 16;
	int cy = 15;
	m_imgListToolBar.DeleteImageList();
	m_imgListToolBar.Create(IDR_MAINFRAME, cx, 0, RGB(192, 192, 192));
	CComQIPtr<IImageList2> pImageList2(reinterpret_cast<IImageList *>(m_imgListToolBar.m_hImageList));
	if (pImageList2)
	{
		cx = MulDiv(16, m_dpi, USER_DEFAULT_SCREEN_DPI);
		cy = MulDiv(15, m_dpi, USER_DEFAULT_SCREEN_DPI);
		HRESULT hr = pImageList2->Resize(cx, cy);
	}
	toolbarCtrl.SetImageList(&m_imgListToolBar);
	toolbarCtrl.SetButtonSize({ cx + 8, cy + 8 });
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	__super::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
