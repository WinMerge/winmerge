/**
 * @file  coretools.h
 *
 * @brief Declaration file for Coretools.cpp
 */
#pragma once

#include "UnicodeString.h"

void replace_char(TCHAR *s, int target, int repl);

size_t linelen(const char *string, size_t maxlen);

#ifdef __cplusplus

#ifdef _WIN32
#include <strsafe.h>
#else
#include <string.h>
#endif

template <typename T, size_t N>
T *_tcscpy_safe(T(&dst)[N], const T *src)
{
#ifdef _WIN32
	StringCchCopy(reinterpret_cast<T *>(&dst), N, src);
#else
	strlcpy(dst, src, N);
#endif
	return reinterpret_cast<T *>(&dst);
}

#endif
