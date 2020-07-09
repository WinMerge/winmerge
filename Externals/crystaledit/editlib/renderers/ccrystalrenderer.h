/** 
 * @file  ccrystalrenderer.h
 *
 * @brief Declaration file for CCrystalRenderer
 */

#pragma once

////////////////////////////////////////////////////////////////////////////
// Forward class declarations


////////////////////////////////////////////////////////////////////////////
// CCrystalRenderer class declaration


struct CCrystalRenderer
{
	/** @brief Width of icons printed in the margin. */
	static const UINT MARGIN_ICON_WIDTH = 12;
	/** @brief Height of icons printed in the margin. */
	static const UINT MARGIN_ICON_HEIGHT = 12;

	virtual ~CCrystalRenderer() {};

	virtual void BindDC(const CDC& dc, const CRect& rc) = 0;
	virtual void BeginDraw() = 0;
	virtual bool EndDraw() = 0;
	virtual void SetFont(const LOGFONT &lf) = 0;
	virtual void SwitchFont(bool italic, bool bold) = 0;
	virtual CSize GetCharWidthHeight() = 0;
	virtual bool GetCharWidth(unsigned start, unsigned end, int *nWidthArray) = 0;
	virtual void SetTextColor(COLORREF clr) = 0;
	virtual void SetBkColor(COLORREF clr) = 0;
	virtual void DrawText(int x, int y, const CRect &rc, const TCHAR *text, size_t len, const int nWidths[]) = 0;
	virtual void FillRectangle(const CRect &rc) = 0;
	virtual void FillSolidRectangle(const CRect &rc, COLORREF color) = 0;
	virtual void DrawRoundRectangle(int left, int top , int right, int bottom, int width, int height) = 0;
	virtual void PushAxisAlignedClip(const CRect &rc) = 0;
	virtual void PopAxisAlignedClip() = 0;
	virtual void DrawMarginIcon(int x, int y, int iconIndex) = 0;
	virtual void DrawMarginLineNumber(int x, int y, int number) = 0;
	virtual void DrawBoundaryLine(int left, int right, int y) = 0;
	virtual void DrawHorizontalLine(int left, int right, int y) = 0;
	virtual void DrawVerticalLine(int x, int top, int bototm) = 0;
	virtual void DrawLineCursor(int left, int right, int y, int height) = 0;
	virtual void DrawRuler(int left, int top, int width, int height, int charwidth, int offset) = 0;
};

