#pragma once

#include <windef.h>
#include <wtypes.h>
#include <Unknwn.h>

struct WebDiffEvent
{
	enum EVENT_TYPE { ZoomFactorChanged, NewWindowRequested, WindowCloseRequested, NavigationStarting, HistoryChanged, SourceChanged, DocumentTitleChanged, NavigationCompleted, TabChanged, HSCROLL, VSCROLL };
	EVENT_TYPE type;
	int pane;
};

struct WebDiffCallbackResult
{
	HRESULT errorCode;
	const wchar_t* returnObjectAsJson;
};

struct __declspec(uuid("{DC951F69-C8CA-44DD-8C3C-8A9C76B0022C}")) IWebDiffCallback : IUnknown
{
	virtual HRESULT __stdcall Invoke(const WebDiffCallbackResult& result) = 0;
};

struct __declspec(uuid("{0C75D925-378C-46E2-A5AA-228133AD22EB}")) IWebDiffEventHandler: IUnknown
{
	virtual HRESULT __stdcall Invoke(const WebDiffEvent& event) = 0;
};

struct IWebDiffWindow
{
	enum UserdataFolderType
	{
		APPDATA, INSTALL
	};
	enum FormatType
	{
		SCREENSHOT, FULLSIZE_SCREENSHOT, HTML, TEXT, RESOURCETREE
	};
	enum BrowsingDataType
	{
		FILE_SYSTEMS        = ( 1 << 0 ),
		INDEXED_DB          = ( 1 << 1 ),
		LOCAL_STORAGE       = ( 1 << 2 ),
		WEB_SQL             = ( 1 << 3 ),
		CACHE_STORAGE       = ( 1 << 4 ),
		ALL_DOM_STORAGE     = ( 1 << 5 ),
		COOKIES             = ( 1 << 6 ),
		ALL_SITE            = ( 1 << 7 ),
		DISK_CACHE          = ( 1 << 8 ),
		DOWNLOAD_HISTORY    = ( 1 << 9 ),
		GENERAL_AUTOFILL    = ( 1 << 10 ),
		PASSWORD_AUTOSAVE   = ( 1 << 11 ),
		BROWSING_HISTORY    = ( 1 << 12 ),
		SETTINGS            = ( 1 << 13 ),
		ALL_PROFILE         = ( 1 << 14 ) 
	};
	struct DiffOptions
	{
		enum DiffAlgorithm {
			MYERS_DIFF, MINIMAL_DIFF, PATIENCE_DIFF, HISTOGRAM_DIFF, NONE_DIFF
		};
		int  ignoreWhitespace; /**< Ignore whitespace -option. */
		bool ignoreCase; /**< Ignore case -option. */
		bool ignoreNumbers; /**< Ignore numbers -option. */
		bool ignoreBlankLines; /**< Ignore blank lines -option. */
		bool ignoreEol; /**< Ignore EOL differences -option. */
		bool bFilterCommentsLines; /**< Ignore Multiline comments differences -option. */
		int  diffAlgorithm; /**< Diff algorithm -option. */
		bool indentHeuristic; /**< Ident heuristic -option */
		bool completelyBlankOutIgnoredChanges;
	};
	virtual bool IsWebView2Installed() const = 0;
	virtual bool DownloadWebView2() const = 0;
	virtual void AddEventListener(IWebDiffEventHandler *handler) = 0;
	virtual void SetUserDataFolderType(UserdataFolderType userDataFolderType, bool perPane) = 0;
	virtual HRESULT New(int nUrls, IWebDiffCallback* callback) = 0;
	virtual HRESULT Open(const wchar_t* url1, const wchar_t* url2, IWebDiffCallback* callback) = 0;
	virtual HRESULT Open(const wchar_t* url1, const wchar_t* url2, const wchar_t* url3, IWebDiffCallback* callback) = 0;
	virtual void Close() = 0;
	virtual void NewTab(int pane, const wchar_t *url, IWebDiffCallback* callback) = 0;
	virtual void CloseActiveTab(int pane) = 0;
	virtual HRESULT Reload(int pane) = 0;
	virtual HRESULT ReloadAll() = 0;
	virtual HRESULT Recompare(IWebDiffCallback* callback) = 0;
	virtual HRESULT SaveFile(int pane, FormatType kind, const wchar_t* filename, IWebDiffCallback* callback) = 0;
	virtual HRESULT SaveFiles(FormatType kind, const wchar_t* filenames[], IWebDiffCallback* callback) = 0;
	virtual HRESULT ClearBrowsingData(int pane, BrowsingDataType datakinds) = 0;
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
	virtual COLORREF GetTextDiffColor() const = 0;
	virtual void SetTextDiffColor(COLORREF clrDiffTextColor) = 0;
	virtual COLORREF GetSelDiffColor() const = 0;
	virtual void SetSelDiffColor(COLORREF clrSelDiffColor) = 0;
	virtual COLORREF GetSelTextDiffColor() const = 0;
	virtual void SetSelTextDiffColor(COLORREF clrSelTextDiffColor) = 0;
	virtual COLORREF GetSNPDiffColor() const = 0;
	virtual void SetSNPDiffColor(COLORREF clrSNPTextDiffColor) = 0;
	virtual COLORREF GetSNPTextDiffColor() const = 0;
	virtual void SetSNPTextDiffColor(COLORREF clrSNPTextDiffColor) = 0;
	virtual COLORREF GetSelSNPDiffColor() const = 0;
	virtual void SetSelSNPDiffColor(COLORREF clrSNPTextDiffColor) = 0;
	virtual COLORREF GetSelSNPTextDiffColor() const = 0;
	virtual void SetSelSNPTextDiffColor(COLORREF clrSelSNPTextDiffColor) = 0;
	virtual double GetZoom() const = 0;
	virtual void SetZoom(double zoom) = 0;
	virtual const wchar_t* GetUserAgent() const = 0;
	virtual void SetUserAgent(const wchar_t* userAgent) = 0;
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
	virtual bool Copy() = 0;
	virtual bool Cut() = 0;
	virtual bool Delete() = 0;
	virtual bool Paste() = 0;
	virtual bool SelectAll() = 0;
	virtual bool Undo() = 0;
	virtual bool Redo() = 0;
	virtual bool CanUndo() = 0;
	virtual bool CanRedo() = 0;
	virtual const DiffOptions& GetDiffOptions() const = 0;
	virtual void SetDiffOptions(const DiffOptions& diffOptions) = 0;
};

extern "C"
{
	IWebDiffWindow* WinWebDiff_CreateWindow(HINSTANCE hInstance, HWND hWndParent, int nID = 0);
	bool WinWebDiff_DestroyWindow(IWebDiffWindow* pWebDiffWindow);
};

