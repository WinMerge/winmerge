class ByteComparator
{
public:
	ByteComparator(int ignore_case, int ignore_space_change, int ignore_all_space
		, int ignore_eol_diff, int ignore_blank_lines)
		// settings
		: m_ignore_case(!!ignore_case)
		, m_ignore_space_change(!!ignore_space_change)
		, m_ignore_all_space(!!ignore_all_space)
		, m_ignore_eol_diff(!!ignore_eol_diff)
		, m_ignore_blank_lines(!!ignore_blank_lines)
		// state
		, m_wsflag(false)
		, m_eol0(false)
		, m_eol1(false)
		, m_cr0(false)
		, m_cr1(false)
		, m_bol0(true)
		, m_bol1(true)
		{
		}

public:
	typedef enum { RESULT_DIFF, RESULT_SAME, NEED_MORE_0, NEED_MORE_1, NEED_MORE_BOTH } COMP_RESULT;

	COMP_RESULT CompareBuffers(LPCSTR &ptr0, LPCSTR &ptr1, LPCSTR end0, LPCSTR end1, bool eof0, bool eof1);

private:
	// settings
	bool m_ignore_case;
	bool m_ignore_space_change;
	bool m_ignore_all_space;
	bool m_ignore_eol_diff;
	bool m_ignore_blank_lines;
	// state
	bool m_wsflag; // ignore_space_change & in a whitespace area
	bool m_eol0; // 0-side has an eol
	bool m_eol1; // 1-side has an eol
	bool m_cr0; // 0-side has a CR at end of buffer (might be split CR/LF)
	bool m_cr1; // 0-side has a CR at end of buffer (might be split CR/LF)
	bool m_bol0; //0-side is at beginning of line (!ignore_eol_differences & ignore_blank_lines)
	bool m_bol1; //0-side is at beginning of line (!ignore_eol_differences & ignore_blank_lines)
};
