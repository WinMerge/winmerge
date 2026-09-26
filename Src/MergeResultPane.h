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
	Conflict,   /**< Unresolved 3-way conflict: all three sides differ */
	Edited,     /**< Segment has been edited by hand in the result pane */
};

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
	 * Source panes (0/1/2) of the content for Auto/Chosen, in order.
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
};

