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

template <int npanes>
static void validateVirtualLineToRealLineMap(
	const std::vector<std::array<int, npanes>>& vlines,
	const std::array<int, npanes>& nlines)
{
	int line[npanes]{};
	for (const auto& v : vlines)
	{
		for (int pane = 0; pane < npanes; ++pane)
		{
			if (v[pane] != DiffMap::GHOST_MAP_ENTRY)
			{
				assert(line[pane] == v[pane]);
				++line[pane];
			}
		}
	}
	for (int pane = 0; pane < npanes; ++pane)
		assert(line[pane] == nlines[pane]);
}

/**
 * @brief Create map from virtual line to real line.
 */
static std::vector<std::array<int, 2>> CreateVirtualLineToRealLineMap(
	const DiffMap& diffmap, int nlines0, int nlines1)
{
	std::vector<std::array<int, 2>> vlines;
	int line0 = 0, line1 = 0;
	while (line0 < nlines0)
	{
		const int map_line0 = diffmap.m_map[line0];
		if (map_line0 == DiffMap::GHOST_MAP_ENTRY || map_line0 == DiffMap::BAD_MAP_ENTRY)
		{
			vlines.push_back({ line0++, DiffMap::GHOST_MAP_ENTRY });
		}
		else
		{
			while (line1 < map_line0)
				vlines.push_back({ DiffMap::GHOST_MAP_ENTRY, line1++ });
			vlines.push_back({ line0++, line1++ });
		}
	}
	while (line1 < nlines1)
		vlines.push_back({ DiffMap::GHOST_MAP_ENTRY, line1++ });
	validateVirtualLineToRealLineMap(vlines, std::array<int, 2>{ nlines0, nlines1 });
	return vlines;
}

/**
 * @brief Create map from virtual line to real line. (3-way)
 */
static std::vector<std::array<int, 3>> CreateVirtualLineToRealLineMap3way(
	const DiffMap& diffmap01, const DiffMap& diffmap12, const DiffMap& diffmap20,
	int nlines0, int nlines1, int nlines2)
{
	std::vector<std::array<int, 2>> vlines01 = CreateVirtualLineToRealLineMap(diffmap01, nlines0, nlines1);
	std::vector<std::array<int, 2>> vlines12 = CreateVirtualLineToRealLineMap(diffmap12, nlines1, nlines2);
	std::vector<std::array<int, 2>> vlines20 = CreateVirtualLineToRealLineMap(diffmap20, nlines2, nlines0);
	std::vector<std::array<int, 3>> vlines;
	size_t i01 = 0, i12 = 0, i20 = 0;
	int line0 = 0, line1 = 0, line2 = 0;
	bool is_vlines20_usable = true;
	for (line1 = 0; line1 < nlines1; ++line1)
	{
		size_t i01b = i01;
		size_t i12b = i12;
		size_t i20b = i20;
		// 1. 
		for (; i01 < vlines01.size(); ++i01)
			if (vlines01[i01][1] == line1)
				break;
		for (; i12 < vlines12.size(); ++i12)
			if (vlines12[i12][0] == line1)
				break;
		assert(i01 < vlines01.size() && i12 < vlines12.size());
		// 2. 
		bool used_vlines20 = false;
		if (is_vlines20_usable)
		{
			if (vlines12[i12][1] != DiffMap::GHOST_MAP_ENTRY && vlines01[i01][0] != DiffMap::GHOST_MAP_ENTRY)
			{
				line2 = vlines12[i12][1];
				line0 = vlines01[i01][0];
				size_t i20tmp;
				for (i20tmp = i20b; i20tmp < vlines20.size(); ++i20tmp)
					if (vlines20[i20tmp][0] == line2 && vlines20[i20tmp][1] == line0)
						break;
				if (i20tmp < vlines20.size())
				{
					for (; i20 < i20tmp; ++i20)
						vlines.push_back({ vlines20[i20][1], DiffMap::GHOST_MAP_ENTRY, vlines20[i20][0] });
					++i20;
					used_vlines20 = true;
				}
				else
				{
					is_vlines20_usable = false;
				}
			}
			else
			{
				is_vlines20_usable = false;
			}
		}
		if (!used_vlines20)
		{
			size_t i01tmp, i12tmp;
			for (i01tmp = i01b, i12tmp = i12b; i01tmp < i01 && i12tmp < i12; ++i01tmp, ++i12tmp)
				vlines.push_back({ vlines01[i01tmp][0], DiffMap::GHOST_MAP_ENTRY, vlines12[i12tmp][1] });
			if (i01 - i01b < i12 - i12b)
			{
				for (; i12tmp < i12; ++i12tmp)
					vlines.push_back({ DiffMap::GHOST_MAP_ENTRY, DiffMap::GHOST_MAP_ENTRY, vlines12[i12tmp][1] });
			}
			else if (i01 - i01b > i12 - i12b)
			{
				for (; i01tmp < i01; ++i01tmp)
					vlines.push_back({ vlines01[i01tmp][0], DiffMap::GHOST_MAP_ENTRY, DiffMap::GHOST_MAP_ENTRY });
			}
		}
		vlines.push_back({ vlines01[i01++][0], line1, vlines12[i12++][1]});
	}
	// 3. 
	if (i01 < vlines01.size() || i12 < vlines12.size())
	{
		if (is_vlines20_usable)
		{
			for (; i20 < vlines20.size(); ++i20)
				vlines.push_back({ vlines20[i20][1], DiffMap::GHOST_MAP_ENTRY, vlines20[i20][0] });
		}
		else
		{
			for (; i01 < vlines01.size() && i12 < vlines12.size(); ++i01, ++i12)
				vlines.push_back({ vlines01[i01][0], DiffMap::GHOST_MAP_ENTRY, vlines12[i12][1] });
			if (vlines01.size() - i01 < vlines12.size() - i12)
			{
				for (; i12 < vlines12.size(); ++i12)
					vlines.push_back({ DiffMap::GHOST_MAP_ENTRY, DiffMap::GHOST_MAP_ENTRY, vlines12[i12][1] });
			}
			if (vlines01.size() - i01 > vlines12.size() - i12)
			{
				for (; i01 < vlines01.size(); ++i01)
					vlines.push_back({ vlines01[i01][0], DiffMap::GHOST_MAP_ENTRY, DiffMap::GHOST_MAP_ENTRY });
			}
		}
	}
	validateVirtualLineToRealLineMap(vlines, std::array<int, 3>{ nlines0, nlines1, nlines2 });
	return vlines;
}

OP_TYPE CMergeDoc::CalcOp3way(
	const std::vector<std::array<int, 3>>& vlines, size_t index,
	const DIFFRANGE& diffrange, const DIFFOPTIONS& diffOptions)
{
	if (vlines[index][0] != DiffMap::GHOST_MAP_ENTRY &&
	    vlines[index][1] == DiffMap::GHOST_MAP_ENTRY &&
	    vlines[index][2] == DiffMap::GHOST_MAP_ENTRY)
	{
		return OP_1STONLY;
	}
	else if (vlines[index][0] == DiffMap::GHOST_MAP_ENTRY &&
             vlines[index][1] != DiffMap::GHOST_MAP_ENTRY &&
		     vlines[index][2] == DiffMap::GHOST_MAP_ENTRY)
	{
		return OP_2NDONLY;
	}
	else if (vlines[index][0] == DiffMap::GHOST_MAP_ENTRY &&
	         vlines[index][1] == DiffMap::GHOST_MAP_ENTRY &&
	         vlines[index][2] != DiffMap::GHOST_MAP_ENTRY)
	{
		return OP_3RDONLY;
	}

	if (vlines[index][0] != DiffMap::GHOST_MAP_ENTRY &&
	    vlines[index][1] != DiffMap::GHOST_MAP_ENTRY &&
	    vlines[index][2] == DiffMap::GHOST_MAP_ENTRY)
	{
		String strLine0 = m_ptBuf[0]->GetLineChars(diffrange.begin[0] + vlines[index][0]);
		String strLine1 = m_ptBuf[1]->GetLineChars(diffrange.begin[1] + vlines[index][1]);
		if (strdiff::Compare(strLine0, strLine1,
			!diffOptions.bIgnoreCase, !diffOptions.bIgnoreEol,
			diffOptions.nIgnoreWhitespace, diffOptions.bIgnoreNumbers) == 0)
			return OP_3RDONLY;
		return OP_DIFF;
	}
	else if (vlines[index][0] != DiffMap::GHOST_MAP_ENTRY &&
	         vlines[index][1] == DiffMap::GHOST_MAP_ENTRY &&
	         vlines[index][2] != DiffMap::GHOST_MAP_ENTRY)
	{
		String strLine0 = m_ptBuf[0]->GetLineChars(diffrange.begin[0] + vlines[index][0]);
		String strLine2 = m_ptBuf[2]->GetLineChars(diffrange.begin[2] + vlines[index][2]);
		if (strdiff::Compare(strLine0, strLine2,
			!diffOptions.bIgnoreCase, !diffOptions.bIgnoreEol,
			diffOptions.nIgnoreWhitespace, diffOptions.bIgnoreNumbers) == 0)
			return OP_2NDONLY;
		return OP_DIFF;
	}
	else if (vlines[index][0] == DiffMap::GHOST_MAP_ENTRY &&
	         vlines[index][1] != DiffMap::GHOST_MAP_ENTRY &&
	         vlines[index][2] != DiffMap::GHOST_MAP_ENTRY)
	{
		String strLine1 = m_ptBuf[1]->GetLineChars(diffrange.begin[1] + vlines[index][1]);
		String strLine2 = m_ptBuf[2]->GetLineChars(diffrange.begin[2] + vlines[index][2]);
		if (strdiff::Compare(strLine1, strLine2,
			!diffOptions.bIgnoreCase, !diffOptions.bIgnoreEol,
			diffOptions.nIgnoreWhitespace, diffOptions.bIgnoreNumbers) == 0)
			return OP_1STONLY;
		return OP_DIFF;
	}
	else
	{
		String strLine0 = m_ptBuf[0]->GetLineChars(diffrange.begin[0] + vlines[index][0]);
		String strLine1 = m_ptBuf[1]->GetLineChars(diffrange.begin[1] + vlines[index][1]);
		String strLine2 = m_ptBuf[2]->GetLineChars(diffrange.begin[2] + vlines[index][2]);
		if (strdiff::Compare(strLine0, strLine1,
			!diffOptions.bIgnoreCase, !diffOptions.bIgnoreEol,
			diffOptions.nIgnoreWhitespace, diffOptions.bIgnoreNumbers == 0))
			return OP_3RDONLY;
		if (strdiff::Compare(strLine0, strLine2,
			!diffOptions.bIgnoreCase, !diffOptions.bIgnoreEol,
			diffOptions.nIgnoreWhitespace, diffOptions.bIgnoreNumbers) == 0)
			return OP_2NDONLY;
		if (strdiff::Compare(strLine1, strLine2,
			!diffOptions.bIgnoreCase, !diffOptions.bIgnoreEol,
			diffOptions.nIgnoreWhitespace, diffOptions.bIgnoreNumbers) == 0)
			return OP_1STONLY;
		return OP_DIFF;
	}
}

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
			const std::vector<WordDiff> worddiffs = GetWordDiffArrayInRange(diffrange.begin, diffrange.end);
			DiffMap diffmap;
			diffmap.InitDiffMap(nlines0);
			AdjustDiffBlock(diffmap, diffrange, worddiffs, 0, 1, lo0, hi0, lo1, hi1);
			std::vector<std::array<int, 2>> vlines = CreateVirtualLineToRealLineMap(diffmap, nlines0, nlines1);

			// divide diff blocks
			int line0 = 0, line1 = 0;
			for (size_t i = 0; i < vlines.size();)
			{
				DIFFRANGE dr;
				size_t ib = i;
				if ((vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
					(vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY))
				{
					while (i < vlines.size() &&
						(vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
						(vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY))
					{
						line0++;
						line1++;
						i++;
					}
					dr.begin[0]  = diffrange.begin[0] + vlines[ib][0];
					dr.begin[1]  = diffrange.begin[1] + vlines[ib][1];
					dr.end[0]    = diffrange.begin[0] + vlines[i - 1][0];
					dr.end[1]    = diffrange.begin[1] + vlines[i - 1][1];
					dr.blank[0]  = dr.blank[1] = -1;
					dr.op        = diffrange.op;
					newDiffList.AddDiff(dr);
				}
				else if ((vlines[i][0] == DiffMap::GHOST_MAP_ENTRY || vlines[i][0] == DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY))
				{
					while (i < vlines.size() &&
					     (vlines[i][0] == DiffMap::GHOST_MAP_ENTRY || vlines[i][0] == DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY))
					{
						line1++;
						i++;
					}
					dr.begin[0]  = diffrange.begin[0] + line0;
					dr.begin[1]  = diffrange.begin[1] + vlines[ib][1];
					dr.end[0]    = diffrange.begin[0] + line0 - 1;
					dr.end[1]    = diffrange.begin[1] + vlines[i - 1][1];
					dr.blank[0]  = dr.blank[1] = -1;
					dr.op        = diffrange.op;
					newDiffList.AddDiff(dr);
				}
				else if ((vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][1] == DiffMap::GHOST_MAP_ENTRY || vlines[i][1] == DiffMap::BAD_MAP_ENTRY))
				{
					while (i < vlines.size() &&
					     (vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][1] == DiffMap::GHOST_MAP_ENTRY || vlines[i][1] == DiffMap::BAD_MAP_ENTRY))
					{
						line0++;
						i++;
					}
					dr.begin[0]  = diffrange.begin[0] + vlines[ib][0];
					dr.begin[1]  = diffrange.begin[1] + line1;
					dr.end[0]    = diffrange.begin[0] + vlines[i - 1][0];
					dr.end[1]    = diffrange.begin[1] + line1 - 1;
					dr.blank[0]  = dr.blank[1] = -1;
					dr.op        = diffrange.op;
					newDiffList.AddDiff(dr);
				}
				else
				{
					assert(0);
				}
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
 * @brief Divide diff blocks to match lines in diff blocks. (3-way)
 */
void CMergeDoc::AdjustDiffBlocks3way()
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
		int nlines2 = diffrange.end[2] - diffrange.begin[2] + 1;
		if ((nlines0 > 0) + (nlines1 > 0) + (nlines2 > 0) > 1)
		{
			// Call worker to do all lines in block
			int lo0 = 0, hi0 = nlines0 - 1;
			int lo1 = 0, hi1 = nlines1 - 1;
			int lo2 = 0, hi2 = nlines2 - 1;
			const std::vector<WordDiff> worddiffs = GetWordDiffArrayInRange(diffrange.begin, diffrange.end);
			DiffMap diffmap01, diffmap12, diffmap20;
			diffmap01.InitDiffMap(nlines0);
			diffmap12.InitDiffMap(nlines1);
			diffmap20.InitDiffMap(nlines2);
			AdjustDiffBlock(diffmap01, diffrange, worddiffs, 0, 1, lo0, hi0, lo1, hi1);
			AdjustDiffBlock(diffmap12, diffrange, worddiffs, 1, 2, lo1, hi1, lo2, hi2);
			AdjustDiffBlock(diffmap20, diffrange, worddiffs, 2, 0, lo2, hi2, lo0, hi0);
			std::vector<std::array<int, 3>> vlines = CreateVirtualLineToRealLineMap3way(diffmap01, diffmap12, diffmap20, nlines0, nlines1, nlines2);

			DIFFOPTIONS diffOptions = {0};
			m_diffWrapper.GetOptions(&diffOptions);
			std::vector<OP_TYPE> opary(vlines.size());
			for (size_t i = 0; i < vlines.size(); ++i)
				opary[i] = (diffrange.op == OP_TRIVIAL) ?
					OP_TRIVIAL :
					CalcOp3way(vlines, i, diffrange, diffOptions);
			// divide diff blocks
			int line0 = 0, line1 = 0, line2 = 0;
			for (size_t i = 0; i < vlines.size();)
			{
				DIFFRANGE dr;
				size_t ib = i;
				OP_TYPE op = opary[i];
				if ((vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
					(vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY) &&
					(vlines[i][2] != DiffMap::GHOST_MAP_ENTRY && vlines[i][2] != DiffMap::BAD_MAP_ENTRY))
				{
					while (i < vlines.size() &&
						(vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
						(vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY) &&
						(vlines[i][2] != DiffMap::GHOST_MAP_ENTRY && vlines[i][2] != DiffMap::BAD_MAP_ENTRY) &&
						op == opary[i])
					{
						line0++;
						line1++;
						line2++;
						i++;
					}
					dr.begin[0]  = diffrange.begin[0] + vlines[ib][0];
					dr.begin[1]  = diffrange.begin[1] + vlines[ib][1];
					dr.begin[2]  = diffrange.begin[2] + vlines[ib][2];
					dr.end[0]    = diffrange.begin[0] + vlines[i - 1][0];
					dr.end[1]    = diffrange.begin[1] + vlines[i - 1][1];
					dr.end[2]    = diffrange.begin[2] + vlines[i - 1][2];
					dr.blank[0]  = dr.blank[1] = dr.blank[2] = -1;
					dr.op        = op;
					newDiffList.AddDiff(dr);
				}
				else if ((vlines[i][0] == DiffMap::GHOST_MAP_ENTRY || vlines[i][0] == DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][2] != DiffMap::GHOST_MAP_ENTRY && vlines[i][2] != DiffMap::BAD_MAP_ENTRY))
				{
					while (i < vlines.size() &&
					     (vlines[i][0] == DiffMap::GHOST_MAP_ENTRY || vlines[i][0] == DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][2] != DiffMap::GHOST_MAP_ENTRY && vlines[i][2] != DiffMap::BAD_MAP_ENTRY) &&
						 op == opary[i])
					{
						line1++;
						line2++;
						i++;
					}
					dr.begin[0]  = diffrange.begin[0] + line0;
					dr.begin[1]  = diffrange.begin[1] + vlines[ib][1];
					dr.begin[2]  = diffrange.begin[2] + vlines[ib][2];
					dr.end[0]    = diffrange.begin[0] + line0 - 1;
					dr.end[1]    = diffrange.begin[1] + vlines[i - 1][1];
					dr.end[2]    = diffrange.begin[2] + vlines[i - 1][2];
					dr.blank[0]  = dr.blank[1] = dr.blank[2] = -1;
					dr.op        = op;
					newDiffList.AddDiff(dr);
				}
				else if ((vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][1] == DiffMap::GHOST_MAP_ENTRY || vlines[i][1] == DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][2] != DiffMap::GHOST_MAP_ENTRY && vlines[i][2] != DiffMap::BAD_MAP_ENTRY))
				{
					while (i < vlines.size() &&
					     (vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][1] == DiffMap::GHOST_MAP_ENTRY || vlines[i][1] == DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][2] != DiffMap::GHOST_MAP_ENTRY && vlines[i][2] != DiffMap::BAD_MAP_ENTRY) &&
						 op == opary[i])
					{
						line0++;
						line2++;
						i++;
					}
					dr.begin[0]  = diffrange.begin[0] + vlines[ib][0];
					dr.begin[1]  = diffrange.begin[1] + line1;
					dr.begin[2]  = diffrange.begin[2] + vlines[ib][2];
					dr.end[0]    = diffrange.begin[0] + vlines[i - 1][0];
					dr.end[1]    = diffrange.begin[1] + line1 - 1;
					dr.end[2]    = diffrange.begin[2] + vlines[i - 1][2];
					dr.blank[0]  = dr.blank[1] = dr.blank[2] = -1;
					dr.op        = op;
					newDiffList.AddDiff(dr);
				}
				else if ((vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][2] == DiffMap::GHOST_MAP_ENTRY || vlines[i][2] == DiffMap::BAD_MAP_ENTRY))
				{
					while (i < vlines.size() &&
					     (vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][2] == DiffMap::GHOST_MAP_ENTRY || vlines[i][2] == DiffMap::BAD_MAP_ENTRY) &&
						 op == opary[i])
					{
						line0++;
						line1++;
						i++;
					}
					dr.begin[0]  = diffrange.begin[0] + vlines[ib][0];
					dr.begin[1]  = diffrange.begin[1] + vlines[ib][1];
					dr.begin[2]  = diffrange.begin[2] + line2;
					dr.end[0]    = diffrange.begin[0] + vlines[i - 1][0];
					dr.end[1]    = diffrange.begin[1] + vlines[i - 1][1];
					dr.end[2]    = diffrange.begin[2] + line2 - 1;
					dr.blank[0]  = dr.blank[1] = dr.blank[2] = -1;
					dr.op        = op;
					newDiffList.AddDiff(dr);
				}
				else if ((vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][1] == DiffMap::GHOST_MAP_ENTRY || vlines[i][1] == DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][2] == DiffMap::GHOST_MAP_ENTRY || vlines[i][2] == DiffMap::BAD_MAP_ENTRY))
				{
					while (i < vlines.size() &&
					     (vlines[i][0] != DiffMap::GHOST_MAP_ENTRY && vlines[i][0] != DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][1] == DiffMap::GHOST_MAP_ENTRY || vlines[i][1] == DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][2] == DiffMap::GHOST_MAP_ENTRY || vlines[i][2] == DiffMap::BAD_MAP_ENTRY) &&
						 op == opary[i])
					{
						line0++;
						i++;
					}
					dr.begin[0]  = diffrange.begin[0] + vlines[ib][0];
					dr.begin[1]  = diffrange.begin[1] + line1;
					dr.begin[2]  = diffrange.begin[2] + line2;
					dr.end[0]    = diffrange.begin[0] + vlines[i - 1][0];
					dr.end[1]    = diffrange.begin[1] + line1 - 1;
					dr.end[2]    = diffrange.begin[2] + line2 - 1;
					dr.blank[0]  = dr.blank[1] = dr.blank[2] = -1;
					dr.op        = op;
					newDiffList.AddDiff(dr);
				}
				else if ((vlines[i][0] == DiffMap::GHOST_MAP_ENTRY || vlines[i][0] == DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][2] == DiffMap::GHOST_MAP_ENTRY || vlines[i][2] == DiffMap::BAD_MAP_ENTRY))
				{
					while (i < vlines.size() &&
					     (vlines[i][0] == DiffMap::GHOST_MAP_ENTRY || vlines[i][0] == DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][1] != DiffMap::GHOST_MAP_ENTRY && vlines[i][1] != DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][2] == DiffMap::GHOST_MAP_ENTRY || vlines[i][2] == DiffMap::BAD_MAP_ENTRY) &&
						 op == opary[i])
					{
						line1++;
						i++;
					}
					dr.begin[0]  = diffrange.begin[0] + line0;
					dr.begin[1]  = diffrange.begin[1] + vlines[ib][1];
					dr.begin[2]  = diffrange.begin[2] + line2;
					dr.end[0]    = diffrange.begin[0] + line0 - 1;
					dr.end[1]    = diffrange.begin[1] + vlines[i - 1][1];
					dr.end[2]    = diffrange.begin[2] + line2 - 1;
					dr.blank[0]  = dr.blank[1] = dr.blank[2] = -1;
					dr.op        = op;
					newDiffList.AddDiff(dr);
				}
				else if ((vlines[i][0] == DiffMap::GHOST_MAP_ENTRY || vlines[i][0] == DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][1] == DiffMap::GHOST_MAP_ENTRY || vlines[i][1] == DiffMap::BAD_MAP_ENTRY) &&
				         (vlines[i][2] != DiffMap::GHOST_MAP_ENTRY && vlines[i][2] != DiffMap::BAD_MAP_ENTRY))
				{
					while (i < vlines.size() &&
					     (vlines[i][0] == DiffMap::GHOST_MAP_ENTRY || vlines[i][0] == DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][1] == DiffMap::GHOST_MAP_ENTRY || vlines[i][1] == DiffMap::BAD_MAP_ENTRY) &&
					     (vlines[i][2] != DiffMap::GHOST_MAP_ENTRY && vlines[i][2] != DiffMap::BAD_MAP_ENTRY) &&
						 op == opary[i])
					{
						line2++;
						i++;
					}
					dr.begin[0]  = diffrange.begin[0] + line0;
					dr.begin[1]  = diffrange.begin[1] + line1;
					dr.begin[2]  = diffrange.begin[2] + vlines[ib][2];
					dr.end[0]    = diffrange.begin[0] + line0 - 1;
					dr.end[1]    = diffrange.begin[1] + line1 - 1;
					dr.end[2]    = diffrange.begin[2] + vlines[i - 1][2];
					dr.blank[0]  = dr.blank[1] = dr.blank[2] = -1;
					dr.op        = op;
					newDiffList.AddDiff(dr);
				}
				else
				{
					assert(0);
				}
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
int CMergeDoc::GetMatchCost(int i0, int i1, int line0, int line1, const std::vector<WordDiff>& worddiffs)
{
	int matchlen = 0;
	for (size_t i = 0; i < worddiffs.size(); ++i)
	{
		if (i == 0)
		{
			if (line0 == worddiffs[i].beginline[i0] && line1 == worddiffs[i].beginline[i1])
				matchlen += worddiffs[i].begin[i0];
		}
		else
		{
			if (worddiffs[i - 1].endline[i0] <= line0 && worddiffs[i - 1].endline[i1] <= line1 &&
			    line0 <= worddiffs[i].beginline[i0] && line1 <= worddiffs[i].beginline[i1])
			{
				if (worddiffs[i - 1].endline[i0] == worddiffs[i].beginline[i0])
					matchlen += worddiffs[i].begin[i0] - worddiffs[i - 1].end[i0] - 1;
				else
					matchlen += worddiffs[i].begin[i0];
			}
		}
		if (i == worddiffs.size() - 1 ||
			worddiffs[i + 1].beginline[i0] != worddiffs[i].endline[i0] && worddiffs[i + 1].beginline[i1] != worddiffs[i].endline[i1])
		{
			if (worddiffs[i].endline[i0] == line0 && worddiffs[i].endline[i1] == line1)
				matchlen += m_ptBuf[i0]->GetFullLineLength(line0) - worddiffs[i].end[i0] - 1;
		}
	}
	return -matchlen;
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
void CMergeDoc::AdjustDiffBlock(DiffMap & diffMap, const DIFFRANGE & diffrange, const std::vector<WordDiff>& worddiffs, int i0, int i1, int lo0, int hi0, int lo1, int hi1)
{
	// Map & lo & hi numbers are all relative to this block
	// We need to know offsets to find actual line strings from buffer
	int offset0 = diffrange.begin[i0];
	int offset1 = diffrange.begin[i1];

	// # of lines on left and right
	int lines0 = hi0 - lo0 + 1;
	int lines1 = hi1 - lo1 + 1;

	// shortcut special case
	if (lines0 == 0)
	{
		return;
	}
	if (lines1 == 0)
	{
		for (int w=0; w<lines0; ++w)
			diffMap.m_map[w] = DiffMap::GHOST_MAP_ENTRY;
		return;
	}
	if (lines0 == 1 && lines1 == 1)
	{
		diffMap.m_map[lo0] = hi1;
		return;
	}

	// Bail out if range is large
	auto tooManyCharacters = [&](int pane, int start, int end)
	{
		size_t charCount = 0;
		const int charCountMax = 4096;
		for (int i = start; i <= end; ++i)
		{
			charCount += m_ptBuf[pane]->GetLineLength(i);
			if (charCount > charCountMax)
				return true;
		}
		return false;
	};
	if (tooManyCharacters(i0, offset0 + lo0, offset0 + hi0) || tooManyCharacters(i1, offset1 + lo1, offset1 + hi1))
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
	for (int i=lo0; i<=hi0; ++i)
	{
		for (int j=lo1; j<=hi1; ++j)
		{
			int savings = GetMatchCost(i0, i1, offset0 + i, offset1 + j, worddiffs);
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
			AdjustDiffBlock(diffMap, diffrange, worddiffs, i0, i1, lo0, ibest-1, lo1, itarget-1);
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
			AdjustDiffBlock(diffMap, diffrange, worddiffs, i0, i1, ibest + 1, hi0,
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
