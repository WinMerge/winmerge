/**
 * @file  FolderStats.cpp
 * @brief Implementation file for FolderStats functions
 */
#include "pch.h"
#include "FolderStats.h"
#include <windows.h>
#include <string>
#include <cstdint>

namespace FolderStats
{
	FolderStatsResult ScanFolder(const String& path, bool recursive)
	{
		FolderStatsResult result;
		WIN32_FIND_DATAW findData;
		HANDLE hFind = FindFirstFile((path + _T("\\*")).c_str(), &findData);
		if (hFind == INVALID_HANDLE_VALUE)
			return result;
		do
		{
			const String name = findData.cFileName;
			if (name == _T(".") || name == _T(".."))
				continue;
			const bool isDir = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			if (isDir)
			{
				result.itemCount++;
				if (recursive)
				{
					FolderStatsResult sub = ScanFolder(path + _T("\\") + name, recursive);
					result.fileCount += sub.fileCount;
					result.itemCount += sub.itemCount;
					result.totalSize += sub.totalSize;
				}
			}
			else
			{
				result.fileCount++;
				result.itemCount++;
				result.totalSize += (static_cast<int64_t>(findData.nFileSizeHigh) << 32)
					+ static_cast<int64_t>(findData.nFileSizeLow);
			}
		} while (FindNextFile(hFind, &findData));
		FindClose(hFind);
		return result;
	}
}
