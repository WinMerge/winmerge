/**
 *  @file DiffItemList.cpp
 *
 *  @brief Implementation of DiffItemList
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "DiffItemList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/**
 * @brief Add new diffitem to CDiffContext array
 */
void DiffItemList::AddDiff(const DIFFITEM & di)
{
	m_dirlist.AddTail(const_cast<DIFFITEM &>(di));
}

/**
 * @brief Remove diffitem from CDiffContext array
 * @param diffpos position of item to remove
 */
void DiffItemList::RemoveDiff(POSITION diffpos)
{
	m_dirlist.RemoveAt(diffpos);
}

/**
 * @brief Empty CDiffContext array
 */
void DiffItemList::RemoveAll()
{
	m_dirlist.RemoveAll();
}

/**
 * @brief Get position of first item in CDiffContext array
 */
POSITION DiffItemList::GetFirstDiffPosition() const
{
	return m_dirlist.GetHeadPosition();
}

/**
 * @brief Get position of next item in CDiffContext array
 * @param diffpos position of current item, updated to next item position
 * @return Diff Item in current position
 */
DIFFITEM DiffItemList::GetNextDiffPosition(POSITION & diffpos) const
{
	return m_dirlist.GetNext(diffpos);
}

/**
 * @brief Get Diff Item at given position of CDiffContext array
 * @param diffpos position of item to return
 */
DIFFITEM DiffItemList::GetDiffAt(POSITION diffpos) const
{
	DIFFITEM di = m_dirlist.GetAt(diffpos);
	return di;
}

/**
 * @brief Get number of items in CDiffContext array
 */
int DiffItemList::GetDiffCount() const
{
	return m_dirlist.GetCount();
}

/**
 * @brief Alter some bit flags of the diffcode.
 *
 * Examples:
 *  SetDiffStatusCode(pos, DIFFCODE::SAME, DIFFCODE::COMPAREFLAGS)
 *   changes the comparison result to be the same.
 * 
 *  SetDiffStatusCode(pos, DIFFCODE::BOTH, DIFFCODE::SIDEFLAG)
 *   changes the side status to be both (sides).
 *
 * SetDiffStatusCode(pos, DIFFCODE::SAME+DIFFCODE::BOTH, DIFFCODE::COMPAREFLAGS+DIFFCODE::SIDEFLAG);
 *  changes the comparison result to be the same and the side status to be both
 */
void DiffItemList::SetDiffStatusCode(POSITION diffpos, UINT diffcode, UINT mask)
{
	ASSERT(diffpos);
	DIFFITEM & di = m_dirlist.GetAt(diffpos);
	ASSERT(! ((~mask) & diffcode) ); // make sure they only set flags in their mask
	di.diffcode &= (~mask); // remove current data
	di.diffcode |= diffcode; // add new data
}

/**
 * @brief Update difference counts.
 */
void DiffItemList::SetDiffCounts(POSITION diffpos, UINT diffs, UINT ignored)
{
	ASSERT(diffpos);
	DIFFITEM & di = m_dirlist.GetAt(diffpos);
	di.ndiffs = diffs + ignored; // see StoreDiffResult() in DirScan.cpp
	di.nsdiffs = diffs;
}