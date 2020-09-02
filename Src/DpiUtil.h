#pragma once

#include <afxwin.h>

namespace DpiUtil
{
	struct NONCLIENTMETRICS6
	{
		UINT    cbSize;
		int     iBorderWidth;
		int     iScrollWidth;
		int     iScrollHeight;
		int     iCaptionWidth;
		int     iCaptionHeight;
		LOGFONTW lfCaptionFont;
		int     iSmCaptionWidth;
		int     iSmCaptionHeight;
		LOGFONTW lfSmCaptionFont;
		int     iMenuWidth;
		int     iMenuHeight;
		LOGFONTW lfMenuFont;
		LOGFONTW lfStatusFont;
		LOGFONTW lfMessageFont;
		int     iPaddedBorderWidth;
	};

	using GetDpiForWindowType = UINT (__stdcall*)(HWND hwnd);
	using SystemParametersInfoForDpiType = BOOL(__stdcall*)(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);
	using GetSystemMetricsForDpiType = int(__stdcall*)(int nIndex, UINT dpi);
	using OpenThemeDataForDpiType = HTHEME(__stdcall*)(HWND hwnd, LPCWSTR pszClassList, UINT dpi);

	inline GetDpiForWindowType GetDpiForWindow = nullptr;
	inline SystemParametersInfoForDpiType SystemParametersInfoForDpi = nullptr;
	inline GetSystemMetricsForDpiType GetSystemMetricsForDpi = nullptr;
	inline OpenThemeDataForDpiType OpenThemeDataForDpi = nullptr;

	inline bool succeeded = []()
	{
		HMODULE hLibraryUser32 = GetModuleHandleW(L"user32.dll");
		if (!hLibraryUser32)
			return false;
		HMODULE hLibraryUxTheme = GetModuleHandleW(L"uxtheme.dll");
		if (!hLibraryUxTheme)
			return false;
		GetDpiForWindow = reinterpret_cast<GetDpiForWindowType>(GetProcAddress(hLibraryUser32, "GetDpiForWindow"));
		SystemParametersInfoForDpi = reinterpret_cast<SystemParametersInfoForDpiType>(GetProcAddress(hLibraryUser32, "SystemParametersInfoForDpi"));
		GetSystemMetricsForDpi = reinterpret_cast<GetSystemMetricsForDpiType>(GetProcAddress(hLibraryUser32, "GetSystemMetricsForDpi"));
		OpenThemeDataForDpi = reinterpret_cast<OpenThemeDataForDpiType>(GetProcAddress(hLibraryUxTheme, "OpenThemeDataForDpi"));
		return true;
	}();

	inline int GetDpiForCWnd(CWnd *pWnd)
	{
		if (GetDpiForWindow)
			return GetDpiForWindow(pWnd->m_hWnd);
		return CClientDC(pWnd).GetDeviceCaps(LOGPIXELSX);
	}
	
	inline int GetSystemMetricsForWindow(CWnd *pWnd, int nIndex)
	{
		if (GetSystemMetricsForDpi)
			return GetSystemMetricsForDpi(nIndex, GetDpiForWindow(pWnd->m_hWnd));
		return GetSystemMetrics(nIndex);
	}

	inline void GetMenuLogFont(int dpi, LOGFONT& logFont)
	{
		if (DpiUtil::GetSystemMetricsForDpi)
		{
			DpiUtil::NONCLIENTMETRICS6 ncm = { sizeof DpiUtil::NONCLIENTMETRICS6 };
			if (DpiUtil::SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof DpiUtil::NONCLIENTMETRICS6, &ncm, 0, dpi))
				logFont = ncm.lfMenuFont;
		}
		else
		{
			NONCLIENTMETRICS ncm = { sizeof NONCLIENTMETRICS };
			if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof NONCLIENTMETRICS, &ncm, 0))
				logFont = ncm.lfMenuFont;
		}
	}
}

