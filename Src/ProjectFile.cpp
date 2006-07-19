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
#include "XmlDoc.h"


/** 
 * @brief Standard constructor.
 */
 ProjectFile::ProjectFile()
: m_subfolders(-1)
, m_bLeftReadOnly(FALSE)
, m_bRightReadOnly(FALSE)
{
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
 * @param [in] path Path to project file.
 * @param [out] sError Error string if error happened.
 * @return TRUE if reading succeeded, FALSE if error happened.
 */
BOOL ProjectFile::Read(LPCTSTR path, CString *sError)
{
	return Serialize(false, path, sError);
}

/** 
 * @brief Save data from member variables to path-file.
 * @param [in] path Path to project file.
 * @param [out] sError Error string if error happened.
 * @return TRUE if saving succeeded, FALSE if error happened.
 * @note paths are converted to UTF-8
 */
BOOL ProjectFile::Save(LPCTSTR path, CString *sError)
{
	return Serialize(true, path, sError);
}

	
/** 
 * @brief Read or write project file
 * @param [in] writing TRUE if project file is saved, FALSE if it is loaded.
 * @param [in] path Path to project file.
 * @param [out] sError Error string if error happened.
 * @return TRUE if operation succeeded, FALSE if error happened.
 */
BOOL ProjectFile::Serialize(bool writing, LPCTSTR path, CString *sError)
{
	int leftReadOnly = m_bLeftReadOnly ? 1 : 0;
	int rightReadOnly = m_bRightReadOnly ? 1 : 0;

	try
	{
		XmlDoc::XML_LOADSAVE loadSave = (writing ? XmlDoc::XML_SAVE : XmlDoc::XML_LOAD);

		XmlDoc doc(path, loadSave, _T("UTF-8"));
		doc.Begin();
		{
			XmlElement project(doc, _T("project"));
			{
				XmlElement paths(doc, _T("paths"));
				{
					XmlElement(doc, _T("left"), m_leftFile);
				} {
					XmlElement(doc, _T("left-readonly"), leftReadOnly);
				} {
					XmlElement(doc, _T("right"), m_rightFile);
				} {
					XmlElement(doc, _T("right-readonly"), rightReadOnly);
				} {
					XmlElement(doc, _T("filter"), m_filter);
				} {
					XmlElement(doc, _T("subfolders"), m_subfolders);
				}
			}
		}
		doc.End();

	}
	catch (CException *e)
	{
		return False(e, sError);
	}

	if (!writing)
	{
		m_bLeftReadOnly = (leftReadOnly == 1);
		m_bRightReadOnly = (rightReadOnly == 1);
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
 * @param [out] pReadOnly TRUE if readonly was specified for path.
 */
CString ProjectFile::GetLeft(BOOL * pReadOnly /*=NULL*/) const
{
	if (pReadOnly)
		*pReadOnly = m_bLeftReadOnly;
	return m_leftFile;
}

/** 
 * @brief Returns if left path is specified read-only.
 */
BOOL ProjectFile::GetLeftReadOnly() const
{
	return m_bLeftReadOnly;
}

/** 
 * @brief Set left path, returns old left path.
 * @param [in] sLeft Left path.
 * @param [in] bReadOnly Will path be recorded read-only?
 */
CString ProjectFile::SetLeft(const CString& sLeft, const BOOL * pReadOnly /*=NULL*/)
{
	CString sLeftOld = GetLeft();
	m_leftFile = sLeft;
	if (pReadOnly)
		m_bLeftReadOnly = *pReadOnly;

	return sLeftOld;
}

/** 
 * @brief Returns right path.
 * @param [out] pReadOnly TRUE if readonly was specified for path.
 */
CString ProjectFile::GetRight(BOOL * pReadOnly /*=NULL*/) const
{
	if (pReadOnly)
		*pReadOnly = m_bRightReadOnly;
	return m_rightFile;
}

/** 
 * @brief Returns if right path is specified read-only.
 */
BOOL ProjectFile::GetRightReadOnly() const
{
	return m_bRightReadOnly;
}

/** 
 * @brief Set right path, returns old right path.
 * @param [in] sRight Right path.
 * @param [in] bReadOnly Will path be recorded read-only?
 */
CString ProjectFile::SetRight(const CString& sRight, const BOOL * pReadOnly /*=NULL*/)
{
	CString sRightOld = GetRight();
	m_rightFile = sRight;
	if (pReadOnly)
		m_bRightReadOnly = *pReadOnly;

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
