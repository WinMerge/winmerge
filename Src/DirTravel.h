/** 
 * @file  DirTravel.h
 *
 * @brief Declaration file for Directory traversal functions.
 *
 */
#pragma once

#include <vector>
#include "UnicodeString.h"

struct DirItem;

typedef std::vector<DirItem> DirItemArray;

namespace DirTravel
{
	void LoadAndSortFiles(const String& sDir, DirItemArray* dirs, DirItemArray* files, bool casesensitive);
	void LoadFiles(const String& sDir, DirItemArray* dirs, DirItemArray* files, const String & = _T("*.*"));
}

int collstr(const String& s1, const String& s2, bool casesensitive);
