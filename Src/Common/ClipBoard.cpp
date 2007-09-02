/**
 * @file  ClipBoard.cpp
 *
 * @brief ClipBoard helper functions implementations.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "ClipBoard.h"

/**
 * @brief Copies string to clipboard.
 * @param [in] pszText Text to copy to clipboard.
 * @param [in] currentWindowHandle Handle to current window.
 * @return TRUE if text copying succeeds, FALSE otherwise.
 */
BOOL PutToClipboard(LPCTSTR pszText, HWND currentWindowHandle)
{
	if (pszText == NULL || _tcslen(pszText) == 0)
		return FALSE;

	CWaitCursor wc;
	BOOL bOK = FALSE;
	if (OpenClipboard(currentWindowHandle))
	{
		EmptyClipboard();
		HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (_tcslen(pszText)+1) * sizeof(TCHAR));
		if (hData != NULL)
		{
			LPTSTR pszData = (LPTSTR)::GlobalLock(hData);
			_tcscpy(pszData, pszText);
			GlobalUnlock(hData);
			UINT fmt = GetClipTcharTextFormat();
			bOK = SetClipboardData(fmt, hData) != NULL;
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
BOOL GetFromClipboard(CString & text, HWND currentWindowHandle)
{
	BOOL bSuccess = FALSE;
	if (OpenClipboard(currentWindowHandle))
	{
		UINT fmt = GetClipTcharTextFormat();
		HGLOBAL hData = GetClipboardData(fmt);
		if (hData != NULL)
		{
			LPTSTR pszData = (LPTSTR) GlobalLock(hData);
			if (pszData != NULL)
			{
				text = pszData;
				GlobalUnlock(hData);
				bSuccess = TRUE;
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
BOOL TextInClipboard()
{
	UINT fmt = GetClipTcharTextFormat();
	return IsClipboardFormatAvailable(fmt);
}
