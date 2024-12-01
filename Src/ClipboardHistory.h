/** 
 * @file  ClipboardHistory.h
 *
 * @brief Declaration file for Clipboard history functions
 */
#pragma once

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
	};
	std::vector<Item> GetItems(unsigned num);
}
