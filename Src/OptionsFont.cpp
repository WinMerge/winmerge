/** 
 * @file  OptionsFont.cpp
 *
 * @brief Implementation for OptionsFont class.
 */
#include "OptionsFont.h"
#include "unicoder.h"
#include "ExConverter.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

namespace Options { namespace Font {

/**
 * @brief Set default font values.
 */
void SetDefaults(COptionsMgr *pOptionsMgr)
{
	CodePageInfo cpi = {0};
	cpi.bGDICharset = ANSI_CHARSET;
	cpi.fixedWidthFont = _T("Courier New");

	IExconverter *pexconv = Exconverter::getInstance();
	if (pexconv)
		pexconv->getCodePageInfo(GetACP(), &cpi);

	HDC hDC = GetDC(NULL);
	for (int i = 0; i < 2; ++i)
	{
		String name = (i == 0 ? OPT_FONT_FILECMP : OPT_FONT_DIRCMP);
		pOptionsMgr->InitOption(name + OPT_FONT_USECUSTOM, false);
		pOptionsMgr->InitOption(name + OPT_FONT_POINTSIZE, 0);
		pOptionsMgr->InitOption(name + OPT_FONT_HEIGHT, -MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72));
		pOptionsMgr->InitOption(name + OPT_FONT_ESCAPEMENT, 0);
		pOptionsMgr->InitOption(name + OPT_FONT_ORIENTATION, 0);
		pOptionsMgr->InitOption(name + OPT_FONT_WEIGHT, FW_NORMAL);
		pOptionsMgr->InitOption(name + OPT_FONT_ITALIC, false);
		pOptionsMgr->InitOption(name + OPT_FONT_UNDERLINE, false);
		pOptionsMgr->InitOption(name + OPT_FONT_STRIKEOUT, false);
		pOptionsMgr->InitOption(name + OPT_FONT_OUTPRECISION, OUT_STRING_PRECIS);
		pOptionsMgr->InitOption(name + OPT_FONT_CLIPPRECISION, CLIP_STROKE_PRECIS);
		pOptionsMgr->InitOption(name + OPT_FONT_QUALITY, DRAFT_QUALITY);
		pOptionsMgr->InitOption(name + OPT_FONT_PITCHANDFAMILY, FF_MODERN | FIXED_PITCH);
		pOptionsMgr->InitOption(name + OPT_FONT_CHARSET, (int) cpi.bGDICharset);
		pOptionsMgr->InitOption(name + OPT_FONT_FACENAME, ucr::toTString(cpi.fixedWidthFont));
	}
	ReleaseDC(NULL, hDC);
}

LOGFONT Load(const COptionsMgr *pOptionsMgr, const String& name)
{
	LOGFONT lfnew = { 0 };
	HDC hDC = GetDC(NULL);
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
	lfnew.lfCharSet = pOptionsMgr->GetInt(name + OPT_FONT_CHARSET);
	lfnew.lfOutPrecision = pOptionsMgr->GetInt(name + OPT_FONT_OUTPRECISION);
	lfnew.lfClipPrecision = pOptionsMgr->GetInt(name + OPT_FONT_CLIPPRECISION);
	lfnew.lfQuality = pOptionsMgr->GetInt(name + OPT_FONT_QUALITY);
	lfnew.lfPitchAndFamily = pOptionsMgr->GetInt(name + OPT_FONT_PITCHANDFAMILY);
	lstrcpyn(lfnew.lfFaceName,
		pOptionsMgr->GetString(name + OPT_FONT_FACENAME).c_str(), sizeof(lfnew.lfFaceName)/sizeof(lfnew.lfFaceName[0]));
	ReleaseDC(NULL, hDC);
	return lfnew;
}

void Save(COptionsMgr *pOptionsMgr, const String& name, const LOGFONT* lf, bool bUseCustom)
{
	HDC hDC = GetDC(NULL);
	pOptionsMgr->SaveOption(name + OPT_FONT_USECUSTOM, bUseCustom);
	pOptionsMgr->SaveOption(name + OPT_FONT_POINTSIZE, -MulDiv(lf->lfHeight, 72, GetDeviceCaps(hDC, LOGPIXELSY)));
	pOptionsMgr->SaveOption(name + OPT_FONT_ESCAPEMENT, lf->lfEscapement);
	pOptionsMgr->SaveOption(name + OPT_FONT_ORIENTATION, lf->lfOrientation);
	pOptionsMgr->SaveOption(name + OPT_FONT_WEIGHT, lf->lfWeight);
	pOptionsMgr->SaveOption(name + OPT_FONT_ITALIC, lf->lfItalic);
	pOptionsMgr->SaveOption(name + OPT_FONT_UNDERLINE, lf->lfUnderline);
	pOptionsMgr->SaveOption(name + OPT_FONT_STRIKEOUT, lf->lfStrikeOut);
	pOptionsMgr->SaveOption(name + OPT_FONT_CHARSET, lf->lfCharSet);
	pOptionsMgr->SaveOption(name + OPT_FONT_OUTPRECISION, lf->lfOutPrecision);
	pOptionsMgr->SaveOption(name + OPT_FONT_CLIPPRECISION, lf->lfClipPrecision);
	pOptionsMgr->SaveOption(name + OPT_FONT_QUALITY, lf->lfQuality);
	pOptionsMgr->SaveOption(name + OPT_FONT_PITCHANDFAMILY, (int)lf->lfPitchAndFamily);
	pOptionsMgr->SaveOption(name + OPT_FONT_FACENAME, lf->lfFaceName);
	ReleaseDC(NULL, hDC);
}

void Reset(COptionsMgr *pOptionsMgr, const String& name)
{
	pOptionsMgr->SaveOption(name + OPT_FONT_USECUSTOM, false);
	pOptionsMgr->Reset(name + OPT_FONT_POINTSIZE);
	pOptionsMgr->Reset(name + OPT_FONT_HEIGHT);
	pOptionsMgr->Reset(name + OPT_FONT_ESCAPEMENT);
	pOptionsMgr->Reset(name + OPT_FONT_ORIENTATION);
	pOptionsMgr->Reset(name + OPT_FONT_WEIGHT);
	pOptionsMgr->Reset(name + OPT_FONT_ITALIC);
	pOptionsMgr->Reset(name + OPT_FONT_UNDERLINE);
	pOptionsMgr->Reset(name + OPT_FONT_STRIKEOUT);
	pOptionsMgr->Reset(name + OPT_FONT_OUTPRECISION);
	pOptionsMgr->Reset(name + OPT_FONT_CLIPPRECISION);
	pOptionsMgr->Reset(name + OPT_FONT_QUALITY);
	pOptionsMgr->Reset(name + OPT_FONT_PITCHANDFAMILY);
	pOptionsMgr->Reset(name + OPT_FONT_CHARSET);
	pOptionsMgr->Reset(name + OPT_FONT_FACENAME);
}

}
}
