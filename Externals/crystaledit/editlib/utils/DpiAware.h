#pragma once

#include <afxwin.h>
#include <type_traits>
#include "mfc_templ_defines.h"

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#define WM_DPICHANGED_BEFOREPARENT 0x02E2
#define WM_DPICHANGED_AFTERPARENT 0x02E3
#endif

namespace DpiAware
{
	struct NONCLIENTMETRICS6 : public NONCLIENTMETRICS
	{
		int     iPaddedBorderWidth;
	};

	using AdjustWindowRectExForDpiType = BOOL(__stdcall*)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);
	using GetDpiForWindowType = UINT(__stdcall*)(HWND hwnd);
	using SystemParametersInfoForDpiType = BOOL(__stdcall*)(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);
	using GetSystemMetricsForDpiType = int(__stdcall*)(int nIndex, UINT dpi);
	using OpenThemeDataForDpiType = HTHEME(__stdcall*)(HWND hwnd, LPCWSTR pszClassList, UINT dpi);
	using LoadIconWithScaleDownType = HRESULT(__stdcall*)(HINSTANCE hinst, PCWSTR pszName, int cx, int cy, HICON* phico);

	extern bool DpiAwareSupport;
	extern int DPIOnInit;

	extern AdjustWindowRectExForDpiType AdjustWindowRectExForDpi;
	extern GetDpiForWindowType GetDpiForWindow;
	extern SystemParametersInfoForDpiType SystemParametersInfoForDpi;
	extern GetSystemMetricsForDpiType GetSystemMetricsForDpi;
	extern OpenThemeDataForDpiType OpenThemeDataForDpi;
	extern LoadIconWithScaleDownType LoadIconWithScaleDown;

	void GetPointLogFont(LOGFONT& logFont, float point, const TCHAR* lfFaceName, int dpi);
	bool GetNonClientLogFont(LOGFONT& logFont, size_t memberOffset, int dpi);
	void UpdateAfxDataSysMetrics(int dpi);
	void ListView_UpdateColumnWidths(HWND hwnd, int olddpi, int newdpi);
	void Dialog_UpdateControlInnerWidths(HWND hwnd, int olddpi, int newdpi);
	HIMAGELIST LoadShellImageList(int dpi);

	template<class Base>
	class CDpiAwareWnd : public Base
	{
	public:
		using this_type = CDpiAwareWnd;
		using base_type = Base;
		using Base::Base;
		
		int GetDpi() const { return m_dpi; }

		int PointToPixel(int point) const { return MulDiv(point, m_dpi, 72); }
		template<class T>
		int PointToPixel(T point) const { return static_cast<int>((point * m_dpi) / 72); }
		void UpdateDpi()
		{
			m_dpi = GetDpiForWindow(m_hWnd);
		}
		int GetSystemMetrics(int nIndex) { return GetSystemMetricsForDpi(nIndex, m_dpi); }
		BOOL AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle)
		{
			return DpiAware::AdjustWindowRectExForDpi(lpRect, dwStyle, bMenu, dwExStyle, m_dpi);
		}

		virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = CWnd::adjustBorder)
		{
			DWORD dwExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
			if (nAdjustType == 0)
				dwExStyle &= ~WS_EX_CLIENTEDGE;
			DWORD dwStyle = (DWORD)GetWindowLong(m_hWnd, GWL_STYLE);
			DpiAware::AdjustWindowRectExForDpi(lpClientRect, dwStyle, FALSE, dwExStyle, m_dpi);
		}

		bool IsDifferentDpiFromSystemDpi() const
		{
			return DPIOnInit != m_dpi;
		}

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			UpdateDpi();
			return __super::OnCreate(lpCreateStruct);
		}

		afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam)
		{
			UpdateDpi();
			return 0;
		}

		afx_msg LRESULT OnDpiChangedBeforeParent(WPARAM wParam, LPARAM lParam)
		{
			UpdateDpi();
			return 0;
		}

		int m_dpi = DPIOnInit;

		BEGIN_MESSAGE_MAP_INLINE(this_type, base_type)
			ON_WM_CREATE()
			ON_MESSAGE(WM_DPICHANGED, OnDpiChanged)
			ON_MESSAGE(WM_DPICHANGED_BEFOREPARENT, OnDpiChangedBeforeParent)
		END_MESSAGE_MAP_INLINE()
	};

}

