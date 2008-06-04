/**
 *  @file DirScan.h
 *
 *  @brief Declaration of DirScan module (see DirScan function)
 */ 
// ID line follows -- this is updated by SVN
// $Id$

#ifndef DirScan_h_included
#define DirScan_h_included

#include <vector>

class CDiffContext;
class DiffItemList;
class PathContext;
class IAbortable;
struct DIFFITEM;

int DirScan_GetItems(const PathContext &paths, LPCTSTR leftsubdir,
		LPCTSTR rightsubdir, std::vector<DIFFITEM*> * pList,
		bool casesensitive, int depth, CDiffContext * pCtxt);

int DirScan_CompareItems(std::vector<DIFFITEM*> * pList, CDiffContext * pCtxt);
int DirScan_CompareItems(CDiffContext * pCtxt);

void DirScan_InitializeDefaultCodepage();

#endif // DirScan_h_included
