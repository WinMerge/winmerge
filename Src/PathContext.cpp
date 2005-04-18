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


void PathInfo::SetPath(CString sPath)
{
	m_sPath = sPath;
	m_sNormalizedPath = sPath;
}

void PathInfo::NormalizePath()
{
	m_sNormalizedPath = m_sPath;
	paths_normalize(m_sNormalizedPath);
}

PathContext::PathContext()
{

}

const CString& PathContext::GetLeft(BOOL bNormalized) const
{
	return m_pathLeft.GetPath(bNormalized);
}

const CString& PathContext::GetRight(BOOL bNormalized) const
{
	return m_pathRight.GetPath(bNormalized);
}

void PathContext::SetLeft(LPCTSTR path)
{
	m_pathLeft.SetPath(path);
	m_pathLeft.NormalizePath();
}

void PathContext::SetRight(LPCTSTR path)
{
	m_pathRight.SetPath(path);
	m_pathRight.NormalizePath();
}
