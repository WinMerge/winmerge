/** 
 * @file  SyntaxColors.cpp
 *
 * @brief Implementation for SyntaxColors class.
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include <vector>
#include "UnicodeString.h"
#include "SyntaxColors.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

using namespace std;

/** @brief Setting name for default colors. */
static const TCHAR DefColorsPath[] =_T("DefaultSyntaxColors");
/** @brief Setting name for user-defined custom colors. */
static const TCHAR Section[] = _T("Custom Colors");

/**
 * @brief Constructor, initialise with default colors.
 */
SyntaxColors::SyntaxColors()
: m_pOptionsMgr(NULL)
{
	m_colors.reserve(COLORINDEX_COUNT);
	m_bolds.reserve(COLORINDEX_COUNT);
	SetDefaults();
}

/**
 * @brief Copy constructor.
 * @param [in] pColors Instance to copy.
 */
SyntaxColors::SyntaxColors(const SyntaxColors *pColors)
: m_pOptionsMgr(NULL)
{
	m_colors.reserve(COLORINDEX_COUNT);
	m_bolds.reserve(COLORINDEX_COUNT);
	m_colors = pColors->m_colors;
	m_bolds = pColors->m_bolds;
}

/**
 * @brief Copy colors from another instance.
 * @param [in] Instance from which to copy colors.
 */
void SyntaxColors::Clone(const SyntaxColors *pColors)
{
	m_colors.reserve(COLORINDEX_COUNT);
	m_bolds.reserve(COLORINDEX_COUNT);
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
			*pSysIndex = COLOR_SCROLLBAR;
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
		default:
		//  return RGB(255, 0, 0);
			color = RGB (128, 0, 0);
			break;
		}
		m_colors[i] = color;

		BOOL bBold = FALSE;
		if (i == COLORINDEX_KEYWORD)
			bBold = TRUE;
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
 * @param [in] bold If TRUE bold is enabled.
 */
void SyntaxColors::SetBold(UINT index, BOOL bold)
{
	m_bolds[index] = bold;
}

/**
 * @brief Initialize color table.
 * @param [in] pOptionsMgr pointer to OptionsMgr used as storage.
 */
void SyntaxColors::Initialize(COptionsMgr *pOptionsMgr)
{
	ASSERT(pOptionsMgr);
	String valuename(DefColorsPath);

	m_pOptionsMgr = pOptionsMgr;

	int count = COLORINDEX_COUNT;
	valuename += _T("/Values");
	m_pOptionsMgr->InitOption(valuename.c_str(), count);

	for (unsigned int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		// Since we want to initialize with default colors (already
		// set to array, we must first call OptionsMrg->InitOption()
		// with default value. And since InitOption() reads stored value
		// from storage we must set that value to array we use.
		int color = 0;
		COLORREF ref;
		valuename.reserve(30);
		_sntprintf(&*valuename.begin(), 30, _T("%s/Color%02u"),
			DefColorsPath, i);
		color = m_colors[i];

		// Special handling for themable colors
		// These are text colors which by default follow the current system colors
		// unless the user has overridden this behavior to specify them explicitly
		bool serializable = true;
		if (IsThemeableColorIndex(i))
		{
			if (m_pOptionsMgr->GetBool(OPT_CLR_DEFAULT_TEXT_COLORING))
				serializable = false;
		}
		m_pOptionsMgr->InitOption(valuename.c_str(), color, serializable);
		color = m_pOptionsMgr->GetInt(valuename.c_str());
		ref = color;
		m_colors[i] = ref;
	
		int nBold = 0;
		BOOL bBold = FALSE;
		valuename.reserve(30);
		_sntprintf(&*valuename.begin(), 30, _T("%s/Bold%02u"),
			DefColorsPath, i);
		bBold = m_bolds[i];
		m_pOptionsMgr->InitOption(valuename.c_str(), (int) bBold);
		nBold = m_pOptionsMgr->GetInt(valuename.c_str());
		bBold = nBold ? TRUE : FALSE;
		m_bolds[i] = bBold;
	}
}

/**
 * @brief Save color values to storage
 */
void SyntaxColors::SaveToRegistry()
{
	ASSERT(m_pOptionsMgr);
	String valuename(DefColorsPath);

	int count = COLORINDEX_COUNT;
	valuename += _T("/Values");
	m_pOptionsMgr->SetInt(valuename.c_str(), count);

	for (unsigned int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		valuename.reserve(30);
		_sntprintf(&*valuename.begin(), 30, _T("%s/Color%02u"),
			DefColorsPath, i);
		int color = m_colors[i];
		m_pOptionsMgr->SetInt(valuename.c_str(), color);
		valuename.reserve(30);
		_sntprintf(&*valuename.begin(), 30, _T("%s/Bold%02u"),
			DefColorsPath, i);
		BOOL bold = m_bolds[i];
		m_pOptionsMgr->SetInt(valuename.c_str(), bold);
	}
}

void SyntaxColors_Load(COLORREF * colors, int count)
{
	for (int i = 0; i < count; i++)
	{
		CString sEntry;
		sEntry.Format(_T("%d"), i);
		colors[i] = ::AfxGetApp()->GetProfileInt(Section,
			sEntry, RGB(255, 255, 255));
	}
}

void SyntaxColors_Save(COLORREF * colors, int count)
{
	for (int i = 0; i < count; i++)
	{
		CString sEntry;
		sEntry.Format(_T("%d"), i);
		if (colors[i] == RGB(255, 255, 255))
			::AfxGetApp()->WriteProfileString(Section, sEntry, NULL);
		else 
			::AfxGetApp()->WriteProfileInt(Section, sEntry, colors[i]);
	}
}

