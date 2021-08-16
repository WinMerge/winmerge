/** 
 * @file  codepage_detect.h
 *
 * @brief Declaration file for codepage detection routines.
 */
#pragma once

#include "UnicodeString.h"
#include "FileTextEncoding.h"

namespace codepage_detect
{
/** @brief Buffer size used in this file. */
constexpr int BufSize = 65536;

FileTextEncoding Guess(const String& filepath, int guessEncodingType, ptrdiff_t mapmaxlen = BufSize);
FileTextEncoding Guess(const String& ext, const void* src, size_t len, int guessEncodingType);
}
