/** 
 * @file  SyntaxColors.h
 *
 * @brief Declaration file for SyntaxColors class
 */
// ID line follows -- this is updated by SVN
// $Id$

#pragma once

#include <array>

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
    COLORINDEX_HIGHLIGHTBKGND1, // standard
    COLORINDEX_HIGHLIGHTTEXT1,
    COLORINDEX_HIGHLIGHTBKGND2, // changed
    COLORINDEX_HIGHLIGHTTEXT2,
    COLORINDEX_HIGHLIGHTBKGND3, //  not selected insert/delete
	COLORINDEX_HIGHLIGHTBKGND4, // selected insert/delete
	//
	COLORINDEX_USER1,
    COLORINDEX_USER2,
    //
    COLORINDEX_MARKERBKGND0,
    COLORINDEX_MARKERBKGND1,
    COLORINDEX_MARKERBKGND2,
    COLORINDEX_MARKERBKGND3,
    //  ...
    //  Expandable: custom elements are allowed.
	COLORINDEX_LAST, // Please keep this as last item (not counting masks or
	                 // other special values)
    //
    COLORINDEX_APPLYFORCE = 0x80000000,
};

const int COLORINDEX_COUNT = COLORINDEX_LAST - COLORINDEX_NONE;

/** 
 * @brief Wrapper for Syntax coloring colors.
 *
 * This class is wrapper for syntax colors. We can use this class in editor
 * class and everywhere we need to refer to syntax colors. Class uses our
 * normal options-manager for loading / saving values to storage.
 *
 * @todo We don't really need those arrays to store color values since we now
 * use options-manager.
 */
class SyntaxColors
{
public:
	typedef unsigned COLORREF;
	SyntaxColors();
	explicit SyntaxColors(const SyntaxColors *pColors);
	void Clone(const SyntaxColors *pColors);
	COLORREF GetColor(unsigned index) const { return m_colors[index]; }
	void SetColor(unsigned index, COLORREF color);
	bool GetBold(unsigned index) const { return m_bolds[index]; }
	void SetBold(unsigned index, bool bold);
	void SetDefaults();
	bool IsThemeableColorIndex(int nColorIndex) const;
	bool GetSystemColorIndex(int nColorIndex, int * pSysIndex) const;

// Implementation data
private:
	std::array<COLORREF, COLORINDEX_COUNT> m_colors; /**< Syntax highlight colors */
	std::array<bool, COLORINDEX_COUNT> m_bolds; /**< Bold font enable/disable */
};
