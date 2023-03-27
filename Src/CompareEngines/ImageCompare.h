/**
 * @file  ImageCompare.h
 *
 * @brief Declaration file for ImageCompare compare engine.
 */
#pragma once

#include "UnicodeString.h"

class DIFFITEM;
class PathContext;
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
	ImageCompare();
	~ImageCompare();
	int CompareFiles(const PathContext& files, const DIFFITEM &di) const;

    double GetColorDistanceThreshold() const { return m_colorDistanceThreshold; }
    void SetColorDistanceThreshold(double colorDistanceThreshold) { m_colorDistanceThreshold = colorDistanceThreshold; };
private:
    int compare_files(const String& file1, const String& file2) const;
    mutable IImgMergeWindow *m_pImgMergeWindow;
    double m_colorDistanceThreshold;
    struct HINSTANCE__* m_hModule;
};

} // namespace CompareEngines
