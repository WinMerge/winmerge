#include "StdAfx.h"
#include "DpiAware.h"
#include <commoncontrols.h>
#include <../src/mfc/afximpl.h>

namespace DpiAware
{
	GetDpiForWindowType GetDpiForWindow = nullptr;
	SystemParametersInfoForDpiType SystemParametersInfoForDpi = nullptr;
	GetSystemMetricsForDpiType GetSystemMetricsForDpi = nullptr;
	OpenThemeDataForDpiType OpenThemeDataForDpi = nullptr;
	AdjustWindowRectExForDpiType AdjustWindowRectExForDpi = nullptr;
	LoadIconWithScaleDownType LoadIconWithScaleDown = nullptr;

	int DPIOnInit = 0;

	bool DpiAwareSupport = []()
	{
		DPIOnInit = MulDiv(96, ::GetSystemMetrics(SM_CXSMICON), 16);
		HMODULE hLibraryUser32 = GetModuleHandleW(L"user32.dll");
		if (hLibraryUser32)
		{
			AdjustWindowRectExForDpi = reinterpret_cast<AdjustWindowRectExForDpiType>(GetProcAddress(hLibraryUser32, "AdjustWindowRectExForDpi"));
			GetDpiForWindow = reinterpret_cast<GetDpiForWindowType>(GetProcAddress(hLibraryUser32, "GetDpiForWindow"));
			SystemParametersInfoForDpi = reinterpret_cast<SystemParametersInfoForDpiType>(GetProcAddress(hLibraryUser32, "SystemParametersInfoForDpi"));
			GetSystemMetricsForDpi = reinterpret_cast<GetSystemMetricsForDpiType>(GetProcAddress(hLibraryUser32, "GetSystemMetricsForDpi"));
		}
		HMODULE hLibraryComctl32 = GetModuleHandleW(L"comctl32.dll");
		if (hLibraryComctl32)
		{
			LoadIconWithScaleDown = reinterpret_cast<LoadIconWithScaleDownType>(GetProcAddress(hLibraryComctl32, "LoadIconWithScaleDown"));
		}
		HMODULE hLibraryUxTheme = GetModuleHandleW(L"uxtheme.dll");
		if (hLibraryUxTheme)
		{
			OpenThemeDataForDpi = reinterpret_cast<OpenThemeDataForDpiType>(GetProcAddress(hLibraryUxTheme, "OpenThemeDataForDpi"));
		}
		if (!AdjustWindowRectExForDpi)
			AdjustWindowRectExForDpi = [](LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi) -> BOOL
				{ return AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle); };
		if (!GetDpiForWindow)
			GetDpiForWindow = [](HWND hwnd) -> UINT { return CClientDC(CWnd::FromHandle(hwnd)).GetDeviceCaps(LOGPIXELSX); };
		if (!GetSystemMetricsForDpi)
			GetSystemMetricsForDpi = [](int nIndex, UINT dpi) -> int { return GetSystemMetrics(nIndex); };
		if (!LoadIconWithScaleDown)
			LoadIconWithScaleDown = [](HINSTANCE hinst, PCWSTR pszName, int cx, int cy, HICON *phico) -> HRESULT
			  { *phico = LoadIconW(hinst, pszName); return *phico != nullptr ? S_OK : E_FAIL; };
		if (!OpenThemeDataForDpi)
		{
			OpenThemeDataForDpi = [](HWND hwnd, LPCWSTR pszClassList, UINT dpi) -> HTHEME { return OpenThemeData(hwnd, pszClassList); };
			return false;
		}
		return true;
	}();

	void GetPointLogFont(LOGFONT& logFont, float point, const TCHAR* lfFaceName, int dpi)
	{
		LOGFONT lfv{};
		lfv.lfHeight = static_cast<long>(-point * dpi / 72.0f);
		lfv.lfCharSet = DEFAULT_CHARSET;
		_tcscpy_s(lfv.lfFaceName, lfFaceName);
		logFont = lfv;
	}

	bool GetNonClientLogFont(LOGFONT& logFont, size_t memberOffset, int dpi)
	{
		DpiAware::NONCLIENTMETRICS6 ncm{};
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		if (DpiAware::DpiAwareSupport)
			ncm.cbSize = sizeof DpiAware::NONCLIENTMETRICS6;
		if (!SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, ncm.cbSize, static_cast<NONCLIENTMETRICS *>(&ncm), 0, dpi))
			return false;
		memcpy(&logFont, (char*)&ncm + memberOffset, sizeof(LOGFONT));
		return true;
	}

	void UpdateAfxDataSysMetrics(int dpi)
	{
		afxData.cxIcon = GetSystemMetricsForDpi(SM_CXICON, dpi);
		afxData.cyIcon = GetSystemMetricsForDpi(SM_CYICON, dpi);
		afxData.cxVScroll = GetSystemMetricsForDpi(SM_CXVSCROLL, dpi) + AFX_CX_BORDER;
		afxData.cyHScroll = GetSystemMetricsForDpi(SM_CYHSCROLL, dpi) + AFX_CY_BORDER;
		afxData.cxPixelsPerInch = dpi;
		afxData.cyPixelsPerInch = dpi;
	}

	void ListView_UpdateColumnWidths(HWND hwnd, int olddpi, int newdpi)
	{
		HWND hwndHeader = ListView_GetHeader(hwnd);
		const int nColumns = Header_GetItemCount(hwndHeader);
		for (int i = 0; i < nColumns; ++i)
		{
			const int nColumnWidth = MulDiv(ListView_GetColumnWidth(hwnd, i), newdpi, olddpi);
			ListView_SetColumnWidth(hwnd, i, nColumnWidth);
		}
	}

	void Dialog_UpdateControlInnerWidths(HWND hwnd, int olddpi, int newdpi)
	{
		struct Dpis { int olddpi, newdpi; HWND hwndParent; } dpis{ olddpi, newdpi, hwnd };
		auto enumfunc = [](HWND hwnd, LPARAM lParam) -> BOOL
		{
			const Dpis *pdpis = (const Dpis *)lParam;
			TCHAR name[256];
			GetClassName(hwnd, name, sizeof(name) / sizeof(TCHAR));
			if (_tcsicmp(name, _T("SysListView32")) == 0 && pdpis->hwndParent == GetParent(hwnd))
				ListView_UpdateColumnWidths(hwnd, pdpis->olddpi, pdpis->newdpi);
			return TRUE;
		};
		EnumChildWindows(hwnd, enumfunc, (LPARAM)&dpis);
	}

	HIMAGELIST LoadShellImageList(int dpi)
	{
		SHFILEINFO sfi{};
		if (dpi == DpiAware::DPIOnInit)
		{
			return (HIMAGELIST)SHGetFileInfo(_T(""), 0,
				&sfi, sizeof(sfi), SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
		}
		else
		{
			int size = SHIL_EXTRALARGE;
			if (dpi < 96 * 2)
				size = SHIL_SMALL;
			else if (dpi < 96 * 3)
				size = SHIL_LARGE;
			IImageList *pImageList = nullptr;
			if (FAILED(SHGetImageList(size, IID_IImageList, (void**)&pImageList)))
				return nullptr;
			return IImageListToHIMAGELIST(pImageList);
		}
	}
}

