/** 
 * @file  SyntaxColorsUtil.h
 *
 * @brief Declaration file for SyntaxColors class utility
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _SYNTAX_COLORS_UTIL_H_
#define _SYNTAX_COLORS_UTIL_H_

#include "stdafx.h"
#include <Windows.h>
#include <vector>
#include "SyntaxColors.h"

void SyntaxColors_LoadFromRegistry(SyntaxColors *pSyntaxColors);
void SyntaxColors_SaveToRegistry(const SyntaxColors *pSyntaxColors);
void SyntaxColors_LoadCustomColors(COLORREF * colors, int count);
void SyntaxColors_SaveCustomColors(const COLORREF * colors, int count);


#endif // _SYNTAX_COLORS_UTIL_H_
