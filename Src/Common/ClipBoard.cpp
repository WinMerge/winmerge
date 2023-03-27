/**
 * @file  ClipBoard.cpp
 *
 * @brief ClipBoard helper functions implementations.
 */

#include "pch.h"
#include "ClipBoard.h"
#include <ShlObj.h>

inline CLIPFORMAT GetClipTcharTextFormat() { return (sizeof(tchar_t) == 1 ? CF_TEXT : CF_UNICODETEXT); }

/**
 * @brief Copies string to clipboard.
 * @param [in] text Text to copy to clipboard.
 * @param [in] currentWindowHandle Handle to current window.
 * @return `true` if text copying succeeds, `false` otherwise.
 */
template<>
bool PutToClipboard<HWND>(const String & text, HWND currentWindowHandle)
{
	if (text.empty())
		return false;

	bool bOK = false;
	if (OpenClipboard(currentWindowHandle))
	{
		EmptyClipboard();
		const size_t dataSiz = text.length() + 1;
		HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, dataSiz * sizeof(tchar_t));
		if (hData != nullptr)
		{
			if (tchar_t* pszData = static_cast<tchar_t*>(::GlobalLock(hData)))
			{
				tc::tcslcpy(pszData, dataSiz, text.c_str());
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
bool GetFromClipboard(String & text)
{
	bool bSuccess = false;
	if (OpenClipboard(nullptr))
	{
		CLIPFORMAT fmt = GetClipTcharTextFormat();
		HGLOBAL hData = GetClipboardData(fmt);
		if (hData != nullptr)
		{
			tchar_t* pszData = (tchar_t*) GlobalLock(hData);
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

template<>
void PutFilesToClipboardInternal<HWND>(const String& strPaths, const String& strPathsSepSpc, HWND currentWindowHandle)
{
	// CF_HDROP
	HGLOBAL hDrop = GlobalAlloc(GHND, sizeof(DROPFILES) + sizeof(tchar_t) * strPaths.length());
	if (hDrop == nullptr)
		return;
	if (tchar_t* pDrop = static_cast<tchar_t*>(GlobalLock(hDrop)))
	{
		DROPFILES df = { 0 };
		df.pFiles = sizeof(DROPFILES);
		df.fWide = (sizeof(tchar_t) > 1);
		memcpy(pDrop, &df, sizeof(DROPFILES));
		memcpy((BYTE*)pDrop + sizeof(DROPFILES), (const tchar_t*)strPaths.c_str(), sizeof(tchar_t) * strPaths.length());
		GlobalUnlock(hDrop);
	}

	// CF_DROPEFFECT
	HGLOBAL hDropEffect = GlobalAlloc(GHND, sizeof(DWORD));
	if (hDropEffect == nullptr)
	{
		GlobalFree(hDrop);
		return;
	}
	if (DWORD* p = static_cast<DWORD*>(GlobalLock(hDropEffect)))
	{
		*p = DROPEFFECT_COPY;
		GlobalUnlock(hDropEffect);
	}

	// CF_UNICODETEXT
	HGLOBAL hPathnames = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(tchar_t) * (strPathsSepSpc.length() + 1));
	if (hPathnames == nullptr)
	{
		GlobalFree(hDrop);
		GlobalFree(hDropEffect);
		return;
	}
	if (void* pPathnames = GlobalLock(hPathnames))
	{
		memcpy((BYTE*)pPathnames, (const tchar_t*)strPathsSepSpc.c_str(), sizeof(tchar_t) * strPathsSepSpc.length());
		((tchar_t*)pPathnames)[strPathsSepSpc.length()] = 0;
		GlobalUnlock(hPathnames);
	}

	UINT CF_DROPEFFECT = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	if (::OpenClipboard(currentWindowHandle))
	{
		EmptyClipboard();
		SetClipboardData(CF_HDROP, hDrop);
		SetClipboardData(CF_DROPEFFECT, hDropEffect);
		SetClipboardData(GetClipTcharTextFormat(), hPathnames);
		CloseClipboard();
	}
}

