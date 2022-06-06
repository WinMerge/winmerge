/** 
 * @file  CompareOptions.cpp
 *
 * @brief Compare options implementation.
 */

#include "pch.h"
#include "CompareOptions.h"
#include "diff.h"

// Global defined in diffutils code
extern DECL_TLS int recursive;

/**
 * @brief Default constructor.
 */
CompareOptions::CompareOptions()
: m_ignoreWhitespace(WHITESPACE_COMPARE_ALL)
, m_bIgnoreBlankLines(false)
, m_bIgnoreCase(false)
, m_bIgnoreEOLDifference(false)
, m_bIgnoreNumbers(false)
{
}

/**
 * @brief Sets options from DIFFOPTIONS structure.
 * @param [in] options Diffutils options.
 */
void CompareOptions::SetFromDiffOptions(const DIFFOPTIONS &options)
{
	switch (options.nIgnoreWhitespace)
	{
	case 0:
		m_ignoreWhitespace = WHITESPACE_COMPARE_ALL;
		break;
	case 1:
		m_ignoreWhitespace = WHITESPACE_IGNORE_CHANGE;
		break;
	case 2:
		m_ignoreWhitespace = WHITESPACE_IGNORE_ALL;
		break;
	default:
		throw "Unknown whitespace ignore value!";
		break;
	}
	m_bIgnoreBlankLines = options.bIgnoreBlankLines;
	m_bIgnoreCase = options.bIgnoreCase;
	m_bIgnoreEOLDifference = options.bIgnoreEol;
	m_bIgnoreNumbers = options.bIgnoreNumbers;
}

/**
 * @brief Default constructor.
 */
QuickCompareOptions::QuickCompareOptions()
: m_bStopAfterFirstDiff(false)
{

}

/**
 * @brief Default constructor.
 */
DiffutilsOptions::DiffutilsOptions()
: m_outputStyle(DIFF_OUTPUT_NORMAL)
, m_contextLines(0)
, m_filterCommentsLines(false)
, m_bCompletelyBlankOutIgnoredDiffereneces(false)
, m_bIndentHeuristic(true)
, m_diffAlgorithm(DIFF_ALGORITHM_DEFAULT)
{
}

/**
 * @brief Constructor cloning CompareOptions.
 * @param [in] options CompareOptions instance to clone.
 */
DiffutilsOptions::DiffutilsOptions(const CompareOptions& options)
: CompareOptions(options)
, m_outputStyle(DIFF_OUTPUT_NORMAL)
, m_contextLines(0)
, m_filterCommentsLines(false)
, m_bCompletelyBlankOutIgnoredDiffereneces(false)
, m_bIndentHeuristic(true)
, m_diffAlgorithm(DIFF_ALGORITHM_DEFAULT)
{
}

/**
 * @brief Sets options from DIFFOPTIONS structure.
 * @param [in] options Diffutils options.
 */
void DiffutilsOptions::SetFromDiffOptions(const DIFFOPTIONS & options)
{
	CompareOptions::SetFromDiffOptions(options);
	m_bCompletelyBlankOutIgnoredDiffereneces = options.bCompletelyBlankOutIgnoredChanges;
	m_filterCommentsLines = options.bFilterCommentsLines;
	m_bIndentHeuristic = options.bIndentHeuristic;
	switch (options.nDiffAlgorithm)
	{
	case 0:
		m_diffAlgorithm = DIFF_ALGORITHM_DEFAULT;
		break;
	case 1:
		m_diffAlgorithm = DIFF_ALGORITHM_MINIMAL;
		break;
	case 2:
		m_diffAlgorithm = DIFF_ALGORITHM_PATIENCE;
		break;
	case 3:
		m_diffAlgorithm = DIFF_ALGORITHM_HISTOGRAM;
		break;
	case 4:
		m_diffAlgorithm = DIFF_ALGORITHM_NONE;
		break;
	default:
		throw "Unknown diff algorithm value!";
		break;
	}
}

/**
 * @brief Set options to diffutils.
 * Diffutils uses options from global variables we must set. Those variables
 * aren't most logically named, and most are just int's, with some magic
 * values and meanings, with fancy combinations? So not easy to setup. This
 * function maps our easier to handle compare options to diffutils globals.
 */
void DiffutilsOptions::SetToDiffUtils()
{
	switch (m_outputStyle)
	{
	case DIFF_OUTPUT_NORMAL:
		output_style = OUTPUT_NORMAL;
		break;
	case DIFF_OUTPUT_CONTEXT:
		output_style = OUTPUT_CONTEXT;
		break;
	case DIFF_OUTPUT_UNIFIED:
		output_style = OUTPUT_UNIFIED;
		break;
	case DIFF_OUTPUT_HTML:
		output_style = OUTPUT_HTML;
		break;
	default:
		throw "Unknown output style!";
		break;
	}

	context = m_contextLines;

	if (m_ignoreWhitespace == WHITESPACE_IGNORE_CHANGE)
		ignore_space_change_flag = 1;
	else
		ignore_space_change_flag = 0;

	if (m_ignoreWhitespace == WHITESPACE_IGNORE_ALL)
		ignore_all_space_flag = 1;
	else
		ignore_all_space_flag = 0;

	if (m_bIgnoreBlankLines)
		ignore_blank_lines_flag = 1;
	else
		ignore_blank_lines_flag = 0;

	if (m_bIgnoreCase)
		ignore_case_flag = 1;
	else
		ignore_case_flag = 0;

	ignore_numbers_flag = m_bIgnoreNumbers ? 1 : 0;

	if (m_bIgnoreEOLDifference)
		ignore_eol_diff = 1;
	else
		ignore_eol_diff = 0;

	if (m_ignoreWhitespace != WHITESPACE_COMPARE_ALL || m_bIgnoreCase ||
			m_bIgnoreBlankLines || m_bIgnoreEOLDifference)
		ignore_some_changes = 1;
	else
		ignore_some_changes = 0;

	if (m_ignoreWhitespace != WHITESPACE_COMPARE_ALL)
		length_varies = 1;
	else
		length_varies = 0;

	// We have no interest changing these values, hard-code them.
	always_text_flag = 0; // diffutils needs to detect binary files
	horizon_lines = 0;
	heuristic = 1;
	recursive = 0;

	no_diff_means_no_output = 0;
	no_details_flag = 0;
	line_end_char = '\n';
	tab_align_flag = 0;
	tab_expand_flag = 0;
	paginate_flag = 0;
	switch_string = NULL;
	file_label[0] = NULL;
	file_label[1] = NULL;
}

/**
 * @brief Gets options to DIFFOPTIONS structure.
 * @param [out] options Diffutils options.
 */
void DiffutilsOptions::GetAsDiffOptions(DIFFOPTIONS &options) const
{
	options.bCompletelyBlankOutIgnoredChanges = m_bCompletelyBlankOutIgnoredDiffereneces;
	options.bFilterCommentsLines = m_filterCommentsLines;
	options.bIgnoreBlankLines = m_bIgnoreBlankLines;
	options.bIgnoreCase = m_bIgnoreCase;
	options.bIgnoreEol = m_bIgnoreEOLDifference;
	options.bIgnoreNumbers = m_bIgnoreNumbers;

	switch (m_ignoreWhitespace)
	{
	case WHITESPACE_COMPARE_ALL:
		options.nIgnoreWhitespace = 0;
		break;
	case WHITESPACE_IGNORE_CHANGE:
		options.nIgnoreWhitespace = 1;
		break;
	case WHITESPACE_IGNORE_ALL:
		options.nIgnoreWhitespace = 2;
		break;
	default:
		throw "Unknown whitespace ignore value!";
		break;
	}
}

QuickCompareOptions::QuickCompareOptions(const CompareOptions& options)
: CompareOptions(options)
, m_bStopAfterFirstDiff(false)
{
}
