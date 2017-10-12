/** 
 * @file  codepage_detect.h
 *
 * @brief Declaration file for codepage detection routines.
 */
#pragma once

#include "UnicodeString.h"
#include "FileTextEncoding.h"

/** @brief Buffer size used in this file. */
static const int BufSize = 65536;

FileTextEncoding GuessCodepageEncoding(const String& filepath, int guessEncodingType, ptrdiff_t mapmaxlen = BufSize);
