/** 
 * @file  SyntaxColors.cpp
 *
 * @brief Implementation for SyntaxColors class.
 */
// ID line follows -- this is updated by SVN
// $Id: SyntaxColors.cpp 6727 2009-05-10 08:25:18Z kimmov $

#include "StdAfx.h"
#include <vector>
#include "SyntaxColors.h"

/**
 * @brief Constructor, initialise with default colors.
 */
SyntaxColors::SyntaxColors()
{
	m_colors.resize(COLORINDEX_COUNT);
	m_bolds.resize(COLORINDEX_COUNT);
	SetDefaults();
}

/**
 * @brief Copy constructor.
 * @param [in] pColors Instance to copy.
 */
SyntaxColors::SyntaxColors(const SyntaxColors *pColors)
{
	m_colors.resize(COLORINDEX_COUNT);
	m_bolds.resize(COLORINDEX_COUNT);
	m_colors = pColors->m_colors;
	m_bolds = pColors->m_bolds;
}

/**
 * @brief Copy colors from another instance.
 * @param [in] Instance from which to copy colors.
 */
void SyntaxColors::Clone(const SyntaxColors *pColors)
{
	m_colors.resize(COLORINDEX_COUNT);
	m_bolds.resize(COLORINDEX_COUNT);
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
		COLORREF color;

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
			color = RGB (0, 128, 192);
			break;
		case COLORINDEX_COMMENT:
		//      return RGB(128, 128, 128);
			color = RGB (0, 128, 0);
			break;
		case COLORINDEX_NUMBER:
		//      return RGB(0x80, 0x00, 0x00);
			color = RGB (0xff, 0x00, 0x00);
			break;
		case COLORINDEX_OPERATOR:
		//      return RGB(0x00, 0x00, 0x00);
			color = RGB (96, 96, 96);
			break;
		case COLORINDEX_KEYWORD:
			color = RGB (0, 0, 255);
			break;
		case COLORINDEX_FUNCNAME:
			color = RGB (128, 0, 128);
			break;
		case COLORINDEX_USER1:
			color = RGB (0, 0, 128);
			break;
		case COLORINDEX_USER2:
			color = RGB (0, 128, 192);
			break;
		case COLORINDEX_SELBKGND:
			color = RGB (0, 0, 0);
			break;
		case COLORINDEX_SELTEXT:
			color = RGB (255, 255, 255);
			break;
		case COLORINDEX_HIGHLIGHTBKGND1:
			color = RGB (255, 160, 160);
			break;
		case COLORINDEX_HIGHLIGHTTEXT1:
			color = RGB (0, 0, 0);
			break;
		case COLORINDEX_HIGHLIGHTBKGND2:
			color = RGB (255, 255, 0);
			break;
		case COLORINDEX_HIGHLIGHTTEXT2:
			color = RGB (0, 0, 0);
			break;
		case COLORINDEX_MARKERBKGND0:
			color = RGB (255, 255, 127);
			break;
		case COLORINDEX_MARKERBKGND1:
			color = RGB (127, 255, 127);
			break;
		case COLORINDEX_MARKERBKGND2:
			color = RGB (255, 127, 127);
			break;
		case COLORINDEX_MARKERBKGND3:
			color = RGB (127, 127, 255);
			break;
		default:
		//  return RGB(255, 0, 0);
			color = RGB (128, 0, 0);
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
void SyntaxColors::SetColor(UINT index, COLORREF color)
{
	m_colors[index] = color;
}

/**
 * @brief Set bold value.
 * @param [in] Index index of color to set (COLORINDEX).
 * @param [in] bold If true bold is enabled.
 */
void SyntaxColors::SetBold(UINT index, bool bold)
{
	m_bolds[index] = bold;
}
