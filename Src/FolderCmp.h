/** 
 * @file  FolderCmp.h
 *
 * @brief Declaration file for FolderCmp
 */
#pragma once

#include <memory>
#include "DiffFileData.h"
#include "Wrap_DiffUtils.h"
#include "ByteCompare.h"
#include "BinaryCompare.h"
#include "TimeSizeCompare.h"
#include "PathContext.h"

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
	explicit FolderCmp(CDiffContext *pCtxt);
	~FolderCmp();
	bool RunPlugins(PluginsContext * plugCtxt, String &errStr);
	void CleanupAfterPlugins(PluginsContext *plugCtxt);
	int prepAndCompareFiles(DIFFITEM &di);

	int m_ndiffs;
	int m_ntrivialdiffs;

	DiffFileData m_diffFileData;
	CDiffContext *const m_pCtxt;

private:
	std::unique_ptr<CompareEngines::DiffUtils> m_pDiffUtilsEngine;
	std::unique_ptr<CompareEngines::ByteCompare> m_pByteCompare;
	std::unique_ptr<CompareEngines::BinaryCompare> m_pBinaryCompare;
	std::unique_ptr<CompareEngines::TimeSizeCompare> m_pTimeSizeCompare;
};
