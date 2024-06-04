// SPDX-License-Identifier: GPL-2.0-or-later
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
: m_pMaskFileFilter(nullptr)
, m_pMaskDirFilter(nullptr)
, m_bUseMask(true)
, m_fileFilterMgr(new FileFilterMgr)
, m_currentFilter(nullptr)
{
}

/** 
 * @brief Destructor, deletes filtermanager.
 */
FileFilterHelper::~FileFilterHelper() = default;

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
		if (m_pMaskFileFilter == nullptr)
		{
			m_pMaskFileFilter.reset(new FilterList);
		}
		if (m_pMaskDirFilter == nullptr)
		{
			m_pMaskDirFilter.reset(new FilterList);
		}
	}
	else
	{
		m_pMaskFileFilter.reset();
		m_pMaskDirFilter.reset();
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
	auto [regExpFile, regExpFileExclude, regExpDir, regExpDirExclude] = ParseExtensions(strMask);

	std::string regexp_str_file = ucr::toUTF8(regExpFile);
	std::string regexp_str_file_excluded = ucr::toUTF8(regExpFileExclude);
	std::string regexp_str_dir = ucr::toUTF8(regExpDir);
	std::string regexp_str_dir_excluded = ucr::toUTF8(regExpDirExclude);

	m_pMaskFileFilter->RemoveAllFilters();
	m_pMaskFileFilter->AddRegExp(regexp_str_file, false);
	if (!regexp_str_file_excluded.empty())
		m_pMaskFileFilter->AddRegExp(regexp_str_file_excluded, true);
	m_pMaskDirFilter->RemoveAllFilters();
	m_pMaskDirFilter->AddRegExp(regexp_str_dir, false);
	if (!regexp_str_dir_excluded.empty())
		m_pMaskDirFilter->AddRegExp(regexp_str_dir_excluded, true);
}

static String addPeriodIfNoExtension(const String& path)
{
	String ret, elm;
	bool period = false;
	for (auto ch : path)
	{
		if (ch == '.')
		{
			elm += ch;
			period = true;
		}
		else if (ch == '\\')
		{
			if (!period && !elm.empty() && elm.back() != '*')
				elm += '.';
			elm += ch;
			ret += elm;
			elm.clear();
			period = false;
		}
		else
		{
			elm += ch;
		}
	}
	if (!period && !elm.empty())
		elm += '.';
	ret += elm;
	return ret;
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
		if (m_pMaskFileFilter == nullptr)
		{
			throw "Use mask set, but no filter rules for mask!";
		}

		// preprend a backslash if there is none
		String strFileName = strutils::makelower(szFileName);
		if (strFileName.empty() || strFileName[0] != '\\')
			strFileName = _T("\\") + strFileName;
		// append a point if there is no extension
		strFileName = addPeriodIfNoExtension(strFileName);

		return m_pMaskFileFilter->Match(ucr::toUTF8(strFileName));
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
		if (m_pMaskDirFilter == nullptr)
		{
			throw "Use mask set, but no filter rules for mask!";
		}

		// preprend a backslash if there is none
		String strDirName = strutils::makelower(szDirName);
		if (strDirName.empty() || strDirName[0] != '\\')
			strDirName = _T("\\") + strDirName;
		// append a point if there is no extension
		strDirName = addPeriodIfNoExtension(strDirName);

		return m_pMaskDirFilter->Match(ucr::toUTF8(strDirName));
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

static String ConvertWildcardPatternToRegexp(const String& pattern)
{
	String strRegex;
	for (const tchar_t *p = pattern.c_str(); *p; ++p)
	{
		switch (*p)
		{
		case '\\': strRegex += _T("\\\\");     break;
		case '.':  strRegex += _T("\\.");      break;
		case '[':  strRegex += _T("\\[");      break;
		case ']':  strRegex += _T("\\]");      break;
		case '(':  strRegex += _T("\\(");      break;
		case ')':  strRegex += _T("\\)");      break;
		case '$':  strRegex += _T("\\$");      break;
		case '^':  strRegex += _T("\\^");      break;
		case '+':  strRegex += _T("\\+");      break;
		case '?':  strRegex += _T("[^\\\\]");  break;
		case '*':  strRegex += _T("[^\\\\]*"); break;
		default:   strRegex += *p;             break;
		}
	}
	strRegex += _T("$");
	return _T("(^|\\\\)") + strRegex;
}

/** 
 * @brief Convert user-given extension list to valid regular expression.
 * @param [in] Extension list/mask to convert to regular expression.
 * @return Regular expression that matches extension list.
 */
std::tuple<String, String, String, String> FileFilterHelper::ParseExtensions(const String &extensions) const
{
	String strFileParsed;
	String strDirParsed;
	std::vector<String> filePatterns;
	std::vector<String> filePatternsExclude;
	std::vector<String> dirPatterns;
	std::vector<String> dirPatternsExclude;
	String ext(extensions);
	static const tchar_t pszSeps[] = _T(" ;|,:");

	ext += _T(";"); // Add one separator char to end
	size_t pos = ext.find_first_of(pszSeps);
	
	while (pos != String::npos)
	{
		String token = ext.substr(0, pos); // Get first extension
		ext = ext.substr(pos + 1); // Remove extension + separator
		
		// Only "*." or "*.something" allowed, other ignored
		if (token.length() >= 1)
		{
			bool exclude = token[0] == '!';
			if (exclude)
				token = token.substr(1);
			bool isdir = token.back() == '\\';
			if (isdir)
				token = token.substr(0, token.size() - 1);
			token = addPeriodIfNoExtension(token);
			String strRegex = strutils::makelower(ConvertWildcardPatternToRegexp(token));
			if (exclude)
			{
				if (isdir)
					dirPatternsExclude.push_back(strRegex);
				else
					filePatternsExclude.push_back(strRegex);
			}
			else
			{
				if (isdir)
					dirPatterns.push_back(strRegex);
				else
					filePatterns.push_back(strRegex);
			}
		}

		pos = ext.find_first_of(pszSeps); 
	}

	if (filePatterns.empty())
		strFileParsed = _T(".*"); // Match everything
	else
		strFileParsed = strutils::join(filePatterns.begin(), filePatterns.end(), _T("|"));
	if (dirPatterns.empty())
		strDirParsed = _T(".*"); // Match everything
	else
		strDirParsed = strutils::join(dirPatterns.begin(), dirPatterns.end(), _T("|"));
	String strFileParsedExclude = strutils::join(filePatternsExclude.begin(), filePatternsExclude.end(), _T("|"));
	String strDirParsedExclude = strutils::join(dirPatternsExclude.begin(), dirPatternsExclude.end(), _T("|"));
	return { strFileParsed, strFileParsedExclude, strDirParsed, strDirParsedExclude };
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

	String path = GetFileFilterPath(flt);
	if (!path.empty())
	{
		UseMask(false);
		SetFileFilterPath(path);
	}
	else
	{
		UseMask(true);
		SetMask(flt);
		SetFileFilterPath(_T(""));
		return false;
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

/**
 * @brief Clone file filter helper from another file filter helper.
 * This function clones file filter helper from another file filter helper.
 * Current contents in the file filter helper are removed and new contents added from the given file filter helper.
 * @param [in] pHelper File filter helper to clone.
 */
void FileFilterHelper::CloneFrom(const FileFilterHelper* pHelper)
{
	if (!pHelper)
		return;

	if (pHelper->m_pMaskFileFilter)
	{
		auto filterList = std::make_unique<FilterList>(FilterList());
		m_pMaskFileFilter = std::move(filterList);
		m_pMaskFileFilter->CloneFrom(pHelper->m_pMaskFileFilter.get());
	}

	if (pHelper->m_pMaskDirFilter)
	{
		auto filterList = std::make_unique<FilterList>(FilterList());
		m_pMaskDirFilter = std::move(filterList);
		m_pMaskDirFilter->CloneFrom(pHelper->m_pMaskDirFilter.get());
	}

	if (pHelper->m_fileFilterMgr)
	{
		auto fileFilterMgr = std::make_unique<FileFilterMgr>(FileFilterMgr());
		m_fileFilterMgr = std::move(fileFilterMgr);
		m_fileFilterMgr->CloneFrom(pHelper->m_fileFilterMgr.get());
	}

	m_currentFilter = nullptr;
	if (pHelper->m_currentFilter && pHelper->m_fileFilterMgr)
	{
		int count = pHelper->m_fileFilterMgr->GetFilterCount();
		for (int i = 0; i < count; i++)
			if (pHelper->m_fileFilterMgr->GetFilterByIndex(i) == pHelper->m_currentFilter)
			{
				m_currentFilter = m_fileFilterMgr->GetFilterByIndex(i);
				break;
			}
	}

	m_sFileFilterPath = pHelper->m_sFileFilterPath;
	m_sMask = pHelper->m_sMask;
	m_bUseMask = pHelper->m_bUseMask;
	m_sGlobalFilterPath = pHelper->m_sGlobalFilterPath;
	m_sUserSelFilterPath = pHelper->m_sUserSelFilterPath;
}
