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

	// Helper functions for header bar
	void addToMru(const tchar_t* szItem, const tchar_t* szRegSubKey, UINT nMaxItems = 20);
	String FormatClipboardDescription(time_t timestamp);
	std::vector<RecentItem> GetRecentFiles(int pane, unsigned maxCount, RecentItemType type);
	std::vector<ClipboardItem> GetClipboardHistoryItems(unsigned maxCount);
}
