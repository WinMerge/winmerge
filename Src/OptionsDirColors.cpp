/** 
 * @file  OptionsDirColors.cpp
 *
 * @brief Implementation for OptionsDirColors class.
 */
#include "pch.h"
#include "OptionsDirColors.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include <Windows.h>


namespace Options { namespace DirColors {

void Init(COptionsMgr *pOptionsMgr)
{
	int defaultTextColor = static_cast<int>(GetSysColor(COLOR_WINDOWTEXT));
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_EQUAL, (int)GetSysColor(COLOR_WINDOW));
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_EQUAL_TEXT, defaultTextColor);
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_DIFF, (int)CEColor(240,222,125));
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_DIFF_TEXT, defaultTextColor);
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_NOT_EXIST_ALL, (int)CEColor(221,221,221));
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_NOT_EXIST_ALL_TEXT, defaultTextColor);
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_FILTERED, (int)CEColor(250,245,215));
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_FILTERED_TEXT, defaultTextColor);
	pOptionsMgr->InitOption(OPT_DIRCLR_MARGIN, (int)GetSysColor(COLOR_WINDOW));
	pOptionsMgr->InitOption(OPT_DIRCLR_USE_COLORS, true);
}

void Load(const COptionsMgr *pOptionsMgr, DIRCOLORSETTINGS& colors)
{
	colors.clrDirItemEqual = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_EQUAL);
	colors.clrDirItemEqualText = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_EQUAL_TEXT);
	colors.clrDirItemDiff = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_DIFF);
	colors.clrDirItemDiffText = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_DIFF_TEXT);
	colors.clrDirItemNotExistAll = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_NOT_EXIST_ALL);
	colors.clrDirItemNotExistAllText = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_NOT_EXIST_ALL_TEXT);
	colors.clrDirItemFiltered = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_FILTERED);
	colors.clrDirItemFilteredText = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_FILTERED_TEXT);
	colors.clrDirMargin = pOptionsMgr->GetInt(OPT_DIRCLR_MARGIN);
}

void Save(COptionsMgr *pOptionsMgr, const DIRCOLORSETTINGS& colors)
{
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_EQUAL, (int)colors.clrDirItemEqual);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_EQUAL_TEXT, (int)colors.clrDirItemEqualText);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_DIFF, (int)colors.clrDirItemDiff);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_DIFF_TEXT, (int)colors.clrDirItemDiffText);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_NOT_EXIST_ALL, (int)colors.clrDirItemNotExistAll);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_NOT_EXIST_ALL_TEXT, (int)colors.clrDirItemNotExistAllText);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_FILTERED, (int)colors.clrDirItemFiltered);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_FILTERED_TEXT, (int)colors.clrDirItemFilteredText);
	pOptionsMgr->SaveOption(OPT_DIRCLR_MARGIN, (int)colors.clrDirMargin);
}

}}
