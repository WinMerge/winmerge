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
		Item(const String& path, const String& params, const String& title, const String& desc, const String& icon_path, int icon_index): path(path), params(params), title(title), desc(desc), icon_path(icon_path), icon_index(icon_index) {}
		String path;
		String params;
		String desc;
		String title;
		String icon_path;
		int icon_index;
	};
	bool SetCurrentProcessExplicitAppUserModelID(const std::wstring& appid);
	bool AddToRecentDocs(const String& app_path = _T(""), const String& params = _T(""), const String& title = _T(""), const String& desc = _T(""), const String& icon_path = _T(""), int icon_index = 0);
	std::vector<Item> GetRecentDocs(size_t nMaxItems);
	bool RemoveRecentDocs();
	bool AddUserTasks(const std::vector<Item>& tasks);
}
