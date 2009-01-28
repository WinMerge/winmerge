/**
 *  @file DiffItemList.cpp
 *
 *  @brief Implementation of DiffItemList
 */ 
// ID line follows -- this is updated by SVN
// $Id$

#include <windows.h>
#include <assert.h>
#include "DiffItemList.h"

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
 * @brief Add new diffitem to structured DIFFITEM tree.
 * @param [in] parent Parent item, or NULL if no parent.
 * @return Pointer to the added item.
 */
DIFFITEM* DiffItemList::AddDiff(DIFFITEM *parent)
{
	DIFFITEM *p = new DIFFITEM;
	if (parent)
		parent->children.Append(p);
	else
		m_root.Append(p);
	p->parent = parent;
	return p;
}

/**
 * @brief Remove diffitem from structured DIFFITEM tree
 * @param diffpos position of item to remove
 */
void DiffItemList::RemoveDiff(UINT_PTR diffpos)
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
		RemoveDiff((UINT_PTR)m_root.Flink);
}

/**
 * @brief Get position of first item in structured DIFFITEM tree
 */
UINT_PTR DiffItemList::GetFirstDiffPosition() const
{
	return (UINT_PTR)m_root.IsSibling(m_root.Flink);
}

/**
 * @brief Get position of first child item in structured DIFFITEM tree
 * @param  parentdiffpos [in] Position of parent diff item 
 * @return Position of first child item
 */
UINT_PTR DiffItemList::GetFirstChildDiffPosition(UINT_PTR parentdiffpos) const
{
	DIFFITEM *parent = (DIFFITEM *)parentdiffpos;
	if (parent)
		return (UINT_PTR)parent->children.IsSibling(parent->children.Flink);
	else
		return (UINT_PTR)m_root.IsSibling(m_root.Flink);
}

/**
 * @brief Get position of next item in structured DIFFITEM tree
 * @param diffpos position of current item, updated to next item position
 * @return Diff Item in current position
 */
const DIFFITEM &DiffItemList::GetNextDiffPosition(UINT_PTR & diffpos) const
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
				diffpos = (UINT_PTR)cur->parent->children.IsSibling(cur->Flink);
			else
				diffpos = (UINT_PTR)m_root.IsSibling(cur->Flink);
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
DIFFITEM &DiffItemList::GetNextDiffRefPosition(UINT_PTR & diffpos)
{
	return (DIFFITEM &)GetNextDiffPosition(diffpos);
}

/**
 * @brief Get position of next sibling item in structured DIFFITEM tree
 * @param diffpos position of current item, updated to next sibling item position
 * @return Diff Item in current position
 */
const DIFFITEM &DiffItemList::GetNextSiblingDiffPosition(UINT_PTR & diffpos) const
{
	DIFFITEM *p = (DIFFITEM *)diffpos;
	if (p->parent)
		diffpos = (UINT_PTR)p->parent->children.IsSibling(p->Flink);
	else
		diffpos = (UINT_PTR)m_root.IsSibling(p->Flink);
	return *p;
}

/**
 * @brief Get position of next sibling item in structured DIFFITEM tree
 * @param diffpos position of current item, updated to next sibling item position
 * @return Diff Item (by reference) in current position
 */
DIFFITEM &DiffItemList::GetNextSiblingDiffRefPosition(UINT_PTR & diffpos)
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
void DiffItemList::SetDiffStatusCode(UINT_PTR diffpos, UINT diffcode, UINT mask)
{
	assert(diffpos);
	DIFFITEM & di = GetDiffRefAt(diffpos);
	assert(! ((~mask) & diffcode) ); // make sure they only set flags in their mask
	di.diffcode.diffcode &= (~mask); // remove current data
	di.diffcode.diffcode |= diffcode; // add new data
}

/**
 * @brief Update difference counts.
 */
void DiffItemList::SetDiffCounts(UINT_PTR diffpos, UINT diffs, UINT ignored)
{
	assert(diffpos);
	DIFFITEM & di = GetDiffRefAt(diffpos);
	di.nidiffs = ignored; // see StoreDiffResult() in DirScan.cpp
	di.nsdiffs = diffs;
}

/**
 * @brief Returns item's custom (user) flags.
 * @param [in] diffpos Position of item.
 * @return Custom flags from item.
 */
UINT DiffItemList::GetCustomFlags1(UINT_PTR diffpos) const
{
	assert(diffpos);
	const DIFFITEM & di = GetDiffAt(diffpos);
	return di.customFlags1;
}

/**
 * @brief Sets item's custom (user) flags.
 * @param [in] diffpos Position of item.
 * @param [in] flag Value of flag to set.
 */
void DiffItemList::SetCustomFlags1(UINT_PTR diffpos, UINT flag)
{
	assert(diffpos);
	DIFFITEM & di = GetDiffRefAt(diffpos);
	di.customFlags1 = flag;
}
