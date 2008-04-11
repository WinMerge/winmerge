/** 
 * @file  Bitmap.h
 *
 * @brief Declaration file for Bitmap helper functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _BITMAP_H_
#define _BITMAP_H_

CBitmap *CopyRectToBitmap(CDC *pDC, const CRect & rect);
void DrawBitmap(CDC *pDC, int x, int y, CBitmap *pBitmap);
CBitmap *GetDarkenedBitmap(CDC *pDC, CBitmap *pBitmap);

#endif // _BITMAP_H_
