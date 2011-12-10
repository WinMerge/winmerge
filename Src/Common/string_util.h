/** 
 * @file  string_util.h
 *
 * @brief Char classification routines declarations.
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#define ISXKEYWORD(keywordlist, key, keylen) IsXKeyword(key, keylen, keywordlist, countof(keywordlist), _tcsncmp)
#define ISXKEYWORDI(keywordlist, key, keylen) IsXKeyword(key, keylen, keywordlist, countof(keywordlist), _tcsnicmp)

int xisalnum(wint_t c);
int xisspecial(wint_t c);
int xisalpha(wint_t c);
int xisalnum(wint_t c);
int xisspace(wint_t c);
bool IsXKeyword(LPCTSTR pszKey, size_t nKeyLen, LPCTSTR pszKeywordList[], size_t nKeywordListCount, int (*compare)(LPCTSTR, LPCTSTR, size_t));

#endif // _STRING_UTIL_H_
