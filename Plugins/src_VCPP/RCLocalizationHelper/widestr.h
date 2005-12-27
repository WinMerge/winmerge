#ifndef widestr_h_included
#define widestr_h_included
/*
 * Bare bones wchar_t string
 */
class widestr
{
public:
	widestr(int capacity) 
		: m_wstr(0), m_maxlen(0), m_curlen(0)
	{
		alloc(capacity); 
		m_wstr[0] = 0;
		update();
	}
	~widestr()
	{
		release();
	}
	void makeEmpty()
	{
		m_wstr[0] = 0;
		update();
	}
	int length()
	{
		return m_curlen;
	}
	void set(const wchar_t * wstr, int maxlen=-1)
	{
		if (!wstr || !wstr[0] || !maxlen)
		{
			makeEmpty();
			return;
		}
		if (maxlen == -1)
			maxlen = wcslen(wstr);
		alloc(maxlen+1);
		wcsncpy(m_wstr, wstr, maxlen);
		m_wstr[maxlen] = 0;
		update();
	}
	void alloc(unsigned int maxlen)
	{
		if (maxlen<16) maxlen=16;
		if (maxlen < m_maxlen) return;
		const wchar_t * old = m_wstr;
		m_wstr = (wchar_t *)new wchar_t[maxlen];
		m_maxlen = maxlen;
		wcscpy(m_wstr, old ? old : L"");
		if (old) { delete (wchar_t *)old; }
	}
	void append(const wchar_t * wstr, int maxlen=-1)
	{
		if (!wstr || !wstr[0] || !maxlen)
			return;
		if (maxlen == -1)
			maxlen = wcslen(wstr);
		int total = m_curlen + maxlen;
		alloc(total + 1);
		wcsncat(m_wstr, wstr, maxlen);
		m_wstr[total] = 0;
		update();
	}
	const wchar_t * string() const { return m_wstr; }
	const wchar_t * mid(unsigned int offset) const
	{
		if (offset >= 0 && offset < m_curlen)
			return &m_wstr[offset]; 
		else
			return L"";
	}
	wchar_t at(unsigned int offset) const
	{
		if (offset >= 0 && offset < m_curlen)
			return m_wstr[offset]; 
		else
			return 0;
	}
private:
	void release()
	{
		delete m_wstr;
		m_wstr = 0;
	}
	void update()
	{
		m_curlen = m_wstr ? wcslen(m_wstr) : 0;
	}
private:
	wchar_t * m_wstr;
	unsigned int m_maxlen;
	unsigned int m_curlen;
};

#endif //  widestr_h_included
