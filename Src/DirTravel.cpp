/** 
 * @file  DirTravel.cpp
 *
 * @brief Implementation file for Directory traversal functions.
 *
 */

#include "pch.h"
#include "DirTravel.h"
#include <algorithm>
#include <Poco/DirectoryIterator.h>
#include <Poco/Timestamp.h>
#include <windows.h>
#include "TFile.h"
#include "UnicodeString.h"
#include "DirItem.h"
#include "unicoder.h"
#include "paths.h"
#include "Win_VersionHelper.h"
#include "DebugNew.h"

using Poco::DirectoryIterator;
using Poco::Timestamp;

static void LoadFiles(const String& sDir, DirItemArray * dirs, DirItemArray * files);
static void Sort(DirItemArray * dirs, bool casesensitive);

/**
 * @brief Load arrays with all directories & files in specified dir
 */
void LoadAndSortFiles(const String& sDir, DirItemArray * dirs, DirItemArray * files, bool casesensitive)
{
	LoadFiles(sDir, dirs, files);
	Sort(dirs, casesensitive);
	Sort(files, casesensitive);
}

/**
 * @brief Find file and sub-folder names from given folder.
 * This function saves all file and sub-folder names in given folder to arrays.
 * We use 64-bit version of stat() to get times since find doesn't return
 * valid times for very old files (around year 1970). Even stat() seems to
 * give negative time values but we can live with that. Those around 1970
 * times can happen when file is created so that it doesn't get valid
 * creation or modification dates.
 * @param [in] sDir Base folder for files and subfolders.
 * @param [in, out] dirs Array where subfolder names are stored.
 * @param [in, out] files Array where file names are stored.
 */
static void LoadFiles(const String& sDir, DirItemArray * dirs, DirItemArray * files)
{
	boost::flyweight<String> dir(sDir);
#if 0
	DirectoryIterator it(ucr::toUTF8(sDir));
	DirectoryIterator end;

	for (; it != end; ++it)
	{
		bool bIsDirectory = it->isDirectory();
		if (bIsDirectory)
			continue;

		DirItem ent;
		ent.ctime = it->created();
		if (ent.ctime < 0)
			ent.ctime = 0;
		ent.mtime = it->getLastModified();
		if (ent.mtime < 0)
			ent.mtime = 0;
		ent.size = it->getSize();
		ent.path = dir;
		ent.filename = ucr::toTString(it.name());
		ent.flags.attributes = GetFileAttributes(ucr::toTString(it.name()).c_str());		
		(bIsDirectory ? dirs : files)->push_back(ent);
	}

#else
	String sPattern = paths::ConcatPath(sDir, _T("*.*"));

	WIN32_FIND_DATA ff;
	HANDLE h;
	if (IsWin7_OrGreater())	// (also 'Windows Server 2008 R2' and greater) for FindExInfoBasic and FIND_FIRST_EX_LARGE_FETCH
		h = FindFirstFileEx(TFile(sPattern).wpath().c_str(), FindExInfoBasic, &ff, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
	else
		h = FindFirstFile(TFile(sPattern).wpath().c_str(), &ff);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			bool bIsDirectory = (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
			if (bIsDirectory && tc::tcsstr(_T(".."), ff.cFileName))
				continue;

			DirItem ent;

			// Save filetimes as seconds since January 1, 1970
			// Note that times can be < 0 if they are around that 1970..
			// Anyway that is not sensible case for normal files so we can
			// just use zero for their time.
			ent.ctime = Timestamp::fromFileTimeNP(ff.ftCreationTime.dwLowDateTime, ff.ftCreationTime.dwHighDateTime);
			if (ent.ctime < 0)
				ent.ctime = 0;
			ent.mtime = Timestamp::fromFileTimeNP(ff.ftLastWriteTime.dwLowDateTime, ff.ftLastWriteTime.dwHighDateTime);
			if (ent.mtime < 0)
				ent.mtime = 0;

			if (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				ent.size = DirItem::FILE_SIZE_NONE;  // No size for directories
			else
			{
				ent.size = ((int64_t)ff.nFileSizeHigh << 32) + ff.nFileSizeLow;
			}

			ent.path = dir;
			ent.filename = ff.cFileName;
			ent.flags.attributes = ff.dwFileAttributes;
			
			(bIsDirectory ? dirs : files)->push_back(ent);
		} while (FindNextFile(h, &ff));
		FindClose(h);
	}

#endif
}

static inline int collate(const String &str1, const String &str2)
{
	return tc::tcscoll(str1.c_str(), str2.c_str());
}

static inline int collate_ignore_case(const String &str1, const String &str2)
{
	return tc::tcsicoll(str1.c_str(), str2.c_str());
}


template<int (*compfunc)(const tchar_t *, const tchar_t *)>
struct StringComparer
{
	bool operator()(const DirItem &elem1, const DirItem &elem2)
	{
		return compfunc(elem1.filename.get().c_str(), elem2.filename.get().c_str()) < 0;
	}
};

/**
 * @brief sort specified array
 */
static void Sort(DirItemArray * dirs, bool casesensitive)
{
	if (casesensitive)
		std::sort(dirs->begin(), dirs->end(), StringComparer<tc::tcscoll>());
	else
		std::sort(dirs->begin(), dirs->end(), StringComparer<tc::tcsicoll>());
}

/**
 * @brief  Compare (NLS aware) two strings, either case-sensitive or
 * case-insensitive as caller specifies
 */
int collstr(const String & s1, const String & s2, bool casesensitive)
{
	if (casesensitive)
		return collate(s1, s2);
	else
		return collate_ignore_case(s1, s2);
}
