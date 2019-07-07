/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  DiffList.h
 *
 * @brief Declaration file for DiffList class
 */
#pragma once

#include <vector>
#include <cstring>

/**
 * @brief Operations in diffranges.
 * DIFFRANGE structs op-member can have these values
 */
enum OP_TYPE
{
	OP_NONE = 0,
	OP_1STONLY,
	OP_2NDONLY,
	OP_3RDONLY,
	OP_DIFF,
	OP_TRIVIAL
};

enum
{
	THREEWAYDIFFTYPE_LEFTMIDDLE  = 0,
	THREEWAYDIFFTYPE_LEFTRIGHT,
	THREEWAYDIFFTYPE_MIDDLERIGHT,
	THREEWAYDIFFTYPE_LEFTONLY,
	THREEWAYDIFFTYPE_MIDDLEONLY,
	THREEWAYDIFFTYPE_RIGHTONLY,
	THREEWAYDIFFTYPE_CONFLICT,
};

/**
 * @brief One difference defined by linenumbers.
 *
 * This struct defines one set of different lines "diff".
 * @p begin0, @p end0, @p begin1 & @p end1 are linenumbers
 * in original files. Other struct members point to linenumbers
 * calculated by WinMerge after adding empty lines to make diffs
 * be in line in screen.
 *
 * @note @p blank0 & @p blank1 are -1 if there are no blank lines
 */
struct DIFFRANGE
{
	int begin[3] = {};           /**< First diff line in original file1,2,3 */
	int end[3] = {};             /**< Last diff line in original file1,2,3 */
	int dbegin = 0;              /**< Synchronised (ghost lines added) first diff line in file1,2,3 */
	int dend = 0;	             /**< Synchronised (ghost lines added) last diff line in file1,2,3 */
	int blank[3] = {-1, -1, -1}; /**< Number of blank lines in file1,2,3 */
	OP_TYPE op = OP_NONE;		/**< Operation done with this diff */
	DIFFRANGE()
	{
	}
	void swap_sides(int index1, int index2);
};

/**
 * @brief Relation from left side (0) to right side (1) of a DIFFRANGE
 *
 * Map lines from file1 to file2
 */
class DiffMap
{
public:
	enum
	{
		BAD_MAP_ENTRY = -999999999,
		GHOST_MAP_ENTRY = 888888888
	};

	std::vector<int> m_map;

	// boilerplate ctr, copy ctr
	DiffMap() { }
	DiffMap(const DiffMap & src) : m_map(src.m_map) {}
	void InitDiffMap(int nlines);
};

/**
 * @brief DIFFRANGE with links for chain of non-trivial entries
 *
 * Next and prev are array indices used by the owner (DiffList)
 */
struct DiffRangeInfo: public DIFFRANGE
{
	ptrdiff_t next; /**< link (array index) for doubly-linked chain of non-trivial DIFFRANGEs */
	ptrdiff_t prev; /**< link (array index) for doubly-linked chain of non-trivial DIFFRANGEs */

	DiffRangeInfo() { InitLinks(); }
	explicit DiffRangeInfo(const DIFFRANGE & di) : DIFFRANGE(di) { InitLinks(); }
	void InitLinks() { next = prev = -1; }
};

/**
 * @brief Class for storing differences in files (difflist).
 *
 * This class stores diffs in list and also offers diff-related
 * functions to e.g. check if linenumber is inside diff.
 *
 * There are two kinds of diffs:
 * - significant diffs are 'normal' diffs we want to merge and browse
 * - non-significant diffs are diffs ignored by linefilters
 * 
 * The code assumes diff lists don't grow bigger than 32-bit int type's
 * range. And what a trouble we'd have if we have so many diffs...
 */
class DiffList
{
public:
	DiffList();
	void Clear();
	int GetSize() const;
	int GetSignificantDiffs() const;
	void AddDiff(const DIFFRANGE & di);
	bool IsDiffSignificant(int nDiff) const;
	int GetSignificantIndex(int nDiff) const;
	bool GetDiff(int nDiff, DIFFRANGE & di) const;
	bool SetDiff(int nDiff, const DIFFRANGE & di);
	int LineRelDiff(int nLine, int nDiff) const;
	bool LineInDiff(int nLine, int nDiff) const;
	int LineToDiff(int nLine) const;
	bool GetPrevDiff(int nLine, int & nDiff) const;
	bool GetNextDiff(int nLine, int & nDiff) const;
	bool HasSignificantDiffs() const;
	int PrevSignificantDiffFromLine(int nLine) const;
	int NextSignificantDiffFromLine(int nLine) const;
	int FirstSignificantDiff() const;
	int NextSignificantDiff(int nDiff) const;
	int PrevSignificantDiff(int nDiff) const;
	int LastSignificantDiff() const;
	const DIFFRANGE * FirstSignificantDiffRange() const;
	const DIFFRANGE * LastSignificantDiffRange() const;
	int PrevSignificant3wayDiffFromLine(int nLine, int nDiffType) const;
	int NextSignificant3wayDiffFromLine(int nLine, int nDiffType) const;
	int FirstSignificant3wayDiff(int nDiffType) const;
	int NextSignificant3wayDiff(int nDiff, int nDiffType) const;
	int PrevSignificant3wayDiff(int nDiff, int nDiffType) const;
	int LastSignificant3wayDiff(int nDiffType) const;
	const DIFFRANGE * FirstSignificant3wayDiffRange(int nDiffType) const;
	const DIFFRANGE * LastSignificant3wayDiffRange(int nDiffType) const;
	int GetMergeableSrcIndex(int nDiff, int nDestIndex) const;

	const DIFFRANGE * DiffRangeAt(int nDiff) const;

	void ConstructSignificantChain(); // must be called after diff list is entirely populated
	void Swap(int index1, int index2);
	void GetExtraLinesCounts(int nFiles, int extras[3]);

	std::vector<DiffRangeInfo>& GetDiffRangeInfoVector() { return m_diffs; }

	void AppendDiffList(const DiffList& list, int offset[] = nullptr, int doffset = 0);

private:
	std::vector<DiffRangeInfo> m_diffs; /**< Difference list. */
	int m_firstSignificant; /**< Index of first significant diff in m_diffs */
	int m_lastSignificant; /**< Index of last significant diff in m_diffs */
	int m_firstSignificantLeftMiddle;
	int m_firstSignificantLeftRight;
	int m_firstSignificantMiddleRight;
	int m_firstSignificantLeftOnly;
	int m_firstSignificantMiddleOnly;
	int m_firstSignificantRightOnly;
	int m_firstSignificantConflict;
	int m_lastSignificantLeftMiddle;
	int m_lastSignificantLeftRight;
	int m_lastSignificantMiddleRight;
	int m_lastSignificantLeftOnly;
	int m_lastSignificantMiddleOnly;
	int m_lastSignificantRightOnly;
	int m_lastSignificantConflict;
};

/**
 * @brief Returns count of items in diff list.
 * This function returns total amount of items (diffs) in list. So returned
 * count includes significant and non-significant diffs.
 * @note Use GetSignificantDiffs() to get count of non-ignored diffs.
 */
inline int DiffList::GetSize() const
{
	return (int) m_diffs.size();
}

/**
 * @brief Check if diff-list contains significant diffs.
 * @return true if list has significant diffs, false otherwise.
 */
inline bool DiffList::HasSignificantDiffs() const
{
	return (m_firstSignificant != -1);
}
		
/**
 * @brief Return index to first significant difference.
 * @return Index of first significant difference.
 */
inline int DiffList::FirstSignificantDiff() const
{
	return m_firstSignificant;
}

/**
 * @brief Return index of next significant diff.
 * @param [in] nDiff Index to start looking for next diff.
 * @return Index of next significant difference.
 */
inline int DiffList::NextSignificantDiff(int nDiff) const
{
	return (int)m_diffs[nDiff].next;
}

/**
 * @brief Return index of previous significant diff.
 * @param [in] nDiff Index to start looking for previous diff.
 * @return Index of previous significant difference.
 */
inline int DiffList::PrevSignificantDiff(int nDiff) const
{
	return (int)m_diffs[nDiff].prev;
}

/**
 * @brief Return index to last significant diff.
 * @return Index of last significant difference.
 */
inline int DiffList::LastSignificantDiff() const
{
	return m_lastSignificant;
}
