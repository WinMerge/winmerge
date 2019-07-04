/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file FileFilterMgr.h
 *
 *  @brief Declaration file for FileFilterMgr
 */ 
#pragma once

#include <vector>
#include "UnicodeString.h"
#include "FileFilter.h"

/**
 * @brief Return values for many filter functions.
 */
enum FILTER_RETVALUE
{
	FILTER_OK = 0,  /**< Success */
	FILTER_ERROR_FILEACCESS,  /**< File could not be opened etc. */
	FILTER_NOTFOUND, /**< Filter not found */
};

/**
 * @brief File filter manager for handling filefilters.
 *
 * The FileFilterMgr loads a collection of named file filters from disk,
 * and provides lookup access by name, or array access by index, to these
 * named filters. It also provides test functions for actually using the
 * filters.
 *
 * We are using PCRE for regular expressions. Nice thing in PCRE is it supports
 * UTF-8 unicode, unlike many other libs. For ANSI builds we use just ansi
 * strings, and for unicode we must first convert strings to UTF-8.
 */
class FileFilterMgr
{
private:

public:
	~FileFilterMgr();
	// Reload filter array from specified directory (passed to CFileFind)
	void LoadFromDirectory(const String& dir, const String& szPattern, const String& szExt);
	// Reload an edited filter
	int ReloadFilterFromDisk(FileFilter * pfilter);
	int ReloadFilterFromDisk(const String& szFullPath);
	// Load a filter from a string
	void LoadFilterString(const String& szFilterString);
	int AddFilter(const String& szFilterFile);
	void RemoveFilter(const String& szFilterFile);

	// access to array of filters
	int GetFilterCount() const { return (int) m_filters.size(); }
	String GetFilterName(int i) const;
	String GetFilterName(const FileFilter *pFilter) const;
	String GetFilterPath(int i) const;
	String GetFilterDesc(int i) const;
	String GetFilterDesc(const FileFilter *pFilter) const;
	FileFilter * GetFilterByPath(const String& szFilterName);
	String GetFullpath(FileFilter * pfilter) const;

	// methods to actually use filter
	bool TestFileNameAgainstFilter(const FileFilter * pFilter, const String& szFileName) const;
	bool TestDirNameAgainstFilter(const FileFilter * pFilter, const String& szDirName) const;

	void DeleteAllFilters();

// Implementation methods
protected:
	// Clear the list of known filters
	// Load a filter from a file (if syntax is valid)
	FileFilter * LoadFilterFile(const String& szFilepath, int & errorcode);

// Implementation data
private:
	std::vector<FileFilterPtr> m_filters; /*< List of filters loaded */
};


bool TestAgainstRegList(const std::vector<FileFilterElementPtr> *filterList, const String& szTest);
void EmptyFilterList(std::vector<FileFilterElementPtr> *filterList);

/**
 * @brief Return name of filter.
 *
 * @param [in] i Index of filter.
 * @return Name of filter in given index.
 */
inline String FileFilterMgr::GetFilterName(int i) const
{
	return m_filters[i]->name; 
}

/**
 * @brief Return name of filter.
 * @param [in] pFilter Filter to get name for.
 * @return Given filter's name.
 */
inline String FileFilterMgr::GetFilterName(const FileFilter *pFilter) const
{
	return pFilter->name; 
}

/**
 * @brief Return description of filter.
 *
 * @param [in] i Index of filter.
 * @return Description of filter in given index.
 */
inline String FileFilterMgr::GetFilterDesc(int i) const
{
	return m_filters[i]->description; 
}

/**
 * @brief Return description of filter.
 * @param [in] pFilter Filter to get description for.
 * @return Given filter's description.
 */
inline String FileFilterMgr::GetFilterDesc(const FileFilter *pFilter) const
{
	return pFilter->description;
}

/**
 * @brief Return full path to filter.
 *
 * @param [in] i Index of filter.
 * @return Full path of filter in given index.
 */
inline String FileFilterMgr::GetFilterPath(int i) const
{
	return m_filters[i]->fullpath;
}

/**
 * @brief Return full path to filter.
 *
 * @param [in] pFilter Pointer to filter.
 * @return Full path of filter.
 */
inline String FileFilterMgr::GetFullpath(FileFilter * pfilter) const
{
	return pfilter->fullpath;
}
