#include "precomp.h"

PList::PList()
: Count(0)
{
}

PList::~PList()
{
	DeleteContents();
}

void PList::DeleteContents()
{
	while (Flink != this)
		delete static_cast<PNode *>(Flink);
	Count = 0;
}

void PList::AddTail(PNode *p)
{
	p->Flink = Blink->Flink;
	p->Blink = Blink;
	Blink->Flink = p;
	Blink = p;
	++Count;
}

/**
 * @brief Simple wrapper around _vsntprintf.
 * @note The capacity is read from the passed-in buffer.
 */
int PFormat::Format(LPCTSTR fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t count = capacity - 1;
	int n = _vsntprintf(buffer, count, fmt, args);
	buffer[count] = _T('\0');
	return n >= 0 ? n : count;
}

void TRACE(const char* pszFormat,...)
{
#ifdef _DEBUG
	static BOOL bFirstTime = TRUE;
	FILE* fp = fopen("frhed.log",bFirstTime?"w":"a");
	if(fp)
	{
		bFirstTime = FALSE;
		va_list args;
		va_start( args, pszFormat );
		vfprintf( fp, pszFormat, args );
		fclose(fp);
	}
#endif //_DEBUG
}
