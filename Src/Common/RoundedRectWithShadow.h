/** 
 * @file  RoundedRectWithShadow.h
 *
 * @brief Provides functions to draw rounded rectangles with shadows in a Win32 GDI context.
 *
 */
#pragma once

void DrawRoundedRectWithShadow(HDC hdc, int left, int top, int width, int height, int radius, int shadowWidth, COLORREF roundedRectColor, COLORREF shadowColor, COLORREF backgroundColor);

