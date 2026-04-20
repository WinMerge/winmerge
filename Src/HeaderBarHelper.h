/** 
 * @file  HeaderBarHelper.h
 *
 * @brief Helper functions for IHeaderBar recent items and clipboard history
 */
#pragma once

#include "EditorFilepathBar.h"
#include <vector>

// Helper functions for header bar
std::vector<IHeaderBar::RecentItem> GetRecentFiles(unsigned maxCount, IHeaderBar::RecentItemType type);
std::vector<IHeaderBar::ClipboardItem> GetClipboardHistoryItems(unsigned maxCount);
