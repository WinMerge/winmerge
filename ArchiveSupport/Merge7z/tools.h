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

void NTAPI Release(IUnknown *);
