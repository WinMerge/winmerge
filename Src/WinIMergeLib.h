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
#include <wtypes.h>

struct IImgMergeWindow
{
	enum INSERTION_DELETION_DETECTION_MODE {
		INSERTION_DELETION_DETECTION_NONE = 0, INSERTION_DELETION_DETECTION_VERTICAL, INSERTION_DELETION_DETECTION_HORIZONTAL
	};
	enum OVERLAY_MODE {
		OVERLAY_NONE = 0, OVERLAY_XOR, OVERLAY_ALPHABLEND, OVERLAY_ALPHABLEND_ANIM
	};
	enum EVENT_TYPE {
		LBUTTONDOWN = 0, LBUTTONUP, LBUTTONDBLCLK, 
		RBUTTONDOWN,     RBUTTONUP, RBUTTONDBLCLK,
		MOUSEMOVE, MOUSEWHEEL, CONTEXTMENU,
		KEYDOWN, KEYUP,
		SIZE, HSCROLL, VSCROLL, SETFOCUS, KILLFOCUS,
		REFRESH, SCROLLTODIFF, OPEN, NEW
	};
	enum DRAGGING_MODE {
		NONE = 0, MOVE, ADJUST_OFFSET, VERTICAL_WIPE, HORIZONTAL_WIPE,
		RECTANGLE_SELECT,
		MOVE_IMAGE = 256, RESIZE_WIDTH, RESIZE_HEIGHT, RESIZE_BOTH
	};
	enum OCR_RESULT_TYPE {
		TEXT_ONLY = 0, TEXT_PER_LINE_YAML, TEXT_PER_WORD_YAML
	};
	enum DIFF_ALGORITHM {
		MYERS_DIFF, MINIMAL_DIFF, PATIENCE_DIFF, HISTOGRAM_DIFF, NONE_DIFF
	};
	struct Event
	{
		void *userdata;
		int pane;
		EVENT_TYPE eventType;
		int x;
		int y;
		unsigned flags;
		int delta;
		int width;
		int height;
		int keycode;
		int diffIndex;
	};
	typedef void (*EventListenerFunc)(const Event& evt);
	virtual bool OpenImages(const wchar_t *filename1, const wchar_t *filename2) = 0;
	virtual bool OpenImages(const wchar_t *filename1, const wchar_t *filename2, const wchar_t *filename3) = 0;
	virtual bool ReloadImages() = 0;
	virtual bool SaveImages() = 0;
	virtual bool SaveImage(int pane) = 0;
	virtual bool SaveImageAs(int pane, const wchar_t *filename) = 0;
	virtual bool SaveDiffImageAs(int pane, const wchar_t *filename) = 0;
	virtual const wchar_t *GetFileName(int pane) = 0;
	virtual int  GetPaneCount() const = 0;
	virtual RECT GetPaneWindowRect(int pane) const = 0;
	virtual RECT GetWindowRect() const = 0;
	virtual bool SetWindowRect(const RECT& rc) = 0;
	virtual POINT GetCursorPos(int pane) const = 0;
	virtual RGBQUAD GetPixelColor(int pane, int x, int y) const = 0;
	virtual double GetColorDistance(int pane1, int pane2, int x, int y) const = 0;
	virtual int  GetActivePane() const = 0;
	virtual void SetActivePane(int pane) = 0;
	virtual bool GetReadOnly(int pane) const = 0;
	virtual void SetReadOnly(int pane, bool readOnly) = 0;
	virtual bool GetHorizontalSplit() const = 0;
	virtual void SetHorizontalSplit(bool horizontalSplit) = 0;
	virtual int  GetCurrentPage(int pane) const = 0;
	virtual void SetCurrentPage(int pane, int page) = 0;
	virtual int  GetCurrentMaxPage() const = 0;
	virtual void SetCurrentPageAll(int page) = 0;
	virtual int  GetPageCount(int pane) const = 0;
	virtual int  GetMaxPageCount() const = 0;
	virtual COLORREF GetDiffColor() const = 0;
	virtual void SetDiffColor(COLORREF clrDiffColor) = 0;
	virtual COLORREF GetSelDiffColor() const = 0;
	virtual void SetSelDiffColor(COLORREF clrSelDiffColor) = 0;
	virtual double GetDiffColorAlpha() const = 0;
	virtual void SetDiffColorAlpha(double diffColorAlpha) = 0;
	virtual RGBQUAD GetBackColor() const = 0;
	virtual void SetBackColor(RGBQUAD backColor) = 0;
	virtual bool GetUseBackColor() const = 0;
	virtual void SetUseBackColor(bool useBackColor) = 0;
	virtual double GetZoom() const = 0;
	virtual void SetZoom(double zoom) = 0;
	virtual double GetColorDistanceThreshold() const = 0;
	virtual void SetColorDistanceThreshold(double threshold) = 0;
	virtual int  GetDiffBlockSize() const = 0;
	virtual void SetDiffBlockSize(int blockSize) = 0;
	virtual OVERLAY_MODE GetOverlayMode() const = 0;
	virtual void SetOverlayMode(OVERLAY_MODE overlayMode) = 0;
	virtual double GetOverlayAlpha() const = 0;
	virtual void SetOverlayAlpha(double alpha) = 0;
	virtual bool GetShowDifferences() const = 0;
	virtual void SetShowDifferences(bool visible) = 0;
	virtual bool GetBlinkDifferences() const = 0;
	virtual void SetBlinkDifferences(bool visible) = 0;
	virtual int  GetDiffCount() const = 0;
	virtual int  GetConflictCount() const = 0;
	virtual int  GetCurrentDiffIndex() const = 0;
	virtual bool FirstDiff() = 0;
	virtual bool LastDiff() = 0;
	virtual bool NextDiff() = 0;
	virtual bool PrevDiff() = 0;
	virtual bool FirstConflict() = 0;
	virtual bool LastConflict() = 0;
	virtual bool NextConflict() = 0;
	virtual bool PrevConflict() = 0; 
	virtual bool SelectDiff(int diffIndex) = 0;
	virtual int  GetNextDiffIndex() const = 0;
	virtual int  GetPrevDiffIndex() const = 0;
	virtual int  GetNextConflictIndex() const = 0;
	virtual int  GetPrevConflictIndex() const = 0;
	virtual void CopyDiff(int diffIndex, int srcPane, int dstPane) = 0;
	virtual void CopyDiffAll(int srcPane, int dstPane) = 0;
	virtual int  CopyDiff3Way(int dstPane) = 0;
	virtual bool IsUndoable() const = 0;
	virtual bool IsRedoable() const = 0;
	virtual bool Undo() = 0;
	virtual bool Redo() = 0;
	virtual bool IsModified(int pane) const = 0;
	virtual HWND GetHWND() const = 0;
	virtual HWND GetPaneHWND(int pane) const = 0;
	virtual int  GetImageWidth(int pane) const = 0;
	virtual int  GetImageHeight(int pane) const = 0;
	virtual int  GetImageBitsPerPixel(int pane) const = 0;
	virtual void AddImageOffset(int pane, int dx, int dy) = 0;
	virtual POINT GetImageOffset(int pane) const = 0;
	virtual void AddEventListener(EventListenerFunc func, void *userdata) = 0;
	virtual DRAGGING_MODE GetDraggingMode() const = 0;
	virtual void SetDraggingMode(DRAGGING_MODE draggingMode) = 0;
	virtual size_t GetMetadata(int pane, char *buf, size_t bufsize) const = 0;
	virtual INSERTION_DELETION_DETECTION_MODE GetInsertionDeletionDetectionMode() const = 0;
	virtual void SetInsertionDeletionDetectionMode(INSERTION_DELETION_DETECTION_MODE insertionDeletionDetectionMode) = 0;
	virtual COLORREF GetDiffDeletedColor() const = 0;
	virtual void SetDiffDeletedColor(COLORREF clrDiffDeletedColor) = 0;
	virtual COLORREF GetSelDiffDeletedColor() const = 0;
	virtual void SetSelDiffDeletedColor(COLORREF clrSelDiffDeletedColor) = 0;
	virtual bool ConvertToRealPos(int pane, const POINT& pt, POINT& ptReal) const = 0;
	virtual float GetVectorImageZoomRatio() const = 0;
	virtual void SetVectorImageZoomRatio(float zoom) = 0;
	virtual bool CloseImages() = 0;
	virtual bool NewImages(int nImages, int nPages, int width, int height) = 0;
	virtual bool Copy() = 0;
	virtual bool Cut() = 0;
	virtual bool Delete() = 0;
	virtual bool Paste() = 0;
	virtual bool SelectAll() = 0;
	virtual bool Cancel() = 0;
	virtual RECT GetRectangleSelection(int pane) const = 0;
	virtual bool IsCopyable() const = 0;
	virtual bool IsCuttable() const = 0;
	virtual bool IsPastable() const = 0;
	virtual bool IsCancellable() const = 0;
	virtual bool IsRectangleSelectionVisible(int pane) const = 0;
	virtual BSTR ExtractTextFromImage(int pane, int page, OCR_RESULT_TYPE resultType) = 0;
	virtual int GetSavePoint(int pane) const = 0;
	virtual void SetSavePoint(int pane, int pos) = 0;
	virtual float GetRotation(int pane) const = 0;
	virtual void SetRotation(int pane, float angle) = 0;
	virtual bool GetHorizontalFlip(int pane) const = 0;
	virtual void SetHorizontalFlip(int pane, bool flip) = 0;
	virtual bool GetVerticalFlip(int pane) const = 0;
	virtual void SetVerticalFlip(int pane, bool flip) = 0;
	virtual DIFF_ALGORITHM GetDiffAlgorithm() const = 0;
	virtual void SetDiffAlgorithm(DIFF_ALGORITHM diffAlgorithm) = 0;
	virtual bool IsSaveSupported(int pane) const = 0;
	virtual int GetBlinkInterval() const = 0;
	virtual void SetBlinkInterval(int interval) = 0;
	virtual int GetOverlayAnimationInterval() const = 0;
	virtual void SetOverlayAnimationInterval(int interval) = 0;
	virtual bool IsDarkBackgroundEnabled() const = 0;
	virtual void SetDarkBackgroundEnabled(bool enabled) = 0;
	virtual int GetLastErrorCode() const = 0;
};

struct IImgToolWindow
{
	using TranslateCallback = void(*)(int id, const wchar_t *org, size_t dstbufsize, wchar_t *dst);
	virtual HWND GetHWND() const = 0;
	virtual void Sync() = 0;
	virtual void Translate(TranslateCallback translateCallback) = 0;
};

extern "C"
{
	IImgMergeWindow * WinIMerge_CreateWindow(HINSTANCE hInstance, HWND hWndParent, int nID = 0);
	bool WinIMerge_DestroyWindow(IImgMergeWindow *pImgMergeWindow);
	IImgToolWindow * WinIMerge_CreateToolWindow(HINSTANCE hInstance, HWND hWndParent, IImgMergeWindow *pImgMergeWindow);
	bool WinIMerge_DestroyToolWindow(IImgToolWindow *pImgToolWindow);
	IImgMergeWindow * WinIMerge_CreateWindowless();
};
