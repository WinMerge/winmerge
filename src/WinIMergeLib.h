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

#include <Windows.h>

struct IImgMergeWindow
{
	enum OVERLAY_MODE {
		OVERLAY_NONE = 0, OVERLAY_XOR, OVERLAY_ALPHABLEND
	};
	virtual bool OpenImages(const wchar_t *filename1, const wchar_t *filename2) = 0;
	virtual bool OpenImages(const wchar_t *filename1, const wchar_t *filename2, const wchar_t *filename3) = 0;
	virtual bool SetWindowRect(const RECT& rc) = 0;
	virtual bool GetHorizontalSplit() const = 0;
	virtual void SetHorizontalSplit(bool horizontalSplit) = 0;
	virtual int  GetCurrentPage(int pane) const = 0;
	virtual void SetCurrentPage(int pane, int page) = 0;
	virtual int  GetCurrentMaxPage() const = 0;
	virtual void SetCurrentPageAll(int page) = 0;
	virtual int  GetPageCount(int pane) const = 0;
	virtual int  GetMaxPageCount() const = 0;
	virtual RGBQUAD GetBackColor() const = 0;
	virtual void SetBackColor(RGBQUAD backColor) = 0;
	virtual bool GetUseBackColor() const = 0;
	virtual void SetUseBackColor(bool useBackColor) = 0;
	virtual double GetZoom() const = 0;
	virtual void SetZoom(double zoom) = 0;
	virtual int  GetDiffBlockSize() const = 0;
	virtual void SetDiffBlockSize(int blockSize) = 0;
	virtual OVERLAY_MODE GetOverlayMode() const = 0;
	virtual void SetOverlayMode(OVERLAY_MODE overlayMode) = 0;
	virtual double GetOverlayAlpha() const = 0;
	virtual void SetOverlayAlpha(double alpha) = 0;
	virtual bool GetShowDifferences() const = 0;
	virtual void SetShowDifferences(bool visible) = 0;
	virtual int  GetDiffCount() const = 0;
	virtual int  GetCurrentDiffIndex() const = 0;
	virtual bool FirstDiff() = 0;
	virtual bool LastDiff() = 0;
	virtual bool NextDiff() = 0;
	virtual bool PrevDiff() = 0;
	virtual HWND GetHWND() const = 0;
};

extern "C"
{
	IImgMergeWindow * WinIMerge_CreateWindow(HINSTANCE hInstance, HWND hWndParent);
	bool WinIMerge_DestroyWindow(IImgMergeWindow *pImgMergeWindow);
};
