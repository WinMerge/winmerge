/** 
 * @file  RoundedRectWithShadow.h
 *
 * @brief Provides functions to draw rounded rectangles with shadows in a Win32 GDI context.
 *
 */
#pragma once

void DrawRoundedRectWithShadow(HDC hdc, int left, int top, int width, int height, int radius, int shadowWidth, COLORREF roundedRectColor, COLORREF shadowColor, COLORREF backgroundColor);
inline void DrawRoundedRect(HDC hdc, int left, int top, int width, int height, int radius, COLORREF roundedRectColor, COLORREF backgroundColor)
{
	DrawRoundedRectWithShadow(hdc, left, top, width, height, radius, 0, roundedRectColor, backgroundColor, backgroundColor);
}

