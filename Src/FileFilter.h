// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  FileFilter.h
 *
 * @brief Declaration file for FileFilter
 */
#pragma once

#include <vector>
#include <memory>
#define POCO_NO_UNWINDOWS 1
#include <Poco/RegularExpression.h>
#include "UnicodeString.h"

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

typedef std::shared_ptr<FileFilterElement> FileFilterElementPtr;

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
	std::vector<String> expressionFilters; /**< List of filter expressions */
	std::vector<String> expressionFiltersExclude; /**< List of filter expressions (exclude) */
	FileFilter() : default_include(true) { }
	~FileFilter();
	
	static void EmptyFilterList(std::vector<FileFilterElementPtr> *filterList);
	void CloneFrom(const FileFilter* filter);
};

typedef std::shared_ptr<FileFilter> FileFilterPtr;
