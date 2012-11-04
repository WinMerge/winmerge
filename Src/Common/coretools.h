/**
 * @file  coretools.h
 *
 * @brief Declaration file for Coretools.cpp
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef CORETOOLS_H
#define CORETOOLS_H

#include "UnicodeString.h"

void replace_char(TCHAR *s, int target, int repl);

size_t linelen(const char *string, size_t maxlen);

#endif
