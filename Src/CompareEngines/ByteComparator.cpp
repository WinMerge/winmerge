/**
 * @file  ByteComparator.cpp
 *
 * @brief Implements ByteComparator class.
 */

#include "pch.h"
#include "ByteComparator.h"
#include <cassert>
#include "UnicodeString.h"
#include "FileTextStats.h"
#include "CompareOptions.h"

/**
 * @brief Returns if given char is EOL byte.
 * @param [in] ch Char to test.
 * @return true if char is EOL byte, false otherwise.
 */
static inline bool iseolch(tchar_t ch)
{
	return ch == '\n' || ch == '\r';
}

/**
 * @brief Returns if given char is whitespace char.
 * @param [in] ch Char to test.
 * @return true if char is whitespace char, false otherwise.
 * @todo What about nbsp or various Unicode spacing codes?
 */
static inline bool iswsch(tchar_t ch)
{
	return ch == ' ' || ch == '\t';
}

/**
 * @brief Calculates statistics from given buffer.
 * This function calculates EOL byte and zero-byte statistics from given
 * buffer.
 * @param [in,out] stats Structure holding statistics.
 * @param [in] ptr Pointer to begin of the buffer.
 * @param [in] end Pointer to end of buffer.
 * @param [in] eof Is buffer end also end of file?
 * @param [in] crflag Did previous scan end to CR?
 * @param [in] offset Byte offset in whole file (among several buffers).
 */
static void TextScan(FileTextStats & stats, const char *ptr, const char *end, bool eof,
		bool crflag, int64_t offset)
{
	// Handle any crs left from last buffer
	if (crflag)
	{
		if (ptr < end && *ptr == '\n')
		{
			++stats.ncrlfs;
			++ptr;
		}
		else
		{
			++stats.ncrs;
		}
	}
	for (; ptr < end; ++ptr)
	{
		char ch = *ptr;
		if (ch == 0)
		{
			++stats.nzeros;
		}
		else if (ch == '\r')
		{
			if (ptr + 1 < end)
			{
				if (ptr[1] == '\n')
				{
					++stats.ncrlfs;
					++ptr;
				}
				else
				{
					++stats.ncrs;
				}
			}
			else if (eof)
			{
				++stats.ncrs;
			}
			else
			{
				// else last byte of buffer
				// leave alone, the CompareBuffers loop will set the appropriate m_cr flag
				// and we'll handle it next time we're called
			}
		}
		else if (ch == '\n')
		{
			++stats.nlfs;
		}
	}
}

namespace CompareEngines
{

/**
 * @brief Constructor taking compare options as parameters.
 * @param [in] options Compare options.
 */
ByteComparator::ByteComparator(const QuickCompareOptions * options)
// settings
		: m_ignore_case(options->m_bIgnoreCase)
		, m_ignore_eol_diff(options->m_bIgnoreEOLDifference)
		, m_ignore_blank_lines(options->m_bIgnoreBlankLines)
		, m_ignore_numbers(options->m_bIgnoreNumbers)
// state
		, m_wsflag(false)
		, m_eol0(false)
		, m_eol1(false)
		, m_cr0(false)
		, m_cr1(false)
		, m_bol0(true)
		, m_bol1(true)
{
	if (options->m_ignoreWhitespace == WHITESPACE_IGNORE_CHANGE)
		m_ignore_space_change = true;
	else
		m_ignore_space_change = false;

	if (options->m_ignoreWhitespace == WHITESPACE_IGNORE_ALL)
		m_ignore_all_space = true;
	else
		m_ignore_all_space = false;
}

/**
 * @brief Compare two buffers byte per byte.
 *
 * This function compares two buffers pointed to by @p ptr0 and @p ptr1.
 * Comparing takes account diffutils options flags given to constructor.
 * Buffer pointers are advanced while comparing so they point to current
 * compare position. End of buffers are given by @p end0 and @p end1, which
 * may point past last valid byte in file. Offset-params tell is how far this
 * buffer is into the file (ie, 0 the first time called).
 * @param [in,out] stats0 Statistics for first side.
 * @param [in,out] stats1 Statistics for second side.
 * @param [in,out] ptr0 Pointer to begin of the first buffer.
 * @param [in,out] ptr1 Pointer to begin of the second buffer.
 * @param [in] end0 Pointer to end of the first buffer.
 * @param [in] end1 Pointer to end of the second buffer.
 * @param [in] eof0 Is first buffers end also end of the file?
 * @param [in] eof1 Is second buffers end also end of the file?
 * @param [in] offset0 Offset of the buffer begin in the first file.
 * @param [in] offset1 Offset of the buffer begin in the second file.
 * @return COMP_RESULT telling result of the compare.
 */
ByteComparator::COMP_RESULT ByteComparator::CompareBuffers(
	FileTextStats & stats0, FileTextStats & stats1, const char* &ptr0, const char* &ptr1,
	const char* end0, const char* end1, bool eof0, bool eof1, int64_t offset0, int64_t offset1)
{
	ByteComparator::COMP_RESULT result = RESULT_SAME;

	// First, update file text statistics by doing a full scan
	// for 0s and all types of line delimiters
	TextScan(stats0, ptr0, end0, eof0, m_cr0, offset0);
	TextScan(stats1, ptr1, end1, eof1, m_cr1, offset1);

	const char *orig0 = ptr0;
	const char *orig1 = ptr1;

	// cycle through buffer data performing actual comparison
	while (true)
	{
		if (m_ignore_all_space)
		{
			// Skip over any whitespace on either side
			// skip over all whitespace
			while (ptr0 < end0 && iswsch(*ptr0))
			{
				m_bol0 = false;
				++ptr0;
			}
			// skip over all whitespace
			while (ptr1 < end1 && iswsch(*ptr1))
			{
				m_bol1 = false;
				++ptr1;
			}
			if ((ptr0 == end0 && !eof0) || (ptr1 == end1 && !eof1))
			{
				goto need_more;
			}
		}
		if (m_ignore_space_change)
		{
			// Skip over whitespace change
			// Also skip whitespace on one side if
			//  either end of line or end of file on other

			// Handle case of whitespace on side0
			// (First four cases)
			if (ptr0 < end0 && iswsch(*ptr0))
			{
				// Whitespace on side0

				if (ptr1 < end1)
				{
					if (iswsch(*ptr1))
					{
						// whitespace on both sides
						m_wsflag = true;
						m_bol0 = false;
						++ptr0;
						m_bol1 = false;
						++ptr1;
					}
					else if (iseolch(*ptr1))
					{
						// whitespace on side 0 (end of line on side 1)
						m_wsflag = true;
						m_bol0 = false;
						++ptr0;
					}
				}
				else // ptr1 == end1
				{
					if (!eof1)
					{
						// Whitespace on side0, don't know what is on side1
						// Cannot tell if matching whitespace yet
						goto need_more;
					}
					else // eof1
					{
						// Whitespace on side0, eof on side1
						m_wsflag = true;
						m_bol0 = false;
						++ptr0;
					}
				}
			}
			else
			{
				// Handle case of whitespace on side1
				// but not whitespace on side0 (that was handled above)
				// (Remaining three cases)
				if (ptr1 < end1 && iswsch(*ptr1))
				{
					// Whitespace on side1

					if (ptr0 < end0)
					{
						// "whitespace on both sides"
						// should not come here, it should have been
						// handled above
						assert(!iswsch(*ptr0));

						if (iseolch(*ptr0))
						{
							// whitespace on side 1 (eol on side 0)
							m_wsflag = true;
							m_bol1 = false;
							++ptr1;
						}
					}
					else // ptr0 == end0
					{
						if (!eof0)
						{
							// Whitespace on side1, don't know what is on side0
							// Cannot tell if matching whitespace yet
							goto need_more;
						}
						else // eof0
						{
							// Whitespace on side1, eof on side0
							m_wsflag = true;
							m_bol1 = false;
							++ptr1;
						}
					}
				}
			}

			if (m_wsflag)
			{
				// skip over consecutive whitespace
				while (ptr0 < end0 && iswsch(*ptr0))
				{
					m_bol0 = false;
					++ptr0;
				}
				// skip over consecutive whitespace
				while (ptr1 < end1 && iswsch(*ptr1))
				{
					m_bol1 = false;
					++ptr1;
				}
				if ((ptr0 == end0 && !eof0) || (ptr1 == end1 && !eof1))
				{
					// if run out of buffer on either side
					// must fetch more, to continue skipping whitespace
					m_wsflag = true;
					goto need_more;
				}
			}
			m_wsflag = false;
		}
		if (m_ignore_eol_diff)
		{
			if (m_ignore_blank_lines)
			{
				// skip over any line delimiters on either side
				while (ptr0 < end0 && iseolch(*ptr0))
				{
					// m_bol0 not used because m_ignore_eol_diff
					++ptr0;
				}
				while (ptr1 < end1 && iseolch(*ptr1))
				{
					// m_bol1 not used because m_ignore_eol_diff
					++ptr1;
				}
				if ((ptr0 == end0 && !eof0) || (ptr1 == end1 && !eof1))
				{
					goto need_more;
				}
			}
			else // don't skip blank lines, but still ignore eol difference
			{
				HandleSide0Eol((char **) &ptr0, end0, eof0);
				HandleSide1Eol((char **) &ptr1, end1, eof1);

				if (m_cr0 || m_cr1)
				{
					// these flags mean possible split CR/LF
					goto need_more;
				}
				if (m_eol0 || m_eol1)
				{
					if ((!m_eol0 || !m_eol1) && (orig0 == end0 || orig1 == end1))
					{
						// one side had an end-of-line, but the other didn't
						return RESULT_DIFF;
					}
					if (ptr0 != end0 && ptr1 != end1)
						// This continue statement is needed to handle blank lines
						continue;
				}
			}
		}
		else
		{ // do not ignore eol differences
			if (m_ignore_blank_lines)
			{
				if (m_bol0)
				{
					while (ptr0 < end0 && iseolch(*ptr0))
					{
						++ptr0;
					}
				}
				if (m_bol1)
				{
					while (ptr1 < end1 && iseolch(*ptr1))
					{
						++ptr1;
					}
				}
				if ((ptr0 == end0 && !eof0) || (ptr1 == end1 && !eof1))
				{
					goto need_more;
				}
			}
		}

		if (ptr0 == end0 || ptr1 == end1)
		{
			if (ptr0 == end0 && ptr1 == end1)
			{
				if (!eof0 || !eof1)
					goto need_more;
				else
					return RESULT_SAME;
			}
			else
			{
				// we are at the end on one side?
				if ((!(ptr0 == end0 && eof0) && !(ptr1 == end1 && eof1)) && (orig0 != end0 && orig1 != end1))
				{
					goto need_more;
				}
				else
					return RESULT_DIFF;
			}
		}

		tchar_t c0 = *ptr0, c1 = *ptr1;
		if (m_ignore_case)
		{
			c0 = tc::istupper(c0) ? tc::totlower(c0) : c0;
			c1 = tc::istupper(c1) ? tc::totlower(c1) : c1;
		}
		if (c0 != c1)
			return RESULT_DIFF; // buffers are different
		if (ptr0 < end0 && ptr1 < end1)
		{
			m_bol0 = iseolch(c0);
			m_bol1 = iseolch(c1);
			++ptr0;
			++ptr1;
			continue;
		}
		goto need_more;
	}

need_more:
	if (ptr0 - 1 >= orig0 && *(ptr0 - 1) == '\r')
		m_cr0 = true;
	else
		m_cr0 = false;
	if (ptr1 - 1 >= orig1 && *(ptr1 - 1) == '\r')
		m_cr1 = true;
	else
		m_cr1 = false;
	if (ptr0 == end0 && !eof0)
	{
		if (ptr1 == end1 && !eof1)
			return NEED_MORE_BOTH;
		else
			return NEED_MORE_0;
	}
	else if (ptr1 == end1 && !eof1)
	{
		return NEED_MORE_1;
	}
	else
	{
		return result;
	}
}

/**
 * Brief Handle EOL bytes and differences in them.
 * @param [in, out] ptr Pointer to the buffer begin.
 * @param [in] end Pointer to the buffer end.
 * @param [in] eof Are we at end of the buffer?
 */
inline void ByteComparator::HandleSide0Eol(char **ptr, const char *end, bool eof)
{
	char * pbuf = *ptr;
	if (m_cr0)
	{
		// finish split CR/LF pair on 0-side
		if (pbuf < end && *pbuf == '\n')
		{
			// m_bol0 not used because m_ignore_eol_diff
			++pbuf;
		}
		m_eol0 = true;
		m_cr0 = false;
	}
	if (pbuf < end)
	{
		if (*pbuf == '\n')
		{
			// m_bol0 not used because m_ignore_eol_diff
			++pbuf;
			m_eol0 = true;
		}
		else if (*pbuf == '\r')
		{
			// m_bol0 not used because m_ignore_eol_diff
			++pbuf;
			m_eol0 = true;
			if (pbuf == end && !eof)
			{
				// can't tell if a CR/LF pair yet
				m_cr0 = true;
				m_eol0 = true;
			}
			else if (pbuf < end && *pbuf == '\n')
			{
				++pbuf;
			}
		}
		else
		{
			m_eol0 = false;
		}
	}
	*ptr = pbuf;
}

/**
 * Brief Handle EOL bytes and differences in them.
 * @param [in, out] ptr Pointer to the buffer begin.
 * @param [in] end Pointer to the buffer end.
 * @param [in] eof Are we at end of the buffer?
 */
inline void ByteComparator::HandleSide1Eol(char **ptr, const char *end, bool eof)
{
	char * pbuf = *ptr;

	if (m_cr1)
	{
		// finish split CR/LF pair on 1-side
		if (pbuf < end && *pbuf == '\n')
		{
			// m_bol1 not used because m_ignore_eol_diff
			++pbuf;
		}
		m_eol1 = true;
		m_cr1 = false;
	}
	if (pbuf < end)
	{
		if (*pbuf == '\n')
		{
			// m_bol1 not used because m_ignore_eol_diff
			++pbuf;
			m_eol1 = true;
		}
		else if (*pbuf == '\r')
		{
			// m_bol1 not used because m_ignore_eol_diff
			++pbuf;
			m_eol1 = true;
			if (pbuf == end && !eof)
			{
				// can't tell if a CR/LF pair yet
				m_cr1 = true;
				m_eol1 = true;
			}
			else if (pbuf < end && *pbuf == '\n')
			{
				++pbuf;
			}
		}
		else
		{
			m_eol1 = false;
		}
	}
	*ptr = pbuf;
}

} //namespace CompareEngines

