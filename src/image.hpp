/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////

#pragma once
#pragma warning(disable: 4819)

#include "FreeImagePlus.h"
#include <algorithm>
#include <string>

#ifndef _WIN32
typedef fipImage fipWinImage;
#endif

class fipImageEx : public fipWinImage
{
public:
	fipImageEx(FREE_IMAGE_TYPE image_type = FIT_BITMAP, unsigned width = 0, unsigned height = 0, unsigned bpp = 0) :
	fipWinImage(image_type, width, height, bpp) {}
	fipImageEx(const fipImageEx& Image) : fipWinImage(Image) {}
	explicit fipImageEx(FIBITMAP *bitmap) { *this = bitmap; }
	virtual ~fipImageEx() {}

	fipImageEx& operator=(const fipImageEx& Image)
	{
		if (this != &Image)
		{
			FIBITMAP *clone = FreeImage_Clone(static_cast<FIBITMAP*>(Image._dib));
			if (clone)
			{
				replace(clone);
			}
			else
			{
				FreeImage_Unload(_dib);
				_dib = NULL;
				_bHasChanged = TRUE;
			}
		}
		return *this;
	}

	fipImageEx& operator=(FIBITMAP *dib)
	{
		if (_dib != dib)
			replace(dib);
		return *this;
	}

	void swap(fipImageEx& other)
	{
		std::swap(_dib, other._dib);
		std::swap(this->_fif, other._fif);
		std::swap(this->_bHasChanged, other._bHasChanged);
	}

	FIBITMAP *detach()
	{
		FIBITMAP *dib = _dib;
		_dib = NULL;
		clear();
		return dib;
	}

	BOOL colorQuantizeEx(FREE_IMAGE_QUANTIZE quantize = FIQ_WUQUANT, int PaletteSize = 256, int ReserveSize = 0, RGBQUAD *ReservePalette = NULL)
	{
		if(_dib) {
			FIBITMAP *dib8 = FreeImage_ColorQuantizeEx(_dib, quantize, PaletteSize, ReserveSize, ReservePalette);
			return !!replace(dib8);
		}
		return false;
	}

	bool convertColorDepth(unsigned bpp, RGBQUAD *pPalette = NULL)
	{
		switch (bpp)
		{
		case 1:
			return !!threshold(128);
		case 4:
		{
			fipImageEx tmp = *this;
			tmp.convertTo24Bits();
			if (pPalette)
				tmp.colorQuantizeEx(FIQ_NNQUANT, 16, 16, pPalette);
			else
				tmp.colorQuantizeEx(FIQ_WUQUANT, 16);
			setSize(tmp.getImageType(), tmp.getWidth(), tmp.getHeight(), 4);
			for (unsigned y = 0; y < tmp.getHeight(); ++y)
			{
				const BYTE *line_src = tmp.getScanLine(y);
				BYTE *line_dst = getScanLine(y);
				for (unsigned x = 0; x < tmp.getWidth(); ++x)
					line_dst[x / 2] |= ((x % 2) == 0) ? (line_src[x] << 4) : line_src[x];
			}

			RGBQUAD *rgbq_dst = getPalette();
			RGBQUAD *rgbq_src = pPalette ? pPalette : tmp.getPalette();
			memcpy(rgbq_dst, rgbq_src, sizeof(RGBQUAD) * 16);
			return true;
		}
		case 8:
			convertTo24Bits();
			if (pPalette)
				return !!colorQuantizeEx(FIQ_NNQUANT, 256, 256, pPalette);
			else
				return !!colorQuantizeEx(FIQ_WUQUANT, 256);
		case 15:
			return !!convertTo16Bits555();
		case 16:
			return !!convertTo16Bits565();
		case 24:
			return !!convertTo24Bits();
		default:
		case 32:
			return !!convertTo32Bits();
		}
	}

	void copyAnimationMetadata(fipImage& src)
	{
		fipTag tag;
		fipMetadataFind finder;
		if (finder.findFirstMetadata(FIMD_ANIMATION, src, tag))
		{
			do
			{
				setMetadata(FIMD_ANIMATION, tag.getKey(), tag);
			} while (finder.findNextMetadata(tag));
		}
	}
};

class fipMultiPageEx : public fipMultiPage
{
public:
	explicit fipMultiPageEx(BOOL keep_cache_in_memory = FALSE) : fipMultiPage(keep_cache_in_memory) {}
   
	BOOL openU(const wchar_t* lpszPathName, BOOL create_new, BOOL read_only, int flags = 0)
	{
		char filename[260];
#ifdef _WIN32
		wchar_t shortname[260] = {0};
		GetShortPathNameW(lpszPathName, shortname, sizeof(shortname)/sizeof(shortname[0]));
		wsprintfA(filename, "%S", shortname);
#else
		snprintf(filename, sizeof(filename), "%ls", lpszPathName);
		
#endif
		BOOL result = open(filename, create_new, read_only, flags);
		return result;
	}

	bool saveU(const wchar_t* lpszPathName, int flag = 0) const
	{
		FILE *fp = NULL;
#ifdef _WIN32
		_wfopen_s(&fp, lpszPathName, L"r+b");
#else
		char filename[260];
		snprintf(filename, sizeof(filename), "%ls", lpszPathName);
		fp = fopen(filename, "r+b");
#endif
		if (!fp)
			return false;
		FreeImageIO io;
		io.read_proc  = myReadProc;
		io.write_proc = myWriteProc;
		io.seek_proc  = mySeekProc;
		io.tell_proc  = myTellProc;
		FREE_IMAGE_FORMAT fif = fipImage::identifyFIFU(lpszPathName);
		bool result = !!saveToHandle(fif, &io, (fi_handle)fp, flag);
		fclose(fp);
		return result;
	}

private:
	static unsigned DLL_CALLCONV myReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
		return (unsigned)fread(buffer, size, count, (FILE *)handle);
	}

	static unsigned DLL_CALLCONV myWriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
		return (unsigned)fwrite(buffer, size, count, (FILE *)handle);
	}

	static int DLL_CALLCONV mySeekProc(fi_handle handle, long offset, int origin) {
		return fseek((FILE *)handle, offset, origin);
	}

	static long DLL_CALLCONV myTellProc(fi_handle handle) {
		return ftell((FILE *)handle);
	}
};

class MultiPageImages;

class Image
{
	friend MultiPageImages;
public:
	typedef RGBQUAD Color;
	Image() {}
	Image(int w, int h) : image_(FIT_BITMAP, w, h, 32) {}
	Image(const Image& other) : image_(other.image_) {}
	explicit Image(FIBITMAP *bitmap) : image_(bitmap) {}
	BYTE *scanLine(int y) { return image_.getScanLine(image_.getHeight() - y - 1); }
	const BYTE *scanLine(int y) const { return image_.getScanLine(image_.getHeight() - y - 1); }
	bool convertTo32Bits() { return !!image_.convertTo32Bits(); }
	bool load(const std::wstring& filename) { return !!image_.loadU(filename.c_str()); }
	bool save(const std::wstring& filename)
	{
#ifdef _WIN32
		return !!image_.saveU(filename.c_str());
#else
		char filenameA[260];
		snprintf(filenameA, sizeof(filenameA), "%ls", filename.c_str());
		return !!image_.save(filenameA);
#endif
	}
	int depth() const { return image_.getBitsPerPixel(); }
	unsigned width() const  { return image_.getWidth(); }
	unsigned height() const { return image_.getHeight(); }
	void clear() { image_.clear(); }
	void setSize(int w, int h) { image_.setSize(FIT_BITMAP, w, h, 32); }
	const fipImageEx *getImage() const { return &image_; }
	fipImageEx *getFipImage() { return &image_; }
	Color pixel(int x, int y) const
	{
		RGBQUAD color = {0};
		color.rgbReserved = 0xFF;
		image_.getPixelColor(x, image_.getHeight() - y - 1, &color);
		return color;
	}
	bool pasteSubImage(Image& image, int x, int y)
	{
		return !!image_.pasteSubImage(image.image_, x, y);
	}
	bool pullImageKeepingBPP(const Image& other)
	{
		unsigned bpp =  image_.getBitsPerPixel();
		RGBQUAD palette[256];
		if (image_.getPaletteSize() > 0)
			memcpy(palette, image_.getPalette(), image_.getPaletteSize());
		image_ = other.image_;
		return image_.convertColorDepth(bpp, palette);
	}

	static int valueR(Color color) { return color.rgbRed; }
	static int valueG(Color color) { return color.rgbGreen; }
	static int valueB(Color color) { return color.rgbBlue; }
	static int valueA(Color color) { return color.rgbReserved; }
	static Color Rgb(int r, int g, int b)
	{
		Color color;
		color.rgbRed = r;
		color.rgbGreen = g;
		color.rgbBlue = b;
		return color;
	}
private:
	fipImageEx image_;
};

class MultiPageImages
{
public:
	MultiPageImages() {}
	~MultiPageImages() { multi_.close(); }
	bool close() { return !!multi_.close(); }
	bool isValid() const { return !!multi_.isValid(); }
	int getPageCount() const { return multi_.getPageCount(); }
	bool load(const std::wstring& filename) { return !!multi_.openU(filename.c_str(), FALSE, FALSE); }
	bool save(const std::wstring& filename) { return !!multi_.saveU(filename.c_str()); }
	Image getImage(int page)
	{
		FIBITMAP *bitmaptmp, *bitmap;
		bitmaptmp = FreeImage_LockPage(multi_, page);
		bitmap = FreeImage_Clone(bitmaptmp);
		FreeImage_UnlockPage(multi_, bitmaptmp, false);
		return Image(bitmap);
	}
	void replacePage(int page, const Image& image)
	{
		fipImageEx imgOrg, imgAdd;
		imgAdd = image.image_;
		imgOrg = multi_.lockPage(page);
		imgAdd.copyAnimationMetadata(imgOrg);
		multi_.unlockPage(imgOrg, false);
		multi_.insertPage(page, imgAdd);
		imgAdd.detach();
		multi_.deletePage(page + 1);
	}

	fipMultiPageEx multi_;
};
