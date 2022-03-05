#pragma once

#include <windef.h>
#include <wtypes.h>
#include <Unknwn.h>

struct __declspec(uuid("{DC951F69-C8CA-44DD-8C3C-8A9C76B0022C}")) IWebDiffCallback : IUnknown
{
	virtual HRESULT __stdcall Invoke(HRESULT hr) = 0;
};

struct IWebDiffWindow
{
	virtual bool IsWebView2Installed() const = 0;
	virtual bool DownloadWebView2() const = 0;
	virtual HRESULT New(int nUrls) = 0;
	virtual HRESULT Open(const wchar_t* url1, const wchar_t* url2) = 0;
	virtual HRESULT Open(const wchar_t* url1, const wchar_t* url2, const wchar_t* url3) = 0;
	virtual void Close() = 0;
	virtual void NewTab(int pane, const wchar_t *url) = 0;
	virtual void CloseActiveTab(int pane) = 0;
	virtual HRESULT Reload() = 0;
	virtual HRESULT SaveScreenshot(int pane, const wchar_t* filename, IWebDiffCallback *callback) = 0;
	virtual HRESULT SaveScreenshots(const wchar_t* filenames[], IWebDiffCallback* callback) = 0;
	virtual HRESULT SaveHTML(int pane, const wchar_t* filename, IWebDiffCallback *callback) = 0;
	virtual HRESULT SaveHTMLs(const wchar_t* filenames[], IWebDiffCallback* callback) = 0;
	virtual HRESULT SaveResourceTree(int pane, const wchar_t* filename, IWebDiffCallback *callback) = 0;
	virtual HRESULT SaveResourceTrees(const wchar_t* filenames[], IWebDiffCallback* callback) = 0;
	virtual const wchar_t *GetCurrentUrl(int pane) = 0;
	virtual int  GetPaneCount() const = 0;
	virtual RECT GetPaneWindowRect(int pane) const = 0;
	virtual RECT GetWindowRect() const = 0;
	virtual bool SetWindowRect(const RECT& rc) = 0;
	virtual int  GetActivePane() const = 0;
	virtual void SetActivePane(int pane) = 0;
	virtual bool GetHorizontalSplit() const = 0;
	virtual void SetHorizontalSplit(bool horizontalSplit) = 0;
	virtual COLORREF GetDiffColor() const = 0;
	virtual void SetDiffColor(COLORREF clrDiffColor) = 0;
	virtual COLORREF GetSelDiffColor() const = 0;
	virtual void SetSelDiffColor(COLORREF clrSelDiffColor) = 0;
	virtual double GetDiffColorAlpha() const = 0;
	virtual void SetDiffColorAlpha(double diffColorAlpha) = 0;
	virtual double GetZoom() const = 0;
	virtual void SetZoom(double zoom) = 0;
	virtual bool GetFitToWindow() const = 0;
	virtual void SetFitToWindow(bool fitToWindow) = 0;
	virtual SIZE GetSize() const = 0;
	virtual void SetSize(SIZE rc) = 0;
	virtual bool GetShowDifferences() const = 0;
	virtual void SetShowDifferences(bool visible) = 0;
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
	virtual HWND GetHWND() const = 0;
	virtual HWND GetPaneHWND(int pane) const = 0;
};

extern "C"
{
	IWebDiffWindow* WinWebDiff_CreateWindow(HINSTANCE hInstance, HWND hWndParent, int nID = 0);
	bool WinWebDiff_DestroyWindow(IWebDiffWindow* pWebDiffWindow);
};

