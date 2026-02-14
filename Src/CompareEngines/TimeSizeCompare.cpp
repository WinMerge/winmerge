/**
 * @file  TimeSizeCompare.cpp
 *
 * @brief Implementation file for TimeSizeCompare
 */

#include "pch.h"
#include "TimeSizeCompare.h"
#include <Poco/Timestamp.h>
#include "DiffContext.h"
#include "DiffItem.h"
#include "DiffWrapper.h"

using Poco::Timestamp;

namespace CompareEngines
{

TimeSizeCompare::TimeSizeCompare(CDiffContext& ctxt)
	: m_nfiles(ctxt.GetCompareDirs())
	, m_compMethod(ctxt.GetCompareMethod())
	, m_ignoreSmallDiff(ctxt.m_bIgnoreSmallTimeDiff)
{
}

TimeSizeCompare::~TimeSizeCompare() = default;

/**
 * @brief Compare two specified files, byte-by-byte
 * @param [in,out] di Diffitem info. Results are written to di.diffcode.
 */
void TimeSizeCompare::CompareFiles(DIFFITEM& di) const
{
	unsigned code = DIFFCODE::SAME;
	int64_t nTimeDiff = 0;
	int64_t nTimeDiff12 = 0;
	int64_t nTimeDiff02 = 0;
	auto roundToSeconds = [](Poco::Timestamp tim) -> int64_t
		{
			constexpr auto unit = Poco::Timestamp::resolution();
			return (tim.epochMicroseconds() / unit) * unit;
		};
	if ((m_compMethod == CMP_DATE) || (m_compMethod == CMP_DATE_SIZE))
	{
		// Compare by modified date
		// Check that we have both filetimes
		nTimeDiff   = roundToSeconds(di.diffFileInfo[0].mtime) - roundToSeconds(di.diffFileInfo[1].mtime);
		if (nTimeDiff   < 0) nTimeDiff   *= -1;
		if (m_nfiles > 2)
		{
			nTimeDiff12 = roundToSeconds(di.diffFileInfo[1].mtime) - roundToSeconds(di.diffFileInfo[2].mtime);
			nTimeDiff02 = roundToSeconds(di.diffFileInfo[0].mtime) - roundToSeconds(di.diffFileInfo[2].mtime);
			if (nTimeDiff12 < 0) nTimeDiff12 *= -1;
			if (nTimeDiff02 < 0) nTimeDiff02 *= -1;
		}
		if (m_ignoreSmallDiff)
		{
			// If option to ignore small timediffs (couple of seconds)
			// is set, decrease absolute difference by allowed diff
			nTimeDiff   -= SmallTimeDiff * Timestamp::resolution();
			nTimeDiff12 -= SmallTimeDiff * Timestamp::resolution();
			nTimeDiff02 -= SmallTimeDiff * Timestamp::resolution();
		}
		if (nTimeDiff <= 0 && nTimeDiff12 <= 0)
			code = DIFFCODE::SAME;
		else
			code = DIFFCODE::DIFF;

		for (int i = 0; i < m_nfiles; ++i)
		{
			if (di.diffFileInfo[i].mtime == Poco::Timestamp::TIMEVAL_MIN && di.diffcode.exists(i))
				code = DIFFCODE::CMPERR;
		}
	}
	// This is actual CMP_SIZE method..
	// If file sizes differ mark them different
	if ((m_compMethod == CMP_DATE_SIZE) || (m_compMethod == CMP_SIZE))
	{
		if (di.diffFileInfo[0].size != di.diffFileInfo[1].size || 
		    (m_nfiles > 2 && di.diffFileInfo[1].size != di.diffFileInfo[2].size))
		{
			code = DIFFCODE::DIFF;
		}

		for (int i = 0; i < m_nfiles; ++i)
		{
			if (di.diffFileInfo[i].size == DirItem::FILE_SIZE_NONE && di.diffcode.exists(i))
				code = DIFFCODE::CMPERR;
		}
	}
	if (m_nfiles > 2 && (code & DIFFCODE::COMPAREFLAGS) == DIFFCODE::DIFF)
	{
		if (m_compMethod == CMP_DATE)
		{
			if (nTimeDiff12 <= 0)
				code |= DIFFCODE::DIFF1STONLY;
			else if (nTimeDiff02 <= 0)
				code |= DIFFCODE::DIFF2NDONLY;
			else if (nTimeDiff <= 0)
				code |= DIFFCODE::DIFF3RDONLY;
		}
		else if (m_compMethod == CMP_DATE_SIZE)
		{
			if (nTimeDiff12 <= 0 && di.diffFileInfo[1].size == di.diffFileInfo[2].size)
				code |= DIFFCODE::DIFF1STONLY;
			else if (nTimeDiff02 <= 0 && di.diffFileInfo[0].size == di.diffFileInfo[2].size)
				code |= DIFFCODE::DIFF2NDONLY;
			else if (nTimeDiff <= 0 && di.diffFileInfo[0].size == di.diffFileInfo[1].size)
				code |= DIFFCODE::DIFF3RDONLY;
		}
		else if (m_compMethod == CMP_SIZE)
		{
			if (di.diffFileInfo[1].size == di.diffFileInfo[2].size)
				code |= DIFFCODE::DIFF1STONLY;
			else if (di.diffFileInfo[0].size == di.diffFileInfo[2].size)
				code |= DIFFCODE::DIFF2NDONLY;
			else if (di.diffFileInfo[0].size == di.diffFileInfo[1].size)
				code |= DIFFCODE::DIFF3RDONLY;
		}
	}

	di.diffcode.diffcode &= ~(DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY);
	di.diffcode.diffcode |= DIFFCODE::FILE | code;
}

} // namespace CompareEngines
