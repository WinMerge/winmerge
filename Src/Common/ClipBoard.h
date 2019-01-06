/**
 * @file  ClipBoard.h
 *
 * @brief ClipBoard helper functions definitions.
 */
#pragma once

#include <windows.h>
#include "UnicodeString.h"

inline CLIPFORMAT GetClipTcharTextFormat() { return (sizeof(TCHAR) == 1 ? CF_TEXT : CF_UNICODETEXT); }

bool PutToClipboard(const String & text, HWND currentWindowHandle);
bool GetFromClipboard(String & text, HWND currentWindowHandle);

template<class Container>
void PutFilesToClipboard(const Container& list, HWND currentWindowHandle)
{
	String strPaths, strPathsSepSpc;
	strPaths.reserve(list.size() * MAX_PATH_FULL);
	strPathsSepSpc.reserve(list.size() * MAX_PATH_FULL);

	for (Container::const_iterator it = list.begin(); it != list.end(); ++it)
	{
		String path = *it;

		strPaths += path;
		strPaths += _T('\0');

		strPathsSepSpc += _T("\"");
		strPathsSepSpc += path;
		strPathsSepSpc += _T("\" ");
	}

	strPaths += _T('\0');
	strPathsSepSpc = strutils::trim_ws_end(strPathsSepSpc);

	// CF_HDROP
	HGLOBAL hDrop = GlobalAlloc(GHND, sizeof(DROPFILES) + sizeof(TCHAR) * strPaths.length());
	if (hDrop == nullptr)
		return;
	if (TCHAR *pDrop = static_cast<TCHAR *>(GlobalLock(hDrop)))
	{
		DROPFILES df = {0};
		df.pFiles = sizeof(DROPFILES);
		df.fWide = (sizeof(TCHAR) > 1);
		memcpy(pDrop, &df, sizeof(DROPFILES));
		memcpy((BYTE *)pDrop + sizeof(DROPFILES), (LPCTSTR)strPaths.c_str(), sizeof(TCHAR) * strPaths.length());
		GlobalUnlock(hDrop);
	}

	// CF_DROPEFFECT
	HGLOBAL hDropEffect = GlobalAlloc(GHND, sizeof(DWORD));
	if (hDropEffect == nullptr)
	{
		GlobalFree(hDrop);
		return;
	}
	if (DWORD *p = static_cast<DWORD *>(GlobalLock(hDropEffect)))
	{
		*p = DROPEFFECT_COPY;
		GlobalUnlock(hDropEffect);
	}

	// CF_UNICODETEXT
	HGLOBAL hPathnames = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(TCHAR) * (strPathsSepSpc.length() + 1));
	if (hPathnames == nullptr)
	{
		GlobalFree(hDrop);
		GlobalFree(hDropEffect);
		return;
	}
	if (void *pPathnames = GlobalLock(hPathnames))
	{
		memcpy((BYTE *)pPathnames, (LPCTSTR)strPathsSepSpc.c_str(), sizeof(TCHAR) * strPathsSepSpc.length());
		((TCHAR *)pPathnames)[strPathsSepSpc.length()] = 0;
		GlobalUnlock(hPathnames);
	}

	UINT CF_DROPEFFECT = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	if (::OpenClipboard(AfxGetMainWnd()->GetSafeHwnd()))
	{
		EmptyClipboard();
		SetClipboardData(CF_HDROP, hDrop);
		SetClipboardData(CF_DROPEFFECT, hDropEffect);
		SetClipboardData(GetClipTcharTextFormat(), hPathnames);
		CloseClipboard();
	}
}
