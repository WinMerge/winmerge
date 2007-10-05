/** 
 * @file  DirTravel.h
 *
 * @brief Declaration file for Directory traversal functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _DIR_TRAVEL_H_
#define _DIR_TRAVEL_H_

#include <vector>
#include "UnicodeString.h"

struct DirItem;

typedef std::vector<DirItem> DirItemArray;

void LoadAndSortFiles(LPCTSTR sDir, DirItemArray * dirs, DirItemArray * files, bool casesensitive);
int collstr(const String & s1, const String & s2, bool casesensitive);

#endif // _DIR_TRAVEL_H_
