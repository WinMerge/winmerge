/////////////////////////////////////////////////////////////////////////////
// FileFilterMgr.cpp : implementation file
// see FileFilterMgr.h for description
/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//    You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "FileFilterMgr.h"
#include "RegExp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// I think that CRegExp doesn't copy correctly (I get heap corruption in CRegList::program)
// so I'm using pointers to avoid its copy constructor
// Perry, 2003-05-18

typedef CTypedPtrList<CPtrList, CRegExp*>RegList;
static void DeleteRegList(RegList & reglist)
{
	while (!reglist.IsEmpty())
	{
		CRegExp * regexp = reglist.RemoveHead();
		delete regexp;
	}
}
// One actual filter
// For example, this might be a GNU C filter, excluding *.o files and CVS directories
// That is to say, a filter is a set of file masks and directory masks
struct FileFilter
{
	bool default_include;
	CString name;
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

// Load 
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

void FileFilterMgr::DeleteAllFilters()
{
	for (int i=0; i<m_filters.GetSize(); ++i)
	{
		delete m_filters[i];
		m_filters[i] = 0;
	}
	m_filters.RemoveAll();
}

// Add a single pattern (if nonempty & valid) to a pattern list
// str is a temporary variable (ie, it may be altered)
static void AddFilterPattern(RegList & reglist, CString & str)
{
	str.TrimLeft();
	str.MakeUpper();
	LPCTSTR commentLeader = _T(" ##");
	// anything from commentLeader to end of line is a comment
	int comment = str.Find(commentLeader);
	if (comment >= 0)
	{
		str = str.Left(comment);
	}
	if (str.IsEmpty()) return;
	CRegExp * regexp = new CRegExp;
	if (regexp->RegComp(str))
		reglist.AddTail(regexp);
	else
		delete regexp;
}

// Parse a filter file, and add it to array if valid
FileFilter * FileFilterMgr::LoadFilterFile(LPCTSTR szFilepath, LPCTSTR szFilename)
{
	CStdioFile file;
	if (!file.Open(szFilepath, CFile::modeRead))
		return NULL;
	FileFilter *pfilter = new FileFilter;
	pfilter->fullpath = szFilepath;
	pfilter->name = szFilename; // default if no name
	CString sLine;
	while (file.ReadString(sLine))
	{
		if (0 == _tcsncmp(sLine, _T("name:"), 5))
		{
			// specifies display name
			CString str = sLine.Mid(5);
			str.TrimLeft();
			if (!str.IsEmpty())
				pfilter->name = str;
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

// Give client back a pointer to the actual filter
// We just do a linear search, because this is seldom called
FileFilter * FileFilterMgr::GetFilterByPath(LPCTSTR szFilterPath)
{
	for (int i=0; i<m_filters.GetSize(); ++i)
	{
		if (m_filters[i]->fullpath == szFilterPath)
			return m_filters[i];
	}
	return 0;
}

// return TRUE if string passes
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

// return TRUE if file passes the filter
BOOL FileFilterMgr::TestFileNameAgainstFilter(FileFilter * pFilter, LPCTSTR szFileName)
{
	if (!pFilter) return TRUE;
	if (TestAgainstRegList(pFilter->filefilters, szFileName))
		return !pFilter->default_include;
	return pFilter->default_include;
}

// return TRUE if directory passes the filter
BOOL FileFilterMgr::TestDirNameAgainstFilter(FileFilter * pFilter, LPCTSTR szDirName)
{
	if (!pFilter) return TRUE;
	if (TestAgainstRegList(pFilter->dirfilters, szDirName))
		return !pFilter->default_include;
	return pFilter->default_include;
}

CString FileFilterMgr::GetFilterName(int i) const
{
	return m_filters[i]->name; 
}

CString FileFilterMgr::GetFilterPath(int i) const
{
	return m_filters[i]->fullpath;
}

CString FileFilterMgr::GetFullpath(FileFilter * pfilter) const
{
	return pfilter->fullpath;
}

// Reload filter from disk (by creating a new one to substitute for old one)
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