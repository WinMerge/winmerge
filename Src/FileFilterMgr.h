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
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef FileFilter_h_included
#define FileFilter_h_included

// Uses MFC C++ template containers
#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif
#include "pcre.h"

struct FileFilterElement;
/**
 * @brief List of file filtering rules.
 * @sa FileFilterElement
 * @sa FileFilter
 */
typedef CList<FileFilterElement, FileFilterElement&> FileFilterList;

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
	pcre *pRegExp; /**< Compiled regular expression */
	pcre_extra *pRegExpExtra; /**< Additional information got from regex study */
	FileFilterElement() : pRegExp(NULL), pRegExpExtra(NULL) { };
};

struct FileFilter;

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
	void LoadFromDirectory(LPCTSTR szPattern, LPCTSTR szExt);
	// Reload an edited filter
	int ReloadFilterFromDisk(FileFilter * pfilter);
	int ReloadFilterFromDisk(LPCTSTR szFullPath);
	// Load a filter from a string
	void LoadFilterString(LPCTSTR szFilterString);
	int AddFilter(LPCTSTR szFilterFile);
	void RemoveFilter(LPCTSTR szFilterFile);

	// access to array of filters
	int GetFilterCount() const { return m_filters.GetSize(); }
	CString GetFilterName(int i) const;
	CString GetFilterName(const FileFilter *pFilter) const;
	CString GetFilterPath(int i) const;
	CString GetFilterDesc(int i) const;
	CString GetFilterDesc(const FileFilter *pFilter) const;
	FileFilter * GetFilterByPath(LPCTSTR szFilterName);
	CString GetFullpath(FileFilter * pfilter) const;

	// methods to actually use filter
	BOOL TestFileNameAgainstFilter(const FileFilter * pFilter, LPCTSTR szFileName) const;
	BOOL TestDirNameAgainstFilter(const FileFilter * pFilter, LPCTSTR szDirName) const;

	void DeleteAllFilters();

// Implementation methods
protected:
	// Clear the list of known filters
	// Load a filter from a file (if syntax is valid)
	FileFilter * LoadFilterFile(LPCTSTR szFilepath, int & errorcode);

// Implementation data
private:
	CTypedPtrArray<CPtrArray, FileFilter *> m_filters; /*< List of filters loaded */
};


// I think that CRegExp doesn't copy correctly (I get heap corruption in CRegList::program)
// so I'm using pointers to avoid its copy constructor
// Perry, 2003-05-18

BOOL TestAgainstRegList(const FileFilterList & filterList, LPCTSTR szTest);
void EmptyFilterList(FileFilterList & filterList);


#endif // FileFilter_h_included
