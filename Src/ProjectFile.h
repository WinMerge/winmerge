// SPDX-License-Identifier: GPL-2.0-or-later
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
	bool HasUnpacker() const;
	bool HasPrediffer() const;

	String GetLeft(bool * pReadOnly = nullptr) const;
	bool GetLeftReadOnly() const;
	String GetMiddle(bool * pReadOnly = nullptr) const;
	bool GetMiddleReadOnly() const;
	String GetRight(bool * pReadOnly = nullptr) const;
	bool GetRightReadOnly() const;
	String GetFilter() const;
	int GetSubfolders() const;
	String GetUnpacker() const;
	String GetPrediffer() const;

	void SetLeft(const String& sLeft, const bool * pReadOnly = nullptr);
	void SetMiddle(const String& sMiddle, const bool * pReadOnly = nullptr);
	void SetRight(const String& sRight, const bool * pReadOnly = nullptr);
	void SetFilter(const String& sFilter);
	void SetSubfolders(bool bSubfolder);
	void SetUnpacker(const String& sUnpacker);
	void SetPrediffer(const String& sPrediffer);

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
	bool m_bHasUnpacker; /**< Has unpacker? */
	String m_unpacker; /**< Unpacker name or pipeline */
	bool m_bHasPrediffer; /**< Has prediffer? */
	String m_prediffer; /**< Prediffer name or pipeline */
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
 * @brief Returns if unpacker is defined in projectfile.
 * @return true if project file has unpacker definition.
 */
inline bool ProjectFileItem::HasUnpacker() const
{
	return m_bHasUnpacker;
}

/** 
 * @brief Returns if prediffer is defined in projectfile.
 * @return true if project file has prediffer definition.
 */
inline bool ProjectFileItem::HasPrediffer() const
{
	return m_bHasPrediffer;
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
 * @brief Returns unpacker name or pipeline
 * @return Unpacker name or pipeline
 */
inline String ProjectFileItem::GetUnpacker() const
{
	return m_unpacker;
}

/** 
 * @brief Set unpacker name or pipeline.
 * @param [in] sUnpacker New unpacker name or pipeline to set.
 */
inline void ProjectFileItem::SetUnpacker(const String& sUnpacker)
{
	m_unpacker = sUnpacker;
}

/** 
 * @brief Returns prediffer name or pipeline
 * @return Prediffer name or pipeline
 */
inline String ProjectFileItem::GetPrediffer() const
{
	return m_prediffer;
}

/** 
 * @brief Set prediffer name or pipeline.
 * @param [in] sPrediffer New prediffer name or pipeline to set.
 */
inline void ProjectFileItem::SetPrediffer(const String& sPrediffer)
{
	m_prediffer = sPrediffer;
}

/** 
 * @brief 
 *
 * @param [in] paths Files in project
 * @param [in] bSubFolders If true subfolders included (recursive compare)
 */
inline void ProjectFileItem::SetPaths(const PathContext& paths, bool bSubfolders)
{
	m_paths = paths;
	m_subfolders = bSubfolders;
}

