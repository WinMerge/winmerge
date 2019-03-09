/**
 * @file  coretools.cpp
 *
 * @brief Common routines
 *
 */

#include "pch.h"
#include "coretools.h"

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

