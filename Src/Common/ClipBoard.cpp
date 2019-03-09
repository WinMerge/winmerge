/**
 * @file  ClipBoard.cpp
 *
 * @brief ClipBoard helper functions implementations.
 */

#include "pch.h"
#include "ClipBoard.h"

/**
 * @brief Copies string to clipboard.
 * @param [in] text Text to copy to clipboard.
 * @param [in] currentWindowHandle Handle to current window.
 * @return `true` if text copying succeeds, `false` otherwise.
 */
bool PutToClipboard(const String & text, HWND currentWindowHandle)
{
	if (text.empty())
		return false;

	bool bOK = false;
	if (OpenClipboard(currentWindowHandle))
	{
		EmptyClipboard();
		const size_t dataSiz = text.length() + 1;
		HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, dataSiz * sizeof(TCHAR));
		if (hData != nullptr)
		{
			if (LPTSTR pszData = static_cast<LPTSTR>(::GlobalLock(hData)))
			{
				_tcscpy_s(pszData, dataSiz, text.c_str());
				GlobalUnlock(hData);
			}
			CLIPFORMAT fmt = GetClipTcharTextFormat();
			bOK = SetClipboardData(fmt, hData) != nullptr;
		}
		CloseClipboard();
	}
	return bOK;
}

/**
 * @brief Retrieves the string from clipboard.
 * @param [out] text Text copied from clipboard.
 * @param [in] currentWindowHandle Handle to current window.
 * @return `true` if retrieving the clipboard text succeeds, `false` otherwise.
 */
bool GetFromClipboard(String & text, HWND currentWindowHandle)
{
	bool bSuccess = false;
	if (OpenClipboard(currentWindowHandle))
	{
		CLIPFORMAT fmt = GetClipTcharTextFormat();
		HGLOBAL hData = GetClipboardData(fmt);
		if (hData != nullptr)
		{
			LPTSTR pszData = (LPTSTR) GlobalLock(hData);
			if (pszData != nullptr)
			{
				text = pszData;
				GlobalUnlock(hData);
				bSuccess = true;
			}
		}
		CloseClipboard();
	}
	return bSuccess;
}
