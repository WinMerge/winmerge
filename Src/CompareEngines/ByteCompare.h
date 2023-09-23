/**
 * @file  ByteCompare.h
 *
 * @brief Declaration file for ByteCompare
 */
#pragma once

#include <memory>
#include "FileTextStats.h"

class CompareOptions;
class QuickCompareOptions;
class IAbortable;
struct DiffFileData;

namespace CompareEngines
{

/**
 * @brief A quick compare -compare method implementation class.
 * This compare method compares files in small blocks. Code assumes block size
 * is in range of 32-bit int-type.
 */
class ByteCompare
{
public:
	ByteCompare();
	~ByteCompare();

	void SetCompareOptions(const CompareOptions & options);
	void SetAdditionalOptions(bool stopAfterFirstDiff);
	void SetAbortable(const IAbortable * piAbortable);

	int CompareFiles(DiffFileData* diffData);

private:
	std::unique_ptr<QuickCompareOptions> m_pOptions; /**< Compare options for diffutils. */
	IAbortable * m_piAbortable;
};

} // namespace CompareEngines
