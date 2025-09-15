// SPDX-License-Identifier: MIT
/**
 * @file darkmodelib.h
 * @brief Header file for darkmode.dll
 */

#pragma once
#include <windows.h>

constexpr DWORD CC_FLAGS_DARK = CC_RGBINIT | CC_FULLOPEN | CC_ENABLEHOOK;

namespace ATL { class CImage; };
namespace WinMergeDarkMode { inline void LoadFunc(HMODULE hDll); }

namespace DarkMode
{
	enum class ToolTipsType : unsigned char { tooltip, toolbar, listview, treeview, tabbar, trackbar, rebar };
	enum class DarkModeType : unsigned char { light = 0, dark = 1, classic = 3 };

	inline void DummyInitDarkMode() {}
	inline void DummySetDarkWndSafe(HWND, bool) {}
	inline void DummySetDarkWndNotifySafe(HWND, bool) {}
	inline void DummySetWindowEraseBgSubclass(HWND) {}
	inline void DummySetDarkScrollBar(HWND) {}
	inline void DummySetDarkTooltips(HWND, ToolTipsType) {}
	inline void DummySetDarkListViewCheckboxes(HWND) {}
	inline void DummySetChildCtrlsTheme(HWND) {}
	inline void DummySetWindowCtlColorSubclass(HWND) {}
	inline void DummySetChildCtrlsSubclassAndTheme(HWND, bool, bool) {}
	inline void DummySetTabCtrlUpDownSubclass(HWND) {}
	inline void DummySetDarkModeConfig(UINT) {}
	inline void DummySetDefaultColors(bool) {}
	inline void DummySetDarkTitleBarEx(HWND, bool) {}
	inline void DummySetListViewCtrlSubclass(HWND) {}
	inline void DummySetDarkThemeExperimental(HWND, const wchar_t*) {}
	inline void DummySetWindowNotifyCustomDrawSubclass(HWND) {}
	inline void DummySetTabCtrlSubclass(HWND) {}
	inline void DummyEnableSysLinkCtrlCtlColor(HWND) {}
	inline void DummyRemoveWindowEraseBgSubclass(HWND) {}
	inline bool DummyIsEnabled() { return false; }
	inline bool DummyIsExperimentalActive() { return false; }
	inline bool DummyIsExperimentalSupported() { return false; }
	inline bool DummyIsDarkModeReg() { return false; }
	inline COLORREF DummyGetDarkerTextColor() { return RGB(0,0,0); }
	inline COLORREF DummyGetDlgBackgroundColor() { return RGB(255,255,255); }
	inline COLORREF DummyGetBackgroundColor() { return RGB(255,255,255); }
	inline HBRUSH DummyGetDlgBackgroundBrush() { return nullptr; }
	inline UINT_PTR DummyHookDlgProc(HWND, UINT, WPARAM, LPARAM) { return 0; }

	inline void (*initDarkMode)() = DummyInitDarkMode;
	inline void (*setDarkWndSafe)(HWND, bool) = DummySetDarkWndSafe;
	inline void (*setDarkWndNotifySafe)(HWND, bool) = DummySetDarkWndNotifySafe;
	inline void (*setWindowEraseBgSubclass)(HWND) = DummySetWindowEraseBgSubclass;
	inline void (*setDarkScrollBar)(HWND) = DummySetDarkScrollBar;
	inline void (*setDarkTooltips)(HWND, ToolTipsType) = DummySetDarkTooltips;
	inline void (*setDarkListViewCheckboxes)(HWND) = DummySetDarkListViewCheckboxes;
	inline void (*setChildCtrlsTheme)(HWND) = DummySetChildCtrlsTheme;
	inline void (*setWindowCtlColorSubclass)(HWND) = DummySetWindowCtlColorSubclass;
	inline void (*setChildCtrlsSubclassAndTheme)(HWND, bool, bool) = DummySetChildCtrlsSubclassAndTheme;
	inline void (*setTabCtrlUpDownSubclass)(HWND) = DummySetTabCtrlUpDownSubclass;
	inline void (*setDarkModeConfig)(UINT) = DummySetDarkModeConfig;
	inline void (*setDefaultColors)(bool) = DummySetDefaultColors;
	inline void (*setDarkTitleBarEx)(HWND, bool) = DummySetDarkTitleBarEx;
	inline void (*setListViewCtrlSubclass)(HWND) = DummySetListViewCtrlSubclass;
	inline void (*setDarkThemeExperimental)(HWND, const wchar_t*) = DummySetDarkThemeExperimental;
	inline void (*setWindowNotifyCustomDrawSubclass)(HWND) = DummySetWindowNotifyCustomDrawSubclass;
	inline void (*setTabCtrlSubclass)(HWND) = DummySetTabCtrlSubclass;
	inline void (*enableSysLinkCtrlCtlColor)(HWND) = DummyEnableSysLinkCtrlCtlColor;
	inline void (*removeWindowEraseBgSubclass)(HWND) = DummyRemoveWindowEraseBgSubclass;
	inline bool (*isEnabled)() = DummyIsEnabled;
	inline bool (*isExperimentalActive)() = DummyIsExperimentalActive;
	inline bool (*isExperimentalSupported)() = DummyIsExperimentalSupported;
	inline bool (*isDarkModeReg)() = DummyIsDarkModeReg;
	inline COLORREF (*getDarkerTextColor)() = DummyGetDarkerTextColor;
	inline COLORREF (*getDlgBackgroundColor)() = DummyGetDlgBackgroundColor;
	inline COLORREF (*getBackgroundColor)() = DummyGetBackgroundColor;
	inline HBRUSH (*getDlgBackgroundBrush)() = DummyGetDlgBackgroundBrush;
	inline UINT_PTR (*HookDlgProc)(HWND, UINT, WPARAM, LPARAM) = DummyHookDlgProc;

	inline bool LoadDarkModeDll(const wchar_t* dllName = L"darkmodelib\\darkmode.dll")
	{
		HMODULE hDll = LoadLibraryW(dllName);
		if (!hDll) return false;

#define LOAD_FN(fn) fn = (decltype(fn))GetProcAddress(hDll, "DarkMode_"#fn)

		LOAD_FN(initDarkMode);
		LOAD_FN(setDarkWndSafe);
		LOAD_FN(setDarkWndNotifySafe);
		LOAD_FN(setWindowEraseBgSubclass);
		LOAD_FN(setDarkScrollBar);
		LOAD_FN(setDarkTooltips);
		LOAD_FN(setDarkListViewCheckboxes);
		LOAD_FN(setChildCtrlsTheme);
		LOAD_FN(setWindowCtlColorSubclass);
		LOAD_FN(setChildCtrlsSubclassAndTheme);
		LOAD_FN(setTabCtrlUpDownSubclass);
		LOAD_FN(setDarkModeConfig);
		LOAD_FN(setDefaultColors);
		LOAD_FN(setDarkTitleBarEx);
		LOAD_FN(setListViewCtrlSubclass);
		LOAD_FN(setDarkThemeExperimental);
		LOAD_FN(setWindowNotifyCustomDrawSubclass);
		LOAD_FN(setTabCtrlSubclass);
		LOAD_FN(enableSysLinkCtrlCtlColor);
		LOAD_FN(removeWindowEraseBgSubclass);
		LOAD_FN(isEnabled);
		LOAD_FN(isExperimentalActive);
		LOAD_FN(isExperimentalSupported);
		LOAD_FN(isDarkModeReg);
		LOAD_FN(getDarkerTextColor);
		LOAD_FN(getDlgBackgroundColor);
		LOAD_FN(getBackgroundColor);
		LOAD_FN(getDlgBackgroundBrush);
		LOAD_FN(HookDlgProc);
		WinMergeDarkMode::LoadFunc(hDll);

#undef LOAD_FN
		return true;
	}
}

namespace WinMergeDarkMode
{
	inline void DummyInvertLightness(ATL::CImage&) {}
	inline void DummySetAsciiArtSubclass(HWND) {}

	inline void (*InvertLightness)(ATL::CImage&) = DummyInvertLightness;
	inline void (*SetAsciiArtSubclass)(HWND) = DummySetAsciiArtSubclass;

	inline void LoadFunc(HMODULE hDll)
	{
		InvertLightness = (decltype(InvertLightness))GetProcAddress(hDll, "WinMergeDarkMode_InvertLightness");
		SetAsciiArtSubclass = (decltype(SetAsciiArtSubclass))GetProcAddress(hDll, "WinMergeDarkMode_SetAsciiArtSubclass");
	}

	/**
	 * @brief Get dark mode type based on color mode.
	 */
	inline DarkMode::DarkModeType GetDarkModeType(int colorMode)
	{
		if (colorMode == 1)
			return DarkMode::DarkModeType::dark;
		if (colorMode == 2 && DarkMode::isDarkModeReg())
			return DarkMode::DarkModeType::dark;
		return DarkMode::DarkModeType::classic;
	}

	/**
	 * @brief Get effective color mode based on color mode.
	 */
	inline int GetEffectiveColorMode(int colorMode)
	{
		if (colorMode == 1)
			return 1;
		if (colorMode == 2 && DarkMode::isDarkModeReg())
			return 1;
		return 0;
	}
	inline bool IsImmersiveColorSet(LPCTSTR lpszSection) { return lstrcmp(lpszSection, _T("ImmersiveColorSet")) == 0; }
}
