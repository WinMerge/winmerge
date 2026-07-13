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

struct ClipboardItem
{
	CLIPFORMAT format = 0;
	HGLOBAL hData = nullptr;

	ClipboardItem() = default;

	ClipboardItem(CLIPFORMAT fmt, HGLOBAL h)
		: format(fmt), hData(h)
	{}

	ClipboardItem(const ClipboardItem&) = delete;
	ClipboardItem& operator=(const ClipboardItem&) = delete;

	ClipboardItem(ClipboardItem&& other) noexcept
	{
		format = other.format;
		hData = other.hData;
		other.hData = nullptr;
	}
	ClipboardItem& operator=(ClipboardItem&&) noexcept = default;

	~ClipboardItem()
	{
		if (hData)
			GlobalFree(hData);
	}

	HGLOBAL Detach()
	{
		HGLOBAL h = hData;
		hData = nullptr;
		return h;
	}
};

bool SetClipboardItemMultiple(HWND hwnd, std::vector<ClipboardItem>&& list)
{
	if (!OpenClipboard(hwnd))
		return false;

	EmptyClipboard();

	for (auto& item : list)
	{
		HGLOBAL hData = item.Detach();
		if (::SetClipboardData(item.format, hData) == nullptr)
		{
			GlobalFree(hData);
			CloseClipboard();
			return false;
		}
	}

	CloseClipboard();
	return true;
}

ClipboardItem CreateClipboardText(const String& text)
{
	if (text.empty())
		return {};
	const size_t dataSize = text.length() + 1;
	HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, dataSize * sizeof(tchar_t));
	if (hData == nullptr)
		return {};
	tchar_t* pszData = static_cast<tchar_t*>(GlobalLock(hData));
	if (pszData == nullptr)
	{
		GlobalFree(hData);
		return {};
	}
	tc::tcslcpy(pszData, dataSize, text.c_str());
	GlobalUnlock(hData);
	return { GetClipTcharTextFormat(), hData };
}

ClipboardItem CreateClipboardHDROP(const String& strPaths)
{
	HGLOBAL hDrop = GlobalAlloc(GHND, sizeof(DROPFILES) + sizeof(tchar_t) * strPaths.length());
	if (hDrop == nullptr)
		return {};
	tchar_t* pDrop = static_cast<tchar_t*>(GlobalLock(hDrop));
	if (pDrop == nullptr)
	{
		GlobalFree(hDrop);
		return {};
	}
	DROPFILES df = { 0 };
	df.pFiles = sizeof(DROPFILES);
	df.fWide = (sizeof(tchar_t) > 1);
	memcpy(pDrop, &df, sizeof(DROPFILES));
	memcpy((BYTE*)pDrop + sizeof(DROPFILES), (const tchar_t*)strPaths.c_str(), sizeof(tchar_t) * strPaths.length());
	GlobalUnlock(hDrop);
	return { CF_HDROP, hDrop };
}

ClipboardItem CreateClipboardDropEffect(DWORD dropEffect)
{
	HGLOBAL hData = GlobalAlloc(GHND, sizeof(DWORD));
	if (hData == nullptr)
		return {};
	DWORD* p = static_cast<DWORD*>(GlobalLock(hData));
	if (p == nullptr)
	{
		GlobalFree(hData);
		return {};
	}
	*p = dropEffect;
	GlobalUnlock(hData);
	UINT cfDropEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
	return { CLIPFORMAT(cfDropEffect), hData };
}

ClipboardItem CreateClipboardHTML(const String& htmlContent)
{
	if (htmlContent.empty())
		return {};

	// CF_HTML format constants
	static constexpr char header[] =
		"Version:0.9\r\n"
		"StartHTML:%09d\r\n"
		"EndHTML:%09d\r\n"
		"StartFragment:%09d\r\n"
		"EndFragment:%09d\r\n";

	static constexpr char startFragment[] = "<!--StartFragment -->\r\n";
	static constexpr char endFragment[] = "\r\n<!--EndFragment -->";

	// Convert to UTF-8.
	const std::string htmlUtf8 = ucr::toUTF8(htmlContent);

	// Build CF_HTML header.
	char headerBuf[256];
	const int cbHeader = wsprintfA(headerBuf, header, 0, 0, 0, 0);

	const int startHTML = cbHeader;
	const int endHTML = startHTML + static_cast<int>(sizeof(startFragment) - 1 + htmlUtf8.size() + sizeof(endFragment) - 1);

	const int startFragmentOffset = startHTML + static_cast<int>(sizeof(startFragment) - 1);
	const int endFragmentOffset = startFragmentOffset + static_cast<int>(htmlUtf8.size());

	wsprintfA(headerBuf, header, startHTML, endHTML, startFragmentOffset, endFragmentOffset);

	const SIZE_T totalSize = cbHeader + (sizeof(startFragment) - 1) + htmlUtf8.size() + (sizeof(endFragment) - 1);

	HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, totalSize);
	if (hData == nullptr)
		return {};

	char* p = static_cast<char*>(GlobalLock(hData));
	if (p == nullptr)
	{
		GlobalFree(hData);
		return {};
	}

	memcpy(p, headerBuf, cbHeader);
	p += cbHeader;

	memcpy(p, startFragment, sizeof(startFragment) - 1);
	p += sizeof(startFragment) - 1;

	memcpy(p, htmlUtf8.data(), htmlUtf8.size());
	p += htmlUtf8.size();

	memcpy(p, endFragment, sizeof(endFragment) - 1);

	GlobalUnlock(hData);

	UINT CF_HTML = RegisterClipboardFormat(_T("HTML Format"));

	return { CLIPFORMAT(CF_HTML), hData };
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

	ClipboardItem item = CreateClipboardText(text);
	if (item.hData == nullptr)
		return false;

	std::vector<ClipboardItem> items;
	items.reserve(1);
	items.emplace_back(std::move(item));
	
	return SetClipboardItemMultiple(currentWindowHandle, std::move(items));
}

template<>
void PutFilesInternal<HWND>(const String& strPaths, const String& strPathsSepSpc, HWND currentWindowHandle)
{
	// CF_HDROP
	ClipboardItem itemDrop = CreateClipboardHDROP(strPaths);
	if (itemDrop.hData == nullptr)
		return;

	// CF_DROPEFFECT
	ClipboardItem  itemDropEffect = CreateClipboardDropEffect(DROPEFFECT_COPY);
	if (itemDropEffect.hData == nullptr)
		return;

	// CF_UNICODETEXT
	ClipboardItem itemPathnames = CreateClipboardText(strPathsSepSpc);
	if (itemPathnames.hData == nullptr)
		return;

	std::vector<ClipboardItem> items;
	items.reserve(3);
	items.emplace_back(std::move(itemDrop));
	items.emplace_back(std::move(itemDropEffect));
	items.emplace_back(std::move(itemPathnames));

	SetClipboardItemMultiple(currentWindowHandle, std::move(items));
}

template<>
bool PutFileAndText<HWND>(const String& filename, const String& text, HWND currentWindowHandle)
{
	// CF_HDROP
	String strPaths = filename;
	strPaths += _T('\0');
	strPaths += _T('\0');

	ClipboardItem itemDrop = CreateClipboardHDROP(strPaths);
	if (itemDrop.hData == nullptr)
		return false;

	// CFSTR_PREFERREDDROPEFFECT
	ClipboardItem itemDropEffect = CreateClipboardDropEffect(DROPEFFECT_COPY);
	if (itemDropEffect.hData == nullptr)
		return false;

	// CF_UNICODETEXT
	ClipboardItem itemText = CreateClipboardText(text);
	if (itemText.hData == nullptr)
		return false;

	std::vector<ClipboardItem> items;
	items.reserve(3);
	items.emplace_back(std::move(itemDrop));
	items.emplace_back(std::move(itemDropEffect));
	items.emplace_back(std::move(itemText));
	return SetClipboardItemMultiple(currentWindowHandle, std::move(items));
}

template<>
bool PutFileAndTextAndHTML<HWND>(const String& filename, const String& text, HWND currentWindowHandle)
{
	// CF_HDROP
	String strPaths = filename;
	strPaths += _T('\0');
	strPaths += _T('\0');

	ClipboardItem itemDrop = CreateClipboardHDROP(strPaths);
	if (itemDrop.hData == nullptr)
		return false;

	// CFSTR_PREFERREDDROPEFFECT
	ClipboardItem itemDropEffect = CreateClipboardDropEffect(DROPEFFECT_COPY);
	if (itemDropEffect.hData == nullptr)
		return false;

	// CF_UNICODETEXT
	ClipboardItem itemText = CreateClipboardText(text);
	if (itemText.hData == nullptr)
		return false;

	// CF_HTML
	ClipboardItem itemHTML = CreateClipboardHTML(text);
	if (itemHTML.hData == nullptr)
		return false;

	std::vector<ClipboardItem> items;
	items.reserve(4);
	items.emplace_back(std::move(itemDrop));
	items.emplace_back(std::move(itemDropEffect));
	items.emplace_back(std::move(itemText));
	items.emplace_back(std::move(itemHTML));
	return SetClipboardItemMultiple(currentWindowHandle, std::move(items));
}

}
