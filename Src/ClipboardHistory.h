/** 
 * @file  ClipboardHistory.h
 *
 * @brief Declaration file for Clipboard history functions
 */
#pragma once

#include "UnicodeString.h"
#include "TempFile.h"
#include <memory>
#include <vector>
#include <ctime>

namespace ClipboardHistory
{
	struct Item
	{
		time_t timestamp = 0;
		std::shared_ptr<TempFile> pTextTempFile;
		std::shared_ptr<TempFile> pBitmapTempFile;
		String previewText;  // Preview text (first line) for menu display
		String description;  // Display caption like "Clipboard at 12:34:56"
	};

	std::vector<Item> GetItems(unsigned maxCount, unsigned ensureMinCount = 0);
}
