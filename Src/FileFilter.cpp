// SPDX-License-Identifier: GPL-2.0-or-later
/**
 *  @file FileFilter.cpp
 *
 *  @brief Implementation of FileFilter.
 */ 

#include "pch.h"
#include "FileFilter.h"
#include "FilterEngine/FilterExpression.h"
#include "DiffItem.h"
#include "unicoder.h"
#include "paths.h"
#include <vector>
#include <Poco/RegularExpression.h>
#include <Poco/Exception.h>

using std::vector;

/**
 * @brief Destructor, frees created filter lists.
 */
FileFilter::~FileFilter()
{
	EmptyFilterList(&filefilters);
	EmptyFilterList(&filefiltersExclude);
	EmptyFilterList(&dirfilters);
	EmptyFilterList(&dirfiltersExclude);
	EmptyExpressionList(&fileExpressionFilters);
	EmptyExpressionList(&fileExpressionFiltersExclude);
	EmptyExpressionList(&dirExpressionFilters);
	EmptyExpressionList(&dirExpressionFiltersExclude);
}

/**
 * @brief Add a single pattern (if nonempty & valid) to a pattern list.
 *
 * @param [in] filterList List where pattern is added.
 * @param [in] str Temporary variable (ie, it may be altered)
 * @param [in] lineNumber Line number in filter file, used for error reporting.
 */
void FileFilter::AddFilterPattern(vector<FileFilterElementPtr>* filterList, const String& str, bool fileFilter, int lineNumber)
{
	int re_opts = Poco::RegularExpression::RE_CASELESS;
	std::string regexString = ucr::toUTF8(str);
	re_opts |= Poco::RegularExpression::RE_UTF8;
	try
	{
		filterList->push_back(FileFilterElementPtr(new FileFilterElement(regexString, re_opts, fileFilter)));
	}
	catch (const Poco::RegularExpressionException& e)
	{
		errors.emplace_back(FILTER_ERROR_INVALID_REGULAR_EXPRESSION, lineNumber, -1, str, e.message());
	}
}

/**
 * @brief Add a single expression (if nonempty & valid) to a expression list.
 *
 * @param [in] filterList List where expression is added.
 * @param [in] str Temporary variable (ie, it may be altered)
 * @param [in] lineNumber Line number in filter file, used for error reporting.
*/
void FileFilter::AddFilterExpression(vector<FilterExpressionPtr>* filterList, const String& str, int lineNumber)
{
	String str2 = strutils::trim_ws(str);
	std::shared_ptr<FilterExpression> pExpression(new FilterExpression(ucr::toUTF8(str)));
	if (pExpression->errorCode != 0)
	{
		errors.emplace_back(pExpression->errorCode, lineNumber, pExpression->errorPosition, str2, pExpression->errorMessage);
		return;
	}
	filterList->emplace_back(pExpression);
}

/**
 * @brief Deletes items from filter list.
 *
 * @param [in] filterList List to empty.
 */
void FileFilter::EmptyFilterList(vector<FileFilterElementPtr> *filterList)
{
	filterList->clear();
}

/**
 * @brief Deletes items from expression list.
 *
 * @param [in] expressionList List to empty.
 */
void FileFilter::EmptyExpressionList(vector<FilterExpressionPtr> *expressionList)
{
	expressionList->clear();
}

/**
 * @brief Clone file filter from another filter.
 * This function clones file filter from another filter.
 * Current contents in the filter are removed and new contents added from the given filter.
 * @param [in] filter File filter to clone.
 */
void FileFilter::CloneFrom(const FileFilter* filter)
{
	if (!filter)
		return;

	default_include = filter->default_include;
	name = filter->name;
	description = filter->description;
	fullpath = filter->fullpath;

	filefilters.clear();
	size_t count = filter->filefilters.size();
	filefilters.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		filefilters.emplace_back(std::make_shared<FileFilterElement>(filter->filefilters[i].get()));
	}

	dirfilters.clear();
	count = filter->dirfilters.size();
	dirfilters.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		dirfilters.emplace_back(std::make_shared<FileFilterElement>(filter->dirfilters[i].get()));
	}

	fileExpressionFilters.clear();
	count = filter->fileExpressionFilters.size();
	fileExpressionFilters.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		fileExpressionFilters.emplace_back(std::make_shared<FilterExpression>(*filter->fileExpressionFilters[i].get()));
	}

	dirExpressionFilters.clear();
	count = filter->dirExpressionFilters.size();
	dirExpressionFilters.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		dirExpressionFilters.emplace_back(std::make_shared<FilterExpression>(*filter->dirExpressionFilters[i].get()));
	}

	filefiltersExclude.clear();
	count = filter->filefiltersExclude.size();
	filefiltersExclude.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		filefiltersExclude.emplace_back(std::make_shared<FileFilterElement>(filter->filefiltersExclude[i].get()));
	}

	dirfiltersExclude.clear();
	count = filter->dirfiltersExclude.size();
	dirfiltersExclude.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		dirfiltersExclude.emplace_back(std::make_shared<FileFilterElement>(filter->dirfiltersExclude[i].get()));
	}

	fileExpressionFiltersExclude.clear();
	count = filter->fileExpressionFiltersExclude.size();
	fileExpressionFiltersExclude.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		fileExpressionFiltersExclude.emplace_back(std::make_shared<FilterExpression>(*filter->fileExpressionFiltersExclude[i].get()));
	}

	dirExpressionFiltersExclude.clear();
	count = filter->dirExpressionFiltersExclude.size();
	dirExpressionFiltersExclude.reserve(count);
	for (size_t i = 0; i < count; i++)
	{
		dirExpressionFiltersExclude.emplace_back(std::make_shared<FilterExpression>(*filter->dirExpressionFiltersExclude[i].get()));
	}

	errors = filter->errors;
}

/**
 * @brief Test given string against given regexp list.
 *
 * @param [in] filterList List of regexps to test against.
 * @param [in] szTest String to test against regexps.
 * @return true if string passes
 * @note Matching stops when first match is found.
 */
bool TestAgainstRegList(const vector<FileFilterElementPtr>* filterList, const String& szTest)
{
	if (filterList->size() == 0)
		return false;

	std::string compString, compStringFileName;
	ucr::toUTF8(szTest, compString);
	vector<FileFilterElementPtr>::const_iterator iter = filterList->begin();
	while (iter != filterList->end())
	{
		Poco::RegularExpression::Match match;
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
bool FileFilter::TestAgainstRegList(const vector<FileFilterElementPtr>* filterList, const DIFFITEM& di)
{
	if (filterList->size() == 0)
		return false;

	const int nDirs = di.diffcode.isThreeway() ? 3 : 2;
	int i = 0;
	for (; i < nDirs; ++i)
	{
		if (!di.diffFileInfo[i].filename.get().empty())
			break;
	}
	if (i >= nDirs)
		return false;

	const String& szTest = paths::ConcatPath(di.diffFileInfo[i].path, di.diffFileInfo[i].filename);
	std::string compString, compStringFileName;
	ucr::toUTF8(szTest, compString);
	vector<FileFilterElementPtr>::const_iterator iter = filterList->begin();
	while (iter != filterList->end())
	{
		Poco::RegularExpression::Match match;
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
 * @brief Test given DIFFITEM against given expression list.
 * @param [in] filterList List of expressions to test against.
 * @param [in] di DIFFITEM to test against regexps.
 * @return true if DIFFITEM passes
 * @note Matching stops when first match is found.
 */
bool FileFilter::TestAgainstExpressionList(const vector<FilterExpressionPtr>* filterList, const DIFFITEM& di)
{
	if (filterList->size() == 0)
		return false;

	for (const auto& filter : *filterList)
	{
		if (filter->Evaluate(di))
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
 * @param [in] szFileName Filename to test
 * @return true if file passes the filter
 */
bool FileFilter::TestFileNameAgainstFilter(const String& szFileName) const
{
	if (::TestAgainstRegList(&filefilters, szFileName))
	{
		if (filefiltersExclude.empty() || !::TestAgainstRegList(&filefiltersExclude, szFileName))
			return !default_include;
	}
	return default_include;
}

/**
 * @brief Set diff context for all filters in the given file filter.
 * @param [in] pDiffContext Pointer to diff context to set for all filters.
 */
void FileFilter::SetDiffContext(const CDiffContext* pDiffContext)
{
	for (auto& filters :
		{ fileExpressionFilters, fileExpressionFiltersExclude, dirExpressionFilters, dirExpressionFiltersExclude })
	{
		for (const auto& filter : filters)
			filter->SetDiffContext(pDiffContext);
	}
}

/**
 * @brief Test given DIFFITEM against filefilter.
 * @param [in] di DIFFITEM to test
 * @return true if DIFFITEM passes the filter
 */
bool FileFilter::TestFileDiffItemAgainstFilter(const DIFFITEM& di) const
{
	bool matched = TestAgainstRegList(&filefilters, di);
	if (!matched && TestAgainstExpressionList(&fileExpressionFilters, di))
		matched = true;
	if (matched)
	{
		matched = !TestAgainstRegList(&filefiltersExclude, di);
		if (matched)
			matched = !TestAgainstExpressionList(&fileExpressionFiltersExclude, di);
	}
	if (matched)
		return !default_include;
	return default_include;
}

/**
 * @brief Test given directory name against filefilter.
 *
 * Test directory name against active filefilter. If matching rule is found
 * we must first determine type of rule that matched. If we return false
 * from this function directory scan marks file as skipped.
 *
 * @param [in] szDirName Directory name to test
 * @return true if directory name passes the filter
 */
bool FileFilter::TestDirNameAgainstFilter(const String& szDirName) const
{
	if (::TestAgainstRegList(&dirfilters, szDirName))
	{
		if (dirfiltersExclude.empty() || !::TestAgainstRegList(&dirfiltersExclude, szDirName))
			return !default_include;
	}
	return default_include;
}

/**
 * @brief Test given DIFFITEM against filefilter.
 * @param [in] di DIFFITEM to test
 * @return true if DIFFITEM passes the filter
 */
bool FileFilter::TestDirDiffItemAgainstFilter(const DIFFITEM& di) const
{
	bool matched = TestAgainstRegList(&dirfilters, di);
	if (!matched && TestAgainstExpressionList(&dirExpressionFilters, di))
		matched = true;
	if (matched)
	{
		matched = !TestAgainstRegList(&dirfiltersExclude, di);
		if (matched)
			matched = !TestAgainstExpressionList(&dirExpressionFiltersExclude, di);
	}
	if (matched)
		return !default_include;
	return default_include;
}


