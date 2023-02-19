// SPDX-License-Identifier: BSL-1.0
// Copyright (c) 2020 Takashi Sawanaka
//
// Use, modification and distribution are subject to the 
// Boost Software License, Version 1.0. (See accompanying file 
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "StdAfx.h"
#include "hqbitmap.h"
#include <atlimage.h>
#include <vector>

static void convert24bitImageTo32bit(int width, int height, bool grayscale, COLORREF mask, const BYTE* src, BYTE* dst)
{
	const BYTE maskR = GetRValue(mask);
	const BYTE maskG = GetGValue(mask);
	const BYTE maskB = GetBValue(mask);
	for (int y = 0; y < height; ++y)
	{
		const BYTE* pSrc = src + y * ((width * 3 * 4 + 3) / 4);
		BYTE* pDst = dst + (height - 1 - y) * ((width * 4 * 4 + 3) / 4);
		if (!grayscale)
		{
			for (int x = 0; x < width; ++x)
			{
				if (pSrc[x * 3] == maskR && pSrc[x * 3 + 1] == maskG && pSrc[x * 3 + 2] == maskB)
				{
					pDst[x * 4] = pDst[x * 4 + 1] = pDst[x * 4 + 2] = pDst[x * 4 + 3] = 0;
				}
				else
				{
					pDst[x * 4 + 0] = pSrc[x * 3 + 0];
					pDst[x * 4 + 1] = pSrc[x * 3 + 1];
					pDst[x * 4 + 2] = pSrc[x * 3 + 2];
					pDst[x * 4 + 3] = 0xff;
				}
			}
		}
		else
		{
			for (int x = 0; x < width; ++x)
			{
				if (pSrc[x * 3] == 0xff && pSrc[x * 3 + 1] == 0 && pSrc[x * 3 + 2] == 0xff) // rgb(0xff, 0, 0xff) == mask color
				{
					pDst[x * 4] = pDst[x * 4 + 1] = pDst[x * 4 + 2] = pDst[x * 4 + 3] = 0;
				}
				else
				{
					const BYTE b = pSrc[x * 3];
					const BYTE g = pSrc[x * 3 + 1];
					const BYTE r = pSrc[x * 3 + 2];
					const BYTE gray = static_cast<BYTE>(
						(static_cast<int>(0.114 * 256) * (((255 - b) >> 1) + b)
							+ static_cast<int>(0.587 * 256) * (((255 - g) >> 1) + g)
							+ static_cast<int>(0.299 * 256) * (((255 - r) >> 1) + r)) >> 8);
					pDst[x * 4 + 0] = pDst[x * 4 + 1] = pDst[x * 4 + 2] = gray;
					pDst[x * 4 + 3] = 0xff;
				}
			}
		}
	}
};

static void convert4bitImageTo32bit(int width, int height, bool grayscale, COLORREF mask, ATL::CImage& image, BYTE* dst)
{
	BYTE* src = (BYTE* )image.GetBits();
	RGBQUAD rgbColors[16];
	RGBQUAD rgbMask{ GetRValue(mask), GetGValue(mask), GetBValue(mask), 0 };
	image.GetColorTable(0, 16, rgbColors);
	for (int y = 0; y < height; ++y)
	{
		const BYTE* pSrc = src + y * ((width + 1) / 2);
		BYTE* pDst = dst + (height - 1 - y) * ((width * 4 * 4 + 3) / 4);
		if (!grayscale)
		{
			for (int x = 0; x < width; ++x)
			{
				RGBQUAD rgba = ((x % 2) == 0) ? rgbColors[pSrc[x / 2] >> 4] : rgbColors[pSrc[x / 2] & 0xf];
				if (*((DWORD *)&rgba) == *((DWORD *)&rgbMask))
				{
					pDst[x * 4] = pDst[x * 4 + 1] = pDst[x * 4 + 2] = pDst[x * 4 + 3] = 0;
				}
				else
				{
					pDst[x * 4 + 0] = rgba.rgbBlue;
					pDst[x * 4 + 1] = rgba.rgbGreen;
					pDst[x * 4 + 2] = rgba.rgbRed;
					pDst[x * 4 + 3] = 0xff;
				}
			}
		}
		else
		{
			for (int x = 0; x < width; ++x)
			{
				RGBQUAD rgba = ((x % 2) == 0) ? rgbColors[pSrc[x / 2] >> 4] : rgbColors[pSrc[x / 2] & 0xf];
				if (*((DWORD *)&rgba) == *((DWORD *)&rgbMask))
				{
					pDst[x * 4] = pDst[x * 4 + 1] = pDst[x * 4 + 2] = pDst[x * 4 + 3] = 0;
				}
				else
				{
					const BYTE gray = static_cast<BYTE>(
						(static_cast<int>(0.114 * 256) * (((255 - rgba.rgbBlue) >> 1) + rgba.rgbBlue)
							+ static_cast<int>(0.587 * 256) * (((255 - rgba.rgbGreen) >> 1) + rgba.rgbGreen)
							+ static_cast<int>(0.299 * 256) * (((255 - rgba.rgbRed) >> 1) + rgba.rgbRed)) >> 8);
					pDst[x * 4 + 0] = pDst[x * 4 + 1] = pDst[x * 4 + 2] = gray;
					pDst[x * 4 + 3] = 0xff;
				}
			}
		}
	}
};

HBITMAP LoadBitmapAndConvertTo32bit(HINSTANCE hInstance, int nIDResource, int nNewWidth, int nNewHeight, bool bGrayscale, COLORREF clrMask)
{
	ATL::CImage img;
	img.Attach((HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(nIDResource), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION), ATL::CImage::DIBOR_TOPDOWN);
	const int nWidth = img.GetWidth();
	const int nHeight= img.GetHeight();
	const int stride = (nWidth * 4 * 4 + 3) / 4;
	std::vector<BYTE> buf(static_cast<size_t>(stride) * nHeight);
	switch (img.GetBPP())
	{
	case 24:
		convert24bitImageTo32bit(nWidth, nHeight, bGrayscale, clrMask, reinterpret_cast<BYTE*>(img.GetBits()), buf.data());
		break;
	case 4:
		convert4bitImageTo32bit(nWidth, nHeight, bGrayscale, clrMask, img, buf.data());
		break;
	default:
		ASSERT(true);
		break;
	}
	HBITMAP hBitmap = nullptr;

	if (nWidth != nNewWidth && nHeight != nNewHeight)
	{
		Gdiplus::Bitmap bitmapSrc(nWidth, nHeight, stride, PixelFormat32bppPARGB, buf.data());
		Gdiplus::Bitmap bitmapDst(nNewWidth, nNewHeight, PixelFormat32bppPARGB);
		Gdiplus::Graphics dcDst(&bitmapDst);
		dcDst.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
		dcDst.DrawImage(&bitmapSrc, 0, 0, nNewWidth, nNewHeight);
		bitmapDst.GetHBITMAP(Gdiplus::Color::Transparent, &hBitmap);
	}
	else
	{
		Gdiplus::Bitmap bitmapDst(nWidth, nHeight, stride, PixelFormat32bppPARGB, buf.data());
		bitmapDst.GetHBITMAP(Gdiplus::Color::Transparent, &hBitmap);
	}
	return hBitmap;
}

