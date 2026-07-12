/**
 * @file  Clipboard.h
 *
 * @brief Clipboard helper functions definitions.
 */
#pragma once

#include "UnicodeString.h"

namespace ClipboardUtils
{

template<typename WindowHandle>
bool Put(const String & text, WindowHandle currentWindowHandle);

bool Get(String & text);

template<typename WindowHandle>
void PutFilesInternal(const String& strPaths, const String& strPathsSepSpc, WindowHandle currentWindowHandle);

template<class Container, typename WindowHandle>
void PutFiles(const Container& list, WindowHandle currentWindowHandle)
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

	PutFilesInternal(strPaths, strPathsSepSpc, currentWindowHandle);
}

template<typename WindowHandle>
bool PutFileAndText(const String& filename, const String& text, WindowHandle currentWindowHandle);

template<typename WindowHandle>
bool PutFileAndTextAndHTML(const String& filename, const String& text, WindowHandle currentWindowHandle);

}
