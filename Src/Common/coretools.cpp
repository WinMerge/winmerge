/**
 * @file  coretools.cpp
 *
 * @brief Common routines
 *
 */

#include "pch.h"
#include "coretools.h"

static bool iseolch(char ch)
{
	return ch == '\n' || ch == '\r';
}

size_t linelen(const char *string, size_t maxlen)
{
	const char *q = string + maxlen;
	do
	{
		maxlen = q - string;
	} while (maxlen && iseolch(*--q));
	return maxlen;
}
