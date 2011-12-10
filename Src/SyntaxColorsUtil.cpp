/** 
 * @file  SyntaxColors.cpp
 *
 * @brief Implementation for SyntaxColors class.
 */
// ID line follows -- this is updated by SVN
// $Id: SyntaxColors.cpp 6727 2009-05-10 08:25:18Z kimmov $

#include "StdAfx.h"
#include <vector>
#include "Merge.h"
#include "UnicodeString.h"
#include "SyntaxColorsUtil.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

using std::vector;

/** @brief Setting name for default colors. */
static const TCHAR DefColorsPath[] =_T("DefaultSyntaxColors");
/** @brief Setting name for user-defined custom colors. */
static const TCHAR Section[] = _T("Custom Colors");

/**
 * @brief Load color values from storage
 * @param [out] pSyntaxColors pointer to SyntaxColors
 */
void SyntaxColors_LoadFromRegistry(SyntaxColors *pSyntaxColors)
{
	COptionsMgr *pOptionsMgr = GetOptionsMgr();
	String valuename(DefColorsPath);

	int count = COLORINDEX_COUNT;
	valuename += _T("/Values");
	pOptionsMgr->InitOption(valuename.c_str(), count);

	for (unsigned int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		// Since we want to initialize with default colors (already
		// set to array, we must first call OptionsMrg->InitOption()
		// with default value. And since InitOption() reads stored value
		// from storage we must set that valu1Ge to array we use.
		int color = 0;
		COLORREF ref;
		valuename.resize(30);
		_sntprintf(&*valuename.begin(), 30, _T("%s/Color%02u"),
			DefColorsPath, i);
		color = pSyntaxColors->GetColor(i);

		// Special handling for themable colors
		// These are text colors which by default follow the current system colors
		// unless the user has overridden this behavior to specify them explicitly
		bool serializable = true;
		if (pSyntaxColors->IsThemeableColorIndex(i))
		{
			if (pOptionsMgr->GetBool(OPT_CLR_DEFAULT_TEXT_COLORING))
				serializable = false;
		}
		pOptionsMgr->InitOption(valuename.c_str(), color, serializable);
		color = pOptionsMgr->GetInt(valuename.c_str());
		ref = color;
		pSyntaxColors->SetColor(i, ref);
	
		int nBold = 0;
		BOOL bBold = FALSE;
		valuename.resize(30);
		_sntprintf(&*valuename.begin(), 30, _T("%s/Bold%02u"),
			DefColorsPath, i);
		bBold = pSyntaxColors->GetBold(i);
		pOptionsMgr->InitOption(valuename.c_str(), (int) bBold);
		nBold = pOptionsMgr->GetInt(valuename.c_str());
		bBold = nBold ? TRUE : FALSE;
		pSyntaxColors->SetBold(i, bBold);
	}
}

/**
 * @brief Save color values to storage
 * @param [in] pSyntaxColors pointer to SyntaxColors
 */
void SyntaxColors_SaveToRegistry(const SyntaxColors *pSyntaxColors)
{
	COptionsMgr *pOptionsMgr = GetOptionsMgr();
	String valuename(DefColorsPath);

	int count = COLORINDEX_COUNT;
	valuename += _T("/Values");
	pOptionsMgr->SaveOption(valuename.c_str(), count);

	for (unsigned int i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		valuename.resize(30);
		_sntprintf(&*valuename.begin(), 30, _T("%s/Color%02u"),
			DefColorsPath, i);
		int color = pSyntaxColors->GetColor(i);
		pOptionsMgr->SaveOption(valuename.c_str(), color);
		valuename.resize(30);
		_sntprintf(&*valuename.begin(), 30, _T("%s/Bold%02u"),
			DefColorsPath, i);
		BOOL bold = pSyntaxColors->GetBold(i);
		pOptionsMgr->SaveOption(valuename.c_str(), bold);
	}
}

void SyntaxColors_LoadCustomColors(COLORREF * colors, int count)
{
	for (int i = 0; i < count; i++)
	{
		CString sEntry;
		sEntry.Format(_T("%d"), i);
		colors[i] = ::AfxGetApp()->GetProfileInt(Section,
			sEntry, RGB(255, 255, 255));
	}
}

void SyntaxColors_SaveCustomColors(const COLORREF * colors, int count)
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

