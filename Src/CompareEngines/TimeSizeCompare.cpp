/** 
 * @file  TimeSizeCompare.cpp
 *
 * @brief Implementation file for TimeSizeCompare
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "DiffItem.h"
#include "TimeSizeCompare.h"
#include "DiffWrapper.h"

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
int TimeSizeCompare::CompareFiles(int compMethod, const DIFFITEM &di)
{
	UINT code = 0;

	// Compare by modified date
	// Check that we have both filetimes
	if (di.left.mtime != 0 && di.right.mtime != 0)
	{
		INT64 nTimeDiff = di.left.mtime - di.right.mtime;
		// Remove sign
		nTimeDiff = (nTimeDiff > 0 ? nTimeDiff : -nTimeDiff);
		if (m_ignoreSmallDiff)
		{
			// If option to ignore small timediffs (couple of seconds)
			// is set, decrease absolute difference by allowed diff
			nTimeDiff -= SmallTimeDiff;
		}
		if (nTimeDiff <= 0)
			code = DIFFCODE::SAME;
		else
			code = DIFFCODE::DIFF;
	}
	else
	{
		// Filetimes for item(s) could not be read. So we have to
		// set error status, unless we have DATE_SIZE -compare
		// when we have still hope for size compare..
		if (compMethod == CMP_DATE_SIZE)
			code = DIFFCODE::SAME;
		else
			code = DIFFCODE::CMPERR;
	}
	
	// This is actual CMP_DATE_SIZE method..
	// If file sizes differ mark them different
	if (compMethod == CMP_DATE_SIZE)
	{
		if (di.left.size != di.right.size)
		{
			code &= ~DIFFCODE::SAME;
			code = DIFFCODE::DIFF;
		}
	}
	return code;
}

} // namespace CompareEngines
