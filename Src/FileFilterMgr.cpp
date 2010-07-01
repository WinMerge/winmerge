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

#include <windows.h>
#include <string.h>
#include <vector>
#include <tchar.h>
#include "UnicodeString.h"
#include "FileFilter.h"
#include "pcre.h"
#include "FileFilterMgr.h"
#include "UniFile.h"
#include "coretools.h"
#include "Ucs2Utf8.h"
#include "paths.h"

using std::vector;

static void AddFilterPattern(vector<FileFilterElement*> *filterList, String & str);

/**
 * @brief Destructor, frees all filters.
 */
FileFilterMgr::~FileFilterMgr()
{
	DeleteAllFilters();
}

/**
 * @brief Loads filterfile from disk and adds it to filters.
 * @param [in] szFilterFile Filter file to load.
 * @return FILTER_OK if succeeded or one of FILTER_RETVALUE values on error.
 */
int FileFilterMgr::AddFilter(LPCTSTR szFilterFile)
{
	int errorcode = FILTER_OK;
	FileFilter * pFilter = LoadFilterFile(szFilterFile, errorcode);
	if (pFilter)
		m_filters.push_back(pFilter);
	return errorcode;
}

/**
 * @brief Load all filter files matching pattern from disk into internal filter set.
 * @param [in] dir Directory from where filters are loaded.
 * @param [in] szPattern Pattern for filters to load filters, for example "*.flt".
 * @param [in] szExt File-extension of filter files.
 */
void FileFilterMgr::LoadFromDirectory(LPCTSTR dir, LPCTSTR szPattern, LPCTSTR szExt)
{
	const int extlen = szExt ? (int)_tcslen(szExt) : 0;
	const String pattern = paths_ConcatPath(dir, szPattern);
	WIN32_FIND_DATA ff;
	HANDLE h = FindFirstFile(pattern.c_str(), &ff);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			String filename = ff.cFileName;
			if (szExt)
			{
				// caller specified a specific extension
				// (This is really a workaround for brokenness in windows, which
				//  doesn't screen correctly on extension in pattern)
				String ext = filename.substr(filename.length() - extlen);
				if (string_compare_nocase(szExt, ext) != 0)
					return;
			}

			String filterpath = paths_ConcatPath(dir, ff.cFileName);
			AddFilter(filterpath.c_str());
		} while (FindNextFile(h, &ff));
		FindClose(h);
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
	vector<FileFilter*>::iterator iter = m_filters.begin();
	while (iter != m_filters.end())
	{
		if (string_compare_nocase((*iter)->fullpath, szFilterFile) == 0)
		{
			delete (*iter);
			m_filters.erase(iter);
			break;
		}
		++iter;
	}
}

/**
 * @brief Removes all filters from current list.
 */
void FileFilterMgr::DeleteAllFilters()
{
	while (!m_filters.empty())
	{
		FileFilter* filter = m_filters.back();
		delete filter;
		m_filters.pop_back();
	}
}

/**
 * @brief Add a single pattern (if nonempty & valid) to a pattern list.
 *
 * @param [in] filterList List where pattern is added.
 * @param [in] str Temporary variable (ie, it may be altered)
 */
static void AddFilterPattern(vector<FileFilterElement*> *filterList, String & str)
{
	LPCTSTR commentLeader = _T("##"); // Starts comment
	str = string_trim_ws_begin(str);

	// Ignore lines beginning with '##'
	size_t pos = str.find(commentLeader);
	if (pos == 0)
		return;

	// Find possible comment-separator '<whitespace>##'
	while (pos != std::string::npos && !_istspace(str[pos - 1]))
		pos = str.find(commentLeader, pos + 1);

	// Remove comment and whitespaces before it
	if (pos != std::string::npos)
		str = str.substr(0, pos);
	str = string_trim_ws_end(str);
	if (str.empty())
		return;

	const char * errormsg = NULL;
	int erroroffset = 0;
	char regexString[200] = {0};
	int regexLen = 0;
	int pcre_opts = 0;

#ifdef UNICODE
	// For unicode builds, use UTF-8.
	// Convert pattern to UTF-8 and set option for PCRE to specify UTF-8.
	regexLen = TransformUcs2ToUtf8(str.c_str(), str.length(),
		regexString, (int)sizeof(regexString));
	pcre_opts |= PCRE_UTF8;
#else
	strcpy(regexString, str.c_str());
	regexLen = strlen(regexString);
#endif
	pcre_opts |= PCRE_CASELESS;
	
	pcre *regexp = pcre_compile(regexString, pcre_opts, &errormsg,
		&erroroffset, NULL);
	if (regexp)
	{
		FileFilterElement *elem = new FileFilterElement();
		errormsg = NULL;

		pcre_extra *pe = pcre_study(regexp, 0, &errormsg);
		elem->pRegExp = regexp;
		
		if (pe != NULL && errormsg != NULL)
			elem->pRegExpExtra = pe;
		
		filterList->push_back(elem);
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

	String sLine;
	bool lossy = false;
	bool bLinesLeft = true;
	do
	{
		// Returns false when last line is read
		String tmpLine;
		bLinesLeft = file.ReadString(tmpLine, &lossy);
		sLine = tmpLine;
		sLine = string_trim_ws(sLine);

		if (0 == _tcsncmp(sLine.c_str(), _T("name:"), 5))
		{
			// specifies display name
			String str = sLine.substr(5);
			str = string_trim_ws_begin(str);
			if (!str.empty())
				pfilter->name = str;
		}
		else if (0 == _tcsncmp(sLine.c_str(), _T("desc:"), 5))
		{
			// specifies display name
			String str = sLine.substr(5);
			str = string_trim_ws_begin(str);
			if (!str.empty())
				pfilter->description = str;
		}
		else if (0 == _tcsncmp(sLine.c_str(), _T("def:"), 4))
		{
			// specifies default
			String str = sLine.substr(4);
			str = string_trim_ws_begin(str);
			if (str == _T("0") || str == _T("no") || str == _T("exclude"))
				pfilter->default_include = false;
			else if (str == _T("1") || str == _T("yes") || str == _T("include"))
				pfilter->default_include = true;
		}
		else if (0 == _tcsncmp(sLine.c_str(), _T("f:"), 2))
		{
			// file filter
			String str = sLine.substr(2);
			AddFilterPattern(&pfilter->filefilters, str);
		}
		else if (0 == _tcsncmp(sLine.c_str(), _T("d:"), 2))
		{
			// directory filter
			String str = sLine.substr(2);
			AddFilterPattern(&pfilter->dirfilters, str);
		}
	} while (bLinesLeft);

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
	vector<FileFilter*>::const_iterator iter = m_filters.begin();
	while (iter != m_filters.end())
	{
		if (string_compare_nocase((*iter)->fullpath, szFilterPath) == 0)
			return (*iter);
		++iter;
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
BOOL TestAgainstRegList(const vector<FileFilterElement*> *filterList, LPCTSTR szTest)
{
	vector<FileFilterElement*>::const_iterator iter = filterList->begin();
	while (iter != filterList->end())
	{
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

		pcre * regexp = (*iter)->pRegExp;
		pcre_extra * extra = (*iter)->pRegExpExtra;
		int result = pcre_exec(regexp, extra, compString, stringLen,
			0, 0, ovector, 30);

		free(tempName);

		if (result >= 0)
			return TRUE;

		++iter;
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
	if (TestAgainstRegList(&pFilter->filefilters, szFileName))
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
	if (TestAgainstRegList(&pFilter->dirfilters, szDirName))
		return !pFilter->default_include;
	return pFilter->default_include;
}

/**
 * @brief Return name of filter.
 *
 * @param [in] i Index of filter.
 * @return Name of filter in given index.
 */
String FileFilterMgr::GetFilterName(int i) const
{
	return m_filters[i]->name; 
}

/**
 * @brief Return name of filter.
 * @param [in] pFilter Filter to get name for.
 * @return Given filter's name.
 */
String FileFilterMgr::GetFilterName(const FileFilter *pFilter) const
{
	return pFilter->name; 
}

/**
 * @brief Return description of filter.
 *
 * @param [in] i Index of filter.
 * @return Description of filter in given index.
 */
String FileFilterMgr::GetFilterDesc(int i) const
{
	return m_filters[i]->description; 
}

/**
 * @brief Return description of filter.
 * @param [in] pFilter Filter to get description for.
 * @return Given filter's description.
 */
String FileFilterMgr::GetFilterDesc(const FileFilter *pFilter) const
{
	return pFilter->description;
}

/**
 * @brief Return full path to filter.
 *
 * @param [in] i Index of filter.
 * @return Full path of filter in given index.
 */
String FileFilterMgr::GetFilterPath(int i) const
{
	return m_filters[i]->fullpath;
}

/**
 * @brief Return full path to filter.
 *
 * @param [in] pFilter Pointer to filter.
 * @return Full path of filter.
 */
String FileFilterMgr::GetFullpath(FileFilter * pfilter) const
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
	FileFilter * newfilter = LoadFilterFile(pfilter->fullpath.c_str(), errorcode);

	if (newfilter == NULL)
	{
		return errorcode;
	}

	vector<FileFilter*>::iterator iter = m_filters.begin();
	while (iter != m_filters.end())
	{
		if (pfilter == (*iter))
		{
			delete (*iter);
			m_filters.erase(iter);
			break;
		}
	}
	m_filters.push_back(newfilter);
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
