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
#define POCO_NO_UNWINDOWS 1
#include <Poco/Types.h>
#include <boost/shared_ptr.hpp>

class CDiffContext;
class DiffItemList;
class PathContext;
class IAbortable;
struct DIFFITEM;
struct DiffFuncStruct;

int DirScan_GetItems(const PathContext &paths, const boost::shared_ptr<String> subdir[], DiffFuncStruct *myStruct,
		bool casesensitive, int depth, DIFFITEM *parent, bool bUniques);

int DirScan_CompareItems(DiffFuncStruct *, Poco::UIntPtr parentdiffpos);
int DirScan_CompareRequestedItems(DiffFuncStruct *, Poco::UIntPtr parentdiffpos);

#endif // DirScan_h_included
