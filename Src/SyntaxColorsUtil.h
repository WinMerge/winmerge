/** 
 * @file  SyntaxColorsUtil.h
 *
 * @brief Declaration file for SyntaxColors class utility
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _SYNTAX_COLORS_UTIL_H_
#define _SYNTAX_COLORS_UTIL_H_

class SyntaxColors;

void SyntaxColors_LoadFromRegistry(SyntaxColors *pSyntaxColors);
void SyntaxColors_SaveToRegistry(const SyntaxColors *pSyntaxColors);

#endif // _SYNTAX_COLORS_UTIL_H_
