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
 * @file  FileFilterHelper.h
 *
 * @brief Declaration file for FileFilterHelper
 */
// ID line follows -- this is updated by SVN
// $Id: FileFilterHelper.h 7024 2009-10-22 18:26:45Z kimmov $

#ifndef _FILEFILTERHELPER_H_
#define _FILEFILTERHELPER_H_

#include <vector>
#include <boost/scoped_ptr.hpp>
#include "UnicodeString.h"
#include "DirItem.h"

class FileFilterMgr;
class FilterList;
struct FileFilter;

/**
 * @brief File extension of file filter files.
 */
const TCHAR FileFilterExt[] = _T(".flt");

/**
 * @brief Helper structure for UI and management of filters.
 *
 * This structure is mostly used as UI aid and to manage filters from UI.
 * fileinfo contains time of filter file's last modification time. By
 * comparing fileinfo to filter file in disk we can determine if file
 * is changed since we last time loaded it.
 */
struct FileFilterInfo
{
	String name; 			/**< Name of filter */
	String description; 	/**< Description of filter (shown in UI) */
	String fullpath;		/**< Full path to filter file */
	DirItem fileinfo;		/**< For tracking if file has been modified */
};

/// Interface for testing files & directories for exclusion, as diff traverses file tree
class IDiffFilter
{
public:
	virtual bool includeFile(const String& szFileName) const = 0;
	virtual bool includeDir(const String& szDirName) const = 0;
	bool includeFile(const String& szFileName1, const String& szFileName2) const
	{
		return
		(
			(szFileName1.empty() || includeFile(szFileName1))
		&&	(szFileName2.empty() || includeFile(szFileName2))
		);
	}
	bool includeFile(const String& szFileName1, const String& szFileName2, const String& szFileName3) const
	{
		return
		(
			(szFileName1.empty() || includeFile(szFileName1))
		&&	(szFileName2.empty() || includeFile(szFileName2))
		&&	(szFileName3.empty() || includeFile(szFileName3))
		);
	}
	bool includeDir(const String& szDirName1, const String& szDirName2) const
	{
		return
		(
			(szDirName1.empty() || includeDir(szDirName1))
		&&	(szDirName2.empty() || includeDir(szDirName2))
		);
	}
	bool includeDir(const String& szDirName1, const String& szDirName2, const String& szDirName3) const
	{
		return
		(
			(szDirName1.empty() || includeDir(szDirName1))
		&&	(szDirName2.empty() || includeDir(szDirName2))
		&&	(szDirName3.empty() || includeDir(szDirName3))
		);
	}
};

/**
 * @brief Helper class for using filefilters.
 *
 * A FileFilterHelper object is the owner of any active mask, and of the file filter manager
 * This is kind of a File Filter SuperManager, taking care of both inline filters from strings
 *  and loaded file filters (the latter handled by its internal file filter manager)
 *
 * This class is mainly for handling two ways to filter files in WinMerge:
 * - File masks: *.ext lists (*.cpp *.h etc)
 * - File filters: regular expression rules in separate files
 *
 * There can be only one filter or file mask active at a time. This class
 * keeps track of selected filtering method and provides simple functions for
 * clients for querying if file is included to compare. Clients don't need
 * to care about compare methods etc details.
 */
class FileFilterHelper : public IDiffFilter
{
public:
	FileFilterHelper();
	~FileFilterHelper();

	String GetGlobalFilterPathWithCreate() const;
	String GetUserFilterPathWithCreate() const;

	FileFilterMgr * GetManager() const;
	void SetFileFilterPath(const String& szFileFilterPath);
	void GetFileFilters(std::vector<FileFilterInfo> * filters, String & selected) const;
	String GetFileFilterName(const String& filterPath) const;
	String GetFileFilterPath(const String& filterName) const;
	void SetUserFilterPath(const String & filterPath);

	void ReloadUpdatedFilters();
	void LoadAllFileFilters();

	void LoadFileFilterDirPattern(const String& dir, const String& szPattern);

	void UseMask(bool bUseMask);
	void SetMask(const String& strMask);

	bool IsUsingMask() const;
	String GetFilterNameOrMask() const;
	bool SetFilter(const String &filter);

	bool includeFile(const String& szFileName) const;
	bool includeDir(const String& szDirName) const;

protected:
	String ParseExtensions(const String &extensions) const;

private:
	boost::scoped_ptr<FilterList> m_pMaskFilter;       /*< Filter for filemasks (*.cpp) */
	FileFilter * m_currentFilter;     /*< Currently selected filefilter */
	boost::scoped_ptr<FileFilterMgr> m_fileFilterMgr;  /*< Associated FileFilterMgr */
	String m_sFileFilterPath;        /*< Path to current filter */
	String m_sMask;   /*< File mask (if defined) "*.cpp *.h" etc */
	bool m_bUseMask;   /*< If TRUE file mask is used, filter otherwise */
	String m_sGlobalFilterPath;    /*< Path for shared filters */
	String m_sUserSelFilterPath;     /*< Path for user's private filters */
};

#endif // _FILEFILTERHELPER_H_
