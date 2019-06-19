/** 
 * @file  OptionsDirColors.cpp
 *
 * @brief Implementation for OptionsDirColors class.
 */
#include "pch.h"
#include "OptionsDirColors.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

namespace Options { namespace DirColors {

void SetDefaults(COptionsMgr *pOptionsMgr)
{
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_DIFF, (int)RGB(239,203,5));
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_DIFF_TEXT, (int)RGB(0,0,0));
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_NOT_EXIST_ALL, (int)RGB(192, 192, 192));
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_NOT_EXIST_ALL_TEXT, (int)RGB(0,0,0));
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_FILTERED, (int)RGB(251,242,191));
	pOptionsMgr->InitOption(OPT_DIRCLR_ITEM_FILTERED_TEXT, (int)RGB(0,0,0));
}

void Load(const COptionsMgr *pOptionsMgr, DIRCOLORSETTINGS& colors)
{
	colors.clrDirItemDiff = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_DIFF);
	colors.clrDirItemDiffText = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_DIFF_TEXT);
	colors.clrDirItemNotExistAll = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_NOT_EXIST_ALL);
	colors.clrDirItemNotExistAllText = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_NOT_EXIST_ALL_TEXT);
	colors.clrDirItemFiltered = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_FILTERED);
	colors.clrDirItemFilteredText = pOptionsMgr->GetInt(OPT_DIRCLR_ITEM_FILTERED_TEXT);
}

void Save(COptionsMgr *pOptionsMgr, const DIRCOLORSETTINGS& colors)
{
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_DIFF, (int)colors.clrDirItemDiff);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_DIFF_TEXT, (int)colors.clrDirItemDiffText);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_NOT_EXIST_ALL, (int)colors.clrDirItemNotExistAll);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_NOT_EXIST_ALL_TEXT, (int)colors.clrDirItemNotExistAllText);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_FILTERED, (int)colors.clrDirItemFiltered);
	pOptionsMgr->SaveOption(OPT_DIRCLR_ITEM_FILTERED_TEXT, (int)colors.clrDirItemFilteredText);
}

}}
