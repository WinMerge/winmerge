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

#include "FreeImagePlus.h"
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>

template<class T> struct Point
{
	Point(T x, T y): x(x), y(y) {}
	T x, y;
};

template<class T> struct Size
{
	Size(T cx, T cy): cx(cx), cy(cy) {}
	T cx, cy;
};

template<class T> struct Rect
{
	Rect(T left, T top, T right, T bottom): left(left), top(top), right(right), bottom(bottom) {}
	T left, top, right, bottom;
};

template <class T> struct Array2D
{
	Array2D() : m_width(0), m_height(0), m_data(NULL)
	{
	}

	Array2D(size_t width, size_t height) : m_width(width), m_height(height), m_data(new T[width * height])
	{
		memset(m_data, 0, m_width * m_height * sizeof(T));
	}

	Array2D(const Array2D& other) : m_width(other.m_width), m_height(other.m_height), m_data(new T[other.m_width * other.m_height])
	{
		memcpy(m_data, other.m_data, m_width * m_height * sizeof(T));
	}

	Array2D& operator=(const Array2D& other)
	{
		if (this != &other)
		{
			delete[] m_data;
			m_width  = other.m_width;
			m_height = other.m_height;
			m_data = new T[other.m_width * other.m_height];
			memcpy(m_data, other.m_data, m_width * m_height * sizeof(T));
		}
		return *this;
	}

	~Array2D()
	{
		delete[] m_data;
	}

	void resize(size_t width, size_t height)
	{
		delete[] m_data;
		m_data = new T[width * height];
		m_width  = width;
		m_height = height;
		memset(m_data, 0, sizeof(T) * width * height);
	}

	T& operator()(int x, int y)
	{
		return m_data[y * m_width + x];
	}

	const T& operator()(int x, int y) const
	{
		return m_data[y * m_width + x];
	}

	void clear()
	{
		delete[] m_data;
		m_data = NULL;
		m_width = 0;
		m_height = 0;
	}

	size_t height() const
	{
		return m_height;
	}

	size_t width() const
	{
		return m_width;
	}

	size_t m_width, m_height;
	T* m_data;
};

struct DiffInfo
{
	enum OP_TYPE
	{
		OP_NONE = 0, OP_1STONLY, OP_2NDONLY, OP_3RDONLY, OP_DIFF, OP_TRIVIAL
	};
	DiffInfo(int op, int x, int y) : op(op), rc(x, y, x + 1, y + 1) {}
	int op;
	Rect<int> rc;
};

struct DiffStat { int d1, d2, d3, detc; };

struct UndoRecord
{
	UndoRecord(int pane, FIBITMAP *oldbitmap, FIBITMAP *newbitmap, const int modcountnew[3]) : 
		pane(pane), oldbitmap(oldbitmap), newbitmap(newbitmap)
	{
		for (int i = 0; i < 3; ++i)
			modcount[i] = modcountnew[i];
	}
	int pane;
	int modcount[3];
	FIBITMAP *oldbitmap, *newbitmap;
};

struct UndoRecords
{
	UndoRecords() : m_currentUndoBufIndex(-1)
	{
		clear();
	}

	~UndoRecords()
	{
		clear();
	}

	void push_back(int pane, FIBITMAP *oldbitmap, FIBITMAP *newbitmap)
	{
		++m_currentUndoBufIndex;
		while (m_currentUndoBufIndex < static_cast<int>(m_undoBuf.size()))
		{
			--m_modcount[m_undoBuf.back().pane];
			FreeImage_Unload(m_undoBuf.back().newbitmap);
			FreeImage_Unload(m_undoBuf.back().oldbitmap);
			m_undoBuf.pop_back();
		}
		++m_modcount[pane];
		m_undoBuf.push_back(UndoRecord(pane, oldbitmap, newbitmap, m_modcount));
	}

	const UndoRecord& undo()
	{
		if (m_currentUndoBufIndex < 0)
			throw "no undoable";
		const UndoRecord& rec = m_undoBuf[m_currentUndoBufIndex];
		--m_currentUndoBufIndex;
		return rec;
	}

	const UndoRecord& redo()
	{
		if (m_currentUndoBufIndex >= static_cast<int>(m_undoBuf.size()) - 1)
			throw "no redoable";
		++m_currentUndoBufIndex;
		const UndoRecord& rec = m_undoBuf[m_currentUndoBufIndex];
		return rec;
	}

	bool is_modified(int pane) const
	{
		if (m_currentUndoBufIndex < 0)
			return (m_modcountonsave[pane] != 0);
		else
			return (m_modcountonsave[pane] != m_undoBuf[m_currentUndoBufIndex].modcount[pane]);
	}

	void save(int pane)
	{
		if (m_currentUndoBufIndex < 0)
			m_modcountonsave[pane] = 0;
		else
			m_modcountonsave[pane] = m_undoBuf[m_currentUndoBufIndex].modcount[pane];
	}

	bool undoable() const
	{
		return (m_currentUndoBufIndex >= 0);
	}

	bool redoable() const
	{
		return (m_currentUndoBufIndex < static_cast<int>(m_undoBuf.size()) - 1);
	}

	void clear()
	{
		m_currentUndoBufIndex = -1;
		for (int i = 0; i < 3; ++i)
		{
			m_modcount[i] = 0;
			m_modcountonsave[i] = 0;
		}
		while (!m_undoBuf.empty())
		{
			FreeImage_Unload(m_undoBuf.back().newbitmap);
			FreeImage_Unload(m_undoBuf.back().oldbitmap);
			m_undoBuf.pop_back();
		}
	}

	std::vector<UndoRecord> m_undoBuf;
	int m_currentUndoBufIndex;
	int m_modcount[3];
	int m_modcountonsave[3];
};

#ifndef _WIN32
class fipWinImage : public fipImage
{
public:
	fipWinImage(FREE_IMAGE_TYPE image_type = FIT_BITMAP, unsigned width = 0, unsigned height = 0, unsigned bpp = 0) :
	  fipImage(image_type, width, height, bpp) {}
	fipWinImage(const fipWinImage& Image) : fipImage(Image) {}
	virtual ~fipWinImage() {}
};
#endif

class fipImageEx : public fipImage
{
public:
	fipImageEx(FREE_IMAGE_TYPE image_type = FIT_BITMAP, unsigned width = 0, unsigned height = 0, unsigned bpp = 0) :
	  fipImage(image_type, width, height, bpp) {}
	fipImageEx(const fipImageEx& Image) : fipImage(Image) {}
	virtual ~fipImageEx() {}

	fipImageEx& operator=(const fipImageEx& Image)
	{
		if (this != &Image)
		{
			FIBITMAP *clone = FreeImage_Clone((FIBITMAP*)Image._dib);
			replace(clone);
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
	fipMultiPageEx(BOOL keep_cache_in_memory = FALSE) : fipMultiPage(keep_cache_in_memory) {}
   
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

namespace
{
	int GetColorDistance2(RGBQUAD c1, RGBQUAD c2)
	{
		int rdist = c1.rgbRed - c2.rgbRed;
		int gdist = c1.rgbGreen - c2.rgbGreen;
		int bdist = c1.rgbBlue - c2.rgbBlue;
		int adist = c1.rgbReserved - c2.rgbReserved;
		return rdist * rdist + gdist * gdist + bdist * bdist + adist * adist;
	}
}

class CImgMergeBuffer
{
	typedef Array2D<int> DiffBlocks;

public:
	enum OVERLAY_MODE {
		OVERLAY_NONE = 0, OVERLAY_XOR, OVERLAY_ALPHABLEND
	};

	CImgMergeBuffer() : 
		  m_nImages(0)
		, m_showDifferences(true)
		, m_overlayMode(OVERLAY_NONE)
		, m_overlayAlpha(0.3)
		, m_diffBlockSize(8)
		, m_selDiffColor()
		, m_diffColor()
		, m_diffColorAlpha(0.7)
		, m_colorDistanceThreshold(0.0)
		, m_currentDiffIndex(-1)
		, m_diffCount(0)
	{
		m_selDiffColor.rgbRed = 0xff;
		m_selDiffColor.rgbGreen = 0x40;
		m_selDiffColor.rgbBlue = 0x40;
		m_diffColor.rgbRed   = 0xff;
		m_diffColor.rgbGreen = 0xff;
		m_diffColor.rgbBlue  = 0x40;
		for (int i = 0; i < 3; ++i)
		{
			m_currentPage[i] = 0;
			m_bRO[i] = false;
		}
	}

	~CImgMergeBuffer()
	{
		CloseImages();
	}

	const wchar_t *GetFileName(int pane)
	{
		if (pane < 0 || pane >= m_nImages)
			return NULL;
		return m_filename[pane].c_str();
	}

	int GetPaneCount() const
	{
		return m_nImages;
	}

	RGBQUAD GetPixelColor(int pane, int x, int y) const
	{
		RGBQUAD value = {0xFF, 0xFF, 0xFF, 0x00};
		m_imgOrig32[pane].getPixelColor(x, m_imgOrig32[pane].getHeight() - y - 1, &value);
		return value;
	}

	double GetColorDistance(int pane1, int pane2, int x, int y) const
	{
		return std::sqrt(static_cast<double>(
			::GetColorDistance2(GetPixelColor(pane1, x, y), GetPixelColor(pane2, x, y)) ));
	}

	bool GetReadOnly(int pane) const
	{
		if (pane < 0 || pane >= m_nImages)
			return true;
		return m_bRO[pane];
	}

	void SetReadOnly(int pane, bool readOnly)
	{
		if (pane < 0 || pane >= m_nImages)
			return;
		m_bRO[pane] = readOnly;
	}

	RGBQUAD GetDiffColor() const
	{
		return m_diffColor;
	}

	void SetDiffColor(RGBQUAD clrDiffColor)
	{
		m_diffColor = clrDiffColor;
		RefreshImages();
	}

	RGBQUAD GetSelDiffColor() const
	{
		return m_selDiffColor;
	}

	void SetSelDiffColor(RGBQUAD clrSelDiffColor)
	{
		m_selDiffColor = clrSelDiffColor;
		RefreshImages();
	}

	double GetDiffColorAlpha() const
	{
		return m_diffColorAlpha;
	}

	void SetDiffColorAlpha(double diffColorAlpha)
	{
		m_diffColorAlpha = diffColorAlpha;
		RefreshImages();
	}

	int  GetCurrentPage(int pane) const
	{
		if (pane < 0 || pane >= m_nImages)
			return -1;
		return m_currentPage[pane];
	}

	void SetCurrentPage(int pane, int page)
	{
		if (page >= 0 && page < GetPageCount(pane))
		{
			if (m_imgOrigMultiPage[pane].isValid())
			{
				m_currentPage[pane] = page;
				FIBITMAP *bitmap = m_imgOrigMultiPage[pane].lockPage(page);
				m_imgOrig[pane] = FreeImage_Clone(bitmap);
				m_imgOrig32[pane] = m_imgOrig[pane];
				FreeImage_UnlockPage(m_imgOrigMultiPage[pane], bitmap, false);
				m_imgOrig32[pane].convertTo32Bits();
				CompareImages();
			}
		}
	}

	void SetCurrentPageAll(int page)
	{
		for (int i = 0; i < m_nImages; ++i)
			SetCurrentPage(i, page);
	}

	int  GetCurrentMaxPage() const
	{
		int maxpage = 0;
		for (int i = 0; i < m_nImages; ++i)
		{
			int page = GetCurrentPage(i);
			maxpage = maxpage < page ? page : maxpage;
		}
		return maxpage;
	}

	int  GetPageCount(int pane) const
	{
		if (pane < 0 || pane >= m_nImages)
			return -1;
		if (m_imgOrigMultiPage[pane].isValid())
			return m_imgOrigMultiPage[pane].getPageCount();
		else
			return 1;
	}

	int  GetMaxPageCount() const
	{
		int maxpage = 0;
		for (int i = 0; i < m_nImages; ++i)
		{
			int page = GetPageCount(i);
			maxpage = page > maxpage ? page : maxpage;
		}
		return maxpage;
	}

	double GetColorDistanceThreshold() const
	{
		return m_colorDistanceThreshold;
	}

	void SetColorDistanceThreshold(double threshold)
	{
		m_colorDistanceThreshold = threshold;
		CompareImages();
	}

	int  GetDiffBlockSize() const
	{
		return m_diffBlockSize;
	}
	
	void SetDiffBlockSize(int blockSize)
	{
		m_diffBlockSize = blockSize;
		CompareImages();
	}

	OVERLAY_MODE GetOverlayMode() const
	{
		return m_overlayMode;
	}

	void SetOverlayMode(OVERLAY_MODE overlayMode)
	{
		m_overlayMode = overlayMode;
		RefreshImages();
	}

	double GetOverlayAlpha() const
	{
		return m_overlayAlpha;
	}

	void SetOverlayAlpha(double overlayAlpha)
	{
		m_overlayAlpha = overlayAlpha;
		RefreshImages();
	}

	bool GetShowDifferences() const
	{
		return m_showDifferences;
	}

	void SetShowDifferences(bool visible)
	{
		m_showDifferences = visible;
		CompareImages();
	}

	const DiffInfo *GetDiffInfo(int diffIndex) const
	{
		if (diffIndex < 0 || diffIndex >= m_diffCount)
			return NULL;
		return &m_diffInfos[diffIndex];
	}

	int  GetDiffCount() const
	{
		return m_diffCount;
	}

	int  GetConflictCount() const
	{
		int conflictCount = 0;
		for (int i = 0; i < m_diffCount; ++i)
			if (m_diffInfos[i].op == DiffInfo::OP_DIFF)
				++conflictCount;
		return conflictCount;
	}

	int  GetCurrentDiffIndex() const
	{
		return m_currentDiffIndex;
	}

	bool FirstDiff()
	{
		if (m_diffCount == 0)
			m_currentDiffIndex = -1;
		else
			m_currentDiffIndex = 0;
		RefreshImages();
		return true;
	}

	bool LastDiff()
	{
		m_currentDiffIndex = m_diffCount - 1;
		RefreshImages();
		return true;
	}

	bool NextDiff()
	{
		++m_currentDiffIndex;
		if (m_currentDiffIndex >= m_diffCount)
			m_currentDiffIndex = m_diffCount - 1;
		RefreshImages();
		return true;
	}

	bool PrevDiff()
	{
		if (m_diffCount == 0)
			m_currentDiffIndex = -1;
		else
		{
			--m_currentDiffIndex;
			if (m_currentDiffIndex < 0)
				m_currentDiffIndex = 0;
		}
		RefreshImages();
		return true;
	}

	bool FirstConflict()
	{
		for (size_t i = 0; i < m_diffInfos.size(); ++i)
			if (m_diffInfos[i].op == DiffInfo::OP_DIFF)
				m_currentDiffIndex = static_cast<int>(i);
		RefreshImages();
		return true;
	}

	bool LastConflict()
	{
		for (int i = static_cast<int>(m_diffInfos.size() - 1); i >= 0; --i)
		{
			if (m_diffInfos[i].op == DiffInfo::OP_DIFF)
			{
				m_currentDiffIndex = i;
				break;
			}
		}
		RefreshImages();
		return true;
	}

	bool NextConflict()
	{
		for (size_t i = m_currentDiffIndex + 1; i < m_diffInfos.size(); ++i)
		{
			if (m_diffInfos[i].op == DiffInfo::OP_DIFF)
			{
				m_currentDiffIndex = static_cast<int>(i);
				break;
			}
		}
		RefreshImages();
		return true;
	}

	bool PrevConflict()
	{
		for (int i = m_currentDiffIndex - 1; i >= 0; --i)
		{
			if (m_diffInfos[i].op == DiffInfo::OP_DIFF)
			{
				m_currentDiffIndex = i;
				break;
			}
		}
		RefreshImages();
		return true;
	}

	bool SelectDiff(int diffIndex)
	{
		m_currentDiffIndex = diffIndex;
		RefreshImages();
		return true;
	}
	
	int  GetNextDiffIndex() const
	{
		if (m_diffCount == 0 || m_currentDiffIndex >= m_diffCount - 1)
			return -1;
		return m_currentDiffIndex + 1;
	}

	int  GetPrevDiffIndex() const
	{
		if (m_diffCount == 0 || m_currentDiffIndex <= 0)
			return -1;
		return m_currentDiffIndex - 1;
	}

	int  GetNextConflictIndex() const
	{
		for (size_t i = m_currentDiffIndex + 1; i < m_diffInfos.size(); ++i)
			if (m_diffInfos[i].op == DiffInfo::OP_DIFF)
				return static_cast<int>(i);
		return -1;
	}

	int  GetPrevConflictIndex() const
	{
		for (int i = static_cast<int>(m_currentDiffIndex - 1); i >= 0; --i)
			if (m_diffInfos[i].op == DiffInfo::OP_DIFF)
				return i;
		return -1;
	}

	void CopyDiffInternal(int diffIndex, int srcPane, int dstPane)
	{
		if (srcPane < 0 || srcPane >= m_nImages)
			return;
		if (dstPane < 0 || dstPane >= m_nImages)
			return;
		if (diffIndex < 0 || diffIndex >= m_diffCount)
			return;
		if (m_bRO[dstPane])
			return;

		const Rect<int>& rc = m_diffInfos[diffIndex].rc;
		unsigned wsrc = m_imgOrig32[srcPane].getWidth();
		unsigned hsrc = m_imgOrig32[srcPane].getHeight();
		unsigned wdst = m_imgOrig32[dstPane].getWidth();
		unsigned hdst = m_imgOrig32[dstPane].getHeight();
		if (rc.right * m_diffBlockSize > wdst)
		{
			if ((std::max)(wsrc, wdst) < rc.right * m_diffBlockSize)
				wdst = (std::max)(wsrc, wdst);
			else
				wdst = rc.right * m_diffBlockSize;
		}
		if (rc.bottom * m_diffBlockSize > hdst)
		{
			if ((std::max)(hsrc, hdst) < rc.bottom * m_diffBlockSize)
				hdst = (std::max)(hsrc, hdst);
			else
				hdst = rc.bottom * m_diffBlockSize;
		}
		if (rc.right * m_diffBlockSize > wsrc)
			wdst = wsrc;
		if (rc.bottom * m_diffBlockSize > hsrc)
			hdst = hsrc;
		if (wdst != m_imgOrig32[dstPane].getWidth() || hdst != m_imgOrig32[dstPane].getHeight())
		{
			fipImage imgTemp = m_imgOrig32[srcPane];
			m_imgOrig32[dstPane].setSize(imgTemp.getImageType(), wdst, hdst, imgTemp.getBitsPerPixel());
			m_imgOrig32[dstPane].pasteSubImage(imgTemp, 0, 0);
		}
		
		for (unsigned y = rc.top * m_diffBlockSize; y < rc.bottom * m_diffBlockSize; y += m_diffBlockSize)
		{
			for (unsigned x = rc.left * m_diffBlockSize; x < rc.right * m_diffBlockSize; x += m_diffBlockSize)
			{
				if (m_diff(x / m_diffBlockSize, y / m_diffBlockSize) == diffIndex + 1)
				{
					int sizex = ((x + m_diffBlockSize) < wsrc) ? m_diffBlockSize : (wsrc - x);
					int sizey = ((y + m_diffBlockSize) < hsrc) ? m_diffBlockSize : (hsrc - y);
					if (sizex > 0 && sizey > 0)
					{
						for (int i = 0; i < sizey; ++i)
						{
							const BYTE *scanline_src = m_imgOrig32[srcPane].getScanLine(hsrc - (y + i) - 1);
							BYTE *scanline_dst = m_imgOrig32[dstPane].getScanLine(hdst - (y + i) - 1);
							memcpy(&scanline_dst[x * 4], &scanline_src[x * 4], sizex * 4);
						}
					}
				}
			}
		}
	}

	void CopyDiff(int diffIndex, int srcPane, int dstPane)
	{
		if (srcPane < 0 || srcPane >= m_nImages)
			return;
		if (dstPane < 0 || dstPane >= m_nImages)
			return;
		if (diffIndex < 0 || diffIndex >= m_diffCount)
			return;
		if (m_bRO[dstPane])
			return;
		if (srcPane == dstPane)
			return;

		FIBITMAP *oldbitmap = FreeImage_Clone(m_imgOrig32[dstPane]);

		CopyDiffInternal(diffIndex, srcPane, dstPane);

		FIBITMAP *newbitmap = FreeImage_Clone(m_imgOrig32[dstPane]);
		m_undoRecords.push_back(dstPane, oldbitmap, newbitmap);
		CompareImages();
	}

	void CopyDiffAll(int srcPane, int dstPane)
	{
		if (srcPane < 0 || srcPane >= m_nImages)
			return;
		if (dstPane < 0 || dstPane >= m_nImages)
			return;
		if (m_bRO[dstPane])
			return;
		if (srcPane == dstPane)
			return;

		FIBITMAP *oldbitmap = FreeImage_Clone(m_imgOrig32[dstPane]);

		for (int diffIndex = 0; diffIndex < m_diffCount; ++diffIndex)
			CopyDiffInternal(diffIndex, srcPane, dstPane);

		FIBITMAP *newbitmap = FreeImage_Clone(m_imgOrig32[dstPane]);
		m_undoRecords.push_back(dstPane, oldbitmap, newbitmap);
		CompareImages();
	}

	int CopyDiff3Way(int dstPane)
	{
		if (dstPane < 0 || dstPane >= m_nImages)
			return 0;
		if (m_bRO[dstPane])
			return 0;

		FIBITMAP *oldbitmap = FreeImage_Clone(m_imgOrig32[dstPane]);

		int nMerged = 0;
		for (int diffIndex = 0; diffIndex < m_diffCount; ++diffIndex)
		{
			int srcPane;
			switch (m_diffInfos[diffIndex].op)
			{
			case DiffInfo::OP_1STONLY:
				if (dstPane == 1)
					srcPane = 0;
				else
					srcPane = -1;
				break;
			case DiffInfo::OP_2NDONLY:
				if (dstPane != 1)
					srcPane = 1;
				else
					srcPane = -1;
				break;
			case DiffInfo::OP_3RDONLY:
				if (dstPane == 1)
					srcPane = 2;
				else
					srcPane = -1;
				break;
			case DiffInfo::OP_DIFF:
				srcPane = -1;
				break;
			}

			if (srcPane >= 0)
			{
				CopyDiffInternal(diffIndex, srcPane, dstPane);
				++nMerged;
			}
		}

		FIBITMAP *newbitmap = FreeImage_Clone(m_imgOrig32[dstPane]);
		m_undoRecords.push_back(dstPane, oldbitmap, newbitmap);
		CompareImages();

		return nMerged;
	}

	bool IsModified(int pane) const
	{
		return m_undoRecords.is_modified(pane);
	}

	bool IsUndoable() const
	{
		return m_undoRecords.undoable();
	}

	bool IsRedoable() const
	{
		return m_undoRecords.redoable();
	}

	bool Undo()
	{
		if (!m_undoRecords.undoable())
			return false;
		const UndoRecord& rec = m_undoRecords.undo();
		m_imgOrig32[rec.pane] = FreeImage_Clone(rec.oldbitmap);
		CompareImages();
		return true;
	}

	bool Redo()
	{
		if (!m_undoRecords.redoable())
			return false;
		const UndoRecord& rec = m_undoRecords.redo();
		m_imgOrig32[rec.pane] = FreeImage_Clone(rec.newbitmap);
		CompareImages();
		return true;
	}

	void CompareImages()
	{
		if (m_nImages <= 1)
			return;
		InitializeDiff();
		if (m_nImages == 2)
		{
			CompareImages2(0, 1, m_diff);
			m_diffCount = MarkDiffIndex(m_diff);
		}
		else if (m_nImages == 3)
		{
			CompareImages2(0, 1, m_diff01);
			CompareImages2(2, 1, m_diff21);
			CompareImages2(0, 2, m_diff02);
			Make3WayDiff(m_diff01, m_diff21, m_diff);
			m_diffCount = MarkDiffIndex3way(m_diff01, m_diff21, m_diff02, m_diff);
		}
		if (m_currentDiffIndex >= m_diffCount)
			m_currentDiffIndex = m_diffCount - 1;
		RefreshImages();
	}

	void RefreshImages()
	{
		if (m_nImages <= 1)
			return;
		InitializeDiffImages();
		for (int i = 0; i < m_nImages; ++i)
			CopyOriginalImageToDiffImage(i);
		void (CImgMergeBuffer::*func)(int src, int dst) = NULL;
		if (m_overlayMode == OVERLAY_ALPHABLEND)
			func = &CImgMergeBuffer::AlphaBlendImages2;
		else if (m_overlayMode == OVERLAY_XOR)
			func = &CImgMergeBuffer::XorImages2;
		if (func)
		{
			if (m_nImages == 2)
			{
				(this->*func)(1, 0);
				(this->*func)(0, 1);
			}
			else if (m_nImages == 3)
			{
				(this->*func)(1, 0);
				(this->*func)(0, 1);
				(this->*func)(2, 1);
				(this->*func)(1, 2);
			}
		}
		if (m_showDifferences)
		{
			for (int i = 0; i < m_nImages; ++i)
				MarkDiff(i, m_diff);
		}
	}

	bool OpenImages(int nImages, const wchar_t * const filename[3])
	{
		CloseImages();
		m_nImages = nImages;
		for (int i = 0; i < nImages; ++i)
			m_filename[i] = filename[i];
		return LoadImages();
	}

	bool SaveImage(int pane)
	{
		if (pane < 0 || pane >= m_nImages)
			return false;
		if (m_bRO[pane])
			return false;
		if (!m_undoRecords.is_modified(pane))
			return true;
		bool result = SaveImageAs(pane, m_filename[pane].c_str());
		if (result)
			m_undoRecords.save(pane);
		return result;
	}

	bool SaveImages()
	{
		for (int i = 0; i < m_nImages; ++i)
			if (!SaveImage(i))
				return false;
		return true;
	}

	bool SaveImageAs(int pane, const wchar_t *filename)
	{
		if (pane < 0 || pane >= m_nImages)
			return false;
		unsigned bpp =  m_imgOrig[pane].getBitsPerPixel();
		RGBQUAD palette[256];
		if (m_imgOrig[pane].getPaletteSize() > 0)
			memcpy(palette, m_imgOrig[pane].getPalette(), m_imgOrig[pane].getPaletteSize());
		m_imgOrig[pane] = m_imgOrig32[pane];
		m_imgOrig[pane].convertColorDepth(bpp, palette);
		if (m_imgOrigMultiPage[pane].isValid())
		{
			fipImageEx imgOrg, imgAdd;
			imgAdd = m_imgOrig[pane];
			imgOrg = m_imgOrigMultiPage[pane].lockPage(m_currentPage[pane]);
			imgAdd.copyAnimationMetadata(imgOrg);
			m_imgOrigMultiPage[pane].unlockPage(imgOrg, false);
			m_imgOrigMultiPage[pane].insertPage(m_currentPage[pane], imgAdd);
			imgAdd.detach();
			m_imgOrigMultiPage[pane].deletePage(m_currentPage[pane] + 1);
			return !!m_imgOrigMultiPage[pane].saveU(filename);
		}
		else
		{
#ifdef _WIN32
			return !!m_imgOrig[pane].saveU(filename);
#else
			char filenameA[260];
			snprintf(filenameA, sizeof(filenameA), "%ls", filename);
			return !!m_imgOrig[pane].save(filenameA);
#endif
		}
	}

	bool CloseImages()
	{
		for (int i = 0; i < m_nImages; ++i)
		{
			m_imgOrig[i].clear();
			m_imgOrig32[i].clear();
			m_undoRecords.clear();
		}
		m_nImages = 0;
		return true;
	}

	bool SaveDiffImageAs(int pane, const wchar_t *filename)
	{
		if (pane < 0 || pane >= m_nImages)
			return false;
#ifdef _WIN32
		return !!m_imgDiff[pane].saveU(filename);
#else
		char filenameA[260];
		snprintf(filenameA, sizeof(filenameA), "%ls", filename);
		return !!m_imgDiff[pane].save(filenameA);
#endif
	}

	int  GetImageWidth(int pane) const
	{
		if (pane < 0 || pane >= m_nImages)
			return -1;
		return m_imgOrig32[pane].getWidth();
	}

	int  GetImageHeight(int pane) const
	{
		if (pane < 0 || pane >= m_nImages)
			return -1;
		return m_imgOrig32[pane].getHeight();
	}

	int  GetImageBitsPerPixel(int pane) const
	{
		if (pane < 0 || pane >= m_nImages)
			return -1;
		return m_imgOrig[pane].getBitsPerPixel();
	}

	int GetDiffIndexFromPoint(int x, int y) const
	{
		if (x > 0 && y > 0 && 
			x < static_cast<int>(m_imgDiff[0].getWidth()) && 
			y < static_cast<int>(m_imgDiff[0].getHeight()))
		{
			return m_diff(x / m_diffBlockSize, y / m_diffBlockSize);
		}
		return -1;
	}

	fipWinImage *GetImage(int pane)
	{
		if (pane < 0 || pane >= m_nImages)
			return NULL;
		return &m_imgDiff[pane];
	}

private:
	bool LoadImages()
	{
		bool bSucceeded = true;
		for (int i = 0; i < m_nImages; ++i)
		{
			m_currentPage[i] = 0;
			m_imgOrigMultiPage[i].openU(m_filename[i].c_str(), FALSE, FALSE, 0);
			if (m_imgOrigMultiPage[i].isValid())
			{
				FIBITMAP *bitmap = m_imgOrigMultiPage[i].lockPage(m_currentPage[i]);
				if (bitmap)
				{
					m_imgOrig[i] = FreeImage_Clone(bitmap);
					m_imgOrig32[i] = m_imgOrig[i];
					FreeImage_UnlockPage(m_imgOrigMultiPage[i], bitmap, false);
				}
				else
					m_imgOrigMultiPage[i].close();
			}
			if (!m_imgOrigMultiPage[i].isValid())
			{
#ifdef _WIN32
				if (!m_imgOrig[i].loadU(m_filename[i].c_str()))
					bSucceeded = false;
#else
				char filename[260];
				snprintf(filename, sizeof(filename), "%ls", m_filename[i].c_str());
				if (!m_imgOrig[i].load(filename))
					bSucceeded = false;
#endif
				m_imgOrig32[i] = m_imgOrig[i];
			}

			m_imgOrig32[i].convertTo32Bits();
		}
		return bSucceeded;
	}

	Size<unsigned> GetMaxWidthHeight()
	{
		unsigned wmax = 0;
		unsigned hmax = 0;
		for (int i = 0; i < m_nImages; ++i)
		{
			wmax  = (std::max)(wmax,  m_imgOrig32[i].getWidth());
			hmax = (std::max)(hmax, m_imgOrig32[i].getHeight());
		}
		return Size<unsigned>(wmax, hmax);
	}

	void InitializeDiff()
	{
		Size<unsigned> size = GetMaxWidthHeight();
		int nBlocksX = (size.cx + m_diffBlockSize - 1) / m_diffBlockSize;
		int nBlocksY = (size.cy + m_diffBlockSize - 1) / m_diffBlockSize;

		m_diff.clear();
		m_diff.resize(nBlocksX, nBlocksY);
		if (m_nImages == 3)
		{
			m_diff01.clear();
			m_diff01.resize(nBlocksX, nBlocksY);
			m_diff21.clear();
			m_diff21.resize(nBlocksX, nBlocksY);
			m_diff02.clear();
			m_diff02.resize(nBlocksX, nBlocksY);
		}
		m_diffInfos.clear();
	}

	void InitializeDiffImages()
	{
		Size<unsigned> size = GetMaxWidthHeight();
		for (int i = 0; i < m_nImages; ++i)
			m_imgDiff[i].setSize(FIT_BITMAP, size.cx, size.cy, 32);
	}

	void CompareImages2(int pane1, int pane2, DiffBlocks& diff)
	{
		unsigned w1 = m_imgOrig32[pane1].getWidth();
		unsigned h1 = m_imgOrig32[pane1].getHeight();
		unsigned w2 = m_imgOrig32[pane2].getWidth();
		unsigned h2 = m_imgOrig32[pane2].getHeight();

		const unsigned wmax = (std::max)(w1, w2);
		const unsigned hmax = (std::max)(h1, h2);

		for (unsigned by = 0; by < diff.height(); ++by)
		{
			unsigned bsy = (hmax - by * m_diffBlockSize) >= m_diffBlockSize ? m_diffBlockSize : (hmax - by * m_diffBlockSize); 
			for (unsigned i = 0; i < bsy; ++i)
			{
				unsigned y = by * m_diffBlockSize + i;
				if (y >= h1 || y >= h2)
				{
					for (unsigned bx = 0; bx < wmax / m_diffBlockSize; ++bx)
						diff(bx, by) = -1;
				}
				else
				{
					const BYTE *scanline1 = m_imgOrig32[pane1].getScanLine(h1 - y - 1);
					const BYTE *scanline2 = m_imgOrig32[pane2].getScanLine(h2 - y - 1);
					if (w1 == w2 && m_colorDistanceThreshold == 0.0)
					{
						if (memcmp(scanline1, scanline2, w1 * 4) == 0)
							continue;
					}
					for (unsigned x = 0; x < wmax; ++x)
					{
						if (x >= w1 || x >= w2)
							diff(x / m_diffBlockSize, by) = -1;
						else
						{
							if (m_colorDistanceThreshold > 0.0)
							{
								int bdist = scanline1[x * 4 + 0] - scanline2[x * 4 + 0];
								int gdist = scanline1[x * 4 + 1] - scanline2[x * 4 + 1];
								int rdist = scanline1[x * 4 + 2] - scanline2[x * 4 + 2];
								int adist = scanline1[x * 4 + 3] - scanline2[x * 4 + 3];
								int colorDistance2 = rdist * rdist + gdist * gdist + bdist * bdist + adist * adist;
								if (colorDistance2 > m_colorDistanceThreshold * m_colorDistanceThreshold)
									diff(x / m_diffBlockSize, by) = -1;
							}
							else
							{
								if (scanline1[x * 4 + 0] != scanline2[x * 4 + 0] ||
									scanline1[x * 4 + 1] != scanline2[x * 4 + 1] ||
									scanline1[x * 4 + 2] != scanline2[x * 4 + 2] ||
									scanline1[x * 4 + 3] != scanline2[x * 4 + 3])
								{
									diff(x / m_diffBlockSize, by) = -1;
								}
							}
						}
					}
				}
			}
		}
	}
		
	void FloodFill8Directions(DiffBlocks& data, int x, int y, unsigned val)
	{
		std::vector<Point<int> > stack;
		stack.push_back(Point<int>(x, y));
		while (!stack.empty())
		{
			const Point<int>& pt = stack.back();
			const int x = pt.x;
			const int y = pt.y;
			stack.pop_back();
			if (data(x, y) != -1)
				continue;
			data(x, y) = val;
			if (x + 1 < static_cast<int>(data.width()))
			{
				stack.push_back(Point<int>(x + 1, y));
				if (y + 1 < static_cast<int>(data.height()))
					stack.push_back(Point<int>(x + 1, y + 1));
				if (y - 1 >= 0)
					stack.push_back(Point<int>(x + 1, y - 1));
			}
			if (x - 1 >= 0)
			{
				stack.push_back(Point<int>(x - 1, y));
				if (y + 1 < static_cast<int>(data.height()))
					stack.push_back(Point<int>(x - 1, y + 1));
				if (y - 1 >= 0)
					stack.push_back(Point<int>(x - 1, y - 1));
			}
			if (y + 1 < static_cast<int>(data.height()))
				stack.push_back(Point<int>(x, y + 1));
			if (y - 1 >= 0)
				stack.push_back(Point<int>(x, y - 1));
		}
	}

	int MarkDiffIndex(DiffBlocks& diff)
	{
		int diffCount = 0;
		for (unsigned by = 0; by < diff.height(); ++by)
		{
			for (unsigned bx = 0; bx < diff.width(); ++bx)
			{
				int idx = diff(bx, by);
				if (idx == -1)
				{
					m_diffInfos.push_back(DiffInfo(DiffInfo::OP_DIFF, bx, by));
					++diffCount;
					FloodFill8Directions(diff, bx, by, diffCount);
				}
				else if (idx != 0)
				{
					Rect<int>& rc = m_diffInfos[idx - 1].rc;
					if (static_cast<int>(bx) < rc.left)
						rc.left = bx;
					else if (static_cast<int>(bx + 1) > rc.right)
						rc.right = bx + 1;
					if (static_cast<int>(by) < rc.top)
						rc.top = by;
					else if (static_cast<int>(by + 1) > rc.bottom)
						rc.bottom = by + 1;
				}
			}
		}
		return diffCount;
	}

	int MarkDiffIndex3way(DiffBlocks& diff01, DiffBlocks& diff21, DiffBlocks& diff02, DiffBlocks& diff3)
	{
		int diffCount = MarkDiffIndex(diff3);
		std::vector<DiffStat> counter(m_diffInfos.size());
		for (unsigned by = 0; by < diff3.height(); ++by)
		{
			for (unsigned bx = 0; bx < diff3.width(); ++bx)
			{
				int diffIndex = diff3(bx, by);
				if (diffIndex == 0)
					continue;
				--diffIndex;
				if (diff21(bx, by) == 0)
					++counter[diffIndex].d1;
				else if (diff02(bx, by) == 0)
					++counter[diffIndex].d2;
				else if (diff01(bx, by) == 0)
					++counter[diffIndex].d3;
				else
					++counter[diffIndex].detc;
			}
		}
		
		for (size_t i = 0; i < m_diffInfos.size(); ++i)
		{
			int op;
			if (counter[i].d1 != 0 && counter[i].d2 == 0 && counter[i].d3 == 0 && counter[i].detc == 0)
				op = DiffInfo::OP_1STONLY;
			else if (counter[i].d1 == 0 && counter[i].d2 != 0 && counter[i].d3 == 0 && counter[i].detc == 0)
				op = DiffInfo::OP_2NDONLY;
			else if (counter[i].d1 == 0 && counter[i].d2 == 0 && counter[i].d3 != 0 && counter[i].detc == 0)
				op = DiffInfo::OP_3RDONLY;
			else
				op = DiffInfo::OP_DIFF;
			m_diffInfos[i].op = op;
		}
		return diffCount;
	}

	void Make3WayDiff(const DiffBlocks& diff01, const DiffBlocks& diff21, DiffBlocks& diff3)
	{
		diff3 = diff01;
		for (unsigned bx = 0; bx < diff3.width(); ++bx)
		{
			for (unsigned by = 0; by < diff3.height(); ++by)
			{
				if (diff21(bx, by) != 0)
					diff3(bx, by) = -1;
			}
		}
	}

	void MarkDiff(int pane, const DiffBlocks& diff)
	{
		const unsigned w = m_imgDiff[pane].getWidth();
		const unsigned h = m_imgDiff[pane].getHeight();

		for (unsigned by = 0; by < diff.height(); ++by)
		{
			for (unsigned bx = 0; bx < diff.width(); ++bx)
			{
				int diffIndex = diff(bx, by);
				if (diffIndex != 0 && (
					(pane == 0 && m_diffInfos[diffIndex - 1].op != DiffInfo::OP_3RDONLY) ||
					(pane == 1) ||
					(pane == 2 && m_diffInfos[diffIndex - 1].op != DiffInfo::OP_1STONLY)
					))
				{
					RGBQUAD color = (diffIndex - 1 == m_currentDiffIndex) ? m_selDiffColor : m_diffColor;
					unsigned bsy = (h - by * m_diffBlockSize < m_diffBlockSize) ? (h - by * m_diffBlockSize) : m_diffBlockSize;
					for (unsigned i = 0; i < bsy; ++i)
					{
						unsigned y = by * m_diffBlockSize + i;
						BYTE *scanline = m_imgDiff[pane].getScanLine(h - y - 1);
						unsigned bsx = (w - bx * m_diffBlockSize < m_diffBlockSize) ? (w - bx * m_diffBlockSize) : m_diffBlockSize;
						for (unsigned j = 0; j < bsx; ++j)
						{
							unsigned x = bx * m_diffBlockSize + j;
							scanline[x * 4 + 0] = static_cast<BYTE>(scanline[x * 4 + 0] * (1 - m_diffColorAlpha) + color.rgbBlue * m_diffColorAlpha);
							scanline[x * 4 + 1] = static_cast<BYTE>(scanline[x * 4 + 1] * (1 - m_diffColorAlpha) + color.rgbGreen * m_diffColorAlpha);
							scanline[x * 4 + 2] = static_cast<BYTE>(scanline[x * 4 + 2] * (1 - m_diffColorAlpha) + color.rgbRed * m_diffColorAlpha);
						}
					}
				}
			}
		}
	}

	void CopyOriginalImageToDiffImage(int dst)
	{
		unsigned w = m_imgOrig32[dst].getWidth();
		unsigned h = m_imgOrig32[dst].getHeight();
		for (unsigned y = 0; y < h; ++y)
		{
			const BYTE *scanline_src = m_imgOrig32[dst].getScanLine(h - y - 1);
			BYTE *scanline_dst = m_imgDiff[dst].getScanLine(m_imgDiff[dst].getHeight() - y - 1);
			for (unsigned x = 0; x < w; ++x)
			{
				scanline_dst[x * 4 + 0] = scanline_src[x * 4 + 0];
				scanline_dst[x * 4 + 1] = scanline_src[x * 4 + 1];
				scanline_dst[x * 4 + 2] = scanline_src[x * 4 + 2];
				scanline_dst[x * 4 + 3] = scanline_src[x * 4 + 3];
			}
		}	
	}

	void XorImages2(int src, int dst)
	{
		unsigned w = m_imgOrig32[src].getWidth();
		unsigned h = m_imgOrig32[src].getHeight();
		for (unsigned y = 0; y < h; ++y)
		{
			const BYTE *scanline_src = m_imgOrig32[src].getScanLine(h - y - 1);
			BYTE *scanline_dst = m_imgDiff[dst].getScanLine(m_imgDiff[dst].getHeight() - y - 1);
			for (unsigned x = 0; x < w; ++x)
			{
				scanline_dst[x * 4 + 0] ^= scanline_src[x * 4 + 0];
				scanline_dst[x * 4 + 1] ^= scanline_src[x * 4 + 1];
				scanline_dst[x * 4 + 2] ^= scanline_src[x * 4 + 2];
			}
		}	
	}

	void AlphaBlendImages2(int src, int dst)
	{
		unsigned w = m_imgOrig32[src].getWidth();
		unsigned h = m_imgOrig32[src].getHeight();
		for (unsigned y = 0; y < h; ++y)
		{
			const BYTE *scanline_src = m_imgOrig32[src].getScanLine(h - y - 1);
			BYTE *scanline_dst = m_imgDiff[dst].getScanLine(m_imgDiff[dst].getHeight() - y - 1);
			for (unsigned x = 0; x < w; ++x)
			{
				scanline_dst[x * 4 + 0] = static_cast<BYTE>(scanline_dst[x * 4 + 0] * (1 - m_overlayAlpha) + scanline_src[x * 4 + 0] * m_overlayAlpha);
				scanline_dst[x * 4 + 1] = static_cast<BYTE>(scanline_dst[x * 4 + 1] * (1 - m_overlayAlpha) + scanline_src[x * 4 + 1] * m_overlayAlpha);
				scanline_dst[x * 4 + 2] = static_cast<BYTE>(scanline_dst[x * 4 + 2] * (1 - m_overlayAlpha) + scanline_src[x * 4 + 2] * m_overlayAlpha);
				scanline_dst[x * 4 + 3] = static_cast<BYTE>(scanline_dst[x * 4 + 3] * (1 - m_overlayAlpha) + scanline_src[x * 4 + 3] * m_overlayAlpha);
			}
		}	
	}

	int m_nImages;
	fipMultiPageEx m_imgOrigMultiPage[3];
	fipImageEx m_imgOrig[3];
	fipImageEx m_imgOrig32[3];
	fipWinImage m_imgDiff[3];
	std::wstring m_filename[3];
	bool m_bRO[3];
	bool m_showDifferences;
	OVERLAY_MODE m_overlayMode;
	double m_overlayAlpha;
	unsigned m_diffBlockSize;
	RGBQUAD m_selDiffColor;
	RGBQUAD m_diffColor;
	double   m_diffColorAlpha;
	double   m_colorDistanceThreshold;
	int m_currentPage[3];
	int m_currentDiffIndex;
	int m_diffCount;
	DiffBlocks m_diff, m_diff01, m_diff21, m_diff02;
	std::vector<DiffInfo> m_diffInfos;
	UndoRecords m_undoRecords;
};

