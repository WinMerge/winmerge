/**
 * @file  MergeResultPane.h
 *
 * @brief Declarations for the kdiff3-style merge result pane support:
 *        result segment model and the specialized result text buffer.
 */
#pragma once

#include "UnicodeString.h"
#include <vector>

class CMergeDoc;

/**
 * @brief Resolution state of one result segment.
 */
enum class ResultSegmentState
{
	Common,     /**< Text outside any difference (taken from middle/base pane) */
	Auto,       /**< Difference resolved automatically (non-conflicting change) */
	Chosen,     /**< Difference resolved by an explicit Choose Left/Middle/Right */
	Unresolved, /**< Difference not resolved yet, but the sides do not conflict */
	Conflict,   /**< Unresolved conflict between compared sides */
	Edited,     /**< Segment has been edited by hand in the result pane */
	Semantic,   /**< Conflict resolved by a semantic merge rule (see MergeResultSegment::semanticRule) */
};

/**
 * @brief Semantic merge rule that resolved a segment (Docs/Developers/SemanticMerge.md, section 4).
 */
enum class SemanticRule
{
	None,
	DuplicateAddition,   /**< R1: both sides added the same definition; kept once */
	TokenMerge,          /**< R2: both sides changed different tokens of the same lines */
	MovedDefinition,     /**< R3: one side moved a definition, the other edited it */
	DefinitionChange,    /**< R4: only one side changed the definition */
	LocalizedChange,     /**< R5: one small change replayed onto a refactored definition */
	CommentedOut,        /**< R6: commented-out definition updated with the other side's edit */
};

/** @brief Short display name of a rule, e.g. "Moved definition". */
String SemanticRuleName(SemanticRule rule);

/**
 * @brief One contiguous run of lines in the merge result buffer.
 *
 * The result buffer is fully covered by segments, ordered by start line.
 * Segments with diffIdx == -1 are common text between differences; other
 * segments correspond 1:1 to entries in the document's diff list.
 */
struct MergeResultSegment
{
	int diffIdx = -1; /**< Index in diff list, or -1 for common text */
	ResultSegmentState state = ResultSegmentState::Common;
	/**
	 * Source panes (0/1/2) of the content for Auto/Chosen, in order;
	 * two-way sessions use panes 0 and 1.
	 * Like KDiff3, several sources may be selected for one difference
	 * (their blocks are concatenated); empty for unresolved conflicts.
	 */
	std::vector<int> srcPanes;
	/**
	 * Line count contributed by each entry of srcPanes, in the same
	 * order; used to attribute individual result lines to their source
	 * pane (margin provenance markers).
	 */
	std::vector<int> srcPaneLines;
	int nStartLine = 0; /**< First line of segment in result buffer */
	int nLines = 0;   /**< Number of lines in segment (can be 0) */
	bool bWhiteSpaceOnly = false; /**< Conflict where the sides differ only in white space */
	/**
	 * The chosen source(s) contribute no lines. The buffer then holds one
	 * display-only "<No Lines>" line (nLines == 1) that is not
	 * editable and is left out when the result is saved.
	 */
	bool bNoSrc = false;
	/**
	 * Buffer revision when this segment's content was last generated
	 * (build or Choose). Lines with a higher revision were edited by
	 * hand afterwards; the margin marks exactly those with 'm'.
	 */
	uint32_t nBaseRevision = 0;
	/**
	 * For Conflict/Unresolved segments: the full conflict section in
	 * version-control format (<<<<<<< ... >>>>>>>). This is what gets
	 * saved; the buffer displays either it or a compact placeholder.
	 */
	String blockText;
	int nBlockLines = 0; /**< Line count of blockText */
	SemanticRule semanticRule = SemanticRule::None; /**< Rule that resolved a Semantic segment */
	String semanticReason; /**< One-line explanation of a Semantic resolution */
	int semanticId = 0; /**< Resolution the segment belongs to; one resolution can cover several segments */
};

/**
 * @brief Linked segments as they were before a semantic rule ran;
 * the segments that differ afterwards are the ones the rule resolved.
 */
struct SemanticResolutionSnapshot
{
	struct Entry
	{
		int diffIdx = -1;
		ResultSegmentState state = ResultSegmentState::Common;
		std::vector<int> srcPanes;
		int nLines = 0;
	};
	uint32_t nRevision = 0; /**< Result buffer revision before the rule */
	std::vector<Entry> entries;
};

/** @brief Semantic resolutions per rule, for the Auto Merge report. */
struct SemanticResolutionCounts
{
	int counts[static_cast<int>(SemanticRule::CommentedOut) + 1] = {};
	int Total() const
	{
		int nTotal = 0;
		for (int n : counts)
			nTotal += n;
		return nTotal;
	}
};

