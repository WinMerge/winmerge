/**
 * @file  TimeSizeCompare.cpp
 *
 * @brief Implementation file for TimeSizeCompare
 */

#include "pch.h"
#include "TimeSizeCompare.h"
#include <Poco/Timestamp.h>
#include "DiffItem.h"
#include "DiffWrapper.h"

using Poco::Timestamp;

namespace CompareEngines
{

TimeSizeCompare::TimeSizeCompare()
		: m_ignoreSmallDiff(false)
{
}

TimeSizeCompare::~TimeSizeCompare()
{
}

/**
 * @brief Set compare-type specific options.
 * @param [in] ignoreSmallDiff Ignore small time differences?
 */
void TimeSizeCompare::SetAdditionalOptions(bool ignoreSmallDiff)
{
	m_ignoreSmallDiff = ignoreSmallDiff;
}

/**
 * @brief Compare two specified files, byte-by-byte
 * @param [in] compMethod Compare method used.
 * @param [in] di Diffitem info.
 * @return DIFFCODE
 */
int TimeSizeCompare::CompareFiles(int compMethod, int nfiles, const DIFFITEM &di) const
{
	unsigned code = DIFFCODE::SAME;
	int64_t nTimeDiff = 0;
	int64_t nTimeDiff12 = 0;
	int64_t nTimeDiff02 = 0;
	if ((compMethod == CMP_DATE) || (compMethod == CMP_DATE_SIZE))
	{
		// Compare by modified date
		// Check that we have both filetimes
		nTimeDiff   = di.diffFileInfo[0].mtime - di.diffFileInfo[1].mtime;
		if (nTimeDiff   < 0) nTimeDiff   *= -1;
		if (nfiles > 2)
		{
			nTimeDiff12 = di.diffFileInfo[1].mtime - di.diffFileInfo[2].mtime;
			nTimeDiff02 = di.diffFileInfo[0].mtime - di.diffFileInfo[2].mtime;
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

		for (int i = 0; i < nfiles; ++i)
		{
			if (di.diffFileInfo[i].mtime == 0 && di.diffcode.exists(i))
				code = DIFFCODE::CMPERR;
		}
	}
	// This is actual CMP_SIZE method..
	// If file sizes differ mark them different
	if ((compMethod == CMP_DATE_SIZE) || (compMethod == CMP_SIZE))
	{
		if (di.diffFileInfo[0].size != di.diffFileInfo[1].size || 
		    (nfiles > 2 && di.diffFileInfo[1].size != di.diffFileInfo[2].size))
		{
			code = DIFFCODE::DIFF;
		}

		for (int i = 0; i < nfiles; ++i)
		{
			if (di.diffFileInfo[i].size == DirItem::FILE_SIZE_NONE && di.diffcode.exists(i))
				code = DIFFCODE::CMPERR;
		}
	}
	if (nfiles > 2 && (code & DIFFCODE::COMPAREFLAGS) == DIFFCODE::DIFF)
	{
		if (compMethod == CMP_DATE)
		{
			if (nTimeDiff12 <= 0)
				code |= DIFFCODE::DIFF1STONLY;
			else if (nTimeDiff02 <= 0)
				code |= DIFFCODE::DIFF2NDONLY;
			else if (nTimeDiff <= 0)
				code |= DIFFCODE::DIFF3RDONLY;
		}
		else if (compMethod == CMP_DATE_SIZE)
		{
			if (nTimeDiff12 <= 0 && di.diffFileInfo[1].size == di.diffFileInfo[2].size)
				code |= DIFFCODE::DIFF1STONLY;
			else if (nTimeDiff02 <= 0 && di.diffFileInfo[0].size == di.diffFileInfo[2].size)
				code |= DIFFCODE::DIFF2NDONLY;
			else if (nTimeDiff <= 0 && di.diffFileInfo[0].size == di.diffFileInfo[1].size)
				code |= DIFFCODE::DIFF3RDONLY;
		}
		else if (compMethod == CMP_SIZE)
		{
			if (di.diffFileInfo[1].size == di.diffFileInfo[2].size)
				code |= DIFFCODE::DIFF1STONLY;
			else if (di.diffFileInfo[0].size == di.diffFileInfo[2].size)
				code |= DIFFCODE::DIFF2NDONLY;
			else if (di.diffFileInfo[0].size == di.diffFileInfo[1].size)
				code |= DIFFCODE::DIFF3RDONLY;
		}
	}
	return code;
}

} // namespace CompareEngines
