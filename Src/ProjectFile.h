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

/** @brief File extension for path files */
const TCHAR PROJECTFILE_EXT[] = _T("WinMerge");

/**
 * @brief Class for handling project files.
 *
 * @todo open/save unicode paths - use UTF-8 for xml?
 */
class ProjectFile
{
public:
	ProjectFile();
	BOOL Read(LPCTSTR path, CString *sError);
	BOOL Save(LPCTSTR path, CString *sError);
	
	BOOL HasLeft() const;
	BOOL HasRight() const;
	BOOL HasFilter() const;
	BOOL HasSubfolders() const;

	CString GetLeft() const;
	CString GetRight() const;
	CString GetFilter() const;
	int GetSubfolders() const;

	CString SetLeft(const CString& sLeft);
	CString SetRight(const CString& sRight);
	CString SetFilter(const CString& sFilter);
	int SetSubfolders(const int iSubfolder);

	void GetPaths(CString & sLeft, CString & sRight, BOOL & bSubFolders) const;

protected:
	BOOL GetVal(TCHAR *pPaths, TCHAR *pVal, CString * sval,
		TCHAR *ptag1, TCHAR *ptag2, TCHAR *pbuf);

private:
	CString m_leftFile;
	CString m_rightFile;
	CString m_filter;
	int m_subfolders;
};
