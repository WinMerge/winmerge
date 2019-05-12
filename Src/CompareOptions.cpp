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
, m_diffAlgorithm(DIFF_ALGORITHM_DEFAULT)
, m_bIndentHeuristic(true)
{
}

/**
 * @brief Copy constructor.
 */
CompareOptions::CompareOptions(const CompareOptions & options)
: m_ignoreWhitespace(options.m_ignoreWhitespace)
, m_bIgnoreBlankLines(options.m_bIgnoreBlankLines)
, m_bIgnoreCase(options.m_bIgnoreCase)
, m_bIgnoreEOLDifference(options.m_bIgnoreEOLDifference)
, m_diffAlgorithm(options.m_diffAlgorithm)
, m_bIndentHeuristic(options.m_bIndentHeuristic)
{
}

/**
 * @brief Sets options from DiffutilsOptions structure.
 * @param [in] options Diffutils options.
 */
DiffutilsOptions::DiffutilsOptions(const DiffutilsOptions& options)
: CompareOptions(options)
, m_contextLines(options.m_contextLines)
, m_filterCommentsLines(options.m_filterCommentsLines)
, m_outputStyle(options.m_outputStyle)
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
	default:
		throw "Unknown diff algorithm value!";
		break;
	}
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
{
}

/**
 * @brief Sets options from DIFFOPTIONS structure.
 * @param [in] options Diffutils options.
 */
void DiffutilsOptions::SetFromDiffOptions(const DIFFOPTIONS & options)
{
	CompareOptions::SetFromDiffOptions(options);
	m_filterCommentsLines = options.bFilterCommentsLines;
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
}

/**
 * @brief Gets options to DIFFOPTIONS structure.
 * @param [out] options Diffutils options.
 */
void DiffutilsOptions::GetAsDiffOptions(DIFFOPTIONS &options) const
{
	options.bFilterCommentsLines = m_filterCommentsLines;
	options.bIgnoreBlankLines = m_bIgnoreBlankLines;
	options.bIgnoreCase = m_bIgnoreCase;
	options.bIgnoreEol = m_bIgnoreEOLDifference;
	
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
