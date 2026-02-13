/**
 * @file  BinaryCompare.h
 *
 * @brief Declaration file for BinaryCompare compare engine.
 */
#pragma once

class CDiffContext;
class DIFFITEM;

namespace CompareEngines
{

/**
 * @brief A binary compare class.
 * This compare method compares files by their binary contents.
 */
class BinaryCompare
{
public:
	BinaryCompare(CDiffContext& ctxt) : m_ctxt(ctxt) {}
	~BinaryCompare();
	int CompareFiles(DIFFITEM& di) const;
private:
	CDiffContext& m_ctxt;
};

} // namespace CompareEngines
