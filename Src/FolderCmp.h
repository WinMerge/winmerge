/** 
 * @file  FolderCmp.h
 *
 * @brief Declaration file for FolderCmp
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _FOLDERCMP_H_
#define _FOLDERCMP_H_

#include "DiffFileData.h"
#include "DiffUtils.h"
#include "ByteCompare.h"

//using namespace CompareEngines;

class CDiffContext;
class IAbortable;
class PackingInfo;
class PrediffingInfo;
//class CompareEngines::DiffUtils;

/**
 * @brief Holds plugin-related paths and information.
 */
struct PluginsContext
{
	CString origFileName1;
	CString origFileName2;
	CString filepathUnpacked1;
	CString filepathUnpacked2;
	CString filepathTransformed1;
	CString filepathTransformed2;
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
	bool RunPlugins(CDiffContext * pCtxt, PluginsContext * plugCtxt, CString &errStr);
	void CleanupAfterPlugins(PluginsContext *plugCtxt);
	int prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di);
	int byte_compare_files(BOOL bStopAfterFirstDiff, const IAbortable * piAbortable);

	int m_ndiffs;
	int m_ntrivialdiffs;

	DiffFileData m_diffFileData;
	CDiffContext * m_pCtx;

private:
	CompareEngines::DiffUtils *m_pDiffUtilsEngine;
	CompareEngines::ByteCompare *m_pByteCompare;
};


#endif // _FOLDERCMP_H_
