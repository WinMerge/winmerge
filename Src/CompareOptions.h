/** 
 * @file  CompareOptions.h
 *
 * @brief Compare options classes and types.
 */
#pragma once

#include "diff.h"

/**
 * @brief Whether to ignore whitespace (or to ignore changes in whitespace)
 *
 * Examples:
 * "abc def" is only equivalent to "abcdef" under WHITESPACE_IGNORE_ALL
 *
 * but "abc def" is equivalent to "abc    def" under both 
 *   WHITESPACE_IGNORE_CHANGE and WHITESPACE_IGNORE_ALL
 *
 * Also, trailing and leading whitespace is ignored for both
 *   WHITESPACE_IGNORE_CHANGE and WHITESPACE_IGNORE_ALL
 */
enum WhitespaceIgnoreChoices
{
	WHITESPACE_COMPARE_ALL = 0,    /**< no special handling of whitespace */
	WHITESPACE_IGNORE_CHANGE,      /**< ignore changes in whitespace */
	WHITESPACE_IGNORE_ALL,         /**< ignore whitespace altogether */
};

enum DiffAlgorithm
{
	DIFF_ALGORITHM_DEFAULT = 0,
	DIFF_ALGORITHM_MINIMAL = 1,
	DIFF_ALGORITHM_PATIENCE = 2,
	DIFF_ALGORITHM_HISTOGRAM = 3,
};

/**
 * @brief Patch styles.
 *
 * Diffutils can output patch in these formats. Normal format has original
 * and altered lines listed in separate blocks, prefixed with \< and \>
 * characters. Context format has context lines around difference blocks,
 * which are prefixed with - and + characters. Unified format combines
 * difference blocks and prefixes lines with + and - characters.
 * @note We really use only first three types (normal + context formats).
 * Those three types are the ones mostly used and preferred.
 */
enum DiffOutputType
{
	// NOTE: these values are stored in the user's Registry - don't change their value !!
	/**< Default output style.  */
	DIFF_OUTPUT_NORMAL = OUTPUT_NORMAL,
	/**< Output the differences with lines of context before and after (-c).  */
	DIFF_OUTPUT_CONTEXT = OUTPUT_CONTEXT,
	/**< Output the differences in a unified context diff format (-u). */
	DIFF_OUTPUT_UNIFIED = OUTPUT_UNIFIED,
// These are not used, see the comment above enum.
#if 0
	/**< Output the differences as commands suitable for `ed' (-e).  */
	DIFF_OUTPUT_ED = OUTPUT_ED,
	/**< Output the diff as a forward ed script (-f).  */
	DIFF_OUTPUT_FORWARD_ED = OUTPUT_FORWARD_ED,
	/**< Like -f, but output a count of changed lines in each "command" (-n). */
	DIFF_OUTPUT_RCS = OUTPUT_RCS,
	/**< Output merged #ifdef'd file (-D).  */
	DIFF_OUTPUT_IFDEF = OUTPUT_IFDEF,
	/**< Output sdiff style (-y).  */
	DIFF_OUTPUT_SDIFF = OUTPUT_SDIFF,
#endif
//  ... end of unused
	/** Output html style.  */
	DIFF_OUTPUT_HTML = OUTPUT_HTML,
};

/**
 * @brief Diffutils options.
 */
struct DIFFOPTIONS
{
	int nIgnoreWhitespace; /**< Ignore whitespace -option. */
	bool bIgnoreCase; /**< Ignore case -option. */
	bool bIgnoreBlankLines; /**< Ignore blank lines -option. */
	bool bIgnoreEol; /**< Ignore EOL differences -option. */
	bool bFilterCommentsLines; /**< Ignore Multiline comments differences -option. */
	int nDiffAlgorithm; /**< Diff algorithm -option. */
	bool bIndentHeuristic; /**< Ident heuristic -option */
};

/**
 * @brief General compare options.
 * This class has general compare options we expect every compare engine and
 * routine to implement.
 */
class CompareOptions
{
public:
	CompareOptions();
	CompareOptions(const CompareOptions & options);
	virtual void SetFromDiffOptions(const DIFFOPTIONS & options);

	enum WhitespaceIgnoreChoices m_ignoreWhitespace; /**< Ignore whitespace characters */
	bool m_bIgnoreBlankLines; /**< Ignore blank lines (both sides) */
	bool m_bIgnoreCase; /**< Ignore case differences? */
	bool m_bIgnoreEOLDifference; /**< Ignore EOL style differences? */
	enum DiffAlgorithm m_diffAlgorithm; /** Diff algorithm */
	bool m_bIndentHeuristic; /**< Indent heuristic */
};

/**
 * @brief Compare options used with diffutils.
 * This class adds some diffutils-specific compare options to general compare
 * options class. And also methods for easy setting options and forwarding
 * options to diffutils.
 */
class DiffutilsOptions : public CompareOptions
{
public:
	DiffutilsOptions();
	explicit DiffutilsOptions(const CompareOptions& options);
	DiffutilsOptions(const DiffutilsOptions& options);
	void SetToDiffUtils();
	void GetAsDiffOptions(DIFFOPTIONS &options) const;
	virtual void SetFromDiffOptions(const DIFFOPTIONS & options) override;

	enum DiffOutputType m_outputStyle; /**< Output style (for patch files) */
	int m_contextLines; /**< Number of context lines (for patch files) */
	bool m_filterCommentsLines;/**< Ignore Multiline comments differences.*/
};

/**
 * @brief Compare options used with Quick compare -method.
 * This class has some Quick Compare specifics in addition to general compare
 * options.
 */
class QuickCompareOptions : public CompareOptions
{
public:
	QuickCompareOptions();
	explicit QuickCompareOptions(const CompareOptions& options);

	bool m_bStopAfterFirstDiff; /**< Optimize compare by stopping after first difference? */
};
