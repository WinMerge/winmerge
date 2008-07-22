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
 * @brief Constructor
 */
DiffItemList::DiffItemList()
{
}

/**
 * @brief Destructor
 */
DiffItemList::~DiffItemList()
{
	RemoveAll();
}

/**
 * @brief Add new diffitem to structured DIFFITEM tree
 */
DIFFITEM &DiffItemList::AddDiff(DIFFITEM *parent)
{
	DIFFITEM *p = new DIFFITEM;
	if (parent)
		parent->children.Append(p);
	else
		m_root.Append(p);
	p->parent = parent;
	return *p;
}

/**
 * @brief Remove diffitem from structured DIFFITEM tree
 * @param diffpos position of item to remove
 */
void DiffItemList::RemoveDiff(POSITION diffpos)
{
	DIFFITEM *p = (DIFFITEM *)diffpos;
	p->RemoveSelf();
	delete p;
}

/**
 * @brief Empty structured DIFFITEM tree
 */
void DiffItemList::RemoveAll()
{
	while (m_root.IsSibling(m_root.Flink))
		RemoveDiff((POSITION)m_root.Flink);
}

/**
 * @brief Get position of first item in structured DIFFITEM tree
 */
POSITION DiffItemList::GetFirstDiffPosition() const
{
	return (POSITION)m_root.IsSibling(m_root.Flink);
}

/**
 * @brief Get position of first child item in structured DIFFITEM tree
 * @param  parentdiffpos [in] Position of parent diff item 
 * @return Position of first child item
 */
POSITION DiffItemList::GetFirstChildDiffPosition(POSITION parentdiffpos) const
{
	DIFFITEM *parent = (DIFFITEM *)parentdiffpos;
	if (parent)
		return (POSITION)parent->children.IsSibling(parent->children.Flink);
	else
		return (POSITION)m_root.IsSibling(m_root.Flink);
}

/**
 * @brief Get position of next item in structured DIFFITEM tree
 * @param diffpos position of current item, updated to next item position
 * @return Diff Item in current position
 */
const DIFFITEM &DiffItemList::GetNextDiffPosition(POSITION & diffpos) const
{
	DIFFITEM *p = (DIFFITEM *)diffpos;
	if (p->HasChildren())
	{
		diffpos = GetFirstChildDiffPosition(diffpos);
	}
	else
	{
		DIFFITEM *cur = p;
		do
		{
			if (cur->parent)
				diffpos = (POSITION)cur->parent->children.IsSibling(cur->Flink);
			else
				diffpos = (POSITION)m_root.IsSibling(cur->Flink);
			cur = cur->parent;
		} while (!diffpos && cur);
	}
	return *p;
}

/**
 * @brief Get position of next item in structured DIFFITEM tree
 * @param diffpos position of current item, updated to next item position
 * @return Diff Item (by reference) in current position
 */
DIFFITEM &DiffItemList::GetNextDiffRefPosition(POSITION & diffpos)
{
	return (DIFFITEM &)GetNextDiffPosition(diffpos);
}

/**
 * @brief Get position of next sibling item in structured DIFFITEM tree
 * @param diffpos position of current item, updated to next sibling item position
 * @return Diff Item in current position
 */
const DIFFITEM &DiffItemList::GetNextSiblingDiffPosition(POSITION & diffpos) const
{
	DIFFITEM *p = (DIFFITEM *)diffpos;
	if (p->parent)
		diffpos = (POSITION)p->parent->children.IsSibling(p->Flink);
	else
		diffpos = (POSITION)m_root.IsSibling(p->Flink);
	return *p;
}

/**
 * @brief Get position of next sibling item in structured DIFFITEM tree
 * @param diffpos position of current item, updated to next sibling item position
 * @return Diff Item (by reference) in current position
 */
DIFFITEM &DiffItemList::GetNextSiblingDiffRefPosition(POSITION & diffpos)
{
	return (DIFFITEM &)GetNextSiblingDiffPosition(diffpos);
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
	DIFFITEM & di = GetDiffRefAt(diffpos);
	ASSERT(! ((~mask) & diffcode) ); // make sure they only set flags in their mask
	di.diffcode.diffcode &= (~mask); // remove current data
	di.diffcode.diffcode |= diffcode; // add new data
}

/**
 * @brief Update difference counts.
 */
void DiffItemList::SetDiffCounts(POSITION diffpos, UINT diffs, UINT ignored)
{
	ASSERT(diffpos);
	DIFFITEM & di = GetDiffRefAt(diffpos);
	di.nidiffs = ignored; // see StoreDiffResult() in DirScan.cpp
	di.nsdiffs = diffs;
}

/**
 * @brief Returns item's custom (user) flags.
 * @param [in] diffpos Position of item.
 * @return Custom flags from item.
 */
UINT DiffItemList::GetCustomFlags1(POSITION diffpos) const
{
	ASSERT(diffpos);
	const DIFFITEM & di = GetDiffAt(diffpos);
	return di.customFlags1;
}

/**
 * @brief Sets item's custom (user) flags.
 * @param [in] diffpos Position of item.
 * @param [in] flag Value of flag to set.
 */
void DiffItemList::SetCustomFlags1(POSITION diffpos, UINT flag)
{
	ASSERT(diffpos);
	DIFFITEM & di = GetDiffRefAt(diffpos);
	di.customFlags1 = flag;
}
