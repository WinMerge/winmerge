/**
 * @file  coretools.h
 *
 * @brief Declaration file for Coretools.cpp
 */
#pragma once

#include "UnicodeString.h"

void replace_char(TCHAR *s, int target, int repl);

size_t linelen(const char *string, size_t maxlen);
