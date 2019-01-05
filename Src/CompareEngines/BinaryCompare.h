/**
 * @file  BinaryCompare.h
 *
 * @brief Declaration file for BinaryCompare compare engine.
 */
#pragma once

class DIFFITEM;
class PathContext;

namespace CompareEngines
{

/**
 * @brief A time/size compare class.
 * This compare method compares files by their times and sizes.
 */
class BinaryCompare
{
public:
	BinaryCompare();
	~BinaryCompare();
	int CompareFiles(const PathContext& files, const DIFFITEM &di) const;
};

} // namespace CompareEngines
