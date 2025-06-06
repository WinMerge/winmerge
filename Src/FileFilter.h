// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  FileFilter.h
 *
 * @brief Declaration file for FileFilter
 */
#pragma once

#include "FilterError.h"
#include <vector>
#include <memory>
#define POCO_NO_UNWINDOWS 1
#include <Poco/RegularExpression.h>
#include "UnicodeString.h"

struct FilterExpression;
class CDiffContext;
class DIFFITEM;

/**
 * @brief FileFilter rule.
 *
 * Contains one filtering element definition (rule). In addition to
 * regular expression there is boolean value for defining if rule
 * is inclusive or exclusive. File filters have global inclusive/exclusive
 * selection but this per-rule setting overwrites it.
 *
 * We are using PCRE for regular expressions and pRegExp points to compiled
 * regular expression. pRegExpExtra contains additional information about
 * the expression used to optimize matching.
 */
struct FileFilterElement
{
	Poco::RegularExpression regexp; /**< Compiled regular expression */
	std::string _regex; /**< Regular expression string to set to Poco::RegularExpression */
	int _reOpts; /**< Options to set to Poco::RegularExpression */
	bool _fileNameOnly; /**< If true, indicates that the filter matches only filenames */
	FileFilterElement(const std::string& regex, int reOpts, bool fileFilter) :
		regexp(regex, reOpts), _regex(regex), _reOpts(reOpts), _fileNameOnly(fileFilter && regex.find("\\\\", 0) == String::npos && regex.find_first_of(":/") == String::npos)
	{
	}
	FileFilterElement(const FileFilterElement* element) :
		regexp(element->_regex, element->_reOpts), _regex(element->_regex), _reOpts(element->_reOpts), _fileNameOnly(element->_fileNameOnly)
	{
	}
};

struct FileFilterErrorInfo
{
	int line; /**< Line number in filter file where error occurred */
	FilterErrorCode errorCode; /**< Error code, see FilterErrorCode enum for values */
	int errorPosition; /**< Position in line where error occurred, if applicable */
	String srcText; /**< Source text of the line where error occurred, if applicable */
	std::string errorText; /**< Text describing the error, if applicable */
	FileFilterErrorInfo(FilterErrorCode code, int lineNumber, int position, const String& src, const std::string& msg) :
		errorCode(code), line(lineNumber), errorPosition(position), srcText(src), errorText(msg)
	{
	}
};

typedef std::shared_ptr<FileFilterElement> FileFilterElementPtr;
typedef std::shared_ptr<FilterExpression> FilterExpressionPtr;

/**
 * @brief One actual filter.
 *
 * For example, this might be a GNU C filter, excluding *.o files and CVS
 * directories. That is to say, a filter is a set of file masks and
 * directory masks. Usually FileFilter contains rules from one filter
 * definition file. So it can be thought as filter file contents.
 * @sa FileFilterList
 */
struct FileFilter
{
	bool default_include;	/**< If true, filter rules are inclusive by default */
	String name;			/**< Filter name (shown in UI) */
	String description;	/**< Filter description text */
	String fullpath;		/**< Full path to filter file */
	std::vector<FileFilterElementPtr> filefilters; /**< List of rules for files */
	std::vector<FileFilterElementPtr> filefiltersExclude; /**< List of rules for files (exclude) */
	std::vector<FileFilterElementPtr> dirfilters;  /**< List of rules for directories */
	std::vector<FileFilterElementPtr> dirfiltersExclude;  /**< List of rules for directories (exclude) */
	std::vector<FilterExpressionPtr> fileExpressionFilters; /**< List of file filter expressions */
	std::vector<FilterExpressionPtr> fileExpressionFiltersExclude; /**< List of file filter expressions (exclude) */
	std::vector<FilterExpressionPtr> dirExpressionFilters; /**< List of dir filter expressions */
	std::vector<FilterExpressionPtr> dirExpressionFiltersExclude; /**< List of dir filter expressions (exclude) */
	std::vector<FileFilterErrorInfo> errors; /**< List of errors in filter file */
	FileFilter() : default_include(true) { }
	~FileFilter();
	
	void AddFilterPattern(std::vector<FileFilterElementPtr>* filterList, const String& str, bool fileFilter, int lineNumber);
	void AddFilterExpression(std::vector<FilterExpressionPtr>* filterList, const String& str, int lineNumber);
	static void EmptyFilterList(std::vector<FileFilterElementPtr> *filterList);
	static void EmptyExpressionList(std::vector<FilterExpressionPtr> *filterList);
	void CloneFrom(const FileFilter* filter);
	// methods to actually use filter
	bool TestFileNameAgainstFilter(const String& szFileName) const;
	void SetDiffContext(const CDiffContext* pDiffContext);
	bool TestFileDiffItemAgainstFilter(const DIFFITEM& di) const;
	bool TestDirNameAgainstFilter(const String& szDirName) const;
	bool TestDirDiffItemAgainstFilter(const DIFFITEM& di) const;
};

typedef std::shared_ptr<FileFilter> FileFilterPtr;
