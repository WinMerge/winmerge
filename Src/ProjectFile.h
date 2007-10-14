/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ProjectFile.h
 *
 * @brief Declaration file ProjectFile class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include <scew/scew.h>

/** @brief File extension for path files */
const TCHAR PROJECTFILE_EXT[] = _T("WinMerge");

/**
 * @brief Class for handling project files.
 *
 * This class loads and saves project files. Expat parser and SCEW wrapper for
 * expat are used for XML parsing. We use UTF-8 encoding so Unicode paths are
 * supported.
 */
class ProjectFile
{
public:
	ProjectFile();
	BOOL Read(LPCTSTR path, String *sError);
	BOOL Save(LPCTSTR path, String *sError);
	
	BOOL HasLeft() const;
	BOOL HasRight() const;
	BOOL HasFilter() const;
	BOOL HasSubfolders() const;

	CString GetLeft(BOOL * pReadOnly = NULL) const;
	BOOL GetLeftReadOnly() const;
	CString GetRight(BOOL * pReadOnly = NULL) const;
	BOOL GetRightReadOnly() const;
	CString GetFilter() const;
	int GetSubfolders() const;

	CString SetLeft(const CString& sLeft, const BOOL * pReadOnly = NULL);
	CString SetRight(const CString& sRight, const BOOL * pReadOnly = NULL);
	CString SetFilter(const CString& sFilter);
	int SetSubfolders(int iSubfolder);

	void GetPaths(CString & sLeft, CString & sRight, BOOL & bSubFolders) const;

protected:
	scew_element* GetRootElement(scew_tree * tree);
	BOOL GetPathsData(scew_element * parent);

	scew_element* AddPathsElement(scew_element * parent);
	BOOL AddPathsContent(scew_element * parent);

private:
	BOOL m_bHasLeft; /**< Has left path? */
	CString m_leftFile; /**< Left path */
	BOOL m_bHasRight; /**< Has right path? */
	CString m_rightFile; /**< Right path */
	BOOL m_bHasFilter; /**< Has filter? */
	CString m_filter; /**< Filter name or mask */
	BOOL m_bHasSubfolders; /**< Has subfolders? */
	int m_subfolders; /**< Are subfolders included (recursive scan) */
	BOOL m_bLeftReadOnly; /**< Is left path opened as read-only */
	BOOL m_bRightReadOnly; /**< Is right path opened as read-only */
};
