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
 * @file  ProjectFile.cpp
 *
 * @brief Implementation file for ProjectFile class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "ProjectFile.h"

ProjectFile::ProjectFile()
{
	m_subfolders = -1;
}

/** 
 * @brief Open given path-file and read data from it to member variables.
 */
BOOL ProjectFile::Read(LPCTSTR path, CString *sError)
{
	ASSERT(sError != NULL);
	CFile file;
	CFileException e;

	if (!file.Open(path, CFile::modeRead, &e))
	{
		TCHAR szError[1024];
		e.GetErrorMessage(szError, 1024);
		*sError = szError;
		return FALSE;
	}

	char buf[4096] = {0};
	TCHAR buf2[4096] = {0};
	TCHAR tmpPath[MAX_PATH] = {0};
	UINT bytesRead = file.Read(buf, 4095);

	USES_CONVERSION;
	_tcsncpy(buf2, A2T(buf), 4096);

	if (_tcsstr(buf2, _T("<?xml")) && _tcsstr(buf2, _T("?>")))
	{
		TCHAR *pProject = _tcsstr(buf2, _T("<project>"));
		
		if (pProject)
		{
			TCHAR *pPaths = _tcsstr(buf2, _T("<paths>"));
			TCHAR *pLeft = _tcsstr(buf2, _T("<left>"));
			TCHAR *pRight = _tcsstr(buf2, _T("<right>"));
			TCHAR *pFilter = _tcsstr(buf2, _T("<filter>"));
			TCHAR *pSubs = _tcsstr(buf2, _T("<subfolders>"));

			CString subs;
			GetVal(pPaths, pLeft, &m_leftFile, _T("<left>"), _T("</left>"), buf2);
			GetVal(pPaths, pRight, &m_rightFile, _T("<right>"), _T("</right>"), buf2);
			GetVal(pPaths, pFilter, &m_filter, _T("<filter>"), _T("</filter>"), buf2);
			if (GetVal(pPaths, pSubs, &subs, _T("<subfolders>"), _T("</subfolders>"), buf2))
				m_subfolders = _ttoi(subs);
		}
	}

	file.Close();

	return TRUE;
}

/** 
 * @brief Save data from member variables to path-file.
 */
BOOL ProjectFile::Save(LPCTSTR path)
{
	UINT flags = CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite;

	return TRUE;
}

/** 
 * @brief Returns if left path is defined.
 */
BOOL ProjectFile::HasLeft() const
{
	return !m_leftFile.IsEmpty();
}

/** 
 * @brief Returns if right path is defined.
 */
BOOL ProjectFile::HasRight() const
{
	return !m_rightFile.IsEmpty();
}

/** 
 * @brief Returns if filter is defined.
 */
BOOL ProjectFile::HasFilter() const
{
	return !m_filter.IsEmpty();
}

/** 
 * @brief Returns if subfolder is included.
 */
BOOL ProjectFile::HasSubfolders() const
{
	return (m_subfolders != -1);
}

/** 
 * @brief Returns left path.
 */
CString ProjectFile::GetLeft() const
{
	return m_leftFile;
}

/** 
 * @brief Returns right path.
 */
CString ProjectFile::GetRight() const
{
	return m_rightFile;
}

/** 
 * @brief Returns filter.
 */
CString ProjectFile::GetFilter() const
{
	return m_filter;
}

/** 
 * @brief Returns subfolder included -setting.
 */
int ProjectFile::GetSubfolders() const
{
	return m_subfolders;
}

/** 
 * @brief Reads one value from XML data.
 */
BOOL ProjectFile::GetVal(TCHAR *pPaths, TCHAR *pVal, CString * sval,
		TCHAR *ptag1, TCHAR *ptag2, TCHAR *pbuf)
{
	if (pPaths && pVal && pVal > pPaths)
	{
		TCHAR tmpPath[MAX_PATH] = {0};
		TCHAR *pTagEnd = _tcsstr(pbuf, ptag2);
		if ((pTagEnd - pVal) < (MAX_PATH * sizeof(TCHAR)))
		{
			pVal += _tcslen(ptag1);
			_tcsncpy(tmpPath, pVal, pTagEnd - pVal);
			*sval = tmpPath;
			return TRUE;
		}
	}
	return FALSE;
}

/** 
 * @brief Returns left and right paths and recursive from project file
 * 
 * @param [out] sLeft Left path
 * @param [out] sRight Right path
 * @param [out] bSubFolders If TRUE subfolders included (recursive compare)
 */
void ProjectFile::GetPaths(CString & sLeft, CString & sRight,
	BOOL & bSubfolders) const
{
	if (HasLeft())
		sLeft = GetLeft();
	if (HasRight())
		sRight = GetRight();
	if (HasSubfolders())
		bSubfolders = (GetSubfolders() == 1);
}
