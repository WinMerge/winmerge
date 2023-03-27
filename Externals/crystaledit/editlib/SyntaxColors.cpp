/** 
 * @file  SyntaxColors.cpp
 *
 * @brief Implementation for SyntaxColors class.
 */

#include "pch.h"
#include "SyntaxColors.h"
#include <Windows.h>

/**
 * @brief Constructor, initialise with default colors.
 */
SyntaxColors::SyntaxColors()
{
	SetDefaults();
}

/**
 * @brief Copy constructor.
 * @param [in] pColors Instance to copy.
 */
SyntaxColors::SyntaxColors(const SyntaxColors *pColors)
	: m_colors{pColors->m_colors}
	, m_bolds{pColors->m_bolds}
{
}

/**
 * @brief Copy colors from another instance.
 * @param [in] Instance from which to copy colors.
 */
void SyntaxColors::Clone(const SyntaxColors *pColors)
{
	m_colors = pColors->m_colors;
	m_bolds = pColors->m_bolds;
}

/**
 * @brief Checks if given color is themeable.
 * @param [in] nColorIndex Index of color to check.
 * @return true if color is themeable, false otherwise.
 */
bool SyntaxColors::IsThemeableColorIndex(int nColorIndex) const
{
	int temp = 0;
	return GetSystemColorIndex(nColorIndex, &temp);
}

/**
 * @brief Get system color for this index.
 * Returns the system color for given index in the case it varies by
 * used theme.
 * @param [in] nColorIndex Index of color to get.
 * @param [out] pSysIndex System color index, if any.
 * @return true if system color index was found, false otherwise.
 */
bool SyntaxColors::GetSystemColorIndex(int nColorIndex, int * pSysIndex) const
{
	switch (nColorIndex)
	{
		case COLORINDEX_WHITESPACE :
		case COLORINDEX_BKGND:
			*pSysIndex = COLOR_WINDOW;
			return true;
		case COLORINDEX_NORMALTEXT:
			*pSysIndex = COLOR_WINDOWTEXT;
			return true;
		case COLORINDEX_SELMARGIN:
			*pSysIndex = COLOR_BTNFACE;
			return true;
		case COLORINDEX_SELTEXT:
			*pSysIndex = COLOR_HIGHLIGHTTEXT;
			return true;
		case COLORINDEX_SELBKGND:
			*pSysIndex = COLOR_HIGHLIGHT;
			return true;
	}
	return false;
}

/**
 * @brief Set default color values.
 */
void SyntaxColors::SetDefaults()
{
	for (int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		CEColor color;

		int nSysIndex = 0;
		if (GetSystemColorIndex(i, &nSysIndex))
		{
			// Colors that vary with Windows theme
			color = GetSysColor(nSysIndex);
		}
		else switch (i)
		{
			// Theme colors are handled above by GetSystemColorIndex
			//
			// COLORINDEX_WHITESPACE
			// COLORINDEX_BKGND:
			// COLORINDEX_NORMALTEXT:
			// COLORINDEX_SELMARGIN:

			// Hardcoded defaults
		case COLORINDEX_PREPROCESSOR:
			color = { 0, 128, 192 };
			break;
		case COLORINDEX_COMMENT:
		//      return { 128, 128, 128 };
			color = { 0, 128, 0 };
			break;
		case COLORINDEX_NUMBER:
		//      return { 0x80, 0x00, 0x00 };
			color = { 0xff, 0x00, 0x00 };
			break;
		case COLORINDEX_OPERATOR:
		//      return { 0x00, 0x00, 0x00 };
			color = { 96, 96, 96 };
			break;
		case COLORINDEX_KEYWORD:
			color = { 0, 0, 255 };
			break;
		case COLORINDEX_FUNCNAME:
			color = { 128, 0, 128 };
			break;
		case COLORINDEX_USER1:
			color = { 0, 0, 128 };
			break;
		case COLORINDEX_USER2:
			color = { 0, 128, 192 };
			break;
		case COLORINDEX_SELBKGND:
			color = { 0, 0, 0 };
			break;
		case COLORINDEX_SELTEXT:
			color = { 255, 255, 255 };
			break;
		case COLORINDEX_HIGHLIGHTBKGND1:
			color = { 255, 160, 160 };
			break;
		case COLORINDEX_HIGHLIGHTTEXT1:
			color = { 0, 0, 0 };
			break;
		case COLORINDEX_HIGHLIGHTBKGND2:
			color = { 255, 255, 0 };
			break;
		case COLORINDEX_HIGHLIGHTTEXT2:
			color = { 0, 0, 0 };
			break;
		case COLORINDEX_MARKERBKGND0:
			color = { 255, 255, 127 };
			break;
		case COLORINDEX_MARKERBKGND1:
			color = { 127, 255, 127 };
			break;
		case COLORINDEX_MARKERBKGND2:
			color = { 255, 127, 127 };
			break;
		case COLORINDEX_MARKERBKGND3:
			color = { 127, 127, 255 };
			break;
		default:
		//  return { 255, 0, 0 };
			color = { 128, 0, 0 };
			break;
		}
		m_colors[i] = color;

		bool bBold = false;
		if (i == COLORINDEX_KEYWORD)
			bBold = true;
		m_bolds[i] = bBold;
	}
}

/**
 * @brief Set color value
 * @param [in] Index index of color to set (COLORINDEX).
 * @param [in] color New color value.
 */
void SyntaxColors::SetColor(unsigned index, CEColor color)
{
	m_colors[index] = color;
}

/**
 * @brief Set bold value.
 * @param [in] Index index of color to set (COLORINDEX).
 * @param [in] bold If true bold is enabled.
 */
void SyntaxColors::SetBold(unsigned index, bool bold)
{
	m_bolds[index] = bold;
}
