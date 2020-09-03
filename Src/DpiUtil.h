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

	inline GetDpiForWindowType UnsafeGetDpiForWindow = nullptr;
	inline SystemParametersInfoForDpiType UnsafeSystemParametersInfoForDpi = nullptr;
	inline GetSystemMetricsForDpiType UnsafeGetSystemMetricsForDpi = nullptr;
	inline OpenThemeDataForDpiType UnsafeOpenThemeDataForDpi = nullptr;

	inline bool succeeded = []()
	{
		HMODULE hLibraryUser32 = GetModuleHandleW(L"user32.dll");
		if (!hLibraryUser32)
			return false;
		HMODULE hLibraryUxTheme = GetModuleHandleW(L"uxtheme.dll");
		if (!hLibraryUxTheme)
			return false;
		UnsafeGetDpiForWindow = reinterpret_cast<GetDpiForWindowType>(GetProcAddress(hLibraryUser32, "GetDpiForWindow"));
		UnsafeSystemParametersInfoForDpi = reinterpret_cast<SystemParametersInfoForDpiType>(GetProcAddress(hLibraryUser32, "SystemParametersInfoForDpi"));
		UnsafeGetSystemMetricsForDpi = reinterpret_cast<GetSystemMetricsForDpiType>(GetProcAddress(hLibraryUser32, "GetSystemMetricsForDpi"));
		UnsafeOpenThemeDataForDpi = reinterpret_cast<OpenThemeDataForDpiType>(GetProcAddress(hLibraryUxTheme, "OpenThemeDataForDpi"));
		return true;
	}();

	inline int GetDpiForWindow(HWND hwnd)
	{
		if (UnsafeGetDpiForWindow)
			return UnsafeGetDpiForWindow(hwnd);
		return CClientDC(CWnd::FromHandle(hwnd)).GetDeviceCaps(LOGPIXELSX);
	}
	
	inline int GetSystemMetricsForDpi(int nIndex, int dpi)
	{
		if (UnsafeGetSystemMetricsForDpi)
			return UnsafeGetSystemMetricsForDpi(nIndex, dpi);
		return GetSystemMetrics(nIndex);
	}

	inline HTHEME OpenThemeDataForDpi(HWND hwnd, LPCWSTR pszClassList, UINT dpi)
	{
		if (DpiUtil::UnsafeOpenThemeDataForDpi)
			return UnsafeOpenThemeDataForDpi(hwnd, pszClassList, dpi);
		return OpenThemeData(hwnd, pszClassList);
	}

	inline void GetMenuLogFont(int dpi, LOGFONT& logFont)
	{
		if (DpiUtil::UnsafeGetSystemMetricsForDpi)
		{
			DpiUtil::NONCLIENTMETRICS6 ncm = { sizeof DpiUtil::NONCLIENTMETRICS6 };
			if (DpiUtil::UnsafeSystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof DpiUtil::NONCLIENTMETRICS6, &ncm, 0, dpi))
				logFont = ncm.lfMenuFont;
		}
		else
		{
			NONCLIENTMETRICS ncm = { sizeof NONCLIENTMETRICS };
			if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof NONCLIENTMETRICS, &ncm, 0))
				logFont = ncm.lfMenuFont;
		}
	}

	template<class T>
	class PerMonitorDpiAwareWindow
	{
		T* wnd() { return static_cast<T*>(this); }
	public:
		int GetDpi()
		{
			if (m_dpi == -1)
				m_dpi = GetDpiForWindow(wnd()->m_hWnd);
			return m_dpi;
		}

		virtual void UpdateDpi(int dpi)
		{
			m_dpi = dpi;
		}

		int GetSystemMetrics(int nIndex)
		{
			return GetSystemMetricsForDpi(nIndex, GetDpi());
		}

	protected:
		int m_dpi = -1;
	};
}

