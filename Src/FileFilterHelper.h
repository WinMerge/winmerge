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
class DIFFITEM;
class CDiffContext;
struct FileFilterErrorInfo;

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
	virtual void SetDiffContext(const CDiffContext* pCtxt) = 0;
	virtual std::vector<const FileFilterErrorInfo*> GetErrorList() const = 0;
	virtual bool includeFile(const String& szFileName) const = 0;
	virtual bool includeFile(const DIFFITEM& di) const = 0;
	virtual bool includeDir(const String& szDirName) const = 0;
	virtual bool includeDir(const DIFFITEM& di) const = 0;
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

	FileFilterMgr* GetManager() const;
	std::vector<FileFilterInfo> GetFileFilters() const;
	String GetFileFilterName(const String& filterPath) const;
	String GetFileFilterPath(const String& filterName) const;
	void SetUserFilterPath(const String& filterPath);

	void ReloadUpdatedFilters();
	void LoadAllFileFilters();

	void LoadFileFilterDirPattern(const String& dir, const String& szPattern);

	const String& GetMaskOrExpression() const { return m_sMask; }
	void SetMaskOrExpression(const String& strMask);

	static std::vector<String> SplitFilterGroups(const String& filterGroups);
	static String JoinFilterGroups(const std::vector<String>& filterGroups);

	void SetDiffContext(const CDiffContext* pCtxt) override;
	std::vector<const FileFilterErrorInfo*> GetErrorList() const override;
	bool includeFile(const String& szFileName) const override;
	bool includeFile(const DIFFITEM& di) const override;
	bool includeDir(const String& szDirName) const override;
	bool includeDir(const DIFFITEM& di) const override;

	void CloneFrom(const FileFilterHelper* pHelper);

	std::vector<String> GetPropertyNames() const;

protected:
	struct FilterGroup
	{
		std::unique_ptr<FilterList> m_pMaskFileFilter; /*< Filter for filemasks (*.cpp) */
		std::unique_ptr<FilterList> m_pMaskFileFilterExclude; /*< Filter for filemasks (*.cpp) */
		std::unique_ptr<FilterList> m_pMaskDirFilter;  /*< Filter for dirmasks */
		std::unique_ptr<FilterList> m_pMaskDirFilterExclude;  /*< Filter for dirmasks */
		std::shared_ptr<FileFilter> m_pRegexOrExpressionFilter;
		std::shared_ptr<FileFilter> m_pRegexOrExpressionFilterExclude;
	};
	std::vector<FilterGroup> ParseExtensions(const String& extensions) const;

private:
	std::vector<FilterGroup> m_filterGroups; /*< List of filter groups, each with its own filters */
	std::unique_ptr<FileFilterMgr> m_fileFilterMgr;  /*< Associated FileFilterMgr */
	String m_sMask;   /*< File mask (if defined) "*.cpp *.h" etc */
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
