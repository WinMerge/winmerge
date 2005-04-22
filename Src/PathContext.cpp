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
 * @brief Set path.
 * @param [in] sPath New path for item.
 */
void PathInfo::SetPath(CString sPath)
{
	m_sPath = sPath;
	m_sNormalizedPath = sPath;
}

/**
 * @brief Normalize path.
 */
void PathInfo::NormalizePath()
{
	m_sNormalizedPath = m_sPath;
	paths_normalize(m_sNormalizedPath);
}

PathContext::PathContext()
{

}

/**
 * @brief Return left path.
 * @param [in] sNormalized If TRUE normalized path is returned.
 */
const CString& PathContext::GetLeft(BOOL bNormalized) const
{
	return m_pathLeft.GetPath(bNormalized);
}

/**
 * @brief Return right path.
 * @param [in] sNormalized If TRUE normalized path is returned.
 */
const CString& PathContext::GetRight(BOOL bNormalized) const
{
	return m_pathRight.GetPath(bNormalized);
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

	if (GetLeft().IsEmpty())
	{
		TCHAR name[MAX_PATH];
		if (!::GetTempFileName(strTempPath, _T("_LT"), 0, name))
		{
			LogErrorString(Fmt(_T("GetTempFileName() for left-side failed: %s"),
				GetSysError(GetLastError())));
			return FALSE;
		}
		SetLeft(name);

		if (!paths.GetLeft().IsEmpty())
		{
			if (!::CopyFile(paths.GetLeft(), GetLeft(), FALSE))
			{
				LogErrorString(Fmt(_T("CopyFile() (copy left-side temp file) failed: %s"),
					GetSysError(GetLastError())));
				return FALSE;
			}
		}
		::SetFileAttributes(GetLeft(), FILE_ATTRIBUTE_NORMAL);
	}
	
	if (GetRight().IsEmpty())
	{
		TCHAR name[MAX_PATH];
		if (!::GetTempFileName(strTempPath, _T("_RT"), 0, name))
		{
			LogErrorString(Fmt(_T("GetTempFileName() for right-side failed: %s"),
				strTempPath, GetSysError(GetLastError())));
			return FALSE;
		}
		SetRight(name);

		if (!paths.GetRight().IsEmpty())
		{
			if (!::CopyFile(paths.GetRight(), GetRight(), FALSE))
			{
				LogErrorString(Fmt(_T("CopyFile() (copy right-side temp file) failed: %s"),
					GetSysError(GetLastError())));
				return FALSE;
			}
		}
		::SetFileAttributes(GetRight(), FILE_ATTRIBUTE_NORMAL);
	}
	return TRUE;
}

/**
 * @brief Check if temp files exist.
 * @return TRUE if one of temp files exist.
 */
BOOL TempFileContext::FilesExist()
{
	CFileStatus s1,s2;
	BOOL bLeftExists = FALSE;
	BOOL bRightExists = FALSE;

	if (!GetLeft().IsEmpty())
		bLeftExists = CFile::GetStatus(GetLeft(), s1);
	if (!GetRight().IsEmpty())
		bRightExists = CFile::GetStatus(GetRight(), s2);
	
	return bLeftExists || bRightExists;

}

/**
 * @brief Delete temp files.
 */
void TempFileContext::DeleteFiles()
{
	if (!GetLeft().IsEmpty())
	{
		if (!::DeleteFile(GetLeft()))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) (deleting left-side temp file) failed: %s"),
				GetLeft(), GetSysError(GetLastError())));
		}
		SetLeft(_T(""));

	}
	if (!GetRight().IsEmpty())
	{
		if (!::DeleteFile(GetRight()))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) (deleting right-side temp file) failed: %s"),
				GetRight(), GetSysError(GetLastError())));
		}
		SetRight(_T(""));
	}
}
