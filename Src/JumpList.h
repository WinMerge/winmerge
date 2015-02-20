/** 
 * @file  JumpList.h
 *
 * @brief Declaration file for JumpList helper functions.
 *
 */
#pragma once

#include "UnicodeString.h"
#include <vector>

namespace JumpList
{
	struct Item
	{
		Item(const String& path, const String& params, const String& title, const String& desc): path(path), params(params), title(title), desc(desc) {}
		String path;
		String params;
		String desc;
		String title;
	};
	bool SetCurrentProcessExplicitAppUserModelID(const std::wstring& appid);
	bool AddToRecentDocs(const String& app_path = _T(""), const String& params = _T(""), const String& title = _T(""), const String& desc = _T(""), int icon_index = 0);
	std::vector<Item> GetRecentDocs(size_t nMaxItems);
}
