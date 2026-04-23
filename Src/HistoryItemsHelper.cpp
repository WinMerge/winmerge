/** 
 * @file  HistoryItemsHelper.cpp
 *
 * @brief Implementation of helper functions for recent items and clipboard history
 */
#include "StdAfx.h"
#include "HistoryItemsHelper.h"
#include "ClipboardHistory.h"
#include "locality.h"
#include "unicoder.h"
#include "UniFile.h"
#include "paths.h"
#include <afxwin.h>

namespace HistoryItemsHelper
{
	/**
	 * @brief Convert pane index to registry subkey
	 */
	static const TCHAR* GetRegSubKeyFromPane(int pane)
	{
		switch (pane)
		{
		case 0:
			return _T("Files\\Left");
		case 1:
			return _T("Files\\Right");
		case 2:
			return _T("Files\\Option");
		default:
			return nullptr;
		}
	}

	/**
	 * @brief Utility function to update CSuperComboBox format MRU
	 */
	void addToMru(int pane, const String& sItem, unsigned nMaxItems)
	{
		const TCHAR* szRegSubKey = GetRegSubKeyFromPane(pane);
		if (szRegSubKey == nullptr)
			return;

		std::vector<String> list;
		unsigned cnt = AfxGetApp()->GetProfileInt(szRegSubKey, _T("Count"), 0);
		list.reserve(cnt);
		for (unsigned i = 0; i < cnt; ++i)
		{
			const String s = AfxGetApp()->GetProfileString(szRegSubKey, strutils::format(_T("Item_%d"), i).c_str());
			if (s != sItem)
				list.push_back(s);
		}
		list.insert(list.begin(), sItem);
		cnt = static_cast<unsigned>(list.size());
		if (cnt > nMaxItems)
			cnt = nMaxItems;
		AfxGetApp()->WriteProfileInt(szRegSubKey, _T("Count"), cnt);
		for (unsigned i = 0; i < cnt; ++i)
			AfxGetApp()->WriteProfileString(szRegSubKey, strutils::format(_T("Item_%d"), i).c_str(), list[i].c_str());
	}

	/**
	 * @brief Utility function to retrieve CSuperComboBox format MRU
	 */
	std::vector<String> getMruList(int pane, unsigned nMaxItems)
	{
		const TCHAR* szRegSubKey = GetRegSubKeyFromPane(pane);
		if (szRegSubKey == nullptr)
			return std::vector<String>();

		std::vector<String> list;
		UINT cnt = AfxGetApp()->GetProfileInt(szRegSubKey, _T("Count"), 0);
		list.reserve(cnt);
		for (UINT i = 0; i < cnt && i < nMaxItems; ++i)
		{
			String s = AfxGetApp()->GetProfileString(szRegSubKey, strutils::format(_T("Item_%d"), i).c_str());
			if (!s.empty())
				list.push_back(s);
		}
		return list;
	}

	/**
	 * @brief Get recent files list for HeaderBar
	 */
	std::vector<RecentItem> GetRecentFiles(int pane, unsigned maxCount, RecentItemType type)
	{
		std::vector<RecentItem> items;

		// Get MRU items from the specific pane
		std::vector<String> allPaths = getMruList(pane, maxCount);

		// Filter and create items
		for (const auto& path : allPaths)
		{
			bool isFolder = paths::EndsWithSlash(path);

			// Filter based on type
			if (type == RecentItemType::FilesOnly && isFolder)
				continue;
			if (type == RecentItemType::FoldersOnly && !isFolder)
				continue;

			RecentItem item;
			item.path = path;

			// Extract filename or folder name as title
			if (isFolder)
			{
				// For folders, get the last directory name
				String pathWithoutSlash = path.substr(0, path.length() - 1);
				size_t pos = pathWithoutSlash.find_last_of(_T("\\/"));
				if (pos != String::npos)
					item.title = pathWithoutSlash.substr(pos + 1);
				else
					item.title = pathWithoutSlash;
			}
			else
			{
				// For files, get the filename
				size_t pos = path.find_last_of(_T("\\/"));
				if (pos != String::npos)
					item.title = path.substr(pos + 1);
				else
					item.title = path;
			}

			item.description = path;
			items.push_back(item);

			if (items.size() >= maxCount)
				break;
		}

		return items;
	}

	/**
	 * @brief Format clipboard description with timestamp
	 */
	String FormatClipboardDescription(time_t timestamp)
	{
		int64_t t = timestamp;
		String timestr = t == 0 ? _T("---") : locality::TimeString(&t);
		return strutils::format(_("Clipboard at %s"), timestr);
	}

	/**
	 * @brief Get clipboard history for HeaderBar
	 */
	std::vector<ClipboardItem> GetClipboardHistoryItems(unsigned maxCount)
	{
		std::vector<ClipboardItem> items;

		auto clipItems = ClipboardHistory::GetItems(maxCount, 1);
		for (const auto& clipItem : clipItems)
		{
			ClipboardItem item;
			item.timestamp = clipItem.timestamp;
			item.pTextTempFile = clipItem.pTextTempFile;
			item.pBitmapTempFile = clipItem.pBitmapTempFile;

			// Create description like "Clipboard at 2026-01-23 12:34:56"
			item.description = FormatClipboardDescription(clipItem.timestamp);

			if (clipItem.pTextTempFile)
			{
				UniMemFile file;
				if (file.OpenReadOnly(clipItem.pTextTempFile->GetPath()))
				{
					file.SetUnicoding(ucr::UTF8);
					String line;
					String eol;
					// Read first line as preview
					file.ReadString(line, eol, nullptr);
					// Take first 260 characters as preview
					strutils::replace_chars(line, _T("\t"), _T(" "));
					if (line.length() > MAX_PATH)
						item.previewText = line.substr(0, MAX_PATH) + _T("...");
					else
						item.previewText = line;
					file.Close();
				}
			}

			items.push_back(item);
		}
		return items;
	}
}
