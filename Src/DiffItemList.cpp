/**
 *  @file DiffItemList.cpp
 *
 *  @brief Implementation of DiffItemList
 */ 

#include "DiffItemList.h"
#include <cassert>

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
void DiffItemList::RemoveDiff(uintptr_t diffpos)
{
	DIFFITEM *p = reinterpret_cast<DIFFITEM *>(diffpos);
	p->RemoveSelf();
	delete p;
}

/**
 * @brief Empty structured DIFFITEM tree
 */
void DiffItemList::RemoveAll()
{
	while (m_root.IsSibling(m_root.Flink))
		RemoveDiff((uintptr_t)m_root.Flink);
}

/**
 * @brief Get position of first item in structured DIFFITEM tree
 */
uintptr_t DiffItemList::GetFirstDiffPosition() const
{
	return (uintptr_t)m_root.IsSibling(m_root.Flink);
}

/**
 * @brief Get position of first child item in structured DIFFITEM tree
 * @param  parentdiffpos [in] Position of parent diff item 
 * @return Position of first child item
 */
uintptr_t DiffItemList::GetFirstChildDiffPosition(uintptr_t parentdiffpos) const
{
	DIFFITEM *parent = reinterpret_cast<DIFFITEM *>(parentdiffpos);
	if (parent)
		return (uintptr_t)parent->children.IsSibling(parent->children.Flink);
	else
		return (uintptr_t)m_root.IsSibling(m_root.Flink);
}

/**
 * @brief Get position of next item in structured DIFFITEM tree
 * @param diffpos position of current item, updated to next item position
 * @return Diff Item in current position
 */
const DIFFITEM &DiffItemList::GetNextDiffPosition(uintptr_t & diffpos) const
{
	DIFFITEM *p = reinterpret_cast<DIFFITEM *>(diffpos);
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
				diffpos = (uintptr_t)cur->parent->children.IsSibling(cur->Flink);
			else
				diffpos = (uintptr_t)m_root.IsSibling(cur->Flink);
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
DIFFITEM &DiffItemList::GetNextDiffRefPosition(uintptr_t & diffpos)
{
	return (DIFFITEM &)GetNextDiffPosition(diffpos);
}

/**
 * @brief Get position of next sibling item in structured DIFFITEM tree
 * @param diffpos position of current item, updated to next sibling item position
 * @return Diff Item in current position
 */
const DIFFITEM &DiffItemList::GetNextSiblingDiffPosition(uintptr_t & diffpos) const
{
	DIFFITEM *p = reinterpret_cast<DIFFITEM *>(diffpos);
	if (p->parent)
		diffpos = (uintptr_t)p->parent->children.IsSibling(p->Flink);
	else
		diffpos = (uintptr_t)m_root.IsSibling(p->Flink);
	return *p;
}

/**
 * @brief Get position of next sibling item in structured DIFFITEM tree
 * @param diffpos position of current item, updated to next sibling item position
 * @return Diff Item (by reference) in current position
 */
DIFFITEM &DiffItemList::GetNextSiblingDiffRefPosition(uintptr_t & diffpos)
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
void DiffItemList::SetDiffStatusCode(uintptr_t diffpos, unsigned diffcode, unsigned mask)
{
	assert(diffpos != NULL);
	DIFFITEM & di = GetDiffRefAt(diffpos);
	assert( ((~mask) & diffcode) == 0 ); // make sure they only set flags in their mask
	di.diffcode.diffcode &= (~mask); // remove current data
	di.diffcode.diffcode |= diffcode; // add new data
}

/**
 * @brief Update difference counts.
 */
void DiffItemList::SetDiffCounts(uintptr_t diffpos, unsigned diffs, unsigned ignored)
{
	assert(diffpos != NULL);
	DIFFITEM & di = GetDiffRefAt(diffpos);
	di.nidiffs = ignored; // see StoreDiffResult() in DirScan.cpp
	di.nsdiffs = diffs;
}

/**
 * @brief Returns item's custom (user) flags.
 * @param [in] diffpos Position of item.
 * @return Custom flags from item.
 */
unsigned DiffItemList::GetCustomFlags1(uintptr_t diffpos) const
{
	assert(diffpos != NULL);
	const DIFFITEM & di = GetDiffAt(diffpos);
	return di.customFlags1;
}

/**
 * @brief Sets item's custom (user) flags.
 * @param [in] diffpos Position of item.
 * @param [in] flag Value of flag to set.
 */
void DiffItemList::SetCustomFlags1(uintptr_t diffpos, unsigned flag)
{
	assert(diffpos != NULL);
	DIFFITEM & di = GetDiffRefAt(diffpos);
	di.customFlags1 = flag;
}

void DiffItemList::Swap(int idx1, int idx2)
{
	for (ListEntry *p = m_root.IsSibling(m_root.Flink); p; p = m_root.IsSibling(p->Flink))
		static_cast<DIFFITEM *>(p)->Swap(idx1, idx2);
}
