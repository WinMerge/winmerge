/**
 * @file  ImageCompare.h
 *
 * @brief Declaration file for ImageCompare compare engine.
 */
#pragma once

#include <memory>
#include "UnicodeString.h"
#include <Windows.h>

class DIFFITEM;
class PathContext;
struct IImgMergeWindow;

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
    HMODULE m_hModule;
};

} // namespace CompareEngines
