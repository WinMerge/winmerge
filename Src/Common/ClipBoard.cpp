/**
 * @file  ClipBoard.cpp
 *
 * @brief ClipBoard helper functions implementations.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"
#include "ClipBoard.h"

/**
 * @brief Copies string to clipboard.
 * @param [in] text Text to copy to clipboard.
 * @param [in] currentWindowHandle Handle to current window.
 * @return TRUE if text copying succeeds, FALSE otherwise.
 */
bool PutToClipboard(const String & text, HWND currentWindowHandle)
{
	if (text.empty())
		return false;

	CWaitCursor wc;
	bool bOK = false;
	if (OpenClipboard(currentWindowHandle))
	{
		EmptyClipboard();
		HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (text.length()+1) * sizeof(TCHAR));
		if (hData != NULL)
		{
			LPTSTR pszData = (LPTSTR)::GlobalLock(hData);
			_tcscpy(pszData, text.c_str());
			GlobalUnlock(hData);
			bOK = SetClipboardData(CF_UNICODETEXT, hData) != NULL;
		}
		CloseClipboard();
	}
	return bOK;
}

/**
 * @brief Retrieves the string from clipboard.
 * @param [out] text Text copied from clipboard.
 * @param [in] currentWindowHandle Handle to current window.
 * @return TRUE if retrieving the clipboard text succeeds, FALSE otherwise.
 */
bool GetFromClipboard(String & text, HWND currentWindowHandle)
{
	bool bSuccess = false;
	if (OpenClipboard(currentWindowHandle))
	{
		HGLOBAL hData = GetClipboardData(CF_UNICODETEXT);
		if (hData != NULL)
		{
			LPTSTR pszData = (LPTSTR) GlobalLock(hData);
			if (pszData != NULL)
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

/**
 * @brief Checks if the clipboard allows Unicode format.
 * @return TRUE if Unicode is supported, FALSE otherwise.
 */
bool TextInClipboard()
{
	return IsClipboardFormatAvailable(CF_UNICODETEXT) != FALSE;
}
