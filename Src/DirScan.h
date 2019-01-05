/**
 *  @file DirScan.h
 *
 *  @brief Declaration of DirScan module (see DirScan function)
 */ 
#pragma once

#include "UnicodeString.h"

class CDiffContext;
class DiffItemList;
class PathContext;
class IAbortable;
class DIFFITEM;
struct DiffFuncStruct;

int DirScan_GetItems(const PathContext &paths, const String subdir[], DiffFuncStruct *myStruct,
		bool casesensitive, int depth, DIFFITEM *parent, bool bUniques);
int DirScan_UpdateMarkedItems(DiffFuncStruct *myStruct, DIFFITEM *parentdiffpos);

int DirScan_CompareItems(DiffFuncStruct *, DIFFITEM *parentdiffpos);
int DirScan_CompareRequestedItems(DiffFuncStruct *, DIFFITEM *parentdiffpos);
