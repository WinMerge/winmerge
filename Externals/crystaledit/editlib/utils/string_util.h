/** 
 * @file  string_util.h
 *
 * @brief Char classification routines declarations.
 */

#pragma once

int xisalnum(wint_t c);
int xisspecial(wint_t c);
int xisalpha(wint_t c);
int xisalnum(wint_t c);
int xisspace(wint_t c);

bool IsMBSTrail(const TCHAR *pszChars, int nCol);
