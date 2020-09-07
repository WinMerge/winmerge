#include "StdAfx.h"
#include "DpiAware.h"
#include <../src/mfc/afximpl.h>

namespace DpiAware
{
	GetDpiForWindowType GetDpiForWindow = nullptr;
	SystemParametersInfoForDpiType SystemParametersInfoForDpi = nullptr;
	GetSystemMetricsForDpiType GetSystemMetricsForDpi = nullptr;
	OpenThemeDataForDpiType OpenThemeDataForDpi = nullptr;
	AdjustWindowRectExForDpiType AdjustWindowRectExForDpi = nullptr;
	LoadIconWithScaleDownType LoadIconWithScaleDown = nullptr;

	short SMIconOnInit = 0;

	bool DpiAwareSupport = []()
	{
		SMIconOnInit = static_cast<short>(::GetSystemMetrics(SM_CXSMICON));
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
			  { *phico = LoadIcon(hinst, pszName); return *phico != nullptr ? S_OK : E_FAIL; };
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
		if (DpiAware::DpiAwareSupport)
		{
			DpiAware::NONCLIENTMETRICS6 ncm = { sizeof DpiAware::NONCLIENTMETRICS6 };
			if (!SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof DpiAware::NONCLIENTMETRICS6, &ncm, 0, dpi))
				return false;
			memcpy(&logFont, (char*)&ncm + memberOffset, sizeof(LOGFONT));
		}
		else
		{
			NONCLIENTMETRICS ncm = { sizeof NONCLIENTMETRICS };
			if (!SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof NONCLIENTMETRICS, &ncm, 0))
				return false;
			memcpy(&logFont, (char*)&ncm + memberOffset, sizeof(LOGFONT));
		}
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

}

