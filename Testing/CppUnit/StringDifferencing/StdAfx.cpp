// stdafx.cpp : source file that includes just the standard includes
//	StringDifferencing.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

/**
 * Code below is copied from /Src/StdAfx.h
 */

// Convert any negative inputs to negative char equivalents
// This is aimed at correcting any chars mistakenly 
// sign-extended to negative ints.
// This is ok for the UNICODE build because UCS-2LE code bytes
// do not extend as high as 2Gig (actually even full Unicode
// codepoints don't extend that high).
static wint_t normch(wint_t c)
{
#ifdef _UNICODE
	return (unsigned short)(short)c;
#else
	return (unsigned char)(char)c;
#endif
}

/**
 * @brief Return non-zero if input character is a space.
 * Also converts any negative inputs to negative char equivalents (see normch).
 */
int xisspace (wint_t c)
{
  return _istspace (normch(c));
}
