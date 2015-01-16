/**
 * @file  BinaryCompare.h
 *
 * @brief Declaration file for BinaryCompare compare engine.
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _BINARY_COMPARE_H_
#define _BINARY_COMPARE_H_

struct DIFFITEM;
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

#endif // _BINARY_COMPARE_H_
