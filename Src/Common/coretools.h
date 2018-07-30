/**
 * @file  coretools.h
 *
 * @brief Declaration file for Coretools.cpp
 */
#pragma once

#include "UnicodeString.h"

size_t linelen(const char *string, size_t maxlen);

#ifdef __cplusplus

#include <strsafe.h>

template <typename T, size_t N>
T *_tcscpy_safe(T(&dst)[N], const T *src)
{
	StringCchCopy(reinterpret_cast<T *>(&dst), N, src);
	return reinterpret_cast<T *>(&dst);
}

#endif
