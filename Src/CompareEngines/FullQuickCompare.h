/**
 * @file  FullQuickCompare.h
 *
 * @brief Declaration file for FullQuickCompare compare engine.
 */
#pragma once

#include <memory>

class CDiffContext;
class DIFFITEM;
struct DiffFileData;

namespace CompareEngines
{
	class DiffUtils;
	class ByteCompare;

/**
 * @brief A full contents compare class.
 * This compare method compares files by their full text contents using diffutils.
 */
class FullQuickCompare
{
public:
	explicit FullQuickCompare(CDiffContext& ctxt, int nCompMethod);
	~FullQuickCompare();
	int CompareFiles(DIFFITEM& di) const;
	
private:
	CDiffContext* m_pCtxt;
	mutable std::unique_ptr<DiffUtils> m_pDiffUtilsEngine;
	mutable std::unique_ptr<ByteCompare> m_pByteCompare;
	int m_nCompMethod;
};

} // namespace CompareEngines
