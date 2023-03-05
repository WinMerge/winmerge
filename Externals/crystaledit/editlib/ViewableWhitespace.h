/** 
 * @file  ViewableWhitespace.h
 *
 * @brief Repository of character tables used to display whitespace (when View/Whitespace enabled)
 */

#pragma once

#include "utils/ctchar.h"

/**
 * @brief Structure containing characters for viewable whitespace chars.
 *
 * These characters are used when user wants to see whitespace characters
 * in editor.
 */
struct ViewableWhitespaceChars
{
	int c_codepage; /**< Used codepage, unused in UNICODE. */
	tchar_t c_tab[3]; /**< Visible character for tabs. */
	tchar_t c_space[3]; /**< Visible character for spaces. */
	tchar_t c_cr[3]; /**< Visible character for CR EOL chars. */
	tchar_t c_lf[3]; /**< Visible character for LF EOL chars. */
	tchar_t c_eol[3]; /**< Visible character for general or CRLF EOL chars. */
};

const ViewableWhitespaceChars * GetViewableWhitespaceChars(int codepage, bool directwrite);

