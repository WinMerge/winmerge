/** 
 * @file  SyntaxColors.cpp
 *
 * @brief Implementation for SyntaxColors class.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "SyntaxColors.h"
#include "OptionsMgr.h"

static const TCHAR DefColorsPath[] =_T("DefaultSyntaxColors");

/**
 * @brief Constructor, initialise with default colors.
 */
SyntaxColors::SyntaxColors()
: m_pOptions(NULL)
{
	m_colors.SetSize(COLORINDEX_COUNT);
	m_bolds.SetSize(COLORINDEX_COUNT);
	SetDefaults();
}

/**
 * @brief Copy constructor.
 */
SyntaxColors::SyntaxColors(SyntaxColors *pColors)
: m_pOptions(NULL)
{
	m_colors.SetSize(COLORINDEX_COUNT);
	m_bolds.SetSize(COLORINDEX_COUNT);
	m_colors.Copy(pColors->m_colors);
	m_bolds.Copy(pColors->m_bolds);
}

/**
 * @brief Copy colors from another instance.
 */
void SyntaxColors::Clone(SyntaxColors *pColors)
{
	m_colors.SetSize(COLORINDEX_COUNT);
	m_bolds.SetSize(COLORINDEX_COUNT);
	m_colors.Copy(pColors->m_colors);
	m_bolds.Copy(pColors->m_bolds);
}

/**
 * @brief Set default color values.
 */
void SyntaxColors::SetDefaults()
{
	for (int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		COLORREF color;

		switch (i)
		{
		case COLORINDEX_WHITESPACE :
		case COLORINDEX_BKGND:
			color = GetSysColor (COLOR_WINDOW);
			break;
		case COLORINDEX_NORMALTEXT:
			color = GetSysColor (COLOR_WINDOWTEXT);
			break;
		case COLORINDEX_SELMARGIN:
			color = GetSysColor (COLOR_SCROLLBAR);
			break;
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
		m_colors.SetAt(i, color);

		BOOL bBold = FALSE;
		if (i == COLORINDEX_KEYWORD)
			bBold = TRUE;
		m_bolds.SetAt(i, (int)bBold);
	}
}

/**
 * @brief Set color value
 * @param [in] Index index of color to set (COLORINDEX).
 * @param [in] color New color value.
 */
void SyntaxColors::SetColor(UINT index, COLORREF color)
{
	m_colors.SetAt(index, color);
}

/**
 * @brief Set bold value.
 * @param [in] Index index of color to set (COLORINDEX).
 * @param [in] bold If TRUE bold is enabled.
 */
void SyntaxColors::SetBold(UINT index, BOOL bold)
{
	m_bolds.SetAt(index, bold);
}

/**
 * @brief Initialize color table.
 * @param [in] pOptionsMgr pointer to OptionsMgr used as storage.
 */
void SyntaxColors::Initialize(CRegOptions *pOptionsMgr)
{
	ASSERT(pOptionsMgr);
	CString valuename;

	m_pOptions = pOptionsMgr;

	int count = COLORINDEX_COUNT;
	valuename = DefColorsPath + '/';
	valuename += _T("Values");
	m_pOptions->InitOption(valuename, count);

	for (unsigned int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		// Since we want to initialize with default colors (already
		// set to array, we must first call OptionsMrg->InitOption()
		// with default value. And since InitOption() reads stored value
		// from storage we must set that value to array we use.
		int color = 0;
		COLORREF ref;
		valuename.Format(_T("%s/Color%02u"), DefColorsPath, i);
		color = m_colors.GetAt(i);
		m_pOptions->InitOption(valuename, color);
		color = m_pOptions->GetInt(valuename);
		ref = color;
		m_colors.SetAt(i, ref);
	
		int nBold = 0;
		BOOL bBold = FALSE;
		valuename.Format(_T("%s/Bold%02u"), DefColorsPath, i);
		bBold = m_bolds.GetAt(i);
		m_pOptions->InitOption(valuename, (int) bBold);
		nBold = m_pOptions->GetInt(valuename);
		bBold = nBold ? TRUE : FALSE;
		m_bolds.SetAt(i, bBold);
	}

}

/**
 * @brief Save color values to storage
 */
void SyntaxColors::SaveToRegistry()
{
	ASSERT(m_pOptions);
	CString valuename;

	int count = COLORINDEX_COUNT;
	valuename = DefColorsPath + '/';
	valuename += _T("Values");
	m_pOptions->SetInt(valuename, count);

	for (unsigned int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		valuename.Format(_T("%s/Color%02u"), DefColorsPath, i);
		int color = m_colors.GetAt(i);
		m_pOptions->SetInt(valuename, color);
		valuename.Format(_T("%s/Bold%02u"), DefColorsPath, i);
		BOOL bold = m_bolds.GetAt(i);
		m_pOptions->SetInt(valuename, bold);
	}
}

/**
 * @brief Read color values from storage.
 */
void SyntaxColors::ReadFromRegistry()
{
	ASSERT(m_pOptions);
	CString valuename;

	valuename = DefColorsPath + '/';
	valuename += _T("Values");
	int count = m_pOptions->GetInt(valuename);

	// Color count matches we propably have correct set saved
	// otherwise fallback to default colors
	if (count == COLORINDEX_COUNT)
	{
		for (unsigned int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
		{
			int color = 0;
			COLORREF ref;
			valuename.Format(_T("%s/Color%02u"), DefColorsPath, i);
			color = m_pOptions->GetInt(valuename);
			ref = color;
			m_colors.SetAt(i, ref);

			int nBold = 0;
			BOOL bBold = FALSE;
			valuename.Format(_T("%s/Bold%02u"), DefColorsPath, i);
			nBold = m_pOptions->GetInt(valuename);
			bBold = nBold ? TRUE : FALSE;
			m_bolds.SetAt(i, bBold);
		}
	}
	else
	{
		m_colors.SetSize(COLORINDEX_COUNT);
		SetDefaults();
	}
}
