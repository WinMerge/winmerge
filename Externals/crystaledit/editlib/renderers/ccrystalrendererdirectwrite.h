/** 
 * @file  ccrystalrendererdirectdraw.h
 *
 * @brief Declaration file for CCrystalRendererDirectWrite
 */
#include "ccrystalrenderer.h"
#include "cecolor.h"
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

	virtual void BindDC(const CDC& dc, const CRect& rc) override;
	virtual void BeginDraw() override;
	virtual bool EndDraw() override;
	virtual void SetFont(const LOGFONT &lf) override;
	virtual void SwitchFont(bool italic, bool bold) override;
	virtual CSize GetCharWidthHeight() override;
	virtual bool GetCharWidth(unsigned start, unsigned end, int *nWidthArray) override;
	virtual void SetTextColor(CEColor clr) override;
	virtual void SetBkColor(CEColor clr) override;
	virtual void DrawText(int x, int y, const CRect &rc, const tchar_t* text, size_t len, const int nWidths[]) override;
	virtual void FillRectangle(const CRect &rc) override;
	virtual void FillSolidRectangle(const CRect &rc, CEColor color) override;
	virtual void DrawRoundRectangle(int left, int top , int right, int bottom, int width, int height) override;
	virtual void PushAxisAlignedClip(const CRect &rc) override;
	virtual void PopAxisAlignedClip() override;
	virtual void DrawMarginIcon(int x, int y, int iconIndex, int iconsize) override;
	virtual void DrawMarginLineNumber(int x, int y, int number) override;
	virtual void DrawBoundaryLine(int left, int right, int y) override;
	virtual void DrawGridLine(int x1, int y1, int x2, int y2, int sourceConstantAlpha) override;
	virtual void DrawLineCursor(int left, int right, int y, int height) override;
	virtual void DrawRuler(int left, int top, int width, int height, int charwidth, int offset) override;

private:
	STDMETHODIMP DrawGlyphRun(void* pClientDrawingContext,
		FLOAT fBaselineOriginX, FLOAT fBaselineOriginY, DWRITE_MEASURING_MODE measuringMode,
		const DWRITE_GLYPH_RUN* pGlyphRun, const DWRITE_GLYPH_RUN_DESCRIPTION* pGlyphRunDescription,
		IUnknown* pClientDrawingEffect);

	D2D1::ColorF ColorRefToColorF(CEColor cr) const
	{
		return D2D1::ColorF(cr.GetRVal() / 255.0f, cr.GetGVal() / 255.0f, cr.GetBVal() / 255.0f);
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
};
