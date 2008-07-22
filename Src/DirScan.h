/**
 *  @file DirScan.h
 *
 *  @brief Declaration of DirScan module (see DirScan function)
 */ 
// ID line follows -- this is updated by SVN
// $Id$

#ifndef DirScan_h_included
#define DirScan_h_included

class CDiffContext;
class DiffItemList;
class PathContext;
class IAbortable;
struct DIFFITEM;
struct DiffFuncStruct;

int DirScan_GetItems(const PathContext &paths, const String &leftsubdir, const String &rightsubdir, DiffFuncStruct *myStruct,
		bool casesensitive, int depth, DIFFITEM *parent);

int DirScan_CompareItems(DiffFuncStruct *);
int DirScan_CompareRequestedItems(DiffFuncStruct *);

void DirScan_InitializeDefaultCodepage();

#endif // DirScan_h_included
