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
	auto [regExpFile, regExpFileExclude, regExpDir, regExpDirExclude, pRegexOrExpressionFilter, pRegexOrExpressionFilterExclude]
		= ParseExtensions(m_sMask);

	std::string regexp_str_file = ucr::toUTF8(regExpFile);
	std::string regexp_str_file_excluded = ucr::toUTF8(regExpFileExclude);
	std::string regexp_str_dir = ucr::toUTF8(regExpDir);
	std::string regexp_str_dir_excluded = ucr::toUTF8(regExpDirExclude);

	if (m_pMaskFileFilter)
		m_pMaskFileFilter->RemoveAllFilters();
	if (m_pMaskDirFilter)
		m_pMaskDirFilter->RemoveAllFilters();
	if (m_pMaskFileFilterExclude)
		m_pMaskFileFilterExclude->RemoveAllFilters();
	if (m_pMaskDirFilterExclude)
		m_pMaskDirFilterExclude->RemoveAllFilters();
	if (!regexp_str_file.empty())
	{
		if (!m_pMaskFileFilter)
			m_pMaskFileFilter = std::make_unique<FilterList>();
		m_pMaskFileFilter->AddRegExp(regexp_str_file);
	}
	else
		m_pMaskFileFilter.reset();
	if (!regexp_str_dir.empty())
	{
		if (!m_pMaskDirFilter)
			m_pMaskDirFilter = std::make_unique<FilterList>();
		m_pMaskDirFilter->AddRegExp(regexp_str_dir);
	}
	else
		m_pMaskDirFilter.reset();
	if (!regexp_str_file_excluded.empty())
	{
		if (!m_pMaskFileFilterExclude)
			m_pMaskFileFilterExclude = std::make_unique<FilterList>();
		m_pMaskFileFilterExclude->AddRegExp(regexp_str_file_excluded);
	}
	else
		m_pMaskFileFilterExclude.reset();
	if (!regexp_str_dir_excluded.empty())
	{
		if (!m_pMaskDirFilterExclude)
			m_pMaskDirFilterExclude = std::make_unique<FilterList>();
		m_pMaskDirFilterExclude->AddRegExp(regexp_str_dir_excluded);
	}
	else
		m_pMaskDirFilterExclude.reset();
	m_pRegexOrExpressionFilter = pRegexOrExpressionFilter;
	m_pRegexOrExpressionFilterExclude = pRegexOrExpressionFilterExclude;
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
	if (m_pRegexOrExpressionFilter)
		m_pRegexOrExpressionFilter->SetDiffContext(pCtxt);
	if (m_pRegexOrExpressionFilterExclude)
		m_pRegexOrExpressionFilterExclude->SetDiffContext(pCtxt);
}

std::vector<const FileFilterErrorInfo*> FileFilterHelper::GetErrorList() const
{
	std::vector<const FileFilterErrorInfo*> list;
	for (const auto* pfilter : { m_pRegexOrExpressionFilter.get(), m_pRegexOrExpressionFilterExclude.get() })
	{
		if (pfilter)
		{
			for (const auto& error : pfilter->errors)
				list.push_back(&error);
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
	bool result = m_pMaskFileFilter && m_pMaskFileFilter->Match(strFileNameUtf8Period);
	if (!result)
		result = m_pRegexOrExpressionFilter && TestAgainstRegList(&m_pRegexOrExpressionFilter->filefilters, szFileName);
	if (!result)
		return false;
	if (m_pMaskFileFilterExclude && m_pMaskFileFilterExclude->Match(strFileNameUtf8Period))
		return false;
	if (m_pRegexOrExpressionFilter && TestAgainstRegList(&m_pRegexOrExpressionFilter->filefiltersExclude, szFileName))
		return false;
	if (m_pRegexOrExpressionFilterExclude && !m_pRegexOrExpressionFilterExclude->TestFileNameAgainstFilter(szFileName))
		return false;
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
		result = m_pMaskFileFilter && m_pMaskFileFilter->Match(strFileNameUtf8Period);
	}
	if (!result)
	{
		if (m_pRegexOrExpressionFilter)
		{
			result = FileFilter::TestAgainstRegList(&m_pRegexOrExpressionFilter->filefilters, di);
			if (!result)
				result = FileFilter::TestAgainstExpressionList(&m_pRegexOrExpressionFilter->fileExpressionFilters, di);
		}
	}
	if (!result)
		return false;
	if (i < nDirs)
	{
		if (m_pMaskFileFilterExclude && m_pMaskFileFilterExclude->Match(strFileNameUtf8Period))
			return false;
	}
	if (m_pRegexOrExpressionFilter)
	{
		if (FileFilter::TestAgainstRegList(&m_pRegexOrExpressionFilter->filefiltersExclude, di))
			return false;
		if (FileFilter::TestAgainstExpressionList(&m_pRegexOrExpressionFilter->fileExpressionFiltersExclude, di))
			return false;
	}
	if (m_pRegexOrExpressionFilterExclude && !m_pRegexOrExpressionFilterExclude->TestFileDiffItemAgainstFilter(di))
		return false;
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
	bool result = m_pMaskDirFilter && m_pMaskDirFilter->Match(strDirNameUtf8Period);
	if (!result)
	{
		if (m_pRegexOrExpressionFilter)
			result = TestAgainstRegList(&m_pRegexOrExpressionFilter->dirfilters, strDirName);
	}
	if (!result)
		return false;
	if (m_pMaskDirFilterExclude && m_pMaskDirFilterExclude->Match(strDirNameUtf8Period))
		return false;
	if (m_pRegexOrExpressionFilter && TestAgainstRegList(&m_pRegexOrExpressionFilter->dirfiltersExclude, strDirName))
		return false;
	if (m_pRegexOrExpressionFilterExclude && !m_pRegexOrExpressionFilterExclude->TestDirNameAgainstFilter(strDirName))
		return false;
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
		result = m_pMaskDirFilter && m_pMaskDirFilter->Match(strDirNameUtf8Period);
	}
	if (!result)
	{
		if (m_pRegexOrExpressionFilter)
		{
			result = FileFilter::TestAgainstRegList(&m_pRegexOrExpressionFilter->dirfilters, di);
			if (!result)
				result = FileFilter::TestAgainstExpressionList(&m_pRegexOrExpressionFilter->dirExpressionFilters, di);
		}
	}
	if (!result)
		return false;
	if (i < nDirs)
	{
		if (m_pMaskDirFilterExclude && m_pMaskDirFilterExclude->Match(strDirNameUtf8Period))
			return false;
	}
	if (m_pRegexOrExpressionFilter)
	{
		
		if (FileFilter::TestAgainstRegList(&m_pRegexOrExpressionFilter->dirfiltersExclude, di))
			return false;
		if (FileFilter::TestAgainstExpressionList(&m_pRegexOrExpressionFilter->dirExpressionFiltersExclude, di))
			return false;
	}
	if (m_pRegexOrExpressionFilterExclude && !m_pRegexOrExpressionFilterExclude->TestDirDiffItemAgainstFilter(di))
		return false;
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
			(ch == ';' || (!allowOnlyBasicSeparators && (ch == ',' || ch == '|' || ch == ':' || ch == ' '))))
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
 * @brief Convert user-given extension list to valid regular expression.
 * @param [in] Extension list/mask to convert to regular expression.
 * @return Regular expression that matches extension list.
 */
std::tuple<String, String, String, String, std::shared_ptr<FileFilter>, std::shared_ptr<FileFilter>>
FileFilterHelper::ParseExtensions(const String &extensions) const
{
	String strFileParsed;
	String strDirParsed;
	std::vector<String> filePatterns;
	std::vector<String> filePatternsExclude;
	std::vector<String> dirPatterns;
	std::vector<String> dirPatternsExclude;
	String ext(extensions);
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
	return { strFileParsed, strFileParsedExclude, strDirParsed, strDirParsedExclude, pRegexOrExpressionFilter, pRegexOrExpressionFilterExclude };
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

	if (pHelper->m_pMaskFileFilter)
	{
		auto filterList = std::make_unique<FilterList>(FilterList());
		m_pMaskFileFilter = std::move(filterList);
		m_pMaskFileFilter->CloneFrom(pHelper->m_pMaskFileFilter.get());
	}

	if (pHelper->m_pMaskFileFilterExclude)
	{
		auto filterList = std::make_unique<FilterList>(FilterList());
		m_pMaskFileFilterExclude = std::move(filterList);
		m_pMaskFileFilterExclude->CloneFrom(pHelper->m_pMaskFileFilterExclude.get());
	}

	if (pHelper->m_pMaskDirFilter)
	{
		auto filterList = std::make_unique<FilterList>(FilterList());
		m_pMaskDirFilter = std::move(filterList);
		m_pMaskDirFilter->CloneFrom(pHelper->m_pMaskDirFilter.get());
	}

	if (pHelper->m_pMaskDirFilterExclude)
	{
		auto filterList = std::make_unique<FilterList>(FilterList());
		m_pMaskDirFilterExclude = std::move(filterList);
		m_pMaskDirFilterExclude->CloneFrom(pHelper->m_pMaskDirFilterExclude.get());
	}

	if (pHelper->m_pRegexOrExpressionFilter)
	{
		m_pRegexOrExpressionFilter.reset(new FileFilter());
		m_pRegexOrExpressionFilter->CloneFrom(pHelper->m_pRegexOrExpressionFilter.get());
	}

	if (pHelper->m_pRegexOrExpressionFilterExclude)
	{
		m_pRegexOrExpressionFilterExclude.reset(new FileFilter());
		m_pRegexOrExpressionFilterExclude->CloneFrom(pHelper->m_pRegexOrExpressionFilterExclude.get());
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
