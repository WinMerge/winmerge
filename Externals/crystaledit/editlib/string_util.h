/** 
 * @file  string_util.h
 *
 * @brief Char classification routines declarations.
 */
// ID line follows -- this is updated by SVN
// $Id$

#pragma once

#define ISXKEYWORD(keywordlist, key, keylen) IsXKeyword(key, keylen, keywordlist, sizeof(keywordlist)/sizeof(keywordlist[0]), _tcsncmp)
#define ISXKEYWORDI(keywordlist, key, keylen) IsXKeyword(key, keylen, keywordlist, sizeof(keywordlist)/sizeof(keywordlist[0]), _tcsnicmp)

int xisalnum(wint_t c);
int xisspecial(wint_t c);
int xisalpha(wint_t c);
int xisalnum(wint_t c);
int xisspace(wint_t c);
bool IsXKeyword(LPCTSTR pszKey, size_t nKeyLen, LPCTSTR pszKeywordList[], size_t nKeywordListCount, int (*compare)(LPCTSTR, LPCTSTR, size_t));

bool IsMBSTrail(const TCHAR *pszChars, int nCol);
