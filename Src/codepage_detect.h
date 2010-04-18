/** 
 * @file  codepage_detect.h
 *
 * @brief Declaration file for codepage detection routines.
 */
// ID line follows -- this is updated by SVN
// $Id: codepage_detect.h 5584 2008-07-09 12:09:56Z kimmov $

#ifndef codepage_detect_h_included
#define codepage_detect_h_included

struct FileTextEncoding;

void GuessCodepageEncoding(LPCTSTR filepath, FileTextEncoding * encoding, int guessEncodingType);

unsigned GuessEncoding_from_bytes(LPCTSTR ext, const char *src, size_t len);

#endif // codepage_detect_h_included
