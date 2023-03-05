// SPDX-License-Identifier: GPL-2.0-or-later
/** 
 * @file  FileFilterHelper.h
 *
 * @brief Declaration file for FileFilterHelper
 */
#pragma once

#include <vector>
#include <memory>
#include "UnicodeString.h"
#include "DirItem.h"

class FileFilterMgr;
class FilterList;
struct FileFilter;

/**
 * @brief File extension of file filter files.
 */
inline const tchar_t FileFilterExt[] = _T(".flt");

/**
 * @brief Helper structure for UI and management of filters.
 *
 * This structure is mostly used as UI aid and to manage filters from UI.
 * fileinfo contains time of filter file's last modification time. By
 * comparing fileinfo to filter file in disk we can determine if file
 * is changed since we last time loaded it.
 */
struct FileFilterInfo
{
	String name; 			/**< Name of filter */
	String description; 	/**< Description of filter (shown in UI) */
	String fullpath;		/**< Full path to filter file */
	DirItem fileinfo;		/**< For tracking if file has been modified */
};

/// Interface for testing files & directories for exclusion, as diff traverses file tree
class IDiffFilter
{
public:
	virtual bool includeFile(const String& szFileName) const = 0;
	virtual bool includeDir(const String& szDirName) const = 0;
	bool includeFile(const String& szFileName1, const String& szFileName2) const
	{
		if (!szFileName1.empty())
			return includeFile(szFileName1);
		else if (!szFileName2.empty())
			return includeFile(szFileName2);
		else
			return false;
	}
	bool includeFile(const String& szFileName1, const String& szFileName2, const String& szFileName3) const
	{
		if (!szFileName1.empty())
			return includeFile(szFileName1);
		else if (!szFileName2.empty())
			return includeFile(szFileName2);
		else if (!szFileName3.empty())
			return includeFile(szFileName3);
		else
			return false;
	}
	bool includeDir(const String& szDirName1, const String& szDirName2) const
	{
		if (!szDirName1.empty())
			return includeDir(szDirName1);
		else if (!szDirName2.empty())
			return includeDir(szDirName2);
		else
			return false;
	}
	bool includeDir(const String& szDirName1, const String& szDirName2, const String& szDirName3) const
	{
		if (!szDirName1.empty())
			return includeDir(szDirName1);
		else if (!szDirName2.empty())
			return includeDir(szDirName2);
		else if (!szDirName3.empty())
			return includeDir(szDirName3);
		else
			return false;
	}
};

/**
 * @brief Helper class for using filefilters.
 *
 * A FileFilterHelper object is the owner of any active mask, and of the file filter manager
 * This is kind of a File Filter SuperManager, taking care of both inline filters from strings
 *  and loaded file filters (the latter handled by its internal file filter manager)
 *
 * This class is mainly for handling two ways to filter files in WinMerge:
 * - File masks: *.ext lists (*.cpp *.h etc)
 * - File filters: regular expression rules in separate files
 *
 * There can be only one filter or file mask active at a time. This class
 * keeps track of selected filtering method and provides simple functions for
 * clients for querying if file is included to compare. Clients don't need
 * to care about compare methods etc details.
 */
class FileFilterHelper : public IDiffFilter
{
public:
	FileFilterHelper();
	~FileFilterHelper();

	String GetGlobalFilterPathWithCreate() const;
	String GetUserFilterPathWithCreate() const;

	FileFilterMgr * GetManager() const;
	void SetFileFilterPath(const String& szFileFilterPath);
	std::vector<FileFilterInfo> GetFileFilters(String & selected) const;
	String GetFileFilterName(const String& filterPath) const;
	String GetFileFilterPath(const String& filterName) const;
	void SetUserFilterPath(const String & filterPath);

	void ReloadUpdatedFilters();
	void LoadAllFileFilters();

	void LoadFileFilterDirPattern(const String& dir, const String& szPattern);

	void UseMask(bool bUseMask);
	void SetMask(const String& strMask);

	bool IsUsingMask() const;
	String GetFilterNameOrMask() const;
	bool SetFilter(const String &filter);

	bool includeFile(const String& szFileName) const override;
	bool includeDir(const String& szDirName) const override;

	void CloneFrom(const FileFilterHelper* pHelper);

protected:
	std::tuple<String, String, String, String> ParseExtensions(const String &extensions) const;

private:
	std::unique_ptr<FilterList> m_pMaskFileFilter; /*< Filter for filemasks (*.cpp) */
	std::unique_ptr<FilterList> m_pMaskDirFilter;  /*< Filter for dirmasks */
	FileFilter * m_currentFilter;     /*< Currently selected filefilter */
	std::unique_ptr<FileFilterMgr> m_fileFilterMgr;  /*< Associated FileFilterMgr */
	String m_sFileFilterPath;        /*< Path to current filter */
	String m_sMask;   /*< File mask (if defined) "*.cpp *.h" etc */
	bool m_bUseMask;   /*< If `true` file mask is used, filter otherwise */
	String m_sGlobalFilterPath;    /*< Path for shared filters */
	String m_sUserSelFilterPath;     /*< Path for user's private filters */
};

/**
 * @brief Return filtermanager used.
 */
inline FileFilterMgr * FileFilterHelper::GetManager() const
{
	return m_fileFilterMgr.get();
}

/**
 * @brief Returns true if active filter is a mask.
 */
inline bool FileFilterHelper::IsUsingMask() const
{
	return m_bUseMask;
}

