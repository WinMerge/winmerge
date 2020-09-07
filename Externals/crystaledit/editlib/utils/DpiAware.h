#pragma once

#include <afxwin.h>

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#define WM_DPICHANGED_BEFOREPARENT 0x02E2
#define WM_DPICHANGED_AFTERPARENT 0x02E3
#endif

namespace DpiAware
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

	using AdjustWindowRectExForDpiType = BOOL (__stdcall*)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);
	using GetDpiForWindowType = UINT (__stdcall*)(HWND hwnd);
	using SystemParametersInfoForDpiType = BOOL(__stdcall*)(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);
	using GetSystemMetricsForDpiType = int(__stdcall*)(int nIndex, UINT dpi);
	using OpenThemeDataForDpiType = HTHEME(__stdcall*)(HWND hwnd, LPCWSTR pszClassList, UINT dpi);
	using LoadIconWithScaleDownType = HRESULT(__stdcall*)(HINSTANCE hinst, PCWSTR pszName, int cx, int cy, HICON *phico);

	extern bool DpiAwareSupport;
	extern short SMIconOnInit;
	extern AdjustWindowRectExForDpiType AdjustWindowRectExForDpi;
	extern GetDpiForWindowType GetDpiForWindow;
	extern SystemParametersInfoForDpiType SystemParametersInfoForDpi;
	extern GetSystemMetricsForDpiType GetSystemMetricsForDpi;
	extern OpenThemeDataForDpiType OpenThemeDataForDpi;
	extern LoadIconWithScaleDownType LoadIconWithScaleDown;

	void GetPointLogFont(LOGFONT& logFont, float point, const TCHAR* lfFaceName, int dpi);
	bool GetNonClientLogFont(LOGFONT& logFont, size_t memberOffset, int dpi);
	void UpdateAfxDataSysMetrics(int dpi);

	template<class T>
	class PerMonitorDpiAwareWindow
	{
		T* wnd() { return static_cast<T*>(this); }
	public:
		int GetDpi()
		{
			if (m_dpi == -1)
				UpdateDpi();
			return m_dpi;
		}

		int PointToPixel(int point) { return MulDiv(point, GetDpi(), 72); }
		template<class T>
		int PointToPixel(T point) { return static_cast<int>((point * GetDpi()) / 72); }
		void UpdateDpi()
		{
			m_dpi = GetDpiForWindow(wnd()->m_hWnd);
			m_cxSMIcon = static_cast<short>(GetSystemMetricsForDpi(SM_CXSMICON, m_dpi));
		}
		int GetSystemMetrics(int nIndex) { return GetSystemMetricsForDpi(nIndex, GetDpi()); }
		BOOL AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle)
		{
			return DpiAware::AdjustWindowRectExForDpi(lpRect, dwStyle, bMenu, dwExStyle, GetDpi());
		}

		void CalcWindowRectImpl(LPRECT lpClientRect, UINT nAdjustType = CWnd::adjustBorder)
		{
			DWORD dwExStyle = GetWindowLong(wnd()->m_hWnd, GWL_EXSTYLE);
			if (nAdjustType == 0)
				dwExStyle &= ~WS_EX_CLIENTEDGE;
			DWORD dwStyle = (DWORD)GetWindowLong(wnd()->m_hWnd, GWL_STYLE);
			DpiAware::AdjustWindowRectExForDpi(lpClientRect, dwStyle, FALSE, dwExStyle, GetDpi());
		}

		bool IsDpiChanged() const
		{
			return SMIconOnInit != m_cxSMIcon;
		}

	protected:
		short m_cxSMIcon = SMIconOnInit;
		int m_dpi = -1;
	};

}

