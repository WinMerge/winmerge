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

#include <Windows.h>
#include <string>
#include <algorithm>
#include <cstdio>
#include "FreeImagePlus.h"
#include "CImgWindow.hpp"
#include "WinIMergeLib.h"

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

template <class T> struct Array2D
{
	Array2D() : m_width(0), m_height(0), m_data(NULL)
	{
	}

	Array2D(size_t width, size_t height) : m_width(width), m_height(height), m_data(new T[width * height])
	{
		memset(m_data, 0, m_width * m_height * sizeof(T));
	}

	Array2D(const Array2D<T>& other) : m_width(other.m_width), m_height(other.m_height), m_data(new T[other.m_width * other.m_height])
	{
		memcpy(m_data, other.m_data, m_width * m_height * sizeof(T));
	}

	Array2D& operator=(const Array2D& other)
	{
		delete m_data;
		m_width  = other.m_width;
		m_height = other.m_height;
		m_data = new T[other.m_width * other.m_height];
		memcpy(m_data, other.m_data, m_width * m_height * sizeof(T));
		return *this;
	}

	~Array2D()
	{
		delete m_data;
	}

	void resize(size_t width, size_t height)
	{
		delete m_data;
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
		delete m_data;
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

class CImgMergeWindow : public IImgMergeWindow
{
public:
	CImgMergeWindow() : 
		  m_nImages(0)
		, m_hWnd(NULL)
		, m_hInstance(NULL)
		, m_nDraggingSplitter(-1)
		, m_bHorizontalSplit(false)
		, m_diffBlockSize(16)
		, m_overlayMode(OVERLAY_NONE)
		, m_overlayAlpha(0.3)
		, m_showDifferences(true)
		, m_selDiffColor(RGB(0xff, 0x40, 0x40))
		, m_diffColor(RGB(0xff, 0xff, 0x40))
		, m_currentDiffIndex(-1)
	{}

	~CImgMergeWindow()
	{
	}

	bool Create(HINSTANCE hInstance, HWND hWndParent, const RECT &rc)
	{
		m_hInstance = hInstance;
		MyRegisterClass(hInstance);
		m_hWnd = CreateWindowExW(0, L"WinImgMergeWindowClass", NULL, WS_CHILD | WS_VISIBLE,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hWndParent, NULL, hInstance, this);
		return m_hWnd ? true : false;
	}

	bool Destroy()
	{
		BOOL bSucceeded = DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		return !!bSucceeded;
	}

	bool SetWindowRect(const RECT& rc)
	{
		MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
		return true;
	}

	bool GetHorizontalSplit() const
	{
		return m_bHorizontalSplit;
	}

	void SetHorizontalSplit(bool horizontalSplit)
	{
		m_bHorizontalSplit = horizontalSplit;
		std::vector<RECT> rects = CalcChildImgWindowRect(m_hWnd, m_nImages, m_bHorizontalSplit);
		for (int i = 0; i < m_nImages; ++i)
			m_imgWindow[i].SetWindowRect(rects[i]);
	}

	RGBQUAD GetBackColor() const
	{
		return m_imgWindow[0].GetBackColor();
	}

	void SetBackColor(RGBQUAD backColor)
	{
		for (int i = 0; i < m_nImages; ++i)
			m_imgWindow[i].SetBackColor(backColor);
	}

	bool GetUseBackColor() const
	{
		return m_imgWindow[0].GetUseBackColor();
	}

	void SetUseBackColor(bool useBackColor)
	{
		for (int i = 0; i < m_nImages; ++i)
			m_imgWindow[i].SetUseBackColor(useBackColor);
	}

	double GetZoom() const
	{
		return m_imgWindow[0].GetZoom();
	}

	void SetZoom(double zoom)
	{
		for (int i = 0; i < m_nImages; ++i)
			m_imgWindow[i].SetZoom(zoom);
	}

	int  GetCurrentPage(int pane) const
	{
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
				m_imgOrig32[pane] = FreeImage_Clone(bitmap);
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

	int  GetDiffCount() const
	{
		return m_diffCount;
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
		ScrollToDiff(m_currentDiffIndex);
		return true;
	}

	bool LastDiff()
	{
		m_currentDiffIndex = m_diffCount - 1;
		RefreshImages();
		ScrollToDiff(m_currentDiffIndex);
		return true;
	}

	bool NextDiff()
	{
		++m_currentDiffIndex;
		if (m_currentDiffIndex >= m_diffCount)
			m_currentDiffIndex = m_diffCount - 1;
		RefreshImages();
		ScrollToDiff(m_currentDiffIndex);
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
		ScrollToDiff(m_currentDiffIndex);
		return true;
	}

	void CompareImages()
	{
		if (m_nImages <= 1)
			return;
		InitializeDiff();
		if (m_showDifferences)
		{
			if (m_nImages == 2)
			{
				CompareImages2(0, 1, m_diff);
				m_diffCount = MarkDiffIndex(m_diff);
			}
			else if (m_nImages == 3)
			{
				CompareImages2(0, 1, m_diff01);
				CompareImages2(2, 1, m_diff21);
				Make3WayDiff(m_diff01, m_diff21, m_diff);
				m_diffCount = MarkDiffIndex(m_diff);
			}
		}
		RefreshImages();
	}

	void ScrollToDiff(int diffIndex)
	{
		if (diffIndex >= 0 && diffIndex < m_diffPositions.size())
		{
			for (int i = 0; i < m_nImages; ++i)
				m_imgWindow[i].ScrollTo(m_diffPositions[diffIndex].x * m_diffBlockSize, m_diffPositions[diffIndex].y * m_diffBlockSize);
		}
	}

	void RefreshImages()
	{
		if (m_nImages <= 1)
			return;
		InitializeDiffImages();
		for (int i = 0; i < m_nImages; ++i)
			CopyOriginalImageToDiffImage(i);
		void (CImgMergeWindow::*func)(int src, int dst) = NULL;
		if (m_overlayMode == OVERLAY_ALPHABLEND)
			func = &CImgMergeWindow::AlphaBlendImages2;
		else if (m_overlayMode == OVERLAY_XOR)
			func = &CImgMergeWindow::XorImages2;
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
			if (m_nImages == 2)
			{
				MarkDiff(0, m_diff);
				MarkDiff(1, m_diff);
			}
			else if (m_nImages == 3)
			{
				MarkDiff(0, m_diff);
				MarkDiff(1, m_diff);
				MarkDiff(1, m_diff);
				MarkDiff(2, m_diff);
			}
		}
		for (int i = 0; i < m_nImages; ++i)
			m_imgWindow[i].Invalidate();
	}

	bool OpenImages(int nImages, const wchar_t *filename[3])
	{
		CloseImages();
		m_nImages = nImages;
		for (int i = 0; i < nImages; ++i)
			m_filename[i] = filename[i];
		bool bSucceeded = LoadImages();
		if (!bSucceeded)
		{
			m_nImages = 0;
			return false;
		}
		for (int i = 0; i < nImages; ++i)
		{
			m_imgWindow[i].Create(m_hInstance, m_hWnd);
			for (int j = 0; j < nImages; ++j)
			{
				if (i != j)
					m_imgWindow[i].AddSibling(&m_imgWindow[j]);
			}
		}
		CompareImages();
		std::vector<RECT> rects = CalcChildImgWindowRect(m_hWnd, nImages, m_bHorizontalSplit);
		for (int i = 0; i < nImages; ++i)
		{
			m_imgWindow[i].SetWindowRect(rects[i]);
			m_imgWindow[i].SetImage(&m_imgDiff[i]);
		}
		return bSucceeded;
	}

	bool OpenImages(const wchar_t *filename1, const wchar_t *filename2)
	{
		const wchar_t *filenames[] = {filename1, filename2};
		return OpenImages(2, filenames);
	}

	bool OpenImages(const wchar_t *filename1, const wchar_t *filename2, const wchar_t *filename3)
	{
		const wchar_t *filenames[] = {filename1, filename2, filename3};
		return OpenImages(3, filenames);
	}

	bool CloseImages()
	{
		for (int i = 0; i < m_nImages; ++i)
		{
			m_imgWindow[i].Destroy();
			m_imgOrig[i].clear();
			m_imgOrig32[i].clear();
		}
		return true;
	}

	HWND GetHWND() const
	{
		return m_hWnd;
	}

private:

	ATOM MyRegisterClass(HINSTANCE hInstance)
	{
		WNDCLASSEXW wcex = {0};
		wcex.cbSize         = sizeof(WNDCLASSEX); 
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= (WNDPROC)CImgMergeWindow::WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hCursor        = LoadCursor (NULL, IDC_ARROW);
		wcex.hbrBackground  = (HBRUSH)(COLOR_3DFACE+1);
		wcex.lpszClassName	= L"WinImgMergeWindowClass";
		return RegisterClassExW(&wcex);
	}

	std::vector<RECT> CalcChildImgWindowRect(HWND hWnd, int nImages, bool bHorizontalSplit)
	{
		std::vector<RECT> childrects;
		RECT rcParent;
		GetClientRect(hWnd, &rcParent);
		RECT rc = rcParent;
		if (!bHorizontalSplit)
		{
			int width = (rcParent.left + rcParent.right) / (nImages > 0 ? nImages : 1) - 2;
			rc.left = 0;
			rc.right = rc.left + width;
			for (int i = 0; i < nImages - 1; ++i)
			{
				childrects.push_back(rc);
				rc.left  = rc.right + 2 * 2;
				rc.right = rc.left  + width;
			}
			rc.right = rcParent.right;
			rc.left  = rc.right - width;
			childrects.push_back(rc);
		}
		else
		{
			int height = (rcParent.top + rcParent.bottom) / (nImages > 0 ? nImages : 1) - 2;
			rc.top = 0;
			rc.bottom = rc.top + height;
			for (int i = 0; i < nImages - 1; ++i)
			{
				childrects.push_back(rc);
				rc.top    = rc.bottom + 2 * 2;
				rc.bottom = rc.top    + height;
			}
			rc.bottom = rcParent.bottom;
			rc.top    = rc.bottom - height;
			childrects.push_back(rc);
		}
		return childrects;
	}

	void MoveSplitter(int x, int y)
	{
		RECT rcParent;
		GetClientRect(m_hWnd, &rcParent);

		RECT rc[3];
		for (int i = 0; i < m_nImages; ++i)
			rc[i] = m_imgWindow[i].GetWindowRect();

		if (!m_bHorizontalSplit)
		{
			int minx = rc[m_nDraggingSplitter].left + 32; 
			int maxx = rc[m_nDraggingSplitter + 1].right - 32; 
			if (x < minx)
				rc[m_nDraggingSplitter].right = minx;
			else if (x > maxx)
				rc[m_nDraggingSplitter].right = maxx;
			else
				rc[m_nDraggingSplitter].right = x;
			for (int i = m_nDraggingSplitter + 1; i < m_nImages; ++i)
			{
				int width   = rc[i].right - rc[i].left;
				rc[i].left  = rc[i - 1].right + 2 * 2;
				rc[i].right = rc[i].left + width;
			}
			rc[m_nImages - 1].right = rcParent.right;
		}
		else
		{
			rc[m_nDraggingSplitter].bottom = y;
			int miny = rc[m_nDraggingSplitter].top + 32; 
			int maxy = rc[m_nDraggingSplitter + 1].bottom - 32; 
			if (y < miny)
				rc[m_nDraggingSplitter].bottom = miny;
			else if (y > maxy)
				rc[m_nDraggingSplitter].bottom = maxy;
			else
				rc[m_nDraggingSplitter].bottom = y;
			for (int i = m_nDraggingSplitter + 1; i < m_nImages; ++i)
			{
				int height    = rc[i].bottom - rc[i].top;
				rc[i].top    = rc[i - 1].bottom + 2 * 2;
				rc[i].bottom = rc[i].top + height;
			}
			rc[m_nImages - 1].bottom = rcParent.bottom;
		}

		for (int i = 0; i < m_nImages; ++i)
			m_imgWindow[i].SetWindowRect(rc[i]);
	}

	bool LoadImages()
	{
		for (int i = 0; i < m_nImages; ++i)
		{
			m_currentPage[i] = 0;
			char szFileName[260];
			wsprintfA(szFileName, "%S", m_filename[i].c_str()); 
			m_imgOrigMultiPage[i].open(szFileName, FALSE, FALSE, 0);
			if (m_imgOrigMultiPage[i].isValid())
			{
				FIBITMAP *bitmap = m_imgOrigMultiPage[i].lockPage(m_currentPage[i]);
				if (bitmap)
				{
					m_imgOrig32[i] = FreeImage_Clone(bitmap);
					FreeImage_UnlockPage(m_imgOrigMultiPage[i], bitmap, false);
				}
				else
					m_imgOrigMultiPage[i].close();
			}
			if (!m_imgOrigMultiPage[i].isValid())
			{
				if (!m_imgOrig[i].loadU(m_filename[i].c_str()))
					return false;
				m_imgOrig32[i] = m_imgOrig[i];
			}

			m_imgOrig32[i].convertTo32Bits();
		}
		return true;
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
		}
		m_diffPositions.clear();
	}

	void InitializeDiffImages()
	{
		Size<unsigned> size = GetMaxWidthHeight();
		for (int i = 0; i < m_nImages; ++i)
			m_imgDiff[i].setSize(FIT_BITMAP, size.cx, size.cy, 32);
	}

	void CompareImages2(int pane1, int pane2, Array2D<unsigned>& diff)
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
					if (w1 == w2)
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
		
	void FloodFill8Directions(Array2D<unsigned>& data, int x, int y, unsigned val)
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
			if (x + 1 < data.width())
			{
				stack.push_back(Point<int>(x + 1, y));
				if (y + 1 < data.height())
					stack.push_back(Point<int>(x + 1, y + 1));
				if (y - 1 >= 0)
					stack.push_back(Point<int>(x + 1, y - 1));
			}
			if (x - 1 >= 0)
			{
				stack.push_back(Point<int>(x - 1, y));
				if (y + 1 < data.height())
					stack.push_back(Point<int>(x - 1, y + 1));
				if (y - 1 >= 0)
					stack.push_back(Point<int>(x - 1, y - 1));
			}
			if (y + 1 < data.height())
				stack.push_back(Point<int>(x, y + 1));
			if (y - 1 >= 0)
				stack.push_back(Point<int>(x, y - 1));
		}
	}

	int MarkDiffIndex(Array2D<unsigned>& diff)
	{
		int diffCount = 0;
		for (unsigned bx = 0; bx < diff.width(); ++bx)
		{
			for (unsigned by = 0; by < diff.height(); ++by)
			{
				if (diff(bx, by) == -1)
				{
					m_diffPositions.push_back(Point<int>(bx, by));
					++diffCount;
					FloodFill8Directions(diff, bx, by, diffCount);
				}
			}
		}
		return diffCount;
	}

	void Make3WayDiff(const Array2D<unsigned>& diff01, const Array2D<unsigned>& diff21, Array2D<unsigned>& diff3)
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

	void MarkDiff(int pane, const Array2D<unsigned>& diff)
	{
		const unsigned w = m_imgDiff[pane].getWidth();
		const unsigned h = m_imgDiff[pane].getHeight();
		const double alpha = 0.5;

		for (unsigned by = 0; by < diff.height(); ++by)
		{
			for (unsigned bx = 0; bx < diff.width(); ++bx)
			{
				unsigned diffIndex = diff(bx, by);
				if (diffIndex != 0)
				{
					COLORREF color = (diffIndex - 1 == m_currentDiffIndex) ? m_selDiffColor : m_diffColor;
					unsigned bsy = (h - by * m_diffBlockSize < m_diffBlockSize) ? (h - by * m_diffBlockSize) : m_diffBlockSize;
					for (unsigned i = 0; i < bsy; ++i)
					{
						unsigned y = by * m_diffBlockSize + i;
						BYTE *scanline = m_imgDiff[pane].getScanLine(h - y - 1);
						unsigned bsx = (w - bx * m_diffBlockSize < m_diffBlockSize) ? (w - bx * m_diffBlockSize) : m_diffBlockSize;
						for (unsigned j = 0; j < bsx; ++j)
						{
							unsigned x = bx * m_diffBlockSize + j;
							scanline[x * 4 + 0] = static_cast<BYTE>(scanline[x * 4 + 0] * (1 - alpha) + GetBValue(color) * alpha);
							scanline[x * 4 + 1] = static_cast<BYTE>(scanline[x * 4 + 1] * (1 - alpha) + GetGValue(color) * alpha);
							scanline[x * 4 + 2] = static_cast<BYTE>(scanline[x * 4 + 2] * (1 - alpha) + GetRValue(color) * alpha);
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
				//scanline_dst[x * 4 + 3] ^= scanline_src[x * 4 + 3];
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

	void DrawXorBar(HDC hdc, int x1, int y1, int width, int height)
	{
		static WORD _dotPatternBmp[8] = 
		{ 
			0x00aa, 0x0055, 0x00aa, 0x0055, 
			0x00aa, 0x0055, 0x00aa, 0x0055
		};

		HBITMAP hbm;
		HBRUSH  hbr, hbrushOld;

		hbm = CreateBitmap(8, 8, 1, 1, _dotPatternBmp);
		hbr = CreatePatternBrush(hbm);
		
		SetBrushOrgEx(hdc, x1, y1, 0);
		hbrushOld = (HBRUSH)SelectObject(hdc, hbr);
		
		PatBlt(hdc, x1, y1, width, height, PATINVERT);
		
		SelectObject(hdc, hbrushOld);
		
		DeleteObject(hbr);
		DeleteObject(hbm);
	}

	void OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
	{
	}

	void OnSize(UINT nType, int cx, int cy)
	{
		std::vector<RECT> rects = CalcChildImgWindowRect(m_hWnd, m_nImages, m_bHorizontalSplit);
		for (int i = 0; i < m_nImages; ++i)
			m_imgWindow[i].SetWindowRect(rects[i]);
	}
	
	void OnLButtonDown(UINT nFlags, int x, int y)
	{
		int i;
		for (i = 0; i < m_nImages - 1; ++i)
		{
			if (!m_bHorizontalSplit)
			{
				if (x < m_imgWindow[i + 1].GetWindowRect().left)
					break;
			}
			else
			{
				if (y < m_imgWindow[i + 1].GetWindowRect().top)
					break;
			}
		}
		m_oldSplitPosX = -4;
		m_oldSplitPosY = -4;
		m_nDraggingSplitter = i;
		SetCapture(m_hWnd);
	}

	void OnLButtonUp(UINT nFlags, int x, int y)
	{
		if (m_nDraggingSplitter == -1)
			return;
		ReleaseCapture();
		HDC hdc = GetWindowDC(m_hWnd);
		if (!m_bHorizontalSplit)
			DrawXorBar(hdc, m_oldSplitPosX - 2, 1, 4, m_imgWindow[0].GetWindowRect().bottom);
		else
			DrawXorBar(hdc, 1, m_oldSplitPosY - 2, m_imgWindow[0].GetWindowRect().right, 4);
		ReleaseDC(m_hWnd, hdc);
		MoveSplitter(x, y);
		m_nDraggingSplitter = -1;
	}

	void OnMouseMove(UINT nFlags, int x, int y)
	{
		if (m_nImages < 2)
			return;
		SetCursor(LoadCursor(NULL, m_bHorizontalSplit ? IDC_SIZENS : IDC_SIZEWE));
		if (m_nDraggingSplitter == -1)
			return;
		HDC hdc = GetWindowDC(m_hWnd);
		if (!m_bHorizontalSplit)
		{
			DrawXorBar(hdc, m_oldSplitPosX - 2, 1, 4, m_imgWindow[0].GetWindowRect().bottom);
			DrawXorBar(hdc, x              - 2, 1, 4, m_imgWindow[0].GetWindowRect().bottom);
		}
		else
		{
			DrawXorBar(hdc, 1, m_oldSplitPosY - 2, m_imgWindow[0].GetWindowRect().right, 4);	
			DrawXorBar(hdc, 1, y              - 2, m_imgWindow[0].GetWindowRect().right, 4);	
		}
		m_oldSplitPosX = x;
		m_oldSplitPosY = y;
		ReleaseDC(m_hWnd, hdc);
	}

	void OnDestroy()
	{
	}

	LRESULT OnWndMsg(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (iMsg)
		{
		case WM_CREATE:
			OnCreate(hwnd, (LPCREATESTRUCT)lParam);
			break;
		case WM_SIZE:
			OnSize((UINT)wParam, LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_LBUTTONDOWN:
			OnLButtonDown((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			break;
		case WM_LBUTTONUP:
			OnLButtonUp((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			break;
		case WM_MOUSEMOVE:
			OnMouseMove((UINT)(wParam), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
			break;
		case WM_MOUSEWHEEL:
			PostMessage(m_imgWindow[0].GetHWND(), iMsg, wParam, lParam);
			break;
		case WM_DESTROY:
			OnDestroy();
			break;
		default:
			return DefWindowProc(hwnd, iMsg, wParam, lParam);
		}
		return 0;
	}

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
	{
		if (iMsg == WM_NCCREATE)
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams));
		CImgMergeWindow *pImgWnd = (CImgMergeWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		LRESULT lResult = pImgWnd->OnWndMsg(hwnd, iMsg, wParam, lParam);
		return lResult;
	}

	int m_nImages;
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	fipMultiPage m_imgOrigMultiPage[3];
	fipImage m_imgOrig[3];
	fipImage m_imgOrig32[3];
	fipWinImage m_imgDiff[3];
	CImgWindow m_imgWindow[3];
	std::wstring m_filename[3];
	int m_nDraggingSplitter;
	bool m_bHorizontalSplit;
	int m_oldSplitPosX;
	int m_oldSplitPosY;
	OVERLAY_MODE m_overlayMode;
	bool m_showDifferences;
	double m_overlayAlpha;
	unsigned m_diffBlockSize;
	COLORREF m_selDiffColor;
	COLORREF m_diffColor;
	int m_currentPage[3];
	int m_currentDiffIndex;
	int m_diffCount;
	Array2D<unsigned> m_diff, m_diff01, m_diff21;
	std::vector<Point<int> > m_diffPositions;
};
