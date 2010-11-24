/** 
 * @file  string_util.cpp
 *
 * @brief Char classification routines implementations.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include <windows.h>
#include <tchar.h>
#include "string_util.h"

static wint_t normch(wint_t c);

// Convert any negative inputs to negative char equivalents
// This is aimed at correcting any chars mistakenly 
// sign-extended to negative ints.
// This is ok for the UNICODE build because UCS-2LE code bytes
// do not extend as high as 2Gig (actually even full Unicode
// codepoints don't extend that high).
static wint_t normch(wint_t c)
{
	return (unsigned short)(short)c;
}

/** @brief Return nonzero if input is outside ASCII or is underline. */
int xisspecial(wint_t c)
{
  return normch(c) > (unsigned) _T('\x7f') || c == _T('_');
}

/**
 * @brief Return non-zero if input is alphabetic or "special" (see xisspecial).
 * Also converts any negative inputs to negative char equivalents (see normch).
 */
int xisalpha(wint_t c)
{
  return _istalpha(normch(c)) || xisspecial(normch(c));
}

/**
 * @brief Return non-zero if input is alphanumeric or "special" (see xisspecial).
 * Also converts any negative inputs to negative char equivalents (see normch).
 */
int xisalnum(wint_t c)
{
  return _istalnum(normch(c)) || xisspecial(normch(c));
}

/**
 * @brief Return non-zero if input character is a space.
 * Also converts any negative inputs to negative char equivalents (see normch).
 */
int xisspace(wint_t c)
{
  return _istspace(normch(c));
}
