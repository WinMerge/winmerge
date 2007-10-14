/**
 * @file  PathContext.cpp
 *
 * @brief Implementation file for PathInfo and PathContext
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "PathContext.h"
#include "paths.h"

/**
 * @brief Copy constructor.
 */
PathInfo::PathInfo(const PathInfo &pi)
{
	m_sPath = pi.m_sPath;
}

/**
 * @brief Get path.
 * @param [in] sbNormalized TRUE if path is wanted in normalized format.
 */
String PathInfo::GetPath(BOOL bNormalized /*= TRUE*/) const
{ 
	if (!bNormalized)
	{
		if (!paths_EndsWithSlash(m_sPath.c_str()))
			return m_sPath + _T("\\");
		else
			return m_sPath;
	}
	else
		return m_sPath;
}

/**
 * @brief Set path.
 * @param [in] sPath New path for item.
 */
void PathInfo::SetPath(LPCTSTR sPath)
{
	m_sPath = sPath;
}

/**
 * @brief Normalize path.
 */
void PathInfo::NormalizePath()
{
	paths_normalize(m_sPath);
}

PathContext::PathContext()
{

}

PathContext::PathContext(LPCTSTR sLeft, LPCTSTR sRight)
{
	m_pathLeft.SetPath(sLeft);
	m_pathRight.SetPath(sRight);
}

/**
 * @brief Return left path.
 * @param [in] sNormalized If TRUE normalized path is returned.
 */
String PathContext::GetLeft(BOOL bNormalized) const
{
	return m_pathLeft.GetPath(bNormalized);
}

/**
 * @brief Return right path.
 * @param [in] sNormalized If TRUE normalized path is returned.
 */
String PathContext::GetRight(BOOL bNormalized) const
{
	return m_pathRight.GetPath(bNormalized);
}

/**
 * @brief Return path
 * @param [in] index index of path to return
 * @param [in] sNormalized If TRUE normalized path is returned.
 */
String PathContext::GetPath(int index, BOOL bNormalized) const
{
	return index == 0 ? m_pathLeft.GetPath(bNormalized) : m_pathRight.GetPath(bNormalized);
}

/**
 * @brief Set left path.
 * @param [in] path New path for item.
 */
void PathContext::SetLeft(LPCTSTR path)
{
	m_pathLeft.SetPath(path);
	m_pathLeft.NormalizePath();
}

/**
 * @brief Set right path.
 * @param [in] path New path for item.
 */
void PathContext::SetRight(LPCTSTR path)
{
	m_pathRight.SetPath(path);
	m_pathRight.NormalizePath();
}

/**
 * @brief Set path
 * @param [in] index index of path to set
 * @param [in] path New path for item.
 */
void PathContext::SetPath(int index, LPCTSTR path)
{
	if (index == 0)
	{
		m_pathLeft.SetPath(path);
		m_pathLeft.NormalizePath();
	}
	else
	{
		m_pathRight.SetPath(path);
		m_pathRight.NormalizePath();
	}
}

/**
 * @brief Swap paths.
 */
void PathContext::Swap()
{
	m_pathLeft.m_sPath.swap(m_pathRight.m_sPath);
}

/**
 * @brief Destructor, deletes existing temp files.
 */
TempFileContext::~TempFileContext()
{
	if (FilesExist())
		DeleteFiles();
}

/**
 * @brief Create temp files.
 *
 * Creates new temp files. Temp files are named based on
 * PathContext paths given as parameter.
 *
 * @param [in] paths PathContext whose paths are used as basis.
 */
BOOL TempFileContext::CreateFiles(const PathContext &paths)
{
	TCHAR strTempPath[MAX_PATH] = {0};

	if (!::GetTempPath(MAX_PATH, strTempPath))
	{
		LogErrorString(Fmt(_T("GetTempPath() failed: %s"),
			GetSysError(GetLastError())));
		return FALSE;
	}

	m_sTempPath = strTempPath;

	if (GetLeft().empty())
	{
		int nerr=0;
		String sTempPath = paths_GetTempFileName(strTempPath, _T("_LT"), &nerr);
		if (sTempPath.empty())
		{
			LogErrorString(Fmt(_T("GetTempFileName() for left-side failed: %s"),
				GetSysError(nerr)));
			return FALSE;
		}
		SetLeft(sTempPath.c_str());

		if (!paths.GetLeft().empty())
		{
			if (!::CopyFile(paths.GetLeft().c_str(), GetLeft().c_str(), FALSE))
			{
				LogErrorString(Fmt(_T("CopyFile() (copy left-side temp file) failed: %s"),
					GetSysError(GetLastError())));
				return FALSE;
			}
		}
		::SetFileAttributes(GetLeft().c_str(), FILE_ATTRIBUTE_NORMAL);
	}
	
	if (GetRight().empty())
	{
		TCHAR name[MAX_PATH];
		if (!::GetTempFileName(strTempPath, _T("_RT"), 0, name))
		{
			LogErrorString(Fmt(_T("GetTempFileName() for right-side failed: %s"),
				strTempPath, GetSysError(GetLastError())));
			return FALSE;
		}
		SetRight(name);

		if (!paths.GetRight().empty())
		{
			if (!::CopyFile(paths.GetRight().c_str(), GetRight().c_str(), FALSE))
			{
				LogErrorString(Fmt(_T("CopyFile() (copy right-side temp file) failed: %s"),
					GetSysError(GetLastError())));
				return FALSE;
			}
		}
		::SetFileAttributes(GetRight().c_str(), FILE_ATTRIBUTE_NORMAL);
	}
	return TRUE;
}

/**
 * @brief Check if temp files exist.
 * @return TRUE if one of temp files exist.
 */
BOOL TempFileContext::FilesExist() const
{
	BOOL bLeftExists = FALSE;
	BOOL bRightExists = FALSE;

	if (!GetLeft().empty())
		bLeftExists = (paths_DoesPathExist(GetLeft().c_str()) == IS_EXISTING_FILE);
	if (!GetRight().empty())
		bLeftExists = (paths_DoesPathExist(GetRight().c_str()) == IS_EXISTING_FILE);
	
	return bLeftExists || bRightExists;
}

/**
 * @brief Delete temp files.
 */
void TempFileContext::DeleteFiles()
{
	if (!GetLeft().empty())
	{
		if (!::DeleteFile(GetLeft().c_str()))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) (deleting left-side temp file) failed: %s"),
				GetLeft(), GetSysError(GetLastError())));
		}
		SetLeft(_T(""));

	}
	if (!GetRight().empty())
	{
		if (!::DeleteFile(GetRight().c_str()))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) (deleting right-side temp file) failed: %s"),
				GetRight(), GetSysError(GetLastError())));
		}
		SetRight(_T(""));
	}
}
