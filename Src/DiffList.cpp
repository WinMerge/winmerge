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

DiffList::DiffList()
{
	m_diffs.SetSize(64);
}

void DiffList::Clear()
{
	m_diffs.RemoveAll();
}

int DiffList::GetSize()
{
	return m_diffs.GetSize();
}

void DiffList::SetSize(UINT nSize)
{
	m_diffs.SetSize(nSize);
}

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
 * @brief Checks if line is inside given diff
 * @param [in] nLine Linenumber to text buffer (not "real" number)
 * @param [in] nDiff Index to diff table
 */
BOOL DiffList::LineInDiff(UINT nLine, UINT nDiff) const
{
	ASSERT(nDiff < m_diffs.GetSize());
	if (nLine >= m_diffs[nDiff].dbegin0 &&
			nLine <= m_diffs[nDiff].dend0)
		return TRUE;
	else
		return FALSE;
}

/**
 * @brief Checks if given line is inside diff and
 * @param [in] nLine Linenumber, 0-based.
 * @return Index to diff table, -1 if line no inside any diff.
 */
int DiffList::LineToDiff(UINT nLine) const
{
	for (UINT i = 0; i < m_diffs.GetSize(); i++)
	{
		if (LineInDiff(nLine, i))
			return i;
	}
	return -1;
}

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
		for (UINT i = 0; i < m_diffs.GetSize(); i++)
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

int DiffList::NextDiffFromLine(UINT nLine) const
{
	int nDiff = -1;

	for (UINT i = 0; i < m_diffs.GetSize(); i++)
	{
		if (m_diffs[i].dbegin0 >= nLine)
		{
			nDiff = i;
			break;
		}
	}
	return nDiff;
}
