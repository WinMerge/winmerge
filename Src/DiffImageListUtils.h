/**
 * @file  DiffImageListUtils.h
 *
 * @brief Utilities for initializing list control image lists.
 */
#pragma once

class CImageList;
struct IMergeDoc;

namespace DiffImageListUtils
{
	void InitializeDiffImageList(CImageList& imageList);
	int GetDiffImageIndex(IMergeDoc* pDoc);
}
