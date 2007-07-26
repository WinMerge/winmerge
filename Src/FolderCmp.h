/** 
 * @file  FolderCmp.h
 *
 * @brief Declaration file for FolderCmp
 */

#ifndef _FOLDERCMP_H_
#define _FOLDERCMP_H_

#include "DiffFileData.h"

class CDiffContext;
class IAbortable;

/**
 * @brief Class implementing file compare for folder compare.
 * This class implements (called from DirScan.cpp) compare of two files
 * during folder compare. The class implements both diffutils compare and
 * quick compare.
 */
class FolderCmp
{
public:
	
	enum { DIFFS_UNKNOWN=-1, DIFFS_UNKNOWN_QUICKCOMPARE=-9 };

	FolderCmp();
	int prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di);
	int diffutils_compare_files(int depth);
	int byte_compare_files(BOOL bStopAfterFirstDiff, const IAbortable * piAbortable);
	BOOL Diff2Files(struct change ** diffs, int depth,
		int * bin_status, BOOL bMovedBlocks, int * bin_file);
	bool FolderCmp::RegExpFilter(int StartPos, int EndPos, int FileNo);

	int m_ndiffs;
	int m_ntrivialdiffs;

	DiffFileData m_diffFileData;
	CDiffContext * m_pCtx;
};


#endif // _FOLDERCMP_H_
