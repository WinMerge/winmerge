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
#define POCO_NO_UNWINDOWS 1
#include <Poco/Types.h>

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
	void RemoveDiff(Poco::UIntPtr diffpos);
	void RemoveAll();

	// to iterate over all differences on list
	Poco::UIntPtr GetFirstDiffPosition() const;
	Poco::UIntPtr GetFirstChildDiffPosition(Poco::UIntPtr parentdiffpos) const;
	const DIFFITEM & GetNextDiffPosition(Poco::UIntPtr & diffpos) const;
	DIFFITEM & GetNextDiffRefPosition(Poco::UIntPtr & diffpos);
	const DIFFITEM & GetNextSiblingDiffPosition(Poco::UIntPtr & diffpos) const;
	DIFFITEM &GetNextSiblingDiffRefPosition(Poco::UIntPtr & diffpos);
	const DIFFITEM & GetDiffAt(Poco::UIntPtr diffpos) const;
	DIFFITEM & GetDiffRefAt(Poco::UIntPtr diffpos);

	void SetDiffStatusCode(Poco::UIntPtr diffpos, unsigned diffcode, unsigned mask);
	void SetDiffCounts(Poco::UIntPtr diffpos, unsigned diffs, unsigned ignored);
	unsigned GetCustomFlags1(Poco::UIntPtr diffpos) const;
	void SetCustomFlags1(Poco::UIntPtr diffpos, unsigned flag);

protected:
	ListEntry m_root; /**< Root of list of diffitems */
};

/**
 * @brief Get copy of Diff Item at given position in difflist.
 * @param diffpos position of item to return
 */
inline const DIFFITEM & DiffItemList::GetDiffAt(Poco::UIntPtr diffpos) const
{
	return *reinterpret_cast<DIFFITEM *>(diffpos);
}

/**
 * @brief Get Diff Item (by reference) at given position in difflist.
 * @param diffpos position of item to return
 */
inline DIFFITEM & DiffItemList::GetDiffRefAt(Poco::UIntPtr diffpos)
{
	return *reinterpret_cast<DIFFITEM *>(diffpos);
}

#endif // _DIFF_ITEM_LIST_H_
