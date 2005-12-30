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
#include "markdown.h"

ProjectFile::ProjectFile()
{
	m_subfolders = -1;
}

/** 
 * @brief Get message from exception into sError, or else throw it.
 *
 * If this successfully extracts the error description into the string, it simply returns FALSE
 * If it fails to extract the error description, it rethrows the exception
 */
static BOOL NTAPI False(CException *e, CString *sError)
{
	if (sError == NULL)
		throw e;
	TCHAR szError[1024];
	e->GetErrorMessage(szError, 1024);
	*sError = szError;
	e->Delete();
	return FALSE;
}

/** 
 * @brief Open given path-file and read data from it to member variables.
 */
BOOL ProjectFile::Read(LPCTSTR path, CString *sError)
{
	try
	{
		CMarkdown::EntityMap entities;
		entities.Load();
		CMarkdown::File xmlfile = path;
		if (xmlfile.pImage == NULL)
		{
			CFileException::ThrowOsError(GetLastError(), path);
		}
		// If encoding is other than UTF-8, assume CP_ACP
		CMarkdown::String encoding = CMarkdown(xmlfile).Move("?xml").GetAttribute("encoding");
		UINT codepage = lstrcmpiA(encoding.A, "UTF-8") == 0 ? CP_UTF8 : CP_ACP;

		CMarkdown project = CMarkdown(xmlfile).Move("project").Pop();
		CMarkdown paths = CMarkdown(project).Move("paths").Pop();
		m_leftFile = CMarkdown::String(CMarkdown(paths).Move("left").GetInnerText()->Unicode(codepage)->Resolve(entities)).W;
		m_rightFile = CMarkdown::String(CMarkdown(paths).Move("right").GetInnerText()->Unicode(codepage)->Resolve(entities)).W;
		m_filter = CMarkdown::String(CMarkdown(paths).Move("filter").GetInnerText()->Unicode(codepage)->Resolve(entities)).W;
		sscanf(CMarkdown::String(CMarkdown(paths).Move("subfolders").GetInnerText()).A, "%d", &m_subfolders);
	}
	catch (CException *e)
	{
		return False(e, sError);
	}
	return TRUE;
}

/** 
 * @brief Save data from member variables to path-file.
 * @note paths are converted to UTF-8
 */
BOOL ProjectFile::Save(LPCTSTR path, CString *sError)
{
	try
	{
		static const char szFormat[]
		(
			"<?xml version='1.0' encoding='UTF-8'?>\n"
			"<project>\n"
			"\t<paths>\n"
			"\t\t<left>%s</left>\n"
			"\t\t<right>%s</right>\n"
			"\t\t<filter>%s</filter>\n"
			"\t\t<subfolders>%d</subfolders>\n"
			"\t</paths>\n"
			"</project>\n"
		);
		fprintf
		(
			CStdioFile(path, CFile::modeCreate|CFile::modeWrite|CFile::typeText).m_pStream,
			szFormat,
			CMarkdown::String(CMarkdown::HSTR(GetLeft().AllocSysString())->Entities()->Octets(CP_UTF8)).A,
			CMarkdown::String(CMarkdown::HSTR(GetRight().AllocSysString())->Entities()->Octets(CP_UTF8)).A,
			CMarkdown::String(CMarkdown::HSTR(GetFilter().AllocSysString())->Entities()->Octets(CP_UTF8)).A,
			GetSubfolders() ? 1 : 0
		);
	}
	catch (CException *e)
	{
		return False(e, sError);
	}
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
 * @brief Set left path, returns old left path.
 */
CString ProjectFile::SetLeft(const CString& sLeft)
{
	CString sLeftOld = GetLeft();
	m_leftFile = sLeft;

	return sLeftOld;
}

/** 
 * @brief Returns right path.
 */
CString ProjectFile::GetRight() const
{
	return m_rightFile;
}

/** 
 * @brief Set right path, returns old right path.
 */
CString ProjectFile::SetRight(const CString& sRight)
{
	CString sRightOld = GetRight();
	m_rightFile = sRight;

	return sRightOld;
}

/** 
 * @brief Returns filter.
 */
CString ProjectFile::GetFilter() const
{
	return m_filter;
}

/** 
 * @brief Set filter, returns old filter.
 */
CString ProjectFile::SetFilter(const CString& sFilter)
{
	CString sFilterOld = GetFilter();
	m_filter = sFilter;

	return sFilterOld;
}

/** 
 * @brief Returns subfolder included -setting.
 */
int ProjectFile::GetSubfolders() const
{
	return m_subfolders;
}

/** 
 * @brief set subfolder, returns old subfolder value.
 */
int ProjectFile::SetSubfolders(const int iSubfolder)
{
	int iSubfoldersOld = GetSubfolders(); 
	m_subfolders = iSubfolder ? 1 : 0;

	return iSubfoldersOld;
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
