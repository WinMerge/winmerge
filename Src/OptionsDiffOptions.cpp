/** 
 * @file  OptionsDiffOptions.cpp
 *
 * @brief Implementation for OptionsDiffOptions class.
 */
#include "pch.h"
#include "OptionsDiffOptions.h"
#include "CompareOptions.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

namespace Options { namespace DiffOptions {

/**
 * @brief Set default diff option values.
 */
void Init(COptionsMgr *pOptionsMgr)
{
	pOptionsMgr->InitOption(OPT_CMP_IGNORE_WHITESPACE, (int)0, 0, 2);
	pOptionsMgr->InitOption(OPT_CMP_IGNORE_BLANKLINES, false);
	pOptionsMgr->InitOption(OPT_CMP_FILTER_COMMENTLINES, false);
	pOptionsMgr->InitOption(OPT_CMP_IGNORE_CASE, false);
	pOptionsMgr->InitOption(OPT_CMP_IGNORE_NUMBERS, false);
	pOptionsMgr->InitOption(OPT_CMP_IGNORE_EOL, false);
	pOptionsMgr->InitOption(OPT_CMP_DIFF_ALGORITHM, (int)0, 0, 4);
	pOptionsMgr->InitOption(OPT_CMP_INDENT_HEURISTIC, true);
	pOptionsMgr->InitOption(OPT_CMP_COMPLETELY_BLANK_OUT_IGNORED_CHANGES, false);
	pOptionsMgr->InitOption(OPT_CMP_IGNORE_MISSING_TRAILING_EOL, false);
	pOptionsMgr->InitOption(OPT_CMP_IGNORE_LINE_BREAKS, false);
}

void Load(const COptionsMgr *pOptionsMgr, DIFFOPTIONS& options)
{
	options.nIgnoreWhitespace = pOptionsMgr->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.nDiffAlgorithm = pOptionsMgr->GetInt(OPT_CMP_DIFF_ALGORITHM);
	options.bIgnoreBlankLines = pOptionsMgr->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	options.bFilterCommentsLines = pOptionsMgr->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	options.bIgnoreCase = pOptionsMgr->GetBool(OPT_CMP_IGNORE_CASE);
	options.bIgnoreNumbers = pOptionsMgr->GetBool(OPT_CMP_IGNORE_NUMBERS);
	options.bIgnoreEol = pOptionsMgr->GetBool(OPT_CMP_IGNORE_EOL);
	options.bIgnoreMissingTrailingEol = pOptionsMgr->GetBool(OPT_CMP_IGNORE_MISSING_TRAILING_EOL);
	options.bIgnoreLineBreaks = pOptionsMgr->GetBool(OPT_CMP_IGNORE_LINE_BREAKS);
	options.bIndentHeuristic = pOptionsMgr->GetBool(OPT_CMP_INDENT_HEURISTIC);
	options.bCompletelyBlankOutIgnoredChanges = pOptionsMgr->GetBool(OPT_CMP_COMPLETELY_BLANK_OUT_IGNORED_CHANGES);
}

void Save(COptionsMgr *pOptionsMgr, const DIFFOPTIONS& options)
{
	pOptionsMgr->SaveOption(OPT_CMP_IGNORE_WHITESPACE, options.nIgnoreWhitespace);
	pOptionsMgr->SaveOption(OPT_CMP_DIFF_ALGORITHM, options.nDiffAlgorithm);
	pOptionsMgr->SaveOption(OPT_CMP_IGNORE_BLANKLINES, options.bIgnoreBlankLines);
	pOptionsMgr->SaveOption(OPT_CMP_FILTER_COMMENTLINES, options.bFilterCommentsLines);
	pOptionsMgr->SaveOption(OPT_CMP_IGNORE_CASE, options.bIgnoreCase);
	pOptionsMgr->SaveOption(OPT_CMP_IGNORE_NUMBERS, options.bIgnoreNumbers);
	pOptionsMgr->SaveOption(OPT_CMP_IGNORE_EOL, options.bIgnoreEol);
	pOptionsMgr->SaveOption(OPT_CMP_IGNORE_MISSING_TRAILING_EOL, options.bIgnoreMissingTrailingEol);
	pOptionsMgr->SaveOption(OPT_CMP_IGNORE_LINE_BREAKS, options.bIgnoreLineBreaks);
	pOptionsMgr->SaveOption(OPT_CMP_INDENT_HEURISTIC, options.bIndentHeuristic);
	pOptionsMgr->SaveOption(OPT_CMP_COMPLETELY_BLANK_OUT_IGNORED_CHANGES, options.bCompletelyBlankOutIgnoredChanges);
}

}
}
