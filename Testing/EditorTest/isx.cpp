#include "stdafx.h"
#include "isx.h"

int normch(int c)
{
#ifdef _UNICODE
	return (unsigned short)(short)c;
#else
	return (unsigned char)(char)c;
#endif
}

// Returns nonzero if input is outside ASCII or is underline
int
xisspecial (int c)
{
  return normch(c) > (unsigned) _T ('\x7f') || c == _T ('_');
}

// Returns non-zero if input is alphabetic or "special" (see xisspecial)
// Also converts any negative inputs to negative char equivalents (see normch)
int
xisalpha (int c)
{
  return _istalpha (normch(c)) || xisspecial (normch(c));
}

// Returns non-zero if input is alphanumeric or "special" (see xisspecial)
// Also converts any negative inputs to negative char equivalents (see normch)
int
xisalnum (int c)
{
  return _istalnum (normch(c)) || xisspecial (normch(c));
}

// Returns non-zero if input character is a space
// Also converts any negative inputs to negative char equivalents (see normch)
int
xisspace (int c)
{
  return _istspace (normch(c));
}


// Get appropriate clipboard format for TCHAR text
int GetClipTcharTextFormat()
{
#ifdef _UNICODE
	return CF_UNICODETEXT;
#else
	return CF_TEXT;
#endif // _UNICODE
}

BOOL IsXKeyword(LPCTSTR pszKey, size_t nKeyLen, LPCTSTR pszKeywordList[], size_t nKeywordListCount, int (*compare)(LPCTSTR, LPCTSTR, size_t))
{
	TCHAR **base = (TCHAR **)pszKeywordList;
	int lim, cmp;
	TCHAR **p;

	for (lim = nKeywordListCount; lim != 0; lim >>= 1) {
		p = base + (lim >> 1) ;
		cmp = (*compare)(pszKey, *p, nKeyLen);
		if (cmp == 0 && (*p)[nKeyLen] == 0)
			return TRUE;
		if (cmp > 0) {	/* key > p: move right */
			base = (TCHAR **)p + 1;
			lim--;
		} /* else move left */
	}
	return FALSE;
}

