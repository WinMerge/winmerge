/**
 *  @file DirScan.h
 *
 *  @brief Declaration of DirScan module (see DirScan function)
 */ 
// ID line follows -- this is updated by SVN
// $Id: DirScan.h 6136 2008-12-01 17:04:25Z kimmov $

#ifndef DirScan_h_included
#define DirScan_h_included

#include "UnicodeString.h"
#include <cstdint>

class CDiffContext;
class DiffItemList;
class PathContext;
class IAbortable;
struct DIFFITEM;
struct DiffFuncStruct;

int DirScan_GetItems(const PathContext &paths, const String subdir[], DiffFuncStruct *myStruct,
		bool casesensitive, int depth, DIFFITEM *parent, bool bUniques);

int DirScan_CompareItems(DiffFuncStruct *, uintptr_t parentdiffpos);
int DirScan_CompareRequestedItems(DiffFuncStruct *, uintptr_t parentdiffpos);

#endif // DirScan_h_included
