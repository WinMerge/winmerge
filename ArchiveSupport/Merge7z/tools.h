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
	int Alert(HWND, UINT = MB_ICONSTOP|MB_TASKMODAL);
};

void ComplainCreateObject(HMODULE, LPCTSTR);
void ComplainNotFound(LPCTSTR);
void ComplainCantOpen(LPCTSTR);

void NTAPI Release(IUnknown *);

struct EnumList
{
	TCHAR buffer[INT_MAX];
	int Find(LPCTSTR r, BOOL fCaseSens) const;
};

#define ENUM(X) X,
#define _ENUM(X) _##X,
#define ENUM_LIST(T,X) struct T: ::EnumList{enum{X N};} const*const T=(struct T const*)_T(#X);
 
