/**
 * @file  ExistenceCompare.h
 *
 * @brief Declaration file for ExistenceCompare compare engine.
 */
#pragma once

class DIFFITEM;

namespace CompareEngines
{

/**
 * @brief A compare engine that compares files by their existence.
 */
class ExistenceCompare
{
public:
	ExistenceCompare();
	~ExistenceCompare();
	int CompareFiles(int compMethod, int nfiles, const DIFFITEM &di) const;
};

} // namespace CompareEngines
