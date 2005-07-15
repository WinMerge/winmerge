/**
 *  @file DirScan.h
 *
 *  @brief Declaration of DirScan module (see DirScan function)
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef DirScan_h_included
#define DirScan_h_included

class CDiffContext;
class DiffItemList;
class PathContext;

/** @brief callback to check if dirscan needs to abort */
class IAbortable
{
public:
	virtual bool ShouldAbort() = 0;
};

int DirScan_GetItems(const PathContext &paths, const CString & leftsubdir, const CString & rightsubdir, DiffItemList * pLst,
		bool casesensitive, int depth, CDiffContext * pCtxt, IAbortable * piAbortable);

int DirScan_CompareItems(DiffItemList & list, CDiffContext * pCtxt, IAbortable * piAbortable);
int DirScan_CompareItems(CDiffContext * pCtxt, IAbortable * piAbortable);

void DirScan_InitializeDefaultCodepage();

#endif // DirScan_h_included
