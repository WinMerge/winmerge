/**
 *  @file DiffItemList.cpp
 *
 *  @brief Implementation of DiffItemList
 */ 

#include "stdafx.h"
#include "DiffItemList.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Constructor
 */
DiffItemList::DiffItemList() : m_pRoot(nullptr)
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
 * @param [in] par Parent item, or `nullptr` if no parent.
 * @return Pointer to the added item.
 */
DIFFITEM *DiffItemList::AddNewDiff(DIFFITEM *par)
{
	if (par == nullptr)
	{
		// if there is no `parent`, this item becomes a child of m_pRoot
		assert(m_pRoot != nullptr);
		par = m_pRoot;
	}

	DIFFITEM *p = new DIFFITEM;
	// if there is a `parent`, this item becomes a child of that parent
	p->parent = par;
	if (par->children == nullptr)
		par->children = p;
	else
		par->children->Append(p);

	return p;
}

/**
 * @brief Empty structured DIFFITEM tree
 */
void DiffItemList::RemoveAll()
{
	delete m_pRoot;
	m_pRoot = nullptr;
}

void DiffItemList::InitDiffItemList()
{
	assert(m_pRoot == nullptr);
	m_pRoot = new DIFFITEM;
}

/**
 * @brief Get position of first item in structured DIFFITEM tree
 */
DIFFITEM *DiffItemList::GetFirstDiffPosition() const
{
	return m_pRoot->children;
}

/**
 * @brief Get position of first child item in structured DIFFITEM tree
 * @param  [in] par Position of parent diff item (maybe `nullptr`)
 * @return Position of first child item (or `nullptr` if no children)
 */
DIFFITEM *DiffItemList::GetFirstChildDiffPosition(const DIFFITEM *par) const
{
	if (par != nullptr)
		return par->children;
	else
		return m_pRoot->children;
}

/**
 * @brief Get position of next item in structured DIFFITEM tree
 * @param [in,out] diffpos Position of current item, updated to next item position
 * @return Diff Item (by reference) in current position
 */
const DIFFITEM &DiffItemList::GetNextDiffPosition(DIFFITEM *&diffpos) const
{
	DIFFITEM *p = diffpos;
	if (diffpos->HasChildren())
	{
		diffpos = GetFirstChildDiffPosition(diffpos);
	}
	else
	{
		DIFFITEM *cur = diffpos;
		do
		{
			diffpos = cur->GetFwdSiblingLink();
			cur = cur->parent;
			assert(cur != nullptr);
		} while (diffpos == nullptr && cur->parent != nullptr);
	}
	return *p;
}

/**
 * @brief Get position of next item in structured DIFFITEM tree
 * @param [in,out] diffpos Position of current item, updated to next item position
 * @return Diff Item (by reference) in current position
 */
DIFFITEM &DiffItemList::GetNextDiffRefPosition(DIFFITEM *&diffpos)
{
	return (DIFFITEM &)GetNextDiffPosition(diffpos);
}

/**
 * @brief Get position of next sibling item in structured DIFFITEM tree
 * @param [in,out] diffpos Position of current item, updated to next sibling item position
 * @return Diff Item (by reference) in current position
 */
const DIFFITEM &DiffItemList::GetNextSiblingDiffPosition(DIFFITEM *&diffpos) const
{
	DIFFITEM *p = diffpos;
	diffpos = p->GetFwdSiblingLink();
	assert(p==nullptr || diffpos==nullptr || p->parent == diffpos->parent);
	assert(p->parent != nullptr);
	return *p;
}

/**
 * @brief Get position of next sibling item in structured DIFFITEM tree
 * @param [in,out] diffpos Position of current item, updated to next sibling item position
 * @return Diff Item (by reference) in current position
 */
DIFFITEM &DiffItemList::GetNextSiblingDiffRefPosition(DIFFITEM *&diffpos)
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
void DiffItemList::SetDiffStatusCode(DIFFITEM *diffpos, unsigned diffcode, unsigned mask)
{
	assert(diffpos != nullptr);
	DIFFITEM &di = GetDiffRefAt(diffpos);
	assert( ((~mask) & diffcode) == 0 ); // make sure they only set flags in their mask
	di.diffcode.diffcode &= (~mask); // remove current data
	di.diffcode.diffcode |= diffcode; // add new data
}

/**
 * @brief Update difference counts.
 */
void DiffItemList::SetDiffCounts(DIFFITEM *diffpos, unsigned diffs, unsigned ignored)
{
	assert(diffpos != nullptr);
	DIFFITEM &di = GetDiffRefAt(diffpos);
	di.nidiffs = ignored; // see StoreDiffResult() in DirScan.cpp
	di.nsdiffs = diffs;
}

/**
 * @brief Returns item's custom (user) flags.
 * @param [in] diffpos Position of item.
 * @return Custom flags from item.
 */
unsigned DiffItemList::GetCustomFlags1(DIFFITEM *diffpos) const
{
	assert(diffpos != nullptr);
	const DIFFITEM &di = GetDiffAt(diffpos);
	return di.customFlags1;
}

/**
 * @brief Sets item's custom (user) flags.
 * @param [in] diffpos Position of item.
 * @param [in] flag Value of flag to set.
 */
void DiffItemList::SetCustomFlags1(DIFFITEM *diffpos, unsigned flag)
{
	assert(diffpos != nullptr);
	DIFFITEM &di = GetDiffRefAt(diffpos);
	di.customFlags1 = flag;
}

void DiffItemList::Swap(int idx1, int idx2)
{
	assert(m_pRoot != nullptr);
	for (DIFFITEM *p = m_pRoot->children; p != nullptr; p = p->GetFwdSiblingLink())
		p->Swap(idx1, idx2);
}
