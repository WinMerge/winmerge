/**
 * @file  ccrystalrenderergdi.cpp
 *
 * @brief Implementation of the CCrystalRendererGDI class
 */

#include "StdAfx.h"
#include "ccrystalrenderergdi.h"
#include "utils/hqbitmap.h"
#include "resource.h"

std::map<int, std::unique_ptr<CImageList>> CCrystalRendererGDI::s_mapIcons;

/////////////////////////////////////////////////////////////////////////////
// CCrystalRendererGDI construction/destruction

CCrystalRendererGDI::CCrystalRendererGDI() :
  m_pDC(nullptr)
, m_lfBaseFont{}
, m_linePen(PS_SOLID, 0, RGB(0, 0, 0))
{
}

CCrystalRendererGDI::~CCrystalRendererGDI ()
{
}

void CCrystalRendererGDI::BindDC(const CDC& dc, const CRect& rc)
{
	m_pDC = const_cast<CDC *>(&dc);
}

void CCrystalRendererGDI::BeginDraw()
{
	m_pDC->SaveDC();
}

bool CCrystalRendererGDI::EndDraw()
{
	m_pDC->RestoreDC(-1);
	return true;
}

static LONG GetFontWeight(const LOGFONT& lf, bool bold)
{
	const long weight = bold ? (lf.lfWeight + 300) : lf.lfWeight;
	if (weight <= 100) return FW_THIN;
	else if (weight <= 200) return FW_EXTRALIGHT;
	else if (weight <= 300) return FW_LIGHT;
	else if (weight <= 400) return FW_NORMAL;
	else if (weight <= 500) return FW_MEDIUM;
	else if (weight <= 600) return FW_SEMIBOLD;
	else if (weight <= 700) return FW_BOLD;
	else if (weight <= 800) return FW_EXTRABOLD;
	else return FW_BLACK;
}

void CCrystalRendererGDI::SetFont(const LOGFONT &lf)
{
	m_lfBaseFont = lf;
	for (int nIndex = 0; nIndex < 4; ++nIndex)
	{
		bool bold = (nIndex & 1) != 0;
		bool italic = (nIndex & 2) != 0;
		m_apFonts[nIndex].reset(new CFont());
		if (!m_lfBaseFont.lfHeight)
		{
			CClientDC dc (CWnd::GetDesktopWindow());
			m_lfBaseFont.lfHeight = -MulDiv (11, dc.GetDeviceCaps (LOGPIXELSY), 72);
		}
		m_lfBaseFont.lfWeight = GetFontWeight(lf, bold);
		m_lfBaseFont.lfItalic = (BYTE) italic;
		if (!m_apFonts[nIndex]->CreateFontIndirect(&m_lfBaseFont))
			m_apFonts[nIndex].reset(nullptr);
	}
}

void CCrystalRendererGDI::SwitchFont(bool italic, bool bold)
{
	int nIndex = 0;
	if (bold)
		nIndex |= 1;
	if (italic)
		nIndex |= 2;
	m_pDC->SelectObject(m_apFonts[nIndex].get());
}

CSize CCrystalRendererGDI::GetCharWidthHeight()
{
	if (!m_apFonts[3])
		SetFont(m_lfBaseFont);
	CClientDC dc (CWnd::GetDesktopWindow());
	CFont *pOldFont = dc.SelectObject(m_apFonts[3].get());
	CSize sizeItalicBold = dc.GetTextExtent(_T("X"));
	dc.SelectObject(m_apFonts[0].get());
	CSize sizeNormal = dc.GetTextExtent(_T("X"));
	dc.SelectObject(pOldFont);
	return CSize{ sizeNormal.cx, (std::max)(sizeNormal.cy, sizeItalicBold.cy) };
}

bool CCrystalRendererGDI::GetCharWidth(unsigned start, unsigned end, int * nWidthArray)
{
	if (!m_apFonts[3])
		SetFont(m_lfBaseFont);
	CClientDC dc (CWnd::GetDesktopWindow());
	CFont *pOldFont = dc.SelectObject(m_apFonts[3].get());
	bool succeeded = !!GetCharWidth32(dc.m_hDC, start, end, nWidthArray);
	dc.SelectObject(pOldFont);
	return succeeded;
}

void CCrystalRendererGDI::SetLineColor(CEColor clr)
{
	m_linePen.DeleteObject();
	m_linePen.CreatePen(PS_SOLID, 1, clr);
}

void CCrystalRendererGDI::SetTextColor(CEColor clr)
{
	m_pDC->SetTextColor(clr);
}

void CCrystalRendererGDI::SetBkColor(CEColor clr)
{
	m_pDC->SetBkColor(clr);
}

void CCrystalRendererGDI::FillRectangle(const CRect &rc)
{
	m_pDC->FillSolidRect(&rc, m_pDC->GetBkColor());
}

void CCrystalRendererGDI::FillSolidRectangle(const CRect &rc, CEColor color)
{
	m_pDC->FillSolidRect(&rc, color);
}

void CCrystalRendererGDI::DrawRoundRectangle(int left, int top, int right, int bottom, int width, int height)
{
	CBrush brush;
	brush.CreateStockObject(NULL_BRUSH);
	CBrush* pOldBrush = m_pDC->SelectObject(&brush);
	CPen pen(PS_SOLID, 1, m_pDC->GetTextColor());
	CPen *pOldPen = m_pDC->SelectObject(&pen);

	m_pDC->RoundRect(left, top, right, bottom, width, height);

	m_pDC->SelectObject(pOldPen);
	m_pDC->SelectObject(pOldBrush);
}

void CCrystalRendererGDI::PushAxisAlignedClip(const CRect & rc)
{
	m_pDC->SaveDC();
	m_pDC->IntersectClipRect(&rc);
}

void CCrystalRendererGDI::PopAxisAlignedClip()
{
	m_pDC->RestoreDC(-1);
}

void CCrystalRendererGDI::DrawMarginIcon(int x, int y, int iconIndex, int iconsize)
{
	CImageList* pIcons = nullptr;
	if (s_mapIcons.find(iconsize) == s_mapIcons.end())
	{
		s_mapIcons.emplace(iconsize, new CImageList);
		pIcons = s_mapIcons[iconsize].get();
		VERIFY(pIcons->Create(iconsize, iconsize,
			ILC_COLOR32 | ILC_MASK, 0, 1));
		CBitmap bmp;
		bmp.Attach(LoadBitmapAndConvertTo32bit(AfxGetInstanceHandle(), IDR_MARGIN_ICONS,
			iconsize * MARGIN_ICON_COUNT, iconsize, false, RGB(255, 255, 255)));
		pIcons->Add(&bmp, nullptr);
	}
	if (iconIndex >= 0)
	{
		CPoint pt(x, y);
		VERIFY(s_mapIcons[iconsize]->Draw(m_pDC, iconIndex, pt, ILD_TRANSPARENT));
	}
}

void CCrystalRendererGDI::DrawMarginLineNumber(int x, int y, int number)
{
	CFont *pOldFont = m_pDC->SelectObject(m_apFonts[0].get());
	tchar_t szNumbers[32];
	int len = wsprintf(szNumbers, _T("%d"), number);
	UINT uiOldAlign = m_pDC->SetTextAlign(TA_RIGHT);
	m_pDC->TextOut(x - (m_pDC->IsPrinting() ? 0 : 4), y, szNumbers, len);
	m_pDC->SetTextAlign(uiOldAlign);
	m_pDC->SelectObject(pOldFont);
}

void CCrystalRendererGDI::DrawBoundaryLine(int left, int right, int y)
{
	CPen *pOldPen = (CPen *)m_pDC->SelectObject(&m_linePen);
	m_pDC->MoveTo(left, y);
	m_pDC->LineTo(right, y);
	m_pDC->SelectObject(pOldPen);
}

void CCrystalRendererGDI::DrawGridLine(int x1, int y1, int x2, int y2, int sourceConstantAlpha)
{
	Gdiplus::Graphics graphics(m_pDC->GetSafeHdc());

	LOGPEN logPen;
	m_linePen.GetLogPen(&logPen);
	Gdiplus::Color color(logPen.lopnColor);

	const BYTE alpha = static_cast<BYTE>(sourceConstantAlpha);
	Gdiplus::Color transparentColor(alpha, color.GetR(), color.GetG(), color.GetB());

	Gdiplus::Pen pen(transparentColor);

	graphics.DrawLine(&pen, x1, y1, x2, y2);
}

void CCrystalRendererGDI::DrawLineCursor(int left, int right, int y, int height)
{
	DrawGridLine(left, y, right, y, 32);
}

void CCrystalRendererGDI::DrawText(int x, int y, const CRect &rc, const tchar_t* text, size_t len, const int nWidths[])
{
	m_pDC->ExtTextOut(x, y, ETO_CLIPPED | ETO_OPAQUE, &rc, text, static_cast<UINT>(len), const_cast<int *>(nWidths));
}

void CCrystalRendererGDI::DrawRuler(int left, int top, int width, int height, int charwidth, int offset)
{
	CFont *pOldFont = m_pDC->SelectObject(m_apFonts[0].get());
	UINT uiOldAlign = m_pDC->SetTextAlign(TA_LEFT);
	int bottom = top + height - 1;
	int prev10 = (offset / 10) * 10;
	tchar_t szNumbers[32];
	int len = wsprintf(szNumbers, _T("%d"), prev10);
	if ((offset % 10) != 0 && offset - prev10 < len)
		m_pDC->TextOut(left, bottom - height, szNumbers + (offset - prev10), len - (offset - prev10));
	for (int i = 0; i < width / charwidth; ++i)
	{
		int x = left + i * charwidth;
		if (((i + offset) % 10) == 0)
		{
			len = wsprintf(szNumbers, _T("%d"), offset + i);
			m_pDC->TextOut(x, bottom - height + 1, szNumbers, len);
		}
		float tickscale = [](int i, int offset) {
			if (((i + offset) % 10) == 0)
				return 0.6f;
			else if (((i + offset) % 5) == 0)
				return 0.4f;
			else
				return 0.2f;
		}(i, offset);
		DrawGridLine(x, bottom - static_cast<int>(height * tickscale), x, bottom, 70);
	}
	DrawGridLine(left, bottom, left + width, bottom, 70);
	m_pDC->SetTextAlign(uiOldAlign);
	m_pDC->SelectObject(pOldFont);
}

