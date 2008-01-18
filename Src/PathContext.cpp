/**
 * @file  PathContext.cpp
 *
 * @brief Implementation file for PathInfo and PathContext
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

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
