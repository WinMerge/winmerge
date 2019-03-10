/** 
 * @file  OptionsFont.cpp
 *
 * @brief Implementation for Options::Font class.
 */
#include "pch.h"
#include "OptionsFont.h"
#include "unicoder.h"
#include "ExConverter.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include <cassert>

namespace Options { namespace Font {

/**
 * @brief Initialize basic default values in a LOGFONT structure. 
 * A helper function for SetDefaults(); it should not be used otherwise.
 */
void InitializeLogFont(LOGFONT &logfont, int lfHeight, int lfCharSet, int lfPitchAndFamily, String lfFaceName)
{
	logfont.lfHeight = lfHeight;
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = FW_NORMAL;
	logfont.lfItalic = false;
	logfont.lfUnderline = false;
	logfont.lfStrikeOut = false;
	logfont.lfCharSet = static_cast<BYTE>(lfCharSet);
	logfont.lfOutPrecision = OUT_STRING_PRECIS;
	logfont.lfClipPrecision = CLIP_STROKE_PRECIS;
	logfont.lfQuality = DRAFT_QUALITY;
	logfont.lfPitchAndFamily = static_cast<BYTE>(lfPitchAndFamily);
	lstrcpyn(logfont.lfFaceName, lfFaceName.c_str(), (sizeof(logfont.lfFaceName)/sizeof(logfont.lfFaceName[0])) );
}

/**
 * @brief Initialize the in-memory Options::Font structure (and possibly the related Registry entries)
 * for both File-Contents and Directory-Tree windows.
 */
void SetDefaults(COptionsMgr *pOptionsMgr)
{
	HDC hDC = GetDC(nullptr);
	const int logPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);

	// *****
	// File-Contents windows use fixed-width fonts.  Here we discover the 
	// correct default fontname from the system code page, if possible.  The 
	// default size is 12 points.  If a fontname cannot be found (why?),   
	// "Courier New" will be used for the fontname.
	LOGFONT fontFile = {0};
	const int pointsFile = 12;

	CodePageInfo cpi = {0};
	IExconverter *pexconv = Exconverter::getInstance();
	if (pexconv==nullptr || !pexconv->getCodePageInfo(GetACP(), &cpi))
	{
		assert(false);	// this should never fail (???)
		cpi.bGDICharset = ANSI_CHARSET;
		cpi.fixedWidthFont = _T("Courier New");
	}
	InitializeLogFont(fontFile, -::MulDiv(pointsFile, logPixelsY, 72), cpi.bGDICharset, FF_MODERN | FIXED_PITCH, cpi.fixedWidthFont);

	
	// *****
	// The Directory-Tree window uses (by default) the program's Menu font and size;  if 
	// that cannot be determined (why?), 9-point "Segoe UI" is used (typical since Win7).
	LOGFONT fontDir = {0};
	const int pointsDir = 9;
	NONCLIENTMETRICS ncm = { 0 };
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0))
	{
		const int lfHeight = -::MulDiv(pointsDir, logPixelsY, 72);
		fontDir = ncm.lfMenuFont;
		if (abs(fontDir.lfHeight) > abs(lfHeight))
			fontDir.lfHeight = lfHeight;
		if (wcscmp(fontDir.lfFaceName, L"Meiryo") == 0 || wcscmp(fontDir.lfFaceName, L"\U000030e1\U000030a4\U000030ea\U000030aa"/* "Meiryo" in Japanese */) == 0)
			wcscpy_s(fontDir.lfFaceName, L"Meiryo UI");
	}
	else
	{	
		// in the case of Windows XP
		InitializeLogFont(fontDir, -::MulDiv(pointsDir, logPixelsY, 72), DEFAULT_CHARSET, FF_DONTCARE, L"Segoe UI");
	}
	
	// *****
	// The values generated above are used as the 'default' Options::Font values.  The 'actual' value 
	// for each option is obtained from the Registry, if the entry exists.  If the Registry entry
	// does not exist, it is built using this given 'default' value and the 'actual' value becomes
	// the same as the 'default'.
	for (int i = 0; i < 2; ++i)
	{
		LOGFONT thisFont = (i == 0 ? fontFile : fontDir);
		String name = (i == 0 ? OPT_FONT_FILECMP : OPT_FONT_DIRCMP);

		pOptionsMgr->InitOption(name + OPT_FONT_USECUSTOM, false);
		pOptionsMgr->InitOption(name + OPT_FONT_POINTSIZE, ::MulDiv(abs(thisFont.lfHeight), 72, logPixelsY));
		pOptionsMgr->InitOption(name + OPT_FONT_HEIGHT, thisFont.lfHeight);
		pOptionsMgr->InitOption(name + OPT_FONT_ESCAPEMENT, thisFont.lfEscapement);
		pOptionsMgr->InitOption(name + OPT_FONT_ORIENTATION, thisFont.lfOrientation);
		pOptionsMgr->InitOption(name + OPT_FONT_WEIGHT, thisFont.lfWeight);
		pOptionsMgr->InitOption(name + OPT_FONT_ITALIC, thisFont.lfItalic != 0);
		pOptionsMgr->InitOption(name + OPT_FONT_UNDERLINE, thisFont.lfUnderline != 0);
		pOptionsMgr->InitOption(name + OPT_FONT_STRIKEOUT, thisFont.lfStrikeOut != 0);
		pOptionsMgr->InitOption(name + OPT_FONT_CHARSET, thisFont.lfCharSet);
		pOptionsMgr->InitOption(name + OPT_FONT_OUTPRECISION, thisFont.lfOutPrecision);
		pOptionsMgr->InitOption(name + OPT_FONT_CLIPPRECISION, thisFont.lfClipPrecision);
		pOptionsMgr->InitOption(name + OPT_FONT_QUALITY, thisFont.lfQuality);
		pOptionsMgr->InitOption(name + OPT_FONT_PITCHANDFAMILY, thisFont.lfPitchAndFamily);
		pOptionsMgr->InitOption(name + OPT_FONT_FACENAME, ucr::toTString(thisFont.lfFaceName));
	}
	ReleaseDC(nullptr, hDC);
}

LOGFONT Load(const COptionsMgr *pOptionsMgr, const String& name)
{
	// Build a new LOGFONT with values from the 'actual' values of the in-memory Options::Font table.
	// The Registry is not accessed.
	LOGFONT lfnew = { 0 };
	HDC hDC = GetDC(nullptr);
	lfnew.lfHeight = -MulDiv(pOptionsMgr->GetInt(name + OPT_FONT_POINTSIZE), GetDeviceCaps(hDC, LOGPIXELSY), 72);
	if (lfnew.lfHeight == 0)
		lfnew.lfHeight = pOptionsMgr->GetInt(name + OPT_FONT_HEIGHT);
	lfnew.lfWidth = 0;
	lfnew.lfEscapement = pOptionsMgr->GetInt(name + OPT_FONT_ESCAPEMENT);
	lfnew.lfOrientation = pOptionsMgr->GetInt(name + OPT_FONT_ORIENTATION);
	lfnew.lfWeight = pOptionsMgr->GetInt(name + OPT_FONT_WEIGHT);
	lfnew.lfItalic = pOptionsMgr->GetBool(name + OPT_FONT_ITALIC);
	lfnew.lfUnderline = pOptionsMgr->GetBool(name + OPT_FONT_UNDERLINE);
	lfnew.lfStrikeOut = pOptionsMgr->GetBool(name + OPT_FONT_STRIKEOUT);
	lfnew.lfCharSet = static_cast<BYTE>(pOptionsMgr->GetInt(name + OPT_FONT_CHARSET));
	lfnew.lfOutPrecision = static_cast<BYTE>(pOptionsMgr->GetInt(name + OPT_FONT_OUTPRECISION));
	lfnew.lfClipPrecision = static_cast<BYTE>(pOptionsMgr->GetInt(name + OPT_FONT_CLIPPRECISION));
	lfnew.lfQuality = static_cast<BYTE>(pOptionsMgr->GetInt(name + OPT_FONT_QUALITY));
	lfnew.lfPitchAndFamily = static_cast<BYTE>(pOptionsMgr->GetInt(name + OPT_FONT_PITCHANDFAMILY));
	lstrcpyn(lfnew.lfFaceName,
		pOptionsMgr->GetString(name + OPT_FONT_FACENAME).c_str(), sizeof(lfnew.lfFaceName)/sizeof(lfnew.lfFaceName[0]));
	ReleaseDC(nullptr, hDC);
	return lfnew;
}

void Save(COptionsMgr *pOptionsMgr, const String& name, const LOGFONT* lf, bool bUseCustom)
{
	// Store LOGFONT values into both the 'actual' value of the in-memory Options::Font table, and 
	// into the appropriate Registry entries.
	HDC hDC = GetDC(nullptr);
	pOptionsMgr->SaveOption(name + OPT_FONT_USECUSTOM, bUseCustom);
	pOptionsMgr->SaveOption(name + OPT_FONT_POINTSIZE, -MulDiv(lf->lfHeight, 72, GetDeviceCaps(hDC, LOGPIXELSY)));
	pOptionsMgr->SaveOption(name + OPT_FONT_HEIGHT, lf->lfHeight);
	pOptionsMgr->SaveOption(name + OPT_FONT_ESCAPEMENT, lf->lfEscapement);
	pOptionsMgr->SaveOption(name + OPT_FONT_ORIENTATION, lf->lfOrientation);
	pOptionsMgr->SaveOption(name + OPT_FONT_WEIGHT, lf->lfWeight);
	pOptionsMgr->SaveOption(name + OPT_FONT_ITALIC, lf->lfItalic != 0);
	pOptionsMgr->SaveOption(name + OPT_FONT_UNDERLINE, lf->lfUnderline != 0);
	pOptionsMgr->SaveOption(name + OPT_FONT_STRIKEOUT, lf->lfStrikeOut != 0);
	pOptionsMgr->SaveOption(name + OPT_FONT_CHARSET, lf->lfCharSet);
	pOptionsMgr->SaveOption(name + OPT_FONT_OUTPRECISION, lf->lfOutPrecision);
	pOptionsMgr->SaveOption(name + OPT_FONT_CLIPPRECISION, lf->lfClipPrecision);
	pOptionsMgr->SaveOption(name + OPT_FONT_QUALITY, lf->lfQuality);
	pOptionsMgr->SaveOption(name + OPT_FONT_PITCHANDFAMILY, (int)lf->lfPitchAndFamily);
	pOptionsMgr->SaveOption(name + OPT_FONT_FACENAME, lf->lfFaceName);
	ReleaseDC(nullptr, hDC);
}

void Reset(COptionsMgr *pOptionsMgr, const String& name)
{
	// Resets the in-memory Options::Font 'actual' values to be original 'default' values.
	// The Registry values are not modified, except to turn off the OPT_FONT_USECUSTOM 
	// Registry entry.
	pOptionsMgr->SaveOption(name + OPT_FONT_USECUSTOM, false);
	pOptionsMgr->Reset(name + OPT_FONT_POINTSIZE);
	pOptionsMgr->Reset(name + OPT_FONT_HEIGHT);
	pOptionsMgr->Reset(name + OPT_FONT_ESCAPEMENT);
	pOptionsMgr->Reset(name + OPT_FONT_ORIENTATION);
	pOptionsMgr->Reset(name + OPT_FONT_WEIGHT);
	pOptionsMgr->Reset(name + OPT_FONT_ITALIC);
	pOptionsMgr->Reset(name + OPT_FONT_UNDERLINE);
	pOptionsMgr->Reset(name + OPT_FONT_STRIKEOUT);
	pOptionsMgr->Reset(name + OPT_FONT_CHARSET);
	pOptionsMgr->Reset(name + OPT_FONT_OUTPRECISION);
	pOptionsMgr->Reset(name + OPT_FONT_CLIPPRECISION);
	pOptionsMgr->Reset(name + OPT_FONT_QUALITY);
	pOptionsMgr->Reset(name + OPT_FONT_PITCHANDFAMILY);
	pOptionsMgr->Reset(name + OPT_FONT_FACENAME);
}

}
}
