/** 
 * @file  OptionsDiffOptions.cpp
 *
 * @brief Implementation for OptionsDiffOptions class.
 */
#include "OptionsDiffOptions.h"
#include "CompareOptions.h"
#include "unicoder.h"
#include "MergeApp.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

namespace Options { namespace DiffOptions {

/**
 * @brief Set default diff option values.
 */
void SetDefaults()
{
	COptionsMgr *pOptionsMgr = GetOptionsMgr();
	pOptionsMgr->InitOption(OPT_CMP_IGNORE_WHITESPACE, (int)0);
	pOptionsMgr->InitOption(OPT_CMP_IGNORE_BLANKLINES, false);
	pOptionsMgr->InitOption(OPT_CMP_FILTER_COMMENTLINES, false);
	pOptionsMgr->InitOption(OPT_CMP_IGNORE_CASE, false);
	pOptionsMgr->InitOption(OPT_CMP_IGNORE_EOL, false);
}

void Load(DIFFOPTIONS& options)
{
	COptionsMgr *pOptionsMgr = GetOptionsMgr();
	options.nIgnoreWhitespace = pOptionsMgr->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = pOptionsMgr->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	options.bFilterCommentsLines = pOptionsMgr->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	options.bIgnoreCase = pOptionsMgr->GetBool(OPT_CMP_IGNORE_CASE);
	options.bIgnoreEol = pOptionsMgr->GetBool(OPT_CMP_IGNORE_EOL);
}

void Save(const DIFFOPTIONS& options)
{
	COptionsMgr *pOptionsMgr = GetOptionsMgr();
	pOptionsMgr->SaveOption(OPT_CMP_IGNORE_WHITESPACE, options.nIgnoreWhitespace);
	pOptionsMgr->SaveOption(OPT_CMP_IGNORE_BLANKLINES, options.bIgnoreBlankLines);
	pOptionsMgr->SaveOption(OPT_CMP_FILTER_COMMENTLINES, options.bFilterCommentsLines);
	pOptionsMgr->SaveOption(OPT_CMP_IGNORE_CASE, options.bIgnoreCase);
	pOptionsMgr->SaveOption(OPT_CMP_IGNORE_EOL, options.bIgnoreEol);
}

}
}
