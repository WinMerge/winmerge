/** 
 * @file  ByteComparator.cpp
 *
 * @brief QuickCompare code; code to compare two buffers, byte-by-byte, respecting compare options
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "ByteComparator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static inline bool iseolch(TCHAR ch)
{
    return ch=='\n' || ch=='\r';
}

static inline bool iswsch(TCHAR ch)
{
	// What about nbsp?
	// What about various Unicode spacing codes?
    return ch==' ' || ch=='\t';
}

/**
 * Compare buffers pointed to by ptr0 and ptr1, advancing them
 * End of buffers given by end0 and end1
 * Return true if equal, false if different
 * Take into account global diffutils flags such as ignore_space_change_flag
 */
ByteComparator::COMP_RESULT
ByteComparator::CompareBuffers(LPCSTR &ptr0, LPCSTR &ptr1, LPCSTR end0, LPCSTR end1, bool eof0, bool eof1)
{
	// cycle through buffer data performing actual comparison
	while (true)
	{
		if (m_ignore_all_space)
		{
			// Skip over any whitespace on either side
			// skip over all whitespace
			while (ptr0 < end0 && iswsch(*ptr0))
			{
				m_bol0=false;
				++ptr0;
			}
			// skip over all whitespace
			while (ptr1 < end1 && iswsch(*ptr1))
			{
				m_bol1=false;
				++ptr1;
			}
			if ( (ptr0 == end0 && !eof0) || (ptr1 == end1 && !eof1) )
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
						m_bol0=false;
						++ptr0;
						m_bol1=false;
						++ptr1;
					}
					else if (iseolch(*ptr1))
					{
						// whitespace on side 0 (end of line on side 1)
						m_wsflag = true;
						m_bol0=false;
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
						m_bol0=false;
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
						ASSERT(!iswsch(*ptr0));

						if (iseolch(*ptr0))
						{
							// whitespace on side 1 (eol on side 0)
							m_wsflag = true;
							m_bol1=false;
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
							m_bol1=false;
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
					m_bol0=false;
					++ptr0;
				}
				// skip over consecutive whitespace
				while (ptr1 < end1 && iswsch(*ptr1))
				{
					m_bol1=false;
					++ptr1;
				}
				if ( (ptr0 == end0 && !eof0) || (ptr1 == end1 && !eof1) )
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
				if ( (ptr0 == end0 && !eof0) || (ptr1 == end1 && !eof1) )
				{
					goto need_more;
				}
			}
			else // don't skip blank lines, but still ignore eol difference
			{
				if (m_cr0)
				{
					// finish split CR/LF pair on 0-side
					if (ptr0 < end0 && *ptr0 == '\n')
					{
						// m_bol0 not used because m_ignore_eol_diff
						++ptr0;
					}
					m_eol0 = true;
					m_cr0 = false;
				}
				if (ptr0 < end0)
				{
					if (*ptr0 == '\n')
					{
						// m_bol0 not used because m_ignore_eol_diff
						++ptr0;
						m_eol0 = true;
					}
					else if (*ptr0 == '\r')
					{
						// m_bol0 not used because m_ignore_eol_diff
						++ptr0;
						m_eol0 = true;
						if (ptr0 == end0 && !eof0)
						{
							// can't tell if a CR/LF pair yet
							m_cr0 = true;
							m_eol0 = true;
						}
						else if (ptr0 < end0 && *ptr0 == '\n')
						{
							++ptr0;
						}
					}
					else
					{
						m_eol0 = false;
					}
				}
				if (m_cr1)
				{
					// finish split CR/LF pair on 1-side
					if (ptr1 < end1 && *ptr1 == '\n')
					{
						// m_bol1 not used because m_ignore_eol_diff
						++ptr1;
					}
					m_eol1 = true;
					m_cr1 = false;
				}
				if (ptr1 < end1)
				{
					if (*ptr1 == '\n')
					{
						// m_bol1 not used because m_ignore_eol_diff
						++ptr1;
						m_eol1 = true;
					}
					else if (*ptr1 == '\r')
					{
						// m_bol1 not used because m_ignore_eol_diff
						++ptr1;
						m_eol1 = true;
						if (ptr1 == end1 && !eof1)
						{
							// can't tell if a CR/LF pair yet
							m_cr1 = true;
							m_eol1 = true;
						}
						else if (ptr1 < end1 && *ptr1 == '\n')
						{
							++ptr1;
						}
					}
					else
					{
						m_eol1 = false;
					}
				}
				if (m_cr0 || m_cr1)
				{
					// these flags mean possible split CR/LF 
					goto need_more;
				}
				if (m_eol0 || m_eol1)
				{
					if (!m_eol0 || !m_eol1)
					{
						// one side had an end-of-line, but the other didn't
						return RESULT_DIFF;
					}
					// otherwise, both sides had end-of-line
					// pointers have already been advanced, so just continue happily
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
				if ( (ptr0 == end0 && !eof0) || (ptr1 == end1 && !eof1) )
				{
					goto need_more;
				}
			}
		}

		if ( ptr0 == end0 || ptr1 == end1)
		{
			if ( ptr0 == end0 && ptr1 == end1)
			{
				if (!eof0 || !eof1)
					goto need_more;
				else
					return RESULT_SAME;
			}
			else
			{
				return RESULT_DIFF;
			}
		}

		TCHAR c0 = *ptr0, c1 = *ptr1;
		if (m_ignore_case)
		{
			c0 = _istupper(c0) ? _totlower(c0) : c0;
			c1 = _istupper(c1) ? _totlower(c1) : c1;
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
	if (ptr0 == end0 && !eof0)
	{
		if (ptr1 == end1 && !eof1)
			return NEED_MORE_BOTH;
		else
			return NEED_MORE_0;
	}
	else if(ptr1 == end1 && !eof1)
	{
		return NEED_MORE_1;
	}
	else
	{
		return RESULT_SAME;
	}
}

/**
 * @brief Reset all ignore settings for compare.
 * Causes compare to be done byte-per-byte which we want for binary files.
 */
void ByteComparator::ResetIgnore()
{
	m_ignore_case = false;
	m_ignore_space_change = false;
	m_ignore_all_space = false;
	m_ignore_eol_diff = false;
	m_ignore_blank_lines = false;
}
