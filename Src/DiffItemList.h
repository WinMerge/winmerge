/**
 *  @file DiffItemList.h
 *
 *  @brief Declaration of DiffItemList
 */
#pragma once

#include "DiffItem.h"

/**
 * @brief List of DIFFITEMs in folder compare.
 * This class holds a list of items we have in the folder compare. Basically
 * we have a linked list of DIFFITEMs. But there is a structure that follows
 * the actual folder structure. Each DIFFITEM can have a parent folder and
 * another list of child items. Parent DIFFITEM is always a folder item.
 */
class DiffItemList
{
public:
	DiffItemList();
	~DiffItemList();
	// add & remove differences
	DIFFITEM *AddDiff(DIFFITEM *parent);
	void RemoveDiff(uintptr_t diffpos);
	void RemoveAll();

	// to iterate over all differences on list
	uintptr_t GetFirstDiffPosition() const;
	uintptr_t GetFirstChildDiffPosition(uintptr_t parentdiffpos) const;
	const DIFFITEM & GetNextDiffPosition(uintptr_t & diffpos) const;
	DIFFITEM & GetNextDiffRefPosition(uintptr_t & diffpos);
	const DIFFITEM & GetNextSiblingDiffPosition(uintptr_t & diffpos) const;
	DIFFITEM &GetNextSiblingDiffRefPosition(uintptr_t & diffpos);
	const DIFFITEM & GetDiffAt(uintptr_t diffpos) const;
	DIFFITEM & GetDiffRefAt(uintptr_t diffpos);

	void SetDiffStatusCode(uintptr_t diffpos, unsigned diffcode, unsigned mask);
	void SetDiffCounts(uintptr_t diffpos, unsigned diffs, unsigned ignored);
	unsigned GetCustomFlags1(uintptr_t diffpos) const;
	void SetCustomFlags1(uintptr_t diffpos, unsigned flag);

	void Swap(int idx1, int idx2);

protected:
	ListEntry m_root; /**< Root of list of diffitems */
};

/**
 * @brief Get copy of Diff Item at given position in difflist.
 * @param diffpos position of item to return
 */
inline const DIFFITEM & DiffItemList::GetDiffAt(uintptr_t diffpos) const
{
	return *reinterpret_cast<DIFFITEM *>(diffpos);
}

/**
 * @brief Get Diff Item (by reference) at given position in difflist.
 * @param diffpos position of item to return
 */
inline DIFFITEM & DiffItemList::GetDiffRefAt(uintptr_t diffpos)
{
	return *reinterpret_cast<DIFFITEM *>(diffpos);
}
