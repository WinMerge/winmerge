/**
 * @file  ImageCompare.h
 *
 * @brief Declaration file for ImageCompare compare engine.
 */
#pragma once

#include "UnicodeString.h"

class CDiffContext;
class DIFFITEM;
class IAbortable;
struct IImgMergeWindow;
struct HINSTANCE__;

namespace CompareEngines
{

/**
 * @brief A image compare class.
 * This compare method compares files by their image contents.
 */
class ImageCompare
{
public:
	ImageCompare(CDiffContext& ctxt);
	~ImageCompare();
	int CompareFiles(DIFFITEM& di) const;

private:
	int compare_files(const String& file1, const String& file2, const IAbortable *piAbortable) const;
	mutable IImgMergeWindow *m_pImgMergeWindow;
	double m_colorDistanceThreshold;
	struct HINSTANCE__* m_hModule;
	CDiffContext& m_ctxt;
};

} // namespace CompareEngines
