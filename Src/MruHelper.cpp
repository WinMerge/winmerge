/** 
 * @file  MruHelper.cpp
 *
 * @brief Implementation of MRU (Most Recently Used) helper functions
 */
#include "StdAfx.h"
#include "MruHelper.h"
#include "paths.h"
#include <afxwin.h>

namespace MruHelper
{
	/**
	 * @brief Convert pane index to registry subkey
	 */
	static const TCHAR* GetRegSubKeyFromPane(int pane)
	{
		switch (pane)
		{
		case 0:
			return _T("Files\\Left");
		case 1:
			return _T("Files\\Right");
		case 2:
			return _T("Files\\Option");
		default:
			return nullptr;
		}
	}

	/**
	 * @brief Utility function to update CSuperComboBox format MRU
	 */
	void addToMru(int pane, const String& sItem, unsigned nMaxItems)
	{
		const TCHAR* szRegSubKey = GetRegSubKeyFromPane(pane);
		if (!szRegSubKey)
			return;
		auto list = getMruList(pane, nMaxItems);
		list.erase(std::remove(list.begin(), list.end(), sItem), list.end());
		list.insert(list.begin(), sItem);
		if (list.size() > nMaxItems)
			list.resize(nMaxItems);
		AfxGetApp()->WriteProfileInt(szRegSubKey, _T("Count"), static_cast<unsigned>(list.size()));
		for (unsigned i = 0; i < list.size(); ++i)
			AfxGetApp()->WriteProfileString(szRegSubKey, strutils::format(_T("Item_%d"), i).c_str(), list[i].c_str());
	}

	/**
	 * @brief Utility function to retrieve CSuperComboBox format MRU
	 */
	std::vector<String> getMruList(int pane, unsigned nMaxItems)
	{
		const TCHAR* szRegSubKey = GetRegSubKeyFromPane(pane);
		if (szRegSubKey == nullptr)
			return std::vector<String>();

		std::vector<String> list;
		UINT cnt = AfxGetApp()->GetProfileInt(szRegSubKey, _T("Count"), 0);
		list.reserve(cnt);
		for (UINT i = 0; i < cnt && i < nMaxItems; ++i)
		{
			String s = AfxGetApp()->GetProfileString(szRegSubKey, strutils::format(_T("Item_%d"), i).c_str());
			if (!s.empty())
				list.push_back(s);
		}
		return list;
	}

	/**
	 * @brief Get recent files list for HeaderBar
	 */
	std::vector<RecentItem> GetRecentFiles(int pane, unsigned maxCount, RecentItemType type)
	{
		std::vector<RecentItem> items;

		// Get MRU items from the specific pane
		std::vector<String> allPaths = getMruList(pane, maxCount);

		// Filter and create items
		for (const auto& path : allPaths)
		{
			bool isFolder = paths::EndsWithSlash(path);

			// Filter based on type
			if (type == RecentItemType::FilesOnly && isFolder)
				continue;
			if (type == RecentItemType::FoldersOnly && !isFolder)
				continue;

			RecentItem item;
			item.path = path;

			// Extract filename or folder name as title
			if (isFolder)
			{
				// For folders, get the last directory name
				String pathWithoutSlash = path.substr(0, path.length() - 1);
				size_t pos = pathWithoutSlash.find_last_of(_T("\\/"));
				if (pos != String::npos)
					item.title = pathWithoutSlash.substr(pos + 1);
				else
					item.title = pathWithoutSlash;
			}
			else
			{
				// For files, get the filename
				size_t pos = path.find_last_of(_T("\\/"));
				if (pos != String::npos)
					item.title = path.substr(pos + 1);
				else
					item.title = path;
			}

			items.push_back(item);

			if (items.size() >= maxCount)
				break;
		}

		return items;
	}
}
