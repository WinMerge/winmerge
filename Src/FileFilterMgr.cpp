// SPDX-License-Identifier: GPL-2.0-or-later
/**
 *  @file FileFilterMgr.cpp
 *
 *  @brief Implementation of FileFilterMgr and supporting routines
 */ 

#include "pch.h"
#include "FileFilterMgr.h"
#include "FilterEngine/FilterExpression.h"
#include <vector>
#include <Poco/Glob.h>
#include <Poco/RegularExpression.h>
#include <Poco/Exception.h>
#include "DirTravel.h"
#include "DiffItem.h"
#include "UnicodeString.h"
#include "FileFilter.h"
#include "UniFile.h"
#include "paths.h"

using std::vector;
using Poco::Glob;
using Poco::RegularExpression;

/**
 * @brief Destructor, frees all filters.
 */
FileFilterMgr::~FileFilterMgr()
{
	DeleteAllFilters();
}

/**
 * @brief Loads filterfile from disk and adds it to filters.
 * @param [in] szFilterFile Filter file to load.
 * @return FILTER_OK if succeeded or one of FILTER_RETVALUE values on error.
 */
int FileFilterMgr::AddFilter(const String& szFilterFile)
{
	int errorcode = FILTER_OK;
	FileFilter * pFilter = LoadFilterFile(szFilterFile, errorcode);
	if (pFilter != nullptr)
		m_filters.push_back(FileFilterPtr(pFilter));
	return errorcode;
}

/**
 * @brief Load all filter files matching pattern from disk into internal filter set.
 * @param [in] dir Directory from where filters are loaded.
 * @param [in] szPattern Pattern for filters to load filters, for example "*.flt".
 * @param [in] szExt File-extension of filter files.
 */
void FileFilterMgr::LoadFromDirectory(const String& dir, const String& szPattern, const String& szExt)
{
	try
	{
		DirItemArray dirs, files;
		LoadAndSortFiles(dir, &dirs, &files, false);
		Glob glb(ucr::toUTF8(szPattern));
	
		for (DirItem& item: files)
		{
			String filename = item.filename;
			if (!glb.match(ucr::toUTF8(filename)))
				continue;
			if (!szExt.empty())
			{
				// caller specified a specific extension
				// (This is really a workaround for brokenness in windows, which
				//  doesn't screen correctly on extension in pattern)
				const String ext = filename.substr(filename.length() - szExt.length());
				if (strutils::compare_nocase(szExt, ext) != 0)
					return;
			}

			String filterpath = paths::ConcatPath(dir, filename);
			AddFilter(filterpath);
		}
	}
	catch (...)
	{
	}
}

/**
 * @brief Removes filter from filterlist.
 *
 * @param [in] szFilterFile Filename of filter to remove.
 */
void FileFilterMgr::RemoveFilter(const String& szFilterFile)
{
	// Note that m_filters.GetSize can change during loop
	vector<FileFilterPtr>::iterator iter = m_filters.begin();
	while (iter != m_filters.end())
	{
		if (strutils::compare_nocase((*iter)->fullpath, szFilterFile) == 0)
		{
			m_filters.erase(iter);
			break;
		}
		++iter;
	}
}

/**
 * @brief Removes all filters from current list.
 */
void FileFilterMgr::DeleteAllFilters()
{
	m_filters.clear();
}

static bool RemoveComment(String& str)
{
	const String& commentLeader = _T("##"); // Starts comment
	str = strutils::trim_ws_begin(str);

	// Ignore lines beginning with '##'
	size_t pos = str.find(commentLeader);
	if (pos == 0)
		return true;

	// Find possible comment-separator '<whitespace>##'
	while (pos != std::string::npos && !(str[pos - 1] == ' ' || str[pos - 1] == '\t'))
		pos = str.find(commentLeader, pos + 1);

	// Remove comment and whitespaces before it
	if (pos != std::string::npos)
		str.resize(pos);
	str = strutils::trim_ws_end(str);
	return (str.empty());
}

/**
 * @brief Add a single pattern (if nonempty & valid) to a pattern list.
 *
 * @param [in] pfilter Pointer to file filter, used for error reporting.
 * @param [in] filterList List where pattern is added.
 * @param [in] str Temporary variable (ie, it may be altered)
 * @param [in] lineNumber Line number in filter file, used for error reporting.
 */
static void AddFilterPattern(FileFilter* pfilter, vector<FileFilterElementPtr> *filterList, String & str, bool fileFilter, int lineNumber)
{
	if (RemoveComment(str))
		return;
	pfilter->AddFilterPattern(filterList, str, fileFilter, lineNumber);
}

/**
 * @brief Add a single expression (if nonempty & valid) to a expression list.
 *
 * @param [in] pfilter Pointer to file filter, used for error reporting.
 * @param [in] filterList List where expression is added.
 * @param [in] str Temporary variable (ie, it may be altered)
 * @param [in] lineNumber Line number in filter file, used for error reporting.
*/
static void AddFilterExpression(FileFilter* pfilter, vector<FilterExpressionPtr>* filterList, String& str, int lineNumber)
{
	if (RemoveComment(str))
		return;
	pfilter->AddFilterExpression(filterList, str, lineNumber);
}

/**
 * @brief Parse a filter file, and add it to array if valid.
 *
 * @param [in] szFilePath Path (w/ filename) to file to load.
 * @param [out] error Error-code if loading failed (returned `nullptr`).
 * @return Pointer to new filter, or `nullptr` if error (check error code too).
 */
FileFilter * FileFilterMgr::LoadFilterFile(const String& szFilepath, int & error)
{
	UniMemFile file;
	if (!file.OpenReadOnly(szFilepath))
	{
		error = FILTER_ERROR_FILEACCESS;
		return nullptr;
	}

	file.ReadBom(); // in case it is a Unicode file, let UniMemFile handle BOM
	if (!file.IsUnicode() && !ucr::CheckForInvalidUtf8(
		reinterpret_cast<const char*>(file.GetBase()), static_cast<size_t>(file.GetFileSize())))
		file.SetUnicoding(ucr::UTF8);

	String fileName;
	paths::SplitFilename(szFilepath, nullptr, &fileName, nullptr);
	FileFilter *pfilter = new FileFilter;
	pfilter->fullpath = szFilepath;
	pfilter->name = std::move(fileName); // Filename is the default name

	String sLine;
	bool lossy = false;
	bool bLinesLeft = true;
	int lineNumber = 0;
	do
	{
		// Returns false when last line is read
		String tmpLine;
		bLinesLeft = file.ReadString(tmpLine, &lossy);
		sLine = std::move(tmpLine);
		sLine = strutils::trim_ws(sLine);

		if (0 == sLine.compare(0, 5, _T("name:"), 5))
		{
			// specifies display name
			String str = sLine.substr(5);
			str = strutils::trim_ws_begin(str);
			if (!str.empty())
				pfilter->name = std::move(str);
		}
		else if (0 == sLine.compare(0, 5, _T("desc:"), 5))
		{
			// specifies display name
			String str = sLine.substr(5);
			str = strutils::trim_ws_begin(str);
			if (!str.empty())
				pfilter->description = std::move(str);
		}
		else if (0 == sLine.compare(0, 4, _T("def:"), 4))
		{
			// specifies default
			String str = sLine.substr(4);
			str = strutils::trim_ws_begin(str);
			if (str == _T("0") || str == _T("no") || str == _T("exclude"))
				pfilter->default_include = false;
			else if (str == _T("1") || str == _T("yes") || str == _T("include"))
				pfilter->default_include = true;
		}
		else if (0 == sLine.compare(0, 2, _T("f:"), 2))
		{
			// file filter
			String str = sLine.substr(2);
			AddFilterPattern(pfilter, &pfilter->filefilters, str, true, lineNumber);
		}
		else if (0 == sLine.compare(0, 2, _T("d:"), 2))
		{
			// directory filter
			String str = sLine.substr(2);
			AddFilterPattern(pfilter, &pfilter->dirfilters, str, false, lineNumber);
		}
		else if (0 == sLine.compare(0, 3, _T("fe:"), 3))
		{
			// file expression filter
			String str = sLine.substr(3);
			AddFilterExpression(pfilter, &pfilter->fileExpressionFilters, str, lineNumber);
		}
		else if (0 == sLine.compare(0, 3, _T("de:"), 3))
		{
			// directory expression filter
			String str = sLine.substr(3);
			AddFilterExpression(pfilter, &pfilter->dirExpressionFilters, str, lineNumber);
		}
		else if (0 == sLine.compare(0, 3, _T("f!:"), 3))
		{
			// file filter
			String str = sLine.substr(3);
			AddFilterPattern(pfilter, &pfilter->filefiltersExclude, str, true, lineNumber);
		}
		else if (0 == sLine.compare(0, 3, _T("d!:"), 3))
		{
			// directory filter
			String str = sLine.substr(3);
			AddFilterPattern(pfilter, &pfilter->dirfiltersExclude, str, false, lineNumber);
		}
		else if (0 == sLine.compare(0, 4, _T("fe!:"), 4))
		{
			// file expression filter
			String str = sLine.substr(4);
			AddFilterExpression(pfilter, &pfilter->fileExpressionFiltersExclude, str, lineNumber);
		}
		else if (0 == sLine.compare(0, 4, _T("de!:"), 4))
		{
			// directory expression filter
			String str = sLine.substr(4);
			AddFilterExpression(pfilter, &pfilter->dirExpressionFiltersExclude, str, lineNumber);
		}
		lineNumber++;
	} while (bLinesLeft);

	return pfilter;
}

/**
 * @brief Give client back a pointer to the actual filter.
 *
 * @param [in] szFilterPath Full path to filterfile.
 * @return Pointer to found filefilter or `nullptr`;
 * @note We just do a linear search, because this is seldom called
 */
FileFilter * FileFilterMgr::GetFilterByPath(const String& szFilterPath)
{
	vector<FileFilterPtr>::const_iterator iter = m_filters.begin();
	while (iter != m_filters.end())
	{
		if (strutils::compare_nocase((*iter)->fullpath, szFilterPath) == 0)
			return (*iter).get();
		++iter;
	}
	return 0;
}

/**
 * @brief Give client back a pointer to the actual filter.
 *
 * @param [in] i Index of filter.
 * @return Pointer to filefilter in given index or `nullptr`.
 */
FileFilter * FileFilterMgr::GetFilterByIndex(int i)
{
	if (i < 0 || i >= m_filters.size())
		return nullptr;

	return m_filters[i].get();
}

/**
 * @brief Reload filter from disk
 *
 * Reloads filter from disk. This is done by creating a new one
 * to substitute for old one.
 * @param [in] pFilter Pointer to filter to reload.
 * @return FILTER_OK when succeeds, one of FILTER_RETVALUE values on error.
 * @note Given filter (pfilter) is freed and must not be used anymore.
 * @todo Should return new filter.
 */
int FileFilterMgr::ReloadFilterFromDisk(FileFilter * pfilter)
{
	int errorcode = FILTER_OK;
	FileFilter * newfilter = LoadFilterFile(pfilter->fullpath, errorcode);

	if (newfilter == nullptr)
	{
		return errorcode;
	}

	vector<FileFilterPtr>::iterator iter = m_filters.begin();
	while (iter != m_filters.end())
	{
		if (pfilter == (*iter).get())
		{
			m_filters.erase(iter);
			break;
		}
	}
	m_filters.push_back(FileFilterPtr(newfilter));
	return errorcode;
}

/**
 * @brief Reload filter from disk.
 *
 * Reloads filter from disk. This is done by creating a new one
 * to substitute for old one.
 * @param [in] szFullPath Full path to filter file to reload.
 * @return FILTER_OK when succeeds or one of FILTER_RETVALUE values when fails.
 */
int FileFilterMgr::ReloadFilterFromDisk(const String& szFullPath)
{
	int errorcode = FILTER_OK;
	FileFilter * filter = GetFilterByPath(szFullPath);
	if (filter)
		errorcode = ReloadFilterFromDisk(filter);
	else
		errorcode = FILTER_NOTFOUND;
	return errorcode;
}

/**
 * @brief Clone file filter manager from another file filter Manager.
 * This function clones file filter manager from another file filter manager.
 * Current contents in the file filter manager are removed and new contents added from the given file filter manager.
 * @param [in] fileFilterManager File filter manager to clone.
 */
void FileFilterMgr::CloneFrom(const FileFilterMgr* fileFilterMgr)
{
	if (!fileFilterMgr)
		return;

	m_filters.clear();

	size_t count = fileFilterMgr->m_filters.size();
	m_filters.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		auto ptr = std::make_shared<FileFilter>(FileFilter());
		ptr->CloneFrom(fileFilterMgr->m_filters[i].get());
		m_filters.push_back(ptr);
	}
}
