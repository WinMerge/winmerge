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

#include "DarkModeSubclass.h"

// allow only x64 and arm64 for compatibility for older OS
#if !defined(_DARKMODELIB_NOT_USED) \
	&& (defined(__x86_64__) || defined(_M_X64) \
	|| defined(__arm64__) || defined(__arm64) || defined(_M_ARM64))
#define USE_DARKMODELIB
namespace WinMergeDarkMode { inline constexpr bool UseDarkModeLib = true; };
#else
namespace WinMergeDarkMode { inline constexpr bool UseDarkModeLib = false; };
#endif

#if defined(USE_DARKMODELIB)
inline constexpr DWORD CC_FLAGS_DARK = CC_RGBINIT | CC_FULLOPEN | CC_ENABLEHOOK;

namespace ATL
{
	class CImage; // from atlimage.h
};

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

#else

namespace WinMergeDarkMode
{
	inline bool IsDarkModeAvailable() { return false; }
	inline int GetEffectiveColorMode(int colorMode) { return 0; }
}

#endif // USE_DARKMODELIB
