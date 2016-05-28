#include <tchar.h>

#ifdef _DEBUG
static int FAIL_AT_LINE = 0;
#define COMPLAIN(X) ((X) || FAIL_AT_LINE == __LINE__)
#else
#define COMPLAIN(X) (X)
#endif

class Complain
{
public:
	TCHAR msg[1024];
	Complain(LPCTSTR, ...);
	Complain(DWORD, LPCTSTR, HMODULE = 0);
	int Alert(HWND, UINT = MB_ICONSTOP|MB_TASKMODAL);
};

class AutoBSTR
{
public:
	AutoBSTR(BSTR str) : m_str(str) { }
	~AutoBSTR() { SysFreeString(m_str); }
	UINT Len() const { return SysStringLen(m_str); }
	UINT ByteLen() const { return SysStringByteLen(m_str); }
	BSTR const m_str;
};

void NTAPI Release(IUnknown *);
