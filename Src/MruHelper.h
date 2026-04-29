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
	enum class RecentItemType { All, FilesOnly, FoldersOnly };

	// Helper functions for history items
	void addToMru(int pane, const String& sItem, unsigned nMaxItems = 20);
	std::vector<String> getMruList(int pane, unsigned nMaxItems);
	std::vector<String> GetRecentFiles(int pane, unsigned maxCount, RecentItemType type);
}
