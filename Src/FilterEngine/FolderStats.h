/**
 * @file FolderStats.h
 * @brief Declaration file for FolderStats functions
 */
#pragma once

#include "UnicodeString.h"

namespace FolderStats
{
	struct FolderStatsResult
	{
		int fileCount = 0;
		int itemCount = 0;
		int64_t totalSize = 0;
	};
	FolderStatsResult ScanFolder(const String& path, bool recursive);
}