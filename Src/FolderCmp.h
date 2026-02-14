/** 
 * @file  FolderCmp.h
 *
 * @brief Declaration file for FolderCmp
 */
#pragma once

#include <memory>
#include "FullQuickCompare.h"
#include "BinaryCompare.h"
#include "TimeSizeCompare.h"
#include "ExistenceCompare.h"
#include "ImageCompare.h"

class CDiffContext;

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
	void LogError(const DIFFITEM& di);
	void prepAndCompareFiles(DIFFITEM &di);

	CDiffContext *const m_pCtxt;

private:
	std::unique_ptr<CompareEngines::FullQuickCompare> m_pFullCompare;
	std::unique_ptr<CompareEngines::FullQuickCompare> m_pQuickCompare;
	std::unique_ptr<CompareEngines::BinaryCompare> m_pBinaryCompare;
	std::unique_ptr<CompareEngines::TimeSizeCompare> m_pTimeSizeCompare;
	std::unique_ptr<CompareEngines::ExistenceCompare> m_pExistenceCompare;
	std::unique_ptr<CompareEngines::ImageCompare> m_pImageCompare;
};
