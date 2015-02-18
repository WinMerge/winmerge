/** 
 * @file  codepage_detect.h
 *
 * @brief Declaration file for codepage detection routines.
 */
#pragma once

#include "UnicodeString.h"
#include "FileTextEncoding.h"

FileTextEncoding GuessCodepageEncoding(const String& filepath, int guessEncodingType);
