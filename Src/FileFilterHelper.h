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
 * @brief Structure containing user-visible information about filter.
 */
struct FileFilterInfo
{
	CString name; 			/**< Name of filter */
	CString description; 	/**< Description of filter (shown in UI) */
	CString fullpath;		/**< Full path to filter file */
};

typedef CArray<FileFilterInfo, FileFilterInfo> FILEFILTER_INFOLIST;

/// Interface for testing files & directories for exclusion, as diff traverses file tree
class IDiffFilter
{
public:
	virtual BOOL includeFile(LPCTSTR szFileName) = 0;
	virtual BOOL includeDir(LPCTSTR szDirName) = 0;
};

/**
 * @brief Helper class for using filefilters.
 */
class FileFilterHelper : public IDiffFilter
{
public:
	FileFilterHelper();

	FileFilterMgr * GetManager();
	void SetManager(FileFilterMgr * pFilterManager);
	CString GetFileFilterPath() const { return m_sFileFilterPath; }
	void SetFileFilterPath(LPCTSTR szFileFilterPath);
	void EditFileFilter(LPCTSTR szFileFilterName);
	void GetFileFilters(FILEFILTER_INFOLIST * filters, CString & selected) const;
	CString GetFileFilterName(CString filterPath);
	CString GetFileFilterPath(CString filterName);

	void LoadFileFilterDirPattern(CMap<CString, LPCTSTR, int, int> & patternsLoaded, const CString & sPattern);

	void UseMask(BOOL bUseMask);
	void SetMask(LPCTSTR strMask);

	BOOL GetUseMask();
	CString GetFilter();
	BOOL SetFilter(CString filter);

	BOOL includeFile(LPCTSTR szFileName);
	BOOL includeDir(LPCTSTR szDirName);

protected:
	CString ParseExtensions(CString extensions);

private:
	FileFilter * m_currentFilter;
	FileFilterMgr * m_fileFilterMgr;
	CString m_sFileFilterPath;
	CString m_sMask;
	BOOL m_bUseMask;

	CRegExp m_rgx;

};

#endif // _FILEFILTERHELPER_H_
