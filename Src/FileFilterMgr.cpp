/////////////////////////////////////////////////////////////////////////////
// FileFilterMgr.cpp : implementation file
// see FileFilterMgr.h for description
/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//    You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file FileFilterMgr.cpp
 *
 *  @brief Implementation of FileFilterMgr and supporting routines
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "FileFilterMgr.h"
#include "RegExp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void DeleteRegList(RegList & reglist)
{
	while (!reglist.IsEmpty())
	{
		CRegExp * regexp = reglist.RemoveHead();
		delete regexp;
	}
}

/**
 * @brief One actual filter.
 *
 * For example, this might be a GNU C filter, excluding *.o files and CVS
 * directories. That is to say, a filter is a set of file masks and
 * directory masks
 */
struct FileFilter
{
	bool default_include;
	CString name;
	CString description;
	CString fullpath;
	RegList filefilters; 
	RegList dirfilters; 
	FileFilter() : default_include(true) { }
	~FileFilter();
};
FileFilter::~FileFilter()
{
	DeleteRegList(filefilters);
	DeleteRegList(dirfilters);
}

FileFilterMgr::~FileFilterMgr()
{
	DeleteAllFilters();
}

/**
 * @brief Load filter file from disk.
 *
 * @param [in] szPattern Pattern from where to load filters, for example "\\Filters\\*.flt"
 * @param [in] szExt File-extension of filter files
 */
void FileFilterMgr::LoadFromDirectory(LPCTSTR szPattern, LPCTSTR szExt)
{
	// DeleteAllFilters();
	CFileFind finder;
	BOOL bWorking = finder.FindFile(szPattern);
	int extlen = szExt ? _tcslen(szExt) : 0;
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDots() || finder.IsDirectory())
			continue;
		CString sFilename = finder.GetFileName();
		if (szExt)
		{
			// caller specified a specific extension
			// (This is really a workaround for brokenness in windows, which
			//  doesn't screen correctly on extension in pattern)
			if (sFilename.Right(extlen).CompareNoCase(szExt))
				return;
		}
		FileFilter * pfilter = LoadFilterFile(finder.GetFilePath(), sFilename);
		m_filters.Add(pfilter);
	}
}

/**
 * @brief Removes all filters from current list.
 */
void FileFilterMgr::DeleteAllFilters()
{
	for (int i=0; i<m_filters.GetSize(); ++i)
	{
		delete m_filters[i];
		m_filters[i] = 0;
	}
	m_filters.RemoveAll();
}

/**
 * @brief Add a single pattern (if nonempty & valid) to a pattern list.
 *
 * @param [in] RegList List where pattern is added.
 * @param [in] str Temporary variable (ie, it may be altered)
 */
static void AddFilterPattern(RegList & reglist, CString & str)
{
	LPCTSTR commentLeader = _T("##"); // Starts comment
	str.MakeUpper();
	str.TrimLeft();

	// Ignore lines beginning with '##'
	int pos = str.Find(commentLeader);
	if (pos == 0)
		return;

	// Find possible comment-separator '<whitespace>##'
	while (pos > 0 && !_istspace(str[pos - 1]))
		pos = str.Find(commentLeader, pos);	

	// Remove comment and whitespaces before it
	if (pos > 0)
		str = str.Left(pos);
	str.TrimRight();
	if (str.IsEmpty())
		return;

	CRegExp * regexp = new CRegExp;
	if (regexp->RegComp(str))
		reglist.AddTail(regexp);
	else
		delete regexp;
}

/**
 * @brief Parse a filter file, and add it to array if valid.
 *
 * @param [in] szFilePath Path (w/o filename) to file to load.
 * @param [in] szFilename Name of file to load.
 */
FileFilter * FileFilterMgr::LoadFilterFile(LPCTSTR szFilepath, LPCTSTR szFilename)
{
	CStdioFile file;
	if (!file.Open(szFilepath, CFile::modeRead | CFile::shareDenyNone))
		return NULL;
	FileFilter *pfilter = new FileFilter;
	pfilter->fullpath = szFilepath;
	pfilter->name = szFilename; // default if no name
	CString sLine;
	while (file.ReadString(sLine))
	{
		sLine.TrimLeft();
		sLine.TrimRight();

		if (0 == _tcsncmp(sLine, _T("name:"), 5))
		{
			// specifies display name
			CString str = sLine.Mid(5);
			str.TrimLeft();
			if (!str.IsEmpty())
				pfilter->name = str;
		}
		else if (0 == _tcsncmp(sLine, _T("desc:"), 5))
		{
			// specifies display name
			CString str = sLine.Mid(5);
			str.TrimLeft();
			if (!str.IsEmpty())
				pfilter->description = str;
		}
		else if (0 == _tcsncmp(sLine, _T("def:"), 4))
		{
			// specifies default
			CString str = sLine.Mid(4);
			str.TrimLeft();
			if (str == _T("0") || str == _T("no") || str == _T("exclude"))
				pfilter->default_include = false;
			else if (str == _T("1") || str == _T("yes") || str == _T("include"))
				pfilter->default_include = true;
		}
		else if (0 == _tcsncmp(sLine, _T("f:"), 2))
		{
			// file filter
			CString str = sLine.Mid(2);
			AddFilterPattern(pfilter->filefilters, str);
		}
		else if (0 == _tcsncmp(sLine, _T("d:"), 2))
		{
			// directory filter
			CString str = sLine.Mid(2);
			AddFilterPattern(pfilter->dirfilters, str);
		}
	}
	return pfilter;
}

/**
 * @brief Give client back a pointer to the actual filter.
 *
 * @param [in] szFilterPath Full path to filterfile.
 * @return Pointer to found filefilter or NULL;
 * @note We just do a linear search, because this is seldom called
 */
FileFilter * FileFilterMgr::GetFilterByPath(LPCTSTR szFilterPath)
{
	for (int i=0; i<m_filters.GetSize(); ++i)
	{
		if (m_filters[i]->fullpath == szFilterPath)
			return m_filters[i];
	}
	return 0;
}

/**
 * @brief Test given string against given regexp list.
 *
 * @param [in] reglist List of regexps to test against.
 * @param [in] szTest String to test against regexps.
 * @return TRUE if string passes
 */
BOOL TestAgainstRegList(const RegList & reglist, LPCTSTR szTest)
{
	CString str = szTest;
	str.MakeUpper();
	for (POSITION pos = reglist.GetHeadPosition(); pos; )
	{
		CRegExp * regexp = reglist.GetNext(pos);
		if (regexp->RegFind(str) != -1)
			return TRUE;
	}
	return FALSE;
}

/**
 * @brief Test given filename against filefilter.
 *
 * Test filename against active filefilter. If matching rule is found
 * we must first determine type of rule that matched. If we return FALSE
 * from this function directory scan marks file as skipped.
 *
 * @param [in] pFilter Pointer to filefilter
 * @param [in] szFileName Filename to test
 * @return TRUE if file passes the filter
 */
BOOL FileFilterMgr::TestFileNameAgainstFilter(FileFilter * pFilter, LPCTSTR szFileName)
{
	if (!pFilter) return TRUE;
	if (TestAgainstRegList(pFilter->filefilters, szFileName))
		return !pFilter->default_include;
	return pFilter->default_include;
}

/**
 * @brief Test given directory name against filefilter.
 *
 * Test directory name against active filefilter. If matching rule is found
 * we must first determine type of rule that matched. If we return FALSE
 * from this function directory scan marks file as skipped.
 *
 * @param [in] pFilter Pointer to filefilter
 * @param [in] szDirName Directory name to test
 * @return TRUE if directory name passes the filter
 */
BOOL FileFilterMgr::TestDirNameAgainstFilter(FileFilter * pFilter, LPCTSTR szDirName)
{
	if (!pFilter) return TRUE;
	if (TestAgainstRegList(pFilter->dirfilters, szDirName))
		return !pFilter->default_include;
	return pFilter->default_include;
}

/**
 * @brief Return name of filter.
 *
 * @param [in] i Index of filter.
 * @return Name of filter in given index.
 */
CString FileFilterMgr::GetFilterName(int i) const
{
	return m_filters[i]->name; 
}

/**
 * @brief Return description of filter.
 *
 * @param [in] i Index of filter.
 * @return Description of filter in given index.
 */
CString FileFilterMgr::GetFilterDesc(int i) const
{
	return m_filters[i]->description; 
}

/**
 * @brief Return full path to filter.
 *
 * @param [in] i Index of filter.
 * @return Full path of filter in given index.
 */
CString FileFilterMgr::GetFilterPath(int i) const
{
	return m_filters[i]->fullpath;
}

/**
 * @brief Return full path to filter.
 *
 * @param [in] pFilter Pointer to filter.
 * @return Full path of filter.
 */
CString FileFilterMgr::GetFullpath(FileFilter * pfilter) const
{
	return pfilter->fullpath;
}

/**
 * @brief Reload filter from disk
 *
 * Reloads filter from disk. This is done by creating a new one
 * to substitute for old one.
 * @param [in] pFilter Pointer to filter to reload.
 */
void FileFilterMgr::ReloadFilterFromDisk(FileFilter * pfilter)
{
	FileFilter * newfilter = LoadFilterFile(pfilter->fullpath, pfilter->name);
	for (int i=0; i<m_filters.GetSize(); ++i)
	{
		if (pfilter == m_filters[i])
		{
			m_filters.RemoveAt(i);
			delete pfilter;
			break;
		}
	}
	m_filters.Add(newfilter);
}

/**
 * @brief Reload filter from disk
 *
 * Reloads filter from disk. This is done by creating a new one
 * to substitute for old one.
 * @param [in] szFullPath Full path to filter file to reload.
 */
void FileFilterMgr::ReloadFilterFromDisk(LPCTSTR szFullPath)
{
	FileFilter * filter = GetFilterByPath(szFullPath);
	ReloadFilterFromDisk(filter);
}
