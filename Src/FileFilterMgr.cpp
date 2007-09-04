/////////////////////////////////////////////////////////////////////////////
// FileFilterMgr.cpp : implementation file
// see FileFilterMgr.h for description
/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify it
//    under the terms of the GNU General Public License as published by the
//    Free Software Foundation; either version 2 of the License, or (at your
//    option) any later version.
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file FileFilterMgr.cpp
 *
 *  @brief Implementation of FileFilterMgr and supporting routines
 */ 
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include <string.h>
#include "UnicodeString.h"
#include "pcre.h"
#include "FileFilterMgr.h"
#include "UniFile.h"
#include "coretools.h"
#include "Ucs2Utf8.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Deletes items from filter list.
 *
 * @param [in] filterList List to empty.
 */
void EmptyFilterList(FileFilterList & filterList)
{
	while (!filterList.IsEmpty())
	{
		FileFilterElement &elem = filterList.GetHead();
		pcre_free(elem.pRegExp);
		pcre_free(elem.pRegExpExtra);
		filterList.RemoveHead();
	}
}

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
	CString name;			/**< Filter name (shown in UI) */
	CString description;	/**< Filter description text */
	CString fullpath;		/**< Full path to filter file */
	FileFilterList filefilters; /**< List of rules for files */
	FileFilterList dirfilters;  /**< List of rules for directories */
	FileFilter() : default_include(true) { }
	~FileFilter();
};

FileFilter::~FileFilter()
{
	EmptyFilterList(filefilters);
	EmptyFilterList(dirfilters);
}

FileFilterMgr::~FileFilterMgr()
{
	DeleteAllFilters();
}

/**
 * @brief Loads filterfile from disk and adds it to filters.
 * @param [in] szFilterFile to load.
 * @return FILTER_OK if succeeded or one of FILTER_RETVALUE values on error.
 */
int FileFilterMgr::AddFilter(LPCTSTR szFilterFile)
{
	int errorcode = FILTER_OK;
	FileFilter * pFilter = LoadFilterFile(szFilterFile, errorcode);
	if (pFilter)
		m_filters.Add(pFilter);
	return errorcode;
}

/**
 * @brief Load all filter files matching pattern from disk into internal filter set.
 *
 * @param [in] szPattern Pattern from where to load filters, for example "\\Filters\\*.flt"
 * @param [in] szExt File-extension of filter files
 */
void FileFilterMgr::LoadFromDirectory(LPCTSTR szPattern, LPCTSTR szExt)
{
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
		AddFilter(finder.GetFilePath());
	}
}

/**
 * @brief Removes filter from filterlist.
 *
 * @param [in] szFilterFile Filename of filter to remove.
 */
void FileFilterMgr::RemoveFilter(LPCTSTR szFilterFile)
{
	// Note that m_filters.GetSize can change during loop
	for (int i = 0; i < m_filters.GetSize(); i++)
	{
		FileFilter * pFilter = m_filters.GetAt(i);
		if (pFilter->fullpath.CompareNoCase(szFilterFile) == 0)
		{
			m_filters.RemoveAt(i);
			delete pFilter;
		}
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
 * @param [in] filterList List where pattern is added.
 * @param [in] str Temporary variable (ie, it may be altered)
 */
static void AddFilterPattern(FileFilterList & filterList, CString & str)
{
	LPCTSTR commentLeader = _T("##"); // Starts comment
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

	const char * errormsg = NULL;
	int erroroffset = 0;
	char regexString[200] = {0};
	int regexLen = 0;
	int pcre_opts = 0;

#ifdef UNICODE
	// For unicode builds, use UTF-8.
	// Convert pattern to UTF-8 and set option for PCRE to specify UTF-8.
	regexLen = TransformUcs2ToUtf8((LPCTSTR)str, _tcslen(str),
		regexString, sizeof(regexString));
	pcre_opts |= PCRE_UTF8;
#else
	strcpy(regexString, (LPCTSTR)str);
	regexLen = strlen(regexString);
#endif
	pcre_opts |= PCRE_CASELESS;
	
	pcre *regexp = pcre_compile(regexString, pcre_opts, &errormsg,
		&erroroffset, NULL);
	if (regexp)
	{
		FileFilterElement elem;
		errormsg = NULL;

		pcre_extra *pe = pcre_study(regexp, 0, &errormsg);
		elem.pRegExp = regexp;
		
		if (pe != NULL && errormsg != NULL)
			elem.pRegExpExtra = pe;
		
		filterList.AddTail(elem);
	}
}

/**
 * @brief Parse a filter file, and add it to array if valid.
 *
 * @param [in] szFilePath Path (w/ filename) to file to load.
 * @param [out] error Error-code if loading failed (returned NULL).
 * @return Pointer to new filter, or NULL if error (check error code too).
 */
FileFilter * FileFilterMgr::LoadFilterFile(LPCTSTR szFilepath, int & error)
{
	UniMemFile file;
	if (!file.OpenReadOnly(szFilepath))
	{
		error = FILTER_ERROR_FILEACCESS;
		return NULL;
	}

	file.ReadBom(); // in case it is a Unicode file, let UniMemFile handle BOM

	String fileName;
	SplitFilename(szFilepath, NULL, &fileName, NULL);
	FileFilter *pfilter = new FileFilter;
	pfilter->fullpath = szFilepath;
	pfilter->name = fileName.c_str(); // Filename is the default name

	CString sLine;
	bool lossy = false;
	BOOL bLinesLeft = TRUE;
	do
	{
		// Returns false when last line is read
		bLinesLeft = file.ReadString(sLine, &lossy);
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
	} while (bLinesLeft == TRUE);

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
		if (m_filters[i]->fullpath.CompareNoCase(szFilterPath) == 0)
			return m_filters[i];
	}
	return 0;
}

/**
 * @brief Test given string against given regexp list.
 *
 * @param [in] filterList List of regexps to test against.
 * @param [in] szTest String to test against regexps.
 * @return TRUE if string passes
 * @note Matching stops when first match is found.
 */
BOOL TestAgainstRegList(const FileFilterList & filterList, LPCTSTR szTest)
{
	for (POSITION pos = filterList.GetHeadPosition(); pos; )
	{
		const FileFilterElement & elem = filterList.GetNext(pos);
		int ovector[30];
		char compString[200] = {0};
		int stringLen = 0;
		TCHAR * tempName = _tcsdup(szTest); // Create temp copy for conversions
		TCHAR * cmpStr = _tcsupr(tempName);

#ifdef UNICODE
		stringLen = TransformUcs2ToUtf8(cmpStr, _tcslen(cmpStr),
			compString, sizeof(compString));
#else
		strcpy(compString, cmpStr);
		stringLen = strlen(compString);
#endif

		pcre * regexp = elem.pRegExp;
		pcre_extra * extra = elem.pRegExpExtra;
		int result = pcre_exec(regexp, extra, compString, stringLen,
			0, 0, ovector, 30);

		free(tempName);

		if (result >= 0)
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
BOOL FileFilterMgr::TestFileNameAgainstFilter(const FileFilter * pFilter,
	LPCTSTR szFileName) const
{
	if (!pFilter)
		return TRUE;
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
BOOL FileFilterMgr::TestDirNameAgainstFilter(const FileFilter * pFilter,
	LPCTSTR szDirName) const
{
	if (!pFilter)
		return TRUE;
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

/** @brief Return name of filter. */
CString FileFilterMgr::GetFilterName(const FileFilter *pFilter) const
{
	return pFilter->name; 
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

/** @brief Return description of filter. */
CString FileFilterMgr::GetFilterDesc(const FileFilter *pFilter) const
{
	return pFilter->description;
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
 * @return FILTER_OK when succeeds, one of FILTER_RETVALUE values on error.
 * @note Given filter (pfilter) is freed and must not be used anymore.
 * @todo Should return new filter.
 */
int FileFilterMgr::ReloadFilterFromDisk(FileFilter * pfilter)
{
	int errorcode = FILTER_OK;
	FileFilter * newfilter = LoadFilterFile(pfilter->fullpath, errorcode);

	if (newfilter == NULL)
	{
		return errorcode;
	}

	for (int i = 0; i < m_filters.GetSize(); ++i)
	{
		if (pfilter == m_filters[i])
		{
			m_filters.RemoveAt(i);
			delete pfilter;
			break;
		}
	}
	m_filters.Add(newfilter);
	return errorcode;
}

/**
 * @brief Reload filter from disk.
 *
 * Reloads filter from disk. This is done by creating a new one
 * to substitute for old one.
 * @param [in] szFullPath Full path to filter file to reload.
 * @return FILTER_OK when succeeds or one of FILTER_RETVALUE values when fails.
 */
int FileFilterMgr::ReloadFilterFromDisk(LPCTSTR szFullPath)
{
	int errorcode = FILTER_OK;
	FileFilter * filter = GetFilterByPath(szFullPath);
	if (filter)
		errorcode = ReloadFilterFromDisk(filter);
	else
		errorcode = FILTER_NOTFOUND;
	return errorcode;
}
