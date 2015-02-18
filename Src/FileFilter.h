/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
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
	FileFilterElement(const std::string &regex, int reOpts) : regexp(regex, reOpts)
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
	std::vector<FileFilterElementPtr> dirfilters;  /**< List of rules for directories */
	FileFilter() : default_include(true) { }
	~FileFilter();
	
	static void EmptyFilterList(std::vector<FileFilterElementPtr> *filterList);
};

typedef std::shared_ptr<FileFilter> FileFilterPtr;
