/** 
 * @file  OptionsSyntaxColors.cpp
 *
 * @brief Implementation for OptionsSyntaxColors class.
 */

#include "pch.h"
#include "OptionsSyntaxColors.h"
#include <vector>
#include "SyntaxColors.h"
#include "UnicodeString.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

using std::vector;

/** @brief Setting name for default colors. */
static const tchar_t DefColorsPath[] =_T("DefaultSyntaxColors");

namespace Options { namespace SyntaxColors {

void Init(COptionsMgr *pOptionsMgr, ::SyntaxColors *pSyntaxColors)
{
	String valuename(DefColorsPath);

	int count = COLORINDEX_COUNT;
	valuename += _T("/Values");
	pOptionsMgr->InitOption(valuename, count);

	for (unsigned i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		// Since we want to initialize with default colors (already
		// set to array, we must first call OptionsMrg->InitOption()
		// with default value. And since InitOption() reads stored value
		// from storage we must set that valu1Ge to array we use.
		int color = 0;
		CEColor ref;
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
		valuename = strutils::format(_T("%s/Color%02u"), DefColorsPath, i);
		pOptionsMgr->InitOption(valuename, color, serializable);
		color = pOptionsMgr->GetInt(valuename);
		ref = color;
		pSyntaxColors->SetColor(i, ref);
	
		valuename = strutils::format(_T("%s/Bold%02u"), DefColorsPath, i);
		pOptionsMgr->InitOption(valuename, pSyntaxColors->GetBold(i));
		pSyntaxColors->SetBold(i, pOptionsMgr->GetBool(valuename));
	}
}

/**
 * @brief Load color values from storage
 * @param [out] pSyntaxColors pointer to SyntaxColors
 */
void Load(COptionsMgr *pOptionsMgr, ::SyntaxColors *pSyntaxColors)
{
	for (unsigned i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		String valuename = strutils::format(_T("%s/Color%02u"), DefColorsPath, i);
		int color = pOptionsMgr->GetInt(valuename);
		CEColor ref = color;
		pSyntaxColors->SetColor(i, ref);
	
		valuename = strutils::format(_T("%s/Bold%02u"), DefColorsPath, i);
		pSyntaxColors->SetBold(i, pOptionsMgr->GetBool(valuename));
	}
}

/**
 * @brief Save color values to storage
 * @param [in] pSyntaxColors pointer to SyntaxColors
 */
void Save(COptionsMgr *pOptionsMgr, const ::SyntaxColors *pSyntaxColors)
{
	String valuename(DefColorsPath);

	int count = COLORINDEX_COUNT;
	valuename += _T("/Values");
	pOptionsMgr->SaveOption(valuename, count);

	for (unsigned i = COLORINDEX_NONE; i < COLORINDEX_LAST; i++)
	{
		int color = pSyntaxColors->GetColor(i);
		pOptionsMgr->SaveOption(strutils::format(_T("%s/Color%02u"), DefColorsPath, i), color);
		bool bold = pSyntaxColors->GetBold(i);
		pOptionsMgr->SaveOption(strutils::format(_T("%s/Bold%02u"), DefColorsPath, i), bold);
	}
}

}}
