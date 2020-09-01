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

	inline auto GetDpiForWindow = reinterpret_cast<GetDpiForWindowType>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow"));
	inline auto SystemParametersInfoForDpi = reinterpret_cast<SystemParametersInfoForDpiType>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SystemParametersInfoForDpi"));
	inline auto GetSystemMetricsForDpi = reinterpret_cast<GetSystemMetricsForDpiType>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetSystemMetricsForDpi"));

	int GetSystemMetricsForWindow(CWnd *pWnd, int nIndex)
	{
		if (GetSystemMetricsForDpi)
			return GetSystemMetricsForDpi(nIndex, GetDpiForWindow(pWnd->m_hWnd));
		return GetSystemMetrics(nIndex);
	}
}

