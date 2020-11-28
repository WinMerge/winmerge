/**
 * @file  BinaryCompare.h
 *
 * @brief Declaration file for BinaryCompare compare engine.
 */
#pragma once

class DIFFITEM;
class PathContext;
class IAbortable;

namespace CompareEngines
{

/**
 * @brief A binary compare class.
 * This compare method compares files by their binary contents.
 */
class BinaryCompare
{
public:
	BinaryCompare();
	~BinaryCompare();
	void SetAbortable(const IAbortable * piAbortable);
	int CompareFiles(const PathContext& files, const DIFFITEM &di) const;
private:
	IAbortable * m_piAbortable;
};

} // namespace CompareEngines
