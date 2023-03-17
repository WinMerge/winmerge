/**
 * @file  ClipBoard.h
 *
 * @brief ClipBoard helper functions definitions.
 */
#pragma once

#include "UnicodeString.h"

template<typename WindowHandle>
bool PutToClipboard(const String & text, WindowHandle currentWindowHandle);
bool GetFromClipboard(String & text);
template<typename WindowHandle>
void PutFilesToClipboardInternal(const String& strPaths, const String& strPathsSepSpc, WindowHandle currentWindowHandle);

template<class Container, typename WindowHandle>
void PutFilesToClipboard(const Container& list, WindowHandle currentWindowHandle)
{
	constexpr size_t MaxPathFull = 32767;
	String strPaths, strPathsSepSpc;
	strPaths.reserve(list.size() * MaxPathFull);
	strPathsSepSpc.reserve(list.size() * MaxPathFull);

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

	PutFilesToClipboardInternal(strPaths, strPathsSepSpc, currentWindowHandle);
}
