// ByteComparator.cpp
//

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
			// Skip over any whitespace if on both sides
			if (m_wsflag || (!eof0 && !eof1 && iswsch(*ptr0) && iswsch(*ptr1)))
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
					++ptr0;
				}
				while (ptr1 < end1 && iseolch(*ptr1))
				{
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
						++ptr0;
					}
					m_eol0 = true;
					m_cr0 = false;
				}
				if (!m_eol0)
				{
					if (ptr0 < end0 && *ptr0 == '\n')
					{
						++ptr0;
						m_eol0 = true;
					}
					else if (ptr0 < end0 && *ptr0 == '\r')
					{
						++ptr0;
						m_eol0 = true;
						if (ptr0 == end0 && !eof0)
						{
							// can't tell if a CR/LF pair yet
							m_cr0 = true;
							m_eol0 = true;
						}
					}
				}
				if (m_cr1)
				{
					// finish split CR/LF pair on 1-side
					if (ptr1 < end1 && *ptr1 == '\n')
					{
						++ptr1;
					}
					m_eol1 = true;
					m_cr1 = false;
				}
				if (!m_eol1)
				{
					if (ptr1 < end1 && *ptr1 == '\n')
					{
						++ptr1;
						m_eol1 = true;
					}
					else if (ptr1 < end1 && *ptr1 == '\r')
					{
						++ptr1;
						m_eol1 = true;
						if (ptr1 == end1 && !eof1)
						{
							// can't tell if a CR/LF pair yet
							m_cr1 = true;
							m_eol1 = true;
						}
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
						return RESULT_DIFF;
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
