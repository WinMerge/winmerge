/** 
 * @file  SyntaxColors.h
 *
 * @brief Declaration file for SyntaxColors class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _SYNTAX_COLORS_H_
#define _SYNTAX_COLORS_H_

#include "stdafx.h"
#include <Windows.h>

/** 
 * @brief Indexes to color table
 */
enum COLORINDEX
{
	//
	COLORINDEX_NONE,
    //  Base colors
    COLORINDEX_WHITESPACE,
    COLORINDEX_BKGND,
    COLORINDEX_NORMALTEXT,
    COLORINDEX_SELMARGIN,
    COLORINDEX_SELBKGND,
    COLORINDEX_SELTEXT,
    //  Syntax colors
    COLORINDEX_KEYWORD,
    COLORINDEX_FUNCNAME,
    COLORINDEX_COMMENT,
    COLORINDEX_NUMBER,
    COLORINDEX_OPERATOR,
    COLORINDEX_STRING,
    COLORINDEX_PREPROCESSOR,
    //
    COLORINDEX_HIGHLIGHTBKGND1,
    COLORINDEX_HIGHLIGHTTEXT1,
    COLORINDEX_HIGHLIGHTBKGND2,
    COLORINDEX_HIGHLIGHTTEXT2,
	//
	COLORINDEX_USER1,
    COLORINDEX_USER2,
    //  ...
    //  Expandable: custom elements are allowed.
	COLORINDEX_LAST, // Please keep this as last item (not counting masks or
	                 // other special values)
    //
    COLORINDEX_APPLYFORCE = 0x80000000,
};

const int COLORINDEX_COUNT = COLORINDEX_LAST - COLORINDEX_NONE;

typedef CArray<COLORREF, COLORREF&> ColorArray;
typedef CArray<BOOL, BOOL&> BoolArray;

/** 
 * @brief Stores color information for syntax highlight.
 */
class SyntaxColors
{
public:
	SyntaxColors();
	SyntaxColors(SyntaxColors *pColors);
	void Clone(SyntaxColors *pColors);
	COLORREF GetColor(UINT index) const { return m_colors[index]; }
	void SetColor(UINT index, COLORREF color);
	BOOL GetBold(UINT index) const { return m_bolds[index]; }
	void SetBold(UINT index, BOOL bold);
	void SetDefaults();
	void SaveToRegistry();
	void ReadFromRegistry();

private:
	ColorArray m_colors; /**< Syntax highlight colors */
	BoolArray m_bolds; /**< Bold font enable/disable */
};


#endif // _SYNTAX_COLORS_H_
