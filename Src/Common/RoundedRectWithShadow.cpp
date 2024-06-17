// Copyright (c) 2024 Takashi Sawanaka
// SPDX-License-Identifier: BSL-1.0
/** 
 * @file  RoundedRectWithShadow.cpp
 *
 * @brief Provides functions to draw rounded rectangles with shadows in a Win32 GDI context.
 *
 */
#include "StdAfx.h"
#include <cmath>
#include <algorithm>

struct RoundedRectWithShadow
{
	using dibcolor_t = unsigned int;

	RoundedRectWithShadow(int left, int top, int width, int height, int radius, int shadowWidth,
		COLORREF colorRoundedRect, COLORREF colorShadow, COLORREF colorBackground)
		: m_left(left)
		, m_top(top)
		, m_width(width)
		, m_height(height)
		, m_radius(radius)
		, m_shadowWidth(shadowWidth)
		, m_colorRoundedRect(COLORREFtoDIBColor_t(colorRoundedRect))
		, m_colorShadow(COLORREFtoDIBColor_t(colorShadow))
		, m_colorBackground(COLORREFtoDIBColor_t(colorBackground))
		, m_leftCircleCenterX(left + radius)
		, m_topCircleCenterY(top + radius)
		, m_rightCircleCenterX(left + width - radius)
		, m_bottomCircleCenterY(top + height - radius)
	{
	}

	static inline double sqr(double x) { return x * x; }

	static double distance(double x1, double y1, double x2, double y2)
	{
		return std::sqrt(sqr(x1 - x2) + sqr(y1 - y2));
	}

	static dibcolor_t COLORREFtoDIBColor_t(COLORREF color)
	{
		const BYTE r = GetRValue(color);
		const BYTE g = GetGValue(color);
		const BYTE b = GetBValue(color);
		return (r << 16) | (g << 8) | b;
	}

	static dibcolor_t interpolateColor(dibcolor_t color1, dibcolor_t color2, double t)
	{
		const unsigned char r1 = (color1 >> 16) & 0xff;
		const unsigned char g1 = (color1 >> 8) & 0xff;
		const unsigned char b1 = (color1) & 0xff;

		const unsigned char r2 = (color2 >> 16) & 0xff;
		const unsigned char g2 = (color2 >> 8) & 0xff;
		const unsigned char b2 = (color2) & 0xff;

		const unsigned char r = static_cast<unsigned char>(r1 + t * (r2 - r1));
		const unsigned char g = static_cast<unsigned char>(g1 + t * (g2 - g1));
		const unsigned char b = static_cast<unsigned char>(b1 + t * (b2 - b1));

		return (r << 16) | (g << 8) | b;
	}

	dibcolor_t getColor(int x, int y) const
	{
		if ((m_leftCircleCenterX <= x && x < m_rightCircleCenterX &&
			m_top <= y && y < m_top + m_height) ||
			(m_left <= x && x < m_left + m_width &&
				m_topCircleCenterY <= y && y < m_bottomCircleCenterY))
		{
			return m_colorRoundedRect;
		}

		double min_distance = (std::numeric_limits<double>::max)();
		if (y < m_top)
		{
			if (m_leftCircleCenterX <= x && x < m_rightCircleCenterX)
				min_distance = m_top - y;
		}
		else if (y > m_top + m_height)
		{
			if (m_leftCircleCenterX <= x && x < m_rightCircleCenterX)
				min_distance = y - (m_top + m_height) + 1;
		}
		else if (x < m_left)
		{
			if (m_topCircleCenterY <= y && y < m_bottomCircleCenterY)
				min_distance = m_left - x;
		}
		else if (x > m_left + m_width)
		{
			if (m_topCircleCenterY <= y && y < m_bottomCircleCenterY)
				min_distance = x - (m_left + m_width) + 1;
		}

		if (min_distance == (std::numeric_limits<double>::max)())
		{
			const double cx = (x < (m_left + m_left + m_width) / 2) ? m_leftCircleCenterX : m_rightCircleCenterX - 1;
			const double cy = (y < (m_top + m_top + m_height) / 2) ? m_topCircleCenterY : m_bottomCircleCenterY - 1;
			const double dist = distance(x, y, cx, cy);
			if (dist <= m_radius)
				return m_colorRoundedRect;
			min_distance = (std::min)(min_distance, dist - m_radius);
		}

		if (min_distance < 1.0)
			return interpolateColor(m_colorRoundedRect, m_shadowWidth == 0 ? m_colorBackground : m_colorShadow, min_distance);
		if (min_distance > m_shadowWidth)
			return m_colorBackground;
		const double t = std::sqrt(1.0 - sqr((min_distance - 1.0) / m_shadowWidth - 1.0));
		return interpolateColor(m_colorShadow, m_colorBackground, t);
	}

	void DrawPartialRoundedRectWithShadow(void* pBits, int pleft, int ptop, int pwidth, int pheight)
	{
		if (pleft < m_left - m_shadowWidth || pleft + pwidth > m_left + m_width + m_shadowWidth)
			return;
		if (ptop < m_top - m_shadowWidth || ptop + pheight > m_top + m_height + m_shadowWidth)
			return;
		const int left = m_left;
		const int top = m_top;
		const int shadowWidth = m_shadowWidth;
		const int pitch = m_width + m_shadowWidth * 2;
		if (m_left + m_radius <= pleft && pleft + pwidth <= m_left + m_width - m_radius)
		{
			for (int y = ptop; y < ptop + pheight; y++)
			{
				dibcolor_t color = getColor(pleft, y);
				for (int x = pleft; x < pleft + pwidth; x++)
					((dibcolor_t*)pBits)[(y - (top - shadowWidth)) * pitch + (x - (left - shadowWidth))] = color;
			}
		}
		else if (m_top + m_radius <= ptop && ptop + pheight <= m_top + m_height - m_radius)
		{
			for (int x = pleft; x < pleft + pwidth; x++)
			{
				dibcolor_t color = getColor(x, ptop);
				for (int y = ptop; y < ptop + pheight; y++)
					((dibcolor_t*)pBits)[(y - (top - shadowWidth)) * pitch + (x - (left - shadowWidth))] = color;
			}
		}
		else
		{
			for (int y = ptop; y < ptop + pheight; y++)
			{
				for (int x = pleft; x < pleft + pwidth; x++)
				{
					dibcolor_t color = getColor(x, y);
					((dibcolor_t*)pBits)[(y - (top - shadowWidth)) * pitch + (x - (left - shadowWidth))] = color;
				}
			}
		}
	}

	int m_left;
	int m_top;
	int m_width;
	int m_height;
	int m_radius;
	int m_shadowWidth;
	int m_leftCircleCenterX;
	int m_topCircleCenterY;
	int m_rightCircleCenterX;
	int m_bottomCircleCenterY;
	dibcolor_t m_colorRoundedRect;
	dibcolor_t m_colorShadow;
	dibcolor_t m_colorBackground;
};

void DrawRoundedRectWithShadow(HDC hdc, int left, int top, int width, int height, int radius, int shadowWidth,
	COLORREF roundedRectColor, COLORREF shadowColor, COLORREF backgroundColor)
{
	RoundedRectWithShadow rr(left, top, width, height, radius, shadowWidth, roundedRectColor, shadowColor, backgroundColor);

	HDC hdcMem = CreateCompatibleDC(hdc);
	if (!hdcMem)
		return;
	BITMAPINFO bmi{ sizeof(BITMAPINFOHEADER), width + shadowWidth * 2, -(height + shadowWidth * 2), 1, 32, BI_RGB };
	RoundedRectWithShadow::dibcolor_t* pBits;
	HBITMAP memBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, (void**)&pBits, nullptr, 0);
	if (memBitmap)
	{
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, memBitmap);

		std::fill_n(pBits, (width + shadowWidth * 2) * (height + shadowWidth * 2), rr.COLORREFtoDIBColor_t(roundedRectColor));

		// Draw the shadow on the top side
		rr.DrawPartialRoundedRectWithShadow(pBits, left + radius, top - shadowWidth, width - 2 * radius, shadowWidth);
		// Draw the shadow on the bottom side
		rr.DrawPartialRoundedRectWithShadow(pBits, left + radius, top + height, width - 2 * radius, shadowWidth);
		// Draw the shadow on the left side
		rr.DrawPartialRoundedRectWithShadow(pBits, left - shadowWidth, top + radius, shadowWidth, height - 2 * radius);
		// Draw the shadow on the right side
		rr.DrawPartialRoundedRectWithShadow(pBits, left + width, top + radius, shadowWidth, height - 2 * radius);
		// Draw the top-left rounded corner and shadow
		rr.DrawPartialRoundedRectWithShadow(pBits, left - shadowWidth, top - shadowWidth, shadowWidth + radius, shadowWidth + radius);
		// Draw the top-right rounded corner and shadow
		rr.DrawPartialRoundedRectWithShadow(pBits, left + width - radius, top - shadowWidth, shadowWidth + radius, shadowWidth + radius);
		// Draw the bottom-left rounded corner and shadow
		rr.DrawPartialRoundedRectWithShadow(pBits, left - shadowWidth, top + height - radius, shadowWidth + radius, shadowWidth + radius);
		// Draw the bottom-right rounded corner and shadow
		rr.DrawPartialRoundedRectWithShadow(pBits, left + width - radius, top + height - radius, shadowWidth + radius, shadowWidth + radius);

		BitBlt(hdc, left - shadowWidth, top - shadowWidth, width + 2 * shadowWidth, height + 2 * shadowWidth, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteObject(memBitmap);
	}

	DeleteDC(hdcMem);
}
