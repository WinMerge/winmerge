/** 
 * @file  ViewableWhitespace.cpp
 *
 * @brief Repository of character tables used to display whitespace (when View/Whitespace enabled)
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "StdAfx.h" 
#include "ViewableWhitespace.h"

/** @brief Is structure initialized? */
static bool f_initialized = false;

// For UNICODE build, there is just one set which is always used
// tab, space, cr, lf, eol

/** @brief Structure for whitespace characters.
 *  Char order is tab, space, cr, lf, eol.
 */
static struct ViewableWhitespaceChars
// Do not use L literals, as they involve runtime mbcs expansion, apparently
 f_specialChars = {
	0,
	L" ", // U+BB: RIGHT POINTING DOUBLE ANGLE QUOTATION MARK
	L" ", // U+B7: MIDDLE DOT
	L" ", // U+A7: SECTION SIGN
	L" ", // U+B6: PILCROW SIGN
	L" " // U+A4: CURRENCY SIGN
};

/**
 * @brief Initialize whitespace chars structure.
 */
static void initialize()
{
	f_specialChars.c_tab[0] = 0xBB;
	f_specialChars.c_space[0] = 0xB7;
	f_specialChars.c_cr[0] = 0xA7;
	f_specialChars.c_lf[0] = 0xB6;
	f_specialChars.c_eol[0] = 0xA4;

	f_initialized = true;
}

/**
 * @brief Return viewable whitespace chars.
 * @param [in] codepage Used codepage (only efective in ANSI builds).
 * @return Pointer to structure having viewable chars.
 */
const ViewableWhitespaceChars * GetViewableWhitespaceChars(int codepage)
{
	if (!f_initialized)
		initialize();
	return &f_specialChars;
}
