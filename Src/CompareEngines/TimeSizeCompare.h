/**
 * @file  TimeSizeCompare.h
 *
 * @brief Declaration file for TimeSizeCompare compare engine.
 */
#pragma once

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

	TimeSizeCompare();
	~TimeSizeCompare();
	void SetAdditionalOptions(bool ignoreSmallDiff);
	int CompareFiles(int compMethod, int nfiles, const DIFFITEM &di) const;

private:
	bool m_ignoreSmallDiff;
};

} // namespace CompareEngines
