/**
 * @file  coretools.cpp
 *
 * @brief Common routines
 *
 */

#include "coretools.h"
#ifdef _WIN32
#  include <mbctype.h> // MBCS (multibyte codepage stuff)
#else
#  define _tcsinc(x) ((x) + 1)
#endif

size_t linelen(const char *string, size_t maxlen)
{
	size_t stringlen = 0;
	while (stringlen < maxlen)
	{
		char c = string[stringlen];
		if (c == '\r' || c == '\n' || c == '\0')
			break;
		++stringlen;
	}
	return stringlen;
}

void replace_char(TCHAR *s, int target, int repl)
{
	TCHAR *p;
	for (p=s; *p != _T('\0'); p = _tcsinc(p))
		if (*p == target)
			*p = (TCHAR)repl;
}

