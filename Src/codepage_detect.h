/** 
 * @file  codepage_detect.h
 *
 * @brief Declaration file for codepage detection routines.
 */
// ID line follows -- this is updated by SVN
// $Id: codepage_detect.h 5584 2008-07-09 12:09:56Z kimmov $

#ifndef codepage_detect_h_included
#define codepage_detect_h_included

#include "UnicodeString.h"
#include "FileTextEncoding.h"

FileTextEncoding GuessCodepageEncoding(const String& filepath, int guessEncodingType);

#endif // codepage_detect_h_included
