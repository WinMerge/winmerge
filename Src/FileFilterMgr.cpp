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
#include "DirTravel.h"
#include "DiffItem.h"
#include "UnicodeString.h"
#include "FileFilter.h"
#include "UniFile.h"
#include "paths.h"

using std::vector;
using Poco::Glob;
using Poco::RegularExpression;

static void AddFilterPattern(vector<FileFilterElementPtr> *filterList, String & str, bool fileFilter);
static void AddFilterExpression(vector<FilterExpressionPtr>* filterList, String& str);

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
		str = str.substr(0, pos);
	str = strutils::trim_ws_end(str);
	return (str.empty());
}

/**
 * @brief Add a single pattern (if nonempty & valid) to a pattern list.
 *
 * @param [in] filterList List where pattern is added.
 * @param [in] str Temporary variable (ie, it may be altered)
 */
static void AddFilterPattern(vector<FileFilterElementPtr> *filterList, String & str, bool fileFilter)
{
	if (RemoveComment(str))
		return;

	int re_opts = RegularExpression::RE_CASELESS;
	std::string regexString = ucr::toUTF8(str);
	re_opts |= RegularExpression::RE_UTF8;
	try
	{
		filterList->push_back(FileFilterElementPtr(new FileFilterElement(regexString, re_opts, fileFilter)));
	}
	catch (...)
	{
		// TODO:
	}
}

/**
 * @brief Add a single expression (if nonempty & valid) to a expression list.
 *
 * @param [in] filterList List where expression is added.
 * @param [in] str Temporary variable (ie, it may be altered)
*/
static void AddFilterExpression(vector<FilterExpressionPtr>* filterList, String& str)
{
	if (RemoveComment(str))
		return;
	str = strutils::trim_ws(str);
	filterList->emplace_back(new FilterExpression(ucr::toUTF8(str)));
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
			AddFilterPattern(&pfilter->filefilters, str, true);
		}
		else if (0 == sLine.compare(0, 2, _T("d:"), 2))
		{
			// directory filter
			String str = sLine.substr(2);
			AddFilterPattern(&pfilter->dirfilters, str, false);
		}
		else if (0 == sLine.compare(0, 3, _T("fe:"), 3))
		{
			// file expression filter
			String str = sLine.substr(3);
			AddFilterExpression(&pfilter->fileExpressionFilters, str);
		}
		else if (0 == sLine.compare(0, 3, _T("de:"), 3))
		{
			// directory expression filter
			String str = sLine.substr(3);
			AddFilterExpression(&pfilter->dirExpressionFilters, str);
		}
		else if (0 == sLine.compare(0, 3, _T("f!:"), 3))
		{
			// file filter
			String str = sLine.substr(3);
			AddFilterPattern(&pfilter->filefiltersExclude, str, true);
		}
		else if (0 == sLine.compare(0, 3, _T("d!:"), 3))
		{
			// directory filter
			String str = sLine.substr(3);
			AddFilterPattern(&pfilter->dirfiltersExclude, str, false);
		}
		else if (0 == sLine.compare(0, 4, _T("fe!:"), 4))
		{
			// file expression filter
			String str = sLine.substr(4);
			AddFilterExpression(&pfilter->fileExpressionFiltersExclude, str);
		}
		else if (0 == sLine.compare(0, 4, _T("de!:"), 4))
		{
			// directory expression filter
			String str = sLine.substr(4);
			AddFilterExpression(&pfilter->dirExpressionFiltersExclude, str);
		}
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
 * @brief Test given string against given regexp list.
 *
 * @param [in] filterList List of regexps to test against.
 * @param [in] szTest String to test against regexps.
 * @return true if string passes
 * @note Matching stops when first match is found.
 */
bool TestAgainstRegList(const vector<FileFilterElementPtr> *filterList, const String& szTest)
{
	if (filterList->size() == 0)
		return false;

	std::string compString, compStringFileName;
	ucr::toUTF8(szTest, compString);
	vector<FileFilterElementPtr>::const_iterator iter = filterList->begin();
	while (iter != filterList->end())
	{
		RegularExpression::Match match;
		try
		{
			if ((*iter)->_fileNameOnly && compStringFileName.empty())
				ucr::toUTF8(paths::FindFileName(szTest), compStringFileName);
			if ((*iter)->regexp.match((*iter)->_fileNameOnly ? compStringFileName : compString, 0, match) > 0)
				return true;
		}
		catch (...)
		{
			// TODO:
		}
		
		++iter;
	}
	return false;
}

/**
 * @brief Test given DIFFITEM against given regexp list.
 * @param [in] filterList List of regexps to test against.
 * @param [in] di DIFFITEM to test against regexps.
 * @return true if DIFFITEM passes
 * @note Matching stops when first match is found.
 */
bool TestAgainstRegList(const vector<FileFilterElementPtr> *filterList, const DIFFITEM& di)
{
	if (filterList->size() == 0)
		return false;

	const int nDirs = di.diffcode.isThreeway() ? 3 : 2;
	for (int i = 0; i < nDirs; ++i)
	{
		const String& szTest = paths::ConcatPath(di.diffFileInfo[i].path, di.diffFileInfo[i].filename);
		std::string compString, compStringFileName;
		ucr::toUTF8(szTest, compString);
		vector<FileFilterElementPtr>::const_iterator iter = filterList->begin();
		while (iter != filterList->end())
		{
			RegularExpression::Match match;
			try
			{
				if ((*iter)->_fileNameOnly && compStringFileName.empty())
					ucr::toUTF8(paths::FindFileName(szTest), compStringFileName);
				if ((*iter)->regexp.match((*iter)->_fileNameOnly ? compStringFileName : compString, 0, match) > 0)
					return true;
			}
			catch (...)
			{
				// TODO:
			}

			++iter;
		}
	}
	return false;
}

/**
 * @brief Test given DIFFITEM against given expression list.
 * @param [in] filterList List of expressions to test against.
 * @param [in] di DIFFITEM to test against regexps.
 * @return true if DIFFITEM passes
 * @note Matching stops when first match is found.
 */
bool TestAgainstExpressionList(const vector<FilterExpressionPtr>* filterList, const DIFFITEM& di)
{
	if (filterList->size() == 0)
		return false;

	for (const auto& filter : *filterList)
	{
		if (filter->errorCode == 0 && filter->Evaluate(di))
			return true;
	}

	return false;
}

/**
 * @brief Test given filename against filefilter.
 *
 * Test filename against active filefilter. If matching rule is found
 * we must first determine type of rule that matched. If we return false
 * from this function directory scan marks file as skipped.
 *
 * @param [in] pFilter Pointer to filefilter
 * @param [in] szFileName Filename to test
 * @return true if file passes the filter
 */
bool FileFilterMgr::TestFileNameAgainstFilter(const FileFilter * pFilter,
	const String& szFileName) const
{
	if (pFilter == nullptr)
		return true;
	if (TestAgainstRegList(&pFilter->filefilters, szFileName))
	{
		if (pFilter->filefiltersExclude.empty() || !TestAgainstRegList(&pFilter->filefiltersExclude, szFileName))
			return !pFilter->default_include;
	}
	return pFilter->default_include;
}

/**
 * @brief Set diff context for all filters in the given file filter.
 * @param [in] pFilter Pointer to file filter.
 * @param [in] pDiffContext Pointer to diff context to set for all filters.
 */
void FileFilterMgr::SetDiffContext(FileFilter * pFilter, const CDiffContext* pDiffContext)
{
	for (auto& filters :
		{ pFilter->fileExpressionFilters, pFilter->fileExpressionFiltersExclude, pFilter->dirExpressionFilters, pFilter->dirExpressionFiltersExclude })
	{
		for (const auto& filter : filters)
			filter->SetDiffContext(pDiffContext);
	}
}

/**
 * @brief Test given DIFFITEM against filefilter.
 * @param [in] pFilter Pointer to filefilter
 * @param [in] di DIFFITEM to test
 * @return true if DIFFITEM passes the filter
 */
bool FileFilterMgr::TestFileDiffItemAgainstFilter(const FileFilter* pFilter, const DIFFITEM& di) const
{
	if (pFilter == nullptr)
		return true;
	bool matched = TestAgainstRegList(&pFilter->filefilters, di);
	if (!matched && TestAgainstExpressionList(&pFilter->fileExpressionFilters, di))
		matched = true;
	if (matched)
	{
		matched = !TestAgainstRegList(&pFilter->filefiltersExclude, di);
		if (!matched && !TestAgainstExpressionList(&pFilter->fileExpressionFiltersExclude, di))
			matched = true;
	}
	if (matched)
		return !pFilter->default_include;
	return pFilter->default_include;
}

/**
 * @brief Test given directory name against filefilter.
 *
 * Test directory name against active filefilter. If matching rule is found
 * we must first determine type of rule that matched. If we return false
 * from this function directory scan marks file as skipped.
 *
 * @param [in] pFilter Pointer to filefilter
 * @param [in] szDirName Directory name to test
 * @return true if directory name passes the filter
 */
bool FileFilterMgr::TestDirNameAgainstFilter(const FileFilter * pFilter,
	const String& szDirName) const
{
	if (pFilter == nullptr)
		return true;
	if (TestAgainstRegList(&pFilter->dirfilters, szDirName))
	{
		if (pFilter->dirfiltersExclude.empty() || !TestAgainstRegList(&pFilter->dirfiltersExclude, szDirName))
			return !pFilter->default_include;
	}
	return pFilter->default_include;
}

/**
 * @brief Test given DIFFITEM against filefilter.
 * @param [in] pFilter Pointer to filefilter
 * @param [in] di DIFFITEM to test
 * @return true if DIFFITEM passes the filter
 */
bool FileFilterMgr::TestDirDiffItemAgainstFilter(const FileFilter* pFilter, const DIFFITEM& di) const
{
	if (pFilter == nullptr)
		return true;
	bool matched = TestAgainstRegList(&pFilter->dirfilters, di);
	if (!matched && TestAgainstExpressionList(&pFilter->dirExpressionFilters, di))
		matched = true;
	if (matched)
	{
		matched = !TestAgainstRegList(&pFilter->dirfiltersExclude, di);
		if (!matched && !TestAgainstExpressionList(&pFilter->dirExpressionFiltersExclude, di))
			matched = true;
	}
	if (matched)
		return !pFilter->default_include;
	return pFilter->default_include;
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
