// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  FileFilterHelper.cpp
 *
 * @brief Implementation file for FileFilterHelper class
 */

#include "pch.h"
#include "FileFilterHelper.h"
#include "FilterExpression.h"
#include "UnicodeString.h"
#include "FilterList.h"
#include "DiffItem.h"
#include "FileFilterMgr.h"
#include "paths.h"
#include "Environment.h"
#include "unicoder.h"

/** 
 * @brief Constructor, creates new filtermanager.
 */
FileFilterHelper::FileFilterHelper()
: m_fileFilterMgr(new FileFilterMgr)
{
}

/** 
 * @brief Destructor, deletes filtermanager.
 */
FileFilterHelper::~FileFilterHelper() = default;

/**
 * @brief Get list of filters currently available.
 *
 * @return Filter list to receive found filters.
 */
std::vector<FileFilterInfo> FileFilterHelper::GetFileFilters() const
{
	std::vector<FileFilterInfo> filters;
	if (!m_fileFilterMgr)
		return filters;
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
	for (const auto& filter : GetFileFilters())
	{
		if (filter.fullpath == filterPath)
			return filter.name;
	}
	return String();
}

/** 
 * @brief Return path to filter with given name.
 * @param [in] filterName Name of filter.
 * @sa FileFilterHelper::GetFileFilterName()
 */
String FileFilterHelper::GetFileFilterPath(const String& filterName) const
{
	for (const auto& filter : GetFileFilters())
	{
		if (filter.name == filterName)
			return filter.fullpath;
	}
	return String();
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
 * @brief Set filemask for filtering.
 * @param [in] strMask Mask to set (e.g. *.cpp;*.h).
 */
void FileFilterHelper::SetMaskOrExpression(const String& strMask)
{
	String flt = strutils::trim_ws(strMask);
	String path = GetFileFilterPath(flt);
	if (!path.empty())
		flt = _T("fp:") + flt;

	m_sMask = flt;
	m_filterGroups = ParseExtensions(m_sMask);
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

void FileFilterHelper::SetDiffContext(const CDiffContext* pCtxt)
{
	for (const auto& filterGroup : m_filterGroups)
	{
		if (filterGroup.m_pRegexOrExpressionFilter)
			filterGroup.m_pRegexOrExpressionFilter->SetDiffContext(pCtxt);
		if (filterGroup.m_pRegexOrExpressionFilterExclude)
			filterGroup.m_pRegexOrExpressionFilterExclude->SetDiffContext(pCtxt);
	}
}

std::vector<const FileFilterErrorInfo*> FileFilterHelper::GetErrorList() const
{
	std::vector<const FileFilterErrorInfo*> list;
	for (const auto& filterGroup : m_filterGroups)
	{
		for (const auto* pfilter : { filterGroup.m_pRegexOrExpressionFilter.get(), filterGroup.m_pRegexOrExpressionFilterExclude.get() })
		{
			if (pfilter)
			{
				for (const auto& error : pfilter->errors)
					list.push_back(&error);
			}
		}
	}
	return list;
}

/**
 * @brief Check if any of filefilter rules match to filename.
 *
 * @param [in] szFileName Filename to test.
 * @return true unless we're suppressing this file by filter
 */
bool FileFilterHelper::includeFile(const String& szFileName) const
{
	// preprend a backslash if there is none
	String strFileName = strutils::makelower(szFileName);
	if (strFileName.empty() || strFileName[0] != '\\')
		strFileName = _T("\\") + strFileName;
	// append a point if there is no extension
	std::string strFileNameUtf8Period = ucr::toUTF8(addPeriodIfNoExtension(strFileName));
	for (const auto& filterGroup : m_filterGroups)
	{
		bool result = filterGroup.m_pMaskFileFilter && filterGroup.m_pMaskFileFilter->Match(strFileNameUtf8Period);
		if (!result)
			result = filterGroup.m_pRegexOrExpressionFilter && TestAgainstRegList(&filterGroup.m_pRegexOrExpressionFilter->filefilters, szFileName);
		if (!result)
			return false;
		if (filterGroup.m_pMaskFileFilterExclude && filterGroup.m_pMaskFileFilterExclude->Match(strFileNameUtf8Period))
			return false;
		if (filterGroup.m_pRegexOrExpressionFilter && TestAgainstRegList(&filterGroup.m_pRegexOrExpressionFilter->filefiltersExclude, szFileName))
			return false;
		if (filterGroup.m_pRegexOrExpressionFilterExclude && !filterGroup.m_pRegexOrExpressionFilterExclude->TestFileNameAgainstFilter(szFileName))
			return false;
	}
	return true;
}

bool FileFilterHelper::includeFile(const DIFFITEM& di) const
{
	const int nDirs = di.diffcode.isThreeway() ? 3 : 2;
	int i = 0;
	for (; i < nDirs; ++i)
	{
		if (!di.diffFileInfo[i].filename.get().empty())
			break;
	}
	std::string strFileNameUtf8Period;
	bool result = true;
	if (i < nDirs)
	{
		String szFileName = paths::ConcatPath(di.diffFileInfo[i].path, di.diffFileInfo[i].filename);
		// preprend a backslash if there is none
		String strFileName = strutils::makelower(szFileName);
		if (strFileName.empty() || strFileName[0] != '\\')
			strFileName = _T("\\") + strFileName;
		// append a point if there is no extension
		strFileNameUtf8Period = ucr::toUTF8(addPeriodIfNoExtension(strFileName));
	}
	for (const auto& filterGroup : m_filterGroups)
	{
		if (i < nDirs)
			result = filterGroup.m_pMaskFileFilter && filterGroup.m_pMaskFileFilter->Match(strFileNameUtf8Period);
		if (!result)
		{
			if (filterGroup.m_pRegexOrExpressionFilter)
			{
				result = FileFilter::TestAgainstRegList(&filterGroup.m_pRegexOrExpressionFilter->filefilters, di);
				if (!result)
					result = FileFilter::TestAgainstExpressionList(&filterGroup.m_pRegexOrExpressionFilter->fileExpressionFilters, di);
			}
		}
		if (!result)
			return false;
		if (i < nDirs)
		{
			if (filterGroup.m_pMaskFileFilterExclude && filterGroup.m_pMaskFileFilterExclude->Match(strFileNameUtf8Period))
				return false;
		}
		if (filterGroup.m_pRegexOrExpressionFilter)
		{
			if (FileFilter::TestAgainstRegList(&filterGroup.m_pRegexOrExpressionFilter->filefiltersExclude, di))
				return false;
			if (FileFilter::TestAgainstExpressionList(&filterGroup.m_pRegexOrExpressionFilter->fileExpressionFiltersExclude, di))
				return false;
		}
		if (filterGroup.m_pRegexOrExpressionFilterExclude && !filterGroup.m_pRegexOrExpressionFilterExclude->TestFileDiffItemAgainstFilter(di))
			return false;
	}
	return true;
}

/**
 * @brief Check if any of filefilter rules match to directoryname.
 *
 * @param [in] szFileName Directoryname to test.
 * @return true unless we're suppressing this directory by filter
 */
bool FileFilterHelper::includeDir(const String& szDirName) const
{
	// preprend a backslash if there is none
	String strDirName = strutils::makelower(szDirName);
	if (strDirName.empty() || strDirName[0] != '\\')
		strDirName = _T("\\") + strDirName;
	// append a point if there is no extension
	std::string strDirNameUtf8Period = ucr::toUTF8(addPeriodIfNoExtension(strDirName));
	for (const auto& filterGroup : m_filterGroups)
	{
		bool result = filterGroup.m_pMaskDirFilter && filterGroup.m_pMaskDirFilter->Match(strDirNameUtf8Period);
		if (!result)
		{
			if (filterGroup.m_pRegexOrExpressionFilter)
				result = TestAgainstRegList(&filterGroup.m_pRegexOrExpressionFilter->dirfilters, strDirName);
		}
		if (!result)
			return false;
		if (filterGroup.m_pMaskDirFilterExclude && filterGroup.m_pMaskDirFilterExclude->Match(strDirNameUtf8Period))
			return false;
		if (filterGroup.m_pRegexOrExpressionFilter && TestAgainstRegList(&filterGroup.m_pRegexOrExpressionFilter->dirfiltersExclude, strDirName))
			return false;
		if (filterGroup.m_pRegexOrExpressionFilterExclude && !filterGroup.m_pRegexOrExpressionFilterExclude->TestDirNameAgainstFilter(strDirName))
			return false;
	}
	return true;
}

bool FileFilterHelper::includeDir(const DIFFITEM& di) const
{
	const int nDirs = di.diffcode.isThreeway() ? 3 : 2;
	int i = 0;
	for (; i < nDirs; ++i)
	{
		if (!di.diffFileInfo[i].filename.get().empty())
			break;
	}
	std::string strDirNameUtf8Period;
	bool result = true;
	if (i < nDirs)
	{
		String szDirName = paths::ConcatPath(di.diffFileInfo[i].path, di.diffFileInfo[i].filename);
		// preprend a backslash if there is none
		String strDirName = strutils::makelower(szDirName);
		if (strDirName.empty() || strDirName[0] != '\\')
			strDirName = _T("\\") + strDirName;
		// append a point if there is no extension
		strDirNameUtf8Period = ucr::toUTF8(addPeriodIfNoExtension(strDirName));
	}
	for (const auto& filterGroup : m_filterGroups)
	{
		if (i < nDirs)
			result = filterGroup.m_pMaskDirFilter && filterGroup.m_pMaskDirFilter->Match(strDirNameUtf8Period);
		if (!result)
		{
			if (filterGroup.m_pRegexOrExpressionFilter)
			{
				result = FileFilter::TestAgainstRegList(&filterGroup.m_pRegexOrExpressionFilter->dirfilters, di);
				if (!result)
					result = FileFilter::TestAgainstExpressionList(&filterGroup.m_pRegexOrExpressionFilter->dirExpressionFilters, di);
			}
		}
		if (!result)
			return false;
		if (i < nDirs)
		{
			if (filterGroup.m_pMaskDirFilterExclude && filterGroup.m_pMaskDirFilterExclude->Match(strDirNameUtf8Period))
				return false;
		}
		if (filterGroup.m_pRegexOrExpressionFilter)
		{

			if (FileFilter::TestAgainstRegList(&filterGroup.m_pRegexOrExpressionFilter->dirfiltersExclude, di))
				return false;
			if (FileFilter::TestAgainstExpressionList(&filterGroup.m_pRegexOrExpressionFilter->dirExpressionFiltersExclude, di))
				return false;
		}
		if (filterGroup.m_pRegexOrExpressionFilterExclude && !filterGroup.m_pRegexOrExpressionFilterExclude->TestDirDiffItemAgainstFilter(di))
			return false;
	}
	return true;
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
 * @brief Convert wildcard pattern to regular expression.
 */
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
 * @brief Find separator in string and return position of it.
 */
static std::size_t findSeparator(const String& str, String& prefix, std::size_t startPos = 0)
{
	prefix.clear();
	bool inQuotes = false;
	bool allowOnlyBasicSeparators = false;
	while (startPos < str.size() && str[startPos] == ' ')
		++startPos;
	const String prefixes[] = { _T("f:"), _T("d:"), _T("f!:"), _T("d!:"), _T("fe:"), _T("de:"), _T("fe!:"), _T("de!:"), _T("fp:") };
	for (const auto& pf : prefixes)
	{
		if (str.compare(startPos, pf.size(), pf) == 0)
		{
			startPos += pf.size();
			allowOnlyBasicSeparators = true;
			prefix = pf;
			break;
		}
	}
	for (std::size_t i = startPos; i < str.size(); ++i)
	{
		const auto ch = str[i];
		if (ch == '"')
			inQuotes = !inQuotes;
		else if (!inQuotes &&
			(ch == ';' || ch == '|' || (!allowOnlyBasicSeparators && (ch == ',' || ch == ':' || ch == ' '))))
			return i;
	}
	return String::npos;
}

/**
 * @brief Merge filter into regex or expression filter.
 */
static void mergeFilter(FileFilter* dest, const FileFilter* src)
{
	dest->filefilters.insert(dest->filefilters.end(), src->filefilters.begin(), src->filefilters.end());
	dest->filefiltersExclude.insert(dest->filefiltersExclude.end(), src->filefiltersExclude.begin(), src->filefiltersExclude.end());
	dest->dirfilters.insert(dest->dirfilters.end(), src->dirfilters.begin(), src->dirfilters.end());
	dest->dirfiltersExclude.insert(dest->dirfiltersExclude.end(), src->dirfiltersExclude.begin(), src->dirfiltersExclude.end());
	dest->fileExpressionFilters.insert(dest->fileExpressionFilters.end(), src->fileExpressionFilters.begin(), src->fileExpressionFilters.end());
	dest->fileExpressionFiltersExclude.insert(dest->fileExpressionFiltersExclude.end(), src->fileExpressionFiltersExclude.begin(), src->fileExpressionFiltersExclude.end());
	dest->dirExpressionFilters.insert(dest->dirExpressionFilters.end(), src->dirExpressionFilters.begin(), src->dirExpressionFilters.end());
	dest->dirExpressionFiltersExclude.insert(dest->dirExpressionFiltersExclude.end(), src->dirExpressionFiltersExclude.begin(), src->dirExpressionFiltersExclude.end());
	dest->errors.insert(dest->errors.end(), src->errors.begin(), src->errors.end());
}

/**
 * @brief Split filter groups from a string.
 */
std::vector<String> FileFilterHelper::SplitFilterGroups(const String& filterGroups)
{
	std::vector<String> result;
	const tchar_t* p = filterGroups.c_str();
	bool inQuotes = false;
	String filterGroup;
	while (*p)
	{
		if (!inQuotes)
		{
			if (*p == '"')
				inQuotes = true;
			else if (*p == '|')
			{
				++p;
				if (*p != '|')
				{
					result.push_back(filterGroup);
					filterGroup.clear();
					continue;
				}
			}
		}
		else
		{
			if (*p == '"')
				inQuotes = false;
		}
		filterGroup += *p++;
	};
	result.push_back(filterGroup);
	return result;
}

/**
 * @brief Join filter groups into a single string.
 */
String FileFilterHelper::JoinFilterGroups(const std::vector<String>& filterGroups)
{
	std::vector<String> filterGroups2;
	for (auto& group : filterGroups)
	{
		String group2 = group;
		strutils::replace(group2, _T("|"), _T("||"));
		filterGroups2.push_back(group2);
	}
	return strutils::join(filterGroups2.begin(), filterGroups2.end(), _T("|"));
}

/** 
 * @brief Convert user-given extension list to valid regular expression.
 * @param [in] Extension list/mask to convert to regular expression.
 * @return Regular expression that matches extension list.
 */
std::vector<FileFilterHelper::FilterGroup>
FileFilterHelper::ParseExtensions(const String &extensions) const
{
	std::vector<FilterGroup> filterGroups;
	std::vector<String> groups = SplitFilterGroups(extensions);
	for (const auto& group : groups)
	{
		String strFileParsed;
		String strDirParsed;
		std::vector<String> filePatterns;
		std::vector<String> filePatternsExclude;
		std::vector<String> dirPatterns;
		std::vector<String> dirPatternsExclude;
		String ext(group);
		String prefix;
		std::shared_ptr<FileFilter> pRegexOrExpressionFilter;
		std::shared_ptr<FileFilter> pRegexOrExpressionFilterExclude;
		size_t pos = 0;
		for (;;)
		{
			pos = findSeparator(ext, prefix);
			String token = ext.substr(0, pos == String::npos ? ext.size() : pos);
			if (token.length() >= 1 && prefix.empty())
			{
				// Only "*." or "*.something" allowed, other ignored
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
			else if (!prefix.empty())
			{
				if (!pRegexOrExpressionFilter)
				{
					pRegexOrExpressionFilter = std::make_shared<FileFilter>();
					pRegexOrExpressionFilter->default_include = false;
					pRegexOrExpressionFilter->name = extensions;
				}
				token = strutils::trim_ws(token.substr(token.find(':') + 1));
				if (prefix == _T("f:"))
					pRegexOrExpressionFilter->AddFilterPattern(
						&pRegexOrExpressionFilter->filefilters, token, true, 0);
				else if (prefix == _T("f!:"))
					pRegexOrExpressionFilter->AddFilterPattern(
						&pRegexOrExpressionFilter->filefiltersExclude, token, true, 0);
				else if (prefix == _T("d:"))
					pRegexOrExpressionFilter->AddFilterPattern(
						&pRegexOrExpressionFilter->dirfilters, token, false, 0);
				else if (prefix == _T("d!:"))
					pRegexOrExpressionFilter->AddFilterPattern(
						&pRegexOrExpressionFilter->dirfiltersExclude, token, false, 0);
				else if (prefix == _T("fe:"))
					pRegexOrExpressionFilter->AddFilterExpression(
						&pRegexOrExpressionFilter->fileExpressionFilters, token, 0);
				else if (prefix == _T("fe!:"))
					pRegexOrExpressionFilter->AddFilterExpression(
						&pRegexOrExpressionFilter->fileExpressionFiltersExclude, token, 0);
				else if (prefix == _T("de:"))
					pRegexOrExpressionFilter->AddFilterExpression(
						&pRegexOrExpressionFilter->dirExpressionFilters, token, 0);
				else if (prefix == _T("de!:"))
					pRegexOrExpressionFilter->AddFilterExpression(
						&pRegexOrExpressionFilter->dirExpressionFiltersExclude, token, 0);
				else if (prefix == _T("fp:"))
				{
					const String path = GetFileFilterPath(token);
					if (!path.empty())
					{
						const FileFilter* filter = m_fileFilterMgr->GetFilterByPath(path);
						if (filter)
						{
							if (!filter->default_include)
								mergeFilter(pRegexOrExpressionFilter.get(), filter);
							else
							{
								if (!pRegexOrExpressionFilterExclude)
								{
									pRegexOrExpressionFilterExclude = std::make_shared<FileFilter>();
									pRegexOrExpressionFilterExclude->default_include = true;
									pRegexOrExpressionFilterExclude->name = extensions;
								}
								mergeFilter(pRegexOrExpressionFilterExclude.get(), filter);
							}
						}
					}
					else
					{
						pRegexOrExpressionFilter->errors.emplace_back(FILTER_ERROR_FILTER_NAME_NOT_FOUND, -1, -1, token, "");
					}
				}
			}
			if (pos == String::npos)
				break; // No more separators found
			ext = ext.substr(pos + 1); // Remove extension + separator
		}

		if (filePatterns.empty() && (!pRegexOrExpressionFilter || (pRegexOrExpressionFilter->filefilters.empty() && pRegexOrExpressionFilter->fileExpressionFilters.empty())))
			strFileParsed = _T(".*"); // Match everything
		else
			strFileParsed = strutils::join(filePatterns.begin(), filePatterns.end(), _T("|"));
		if (dirPatterns.empty() && (!pRegexOrExpressionFilter || (pRegexOrExpressionFilter->dirfilters.empty() && pRegexOrExpressionFilter->dirExpressionFilters.empty())))
			strDirParsed = _T(".*"); // Match everything
		else
			strDirParsed = strutils::join(dirPatterns.begin(), dirPatterns.end(), _T("|"));
		String strFileParsedExclude = strutils::join(filePatternsExclude.begin(), filePatternsExclude.end(), _T("|"));
		String strDirParsedExclude = strutils::join(dirPatternsExclude.begin(), dirPatternsExclude.end(), _T("|"));

		std::string regexp_str_file = ucr::toUTF8(strFileParsed);
		std::string regexp_str_file_excluded = ucr::toUTF8(strFileParsedExclude);
		std::string regexp_str_dir = ucr::toUTF8(strDirParsed);
		std::string regexp_str_dir_excluded = ucr::toUTF8(strDirParsedExclude);

		FilterGroup filterGroup;
		if (!regexp_str_file.empty())
		{
			filterGroup.m_pMaskFileFilter = std::make_unique<FilterList>();
			filterGroup.m_pMaskFileFilter->AddRegExp(regexp_str_file);
		}
		if (!regexp_str_dir.empty())
		{
			filterGroup.m_pMaskDirFilter = std::make_unique<FilterList>();
			filterGroup.m_pMaskDirFilter->AddRegExp(regexp_str_dir);
		}
		if (!regexp_str_file_excluded.empty())
		{
			filterGroup.m_pMaskFileFilterExclude = std::make_unique<FilterList>();
			filterGroup.m_pMaskFileFilterExclude->AddRegExp(regexp_str_file_excluded);
		}
		if (!regexp_str_dir_excluded.empty())
		{
			filterGroup.m_pMaskDirFilterExclude = std::make_unique<FilterList>();
			filterGroup.m_pMaskDirFilterExclude->AddRegExp(regexp_str_dir_excluded);
		}
		filterGroup.m_pRegexOrExpressionFilter = pRegexOrExpressionFilter;
		filterGroup.m_pRegexOrExpressionFilterExclude = pRegexOrExpressionFilterExclude;
		filterGroups.push_back(std::move(filterGroup));
	}
	return filterGroups;
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
	std::vector<FileFilterInfo> filters = GetFileFilters();
	std::vector<FileFilterInfo>::const_iterator iter = filters.begin();
	while (iter != filters.end())
	{
		String path = (*iter).fullpath;

		fileInfo.Update(path);
		if (fileInfo.mtime != (*iter).fileinfo.mtime ||
			fileInfo.size != (*iter).fileinfo.size)
		{
			// Reload filter after changing it
			m_fileFilterMgr->ReloadFilterFromDisk(path);
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

	m_filterGroups.clear();

	for (const auto& filterGroupSrc : pHelper->m_filterGroups)
	{
		FilterGroup filterGroup;

		if (filterGroupSrc.m_pMaskFileFilter)
		{
			auto filterList = std::make_unique<FilterList>(FilterList());
			filterGroup.m_pMaskFileFilter = std::move(filterList);
			filterGroup.m_pMaskFileFilter->CloneFrom(filterGroupSrc.m_pMaskFileFilter.get());
		}

		if (filterGroupSrc.m_pMaskFileFilterExclude)
		{
			auto filterList = std::make_unique<FilterList>(FilterList());
			filterGroup.m_pMaskFileFilterExclude = std::move(filterList);
			filterGroup.m_pMaskFileFilterExclude->CloneFrom(filterGroupSrc.m_pMaskFileFilterExclude.get());
		}

		if (filterGroupSrc.m_pMaskDirFilter)
		{
			auto filterList = std::make_unique<FilterList>(FilterList());
			filterGroup.m_pMaskDirFilter = std::move(filterList);
			filterGroup.m_pMaskDirFilter->CloneFrom(filterGroupSrc.m_pMaskDirFilter.get());
		}

		if (filterGroupSrc.m_pMaskDirFilterExclude)
		{
			auto filterList = std::make_unique<FilterList>(FilterList());
			filterGroup.m_pMaskDirFilterExclude = std::move(filterList);
			filterGroup.m_pMaskDirFilterExclude->CloneFrom(filterGroupSrc.m_pMaskDirFilterExclude.get());
		}

		if (filterGroupSrc.m_pRegexOrExpressionFilter)
		{
			filterGroup.m_pRegexOrExpressionFilter.reset(new FileFilter());
			filterGroup.m_pRegexOrExpressionFilter->CloneFrom(filterGroupSrc.m_pRegexOrExpressionFilter.get());
		}

		if (filterGroupSrc.m_pRegexOrExpressionFilterExclude)
		{
			filterGroup.m_pRegexOrExpressionFilterExclude.reset(new FileFilter());
			filterGroup.m_pRegexOrExpressionFilterExclude->CloneFrom(filterGroupSrc.m_pRegexOrExpressionFilterExclude.get());
		}

		m_filterGroups.push_back(std::move(filterGroup));
	}

	if (pHelper->m_fileFilterMgr)
	{
		auto fileFilterMgr = std::make_unique<FileFilterMgr>(FileFilterMgr());
		m_fileFilterMgr = std::move(fileFilterMgr);
		m_fileFilterMgr->CloneFrom(pHelper->m_fileFilterMgr.get());
	}

	m_sMask = pHelper->m_sMask;
	m_sGlobalFilterPath = pHelper->m_sGlobalFilterPath;
	m_sUserSelFilterPath = pHelper->m_sUserSelFilterPath;
}
