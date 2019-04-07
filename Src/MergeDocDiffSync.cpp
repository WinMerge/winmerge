/** 
 * @file  MergeDocDiffSync.cpp
 *
 * @brief Code to layout diff blocks, to find matching lines, and insert ghost lines
 *
 */

#include "StdAfx.h"
#include <vector>
#include "MergeDoc.h"

#include "Merge.h"
#include "DiffList.h"
#include "stringdiffs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using std::vector;

/**
 * @brief Divide diff blocks to match lines in diff blocks.
 */
void CMergeDoc::AdjustDiffBlocks()
{
	int nDiff;
	int nDiffCount = m_diffList.GetSize();

	// Go through and do our best to line up lines within each diff block
	// between left side and right side
	DiffList newDiffList;
	newDiffList.Clear();
	for (nDiff = 0; nDiff < nDiffCount; nDiff++)
	{
		const DIFFRANGE & diffrange = *m_diffList.DiffRangeAt(nDiff);
		// size map correctly (it will hold one entry for each left-side line
		int nlines0 = diffrange.end[0] - diffrange.begin[0] + 1;
		int nlines1 = diffrange.end[1] - diffrange.begin[1] + 1;
		if (nlines0>0 && nlines1>0)
		{
			// Call worker to do all lines in block
			int lo0 = 0, hi0 = nlines0-1;
			int lo1 = 0, hi1 = nlines1-1;
			DiffMap diffmap;
			diffmap.InitDiffMap(nlines0);
			AdjustDiffBlock(diffmap, diffrange, lo0, hi0, lo1, hi1);

			// divide diff blocks
			DIFFRANGE dr;
			int line0, line1, lineend0;
			for (line0 = 0, line1 = 0; line0 < nlines0;)
			{
				const int map_line0 = diffmap.m_map[line0];
				if (map_line0 == DiffMap::GHOST_MAP_ENTRY ||
						map_line0 == DiffMap::BAD_MAP_ENTRY)
				{
					for (lineend0 = line0; lineend0 < nlines0; lineend0++)
					{
						const int map_lineend0 = diffmap.m_map[lineend0];
						if (map_lineend0 != DiffMap::GHOST_MAP_ENTRY &&
								map_lineend0 != DiffMap::BAD_MAP_ENTRY)
							break;
					}
					dr.begin[0]  = diffrange.begin[0] + line0;
					dr.begin[1]  = diffrange.begin[1] + line1;
					dr.end[0]    = diffrange.begin[0] + lineend0 - 1;
					dr.end[1]    = dr.begin[1] - 1;
					dr.blank[0]  = dr.blank[1] = -1;
					dr.op        = OP_DIFF;
					newDiffList.AddDiff(dr);
					line0 = lineend0;
				}
				else
				{
					if (map_line0 > line1)
					{
						dr.begin[0]  = diffrange.begin[0] + line0;
						dr.begin[1]  = diffrange.begin[1] + line1;
						dr.end[0]    = dr.begin[0] - 1;
						dr.end[1]    = diffrange.begin[1] + map_line0 - 1;
						dr.blank[0]  = dr.blank[1] = -1;
						dr.op        = OP_DIFF;
						newDiffList.AddDiff(dr);
						line1 = map_line0;
					} 

					for (lineend0 = line0 + 1; lineend0 < nlines0; lineend0++)
					{
						if (map_line0 != diffmap.m_map[lineend0 - 1] + 1)
							break;
					}
					dr.begin[0]  = diffrange.begin[0] + line0;
					dr.begin[1]  = diffrange.begin[1] + line1;
					dr.end[0]    = diffrange.begin[0] + lineend0 - 1;
					dr.end[1]    = diffrange.begin[1] + diffmap.m_map[lineend0 - 1];
					dr.blank[0]  = dr.blank[1] = -1;
					dr.op        = diffrange.op == OP_TRIVIAL ? OP_TRIVIAL : OP_DIFF;
					newDiffList.AddDiff(dr);
					line0 = lineend0;
					line1 = diffmap.m_map[lineend0 - 1] + 1;
				}
			}
			if (line1 <= hi1)
			{
				dr.begin[0]  = diffrange.begin[0] + line0;
				dr.begin[1]  = diffrange.begin[1] + line1;
				dr.end[0]    = dr.begin[0] - 1;
				dr.end[1]    = diffrange.begin[1] + hi1;
				dr.blank[0]  = dr.blank[1] = -1;
				dr.op        = diffrange.op == OP_TRIVIAL ? OP_TRIVIAL : OP_DIFF;
				newDiffList.AddDiff(dr);
			}
		}
		else
		{
			newDiffList.AddDiff(diffrange);
		}
	}

	// recreate m_diffList
	m_diffList.Clear();
	nDiffCount = newDiffList.GetSize();
	for (nDiff = 0; nDiff < nDiffCount; nDiff++)
		m_diffList.AddDiff(*newDiffList.DiffRangeAt(nDiff));
}

/**
 * @brief Return cost of making strings equal
 *
 * The cost of making them equal is the measure of their dissimilarity
 * which is their Levenshtein distance.
 */
int CMergeDoc::GetMatchCost(const String &sLine0, const String &sLine1)
{
	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);

	String str[2];
	str[0] = sLine0;
	str[1] = sLine1;

	// Options that affect comparison
	bool casitive = !diffOptions.bIgnoreCase;
	int xwhite = diffOptions.nIgnoreWhitespace;
	int breakType = GetBreakType(); // whitespace only or include punctuation
	bool byteColoring = GetByteColoringOption();

	std::vector<strdiff::wdiff> worddiffs = strdiff::ComputeWordDiffs(2, str, casitive, xwhite, breakType, byteColoring);

	int nDiffLenSum = 0;
	for (std::vector<strdiff::wdiff>::const_iterator it = worddiffs.begin(); it != worddiffs.end(); ++it)
	{
		nDiffLenSum += it->end[0] - it->begin[0] + 1;
	}

	return -static_cast<int>(sLine0.length() - nDiffLenSum);
}

/**
 * @brief Map lines from left to right for specified range in diff block, as best we can
 * Map left side range [lo0;hi0] to right side range [lo1;hi1]
 * (ranges include ends)
 *
 * Algorithm:
 * Find best match, and use that to split problem into two parts (above & below match)
 * and call ourselves recursively to solve each smaller problem
 */
void CMergeDoc::AdjustDiffBlock(DiffMap & diffMap, const DIFFRANGE & diffrange, int lo0, int hi0, int lo1, int hi1)
{
	// Map & lo & hi numbers are all relative to this block
	// We need to know offsets to find actual line strings from buffer
	int offset0 = diffrange.begin[0];
	int offset1 = diffrange.begin[1];

	// # of lines on left and right
	int lines0 = hi0 - lo0 + 1;
	int lines1 = hi1 - lo1 + 1;


	ASSERT(lines0 > 0 && lines1 > 0);

	// shortcut special case
	if (lines0 == 1 && lines1 == 1)
	{
		diffMap.m_map[lo0] = hi1;
		return;
	}

	// Bail out if range is large
	if (lines0 > 15 || lines1 > 15)
	{
		// Do simple 1:1 mapping
		// but try to stay within original block
		// Cannot be outside both sides, so just test for outside one side
		int tlo=0, thi=0;
		if (lo1 < 0)
		{
			// right side is off the bottom
			// so put it as close to bottom as possible
			tlo = hi1 - (hi0-lo0);
			thi = hi1;
		}
		else
		{
			// put it as close to top as possible
			tlo = lo1;
			thi = lo1 + (hi0-lo0);
		}

		for (int w=0; w<lines0; ++w)
		{
			if (w < lines1)
				diffMap.m_map[w] = w + tlo;
			else
				diffMap.m_map[w] = DiffMap::GHOST_MAP_ENTRY;
		}
		return;
	}

	// Find best fit
	int ibest=-1, isavings=0x7fffffff, itarget=-1;
	CString sLine0, sLine1;
	for (int i=lo0; i<=hi0; ++i)
	{
		m_ptBuf[0]->GetLine(offset0 + i, sLine0);
		for (int j=lo1; j<=hi1; ++j)
		{
			m_ptBuf[1]->GetLine(offset1 + j, sLine1);
			int savings = GetMatchCost((LPCTSTR)sLine0, (LPCTSTR)sLine1);
			// TODO
			// Need to penalize assignments that push us outside the box
			// further than is required
			if (savings < isavings)
			{
				ibest = i;
				itarget = j;
				isavings = savings;
			}
		}
	}
	// I think we have to find at least one
	// because cost (Levenshtein distance) is less than or equal to maxlen
	// so cost is always nonnegative
	ASSERT(ibest >= 0);

	ASSERT(diffMap.m_map[ibest] == DiffMap::BAD_MAP_ENTRY);

	diffMap.m_map[ibest] = itarget;

	// Half of the remaining problem is below our match
	if (lo0 < ibest)
	{
		if (lo1 < itarget)
		{
			AdjustDiffBlock(diffMap, diffrange, lo0, ibest-1, lo1, itarget-1);
		}
		else
		{
			// No target space for the ones below our match
			for (int x = lo0; x < ibest; ++x)
			{
				diffMap.m_map[x] = DiffMap::GHOST_MAP_ENTRY;
			}
		}
	}

	// Half of the remaining problem is above our match
	if (hi0 > ibest)
	{
		if (itarget < hi1)
		{
			AdjustDiffBlock(diffMap, diffrange, ibest + 1, hi0,
					itarget + 1, hi1);
		}
		else
		{
			// No target space for the ones above our match
			for (int x = ibest + 1; x <= hi0; ++x)
			{
				diffMap.m_map[x] = DiffMap::GHOST_MAP_ENTRY;
			}
		}
	}
}
