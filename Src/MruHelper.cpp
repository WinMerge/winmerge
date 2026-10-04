/** 
 * @file  MruHelper.cpp
 *
 * @brief Implementation of MRU (Most Recently Used) helper functions
 */
#include "StdAfx.h"
#include "MruHelper.h"
#include "paths.h"
#include "JumpList.h"
#include "MergeApp.h"
#include "OptionsMgr.h"
#include <afxwin.h>

namespace MruHelper
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
		if (!szRegSubKey)
			return;
		AfxGetApp()->WriteProfileInt(szRegSubKey, _T("Empty"), sItem.empty() ? 1 : 0);
		if (sItem.empty())
			return;
		auto list = getMruList(pane, nMaxItems);
		list.erase(std::remove(list.begin(), list.end(), sItem), list.end());
		list.insert(list.begin(), sItem);
		if (list.size() > nMaxItems)
			list.resize(nMaxItems);
		AfxGetApp()->WriteProfileInt(szRegSubKey, _T("Count"), static_cast<unsigned>(list.size()));
		for (unsigned i = 0; i < list.size(); ++i)
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
	std::vector<String> GetRecentFiles(int pane, unsigned maxCount, RecentItemType type)
	{
		std::vector<String> items;

		// Get MRU items from the specific pane
		std::vector<String> allPaths = getMruList(pane, maxCount);

		// Filter paths based on type
		for (const auto& path : allPaths)
		{
			bool isFolder = paths::EndsWithSlash(path);

			// Filter based on type
			if (type == RecentItemType::FilesOnly && isFolder)
				continue;
			if (type == RecentItemType::FoldersOnly && !isFolder)
				continue;

			items.push_back(path);

			if (items.size() >= maxCount)
				break;
		}

		return items;
	}

	/**
	 * WinMerge's own recent comparison list: one stored value per comparison, named after a hash of its command
	 * line parameters and holding "<last opened time>\t<title>\t<params>". Adding a comparison writes only its own
	 * value, and the list is always read from the storage, so WinMerge processes running at the same time keep each
	 * other's entries. Only the oldest entries beyond the maximum are removed.
	 */
	static const TCHAR RecentCompareSection[] = _T("Recent Compare List");

	struct StoredRecentCompare
	{
		long long time;    /**< Last opened, FILETIME in 100 ns units */
		String valueName;  /**< Name of the stored value */
		RecentCompare item;
	};

	/**
	 * @brief Name of the stored value of a comparison: FNV-1a hash of its parameters, so the same comparison keeps one value
	 */
	static String GetRecentCompareValueName(const String& params)
	{
		unsigned long long hash = 14695981039346656037ULL;
		for (tchar_t ch : params)
		{
			hash ^= static_cast<unsigned long long>(ch);
			hash *= 1099511628211ULL;
		}
		return strutils::format(_T("C%016llx"), hash);
	}

	/**
	 * @brief Read WinMerge's own recent comparison list from the storage, newest first
	 */
	static std::vector<StoredRecentCompare> LoadRecentCompares()
	{
		std::vector<StoredRecentCompare> list;
		for (const auto& [name, value] : GetOptionsMgr()->ReadStoredSection(RecentCompareSection))
		{
			const size_t nTab1 = value.find('\t');
			const size_t nTab2 = (nTab1 == String::npos) ? String::npos : value.find('\t', nTab1 + 1);
			if (nTab2 == String::npos || nTab2 + 1 >= value.length())
				continue;
			list.push_back({ tc::tcstoll(value.c_str(), nullptr, 10), name,
				{ value.substr(nTab1 + 1, nTab2 - nTab1 - 1), value.substr(nTab2 + 1) } });
		}
		std::sort(list.begin(), list.end(), [](const StoredRecentCompare& a, const StoredRecentCompare& b)
			{ return a.time != b.time ? a.time > b.time : a.valueName < b.valueName; });
		return list;
	}

	/**
	 * @brief Get the recent comparisons, newest first: the jump list where Windows keeps its history, otherwise WinMerge's own list
	 */
	std::vector<RecentCompare> GetRecentCompares(unsigned nMaxItems)
	{
		std::vector<RecentCompare> list;
		if (!JumpList::IsRecentDocsTrackingEnabled())
		{
			for (const auto& entry : LoadRecentCompares())
			{
				if (list.size() >= nMaxItems)
					break;
				list.push_back(entry.item);
			}
			return list;
		}
		for (const auto& doc : JumpList::GetRecentDocs(nMaxItems))
			list.push_back({ doc.title, doc.params });
		return list;
	}

	/**
	 * @brief Add a comparison to WinMerge's own recent comparison list, or move it to the top when it is already there,
	 * then remove the entries beyond nMaxItems (the oldest ones).
	 * Nothing is stored where Windows keeps the jump list history, which already has the comparison.
	 */
	void AddRecentCompare(const RecentCompare& item, unsigned nMaxItems)
	{
		if (item.params.empty() || JumpList::IsRecentDocsTrackingEnabled())
			return;
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);
		const long long time = (static_cast<long long>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
		String title = item.title;
		std::replace(title.begin(), title.end(), _T('\t'), _T(' '));
		AfxGetApp()->WriteProfileString(RecentCompareSection, GetRecentCompareValueName(item.params).c_str(),
			strutils::format(_T("%lld\t%s\t%s"), time, title, item.params).c_str());
		// ReadStoredSection() waits for the write above, so the new entry is in the list
		const auto list = LoadRecentCompares();
		for (size_t i = nMaxItems; i < list.size(); ++i)
			GetOptionsMgr()->RemoveOption(strutils::format(_T("%s/%s"), RecentCompareSection, list[i].valueName));
	}

	/**
	 * @brief Remove all recent comparisons
	 */
	void ClearRecentCompares()
	{
		AfxGetApp()->WriteProfileString(RecentCompareSection, nullptr, nullptr);
	}
}
