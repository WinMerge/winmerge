/** 
 * @file ByteComparator.h
 *
 * @brief Declaration file for ByteComparator class.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef ByteComparator_h_included
#define ByteComparator_h_included

struct FileTextStats;


/**
 * @brief Byte per byte compare class implementing Quick Compare.
 *
 * This class implements WinMerge's Quick Compare -compare method. Basically it
 * compares two buffers byte per byte. But it also respects WinMerge compare
 * options for whitespace ignore etc. Which makes it more complex than just
 * simple byte per byte compare. Also counts EOL / 0-byte statistics from
 * buffers so we can detect binary files and EOL types.
 */
class ByteComparator
{
public:
	ByteComparator(int ignore_case, int ignore_space_change, int ignore_all_space,
		int ignore_eol_diff, int ignore_blank_lines);

public:
	/** @brief Return values for CompareBuffers method. */
	typedef enum
	{
		RESULT_DIFF, /**< Buffers are different */
		RESULT_SAME, /**< Buffers are identical */
		NEED_MORE_0, /**< First buffer needs more data */
		NEED_MORE_1, /**< Second buffer needs more data */
		NEED_MORE_BOTH, /**< Both buffers need more data */
	} COMP_RESULT;

	COMP_RESULT CompareBuffers(FileTextStats & stats0, FileTextStats & stats1,
		LPCSTR &ptr0, LPCSTR &ptr1, LPCSTR end0, LPCSTR end1, bool eof0, bool eof1,
		__int64 offset0, __int64 offset1);

private:
	// settings
	bool m_ignore_case; /**< Ignore character case */
	bool m_ignore_space_change; /**< Ignore change in whitespace char count */
	bool m_ignore_all_space; /**< Ignore all whitespace changes */
	bool m_ignore_eol_diff; /**< Ignore differences in EOL bytes */
	bool m_ignore_blank_lines; /**< Ignore blank lines */
	// state
	bool m_wsflag; /**< ignore_space_change & in a whitespace area */
	bool m_eol0; /**< 0-side has an eol */
	bool m_eol1; /**< 1-side has an eol */
	bool m_cr0; /**< 0-side has a CR at end of buffer (might be split CR/LF) */
	bool m_cr1; /**< 1-side has a CR at end of buffer (might be split CR/LF) */
	bool m_bol0; /**< 0-side is at beginning of line (!ignore_eol_differences & ignore_blank_lines) */
	bool m_bol1; /**< 1-side is at beginning of line (!ignore_eol_differences & ignore_blank_lines) */
};

#endif // ByteComparator_h_included
