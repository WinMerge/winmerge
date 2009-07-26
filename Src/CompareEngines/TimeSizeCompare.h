/**
 * @file  TimeSizeCompare.h
 *
 * @brief Declaration file for TimeSizeCompare compare engine.
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _TIMESIZE_COMPARE_H_
#define _TIMESIZE_COMPARE_H_

namespace CompareEngines
{

/**
 * @brief A time/size compare class.
 * This compare method compares files by their times and sizes.
 */
class TimeSizeCompare
{
public:
	TimeSizeCompare();
	~TimeSizeCompare();
	void SetAdditionalOptions(bool ignoreSmallDiff);
	int CompareFiles(int compMethod, const DIFFITEM &di);

private:
	bool m_ignoreSmallDiff;
};

} // namespace CompareEngines

#endif // _TIMESIZE_COMPARE_H_
