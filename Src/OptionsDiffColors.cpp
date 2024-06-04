/** 
 * @file  OptionsDiffColors.cpp
 *
 * @brief Implementation for OptionsDiffColors class.
 */
#include "pch.h"
#include "OptionsDiffColors.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

namespace Options { namespace DiffColors {

void Init(COptionsMgr *pOptionsMgr)
{
	pOptionsMgr->InitOption(OPT_CLR_DIFF, (int)CEColor(239,203,5));
	pOptionsMgr->InitOption(OPT_CLR_DIFF_DELETED, (int)CEColor(192, 192, 192));
	pOptionsMgr->InitOption(OPT_CLR_DIFF_TEXT, (int)CEColor(0,0,0));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_DIFF, (int)CEColor(239,119,116));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_DIFF_DELETED, (int)CEColor(240, 192, 192));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_DIFF_TEXT, (int)CEColor(0,0,0));
	pOptionsMgr->InitOption(OPT_CLR_TRIVIAL_DIFF, (int)CEColor(251,242,191));
	pOptionsMgr->InitOption(OPT_CLR_TRIVIAL_DIFF_DELETED, (int)CEColor(233,233,233));
	pOptionsMgr->InitOption(OPT_CLR_TRIVIAL_DIFF_TEXT, (int)CEColor(0,0,0));
	pOptionsMgr->InitOption(OPT_CLR_MOVEDBLOCK, (int)CEColor(228,155,82));
	pOptionsMgr->InitOption(OPT_CLR_MOVEDBLOCK_DELETED, (int)CEColor(192, 192, 192));
	pOptionsMgr->InitOption(OPT_CLR_MOVEDBLOCK_TEXT, (int)CEColor(0,0,0));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_MOVEDBLOCK, (int)CEColor(248,112,78));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_MOVEDBLOCK_DELETED, (int)CEColor(252, 181, 163));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_MOVEDBLOCK_TEXT, (int)CEColor(0,0,0));
	pOptionsMgr->InitOption(OPT_CLR_SNP, (int)CEColor(251,250,223));
	pOptionsMgr->InitOption(OPT_CLR_SNP_DELETED, (int)CEColor(233, 233, 233));
	pOptionsMgr->InitOption(OPT_CLR_SNP_TEXT, (int)CEColor(0,0,0));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_SNP, (int)CEColor(239,183,180));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_SNP_DELETED, (int)CEColor(240, 224, 224));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_SNP_TEXT, (int)CEColor(0,0,0));
	pOptionsMgr->InitOption(OPT_CLR_WORDDIFF, (int)CEColor(241,226,173));
	pOptionsMgr->InitOption(OPT_CLR_WORDDIFF_DELETED, (int)CEColor(255,170,130));
	pOptionsMgr->InitOption(OPT_CLR_WORDDIFF_TEXT, (int)CEColor(0,0,0));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_WORDDIFF, (int)CEColor(255,160,160));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_WORDDIFF_DELETED, (int)CEColor(200,129,108));
	pOptionsMgr->InitOption(OPT_CLR_SELECTED_WORDDIFF_TEXT, (int)CEColor(0,0,0));
	pOptionsMgr->InitOption(OPT_CLR_DEFAULT_TEXT_COLORING, true);
}

void Load(const COptionsMgr *pOptionsMgr, COLORSETTINGS& colors)
{
	colors.clrDiff = pOptionsMgr->GetInt(OPT_CLR_DIFF);
	colors.clrDiffDeleted = pOptionsMgr->GetInt(OPT_CLR_DIFF_DELETED);
	colors.clrDiffText = pOptionsMgr->GetInt(OPT_CLR_DIFF_TEXT);
	colors.clrSelDiff = pOptionsMgr->GetInt(OPT_CLR_SELECTED_DIFF);
	colors.clrSelDiffDeleted = pOptionsMgr->GetInt(OPT_CLR_SELECTED_DIFF_DELETED);
	colors.clrSelDiffText = pOptionsMgr->GetInt(OPT_CLR_SELECTED_DIFF_TEXT);
	colors.clrTrivial = pOptionsMgr->GetInt(OPT_CLR_TRIVIAL_DIFF);
	colors.clrTrivialDeleted = pOptionsMgr->GetInt(OPT_CLR_TRIVIAL_DIFF_DELETED);
	colors.clrTrivialText = pOptionsMgr->GetInt(OPT_CLR_TRIVIAL_DIFF_TEXT);
	colors.clrMoved = pOptionsMgr->GetInt(OPT_CLR_MOVEDBLOCK);
	colors.clrMovedDeleted = pOptionsMgr->GetInt(OPT_CLR_MOVEDBLOCK_DELETED);
	colors.clrMovedText = pOptionsMgr->GetInt(OPT_CLR_MOVEDBLOCK_TEXT);
	colors.clrSelMoved = pOptionsMgr->GetInt(OPT_CLR_SELECTED_MOVEDBLOCK);
	colors.clrSelMovedDeleted = pOptionsMgr->GetInt(OPT_CLR_SELECTED_MOVEDBLOCK_DELETED);
	colors.clrSelMovedText = pOptionsMgr->GetInt(OPT_CLR_SELECTED_MOVEDBLOCK_TEXT);
	colors.clrSNP = pOptionsMgr->GetInt(OPT_CLR_SNP);
	colors.clrSNPDeleted = pOptionsMgr->GetInt(OPT_CLR_SNP_DELETED);
	colors.clrSNPText = pOptionsMgr->GetInt(OPT_CLR_SNP_TEXT);
	colors.clrSelSNP = pOptionsMgr->GetInt(OPT_CLR_SELECTED_SNP);
	colors.clrSelSNPDeleted = pOptionsMgr->GetInt(OPT_CLR_SELECTED_SNP_DELETED);
	colors.clrSelSNPText = pOptionsMgr->GetInt(OPT_CLR_SELECTED_SNP_TEXT);
	colors.clrWordDiff = pOptionsMgr->GetInt(OPT_CLR_WORDDIFF);
	colors.clrWordDiffDeleted = pOptionsMgr->GetInt(OPT_CLR_WORDDIFF_DELETED);
	colors.clrWordDiffText = pOptionsMgr->GetInt(OPT_CLR_WORDDIFF_TEXT);
	colors.clrSelWordDiff = pOptionsMgr->GetInt(OPT_CLR_SELECTED_WORDDIFF);
	colors.clrSelWordDiffDeleted = pOptionsMgr->GetInt(OPT_CLR_SELECTED_WORDDIFF_DELETED);
	colors.clrSelWordDiffText = pOptionsMgr->GetInt(OPT_CLR_SELECTED_WORDDIFF_TEXT);
}

void Save(COptionsMgr *pOptionsMgr, const COLORSETTINGS& colors)
{
	pOptionsMgr->SaveOption(OPT_CLR_DIFF, (int)colors.clrDiff);
	pOptionsMgr->SaveOption(OPT_CLR_DIFF_DELETED, (int)colors.clrDiffDeleted);
	pOptionsMgr->SaveOption(OPT_CLR_DIFF_TEXT, (int)colors.clrDiffText);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_DIFF, (int)colors.clrSelDiff);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_DIFF_DELETED, (int)colors.clrSelDiffDeleted);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_DIFF_TEXT, (int)colors.clrSelDiffText);
	pOptionsMgr->SaveOption(OPT_CLR_TRIVIAL_DIFF, (int)colors.clrTrivial);
	pOptionsMgr->SaveOption(OPT_CLR_TRIVIAL_DIFF_DELETED, (int)colors.clrTrivialDeleted);
	pOptionsMgr->SaveOption(OPT_CLR_TRIVIAL_DIFF_TEXT, (int)colors.clrTrivialText);
	pOptionsMgr->SaveOption(OPT_CLR_MOVEDBLOCK, (int)colors.clrMoved);
	pOptionsMgr->SaveOption(OPT_CLR_MOVEDBLOCK_DELETED, (int)colors.clrMovedDeleted);
	pOptionsMgr->SaveOption(OPT_CLR_MOVEDBLOCK_TEXT, (int)colors.clrMovedText);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_MOVEDBLOCK, (int)colors.clrSelMoved);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_MOVEDBLOCK_DELETED, (int)colors.clrSelMovedDeleted);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_MOVEDBLOCK_TEXT, (int)colors.clrSelMovedText);
	pOptionsMgr->SaveOption(OPT_CLR_SNP, (int)colors.clrSNP);
	pOptionsMgr->SaveOption(OPT_CLR_SNP_DELETED, (int)colors.clrSNPDeleted);
	pOptionsMgr->SaveOption(OPT_CLR_SNP_TEXT, (int)colors.clrSNPText);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_SNP, (int)colors.clrSelSNP);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_SNP_DELETED, (int)colors.clrSelSNPDeleted);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_SNP_TEXT, (int)colors.clrSelSNPText);
	pOptionsMgr->SaveOption(OPT_CLR_WORDDIFF, (int)colors.clrWordDiff);
	pOptionsMgr->SaveOption(OPT_CLR_WORDDIFF_DELETED, (int)colors.clrWordDiffDeleted);
	pOptionsMgr->SaveOption(OPT_CLR_WORDDIFF_TEXT, (int)colors.clrWordDiffText);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_WORDDIFF, (int)colors.clrSelWordDiff);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_WORDDIFF_DELETED, (int)colors.clrSelWordDiffDeleted);
	pOptionsMgr->SaveOption(OPT_CLR_SELECTED_WORDDIFF_TEXT, (int)colors.clrSelWordDiffText);
}

}}
