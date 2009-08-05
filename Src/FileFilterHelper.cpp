/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  FileFilterHelper.cpp
 *
 * @brief Implementation file for FileFilterHelper class
 */
// ID line follows -- this is updated by SVN
// $Id$

#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include "Ucs2Utf8.h"
#include "UnicodeString.h"
#include "FilterList.h"
#include "DirItem.h"
#include "FileFilterMgr.h"
#include "FileFilterHelper.h"
#include "Coretools.h"
#include "paths.h"

using std::vector;

/** 
 * @brief Constructor, creates new filtermanager.
 */
FileFilterHelper::FileFilterHelper()
: m_pMaskFilter(NULL)
, m_bUseMask(TRUE)
{
	m_fileFilterMgr = new FileFilterMgr;
}

/** 
 * @brief Destructor, deletes filtermanager.
 */
FileFilterHelper::~FileFilterHelper()
{
	delete m_fileFilterMgr;
	delete m_pMaskFilter;
}

/** 
 * @brief Return filtermanager used.
 */
FileFilterMgr * FileFilterHelper::GetManager() const
{
	return m_fileFilterMgr;
}

/**
 * @brief Store current filter path.
 *
 * Select filter based on filepath. If filter with that path
 * is found select it. Otherwise set path to empty (default).
 * @param [in] szFileFilterPath Full path to filter to select.
 */
void FileFilterHelper::SetFileFilterPath(LPCTSTR szFileFilterPath)
{
	// Use none as default path
	m_sFileFilterPath.clear();

	if (!m_fileFilterMgr)
		return;

	// Don't bother to lookup empty path
	if (_tcslen(szFileFilterPath) > 0)
	{
		m_currentFilter = m_fileFilterMgr->GetFilterByPath(szFileFilterPath);
		if (m_currentFilter)
			m_sFileFilterPath = szFileFilterPath;
	}
}

/**
 * @brief Get list of filters currently available.
 *
 * @param [out] filters Filter list to receive found filters.
 * @param [out] selected Filepath of currently selected filter.
 */
void FileFilterHelper::GetFileFilters(std::vector<FileFilterInfo> * filters, String & selected) const
{
	if (m_fileFilterMgr)
	{
		const int count = m_fileFilterMgr->GetFilterCount();
		filters->reserve(count);
		for (int i = 0; i < count; ++i)
		{
			FileFilterInfo filter;
			filter.fullpath = m_fileFilterMgr->GetFilterPath(i);
			filter.name = m_fileFilterMgr->GetFilterName(i);
			filter.description = m_fileFilterMgr->GetFilterDesc(i);
			filters->push_back(filter);
		}
	}
	selected = m_sFileFilterPath;
}

/**
 * @brief Return name of filter in given file.
 * If no filter cannot be found, return empty string.
 * @param [in] filterPath Path to filterfile.
 * @sa FileFilterHelper::GetFileFilterPath()
 */
String FileFilterHelper::GetFileFilterName(LPCTSTR filterPath) const
{
	vector<FileFilterInfo> filters;
	String selected;
	String name;

	GetFileFilters(&filters, selected);
	vector<FileFilterInfo>::const_iterator iter = filters.begin();
	while (iter != filters.end())
	{
		if ((*iter).fullpath == filterPath)
		{
			name = (*iter).name;
			break;
		}
		iter++;
	}
	return name;
}

/** 
 * @brief Return path to filter with given name.
 * @param [in] filterName Name of filter.
 * @sa FileFilterHelper::GetFileFilterName()
 */
String FileFilterHelper::GetFileFilterPath(LPCTSTR filterName) const
{
	vector<FileFilterInfo> filters;
	String selected;
	String path;

	GetFileFilters(&filters, selected);
	vector<FileFilterInfo>::const_iterator iter = filters.begin();
	while (iter != filters.end())
	{
		if ((*iter).name == filterName)
		{
			path = (*iter).fullpath;
			break;
		}
		iter++;
	}
	return path;
}

/** 
 * @brief Set User's filter folder.
 * @param [in] filterPath Location of User's filters.
 */
void FileFilterHelper::SetUserFilterPath(const String & filterPath)
{
	m_sUserSelFilterPath = filterPath;
	if (filterPath[filterPath.length() - 1] != '\\')
		m_sUserSelFilterPath += L"\\";
}

/** 
 * @brief Select between mask and filterfile.
 * @param [in] bUseMask If TRUE we use mask instead of filter files.
 */
void FileFilterHelper::UseMask(BOOL bUseMask)
{
	m_bUseMask = bUseMask;
	if (m_bUseMask)
	{
		if (m_pMaskFilter == NULL)
		{
			m_pMaskFilter = new FilterList;
		}
	}
	else
	{
		delete m_pMaskFilter;
		m_pMaskFilter = NULL;
	}
}

/** 
 * @brief Set filemask for filtering.
 * @param [in] strMask Mask to set (e.g. *.cpp;*.h).
 */
void FileFilterHelper::SetMask(LPCTSTR strMask)
{
	if (!m_bUseMask)
	{
		_RPTF0(_CRT_ERROR, "Filter mask tried to set when masks disabled!");
		return;
	}
	m_sMask = strMask;
	String regExp = ParseExtensions(strMask);

	char * regexp_str;
	FilterList::EncodingType type;

#ifdef UNICODE
	regexp_str = UCS2UTF8_ConvertToUtf8(regExp.c_str());
	type = FilterList::ENC_UTF8;
#else
	regexp_str = &*regExp.begin();
	type = FilterList::ENC_ANSI;
#endif

	m_pMaskFilter->RemoveAllFilters();
	m_pMaskFilter->AddRegExp(regexp_str, type);

#ifdef UNICODE
	UCS2UTF8_Dealloc(regexp_str);
#endif
}

/**
 * @brief Check if any of filefilter rules match to filename.
 *
 * @param [in] szFileName Filename to test.
 * @return TRUE unless we're suppressing this file by filter
 */
BOOL FileFilterHelper::includeFile(LPCTSTR szFileName)
{
	if (m_bUseMask)
	{
		if (m_pMaskFilter == NULL)
		{
			_RPTF0(_CRT_ERROR, "Use mask set, but no filter rules for mask!");
			return TRUE;
		}

		// preprend a backslash if there is none
		String strFileName = szFileName;
		strFileName = string_makelower(strFileName);
		if (strFileName[0] != '\\')
			strFileName = L"\\" + strFileName;
		// append a point if there is no extension
		if (strFileName.find('.') == -1)
			strFileName = strFileName + L".";

		char * name_utf = UCS2UTF8_ConvertToUtf8(strFileName.c_str());
		bool match = m_pMaskFilter->Match(name_utf);
		UCS2UTF8_Dealloc(name_utf);
		return match;
	}
	else
	{
		if (!m_fileFilterMgr || !m_currentFilter)
			return TRUE;
		return m_fileFilterMgr->TestFileNameAgainstFilter(m_currentFilter, szFileName);
	}
}

/**
 * @brief Check if any of filefilter rules match to directoryname.
 *
 * @param [in] szFileName Directoryname to test.
 * @return TRUE unless we're suppressing this directory by filter
 */
BOOL FileFilterHelper::includeDir(LPCTSTR szDirName)
{
	if (m_bUseMask)
	{
		// directories have no extension
		return TRUE; 
	}
	else
	{
		if (!m_fileFilterMgr || !m_currentFilter)
			return TRUE;

		// Add a backslash
		String strDirName(_T("\\"));
		strDirName += szDirName;

		return m_fileFilterMgr->TestDirNameAgainstFilter(m_currentFilter, strDirName.c_str());
	}
}

/**
 * @brief Load in all filters in a folder.
 * @param [in] dir Folder from where to load filters.
 * @param [in] sPattern Wildcard defining files to add to map as filter files.
 *   It is filemask, for example, "*.flt"
 */
void FileFilterHelper::LoadFileFilterDirPattern(LPCTSTR dir, LPCTSTR szPattern)
{
	m_fileFilterMgr->LoadFromDirectory(dir, szPattern, FileFilterExt);
}

/** 
 * @brief Convert user-given extension list to valid regular expression.
 * @param [in] Extension list/mask to convert to regular expression.
 * @return Regular expression that matches extension list.
 */
String FileFilterHelper::ParseExtensions(const String &extensions) const
{
	String strParsed;
	String strPattern;
	String ext(extensions);
	BOOL bFilterAdded = FALSE;
	static const TCHAR pszSeps[] = _T(" ;|,:");

	ext += _T(";"); // Add one separator char to end
	size_t pos = ext.find_first_of(pszSeps);
	
	while (pos != String::npos)
	{
		String token = ext.substr(0, pos); // Get first extension
		ext = ext.substr(pos + 1); // Remove extension + separator
		
		// Only "*." or "*.something" allowed, other ignored
		if (token.length() >= 2 && token[0] == '*' && token[1] == '.')
		{
			bFilterAdded = TRUE;
			strPattern += _T(".*\\.");
			strPattern += token.substr(2);
			strPattern += _T("$");
		}
		else
			bFilterAdded = FALSE;

		pos = ext.find_first_of(pszSeps); 
		if (bFilterAdded && pos >= 0)
			strPattern += _T("|");
	}

	if (strPattern.empty())
		strParsed = _T(".*"); // Match everything
	else
	{
		strParsed = _T("^");

		strPattern = string_makelower(strPattern);
		strParsed = strPattern; //+ _T("$");
	}
	return strParsed;
}

/** 
 * @brief Returns TRUE if active filter is a mask.
 */
BOOL FileFilterHelper::IsUsingMask() const
{
	return m_bUseMask;
}

/** 
 * @brief Returns active filter (or mask string)
 * @return The active filter.
 */
String FileFilterHelper::GetFilterNameOrMask() const
{
	String sFilter;

	if (!IsUsingMask())
		sFilter = GetFileFilterName(m_sFileFilterPath.c_str());
	else
		sFilter = m_sMask;

	return sFilter;
}

/** 
 * @brief Set filter.
 *
 * Simple-to-use function to select filter. This function determines
 * filter type so caller doesn't need to care about it.
 *
 * @param [in] filter File mask or filter name.
 * @return TRUE if given filter was set, FALSE if default filter was set.
 * @note If function returns FALSE, you should ask filter set with
 * GetFilterNameOrMask().
 */
BOOL FileFilterHelper::SetFilter(const String &filter)
{
	// If filter is empty string set default filter
	if (filter.empty())
	{
		UseMask(TRUE);
		SetMask(_T("*.*"));
		SetFileFilterPath(_T(""));
		return FALSE;
	}

	// Remove leading and trailing whitespace characters from the string.
	String flt = string_trim_ws(filter);

	// Star means we have a file extension mask
	if (flt[0] == '*')
	{
		UseMask(TRUE);
		SetMask(flt.c_str());
		SetFileFilterPath(_T(""));
	}
	else
	{
		String path = GetFileFilterPath(flt.c_str());
		if (!path.empty())
		{
			UseMask(FALSE);
			SetFileFilterPath(path.c_str());
		}
		// If filter not found with given name, use default filter
		else
		{
			UseMask(TRUE);
			SetMask(_T("*.*"));
			SetFileFilterPath(_T(""));
			return FALSE;
		}
	}
	return TRUE;
}

/** 
 * @brief Reloads changed filter files
 *
 * Checks if filter file has been modified since it was last time
 * loaded/reloaded. If file has been modified we reload it.
 * @todo How to handle an error in reloading filter?
 */
void FileFilterHelper::ReloadUpdatedFilters()
{
	vector<FileFilterInfo> filters;
	DirItem fileInfo;
	String selected;

	GetFileFilters(&filters, selected);
	vector<FileFilterInfo>::const_iterator iter = filters.begin();
	while (iter != filters.end())
	{
		String path = (*iter).fullpath;

		fileInfo.Update(path);
		if (fileInfo.mtime != (*iter).fileinfo.mtime ||
			fileInfo.size != (*iter).fileinfo.size)
		{
			// Reload filter after changing it
			int retval = m_fileFilterMgr->ReloadFilterFromDisk(path.c_str());
			
			if (retval == FILTER_OK)
			{
				// If it was active filter we have to re-set it
				if (path == selected)
					SetFileFilterPath(path.c_str());
			}
		}
		iter++;
	}
}

/**
 * @brief Load any known file filters
 * @todo Preserve filter selection? How?
 */
void FileFilterHelper::LoadAllFileFilters()
{
	// First delete existing filters
	m_fileFilterMgr->DeleteAllFilters();

	// Program application directory
	m_sGlobalFilterPath = GetModulePath() + L"\\Filters";
	String pattern(L"*");
	pattern += FileFilterExt;
	LoadFileFilterDirPattern(m_sGlobalFilterPath.c_str(), pattern.c_str());
	LoadFileFilterDirPattern(m_sUserSelFilterPath.c_str(), pattern.c_str());
}

/**
 * @brief Return path to global filters (& create if needed), or empty if cannot create
 */
String FileFilterHelper::GetGlobalFilterPathWithCreate() const
{
	return paths_EnsurePathExist(m_sGlobalFilterPath);
}

/**
 * @brief Return path to user filters (& create if needed), or empty if cannot create
 */
String FileFilterHelper::GetUserFilterPathWithCreate() const
{
	return paths_EnsurePathExist(m_sUserSelFilterPath);
}

