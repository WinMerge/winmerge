/**
 * @file  TimeSizeCompare.h
 *
 * @brief Declaration file for TimeSizeCompare compare engine.
 */
#pragma once

class CDiffContext;
class DIFFITEM;

namespace CompareEngines
{

/**
 * @brief A time/size compare class.
 * This compare method compares files by their times and sizes.
 */
class TimeSizeCompare
{
public:
	/// Seconds ignored in filetime differences if option enabled
	enum {
		SmallTimeDiff = 2
	};

	TimeSizeCompare(CDiffContext& ctxt);
	~TimeSizeCompare();
	int CompareFiles(const DIFFITEM &di) const;

private:
	int m_nfiles;
	int m_compMethod;
	bool m_ignoreSmallDiff;
};

} // namespace CompareEngines
