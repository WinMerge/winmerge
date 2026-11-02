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

	/** @brief One entry of the recent comparison list */
	struct RecentCompare
	{
		String title;  /**< Display name, e.g. "a.txt - b.txt" */
		String params; /**< Command line parameters without the program name (paths, flags, options); identifies the entry */
	};

	// Recent comparisons (file or folder pairs/triples), newest first. They come from the Windows jump list where
	// Windows keeps its history (JumpList::IsRecentDocsTrackingEnabled), otherwise from a list WinMerge keeps in its
	// own settings. AddRecentCompare stores an entry only in the latter case.
	std::vector<RecentCompare> GetRecentCompares(unsigned nMaxItems);
	void AddRecentCompare(const RecentCompare& item, unsigned nMaxItems);
	void ClearRecentCompares();
}
