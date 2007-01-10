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
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "Ucs2Utf8.h"
#include "FilterList.h"
#include "FileInfo.h"
#include "FileFilterMgr.h"
#include "FileFilterHelper.h"
#include "Coretools.h"
#include "paths.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
FileFilterMgr * FileFilterHelper::GetManager()
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
	m_sFileFilterPath.Empty();

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
void FileFilterHelper::GetFileFilters(FILEFILTER_INFOLIST * filters, CString & selected) const
{
	if (m_fileFilterMgr)
	{
		int count = m_fileFilterMgr->GetFilterCount();
		filters->SetSize(count);
		for (int i=0; i<count; ++i)
		{
			FileFilterInfo filter;
			filter.fullpath = m_fileFilterMgr->GetFilterPath(i);
			filter.name = m_fileFilterMgr->GetFilterName(i);
			filter.description = m_fileFilterMgr->GetFilterDesc(i);
			filters->SetAt(i, filter);
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
CString FileFilterHelper::GetFileFilterName(CString filterPath) const
{
	FILEFILTER_INFOLIST filters;
	CString selected;
	CString name;

	GetFileFilters(&filters, selected);
	for (int i = 0; i < filters.GetSize(); i++)
	{
		if (filters.GetAt(i).fullpath == filterPath)
		{
			name = filters.GetAt(i).name;
			break;
		}
	}
	return name;
}

/** 
 * @brief Return path to filter with given name.
 * @param [in] filterName Name of filter.
 * @sa FileFilterHelper::GetFileFilterName()
 */
CString FileFilterHelper::GetFileFilterPath(CString filterName) const
{
	FILEFILTER_INFOLIST filters;
	CString selected;
	CString path;

	GetFileFilters(&filters, selected);
	for (int i = 0; i < filters.GetSize(); i++)
	{
		if (filters.GetAt(i).name == filterName)
		{
			path = filters.GetAt(i).fullpath;
			break;
		}
	}
	return path;
}

/** 
 * @brief Set User's filter folder.
 * @param [in] filterPath Location of User's filters.
 */
void FileFilterHelper::SetUserFilterPath(const CString & filterPath)
{
	CString path(filterPath);

	if (path[path.GetLength() - 1] != '\\')
		path += _T("\\");
	m_sUserSelFilterPath = path;
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
	CString regExp = ParseExtensions(strMask);

	char * regexp_utf = UCS2UTF8_ConvertToUtf8(regExp);
	m_pMaskFilter->AddRegExp(regexp_utf);
	UCS2UTF8_Dealloc(regexp_utf);
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
		CString strFileName = szFileName;
		strFileName.MakeLower();
		if (strFileName[0] != _T('\\'))
			strFileName = _T('\\') + strFileName;
		// append a point if there is no extension
		if (strFileName.Find(_T('.')) == -1)
			strFileName = strFileName + _T('.');

		char * name_utf = UCS2UTF8_ConvertToUtf8(strFileName);
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
		CString strDirName(_T("\\"));
		strDirName += szDirName;

		return m_fileFilterMgr->TestDirNameAgainstFilter(m_currentFilter, strDirName);
	}
}

/**
 * @brief Open filter file to editor (notepad) for modifying.
 *
 * @param [in] szFileFilterterPath Path of filter file to edit.
 * @todo Use external editor defined in options?
 */
void FileFilterHelper::EditFileFilter(LPCTSTR szFileFilterPath)
{
	CString cmdLine = (CString)_T("notepad ") + szFileFilterPath;
	STARTUPINFO stInfo = {0};
	PROCESS_INFORMATION prInfo;
	BOOL processSuccess = FALSE;
	stInfo.cb = sizeof(STARTUPINFO);
	processSuccess = CreateProcess(NULL, (LPTSTR)(LPCTSTR)cmdLine, NULL,
		NULL, FALSE, 0, NULL, NULL, &stInfo, &prInfo);
}

/**
 * @brief Load in all filter patterns in a directory (unless already in map).
 * @param [in,out] patternsLoaded Map where found filterfiles are added.
 * @param [in] sPattern Directory wildcard defining files to add to map as filter files.
 * It is directoryname + filemask, for example, for a filter for all users:
 * "C:\Program Files\WinMerge\Filters\*.flt"
 * Examples of user-specific filters:
 * "C:\Documents and Settings\User\My Documents\WinMergeFilters\*.flt"
 */
void FileFilterHelper::LoadFileFilterDirPattern(FILEFILTER_FILEMAP & patternsLoaded,
		const CString & sPattern)
{
	int n=0;
	if (!patternsLoaded.Lookup(sPattern, n))
	{
		m_fileFilterMgr->LoadFromDirectory(sPattern, FileFilterExt);
	}
	patternsLoaded[sPattern] = ++n;
}

/** 
 * @brief Parse user-given extension list to valid regexp for diffengine.
 */
CString FileFilterHelper::ParseExtensions(CString extensions)
{
	CString strParsed;
	CString strPattern;
	BOOL bFilterAdded = FALSE;
	static const TCHAR pszSeps[] = _T(" ;|,:");

	extensions += _T(";"); // Add one separator char to end
	int pos = extensions.FindOneOf(pszSeps);
	
	while (pos >= 0)
	{
		CString token = extensions.Left(pos); // Get first extension
		extensions.Delete(0, pos + 1); // Remove extension + separator
		
		// Only "*." or "*.something" allowed, other ignored
		if (token.GetLength() >= 2 && token[0] == '*' && token[1] == '.')
		{
			bFilterAdded = TRUE;
			strPattern += _T(".*\\.");
			strPattern += token.Mid(2);
			strPattern += _T("$");
		}
		else
			bFilterAdded = FALSE;

		pos = extensions.FindOneOf(pszSeps); 
		if (bFilterAdded && pos >= 0)
			strPattern += _T("|");
	}

	if (strPattern.IsEmpty())
		strParsed = _T(".*"); // Match everything
	else
	{
		strParsed = _T("^");
		strPattern.MakeLower();
		strParsed = strPattern; //+ _T("$");
	}
	return strParsed;
}

/** 
 * @brief Returns TRUE if active filter is mask.
 */
BOOL FileFilterHelper::IsUsingMask()
{
	return m_bUseMask;
}

/** 
 * @brief Returns active filter (or mask string)
 */
CString FileFilterHelper::GetFilterNameOrMask()
{
	CString sFilter;

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
 * @return TRUE if given filter was set, FALSE if default filter was set.
 * @note If function returns FALSE, you should ask filter set with
 * GetFilterNameOrMask().
 */
BOOL FileFilterHelper::SetFilter(CString filter)
{
	// If filter is empty string set default filter
	if (filter.IsEmpty())
	{
		UseMask(TRUE);
		SetMask(_T("*.*"));
		SetFileFilterPath(_T(""));
		return FALSE;
	}

	// Remove leading and trailing whitespace characters from the string.
	filter.TrimLeft();
	filter.TrimRight();

	// Star means we have a file extension mask
	if (filter[0] == '*')
	{
		UseMask(TRUE);
		SetMask(filter);
		SetFileFilterPath(_T(""));
	}
	else
	{
		CString path = GetFileFilterPath(filter);
		if (!path.IsEmpty())
		{
			UseMask(FALSE);
			SetFileFilterPath(path);
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
	FILEFILTER_INFOLIST filters;
	FileInfo fileInfo;
	FileInfo *fileInfoStored = NULL;
	FileFilterInfo filter;
	CString selected;

	GetFileFilters(&filters, selected);
	for (int i = 0; i < filters.GetSize(); i++)
	{
		filter = filters.GetAt(i);
		CString path = filter.fullpath;
		fileInfoStored = &filter.fileinfo;

		fileInfo.Update(path);
		if (fileInfo.mtime != fileInfoStored->mtime ||
			fileInfo.size != fileInfoStored->size)
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
	}
}

/**
 * @brief Load any known file filters
 * @todo Preserve filter selection? How?
 */
void FileFilterHelper::LoadAllFileFilters()
{
	// Load filters from all possible subdirectories
	FILEFILTER_FILEMAP patternsLoaded;

	// First delete existing filters
	m_fileFilterMgr->DeleteAllFilters();

	// Program application directory
	m_sGlobalFilterPath = GetModulePath() + _T("\\Filters");
	LoadFileFilterDirPattern(patternsLoaded, m_sGlobalFilterPath + _T("\\*") + FileFilterExt);
	LoadFileFilterDirPattern(patternsLoaded, m_sUserSelFilterPath + _T("\\*") + FileFilterExt);
}

/**
 * @brief Return path to global filters (& create if needed), or empty if cannot create
 */
CString FileFilterHelper::GetGlobalFilterPathWithCreate() const
{
	return paths_EnsurePathExist(m_sGlobalFilterPath);
}

/**
 * @brief Return path to user filters (& create if needed), or empty if cannot create
 */
CString FileFilterHelper::GetUserFilterPathWithCreate() const
{
	return paths_EnsurePathExist(m_sUserSelFilterPath);
}

