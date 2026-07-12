/**
 * @file  Clipboard.cpp
 *
 * @brief Clipboard helper functions implementations.
 */

#include "pch.h"
#include "Clipboard.h"
#include "unicoder.h"
#include <ShlObj.h>

namespace ClipboardUtils
{

inline CLIPFORMAT GetClipTcharTextFormat() { return (sizeof(tchar_t) == 1 ? CF_TEXT : CF_UNICODETEXT); }

/**
 * @brief Retrieves the string from clipboard.
 * @param [out] text Text copied from clipboard.
 * @param [in] currentWindowHandle Handle to current window.
 * @return `true` if retrieving the clipboard text succeeds, `false` otherwise.
 */
bool Get(String & text)
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

struct ClipboardData
{
	CLIPFORMAT format;
	HGLOBAL hData;
};

bool SetClipboardDataMultiple(HWND hwnd, std::initializer_list<ClipboardData> list)
{
	if (!OpenClipboard(hwnd))
		return false;

	EmptyClipboard();

	for (auto& item : list)
	{
		if (::SetClipboardData(item.format, item.hData) == nullptr)
		{
			CloseClipboard();
			return false;
		}
	}

	CloseClipboard();
	return true;
}

HGLOBAL CreateClipboardText(const String& text)
{
	if (text.empty())
		return nullptr;
	const size_t dataSize = text.length() + 1;
	HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, dataSize * sizeof(tchar_t));
	if (hData == nullptr)
		return nullptr;
	if (tchar_t* pszData = static_cast<tchar_t*>(GlobalLock(hData)))
	{
		tc::tcslcpy(pszData, dataSize, text.c_str());
		GlobalUnlock(hData);
	}
	return hData;
}

HGLOBAL CreateClipboardHDROP(const String& strPaths)
{
	HGLOBAL hDrop = GlobalAlloc(GHND, sizeof(DROPFILES) + sizeof(tchar_t) * strPaths.length());
	if (hDrop == nullptr)
		return nullptr;
	if (tchar_t* pDrop = static_cast<tchar_t*>(GlobalLock(hDrop)))
	{
		DROPFILES df = { 0 };
		df.pFiles = sizeof(DROPFILES);
		df.fWide = (sizeof(tchar_t) > 1);
		memcpy(pDrop, &df, sizeof(DROPFILES));
		memcpy((BYTE*)pDrop + sizeof(DROPFILES), (const tchar_t*)strPaths.c_str(), sizeof(tchar_t) * strPaths.length());
		GlobalUnlock(hDrop);
	}
	return hDrop;
}

HGLOBAL CreateClipboardDropEffect(DWORD dropEffect)
{
	HGLOBAL hData = GlobalAlloc(GHND, sizeof(DWORD));
	if (hData == nullptr)
		return nullptr;
	if (DWORD* p = static_cast<DWORD*>(GlobalLock(hData)))
	{
		*p = dropEffect;
		GlobalUnlock(hData);
	}
	return hData;
}

HGLOBAL CreateClipboardHTML(const String& htmlContent)
{
	if (htmlContent.empty())
		return nullptr;

	// CF_HTML format constants
	static const char header[] =
		"Version:0.9\n"
		"StartHTML:%09d\n"
		"EndHTML:%09d\n"
		"StartFragment:%09d\n"
		"EndFragment:%09d\n";
	static const char start[] = "<html><body>\n<!--StartFragment -->";
	static const char end[] = "\n<!--EndFragment -->\n</body>\n</html>\n";
	
	// Convert to UTF-8 for CF_HTML
	std::string htmlUtf8 = ucr::toUTF8(htmlContent);
	std::vector<char> htmlBuffer(htmlUtf8.begin(), htmlUtf8.end());

	// Rewrite CF_HTML header with valid offsets
	char headerBuf[256];
	int cbHeader = wsprintfA(headerBuf, header, 0, 0, 0, 0);
	int size = static_cast<int>(htmlBuffer.size());
	wsprintfA(headerBuf, header, cbHeader,
		size - 1,
		cbHeader + sizeof start - 1,
		size - sizeof end + 1);
	memcpy(htmlBuffer.data(), headerBuf, cbHeader);

	HGLOBAL hData = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, size);
	if (hData == nullptr)
		return nullptr;

	if (void* pData = GlobalLock(hData))
	{
		memcpy(pData, htmlBuffer.data(), size);
		GlobalUnlock(hData);
	}
	return hData;
}

/**
 * @brief Copies string to clipboard.
 * @param [in] text Text to copy to clipboard.
 * @param [in] currentWindowHandle Handle to current window.
 * @return `true` if text copying succeeds, `false` otherwise.
 */
template<>
bool Put<HWND>(const String& text, HWND currentWindowHandle)
{
	if (text.empty())
		return false;

	HGLOBAL hText = CreateClipboardText(text);
	if (hText == nullptr)
		return false;

	return SetClipboardDataMultiple(currentWindowHandle,
		{ { GetClipTcharTextFormat(), hText } });
}

template<>
void PutFilesInternal<HWND>(const String& strPaths, const String& strPathsSepSpc, HWND currentWindowHandle)
{
	// CF_HDROP
	HGLOBAL hDrop = CreateClipboardHDROP(strPaths);
	if (hDrop == nullptr)
		return;

	// CF_DROPEFFECT
	HGLOBAL hDropEffect = CreateClipboardDropEffect(DROPEFFECT_COPY);
	if (hDropEffect == nullptr)
	{
		GlobalFree(hDrop);
		return;
	}

	// CF_UNICODETEXT
	HGLOBAL hPathnames = CreateClipboardText(strPathsSepSpc);
	if (hPathnames == nullptr)
	{
		GlobalFree(hDrop);
		GlobalFree(hDropEffect);
		return;
	}

	UINT cfDropEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	SetClipboardDataMultiple(currentWindowHandle, {
		{ CF_HDROP, hDrop },
		{ CLIPFORMAT(cfDropEffect), hDropEffect },
		{ GetClipTcharTextFormat(), hPathnames }
	});
}

template<>
bool PutFileAndText<HWND>(const String& filename, const String& text, HWND currentWindowHandle)
{
	// CF_HDROP
	String strPaths = filename;
	strPaths += _T('\0');
	strPaths += _T('\0');

	HGLOBAL hDrop = CreateClipboardHDROP(strPaths);
	if (hDrop == nullptr)
		return false;

	// CFSTR_PREFERREDDROPEFFECT
	HGLOBAL hDropEffect = CreateClipboardDropEffect(DROPEFFECT_COPY);
	if (hDropEffect == nullptr)
	{
		GlobalFree(hDrop);
		return false;
	}

	// CF_UNICODETEXT
	HGLOBAL hText = CreateClipboardText(text);
	if (hText == nullptr)
	{
		GlobalFree(hDrop);
		GlobalFree(hDropEffect);
		return false;
	}

	UINT cfDropEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);

	return SetClipboardDataMultiple(currentWindowHandle,
	{
		{ CF_HDROP, hDrop },
		{ CLIPFORMAT(cfDropEffect), hDropEffect },
		{ GetClipTcharTextFormat(), hText }
	});
}

template<>
bool PutFileAndTextAndHTML<HWND>(const String& filename, const String& text, HWND currentWindowHandle)
{
	// CF_HDROP
	String strPaths = filename;
	strPaths += _T('\0');
	strPaths += _T('\0');

	HGLOBAL hDrop = CreateClipboardHDROP(strPaths);
	if (hDrop == nullptr)
		return false;

	// CFSTR_PREFERREDDROPEFFECT
	HGLOBAL hDropEffect = CreateClipboardDropEffect(DROPEFFECT_COPY);
	if (hDropEffect == nullptr)
	{
		GlobalFree(hDrop);
		return false;
	}

	// CF_UNICODETEXT
	HGLOBAL hText = CreateClipboardText(text);
	if (hText == nullptr)
	{
		GlobalFree(hDrop);
		GlobalFree(hDropEffect);
		return false;
	}

	// CF_HTML
	HGLOBAL hHTML = CreateClipboardHTML(text);
	if (hHTML == nullptr)
	{
		GlobalFree(hDrop);
		GlobalFree(hDropEffect);
		GlobalFree(hText);
		return false;
	}

	UINT cfDropEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	UINT CF_HTML = RegisterClipboardFormat(_T("HTML Format"));

	return SetClipboardDataMultiple(currentWindowHandle,
	{
		{ CF_HDROP, hDrop },
		{ CLIPFORMAT(cfDropEffect), hDropEffect },
		{ GetClipTcharTextFormat(), hText },
		{ CLIPFORMAT(CF_HTML), hHTML }
	});
}

}
