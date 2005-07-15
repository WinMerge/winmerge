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
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _FILEFILTERHELPER_H_
#define _FILEFILTERHELPER_H_

class FileFilterMgr;
struct FileFilter;

#ifndef REGEXP_H
#include "RegExp.h"
#endif

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
	CString name; 			/**< Name of filter */
	CString description; 	/**< Description of filter (shown in UI) */
	CString fullpath;		/**< Full path to filter file */
	FileInfo fileinfo;		/**< For tracking if file has been modified */
};

typedef CArray<FileFilterInfo, FileFilterInfo&> FILEFILTER_INFOLIST;
typedef CMap<CString, LPCTSTR, int, int> FILEFILTER_FILEMAP;

/// Interface for testing files & directories for exclusion, as diff traverses file tree
class IDiffFilter
{
public:
	virtual BOOL includeFile(LPCTSTR szFileName) = 0;
	virtual BOOL includeDir(LPCTSTR szDirName) = 0;
	BOOL includeFile(LPCTSTR szFileName1, LPCTSTR szFileName2)
	{
		return
		(
			(szFileName1[0] == '\0' || includeFile(szFileName1))
		&&	(szFileName2[0] == '\0' || includeFile(szFileName2))
		);
	}
	BOOL includeDir(LPCTSTR szDirName1, LPCTSTR szDirName2)
	{
		return
		(
			(szDirName1[0] == '\0' || includeDir(szDirName1))
		&&	(szDirName2[0] == '\0' || includeDir(szDirName2))
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

	FileFilterMgr * GetManager();
	CString GetNewFileFilterPath() const { return m_sNewFileFilterPath; }
	void SetFileFilterPath(LPCTSTR szFileFilterPath);
	void EditFileFilter(LPCTSTR szFileFilterPath);
	void GetFileFilters(FILEFILTER_INFOLIST * filters, CString & selected) const;
	CString GetFileFilterName(CString filterPath) const;
	CString GetFileFilterPath(CString filterName) const;

	void ReloadUpdatedFilters();
	void LoadAllFileFilters();

	void LoadFileFilterDirPattern(FILEFILTER_FILEMAP & patternsLoaded,
		const CString & sPattern);

	void UseMask(BOOL bUseMask);
	void SetMask(LPCTSTR strMask);

	BOOL IsUsingMask();
	CString GetFilterNameOrMask();
	BOOL SetFilter(CString filter);

	BOOL includeFile(LPCTSTR szFileName);
	BOOL includeDir(LPCTSTR szDirName);

protected:
	CString ParseExtensions(CString extensions);
	void TestCandidateFilterPath(const CString & sPath);


private:
	FileFilter * m_currentFilter;     /*< Currently selected filefilter */
	FileFilterMgr * m_fileFilterMgr;  /*< Associated FileFilterMgr */
	CString m_sFileFilterPath;        /*< Path to current filter */
	CString m_sMask;   /*< File mask (if defined) "*.cpp *.h" etc */
	BOOL m_bUseMask;   /*< If TRUE file mask is used, filter otherwise */
	CString m_sNewFileFilterPath;    /*< Path where new filters should be created */

	CRegExp m_rgx;     /*< Compiled file mask regular expression */
};

#endif // _FILEFILTERHELPER_H_
