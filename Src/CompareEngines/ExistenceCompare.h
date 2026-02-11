/**
 * @file  ExistenceCompare.h
 *
 * @brief Declaration file for ExistenceCompare compare engine.
 */
#pragma once

class CDiffContext;
class DIFFITEM;

namespace CompareEngines
{

/**
 * @brief A compare engine that compares files by their existence.
 */
class ExistenceCompare
{
public:
	ExistenceCompare(CDiffContext& ctxt);
	~ExistenceCompare();
	int CompareFiles(const DIFFITEM &di) const;
private:
	int m_nfiles;
};

} // namespace CompareEngines
