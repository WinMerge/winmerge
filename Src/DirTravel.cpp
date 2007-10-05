/** 
 * @file  DirTravel.cpp
 *
 * @brief Implementation file for Directory traversal functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include <algorithm>
#include <windows.h>
#include <tchar.h>
#include "UnicodeString.h"
#include "DirItem.h"
#include "DirTravel.h"

static void LoadFiles(LPCTSTR sDir, DirItemArray * dirs, DirItemArray * files);
static void Sort(DirItemArray * dirs, bool casesensitive);

/**
 * @brief Load arrays with all directories & files in specified dir
 */
void LoadAndSortFiles(LPCTSTR sDir, DirItemArray * dirs, DirItemArray * files, bool casesensitive)
{
	LoadFiles(sDir, dirs, files);
	Sort(dirs, casesensitive);
	Sort(files, casesensitive);
}

/**
 * @brief Convert time in type FILETIME to type int (time_t compatible).
 * @param [in] time Time in FILETIME type.
 * @return Time in time_t compiliant integer.
 */
static __int64 FiletimeToTimeT(FILETIME time)
{
	const __int64 SecsTo100ns = 10000000;
	const __int64 SecsBetweenEpochs = 11644473600;
	__int64 converted_time;
	converted_time = ((__int64)time.dwHighDateTime << 32) + time.dwLowDateTime;
	converted_time -= (SecsBetweenEpochs * SecsTo100ns);
	converted_time /= SecsTo100ns;
	return converted_time;
}

/**
 * @brief Find files and subfolders from given folder.
 * This function saves all files and subfolders in given folder to arrays.
 * We use 64-bit version of stat() to get times since find doesn't return
 * valid times for very old files (around year 1970). Even stat() seems to
 * give negative time values but we can live with that. Those around 1970
 * times can happen when file is created so that it  doesn't get valid
 * creation or modificatio dates.
 * @param [in] sDir Base folder for files and subfolders.
 * @param [in, out] dirs Array where subfolders are stored.
 * @param [in, out] files Array where files are stored.
 */
static void LoadFiles(LPCTSTR sDir, DirItemArray * dirs, DirItemArray * files)
{
	String sPattern(sDir);
	size_t len = sPattern.length();
	if (sPattern[len - 1] != '\\')
		sPattern += _T("\\*.*");
	else
        sPattern += _T("*.*");

	WIN32_FIND_DATA ff;
	HANDLE h = FindFirstFile(sPattern.c_str(), &ff);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			BOOL bIsDirectory = (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
			if (bIsDirectory && _tcsstr(_T(".."), ff.cFileName))
				continue;

			DirItem ent;
			ent.bIsDir = !!bIsDirectory;

			// Save filetimes as seconds since January 1, 1970
			// Note that times can be < 0 if they are around that 1970..
			// Anyway that is not sensible case for normal files so we can
			// just use zero for their time.
			ent.ctime = FiletimeToTimeT(ff.ftCreationTime);
			if (ent.ctime < 0)
				ent.ctime = 0;
			ent.mtime = FiletimeToTimeT(ff.ftLastWriteTime);
			if (ent.mtime < 0)
				ent.mtime = 0;

			if (ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				ent.size = -1;  // No size for directories
			else
			{
				ent.size = ((__int64)ff.nFileSizeHigh << 32) + ff.nFileSizeLow;
			}

			ent.path = sDir;
			ent.filename = ff.cFileName;
			ent.flags.attributes = ff.dwFileAttributes;
			
			(bIsDirectory ? dirs : files)->push_back(ent);
		} while (FindNextFile(h, &ff));
		FindClose(h);
	}
}

static int collate(const String &str1, const String &str2)
{
	return _tcscoll(str1.c_str(), str2.c_str());
}

/**
 * @brief case-sensitive collate function for qsorting an array
 */
static bool __cdecl cmpstring(const DirItem &elem1, const DirItem &elem2)
{
	return collate(elem1.filename, elem2.filename) < 0;
}

static int collate_ignore_case(const String &str1, const String &str2)
{
	String s1(str1);
	String s2(str2);
	int i = 0;
	for (i = 0; i < s1.length(); i++)
		s1[i] = _totlower(s1[i]);
	for (i = 0; i < s2.length(); i++)
		s2[i] = _totlower(s2[i]);
	return _tcscoll(s1.c_str(), s2.c_str());
}

/**
 * @brief case-insensitive collate function for qsorting an array
 */
static bool __cdecl cmpistring(const DirItem &elem1, const DirItem &elem2)
{
	return collate_ignore_case(elem1.filename, elem2.filename) < 0;
}

/**
 * @brief sort specified array
 */
static void Sort(DirItemArray * dirs, bool casesensitive)
{
	if (casesensitive)
        std::sort(dirs->begin(), dirs->end(), cmpstring);
	else
		std::sort(dirs->begin(), dirs->end(), cmpistring);
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
