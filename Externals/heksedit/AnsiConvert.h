class MakeAnsi
{
private:
	BSTR m_bstr;
public:
	MakeAnsi(PCWSTR text, UINT codepage = CP_ACP, int textlen = -1);
	MakeAnsi(): m_bstr(0) { }
	~MakeAnsi() { SysFreeString(m_bstr); }
	operator PCSTR() { return (PCSTR)m_bstr; }
	UINT GetLength()
	{
		return SysStringByteLen(m_bstr);
	}
	PSTR GetBufferSetLength(UINT len)
	{
		assert((len & 1) == 0);
		return SysReAllocStringLen(&m_bstr, 0, len / 2) ? PSTR(m_bstr) : 0;
	}
};

class MakeWide
{
private:
	BSTR m_bstr;
public:
	MakeWide(PCSTR text, UINT codepage = CP_ACP, int textlen = -1);
	MakeWide(): m_bstr(0) { }
	~MakeWide() { SysFreeString(m_bstr); }
	operator PCWSTR() { return m_bstr; }
	UINT GetLength()
	{
		return SysStringLen(m_bstr);
	}
	PWSTR GetBufferSetLength(UINT len)
	{
		return SysReAllocStringLen(&m_bstr, 0, len) ? m_bstr : 0;
	}
};

typedef MakeWide A2W;
typedef MakeAnsi W2A;
#ifdef UNICODE
typedef PCWSTR T2W, W2T;
typedef MakeAnsi T2A;
typedef MakeWide A2T;
#else
typedef PCSTR T2A, A2T;
typedef MakeWide T2W;
typedef MakeAnsi W2T;
#endif
