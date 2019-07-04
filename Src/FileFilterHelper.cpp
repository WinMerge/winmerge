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

#include "pch.h"
#include "FileFilterHelper.h"
#include "UnicodeString.h"
#include "FilterList.h"
#include "DirItem.h"
#include "FileFilterMgr.h"
#include "paths.h"
#include "Environment.h"
#include "unicoder.h"

using std::vector;

/** 
 * @brief Constructor, creates new filtermanager.
 */
FileFilterHelper::FileFilterHelper()
: m_pMaskFilter(nullptr)
, m_bUseMask(true)
, m_fileFilterMgr(new FileFilterMgr)
, m_currentFilter(nullptr)
{
}

/** 
 * @brief Destructor, deletes filtermanager.
 */
FileFilterHelper::~FileFilterHelper()
{
}

/**
 * @brief Store current filter path.
 *
 * Select filter based on filepath. If filter with that path
 * is found select it. Otherwise set path to empty (default).
 * @param [in] szFileFilterPath Full path to filter to select.
 */
void FileFilterHelper::SetFileFilterPath(const String& szFileFilterPath)
{
	// Use none as default path
	m_sFileFilterPath.clear();

	if (m_fileFilterMgr == nullptr)
		return;

	// Don't bother to lookup empty path
	if (!szFileFilterPath.empty())
	{
		m_currentFilter = m_fileFilterMgr->GetFilterByPath(szFileFilterPath);
		if (m_currentFilter != nullptr)
			m_sFileFilterPath = szFileFilterPath;
	}
}

/**
 * @brief Get list of filters currently available.
 *
 * @param [out] selected Filepath of currently selected filter.
 * @return Filter list to receive found filters.
 */
std::vector<FileFilterInfo> FileFilterHelper::GetFileFilters(String & selected) const
{
	std::vector<FileFilterInfo> filters;
	if (m_fileFilterMgr != nullptr)
	{
		const int count = m_fileFilterMgr->GetFilterCount();
		filters.reserve(count);
		for (int i = 0; i < count; ++i)
		{
			FileFilterInfo filter;
			filter.fullpath = m_fileFilterMgr->GetFilterPath(i);
			filter.name = m_fileFilterMgr->GetFilterName(i);
			filter.description = m_fileFilterMgr->GetFilterDesc(i);
			filters.push_back(filter);
		}
	}
	selected = m_sFileFilterPath;
	return filters;
}

/**
 * @brief Return name of filter in given file.
 * If no filter cannot be found, return empty string.
 * @param [in] filterPath Path to filterfile.
 * @sa FileFilterHelper::GetFileFilterPath()
 */
String FileFilterHelper::GetFileFilterName(const String& filterPath) const
{
	String selected;
	String name;
	vector<FileFilterInfo> filters = GetFileFilters(selected);
	vector<FileFilterInfo>::const_iterator iter = filters.begin();
	while (iter != filters.end())
	{
		if ((*iter).fullpath == filterPath)
		{
			name = (*iter).name;
			break;
		}
		++iter;
	}
	return name;
}

/** 
 * @brief Return path to filter with given name.
 * @param [in] filterName Name of filter.
 * @sa FileFilterHelper::GetFileFilterName()
 */
String FileFilterHelper::GetFileFilterPath(const String& filterName) const
{
	String selected;
	String path;
	vector<FileFilterInfo> filters = GetFileFilters(selected);
	vector<FileFilterInfo>::const_iterator iter = filters.begin();
	while (iter != filters.end())
	{
		if ((*iter).name == filterName)
		{
			path = (*iter).fullpath;
			break;
		}
		++iter;
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
	paths::normalize(m_sUserSelFilterPath);
}

/** 
 * @brief Select between mask and filterfile.
 * @param [in] bUseMask If true we use mask instead of filter files.
 */
void FileFilterHelper::UseMask(bool bUseMask)
{
	m_bUseMask = bUseMask;
	if (m_bUseMask)
	{
		if (m_pMaskFilter == nullptr)
		{
			m_pMaskFilter.reset(new FilterList);
		}
	}
	else
	{
		m_pMaskFilter.reset();
	}
}

/** 
 * @brief Set filemask for filtering.
 * @param [in] strMask Mask to set (e.g. *.cpp;*.h).
 */
void FileFilterHelper::SetMask(const String& strMask)
{
	if (!m_bUseMask)
	{
		throw "Filter mask tried to set when masks disabled!";
	}
	m_sMask = strMask;
	String regExp = ParseExtensions(strMask);

	std::string regexp_str = ucr::toUTF8(regExp);

	m_pMaskFilter->RemoveAllFilters();
	m_pMaskFilter->AddRegExp(regexp_str);
}

/**
 * @brief Check if any of filefilter rules match to filename.
 *
 * @param [in] szFileName Filename to test.
 * @return true unless we're suppressing this file by filter
 */
bool FileFilterHelper::includeFile(const String& szFileName) const
{
	if (m_bUseMask)
	{
		if (m_pMaskFilter == nullptr)
		{
			throw "Use mask set, but no filter rules for mask!";
		}

		// preprend a backslash if there is none
		String strFileName = strutils::makelower(szFileName);
		if (strFileName.empty() || strFileName[0] != '\\')
			strFileName = _T("\\") + strFileName;
		// append a point if there is no extension
		if (strFileName.find('.') == String::npos)
			strFileName = strFileName + _T(".");

		return m_pMaskFilter->Match(ucr::toUTF8(strFileName));
	}
	else
	{
		if (m_fileFilterMgr == nullptr || m_currentFilter ==nullptr)
			return true;
		return m_fileFilterMgr->TestFileNameAgainstFilter(m_currentFilter, szFileName);
	}
}

/**
 * @brief Check if any of filefilter rules match to directoryname.
 *
 * @param [in] szFileName Directoryname to test.
 * @return true unless we're suppressing this directory by filter
 */
bool FileFilterHelper::includeDir(const String& szDirName) const
{
	if (m_bUseMask)
	{
		// directories have no extension
		return true; 
	}
	else
	{
		if (m_fileFilterMgr == nullptr || m_currentFilter == nullptr)
			return true;

		// Add a backslash
		String strDirName(_T("\\"));
		strDirName += szDirName;

		return m_fileFilterMgr->TestDirNameAgainstFilter(m_currentFilter, strDirName);
	}
}

/**
 * @brief Load in all filters in a folder.
 * @param [in] dir Folder from where to load filters.
 * @param [in] sPattern Wildcard defining files to add to map as filter files.
 *   It is filemask, for example, "*.flt"
 */
void FileFilterHelper::LoadFileFilterDirPattern(const String& dir, const String& szPattern)
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
	bool bFilterAdded = false;
	static const TCHAR pszSeps[] = _T(" ;|,:");

	ext += _T(";"); // Add one separator char to end
	size_t pos = ext.find_first_of(pszSeps);
	
	while (pos != String::npos)
	{
		String token = ext.substr(0, pos); // Get first extension
		ext = ext.substr(pos + 1); // Remove extension + separator
		
		// Only "*." or "*.something" allowed, other ignored
		if (token.length() >= 1)
		{
			bFilterAdded = true;
			String strRegex = token;
			strutils::replace(strRegex, _T("."), _T("\\."));
			strutils::replace(strRegex, _T("?"), _T("."));
			strutils::replace(strRegex, _T("("), _T("\\("));
			strutils::replace(strRegex, _T(")"), _T("\\)"));
			strutils::replace(strRegex, _T("["), _T("\\["));
			strutils::replace(strRegex, _T("]"), _T("\\]"));
			strutils::replace(strRegex, _T("$"), _T("\\$"));
			strutils::replace(strRegex, _T("*"), _T(".*"));
			strRegex += _T("$");
			strPattern += _T("(^|\\\\)") + strRegex;
		}
		else
			bFilterAdded = false;

		pos = ext.find_first_of(pszSeps); 
		if (bFilterAdded && pos != String::npos && extensions.length() > 1)
			strPattern += _T("|");
	}

	if (strPattern.empty())
		strParsed = _T(".*"); // Match everything
	else
	{

		strPattern = strutils::makelower(strPattern);
		strParsed = strPattern; //+ _T("$");
	}
	return strParsed;
}

/** 
 * @brief Returns active filter (or mask string)
 * @return The active filter.
 */
String FileFilterHelper::GetFilterNameOrMask() const
{
	String sFilter;

	if (!IsUsingMask())
		sFilter = GetFileFilterName(m_sFileFilterPath);
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
 * @return true if given filter was set, false if default filter was set.
 * @note If function returns false, you should ask filter set with
 * GetFilterNameOrMask().
 */
bool FileFilterHelper::SetFilter(const String &filter)
{
	// If filter is empty string set default filter
	if (filter.empty())
	{
		UseMask(true);
		SetMask(_T("*.*"));
		SetFileFilterPath(_T(""));
		return false;
	}

	// Remove leading and trailing whitespace characters from the string.
	String flt = strutils::trim_ws(filter);

	// Star means we have a file extension mask
	if (filter.find_first_of(_T("*?")) != -1)
	{
		UseMask(true);
		SetMask(flt);
		SetFileFilterPath(_T(""));
	}
	else
	{
		String path = GetFileFilterPath(flt);
		if (!path.empty())
		{
			UseMask(false);
			SetFileFilterPath(path);
		}
		// If filter not found with given name, use default filter
		else
		{
			UseMask(true);
			SetMask(_T("*.*"));
			SetFileFilterPath(_T(""));
			return false;
		}
	}
	return true;
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
	DirItem fileInfo;
	String selected;
	vector<FileFilterInfo> filters = GetFileFilters(selected);
	vector<FileFilterInfo>::const_iterator iter = filters.begin();
	while (iter != filters.end())
	{
		String path = (*iter).fullpath;

		fileInfo.Update(path);
		if (fileInfo.mtime != (*iter).fileinfo.mtime ||
			fileInfo.size != (*iter).fileinfo.size)
		{
			// Reload filter after changing it
			int retval = m_fileFilterMgr->ReloadFilterFromDisk(path);
			
			if (retval == FILTER_OK)
			{
				// If it was active filter we have to re-set it
				if (path == selected)
					SetFileFilterPath(path);
			}
		}
		++iter;
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
	m_sGlobalFilterPath = paths::ConcatPath(env::GetProgPath(), _T("Filters"));
	paths::normalize(m_sGlobalFilterPath);
	String pattern(_T("*"));
	pattern += FileFilterExt;
	LoadFileFilterDirPattern(m_sGlobalFilterPath, pattern);
	if (strutils::compare_nocase(m_sGlobalFilterPath, m_sUserSelFilterPath) != 0)
		LoadFileFilterDirPattern(m_sUserSelFilterPath, pattern);
}

/**
 * @brief Return path to global filters (& create if needed), or empty if cannot create
 */
String FileFilterHelper::GetGlobalFilterPathWithCreate() const
{
	return paths::EnsurePathExist(m_sGlobalFilterPath);
}

/**
 * @brief Return path to user filters (& create if needed), or empty if cannot create
 */
String FileFilterHelper::GetUserFilterPathWithCreate() const
{
	return paths::EnsurePathExist(m_sUserSelFilterPath);
}

