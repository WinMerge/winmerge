/** 
 * @file  FolderCmp.h
 *
 * @brief Declaration file for FolderCmp
 */
// ID line follows -- this is updated by SVN
// $Id: FolderCmp.h 6856 2009-06-25 06:34:42Z kimmov $

#ifndef _FOLDERCMP_H_
#define _FOLDERCMP_H_

#include "DiffFileData.h"
#include "DiffUtils.h"
#include "ByteCompare.h"
#include "TimeSizeCompare.h"

class CDiffContext;
class PackingInfo;
class PrediffingInfo;

/**
 * @brief Holds plugin-related paths and information.
 */
struct PluginsContext
{
	PathContext origFileName;
	PathContext filepathUnpacked;
	PathContext filepathTransformed;
	PackingInfo * infoUnpacker;
	PrediffingInfo * infoPrediffer;
};

/**
 * @brief Class implementing file compare for folder compare.
 * This class implements (called from DirScan.cpp) compare of two files
 * during folder compare. The class implements both diffutils compare and
 * quick compare.
 */
class FolderCmp
{
public:
	FolderCmp();
	~FolderCmp();
	bool RunPlugins(CDiffContext * pCtxt, PluginsContext * plugCtxt, String &errStr);
	void CleanupAfterPlugins(PluginsContext *plugCtxt);
	int prepAndCompareFiles(CDiffContext * pCtxt, DIFFITEM &di);
	void SetCodepage(int codepage) { m_codepage = codepage; }

	int m_ndiffs;
	int m_ntrivialdiffs;
	int m_codepage;

	DiffFileData m_diffFileData;
	CDiffContext * m_pCtx;

private:
	CompareEngines::DiffUtils *m_pDiffUtilsEngine;
	CompareEngines::ByteCompare *m_pByteCompare;
	CompareEngines::TimeSizeCompare *m_pTimeSizeCompare;
};


#endif // _FOLDERCMP_H_
