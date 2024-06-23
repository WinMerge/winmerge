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
}

/**
 * @brief Duplicate a bitmap and make it dark
 * @param pDC [in] Device context
 * @param pBitmap [in] the bitmap to darken
 * @param radius [in] 
 * @param lighten [in] make bitmap lighten if ligthen is true
 * @return The bitmap object
 */
CBitmap *GetDarkenedBitmap(CDC *pDC, CBitmap *pBitmap, int radius, bool lighten)
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
	bi.bmiHeader.biHeight = -bm.bmHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = 0;
	bi.bmiHeader.biSizeImage = bm.bmWidth * 4 * bm.bmHeight;
	bi.bmiHeader.biXPelsPerMeter = 0;
	bi.bmiHeader.biYPelsPerMeter = 0;
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	auto pbuf = std::make_unique<BYTE[]>(bi.bmiHeader.biSizeImage);
	GetDIBits(dcMem.m_hDC, (HBITMAP)*pBitmapDarkened, 0, bm.bmHeight, pbuf.get(), &bi, DIB_RGB_COLORS);

	radius = std::clamp(radius, 0, static_cast<int>((std::min)(bm.bmWidth / 3, bm.bmHeight / 3)));

	if (!lighten)
	{
		auto darkenInner = [&pbuf, &bm](int left, int top, int right, int bottom)
			{
				for (int x = left; x < right; x++)
				{
					double b = 0.85 + (0.10 * sin(acos((double)x / bm.bmWidth*2.0 - 1.0)));
					for (int y = top; y < bottom; y++)
					{
						{
							int i = x * 4 + y * bm.bmWidth * 4;
							pbuf[i] = (BYTE)(pbuf[i] * 0.95);
							pbuf[i + 1] = (BYTE)(pbuf[i + 1] * b);
							pbuf[i + 2] = (BYTE)(pbuf[i + 2] * b);
						}
					}
				}

			};
		auto darkenCorner = [&pbuf, &bm](int left, int top, int right, int bottom, int cx, int cy, int radius)
			{
				auto sqr = [](double x) { return x * x; };
				for (int x = left; x < right; x++)
				{
					const double b = 0.85 + (0.10 * sin(acos((double)x / bm.bmWidth*2.0 - 1.0)));
					for (int y = top; y < bottom; y++)
					{
						int i = x * 4 + y * bm.bmWidth * 4;
						const double dist = std::sqrt(sqr(x - cx) + sqr(y - cy)) - static_cast<double>(radius);
						if (dist < 0)
						{
							pbuf[i] = (BYTE)(pbuf[i] * 0.95);
							pbuf[i + 1] = (BYTE)(pbuf[i + 1] * b);
							pbuf[i + 2] = (BYTE)(pbuf[i + 2] * b);
						}
						else if (dist <= 1.0)
						{
							pbuf[i] = (BYTE)(pbuf[i] * (0.95 + 0.05 * dist));
							pbuf[i + 1] = (BYTE)(pbuf[i + 1] * (0.9 + 0.1 * dist));
							pbuf[i + 2] = (BYTE)(pbuf[i + 2] * (0.9 + 0.1 * dist));
						}
					}
				}
			};
		auto darkenTopBottomEdge = [&pbuf, &bm](int y, int left, int right)
			{
				for (int x = left; x < right; x++)
				{
					int i = x * 4 + y * bm.bmWidth * 4;
					pbuf[i] = (BYTE)(pbuf[i] * 0.95);
					pbuf[i + 1] = (BYTE)(pbuf[i + 1] * 0.9);
					pbuf[i + 2] = (BYTE)(pbuf[i + 2] * 0.9);
				}
			};
		auto darkenLeftRightEdge = [&pbuf, &bm](int x, int top, int bottom)
			{
				for (int y = top; y < bottom; y++)
				{
					int i = x * 4 + y * bm.bmWidth * 4;
					pbuf[i] = (BYTE)(pbuf[i] * 0.95);
					pbuf[i + 1] = (BYTE)(pbuf[i + 1] * 0.85);
					pbuf[i + 2] = (BYTE)(pbuf[i + 2] * 0.85);
				}
			};
		darkenInner(radius, 1, bm.bmWidth - radius, radius);
		darkenInner(radius, bm.bmHeight - radius, bm.bmWidth - radius, bm.bmHeight - 1);
		darkenInner(1, radius, bm.bmWidth - 1, bm.bmHeight - radius);
		darkenCorner(0, 0, radius, radius, radius, radius, radius);
		darkenCorner(bm.bmWidth - radius, 0, bm.bmWidth, radius, bm.bmWidth - radius - 1, radius, radius);
		darkenCorner(0, bm.bmHeight - radius, radius, bm.bmHeight, radius, bm.bmHeight - radius - 1, radius);
		darkenCorner(bm.bmWidth - radius, bm.bmHeight - radius, bm.bmWidth, bm.bmHeight, bm.bmWidth - radius - 1, bm.bmHeight - radius - 1, radius);
		darkenTopBottomEdge(0, radius, bm.bmWidth - radius);
		darkenTopBottomEdge(bm.bmHeight - 1, radius, bm.bmWidth - radius);
		darkenLeftRightEdge(0, radius, bm.bmHeight - radius);
		darkenLeftRightEdge(bm.bmWidth - 1, radius, bm.bmHeight - radius);
	}
	else
	{
		auto lightenInner = [&pbuf, &bm](int left, int top, int right, int bottom)
			{
				for (int x = left; x < right; x++)
				{
					int b = static_cast<int>(12.0 + (20.0 * sin(acos((double)x / bm.bmWidth*2.0 - 1.0))));
					for (int y = top; y < bottom; y++)
					{
						int i = x * 4 + y * bm.bmWidth * 4;
						pbuf[i] = (BYTE)((std::min)(pbuf[i] + 40, 255));
						pbuf[i + 1] = (BYTE)((std::min)(pbuf[i + 1] + b, 255));
						pbuf[i + 2] = (BYTE)((std::min)(pbuf[i + 2] + b, 255));
					}
				}
			};
		auto lightenCorner = [&pbuf, &bm](int left, int top, int right, int bottom, int cx, int cy, int radius)
			{
				auto sqr = [](double x) { return x * x; };
				for (int x = left; x < right; x++)
				{
					int b = static_cast<int>(12.0 + (20.0 * sin(acos((double)x / bm.bmWidth*2.0 - 1.0))));
					for (int y = top; y < bottom; y++)
					{
						int i = x * 4 + y * bm.bmWidth * 4;
						const double dist = std::sqrt(sqr(x - cx) + sqr(y - cy)) - static_cast<double>(radius);
						if (dist < 0)
						{
							pbuf[i] = (BYTE)((std::min)(pbuf[i] + 40, 255));
							pbuf[i + 1] = (BYTE)((std::min)(pbuf[i + 1] + b, 255));
							pbuf[i + 2] = (BYTE)((std::min)(pbuf[i + 2] + b, 255));
						}
						else if (dist <= 1.0)
						{
							pbuf[i] = (BYTE)((std::min)(pbuf[i] + (BYTE)(40 * (1.0 - dist)), 255));
							pbuf[i + 1] = (BYTE)((std::min)(pbuf[i + 1] + (BYTE)(32 * (1.0 - dist)), 255));
							pbuf[i + 2] = (BYTE)((std::min)(pbuf[i + 2] + (BYTE)(32 * (1.0 - dist)), 255));
						}
					}
				}
			};
		auto lightenTopBottomEdge = [&pbuf, &bm](int y, int left, int right)
			{
				for (int x = left; x < right; x++)
				{
					int i = x * 4 + y * bm.bmWidth * 4;
					pbuf[i] = (BYTE)((std::min)(pbuf[i] + 40, 255));
					pbuf[i + 1] = (BYTE)((std::min)(pbuf[i + 1] + 32, 255));
					pbuf[i + 2] = (BYTE)((std::min)(pbuf[i + 2] + 32, 255));
				}
			};
		auto lightenLeftRightEdge = [&pbuf, &bm](int x, int top, int bottom)
			{
				for (int y = top; y < bottom; y++)
				{
					int i = x * 4 + y * bm.bmWidth * 4;
					pbuf[i] = (BYTE)((std::min)(pbuf[i] + 40, 255));
					pbuf[i + 1] = (BYTE)((std::min)(pbuf[i + 1] + 32, 255));
					pbuf[i + 2] = (BYTE)((std::min)(pbuf[i + 2] + 32, 255));
				}
			};
		lightenInner(radius, 1, bm.bmWidth - radius, radius);
		lightenInner(radius, bm.bmHeight - radius, bm.bmWidth - radius, bm.bmHeight - 1);
		lightenInner(1, radius, bm.bmWidth - 1, bm.bmHeight - radius);
		lightenCorner(0, 0, radius, radius, radius, radius, radius);
		lightenCorner(bm.bmWidth - radius, 0, bm.bmWidth, radius, bm.bmWidth - radius - 1, radius, radius);
		lightenCorner(0, bm.bmHeight - radius, radius, bm.bmHeight, radius, bm.bmHeight - radius - 1, radius);
		lightenCorner(bm.bmWidth - radius, bm.bmHeight - radius, bm.bmWidth, bm.bmHeight, bm.bmWidth - radius - 1, bm.bmHeight - radius - 1, radius);
		lightenTopBottomEdge(0, radius, bm.bmWidth - radius);
		lightenTopBottomEdge(bm.bmHeight - 1, radius, bm.bmWidth - radius);
		lightenLeftRightEdge(0, radius, bm.bmHeight - radius);
		lightenLeftRightEdge(bm.bmWidth - 1, radius, bm.bmHeight - radius);
	}

	SetDIBits(dcMem.m_hDC, (HBITMAP)*pBitmapDarkened, 0, bm.bmHeight, pbuf.get(), &bi, DIB_RGB_COLORS);
	dcMem.SelectObject(pOldBitmap);
	return pBitmapDarkened;
}

bool LoadImageFromResource(ATL::CImage& image, const tchar_t *pName, const tchar_t *pType)
{
	HRSRC hrsrc = FindResource(nullptr, pName, pType);
	if (hrsrc == nullptr)
		return false;
	DWORD dwResourceSize = SizeofResource(nullptr, hrsrc);
	HGLOBAL hglbImage = LoadResource(nullptr, hrsrc);
	if (hglbImage == nullptr)
		return false;
	LPVOID pvSourceResourceData = LockResource(hglbImage);
	if (pvSourceResourceData == nullptr)
		return false;
	IStream * pStream = SHCreateMemStream(reinterpret_cast<const BYTE *>(pvSourceResourceData), dwResourceSize);
	if (!pStream)
		return false;
	HRESULT hr = image.Load(pStream);
	pStream->Release();
	if (FAILED(hr))
		return false;
	return true;
}