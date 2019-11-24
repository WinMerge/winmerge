/** 
 * @file  ccrystalrendererdirectdraw.h
 *
 * @brief Declaration file for CCrystalRendererDirectWrite
 */
#include "ccrystalrenderer.h"
#include <memory>
#include <array>
#include <afxwin.h>

#pragma once

////////////////////////////////////////////////////////////////////////////
// Forward class declarations
class CCustomTextRenderer;

////////////////////////////////////////////////////////////////////////////
// CCrystalRendererDirectWrite class declaration

class CCrystalRendererDirectWrite : public CCrystalRenderer
{
public:
	CCrystalRendererDirectWrite(int nRenderingMode = 1);
	virtual ~CCrystalRendererDirectWrite();

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
	STDMETHODIMP DrawGlyphRun(void* pClientDrawingContext,
		FLOAT fBaselineOriginX, FLOAT fBaselineOriginY, DWRITE_MEASURING_MODE measuringMode,
		const DWRITE_GLYPH_RUN* pGlyphRun, const DWRITE_GLYPH_RUN_DESCRIPTION* pGlyphRunDescription,
		IUnknown* pClientDrawingEffect);

	D2D1::ColorF ColorRefToColorF(COLORREF cr) const
	{
		return D2D1::ColorF(GetRValue(cr) / 255.0f, GetGValue(cr) / 255.0f, GetBValue(cr) / 255.0f);
	};

	CDCRenderTarget m_renderTarget;
	std::array<std::unique_ptr<CD2DTextFormat>, 4> m_pTextFormat;
	CD2DTextFormat *m_pCurrentTextFormat;
	std::unique_ptr<CD2DSolidColorBrush> m_pTextBrush;
	std::unique_ptr<CD2DSolidColorBrush> m_pTempBrush;
	std::unique_ptr<CD2DSolidColorBrush> m_pBackgroundBrush;
	std::unique_ptr<CCustomTextRenderer> m_pTextRenderer;
	std::unique_ptr<CD2DBitmap> m_pIconBitmap;
	LOGFONT m_lfBaseFont;
	std::unique_ptr<CFont> m_pFont;
	D2D1_SIZE_F m_charSize;
	float m_fontAscent;
};
