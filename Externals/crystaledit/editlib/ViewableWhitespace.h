/** 
 * @file  ViewableWhitespace.h
 *
 * @brief Repository of character tables used to display whitespace (when View/Whitespace enabled)
 */
// RCS ID line follows -- this is updated by CVS
// $Id: ViewableWhitespace.h 3410 2006-07-31 17:14:15Z kimmov $

#pragma once

/**
 * @brief Structure containing characters for viewable whitespace chars.
 *
 * These characters are used when user wants to see whitespace characters
 * in editor.
 */
struct ViewableWhitespaceChars
{
	int c_codepage; /**< Used codepage, unused in UNICODE. */
	TCHAR c_tab[3]; /**< Visible character for tabs. */
	TCHAR c_space[3]; /**< Visible character for spaces. */
	TCHAR c_cr[3]; /**< Visible character for CR EOL chars. */
	TCHAR c_lf[3]; /**< Visible character for LF EOL chars. */
	TCHAR c_eol[3]; /**< Visible character for general or CRLF EOL chars. */
};

const ViewableWhitespaceChars * GetViewableWhitespaceChars(int codepage);

