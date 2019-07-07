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
	return pBitmapDarkened;
}

bool GrayScale(CBitmap *pBitmap)
{
	BITMAP bm;
	pBitmap->GetBitmap(&bm);
	const int nCount = bm.bmWidthBytes * bm.bmHeight;
	const int bypp = bm.bmBitsPixel / 8;
	std::unique_ptr<BYTE[]> pbuf(new BYTE[nCount]);
	pBitmap->GetBitmapBits(nCount, pbuf.get());
	if (bm.bmBitsPixel >= 24)
	{
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
	}
	else if (bm.bmBitsPixel == 16)
	{
		static const int RGB565Table5[32] = { 0, 8, 16, 25, 33, 41, 49, 58, 66, 74, 82, 90, 99, 107, 115, 123, 132, 140, 148, 156, 165, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255 };
		static const int RGB565Table6[64] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 130, 134, 138, 142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190, 194, 198, 202, 206, 210, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255 };
		for (int i = 0, x = 0, y = 0; i < nCount - bypp; i += bypp, x += bypp)
		{
			const WORD w = (pbuf[i + 1] << 8) | pbuf[i];
			const BYTE b = static_cast<BYTE>(RGB565Table5[(w & 0xf800) >> 11]);
			const BYTE g = static_cast<BYTE>(RGB565Table6[(w & 0x07e0) >> 5]);
			const BYTE r = static_cast<BYTE>(RGB565Table5[w & 0x001f]);
			const BYTE gray = static_cast<BYTE>(
				 (static_cast<int>(0.114 * 256) * (((255 - b) >> 1) + b)
				+ static_cast<int>(0.587 * 256) * (((255 - g) >> 1) + g)
				+ static_cast<int>(0.299 * 256) * (((255 - r) >> 1) + r)) >> 8);
			const WORD wo = ((gray >> 3) << 11) | ((gray >> 2) << 5) | ((gray >> 3) << 0);
			pbuf[i] = static_cast<BYTE>(wo & 0xff);
			pbuf[i + 1] = static_cast<BYTE>((wo >> 8));
			if (x >= bm.bmWidthBytes - bypp)
			{
				++y;
				x = 0;
				i = y * bm.bmWidthBytes;
			}
		}
	}
	else
		return false;
	return pBitmap->SetBitmapBits(nCount, pbuf.get()) != 0;
}

bool LoadImageFromResource(ATL::CImage& image, const TCHAR *pName, const TCHAR *pType)
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