/** 
 * @file  HistoryItemsHelper.h
 *
 * @brief Helper functions for IHeaderBar recent items and clipboard history
 */
#pragma once

#include "UnicodeString.h"
#include "TempFile.h"
#include <ctime>
#include <memory>
#include <vector>

namespace HistoryItemsHelper
{
	struct RecentItem
	{
		String title;
		String path;
		String description;
	};
	struct ClipboardItem
	{
		String previewText;
		String description;  // Display caption like "Clipboard at 12:34:56"
		time_t timestamp;
		std::shared_ptr<TempFile> pTextTempFile;
		std::shared_ptr<TempFile> pBitmapTempFile;
	};
	enum class RecentItemType { All, FilesOnly, FoldersOnly };

	// Helper functions for history items
	void addToMru(int pane, const String& sItem, unsigned nMaxItems = 20);
	std::vector<String> getMruList(int pane, unsigned nMaxItems);
	std::vector<RecentItem> GetRecentFiles(int pane, unsigned maxCount, RecentItemType type);
	String FormatClipboardDescription(time_t timestamp);
	std::vector<ClipboardItem> GetClipboardHistoryItems(unsigned maxCount);
}
