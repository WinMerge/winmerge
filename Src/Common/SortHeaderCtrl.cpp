/**
 *  @file SortHeaderCtrl.cpp
 *
 *  @brief Implementation of CSortHeaderCtrl
 */ 

#include "StdAfx.h"
#include "SortHeaderCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl

IMPLEMENT_DYNAMIC(CSortHeaderCtrl, CHeaderCtrl)

BEGIN_MESSAGE_MAP(CSortHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSortHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSortHeaderCtrl::CSortHeaderCtrl() : m_bSortAsc(true), m_nSortCol(-1), m_bMouseTracking(false), m_nTrackingPane(-1)
{
}

CSortHeaderCtrl::~CSortHeaderCtrl()
{
}

int CSortHeaderCtrl::SetSortImage(int nCol, bool bAsc)
{
	int nPrevCol = m_nSortCol;

	m_nSortCol = nCol;
	m_bSortAsc = bAsc;

	HD_ITEM hditem;

	hditem.mask = HDI_FORMAT;

	// Clear HDF_SORTDOWN and HDF_SORTUP flag in all columns.
	int i;
	for (i = 0; i < this->GetItemCount(); i++)
	{
		GetItem( i, &hditem );
		if (hditem.fmt & (HDF_SORTDOWN | HDF_SORTUP))
		{
			hditem.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
			SetItem( i, &hditem );
		}
	}

	GetItem( nCol, &hditem );

	// We can use HDF_SORTUP and HDF_SORTDOWN flag to draw a sort arrow on the header.
	hditem.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
	hditem.fmt |= bAsc ? HDF_SORTUP : HDF_SORTDOWN;
	SetItem( nCol, &hditem );

	// Invalidate header control so that it gets redrawn
	Invalidate();

	return nPrevCol;
}

BOOL CSortHeaderCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);
	pDC->FillSolidRect(rcClient, GetSysColor(COLOR_3DHIGHLIGHT));
	return TRUE;
}

static void DrawSortArrow(CDC& dc, bool up, int cx, int cy)
{
	const int lpx = dc.GetDeviceCaps(LOGPIXELSX);
	auto pointToPixelF = [lpx](float point) { return point * lpx / 72.f; };
	const int arrowWidth = static_cast<int>(pointToPixelF(2.25));
	Gdiplus::Graphics graphics(dc.m_hDC);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	Gdiplus::Color penColor;
	penColor.SetFromCOLORREF(GetSysColor(COLOR_BTNTEXT));
	Gdiplus::Pen pen(penColor, pointToPixelF(0.75));
	Gdiplus::Point points[3];
	if (up)
	{
		points[0] = Gdiplus::Point(cx - arrowWidth, cy + arrowWidth);
		points[1] = Gdiplus::Point(cx, cy);
		points[2] = Gdiplus::Point(cx + arrowWidth, cy + arrowWidth);
	}
	else
	{
		points[0] = Gdiplus::Point(cx - arrowWidth, cy);
		points[1] = Gdiplus::Point(cx, cy + arrowWidth);
		points[2] = Gdiplus::Point(cx + arrowWidth, cy);
	}
	graphics.DrawLines(&pen, points, 3);
}

void CSortHeaderCtrl::OnPaint()
{
	CPaintDC dc(this);
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DFACE));
	CPen* pOldPen = dc.SelectObject(&pen);
	dc.SelectObject(GetFont());
	dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
	dc.SetBkMode(TRANSPARENT);
	const int nItems = this->GetItemCount();
	for (int i = 0; i < nItems; ++i)
	{
		TCHAR buf[256]{};
		HDITEM hi;
		hi.mask = HDI_TEXT | HDI_FORMAT;
		hi.pszText = buf;
		hi.cchTextMax = sizeof(buf) / sizeof(buf[0]);
		GetItem(i, &hi);
		CRect rc;
		GetItemRect(i, &rc);
		dc.FillSolidRect(&rc, GetSysColor(
			m_nTrackingPane == i ? COLOR_3DFACE : COLOR_3DHIGHLIGHT));
		dc.DrawText(hi.pszText, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		dc.MoveTo(rc.right - 1, rc.top);
		dc.LineTo(rc.right - 1, rc.bottom);
		if ((hi.fmt & (HDF_SORTDOWN | HDF_SORTUP)) != 0)
			DrawSortArrow(dc, (hi.fmt & HDF_SORTUP) != 0, (rc.left + rc.right) / 2, rc.top);
	}
	dc.SelectObject(pOldPen);
	pen.DeleteObject();
}

void CSortHeaderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_LBUTTON) == 0)
	{
		if (!m_bMouseTracking)
		{
			TRACKMOUSEEVENT tme = { sizeof TRACKMOUSEEVENT, TME_LEAVE, m_hWnd };
			TrackMouseEvent(&tme);
			m_bMouseTracking = true;
		}
		HDHITTESTINFO hhti;
		GetCursorPos(&hhti.pt);
		ScreenToClient(&hhti.pt);
		HitTest(&hhti);
		for (int pane : {hhti.iItem, m_nTrackingPane})
		{
			if (pane >= 0)
			{
				CRect rcPart;
				GetItemRect(pane, rcPart);
				InvalidateRect(&rcPart, false);
			}
		}
		m_nTrackingPane = hhti.iItem;
	}
	__super::OnMouseMove(nFlags, point);
}

void CSortHeaderCtrl::OnMouseLeave()
{
	TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE | TME_CANCEL, m_hWnd };
	TrackMouseEvent(&tme);
	m_bMouseTracking = false;
	if (m_nTrackingPane >= 0)
	{
		CRect rcPart;
		GetItemRect(m_nTrackingPane, rcPart);
		InvalidateRect(&rcPart, false);
	}
	m_nTrackingPane = -1;
}