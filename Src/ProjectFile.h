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
#pragma once

#include "UnicodeString.h"
#include "PathContext.h"

class ProjectFileItem
{
	friend class ProjectFile;
	friend class ProjectFileHandler;
public:
	ProjectFileItem();
	bool HasLeft() const;
	bool HasMiddle() const;
	bool HasRight() const;
	bool HasFilter() const;
	bool HasSubfolders() const;

	String GetLeft(bool * pReadOnly = nullptr) const;
	bool GetLeftReadOnly() const;
	String GetMiddle(bool * pReadOnly = nullptr) const;
	bool GetMiddleReadOnly() const;
	String GetRight(bool * pReadOnly = nullptr) const;
	bool GetRightReadOnly() const;
	String GetFilter() const;
	int GetSubfolders() const;

	void SetLeft(const String& sLeft, const bool * pReadOnly = nullptr);
	void SetMiddle(const String& sMiddle, const bool * pReadOnly = nullptr);
	void SetRight(const String& sRight, const bool * pReadOnly = nullptr);
	void SetFilter(const String& sFilter);
	void SetSubfolders(bool bSubfolder);

	void GetPaths(PathContext& files, bool & bSubFolders) const;
	void SetPaths(const PathContext& files, bool bSubFolders = false);

private:
	PathContext m_paths;
	bool m_bHasLeft; /**< Has left path? */
	bool m_bHasMiddle; /**< Has middle path? */
	bool m_bHasRight; /**< Has right path? */
	bool m_bHasFilter; /**< Has filter? */
	String m_filter; /**< Filter name or mask */
	bool m_bHasSubfolders; /**< Has subfolders? */
	int m_subfolders; /**< Are subfolders included (recursive scan) */
	bool m_bLeftReadOnly; /**< Is left path opened as read-only */
	bool m_bMiddleReadOnly; /**< Is middle path opened as read-only */
	bool m_bRightReadOnly; /**< Is right path opened as read-only */
};

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
	bool Read(const String& path);
	bool Save(const String& path) const;
	const std::list<ProjectFileItem>& Items() const { return m_items; };
	std::list<ProjectFileItem>& Items() { return m_items; }
	static const String PROJECTFILE_EXT;
private:
	std::list<ProjectFileItem> m_items;
};

/** 
 * @brief Returns if left path is defined in project file.
 * @return true if project file has left path.
 */
inline bool ProjectFileItem::HasLeft() const
{
	return m_bHasLeft;
}

/** 
 * @brief Returns if middle path is defined.
 */
inline bool ProjectFileItem::HasMiddle() const
{
	return m_bHasMiddle;
}

/** 
 * @brief Returns if right path is defined in project file.
 * @return true if project file has right path.
 */
inline bool ProjectFileItem::HasRight() const
{
	return m_bHasRight;
}

/** 
 * @brief Returns if filter is defined in project file.
 * @return true if project file has filter.
 */
inline bool ProjectFileItem::HasFilter() const
{
	return m_bHasFilter;
}

/** 
 * @brief Returns if subfolder is defined in projectfile.
 * @return true if project file has subfolder definition.
 */
inline bool ProjectFileItem::HasSubfolders() const
{
	return m_bHasSubfolders;
}

/** 
 * @brief Returns if left path is specified read-only.
 * @return true if left path is read-only, false otherwise.
 */
inline bool ProjectFileItem::GetLeftReadOnly() const
{
	return m_bLeftReadOnly;
}

/** 
 * @brief Returns if middle path is specified read-only.
 */
inline bool ProjectFileItem::GetMiddleReadOnly() const
{
	return m_bMiddleReadOnly;
}

/** 
 * @brief Returns if right path is specified read-only.
 * @return true if right path is read-only, false otherwise.
 */
inline bool ProjectFileItem::GetRightReadOnly() const
{
	return m_bRightReadOnly;
}

/** 
 * @brief Returns filter.
 * @return Filter string.
 */
inline String ProjectFileItem::GetFilter() const
{
	return m_filter;
}

/** 
 * @brief Set filter.
 * @param [in] sFilter New filter string to set.
 */
inline void ProjectFileItem::SetFilter(const String& sFilter)
{
	m_filter = sFilter;
}

/** 
 * @brief Returns subfolder included -setting.
 * @return != 0 if subfolders are included.
 */
inline int ProjectFileItem::GetSubfolders() const
{
	return m_subfolders;
}

/** 
 * @brief set subfolder.
 * @param [in] iSubfolder New value for subfolder inclusion.
 */
inline void ProjectFileItem::SetSubfolders(bool bSubfolder)
{
	m_subfolders = bSubfolder ? 1 : 0;
}

/** 
 * @brief 
 *
 * @param [in] files Files in project
 * @param [in] bSubFolders If true subfolders included (recursive compare)
 */
inline void ProjectFileItem::SetPaths(const PathContext& files, bool bSubfolders)
{
	m_paths = files;
	m_subfolders = bSubfolders;
}

