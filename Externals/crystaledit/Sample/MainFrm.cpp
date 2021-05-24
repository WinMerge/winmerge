// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Sample.h"

#include "MainFrm.h"
#include "ceditcmd.h"
#include "utils/hqbitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
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
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
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

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIFrameWnd::PreCreateWindow(cs);
}

BOOL CMainFrame::LoadToolBar()
{
	const int ICON_COUNT = 17;
	m_wndToolBar.LoadToolBar(IDR_MAINFRAME);
	CToolBarCtrl& toolbarCtrl = m_wndToolBar.GetToolBarCtrl();
	const int cx = MulDiv(16, GetSystemMetrics(SM_CXSMICON), 16);
	const int cy = MulDiv(15, GetSystemMetrics(SM_CYSMICON), 16);
	m_imgListToolBar.Detach();
	m_imgListToolBar.Create(cx, cy, ILC_COLOR32, ICON_COUNT, 0);
	CBitmap bm;
	bm.Attach(LoadBitmapAndConvertTo32bit(AfxGetInstanceHandle(), IDR_MAINFRAME, cx * ICON_COUNT, cy, false, RGB(0xc0, 0xc0, 0xc0)));
	m_imgListToolBar.Add(&bm, nullptr);
	if (CImageList* pImgList = toolbarCtrl.SetImageList(&m_imgListToolBar))
		pImgList->DeleteImageList();
	toolbarCtrl.SetButtonSize({ cx + 8, cy + 8 });
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
