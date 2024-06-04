/** 
 * @file  ccrystalrenderergdi.h
 *
 * @brief Declaration file for CCrystalRendererGDI
 */
#include "ccrystalrenderer.h"
#include "cecolor.h"
#include "utils/ctchar.h"
#include <memory>
#include <array>
#include <map>

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
	virtual void DrawMarginIcon(int x, int y, int iconIndex,int iconsize) override;
	virtual void DrawMarginLineNumber(int x, int y, int number) override;
	virtual void DrawBoundaryLine(int left, int right, int y) override;
	virtual void DrawGridLine(int x1, int y1, int x2, int y2, int sourceConstantAlpha) override;
	virtual void DrawLineCursor(int left, int right, int y, int height) override;
	virtual void DrawRuler(int left, int top, int width, int height, int charwidth, int offset) override;

private:
	CDC *m_pDC;
	LOGFONT m_lfBaseFont;
	CPen m_gridPen;
	std::array<std::unique_ptr<CFont>, 4> m_apFonts;
	static std::map<int, std::unique_ptr<CImageList>> s_mapIcons;
};
