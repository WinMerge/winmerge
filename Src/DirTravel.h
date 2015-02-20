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

void LoadAndSortFiles(const String& sDir, DirItemArray * dirs, DirItemArray * files, bool casesensitive);
int collstr(const String & s1, const String & s2, bool casesensitive);
