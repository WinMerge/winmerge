#include "stdafx.h"
#include "editcmd.h"
#include "splash.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd dialog

BOOL CSplashWnd::Create(CWnd* pParent)
{
	//{{AFX_DATA_INIT(CSplashWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	if (!CDialog::Create(CSplashWnd::IDD, pParent))
	{
		TRACE0("Warning: creation of CSplashWnd dialog failed\n");
		return FALSE;
	}

	return TRUE;
}

BOOL CSplashWnd::OnInitDialog()
{
	CDialog::OnInitDialog();
	CenterWindow();

	// initialize the big icon control
	m_icon.SubclassDlgItem(IDC_BIGICON, this);
	m_icon.SizeToContent();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd message handlers

/////////////////////////////////////////////////////////////////////////////
// CBigIcon

BEGIN_MESSAGE_MAP(CBigIcon, CButton)
	//{{AFX_MSG_MAP(CBigIcon)
	ON_WM_DRAWITEM()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBigIcon message handlers

#define CY_SHADOW   4
#define CX_SHADOW   4

void CBigIcon::SizeToContent()
{
	m_bitmap.LoadBitmap(IDB_BITMAP48);
	BITMAP bm;
	m_bitmap.GetObject(sizeof(bm), &bm);
	m_sizeBitmap = CSize(bm.bmWidth, bm.bmHeight);
	// get system icon size

	// a big icon should be twice the size of an icon + shadows
	SetWindowPos(NULL, 0, 0, bm.bmWidth + CX_SHADOW + 4, bm.bmHeight + CY_SHADOW + 4,
		SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
}

void CBigIcon::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	ASSERT(pDC != NULL);

	CRect rect;
	GetClientRect(rect);
	int cxClient = rect.Width();
	int cyClient = rect.Height();

	// draw border around icon
	CPen pen;
	pen.CreateStockObject(BLACK_PEN);
	CPen* pPenOld = pDC->SelectObject(&pen);
	pDC->Rectangle(0, 0, cxClient-CX_SHADOW, cyClient-CY_SHADOW);
	if (pPenOld)
		pDC->SelectObject(pPenOld);

	// draw shadows around icon
	CBrush br;
	br.CreateStockObject(DKGRAY_BRUSH);
	rect.SetRect(cxClient-CX_SHADOW, CY_SHADOW, cxClient, cyClient);
	pDC->FillRect(rect, &br);
	rect.SetRect(CX_SHADOW, cyClient-CY_SHADOW, cxClient, cyClient);
	pDC->FillRect(rect, &br);

	// draw the bitmap contents
	CDC dcMem;
	if (!dcMem.CreateCompatibleDC(pDC))
		return;
	CBitmap* pBitmapOld = dcMem.SelectObject(&m_bitmap);
	if (pBitmapOld == NULL)
		return;

	pDC->BitBlt(2, 2, m_sizeBitmap.cx, m_sizeBitmap.cy, &dcMem, 0, 0, SRCCOPY);

	dcMem.SelectObject(pBitmapOld);
}

BOOL CBigIcon::OnEraseBkgnd(CDC*)
{
	return TRUE;    // we don't do any erasing...
}

/////////////////////////////////////////////////////////////////////////////
