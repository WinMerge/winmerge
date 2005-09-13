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
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "DiffList.h"
#include "DiffWrapper.h"

/**
 * @brief Default constructor, initialises difflist to 64 items.
 */
DiffList::DiffList()
{
	m_diffs.SetSize(64);
	m_firstSignificant = -1;
	m_lastSignificant = -1;
}

/**
 * @brief Removes all diffs from list.
 */
void DiffList::Clear()
{
	m_diffs.RemoveAll();
	m_firstSignificant = -1;
	m_lastSignificant = -1;
}

/**
 * @brief Returns count of items in diff list.
 * This function returns total amount of items (diffs) in list. So returned
 * count includes significant and non-significant diffs.
 * @note Use GetSignificantDiffs() to get count of non-ignored diffs.
 */
int DiffList::GetSize()
{
	return m_diffs.GetSize();
}

/**
 * @brief Returns count of significant diffs in the list.
 * This function returns total count of significant diffs in list. So returned
 * count doesn't include non-significant diffs.
 */
int DiffList::GetSignificantDiffs() const
{
	int nSignificants = 0;
	const int nDiffCount = m_diffs.GetSize();

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
 * @brief Sets size of diff list.
 */
void DiffList::SetSize(UINT nSize)
{
	m_diffs.SetSize(nSize);
}

/**
 * @brief Adds given diff to end of list.
 */
void DiffList::AddDiff(DIFFRANGE di)
{
	DiffRangeInfo dri(di);
	m_diffs.Add(dri);
}

/**
 * @brief Returns copy of DIFFRANGE from diff-list.
 * @param [in] nDiff Index of DIFFITEM to return.
 * @param [out] di DIFFITEM returned (empty if error)
 * @return TRUE if DIFFITEM found from given index.
 */
BOOL DiffList::GetDiff(int nDiff, DIFFRANGE &di) const
{
	const DIFFRANGE * dfi = DiffRangeAt(nDiff);
	if (!dfi)
	{
		DIFFRANGE empty;
		di = empty;
		return FALSE;
	}
	di = *dfi;
	return TRUE;
}

/**
 * @brief Return (const) pointer to DIFFRANGE entry requested
 */
const DIFFRANGE * DiffList::DiffRangeAt(int nDiff) const
{
	if (nDiff>=0 && nDiff < m_diffs.GetSize())
	{
		DiffList * pThis = const_cast<DiffList *>(this);
		return &pThis->m_diffs.ElementAt(nDiff).diffrange;
	}
	else
	{
		ASSERT(0);
		return NULL;
	}
}

/**
 * @brief Replaces diff in list in given index with given diff.
 *
 * @param [in] nDiff Index (0-based) of diff to be replaced
 * @param [in] di Diff to put in list.
 */
BOOL DiffList::SetDiff(int nDiff, DIFFRANGE di)
{
	if (nDiff < m_diffs.GetSize())
	{
		m_diffs[nDiff] = di;
		return TRUE;
	}
	else
		return FALSE;
}

/**
 * @brief Checks if line is before, inside or after diff
 * @param [in] nLine Linenumber to text buffer (not "real" number)
 * @param [in] nDiff Index to diff table
 * @return -1 if line is before diff, 0 if line is in diff and
 * 1 if line is after diff.
 */
int DiffList::LineRelDiff(UINT nLine, UINT nDiff) const
{
	const DIFFRANGE * dfi = DiffRangeAt(nDiff);
	if (nLine < dfi->dbegin0)
		return -1;
	else if (nLine > dfi->dend0)
		return 1;
	else
		return 0;
}

/**
 * @brief Checks if line is inside given diff
 * @param [in] nLine Linenumber to text buffer (not "real" number)
 * @param [in] nDiff Index to diff table
 */
BOOL DiffList::LineInDiff(UINT nLine, UINT nDiff) const
{
	const DIFFRANGE * dfi = DiffRangeAt(nDiff);
	if (nLine >= dfi->dbegin0 && nLine <= dfi->dend0)
		return TRUE;
	else
		return FALSE;
}

/**
 * @brief Returns diff index for given line.
 * @param [in] nLine Linenumber, 0-based.
 * @return Index to diff table, -1 if line is not inside any diff.
 */
int DiffList::LineToDiff(UINT nLine) const
{
	const int nDiffCount = m_diffs.GetSize();
	if (nDiffCount == 0)
		return -1;

	// First check line is not before first or after last diff
	if (nLine < DiffRangeAt(0)->dbegin0)
		return -1;
	if (nLine > DiffRangeAt(nDiffCount-1)->dend0)
		return -1;

	// Use binary search to search for a diff.
	int left = 0; // Left limit
	int middle = 0; // Compared item
	int right = nDiffCount - 1; // Right limit

	while (left <= right)
	{
		middle = (left + right) / 2;
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
			_RPTF1(_CRT_ERROR, "Invalid return value %d from LineRelDiff(): "
				"-1, 0 or 1 expected!", result); 
			break;
		}
	}
	return -1;
}

/**
 * @brief Return previous diff from given line.
 * @param [in] nLine First line searched.
 * @param [out] nDiff Index of diff found.
 * @return TRUE if line is inside diff, FALSE otherwise.
 */
BOOL DiffList::GetPrevDiff(int nLine, int &nDiff) const
{
	BOOL bInDiff = TRUE;
	int numDiff = LineToDiff(nLine);

	// Line not inside diff
	if (nDiff == -1)
	{
		bInDiff = FALSE;
		for (int i = m_diffs.GetSize() - 1; i >= 0 ; i--)
		{
			if ((int)DiffRangeAt(i)->dend0 <= nLine)
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
 * @brief Return next diff from given line.
 * @param [in] nLine First line searched.
 * @param [out] nDiff Index of diff found.
 * @return TRUE if line is inside diff, FALSE otherwise.
 */
BOOL DiffList::GetNextDiff(int nLine, int &nDiff) const
{
	BOOL bInDiff = TRUE;
	int numDiff = LineToDiff(nLine);

	// Line not inside diff
	if (numDiff == -1)
	{
		bInDiff = FALSE;
		const int nDiffCount = m_diffs.GetSize();
		for (int i = 0; i < nDiffCount; i++)
		{
			if ((int)DiffRangeAt(i)->dbegin0 >= nLine)
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
 * @brief Check if diff-list contains significant diffs.
 * @return TRUE if list has significant diffs, FALSE otherwise.
 */
BOOL DiffList::HasSignificantDiffs() const
{
	if (m_firstSignificant == -1)
		return FALSE;
	return TRUE;
}

/**
 * @brief Return previous diff index from given line.
 * @param [in] nLine First line searched.
 * @return Index for next difference.
 */
int DiffList::PrevSignificantDiffFromLine(UINT nLine) const
{
	int nDiff = -1;

	for (int i = m_diffs.GetSize() - 1; i >= 0 ; i--)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		if (dfi->op != OP_TRIVIAL && dfi->dend0 <= nLine)
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
 * @return Index for previous difference.
 */
int DiffList::NextSignificantDiffFromLine(UINT nLine) const
{
	int nDiff = -1;
	const int nDiffCount = m_diffs.GetSize();

	for (int i = 0; i < nDiffCount; i++)
	{
		const DIFFRANGE * dfi = DiffRangeAt(i);
		if (dfi->op != OP_TRIVIAL && dfi->dbegin0 >= nLine)
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
	int prev = -1;
	// must be called after diff list is entirely populated
	for (int i = 0; i < m_diffs.GetSize(); ++i)
	{
		if (m_diffs[i].diffrange.op == OP_TRIVIAL)
		{
			m_diffs[i].prev = -1;
			m_diffs[i].next = -1;
		}
		else
		{
			m_diffs[i].prev = prev;
			if (prev != -1)
				m_diffs[prev].next = i;
			prev = i;
			if (m_firstSignificant == -1)
				m_firstSignificant = i;
			m_lastSignificant = i;
		}
	}
}

/**
 * @brief Return index to first significant diff.
 */
int DiffList::FirstSignificantDiff() const
{
	return m_firstSignificant;
}

/**
 * @brief Return index of next significant diff.
 * @param [in] nDiff Index to start looking for next diff.
 */
int DiffList::NextSignificantDiff(int nDiff) const
{
	return m_diffs[nDiff].next;
}

/**
 * @brief Return index of previous significant diff.
 * @param [in] nDiff Index to start looking for previous diff.
 */
int DiffList::PrevSignificantDiff(int nDiff) const
{
	return m_diffs[nDiff].prev;
}

/**
 * @brief Return index to last significant diff.
 */
int DiffList::LastSignificantDiff() const
{
	return m_lastSignificant;
}

/**
 * @brief Return pointer to first significant diff range
 */
const DIFFRANGE * DiffList::FirstSignificantDiffRange() const
{
	if (m_firstSignificant == -1) return NULL;
	return DiffRangeAt(m_firstSignificant);
}

/**
 * @brief Return pointer to last significant diff range
 */
const DIFFRANGE * DiffList::LastSignificantDiffRange() const
{
	if (m_lastSignificant == -1) return NULL;
	return DiffRangeAt(m_lastSignificant);
}
