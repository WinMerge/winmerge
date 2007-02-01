/** 
 * @file  CompareOptions.h
 *
 * @brief Compare options classes and types.
 */

#ifndef CompareOptions_h_included
#define CompareOptions_h_included

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
	/**< Default output style.  */
	DIFF_OUTPUT_NORMAL,
	/**< Output the differences with lines of context before and after (-c).  */
	DIFF_OUTPUT_CONTEXT,
	/**< Output the differences in a unified context diff format (-u). */
	DIFF_OUTPUT_UNIFIED,

// These are not used, see the comment above enum.
#if 0
	/**< Output the differences as commands suitable for `ed' (-e).  */
	DIFF_OUTPUT_ED,
	/**< Output the diff as a forward ed script (-f).  */
	DIFF_OUTPUT_FORWARD_ED,
	/**< Like -f, but output a count of changed lines in each "command" (-n). */
	DIFF_OUTPUT_RCS,
	/**< Output merged #ifdef'd file (-D).  */
	DIFF_OUTPUT_IFDEF,
	/**< Output sdiff style (-y).  */
	DIFF_OUTPUT_SDIFF
#endif
};

/**
 * @brief Diffutils options.
 */
struct DIFFOPTIONS
{
	int nIgnoreWhitespace; /**< Ignore whitespace -option. */
	BOOL bIgnoreCase; /**< Ignore case -option. */
	BOOL bIgnoreBlankLines; /**< Ignore blank lines -option. */
	BOOL bIgnoreEol; /**< Ignore EOL differences -option. */
	BOOL bFilterCommentsLines; /**< Ignore Multiline comments differences -option. */
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

	enum WhitespaceIgnoreChoices m_ignoreWhitespace; /**< Ignore whitespace characters */
	bool m_bIgnoreBlankLines; /**< Ignore blank lines (both sides) */
	bool m_bIgnoreCase; /**< Ignore case differences? */
	bool m_bIgnoreEOLDifference; /**< Ignore EOL style differences? */
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
	void SetFromDiffOptions(const DIFFOPTIONS & options);
	void SetToDiffUtils();
	void GetAsDiffOptions(DIFFOPTIONS &options);

	enum DiffOutputType m_outputStyle; /**< Output style (for patch files) */
	int m_contextLines; /**< Number of context lines (for patch files) */
	int m_filterCommentsLines;/**< Ignore Multiline comments differences.*/
};

#endif // CompareOptions_h_included
