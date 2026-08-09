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

static HBITMAP CreateHBITMAPFromPARGB(const BYTE* pPixels, int width, int height, int stride)
{
	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height; // top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* pBits = nullptr;
	HBITMAP hBitmap = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
	if (hBitmap == nullptr)
		return nullptr;

	if (pBits == nullptr)
	{
		DeleteObject(hBitmap);
		return nullptr;
	}

	BYTE* dst = reinterpret_cast<BYTE*>(pBits);
	for (int y = 0; y < height; ++y)
		memcpy(dst + y * width * 4, pPixels + y * stride, static_cast<size_t>(width) * 4);

	return hBitmap;
}

bool LoadPngResourceAndResize(HINSTANCE hInstance, int nIDResource, int nIconCount, int nNewWidth, int nNewHeight, HBITMAP* phBitmap, HBITMAP* phGrayscaleBitmap)
{
	if (nIconCount <= 0 || nNewWidth <= 0 || nNewHeight <= 0)
		return false;
	*phBitmap = nullptr;
	if (phGrayscaleBitmap)
		*phGrayscaleBitmap = nullptr;
	HRSRC resource = FindResource(hInstance, MAKEINTRESOURCE(nIDResource), _T("IMAGE"));
	if (resource == nullptr)
		return false;
	HGLOBAL resourceData = LoadResource(hInstance, resource);
	if (resourceData == nullptr)
		return false;
	const BYTE* resourceBytes = reinterpret_cast<const BYTE*>(LockResource(resourceData));
	if (resourceBytes == nullptr)
		return false;
	IStream* stream = SHCreateMemStream(resourceBytes, SizeofResource(hInstance, resource));
	if (stream == nullptr)
		return false;
	Gdiplus::Bitmap bitmapSrc(stream);
	stream->Release();
	if (bitmapSrc.GetLastStatus() != Gdiplus::Ok)
		return false;

	const int srcHeight = static_cast<int>(bitmapSrc.GetHeight());
	const int srcWidth = static_cast<int>(bitmapSrc.GetWidth());
	const int srcWidthPerIcon = srcWidth / nIconCount;
	const int newIconWidth = nNewWidth / nIconCount;
	const int resizedStride = nNewWidth * 4;
	std::vector<BYTE> resizedBuf(static_cast<size_t>(resizedStride) * nNewHeight);
	std::vector<BYTE> grayscaleBuf;
	if (phGrayscaleBitmap)
		grayscaleBuf.resize(resizedBuf.size());
	Gdiplus::Bitmap bitmapDst( nNewWidth, nNewHeight, nNewWidth * 4, PixelFormat32bppARGB, resizedBuf.data());
	Gdiplus::Graphics dcDst(&bitmapDst);
	dcDst.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
	dcDst.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
	dcDst.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);

	for (int i = 0; i < nIconCount; ++i)
		dcDst.DrawImage(&bitmapSrc, Gdiplus::Rect( i * newIconWidth, 0, newIconWidth, nNewHeight),
			i * srcWidthPerIcon, 0, srcWidthPerIcon, srcHeight, Gdiplus::UnitPixel);

	for (int y = 0; y < nNewHeight; ++y)
	{
		BYTE* row = resizedBuf.data() + y * resizedStride;
		BYTE* grow = phGrayscaleBitmap ? grayscaleBuf.data() + y * resizedStride : nullptr;
		for (int x = 0; x < nNewWidth; ++x)
		{
			BYTE* p = row + x * 4; // B, G, R, A (straight)
			const BYTE a = p[3];
			const BYTE pb = static_cast<BYTE>(p[0] * a / 255);
			const BYTE pg = static_cast<BYTE>(p[1] * a / 255);
			const BYTE pr = static_cast<BYTE>(p[2] * a / 255);
			p[0] = pb; p[1] = pg; p[2] = pr;

			if (grow)
			{
				BYTE* gp = grow + x * 4;
				const BYTE gray = static_cast<BYTE>(
					(static_cast<int>(0.114 * 256) * (((255 - pb) >> 1) + pb)
						+ static_cast<int>(0.587 * 256) * (((255 - pg) >> 1) + pg)
						+ static_cast<int>(0.299 * 256) * (((255 - pr) >> 1) + pr)) >> 8);
				gp[0] = gp[1] = gp[2] = gray;
				gp[3] = a;
			}
		}
	}

	*phBitmap = CreateHBITMAPFromPARGB(resizedBuf.data(), nNewWidth, nNewHeight, resizedStride);
	if (*phBitmap == nullptr)
		return false;

	if (phGrayscaleBitmap == nullptr)
		return true;

	*phGrayscaleBitmap = CreateHBITMAPFromPARGB(grayscaleBuf.data(), nNewWidth, nNewHeight, resizedStride);
	if (*phGrayscaleBitmap == nullptr)
	{
		DeleteObject(*phBitmap);
		*phBitmap = nullptr;
		return false;
	}
	return true;
}