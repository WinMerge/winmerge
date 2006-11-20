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
 * @brief Implementation file for ProjectFile class.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include <scew/scew.h>

#include "ProjectFile.h"

// Constants for xml element names
const TCHAR Root_element_name[] = _T("project");
const TCHAR Paths_element_name[] = _T("paths");
const TCHAR Left_element_name[] = _T("left");
const TCHAR Right_element_name[] = _T("right");
const TCHAR Filter_element_name[] = _T("filter");
const TCHAR Subfolders_element_name[] = _T("subfolders");
const TCHAR Left_ro_element_name[] = _T("left-readonly");
const TCHAR Right_ro_element_name[] = _T("right-readonly");

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
 * @brief Open given path-file and read data from it to member variables.
 * @param [in] path Path to project file.
 * @param [out] sError Error string if error happened.
 * @return TRUE if reading succeeded, FALSE if error happened.
 */
BOOL ProjectFile::Read(LPCTSTR path, CString *sError)
{
	BOOL loaded = FALSE;
    scew_tree* tree = NULL;
    scew_parser* parser = NULL;

    parser = scew_parser_create();
    scew_parser_ignore_whitespaces(parser, 1);

	FILE * fp = _tfopen(path, _T("r"));
	if (fp)
	{
		if (scew_parser_load_file_fp(parser, fp))
		{
			tree = scew_parser_tree(parser);

			scew_element * root = GetRootElement(tree);
			if (root)
			{
				loaded = TRUE;
				GetPathsData(root);
			};
		}
		scew_tree_free(tree);

		/* Frees the SCEW parser */
		scew_parser_free(parser);
		fclose(fp);
	}
	return loaded;
}

/** 
 * @brief Return project file XML's root element.
 * @param [in] tree XML tree we got from the parser.
 * @return Root element pointer.
 */
scew_element* ProjectFile::GetRootElement(scew_tree * tree)
{
	scew_element * root = NULL;

	if (tree != NULL)
	{
		root = scew_tree_root(tree);
	}

	if (root != NULL)
	{
		// Make sure we have correct root element
		if (_tcscmp(Root_element_name, scew_element_name(root)) != 0)
		{
			root = NULL;
		}
	}
	return root;
}

/** 
 * @brief Reads the paths data from the XML data.
 * This function reads the paths data inside given element in XML data.
 * @param [in] parent Parent element for the paths data.
 */
void ProjectFile::GetPathsData(scew_element * parent)
{
	scew_element *paths = NULL;

	if (parent != NULL)
	{
		paths = scew_element_by_name(parent, Paths_element_name);
	}

	if (paths != NULL)
	{
		scew_element *left = NULL;
		scew_element *right = NULL;
		scew_element *filter = NULL;
		scew_element *subfolders = NULL;
		scew_element *left_ro = NULL;
		scew_element *right_ro = NULL;

		left = scew_element_by_name(paths, Left_element_name);
		right = scew_element_by_name(paths, Right_element_name);
		filter = scew_element_by_name(paths, Filter_element_name);
		subfolders = scew_element_by_name(paths, Subfolders_element_name);
		left_ro = scew_element_by_name(paths, Left_ro_element_name);
		right_ro = scew_element_by_name(paths, Right_ro_element_name);

		if (left)
		{
			LPCTSTR path = NULL;
			path = scew_element_contents(left);
			m_leftFile = path;
		}
		if (right)
		{
			LPCTSTR path = NULL;
			path = scew_element_contents(right);
			m_rightFile = path;
		}
		if (filter)
		{
			LPCTSTR filtername = NULL;
			filtername = scew_element_contents(filter);
			m_filter = filtername;
		}
		if (subfolders)
		{
			LPCTSTR folders = NULL;
			folders = scew_element_contents(subfolders);
			m_subfolders = _ttoi(folders);
		}
		if (left_ro)
		{
			LPCTSTR readonly = NULL;
			readonly = scew_element_contents(left_ro);
			m_bLeftReadOnly = (_ttoi(readonly) != 0);
		}
		if (right_ro)
		{
			LPCTSTR readonly = NULL;
			readonly = scew_element_contents(right_ro);
			m_bRightReadOnly = (_ttoi(readonly) != 0);
		}
	}
}

/** 
 * @brief Save data from member variables to path-file.
 * @param [in] path Path to project file.
 * @param [out] sError Error string if error happened.
 * @return TRUE if saving succeeded, FALSE if error happened.
 */
BOOL ProjectFile::Save(LPCTSTR path, CString *sError)
{
	BOOL success = TRUE;
	scew_tree* tree = NULL;
	scew_element* root = NULL;
	scew_element* paths = NULL;

	tree = scew_tree_create();
	root = scew_tree_add_root(tree, Root_element_name);
	if (root != NULL)
	{
		paths = AddPathsElement(root);
	}
	else
		success = FALSE;

	if (paths != NULL)
	{
		AddPathsContent(paths);
	}
	else
		success = FALSE;
	
	scew_tree_set_xml_encoding(tree, _T("UTF-8"));

	// Set the XML file standalone
	scew_tree_set_xml_standalone(tree, 1);

	FILE * fp = _tfopen(path, _T("w"));
	if (fp)
	{
		if (!scew_writer_tree_fp(tree, fp))
		{
			success = FALSE;
			*sError = LoadResString(IDS_FILEWRITE_ERROR);
		}
		fclose(fp);
	}
	else
	{
		success = FALSE;
	}
	
	/* Frees the SCEW tree */
	scew_tree_free(tree);

	if (success == FALSE)
	{
		*sError = LoadResString(IDS_FILEWRITE_ERROR);
	}
	return success;
}

/**
 * @brief Add paths element into XML tree.
 * @param [in] parent Parent element for the paths element.
 * @return pointer to added paths element.
 */
scew_element* ProjectFile::AddPathsElement(scew_element * parent)
{
	scew_element* element = NULL;
	element = scew_element_add(parent, Paths_element_name);
	return element;
}

/**
 * @brief Add paths data to the XML tree.
 * This function adds our paths data to the XML tree.
 * @param [in] parent Parent element for paths data.
 * @return TRUE if we succeeded, FALSE otherwise.
 */
BOOL ProjectFile::AddPathsContent(scew_element * parent)
{
	scew_element* element = NULL;

	if (!m_leftFile.IsEmpty())
	{
		LPCTSTR path;
		element = scew_element_add(parent, Left_element_name);
		path = m_leftFile.GetBuffer(MAX_PATH);
		scew_element_set_contents(element, path);
		m_leftFile.ReleaseBuffer();
	}

	if (!m_rightFile.IsEmpty())
	{
		LPCTSTR path;
		element = scew_element_add(parent, Right_element_name);
		path = m_rightFile.GetBuffer(MAX_PATH);
		scew_element_set_contents(element, path);
		m_rightFile.ReleaseBuffer();
	}

	if (!m_filter.IsEmpty())
	{
		LPCTSTR filter;
		element = scew_element_add(parent, Filter_element_name);
		filter = m_filter.GetBuffer(MAX_PATH);
		scew_element_set_contents(element, filter);
		m_filter.ReleaseBuffer();
	}

	element = scew_element_add(parent, Subfolders_element_name);
	if (m_subfolders != 0)
		scew_element_set_contents(element, _T("1"));
	else
		scew_element_set_contents(element, _T("0"));

	element = scew_element_add(parent, Left_ro_element_name);
	if (m_bLeftReadOnly)
		scew_element_set_contents(element, _T("1"));
	else
		scew_element_set_contents(element, _T("0"));

	element = scew_element_add(parent, Right_ro_element_name);
	if (m_bRightReadOnly)
		scew_element_set_contents(element, _T("1"));
	else
		scew_element_set_contents(element, _T("0"));

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
