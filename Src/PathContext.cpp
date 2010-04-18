/**
 * @file  PathContext.cpp
 *
 * @brief Implementation file for PathInfo and PathContext
 *
 */
// ID line follows -- this is updated by SVN
// $Id: PathContext.cpp 4929 2008-01-18 20:03:57Z kimmov $

#include "stdafx.h"
#include "PathContext.h"
#include "paths.h"
#include "Environment.h"

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

String& PathInfo::GetRef()
{ 
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

void PathInfo::SetPath(String & sPath)
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
	m_nFiles = 0;
}

PathContext::PathContext(LPCTSTR sLeft)
{
	m_nFiles = 1;
	m_path[0].SetPath(sLeft);
}

PathContext::PathContext(LPCTSTR sLeft, LPCTSTR sRight)
{
	m_nFiles = 2;
	m_path[0].SetPath(sLeft);
	m_path[1].SetPath(sRight);
}

PathContext::PathContext(LPCTSTR sLeft, LPCTSTR sMiddle, LPCTSTR sRight)
{
	m_nFiles = 3;
	m_path[0].SetPath(sLeft);
	m_path[1].SetPath(sMiddle);
	m_path[2].SetPath(sRight);
}

PathContext::PathContext(const PathContext &paths)
{
	m_nFiles = paths.m_nFiles;
	for (int i = 0; i < m_nFiles; i++)
		m_path[i].SetPath(paths[i]);
}

String PathContext::GetAt(int nIndex) const
{
	ASSERT(nIndex < m_nFiles);
	return m_path[nIndex].GetPath();
}

String& PathContext::GetElement(int nIndex)
{
	ASSERT(nIndex < m_nFiles);
	return m_path[nIndex].GetRef();
}

void PathContext::SetAt(int nIndex, const String& newElement)
{
	ASSERT(nIndex < m_nFiles);
	m_path[nIndex].SetPath(newElement.c_str());
}

String PathContext::operator[](int nIndex) const
{
	return GetAt(nIndex);
}

String& PathContext::operator[](int nIndex)
{
	return GetElement(nIndex);
}

/**
 * @brief set number of files.
 */
void PathContext::SetSize(int nFiles)
{
	m_nFiles = nFiles;
}

/**
 * @brief Return number of files.
 */
int PathContext::GetSize() const
{
	return m_nFiles;
}

/**
 * @brief Empty m_path array
 */
void PathContext::RemoveAll()
{
	m_nFiles = 0;
	m_path[0].SetPath(_T(""));
	m_path[1].SetPath(_T(""));
	m_path[2].SetPath(_T(""));
}

/**
 * @brief Return left path.
 * @param [in] sNormalized If TRUE normalized path is returned.
 */
String PathContext::GetLeft(BOOL bNormalized) const
{
	if (m_nFiles == 0)
		return _T("");
	return m_path[0].GetPath(bNormalized);
}

/**
 * @brief Return right path.
 * @param [in] sNormalized If TRUE normalized path is returned.
 */
String PathContext::GetRight(BOOL bNormalized) const
{
	if (m_nFiles < 2)
		return _T("");
	return m_path[m_nFiles - 1].GetPath(bNormalized);
}

/**
 * @brief Return middle path.
 * @param [in] sNormalized If TRUE normalized path is returned.
 */
String PathContext::GetMiddle(BOOL bNormalized) const
{
	if (m_nFiles < 3)
		return _T("");
	return m_path[1].GetPath(bNormalized);
}

/**
 * @brief Return path
 * @param [in] index index of path to return
 * @param [in] sNormalized If TRUE normalized path is returned.
 */
String PathContext::GetPath(int index, BOOL bNormalized) const
{
	return m_path[index].GetPath(bNormalized);
}

/**
 * @brief Set left path.
 * @param [in] path New path for item.
 */
void PathContext::SetLeft(LPCTSTR path, bool bNormalized)
{
	if (m_nFiles == 0)
		m_nFiles = 1;
	m_path[0].SetPath(path);
	if (bNormalized)
		m_path[0].NormalizePath();
}

/**
 * @brief Set right path.
 * @param [in] path New path for item.
 */
void PathContext::SetRight(LPCTSTR path, bool bNormalized)
{
	if (m_nFiles < 2)
		m_nFiles = 2;
	m_path[m_nFiles - 1].SetPath(path);
	if (bNormalized)
		m_path[m_nFiles - 1].NormalizePath();
}

/**
 * @brief Set middle path.
 * @param [in] path New path for item.
 */
void PathContext::SetMiddle(LPCTSTR path, bool bNormalized)
{
	if (m_nFiles < 3)
	{
		m_nFiles = 3;
		m_path[2] = m_path[1];
	}
	m_path[1].SetPath(path);
	if (bNormalized)
		m_path[1].NormalizePath();
}

/**
 * @brief Set path
 * @param [in] index index of path to set
 * @param [in] path New path for item.
 */
void PathContext::SetPath(int index, LPCTSTR path, bool bNormalized)
{
	if (index >= m_nFiles) m_nFiles = index + 1;
	m_path[index].SetPath(path);
	if (bNormalized)
		m_path[index].NormalizePath();
}

/**
 * @brief Swap paths.
 */
void PathContext::Swap()
{
	if (m_nFiles < 3)
		m_path[0].m_sPath.swap(m_path[1].m_sPath);
	else
		m_path[0].m_sPath.swap(m_path[2].m_sPath);
}
