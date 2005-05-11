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
}

/**
 * @brief Removes all diffs from list.
 */
void DiffList::Clear()
{
	m_diffs.RemoveAll();
}

/**
 * @brief Returns count of items in diff list.
 */
int DiffList::GetSize()
{
	return m_diffs.GetSize();
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
	m_diffs.Add(di);
}

/**
 * @brief Returns copy of DIFFITEM from diff-list.
 * @param [in] nDiff Index of DIFFITEM to return.
 * @param [out] di DIFFITEM returned (empty if error)
 * @return TRUE if DIFFITEM found from given index.
 */
BOOL DiffList::GetDiff(int nDiff, DIFFRANGE &di) const
{
	ASSERT(nDiff < m_diffs.GetSize());
	DIFFRANGE diff = {0};
	if (nDiff >= 0 && nDiff < m_diffs.GetSize())
	{
		di = m_diffs[nDiff];
		return TRUE;
	}
	else
	{
		di = diff;
		return FALSE;
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
	ASSERT((int)nDiff < m_diffs.GetSize());
	if (nLine < m_diffs[nDiff].dbegin0)
		return -1;
	else if (nLine > m_diffs[nDiff].dend0)
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
	ASSERT((int)nDiff < m_diffs.GetSize());
	if (nLine >= m_diffs[nDiff].dbegin0 &&
			nLine <= m_diffs[nDiff].dend0)
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
	if (nLine < m_diffs[0].dbegin0)
		return -1;
	if (nLine > m_diffs[nDiffCount-1].dend0)
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
			if ((int)m_diffs[i].dend0 <= nLine)
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
			if ((int)m_diffs[i].dbegin0 >= nLine)
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
 * @return Index for next difference.
 */
int DiffList::PrevDiffFromLine(UINT nLine) const
{
	int nDiff = -1;

	for (int i = m_diffs.GetSize() - 1; i >= 0 ; i--)
	{
		if (m_diffs[i].dend0 <= nLine)
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
int DiffList::NextDiffFromLine(UINT nLine) const
{
	int nDiff = -1;
	const int nDiffCount = m_diffs.GetSize();

	for (int i = 0; i < nDiffCount; i++)
	{
		if (m_diffs[i].dbegin0 >= nLine)
		{
			nDiff = i;
			break;
		}
	}
	return nDiff;
}
