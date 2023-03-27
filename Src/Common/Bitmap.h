/** 
 * @file  Bitmap.h
 *
 * @brief Declaration file for Bitmap helper functions.
 *
 */
#pragma once

#include "utils/ctchar.h"

class CBitmap;
class CDC;
class CRect;
namespace ATL { class CImage; }

CBitmap *CopyRectToBitmap(CDC *pDC, const CRect & rect);
void DrawBitmap(CDC *pDC, int x, int y, CBitmap *pBitmap);
CBitmap *GetDarkenedBitmap(CDC *pDC, CBitmap *pBitmap, bool lighten = false);
bool LoadImageFromResource(ATL::CImage& image, const tchar_t *pName, const tchar_t *pType);
