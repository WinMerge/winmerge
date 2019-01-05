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
	DIFFITEM *AddNewDiff(DIFFITEM *parent);
	void RemoveAll();
	void InitDiffItemList();

	// to iterate over all differences on list
	DIFFITEM *GetFirstDiffPosition() const;
	DIFFITEM *GetFirstChildDiffPosition(const DIFFITEM *par) const;
	const DIFFITEM &GetNextDiffPosition(DIFFITEM *&diffpos) const;
	DIFFITEM &GetNextDiffRefPosition(DIFFITEM *&diffpos);
	const DIFFITEM &GetNextSiblingDiffPosition(DIFFITEM *&diffpos) const;
	DIFFITEM &GetNextSiblingDiffRefPosition(DIFFITEM *&diffpos);
	const DIFFITEM &GetDiffAt(DIFFITEM *diffpos) const;
	DIFFITEM &GetDiffRefAt(DIFFITEM *diffpos);

	void SetDiffStatusCode(DIFFITEM *diffpos, unsigned diffcode, unsigned mask);
	void SetDiffCounts(DIFFITEM *diffpos, unsigned diffs, unsigned ignored);
	unsigned GetCustomFlags1(DIFFITEM *diffpos) const;
	void SetCustomFlags1(DIFFITEM *diffpos, unsigned flag);

	void Swap(int idx1, int idx2);

protected:
	DIFFITEM* m_pRoot; /**< Root of list of diffitems; initially `nullptr`. */
};

/**
 * @brief Get copy of Diff Item at given position in difflist.
 * @param diffpos position of item to return
 */
inline const DIFFITEM &DiffItemList::GetDiffAt(DIFFITEM *diffpos) const
{
	return *diffpos;
}

/**
 * @brief Get Diff Item (by reference) at given position in difflist.
 * @param diffpos position of item to return
 */
inline DIFFITEM &DiffItemList::GetDiffRefAt(DIFFITEM *diffpos)
{
	return *diffpos;
}
