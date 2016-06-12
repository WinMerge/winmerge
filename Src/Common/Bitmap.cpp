/** 
 * @file  Bitmap.cpp
 *
 * @brief Implementation file for Bitmap helper functions.
 *
 */

#include "StdAfx.h"
#include "Bitmap.h"
#include <cmath>
#include <memory>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/**
 * @brief Save an area as a bitmap
 * @param pDC [in] The source device context
 * @param rect [in] The rect to be copied
 * @return The bitmap object
 */
CBitmap *CopyRectToBitmap(CDC *pDC, const CRect & rect)
{
	CRect rc = rect;
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap *pBitmap = new CBitmap;
	pBitmap->CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	CBitmap *pOldBitmap = dcMem.SelectObject(pBitmap);
	dcMem.BitBlt(0, 0, rc.Width(), rc.Height(), pDC, rc.left, rc.top, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
	dcMem.DeleteDC();
	return pBitmap;
}

/**
 * @brief Draw a bitmap image
 * @param pDC [in] The destination device context to draw to
 * @param x [in] The x-coordinate of the upper-left corner of the bitmap
 * @param y [in] The y-coordinate of the upper-left corner of the bitmap
 * @param pBitmap [in] the bitmap to draw
 */
void DrawBitmap(CDC *pDC, int x, int y, CBitmap *pBitmap)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	BITMAP bm;
	pBitmap->GetBitmap(&bm);
	CBitmap *pOldBitmap = dcMem.SelectObject(pBitmap);
	pDC->BitBlt(x, y, bm.bmWidth, bm.bmHeight, &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
	dcMem.DeleteDC();
}

/**
 * @brief Duplicate a bitmap and make it dark
 * @param pDC [in] Device context
 * @param pBitmap [in] the bitmap to darken
 * @return The bitmap object
 */
CBitmap *GetDarkenedBitmap(CDC *pDC, CBitmap *pBitmap)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	BITMAP bm;
	pBitmap->GetObject(sizeof(bm), &bm);
	CBitmap *pBitmapDarkened = new CBitmap();
	pBitmapDarkened->CreateCompatibleBitmap(pDC, bm.bmWidth, bm.bmHeight);
	CBitmap *pOldBitmap = dcMem.SelectObject(pBitmapDarkened);
	DrawBitmap(&dcMem, 0, 0, pBitmap);

	BITMAPINFO bi;
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = bm.bmWidth;
	bi.bmiHeader.biHeight = bm.bmHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = 0;
	bi.bmiHeader.biSizeImage = bm.bmWidth * 4 * bm.bmHeight;
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	std::unique_ptr<BYTE[]> pbuf(new BYTE[bi.bmiHeader.biSizeImage]);
	GetDIBits(dcMem.m_hDC, (HBITMAP)*pBitmapDarkened, 0, bm.bmHeight, pbuf.get(), &bi, DIB_RGB_COLORS);

	int x;
	for (x = 0; x < bm.bmWidth; x++)
	{
		double b = 0.70 + (0.20 * sin(acos((double)x/bm.bmWidth*2.0-1.0)));
		for (int y = 1; y < bm.bmHeight-1; y++)
		{
			int i = x * 4 + y * bm.bmWidth * 4;
			pbuf[i  ] = (BYTE)(pbuf[i] * 0.95);
			pbuf[i+1] = (BYTE)(pbuf[i+1] * b);
			pbuf[i+2] = (BYTE)(pbuf[i+2] * b);
		}
	}
	for (x = 0; x < bm.bmWidth; x++)
	{
		int i = x * 4 + 0 * bm.bmWidth * 4;
		pbuf[i  ] = (BYTE)(pbuf[i] * 0.95);
		pbuf[i+1] = (BYTE)(pbuf[i+1] * 0.8);
		pbuf[i+2] = (BYTE)(pbuf[i+2] * 0.8);
		i = x * 4 + (bm.bmHeight-1) * bm.bmWidth * 4;
		pbuf[i  ] = (BYTE)(pbuf[i] * 0.95);
		pbuf[i+1] = (BYTE)(pbuf[i+1] * 0.8);
		pbuf[i+2] = (BYTE)(pbuf[i+2] * 0.8);
	}
	for (int y = 0; y < bm.bmHeight; y++)
	{
		int i = 0 * 4 + y * bm.bmWidth * 4;
		pbuf[i  ] = (BYTE)(pbuf[i] * 0.95);
		pbuf[i+1] = (BYTE)(pbuf[i+1] * 0.9);
		pbuf[i+2] = (BYTE)(pbuf[i+2] * 0.9);
		i = (bm.bmWidth-1) * 4 + y * bm.bmWidth * 4;
		pbuf[i  ] = (BYTE)(pbuf[i] * 0.95);
		pbuf[i+1] = (BYTE)(pbuf[i+1] * 0.9);
		pbuf[i+2] = (BYTE)(pbuf[i+2] * 0.9);
	}

	SetDIBits(dcMem.m_hDC, (HBITMAP)*pBitmapDarkened, 0, bm.bmHeight, pbuf.get(), &bi, DIB_RGB_COLORS);
	dcMem.SelectObject(pOldBitmap);
	dcMem.DeleteDC();
	return pBitmapDarkened;
}

bool GrayScale(CBitmap *pBitmap)
{
	BITMAP bm;
	pBitmap->GetBitmap(&bm);
	if (bm.bmBitsPixel < 24)
		return false;
	const int nCount = bm.bmWidthBytes * bm.bmHeight;
	const int bypp = bm.bmBitsPixel / 8;
	std::unique_ptr<BYTE[]> pbuf(new BYTE[nCount]);
	pBitmap->GetBitmapBits(nCount, pbuf.get());
	for (int i = 0, x = 0, y = 0; i < nCount - bypp; i += bypp, x += bypp)
	{
		const BYTE b = pbuf[i];
		const BYTE g = pbuf[i + 1];
		const BYTE r = pbuf[i + 2];
		const BYTE gray = static_cast<BYTE>(
			 (static_cast<int>(0.114 * 256) * (((255 - b) >> 1) + b)
			+ static_cast<int>(0.587 * 256) * (((255 - g) >> 1) + g)
			+ static_cast<int>(0.299 * 256) * (((255 - r) >> 1) + r)) >> 8);
		pbuf[i] = gray;
		pbuf[i + 1] = gray;
		pbuf[i + 2] = gray;
		if (x >= bm.bmWidthBytes - bypp)
		{
			++y;
			x = 0;
			i = y * bm.bmWidthBytes;
		}
	}
	return pBitmap->SetBitmapBits(nCount, pbuf.get()) != 0;
}