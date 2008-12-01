/**
 *  @file DiffItemList.h
 *
 *  @brief Declaration of DiffItemList
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _DIFF_ITEM_LIST_H_
#define _DIFF_ITEM_LIST_H_

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
	DIFFITEM &AddDiff(DIFFITEM *parent);
	void RemoveDiff(UINT_PTR diffpos);
	void RemoveAll();

	// to iterate over all differences on list
	UINT_PTR GetFirstDiffPosition() const;
	UINT_PTR GetFirstChildDiffPosition(UINT_PTR parentdiffpos) const;
	const DIFFITEM & GetNextDiffPosition(UINT_PTR & diffpos) const;
	DIFFITEM & GetNextDiffRefPosition(UINT_PTR & diffpos);
	const DIFFITEM & GetNextSiblingDiffPosition(UINT_PTR & diffpos) const;
	DIFFITEM &GetNextSiblingDiffRefPosition(UINT_PTR & diffpos);
	const DIFFITEM & GetDiffAt(UINT_PTR diffpos) const;
	DIFFITEM & GetDiffRefAt(UINT_PTR diffpos);

	void SetDiffStatusCode(UINT_PTR diffpos, UINT diffcode, UINT mask);
	void SetDiffCounts(UINT_PTR diffpos, UINT diffs, UINT ignored);
	UINT GetCustomFlags1(UINT_PTR diffpos) const;
	void SetCustomFlags1(UINT_PTR diffpos, UINT flag);

protected:
	ListEntry m_root; /**< Root of list of diffitems */
};

/**
 * @brief Get copy of Diff Item at given position in difflist.
 * @param diffpos position of item to return
 */
inline const DIFFITEM & DiffItemList::GetDiffAt(UINT_PTR diffpos) const
{
	return *reinterpret_cast<DIFFITEM *>(diffpos);
}

/**
 * @brief Get Diff Item (by reference) at given position in difflist.
 * @param diffpos position of item to return
 */
inline DIFFITEM & DiffItemList::GetDiffRefAt(UINT_PTR diffpos)
{
	return *reinterpret_cast<DIFFITEM *>(diffpos);
}

#endif // _DIFF_ITEM_LIST_H_
