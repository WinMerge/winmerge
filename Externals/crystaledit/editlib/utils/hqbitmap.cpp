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

bool LoadPngResourceAndResize(HINSTANCE hInstance, int nIDResource, int nNewWidth, int nNewHeight, HBITMAP* phBitmap, HBITMAP* phGrayscaleBitmap)
{
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
	if (bitmapSrc.GetLastStatus() != Gdiplus::Ok)
	{
		stream->Release();
		return false;
	}

	const int resizedStride = nNewWidth * 4;
	std::vector<BYTE> resizedBuf(static_cast<size_t>(resizedStride) * nNewHeight);
	Gdiplus::Bitmap bitmapDst(nNewWidth, nNewHeight, resizedStride, PixelFormat32bppPARGB, resizedBuf.data());
	Gdiplus::Graphics dcDst(&bitmapDst);
	dcDst.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQualityBicubic);
	dcDst.DrawImage(&bitmapSrc, 0, 0, nNewWidth, nNewHeight);

	if (bitmapDst.GetHBITMAP(Gdiplus::Color::Transparent, phBitmap) != Gdiplus::Ok)
	{
		stream->Release();
		return false;
	}

	if (phGrayscaleBitmap == nullptr)
	{
		stream->Release();
		return true;
	}

	std::vector<BYTE> grayscaleBuf = resizedBuf;
	for (int y = 0; y < nNewHeight; ++y)
	{
		BYTE* pRow = grayscaleBuf.data() + y * resizedStride;
		for (int x = 0; x < nNewWidth; ++x)
		{
			BYTE* pPixel = pRow + x * 4;
			const BYTE b = pPixel[0];
			const BYTE g = pPixel[1];
			const BYTE r = pPixel[2];
			const BYTE gray = static_cast<BYTE>(
				(static_cast<int>(0.114 * 256) * (((255 - b) >> 1) + b)
					+ static_cast<int>(0.587 * 256) * (((255 - g) >> 1) + g)
					+ static_cast<int>(0.299 * 256) * (((255 - r) >> 1) + r)) >> 8);
			pPixel[0] = pPixel[1] = pPixel[2] = gray;
		}
	}

	Gdiplus::Bitmap grayscaleBitmap(nNewWidth, nNewHeight, resizedStride, PixelFormat32bppPARGB, grayscaleBuf.data());
	if (grayscaleBitmap.GetHBITMAP(Gdiplus::Color::Transparent, phGrayscaleBitmap) != Gdiplus::Ok)
	{
		DeleteObject(*phBitmap);
		*phBitmap = nullptr;
		stream->Release();
		return false;
	}
	stream->Release();
	return true;
}
