/////////////////////////////////////////////////////////////////////////////
//    Dark mode for WinMerge
//    Copyright (C) 2025 ozone10
//    SPDX-License-Identifier: MPL-2.0
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  MergeDarkMode.h
 *
 * @brief Declaration file for Dark mode for WinMerge
 *
 */

#pragma once

// allow only x64 and arm64 for compatibility for older OS
#if !defined(_DARKMODELIB_NOT_USED) \
	&& (defined(__x86_64__) || defined(_M_X64) \
	|| defined(__arm64__) || defined(__arm64) || defined(_M_ARM64))
#include "DarkModeSubclass.h"
#define USE_DARKMODELIB
namespace WinMergeDarkMode { inline constexpr bool UseDarkModeLib = true; };
#else
namespace WinMergeDarkMode { inline constexpr bool UseDarkModeLib = false; };
#endif

inline constexpr DWORD CC_FLAGS_DARK = CC_RGBINIT | CC_FULLOPEN | CC_ENABLEHOOK;

namespace ATL
{
	class CImage; // from atlimage.h
};

#ifndef USE_DARKMODELIB
namespace DarkMode
{
	enum class ToolTipsType : unsigned char { tooltip, toolbar, listview, treeview, tabbar, trackbar, rebar };
	enum class DarkModeType : unsigned char { light = 0, dark = 1, classic = 3 };

	inline void initDarkMode() {}
	inline void setDarkWndSafe(HWND hWnd, bool useWin11Features = true) {}
	inline void setDarkWndNotifySafe(HWND hWnd, bool useWin11Features = true) {}
	inline void setWindowEraseBgSubclass(HWND hWnd) {}
	inline void setDarkScrollBar(HWND hWnd) {}
	inline void setDarkTooltips(HWND hWnd, ToolTipsType type = ToolTipsType::tooltip) {}
	inline void setDarkListViewCheckboxes(HWND hWnd) {}
	inline void setChildCtrlsTheme(HWND hParent) {}
	inline void setWindowCtlColorSubclass(HWND hWnd) {}
	inline void setChildCtrlsSubclassAndTheme(HWND hParent, bool subclass = true, bool theme = true) {}
	inline void setTabCtrlUpDownSubclass(HWND hWnd) {}
	inline void setDarkModeConfig(UINT dmType) {}
	inline void setDefaultColors(bool updateBrushesAndOther) {}
	inline void setDarkTitleBarEx(HWND hWnd, bool useWin11Features) {}
	inline void setListViewCtrlSubclass(HWND hWnd) {}
	inline void setDarkThemeExperimental(HWND hWnd, const wchar_t* themeClassName = L"Explorer") {}
	inline void setWindowNotifyCustomDrawSubclass(HWND hWnd) {}
	inline void setTabCtrlSubclass(HWND hWnd) {}
	inline void enableSysLinkCtrlCtlColor(HWND hWnd) {}
	inline void removeWindowEraseBgSubclass(HWND hWnd) {}
	[[nodiscard]] inline bool isEnabled() { return false; }
	[[nodiscard]] inline bool isExperimentalActive() { return false; }
	[[nodiscard]] inline bool isDarkModeReg() { return false; }
	[[nodiscard]] inline COLORREF getDarkerTextColor() { return 0; }
	[[nodiscard]] inline COLORREF getDlgBackgroundColor() { return 0; }
	[[nodiscard]] inline COLORREF getBackgroundColor() { return 0; }
	[[nodiscard]] inline HBRUSH getDlgBackgroundBrush() { return nullptr; }
	[[nodiscard]] inline UINT_PTR CALLBACK HookDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return FALSE; }
}
#endif

/**
 * @brief Helper class for dark mode for WinMerge
 */
namespace WinMergeDarkMode
{
	/**
	 * @brief Invert bitmap image colors.
	 * @param [in] image Image to invert colors.
	 */
	void InvertLightness(ATL::CImage& image);

	/**
	 * @brief Set sublass procedure for ascii art in About dialog.
	 * @param [in] hWnd About dialog handle.
	 */
	void SetAsciiArtSubclass(HWND hWnd);

	/**
	 * @brief Get dark mode type based on color mode.
	 * @param [in] colorMode Color mode from options.
	 */
	DarkMode::DarkModeType GetDarkModeType(int colorMode);

	/**
	 * @brief Get effective color mode based on color mode.
	 */
	int GetEffectiveColorMode(int colorMode);
	
	/**
	 * @brief Check if dark mode is available.
	 */
	bool IsDarkModeAvailable();

	/**
	 * @brief Check if the section name is "ImmersiveColorSet".
	 */
	bool IsImmersiveColorSet(LPCTSTR lpszSection);
}

