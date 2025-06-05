// SPDX-License-Identifier: GPL-2.0-or-later
/**
 *  @file FileFilter.cpp
 *
 *  @brief Implementation of FileFilter.
 */ 

#include "pch.h"
#include "FileFilter.h"
#include "FilterEngine/FilterExpression.h"
#include "unicoder.h"
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
	catch (Poco::RegularExpressionException e)
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
		errors.emplace_back(pExpression->errorCode, lineNumber, pExpression->errorPosition, str2, pExpression->errorMessage);
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
}
