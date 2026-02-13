/**
 * @file  ExistenceCompare.cpp
 *
 * @brief Implementation file for ExistenceCompare
 */

#include "pch.h"
#include "ExistenceCompare.h"
#include "DiffContext.h"
#include "DiffItem.h"

using Poco::Timestamp;

namespace CompareEngines
{

ExistenceCompare::ExistenceCompare(CDiffContext& ctxt)
	: m_nfiles(ctxt.GetCompareDirs())
{
}

ExistenceCompare::~ExistenceCompare() = default;

/**
 * @brief Compare specified files by their existence
 * @param [in,out] di Diffitem info. Results are written to di.diffcode.
 * @return DIFFCODE
 */
int ExistenceCompare::CompareFiles(DIFFITEM& di) const
{
	unsigned code = DIFFCODE::SAME;
	if (di.diffcode.exists(0) != di.diffcode.exists(1) ||
		(m_nfiles > 2 && di.diffcode.exists(0) != di.diffcode.exists(2)))
	{
		code = DIFFCODE::DIFF;
	}

	if (m_nfiles > 2 && (code & DIFFCODE::COMPAREFLAGS) == DIFFCODE::DIFF)
	{
		if (di.diffcode.exists(1) == di.diffcode.exists(2))
			code |= DIFFCODE::DIFF1STONLY;
		else if (di.diffcode.exists(0) == di.diffcode.exists(2))
			code |= DIFFCODE::DIFF2NDONLY;
		else if (di.diffcode.exists(0) == di.diffcode.exists(1))
			code |= DIFFCODE::DIFF3RDONLY;
	}

	di.diffcode.diffcode &= ~(DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY);
	di.diffcode.diffcode |= DIFFCODE::FILE | code;
	return code;
}

} // namespace CompareEngines
