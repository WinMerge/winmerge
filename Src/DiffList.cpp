/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DiffList.cpp
 *
 * @brief Implementation file for DiffList class
 */

#include "pch.h"
#include "DiffList.h"
#include <cassert>
#include <string>
#include <sstream>
#include <algorithm>

using std::swap;
using std::vector;

/**
 * @brief Swap diff sides.
 */
void DIFFRANGE::swap_sides(int index1, int index2)
{
	swap(begin[index1], begin[index2]);
	swap(end[index1], end[index2]);
	swap(blank[index1], blank[index2]);
}

/**
 * @brief Initialize DiffMap.
 * @param [in] nlines Lines to add to the list.
 */
void DiffMap::InitDiffMap(int nlines)
{
	// sentry value so we can check later that we set them all
	m_map.assign(nlines, BAD_MAP_ENTRY);
}


/**
 * @brief Default constructor, initialises difflist to 64 items.
 */
DiffList::DiffList()
: m_firstSignificant(-1)
, m_lastSignificant(-1)
, m_firstSignificantLeftMiddle(-1)
, m_firstSignificantLeftRight(-1)
, m_firstSignificantMiddleRight(-1)
, m_firstSignificantLeftOnly(-1)
, m_firstSignificantMiddleOnly(-1)
, m_firstSignificantRightOnly(-1)
, m_firstSignificantConflict(-1)
, m_lastSignificantLeftMiddle(-1)
, m_lastSignificantLeftRight(-1)
, m_lastSignificantMiddleRight(-1)
, m_lastSignificantLeftOnly(-1)
, m_lastSignificantMiddleOnly(-1)
, m_lastSignificantRightOnly(-1)
, m_lastSignificantConflict(-1)
{
	m_diffs.reserve(64); // Reserve some initial space to avoid allocations.
}

/**
 * @brief Removes all diffs from list.
 */
void DiffList::Clear()
{
	m_diffs.clear();
	m_firstSignificant = -1;
	m_lastSignificant = -1;
	m_firstSignificantLeftMiddle = -1;
	m_firstSignificantLeftRight = -1;
	m_firstSignificantMiddleRight = -1;
	m_firstSignificantLeftOnly = -1;
	m_firstSignificantMiddleOnly = -1;
	m_firstSignificantRightOnly = -1;
	m_firstSignificantConflict = -1;
	m_lastSignificantLeftMiddle = -1;
	m_lastSignificantLeftRight = -1;
	m_lastSignificantMiddleRight = -1;
	m_lastSignificantLeftOnly = -1;
	m_lastSignificantMiddleOnly = -1;
	m_lastSignificantRightOnly = -1;
	m_lastSignificantConflict = -1;
}

/**
 * @brief Returns count of significant diffs in the list.
 * This function returns total count of significant diffs in list. So returned
 * count doesn't include non-significant diffs.
 * @return Count of significant differences.
 */
int DiffList::GetSignificantDiffs() const
{
	int nSignificants = 0;
	const int nDiffCount = (int) m_diffs.size();

	for (int i = 0; i < nDiffCount; i++)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		if (dfi->op != OP_TRIVIAL)
		{
			++nSignificants;
		}
	}
	return nSignificants;
}

/**
 * @brief Adds given diff to end of the list.
 * Adds given difference to end of the list (append).
 * @param [in] di Difference to add.
 */
void DiffList::AddDiff(const DIFFRANGE & di)
{
	DiffRangeInfo dri(di);

	// Allocate memory for new items exponentially
	if (m_diffs.size() == m_diffs.capacity())
		m_diffs.reserve(m_diffs.size() * 2);
	m_diffs.push_back(dri);
}

/**
 * @brief Checks if diff in given list index is significant or not.
 * @param [in] nDiff Index of DIFFRANGE to check.
 * @return true if diff is significant, false if not.
 */
bool DiffList::IsDiffSignificant(int nDiff) const
{
	const DIFFRANGE * dfi = DiffRangeAt(nDiff);
	if (dfi->op != OP_TRIVIAL)
		return true;
	else
		return false;
}

/**
 * @brief Get significant difference index of the diff.
 * This function returns the index of diff when only significant differences
 * are calculated.
 * @param [in] nDiff Index of difference to check.
 * @return Significant difference index of the diff.
 */
int DiffList::GetSignificantIndex(int nDiff) const
{
	int significants = -1;

	for (int i = 0; i <= nDiff; i++)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		if (dfi->op != OP_TRIVIAL)
		{
			++significants;
		}
	}
	return significants;
}

/**
 * @brief Returns copy of DIFFRANGE from diff-list.
 * @param [in] nDiff Index of DIFFRANGE to return.
 * @param [out] di DIFFRANGE returned (empty if error)
 * @return true if DIFFRANGE found from given index.
 */
bool DiffList::GetDiff(int nDiff, DIFFRANGE & di) const
{
	const DIFFRANGE * dfi = DiffRangeAt(nDiff);
	if (dfi == nullptr)
	{
		DIFFRANGE empty;
		di = empty;
		return false;
	}
	di = *dfi;
	return true;
}

/**
 * @brief Return constant pointer to requested diff.
 * This function returns constant pointer to DIFFRANGE at given index.
 * @param [in] nDiff Index of DIFFRANGE to return.
 * @return Constant pointer to DIFFRANGE.
 */
const DIFFRANGE * DiffList::DiffRangeAt(int nDiff) const
{
	if (nDiff >= 0 && nDiff < (int) m_diffs.size())
	{
		return &m_diffs[nDiff];
	}
	else
	{
		assert(false);
		return nullptr;
	}
}

/**
 * @brief Replaces diff in list in given index with given diff.
 * @param [in] nDiff Index (0-based) of diff to be replaced
 * @param [in] di Diff to put in list.
 * @return true if index was valid and diff put to list.
 */
bool DiffList::SetDiff(int nDiff, const DIFFRANGE & di)
{
	if (nDiff < (int) m_diffs.size())
	{
		m_diffs[nDiff] = DiffRangeInfo(di);
		return true;
	}
	else
		return false;
}

/**
 * @brief Checks if line is before, inside or after diff
 * @param [in] nLine Linenumber to text buffer (not "real" number)
 * @param [in] nDiff Index to diff table
 * @return -1 if line is before diff, 0 if line is in diff and
 * 1 if line is after diff.
 */
int DiffList::LineRelDiff(int nLine, int nDiff) const
{
	const DIFFRANGE * dfi = DiffRangeAt(nDiff);
	if (static_cast<int>(nLine) < dfi->dbegin)
		return -1;
	else if (static_cast<int>(nLine) > dfi->dend)
		return 1;
	else
		return 0;
}

/**
 * @brief Checks if line is inside given diff
 * @param [in] nLine Linenumber to text buffer (not "real" number)
 * @param [in] nDiff Index to diff table
 * @return true if line is inside given difference.
 */
bool DiffList::LineInDiff(int nLine, int nDiff) const
{
	const DIFFRANGE * dfi = DiffRangeAt(nDiff);
	if (static_cast<int>(nLine) >= dfi->dbegin && static_cast<int>(nLine) <= dfi->dend)
		return true;
	else
		return false;
}

/**
 * @brief Returns diff index for given line.
 * @param [in] nLine Linenumber, 0-based.
 * @return Index to diff table, -1 if line is not inside any diff.
 */
int DiffList::LineToDiff(int nLine) const
{
	const int nDiffCount = static_cast<int>(m_diffs.size());
	if (nDiffCount == 0)
		return -1;

	// First check line is not before first or after last diff
	if (nLine < DiffRangeAt(0)->dbegin)
		return -1;
	if (nLine > DiffRangeAt(nDiffCount-1)->dend)
		return -1;

	// Use binary search to search for a diff.
	int left = 0; // Left limit
	int right = nDiffCount - 1; // Right limit

	while (left <= right)
	{
		int middle = (left + right) / 2; // Compared item
		int result = LineRelDiff(nLine, middle);
		switch (result)
		{
		case -1: // Line is before diff in file
			right = middle - 1;
			break;
		case 0: // Line is in diff
			return middle;
			break;
		case 1: // Line is after diff in file
			left = middle + 1;
			break;
		default:
			{
			std::stringstream s;
			s << "Invalid return value " << result << " from LineRelDiff(): -1, 0 or 1 expected!";
			throw s.str();
			}
		}
	}
	return -1;
}

/**
 * @brief Return previous diff from given line.
 * @param [in] nLine First line searched.
 * @param [out] nDiff Index of diff found.
 * @return true if line is inside diff, false otherwise.
 */
bool DiffList::GetPrevDiff(int nLine, int & nDiff) const
{
	bool bInDiff = true;
	int numDiff = LineToDiff(nLine);

	// Line not inside diff
	if (nDiff == -1)
	{
		bInDiff = false;
		const int size = (int) m_diffs.size();
		for (int i = (int) size - 1; i >= 0 ; i--)
		{
			if ((int)DiffRangeAt(i)->dend <= nLine)
			{
				numDiff = i;
				break;
			}
		}
	}
	nDiff = numDiff;
	return bInDiff;
}

/**
 * @brief Return next difference from given line.
 * This function finds next difference from given line. If line is inside
 * difference, that difference is returned. If next difference is not found
 * param @p nDiff is set to -1.
 * @param [in] nLine First line searched.
 * @param [out] nDiff Index of diff found.
 * @return true if line is inside diff, false otherwise.
 */
bool DiffList::GetNextDiff(int nLine, int & nDiff) const
{
	bool bInDiff = true;
	int numDiff = LineToDiff(nLine);

	// Line not inside diff
	if (numDiff == -1)
	{
		bInDiff = false;
		const int nDiffCount = (int) m_diffs.size();
		for (int i = 0; i < nDiffCount; i++)
		{
			if ((int)DiffRangeAt(i)->dbegin >= nLine)
			{
				numDiff = i;
				break;
			}
		}
	}
	nDiff = numDiff;
	return bInDiff;
}

/**
 * @brief Return previous diff index from given line.
 * @param [in] nLine First line searched.
 * @return Index for next difference or -1 if no difference is found.
 */
int DiffList::PrevSignificantDiffFromLine(int nLine) const
{
	int nDiff = -1;
	const int size = (int) m_diffs.size();

	for (int i = size - 1; i >= 0 ; i--)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		if (dfi->op != OP_TRIVIAL && dfi->dend <= static_cast<int>(nLine))
		{
			nDiff = i;
			break;
		}
	}
	return nDiff;
}

/**
 * @brief Return next diff index from given line.
 * @param [in] nLine First line searched.
 * @return Index for previous difference or -1 if no difference is found.
 */
int DiffList::NextSignificantDiffFromLine(int nLine) const
{
	int nDiff = -1;
	const int nDiffCount = static_cast<int>(m_diffs.size());

	for (int i = 0; i < nDiffCount; i++)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		if (dfi->op != OP_TRIVIAL && dfi->dbegin >= static_cast<int>(nLine))
		{
			nDiff = i;
			break;
		}
	}
	return nDiff;
}

/**
 * @brief Construct the doubly-linked chain of significant differences
 */
void DiffList::ConstructSignificantChain()
{
	m_firstSignificant = -1;
	m_lastSignificant = -1;
	m_firstSignificantLeftMiddle = -1;
	m_firstSignificantLeftRight = -1;
	m_firstSignificantMiddleRight = -1;
	m_firstSignificantConflict = -1;
	m_lastSignificantLeftMiddle = -1;
	m_lastSignificantLeftRight = -1;
	m_lastSignificantMiddleRight = -1;
	m_lastSignificantConflict = -1;
	ptrdiff_t prev = -1;
	const ptrdiff_t size = (int) m_diffs.size();

	// must be called after diff list is entirely populated
    for (int i = 0; i < size; ++i)
	{
		if (m_diffs[i].op == OP_TRIVIAL)
		{
			m_diffs[i].prev = -1;
			m_diffs[i].next = -1;
		}
		else
		{
			m_diffs[i].prev = prev;
			if (prev != -1)
				m_diffs[prev].next = (size_t) i;
			prev = i;
			if (m_firstSignificant == -1)
				m_firstSignificant = i;
			m_lastSignificant = i;
			if (m_diffs[i].op != OP_TRIVIAL && m_diffs[i].op != OP_3RDONLY)
			{
				if (m_firstSignificantLeftMiddle == -1)
					m_firstSignificantLeftMiddle = i;
				m_lastSignificantLeftMiddle = i;
			}
			if (m_diffs[i].op != OP_TRIVIAL && m_diffs[i].op != OP_2NDONLY)
			{
				if (m_firstSignificantLeftRight == -1)
					m_firstSignificantLeftRight = i;
				m_lastSignificantLeftRight = i;
			}
			if (m_diffs[i].op != OP_TRIVIAL && m_diffs[i].op != OP_1STONLY)
			{
				if (m_firstSignificantMiddleRight == -1)
					m_firstSignificantMiddleRight = i;
				m_lastSignificantMiddleRight = i;
			}
			if (m_diffs[i].op == OP_1STONLY)
			{
				if (m_firstSignificantLeftOnly == -1)
					m_firstSignificantLeftOnly = i;
				m_lastSignificantLeftOnly = i;
			}
			if (m_diffs[i].op == OP_2NDONLY)
			{
				if (m_firstSignificantMiddleOnly == -1)
					m_firstSignificantMiddleOnly = i;
				m_lastSignificantMiddleOnly = i;
			}
			if (m_diffs[i].op == OP_3RDONLY)
			{
				if (m_firstSignificantRightOnly == -1)
					m_firstSignificantRightOnly = i;
				m_lastSignificantRightOnly = i;
			}
			if (m_diffs[i].op == OP_DIFF)
			{
				if (m_firstSignificantConflict == -1)
					m_firstSignificantConflict = i;
				m_lastSignificantConflict = i;
			}
		}
	}
}


/**
 * @brief Return pointer to first significant diff.
 * @return Constant pointer to first significant difference.
 */
const DIFFRANGE * DiffList::FirstSignificantDiffRange() const
{
	if (m_firstSignificant == -1)
		return nullptr;
	return DiffRangeAt(m_firstSignificant);
}

/**
 * @brief Return pointer to last significant diff.
 * @return Constant pointer to last significant difference.
 */
const DIFFRANGE * DiffList::LastSignificantDiffRange() const
{
	if (m_lastSignificant == -1)
		return nullptr;
	return DiffRangeAt(m_lastSignificant);
}

/**
<<<<<<< .mine
 * @brief Return previous diff index from given line.
 * @param [in] nLine First line searched.
 * @return Index for next difference or -1 if no difference is found.
 */
int DiffList::PrevSignificant3wayDiffFromLine(int nLine, int nDiffType) const
{
	for (int i = static_cast<int>(m_diffs.size()) - 1; i >= 0 ; i--)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		switch (nDiffType)
		{
		case THREEWAYDIFFTYPE_LEFTMIDDLE:
			if (dfi->op != OP_TRIVIAL && dfi->op != OP_3RDONLY && dfi->dend <= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_LEFTRIGHT:
			if (dfi->op != OP_TRIVIAL && dfi->op != OP_2NDONLY && dfi->dend <= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_MIDDLERIGHT:
			if (dfi->op != OP_TRIVIAL && dfi->op != OP_1STONLY && dfi->dend <= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_LEFTONLY:
			if (dfi->op == OP_1STONLY && dfi->dend <= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_MIDDLEONLY:
			if (dfi->op == OP_2NDONLY && dfi->dend <= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_RIGHTONLY:
			if (dfi->op == OP_3RDONLY && dfi->dend <= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_CONFLICT:
			if (dfi->op == OP_DIFF && dfi->dend <= nLine)
				return i;
			break;
		}
	}
	return -1;
}

/**
 * @brief Return next diff index from given line.
 * @param [in] nLine First line searched.
 * @return Index for previous difference or -1 if no difference is found.
 */
int DiffList::NextSignificant3wayDiffFromLine(int nLine, int nDiffType) const
{
	const int nDiffCount = static_cast<int>(m_diffs.size());

	for (int i = 0; i < nDiffCount; i++)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		switch (nDiffType)
		{
		case THREEWAYDIFFTYPE_LEFTMIDDLE:
			if (dfi->op != OP_TRIVIAL && dfi->op != OP_3RDONLY && dfi->dbegin >= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_LEFTRIGHT:
			if (dfi->op != OP_TRIVIAL && dfi->op != OP_2NDONLY && dfi->dbegin >= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_MIDDLERIGHT:
			if (dfi->op != OP_TRIVIAL && dfi->op != OP_1STONLY && dfi->dbegin >= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_LEFTONLY:
			if (dfi->op == OP_1STONLY && dfi->dbegin >= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_MIDDLEONLY:
			if (dfi->op == OP_2NDONLY && dfi->dbegin >= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_RIGHTONLY:
			if (dfi->op == OP_3RDONLY && dfi->dbegin >= static_cast<int>(nLine))
				return i;
			break;
		case THREEWAYDIFFTYPE_CONFLICT:
			if (dfi->op == OP_DIFF && dfi->dbegin >= nLine)
				return i;
			break;
		}
	}
	return -1;
}

/**
 * @brief Return index to first significant difference.
 * @return Index of first significant difference.
 */
int DiffList::FirstSignificant3wayDiff(int nDiffType) const
{
	switch (nDiffType)
	{
	case THREEWAYDIFFTYPE_LEFTMIDDLE:
		return m_firstSignificantLeftMiddle;
	case THREEWAYDIFFTYPE_LEFTRIGHT:
		return m_firstSignificantLeftRight;
	case THREEWAYDIFFTYPE_MIDDLERIGHT:
		return m_firstSignificantMiddleRight;
	case THREEWAYDIFFTYPE_LEFTONLY:
		return m_firstSignificantLeftOnly;
	case THREEWAYDIFFTYPE_MIDDLEONLY:
		return m_firstSignificantLeftOnly;
	case THREEWAYDIFFTYPE_RIGHTONLY:
		return m_firstSignificantRightOnly;
	case THREEWAYDIFFTYPE_CONFLICT:
		return m_firstSignificantConflict;
	}
	return -1;
}

/**
 * @brief Return index of next significant diff.
 * @param [in] nDiff Index to start looking for next diff.
 * @return Index of next significant difference.
 */
int DiffList::NextSignificant3wayDiff(int nDiff, int nDiffType) const
{
	while (m_diffs[nDiff].next != -1)
	{
		nDiff = static_cast<int>(m_diffs[nDiff].next);
		switch (nDiffType)
		{
		case THREEWAYDIFFTYPE_LEFTMIDDLE:
			if (m_diffs[nDiff].op != OP_3RDONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_LEFTRIGHT:
			if (m_diffs[nDiff].op != OP_2NDONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_MIDDLERIGHT:
			if (m_diffs[nDiff].op != OP_1STONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_LEFTONLY:
			if (m_diffs[nDiff].op == OP_1STONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_MIDDLEONLY:
			if (m_diffs[nDiff].op == OP_2NDONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_RIGHTONLY:
			if (m_diffs[nDiff].op == OP_3RDONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_CONFLICT:
			if (m_diffs[nDiff].op == OP_DIFF)
				return nDiff;
			break;
		}
	}
	return -1;
}

/**
 * @brief Return index of previous significant diff.
 * @param [in] nDiff Index to start looking for previous diff.
 * @return Index of previous significant difference.
 */
int DiffList::PrevSignificant3wayDiff(int nDiff, int nDiffType) const
{
	while (m_diffs[nDiff].prev != -1)
	{
		nDiff = static_cast<int>(m_diffs[nDiff].prev);
		switch (nDiffType)
		{
		case THREEWAYDIFFTYPE_LEFTMIDDLE:
			if (m_diffs[nDiff].op != OP_3RDONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_LEFTRIGHT:
			if (m_diffs[nDiff].op != OP_2NDONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_MIDDLERIGHT:
			if (m_diffs[nDiff].op != OP_1STONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_LEFTONLY:
			if (m_diffs[nDiff].op == OP_1STONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_MIDDLEONLY:
			if (m_diffs[nDiff].op == OP_2NDONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_RIGHTONLY:
			if (m_diffs[nDiff].op == OP_3RDONLY)
				return nDiff;
			break;
		case THREEWAYDIFFTYPE_CONFLICT:
			if (m_diffs[nDiff].op == OP_DIFF)
				return nDiff;
			break;
		}
	}
	return -1;
}

/**
 * @brief Return index to last significant diff.
 * @return Index of last significant difference.
 */
int DiffList::LastSignificant3wayDiff(int nDiffType) const
{
	switch (nDiffType)
	{
	case THREEWAYDIFFTYPE_LEFTMIDDLE:
		return m_lastSignificantLeftMiddle;
	case THREEWAYDIFFTYPE_LEFTRIGHT:
		return m_lastSignificantLeftRight;
	case THREEWAYDIFFTYPE_MIDDLERIGHT:
		return m_lastSignificantMiddleRight;
	case THREEWAYDIFFTYPE_LEFTONLY:
		return m_lastSignificantLeftOnly;
	case THREEWAYDIFFTYPE_MIDDLEONLY:
		return m_lastSignificantLeftOnly;
	case THREEWAYDIFFTYPE_RIGHTONLY:
		return m_lastSignificantRightOnly;
	case THREEWAYDIFFTYPE_CONFLICT:
		return m_lastSignificantRightOnly;
	}
	return -1;
}

/**
 * @brief Return pointer to first significant diff.
 * @return Constant pointer to first significant difference.
 */
const DIFFRANGE * DiffList::FirstSignificant3wayDiffRange(int nDiffType) const
{
	switch (nDiffType)
	{
	case THREEWAYDIFFTYPE_LEFTMIDDLE:
		if (m_firstSignificantLeftMiddle == -1) return nullptr;
		return DiffRangeAt(m_firstSignificantLeftMiddle);
	case THREEWAYDIFFTYPE_LEFTRIGHT:
		if (m_firstSignificantLeftRight == -1) return nullptr;
		return DiffRangeAt(m_firstSignificantLeftRight);
	case THREEWAYDIFFTYPE_MIDDLERIGHT:
		if (m_firstSignificantMiddleRight == -1) return nullptr;
		return DiffRangeAt(m_firstSignificantMiddleRight);
	case THREEWAYDIFFTYPE_LEFTONLY:
		if (m_firstSignificantLeftOnly == -1) return nullptr;
		return DiffRangeAt(m_firstSignificantLeftOnly);
	case THREEWAYDIFFTYPE_MIDDLEONLY:
		if (m_firstSignificantMiddleOnly == -1) return nullptr;
		return DiffRangeAt(m_firstSignificantMiddleOnly);
	case THREEWAYDIFFTYPE_RIGHTONLY:
		if (m_firstSignificantRightOnly == -1) return nullptr;
		return DiffRangeAt(m_firstSignificantRightOnly);
	case THREEWAYDIFFTYPE_CONFLICT:
		if (m_firstSignificantConflict == -1) return nullptr;
		return DiffRangeAt(m_firstSignificantConflict);
	}
	return nullptr;
}

/**
 * @brief Return pointer to last significant diff.
 * @return Constant pointer to last significant difference.
 */
const DIFFRANGE * DiffList::LastSignificant3wayDiffRange(int nDiffType) const
{
	switch (nDiffType)
	{
	case THREEWAYDIFFTYPE_LEFTMIDDLE:
		if (m_lastSignificantLeftMiddle == -1) return nullptr;
		return DiffRangeAt(m_lastSignificantLeftMiddle);
	case THREEWAYDIFFTYPE_LEFTRIGHT:
		if (m_lastSignificantLeftRight == -1) return nullptr;
		return DiffRangeAt(m_lastSignificantLeftRight);
	case THREEWAYDIFFTYPE_MIDDLERIGHT:
		if (m_lastSignificantMiddleRight == -1) return nullptr;
		return DiffRangeAt(m_lastSignificantMiddleRight);
	case THREEWAYDIFFTYPE_LEFTONLY:
		if (m_lastSignificantLeftOnly == -1) return nullptr;
		return DiffRangeAt(m_lastSignificantLeftOnly);
	case THREEWAYDIFFTYPE_MIDDLEONLY:
		if (m_lastSignificantMiddleOnly == -1) return nullptr;
		return DiffRangeAt(m_lastSignificantMiddleOnly);
	case THREEWAYDIFFTYPE_RIGHTONLY:
		if (m_lastSignificantRightOnly == -1) return nullptr;
		return DiffRangeAt(m_lastSignificantRightOnly);
	case THREEWAYDIFFTYPE_CONFLICT:
		if (m_lastSignificantConflict == -1) return nullptr;
		return DiffRangeAt(m_lastSignificantConflict);
	}
	return nullptr;
}

/**
 * @brief Swap sides of diffrange
 */
void DiffList::Swap(int index1, int index2)
{
	vector<DiffRangeInfo>::iterator iter = m_diffs.begin();
	vector<DiffRangeInfo>::const_iterator iterEnd = m_diffs.end();
	while (iter != iterEnd)
	{
		(*iter).swap_sides(index1, index2);
		++iter;
	}
}

/**
 * @brief Count number of lines to add to sides (because of synch).
 * @param [out] nLeftLines Number of lines to add to left side.
 * @param [out] nRightLines Number of lines to add to right side.
 */
void DiffList::GetExtraLinesCounts(int nFiles, int extras[3])
{
	extras[0]=0;
	extras[1]=0;
	extras[2]=0;
	const int nDiffCount = GetSize();

	for (int nDiff = 0; nDiff < nDiffCount; ++nDiff)
	{
		DIFFRANGE curDiff;
		GetDiff(nDiff, curDiff);

		// this guarantees that all the diffs are synchronized
		assert(curDiff.begin[0]+extras[0] == curDiff.begin[1]+extras[1]);
		assert(nFiles<3 || curDiff.begin[0]+extras[0] == curDiff.begin[2]+extras[2]);
		int nline[3] = { 0,0,0 };
		int nmaxline = 0;
		int file;
		for (file = 0; file < nFiles; file++)
		{
			nline[file] = curDiff.end[file]-curDiff.begin[file]+1;
			nmaxline = std::max(nmaxline, nline[file]);
		}
		for (file = 0; file < nFiles; file++)
			extras[file] += nmaxline - nline[file];
	}
}

void DiffList::AppendDiffList(const DiffList& list, int offset[] /*= nullptr*/, int doffset /*= 0*/)
{
	for (std::vector<DiffRangeInfo>::const_iterator it = list.m_diffs.begin(); it != list.m_diffs.end(); ++it)
	{
		DiffRangeInfo dr = *it;
		for (int file = 0; file < 3; ++file)
		{
			if (offset != nullptr)
			{
				dr.begin[file] += offset[file];
				dr.end[file] += offset[file];
			}
			if (doffset != 0)
				dr.blank[file] += doffset;
		}
		if (doffset != 0)
		{
			dr.dbegin += doffset;
			dr.dend += doffset;
		}
		AddDiff(dr);
	}
}

int DiffList::GetMergeableSrcIndex(int nDiff, int nDestIndex) const
{
	const DIFFRANGE *pdr = DiffRangeAt(nDiff);
	switch (nDestIndex)
	{
	case 0:
	case 2:
		if (pdr->op == OP_2NDONLY)
			return 1;
		return -1;
	case 1:
		if (pdr->op == OP_1STONLY || pdr->op == OP_2NDONLY)
			return 0;
		else if (pdr->op == OP_3RDONLY)
			return 2;
		return -1;
	default:
		return -1;
	}
}
