/////////////////////////////////////////////////////////////////////////////
//    Dark mode for WinMerge
//    Copyright (C) 2025 ozone10
//    SPDX-License-Identifier: MPL-2.0
/////////////////////////////////////////////////////////////////////////////
/**
 * @file  MergeDarkMode.cpp
 *
 * @brief Implementation file for Dark mode for WinMerge
 *
 */


#include "StdAfx.h"
#include "MergeDarkMode.h"
#include "Win_VersionHelper.h"
#include "Resource.h"

#include <limits>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#if defined(USE_DARKMODELIB)
namespace WinMergeDarkMode
{
	/**
	 * @brief Convert RGB color model to HSL.
	 */
	static void RGBToHSL(BYTE r, BYTE g, BYTE b, double& h, double& s, double& l)
	{
		const double rf = r / 255.0;
		const double gf = g / 255.0;
		const double bf = b / 255.0;

		const double max = std::max<double>({ rf, gf, bf });
		const double min = std::min<double>({ rf, gf, bf });
		l = (max + min) / 2.0;

		static constexpr auto isFloatEqual = [](double n, double m) -> bool
		{
			return std::fabs(n - m) < std::numeric_limits<double>::epsilon();
		};

		if (isFloatEqual(max, min))
		{
			h = s = 0.0;
		}
		else
		{
			const double delta = max - min;
			s = (l > 0.5) ? delta / (2.0 - max - min) : delta / (max + min);

			if (isFloatEqual(max, rf))
				h = (gf - bf) / delta + (gf < bf ? 6.0 : 0.0);
			else if (isFloatEqual(max, gf))
				h = (bf - rf) / delta + 2.0;
			else
				h = (rf - gf) / delta + 4.0;

			h /= 6.0;
		}
	}

	/**
	 * @brief Convert HSL color model to RGB.
	 */
	static void HSLToRGB(double h, double s, double l, BYTE& r, BYTE& g, BYTE& b)
	{
		auto hueToRGB = [](double p, double q, double t) -> double
		{
			if (t < 0.0) t += 1.0;
			if (t > 1.0) t -= 1.0;
			if (t < 1.0 / 6.0) return p + (q - p) * 6.0 * t;
			if (t < 1.0 / 2.0) return q;
			if (t < 2.0 / 3.0) return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
			return p;
		};

		if (s == 0.0)
		{
			r = g = b = static_cast<BYTE>(l * 255.0);
		}
		else
		{
			const double q = (l < 0.5) ? (l * (1.0 + s)) : (l + s - l * s);
			const double p = 2.0 * l - q;
			r = static_cast<BYTE>(hueToRGB(p, q, h + 1.0 / 3.0) * 255.0);
			g = static_cast<BYTE>(hueToRGB(p, q, h) * 255.0);
			b = static_cast<BYTE>(hueToRGB(p, q, h - 1.0 / 3.0) * 255.0);
		}
	}

	/**
	 * @brief Convert bitmap image to use 32-bit color depth, if needed.
	 */
	static void ConvertTo32Bit(CImage& image)
	{
		if (image.GetBPP() != 32)
		{
			CImage convertedImage;
			convertedImage.Create(image.GetWidth(), image.GetHeight(), 32, CImage::createAlphaChannel);

			HDC hdcSrc = image.GetDC();
			HDC hdcDest = convertedImage.GetDC();

			BitBlt(hdcDest, 0, 0, image.GetWidth(), image.GetHeight(), hdcSrc, 0, 0, SRCCOPY);

			image.ReleaseDC();
			convertedImage.ReleaseDC();

			image.Destroy();
			image.Attach(convertedImage.Detach());
		}
	}

	/**
	 * @brief Invert bitmap image colors.
	 */
	void InvertLightness(CImage& image)
	{
		const int width = image.GetWidth();
		const int height = image.GetHeight();

		static constexpr double minLightness = 0.125;
		static constexpr double maxLightness = 1.0;

		static constexpr int threshold = 240;
		const COLORREF darkBackgroundColor = DarkMode::getDlgBackgroundColor();

		ConvertTo32Bit(image);
		if (image.GetBPP() != 32)
			return;

		BYTE* pData = reinterpret_cast<BYTE*>(image.GetBits());
		if (!pData)
			return;

		const int pitch = image.GetPitch();

		const BYTE darkR = GetRValue(darkBackgroundColor);
		const BYTE darkG = GetGValue(darkBackgroundColor);
		const BYTE darkB = GetBValue(darkBackgroundColor);

		for (int y = 0; y < height; ++y)
		{
			RGBQUAD* row = reinterpret_cast<RGBQUAD*>(pData + y * pitch);
			for (int x = 0; x < width; ++x)
			{
				RGBQUAD& pixel = row[x];

				if (pixel.rgbRed >= threshold &&
					pixel.rgbGreen >= threshold &&
					pixel.rgbBlue >= threshold)
				{
					pixel.rgbRed = darkR;
					pixel.rgbGreen = darkG;
					pixel.rgbBlue = darkB;
				}
				else
				{
					double h = 0;
					double s = 0;
					double l = 0;
					RGBToHSL(pixel.rgbRed, pixel.rgbGreen, pixel.rgbBlue, h, s, l);

					l = minLightness + (maxLightness - l);
					l = std::max<double>(0.0, std::min<double>(1.0, l));

					BYTE newR = 0;
					BYTE newG = 0;
					BYTE newB = 0;
					HSLToRGB(h, s, l, newR, newG, newB);

					pixel.rgbRed = newR;
					pixel.rgbGreen = newG;
					pixel.rgbBlue = newB;
				}
			}
		}
	}

	static constexpr UINT_PTR g_aboutAsciiSubclassID = 42;

	/**
	 * @brief Sublass procedure for ascii art in About dialog.
	 */
	static LRESULT CALLBACK AsciiCtlColorSubclass(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam,
		UINT_PTR uIdSubclass,
		[[maybe_unused]] DWORD_PTR /*dwRefData*/
	)
	{
		switch (uMsg)
		{
			case WM_NCDESTROY:
			{
				::RemoveWindowSubclass(hWnd, AsciiCtlColorSubclass, uIdSubclass);
				break;
			}

			case WM_CTLCOLORSTATIC:
			{
				if (DarkMode::isEnabled())
				{
					auto hdc = reinterpret_cast<HDC>(wParam);

					auto hChild = reinterpret_cast<HWND>(lParam);
					const int id = ::GetDlgCtrlID(hChild);

					if (id == IDC_GNU_ASCII)
					{
						::SetTextColor(hdc, DarkMode::getDarkerTextColor());
						::SetBkMode(hdc, TRANSPARENT);
						return reinterpret_cast<LRESULT>(static_cast<HBRUSH>(::GetStockObject(NULL_BRUSH)));
					}
				}
				break;
			}
		}
		return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}

	/**
	 * @brief Set sublass procedure for ascii art in About dialog.
	 */
	void SetAsciiArtSubclass(HWND hWnd)
	{
		if (::GetWindowSubclass(hWnd, AsciiCtlColorSubclass, g_aboutAsciiSubclassID, nullptr) == FALSE)
		{
			::SetWindowSubclass(hWnd, AsciiCtlColorSubclass, g_aboutAsciiSubclassID, 0);
		}
	}

	/**
	 * @brief Get dark mode type based on color mode.
	 */
	DarkMode::DarkModeType GetDarkModeType(int colorMode)
	{
		if (colorMode == 1)
			return DarkMode::DarkModeType::dark;
		if (colorMode == 2 && DarkMode::isDarkModeReg())
			return DarkMode::DarkModeType::dark;
		return DarkMode::DarkModeType::classic;
	}

	/**
	 * @brief Check if dark mode is available.
	 */
	bool IsDarkModeAvailable()
	{
		return IsWin10_OrGreater();
	}

	/**
	 * @brief Check if the section name is "ImmersiveColorSet".
	 */
	bool IsImmersiveColorSet(LPCTSTR lpszSection)
	{
		return lstrcmp(lpszSection, _T("ImmersiveColorSet")) == 0;
	}

} // namespace WinMergeDarkMode
#endif // USE_DARKMODELIB
