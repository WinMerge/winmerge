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
#include "FileFilterMgr.h"
#include "FileFilterHelper.h"
#include "RegExp.h"

FileFilterHelper::FileFilterHelper()
{
	m_fileFilterMgr = NULL;
	m_bUseMask = TRUE;
}

FileFilterMgr * FileFilterHelper::GetManager()
{
	return m_fileFilterMgr;
}

void FileFilterHelper::SetManager(FileFilterMgr * pFilterManager)
{
	if (pFilterManager != NULL)
		m_fileFilterMgr = pFilterManager;
}

/** @brief Store current filter (if filter manager validates the name) */
void FileFilterHelper::SetFileFilterPath(LPCTSTR szFileFilterPath)
{
	VERIFY(m_sFileFilterPath.LoadString(IDS_USERCHOICE_NONE));
	if (!m_fileFilterMgr)
		return;
	m_currentFilter = m_fileFilterMgr->GetFilterByPath(szFileFilterPath);
	if (m_currentFilter)
		m_sFileFilterPath = szFileFilterPath;
}

/** @brief fill list with names of known filters */
void FileFilterHelper::GetFileFilters(StringPairArray * filters, CString & selected) const
{
	if (m_fileFilterMgr)
	{
		int count = m_fileFilterMgr->GetFilterCount();
		filters->SetSize(count);
		for (int i=0; i<count; ++i)
		{
			StringPair pair;
			pair.first = m_fileFilterMgr->GetFilterPath(i);
			pair.second = m_fileFilterMgr->GetFilterName(i);
			filters->SetAt(i, pair);
		}
	}
	selected = m_sFileFilterPath;
}

/** @brief Return name of filter in given file */
CString FileFilterHelper::GetFileFilterName(CString filterPath)
{
	StringPairArray filters;
	CString selected;
	CString name;

	GetFileFilters(&filters, selected);
	for (int i = 0; i < filters.GetSize(); i++)
	{
		if (filters.GetAt(i).first == filterPath)
		{
			name = filters.GetAt(i).second;
			break;
		}
	}
	return name;
}

/** @brief Return path to filter with given name */
CString FileFilterHelper::GetFileFilterPath(CString filterName)
{
	StringPairArray filters;
	CString selected;
	CString path;

	GetFileFilters(&filters, selected);
	for (int i = 0; i < filters.GetSize(); i++)
	{
		if (filters.GetAt(i).second == filterName)
		{
			path = filters.GetAt(i).first;
			break;
		}
	}
	return path;
}

void FileFilterHelper::UseMask(BOOL bUseMask)
{
	m_bUseMask = bUseMask;
}

void FileFilterHelper::SetMaskRegExp(LPCTSTR strRegExp)
{
	m_rgx.RegComp(strRegExp);
}

/** @brief Return TRUE unless we're suppressing this file by filter */
BOOL FileFilterHelper::includeFile(LPCTSTR szFileName)
{
	if (m_bUseMask)
	{
		// preprend a backslash if there is none
		CString strFileName = szFileName;
		if (strFileName[0] != _T('\\'))
			strFileName = _T('\\') + strFileName;
		// append a point if there is no extension
		if (strFileName.Find(_T('.')) == -1)
			strFileName = strFileName + _T('.');
		return (! m_rgx.RegFind(strFileName));
	}
	else
	{
		if (!m_fileFilterMgr || !m_currentFilter)
			return TRUE;

		// preprend a backslash if there is none
		CString strFileName = szFileName;
		if (strFileName[0] != _T('\\'))
			strFileName = _T('\\') + strFileName;
		// append a point if there is no extension
		if (strFileName.Find(_T('.')) == -1)
			strFileName = strFileName + _T('.');

		return m_fileFilterMgr->TestFileNameAgainstFilter(m_currentFilter, strFileName);
	}
}

/** @brief Return TRUE unless we're suppressing this directory by filter */
BOOL FileFilterHelper::includeDir(LPCTSTR szDirName)
{
	if (m_bUseMask)
	{
		// directory have no extension
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

/** @brief Bring up file filter in notepad */
void FileFilterHelper::EditFileFilter(LPCTSTR szFileFilterPath)
{
	FileFilter * filter = m_fileFilterMgr->GetFilterByPath(szFileFilterPath);
	if (!filter)
	{
		ASSERT(0);
		return;
	}

	CString cmdLine = (CString)_T("notepad ") + m_fileFilterMgr->GetFullpath(filter);
	STARTUPINFO stInfo = {0};
	PROCESS_INFORMATION prInfo;
	BOOL processSuccess = FALSE;
	stInfo.cb = sizeof(STARTUPINFO);
	processSuccess = CreateProcess(NULL, (LPTSTR)(LPCTSTR)cmdLine, NULL,
		NULL, FALSE, 0, NULL, NULL, &stInfo, &prInfo);

	if (processSuccess == TRUE)
	{
		// Wait until process closes down
		WaitForSingleObject(prInfo.hProcess, INFINITE);
		CloseHandle(prInfo.hThread);
		CloseHandle(prInfo.hProcess);
	}
	
	// Reload filter after changing it
	m_fileFilterMgr->ReloadFilterFromDisk(filter);
}

/** @brief Load in all filter patterns in a directory (unless already in map) */
void FileFilterHelper::LoadFileFilterDirPattern(CMap<CString, LPCTSTR, int, int> & patternsLoaded, const CString & sPattern)
{
	int n=0;
	if (!patternsLoaded.Lookup(sPattern, n))
	{
		m_fileFilterMgr->LoadFromDirectory(sPattern, _T(".flt"));
	}
	patternsLoaded[sPattern] = ++n;
}
