/** 
 * @file  MruHelper.h
 *
 * @brief Helper functions for MRU (Most Recently Used) items in IHeaderBar
 */
#pragma once

#include "UnicodeString.h"
#include <vector>

namespace MruHelper
{
	struct RecentItem
	{
		String title;       // Display name (filename or folder name)
		String path;        // Full path
	};
	enum class RecentItemType { All, FilesOnly, FoldersOnly };

	// Helper functions for history items
	void addToMru(int pane, const String& sItem, unsigned nMaxItems = 20);
	std::vector<String> getMruList(int pane, unsigned nMaxItems);
	std::vector<RecentItem> GetRecentFiles(int pane, unsigned maxCount, RecentItemType type);
}
