/** 
 * @file  ccrystalrenderergdi.h
 *
 * @brief Declaration file for CCrystalRendererGDI
 */
#include "ccrystalrenderer.h"
#include <memory>

#pragma once

////////////////////////////////////////////////////////////////////////////
// Forward class declarations


////////////////////////////////////////////////////////////////////////////
// CCrystalRendererGDI class declaration


class CCrystalRendererGDI : public CCrystalRenderer
{
public:
	CCrystalRendererGDI();
	virtual ~CCrystalRendererGDI();

	virtual void BindDC(const CDC& dc, const CRect& rc);
	virtual void BeginDraw();
	virtual bool EndDraw();
	virtual void SetFont(const LOGFONT &lf);
	virtual void SwitchFont(bool italic, bool bold);
	virtual CSize GetCharWidthHeight();
	virtual bool GetCharWidth(unsigned start, unsigned end, int *nWidthArray);
	virtual void SetTextColor(COLORREF clr);
	virtual void SetBkColor(COLORREF clr);
	virtual void DrawText(int x, int y, const CRect &rc, const TCHAR *text, size_t len, const int nWidths[]);
	virtual void FillRectangle(const CRect &rc);
	virtual void FillSolidRectangle(const CRect &rc, COLORREF color);
	virtual void DrawRoundRectangle(int left, int top , int right, int bottom, int width, int height);
	virtual void PushAxisAlignedClip(const CRect &rc);
	virtual void PopAxisAlignedClip();
	virtual void DrawMarginIcon(int x, int y, int iconIndex);
	virtual void DrawMarginLineNumber(int x, int y, int number);
	virtual void DrawBoundaryLine(int left, int right, int y);
	virtual void DrawLineCursor(int left, int right, int y, int height);

private:
	CDC *m_pDC;
	LOGFONT m_lfBaseFont;
    std::array<std::unique_ptr<CFont>, 4> m_apFonts;
	static CImageList *s_pIcons;
};
