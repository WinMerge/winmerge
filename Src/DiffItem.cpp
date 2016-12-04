/**
 *  @file DiffItem.cpp
 *
 *  @brief Implementation of DIFFITEM
 */ 

#include "DiffItem.h"
#include "paths.h"

DIFFITEM DIFFITEM::emptyitem;

/** @brief DIFFITEM's destructor */
DIFFITEM::~DIFFITEM()
{
	RemoveChildren();
}

/** @brief Return path to left/right file, including all but file name */
String DIFFITEM::getFilepath(int nIndex, const String &sRoot) const
{
	if (diffcode.exists(nIndex))
	{
		return paths::ConcatPath(sRoot, diffFileInfo[nIndex].path);
	}
	return _T("");
}

/** @brief Return depth of path */
int DIFFITEM::GetDepth() const
{
	const DIFFITEM *cur;
	int depth;
	for (depth = 0, cur = parent; cur; depth++, cur = cur->parent)
		;
	return depth;
}

/**
 * @brief Return whether the specified item is an ancestor of the current item
 */
bool DIFFITEM::IsAncestor(const DIFFITEM *pdi) const
{
	const DIFFITEM *cur;
	for (cur = this; cur; cur = cur->parent)
	{
		if (cur->parent == pdi)
			return true;
	}
	return false;
}

/** @brief Return whether the current item has children */
bool DIFFITEM::HasChildren() const
{
	DIFFITEM *p = static_cast<DIFFITEM *>(children.IsSibling(children.Flink));
	return p ? true : false;
}

void DIFFITEM::RemoveChildren()
{
	while (HasChildren())
	{
		DIFFITEM *p = static_cast<DIFFITEM *>(children.Flink);
		p->RemoveSelf();
		delete p;
	}
}

void DIFFITEM::Swap(int idx1, int idx2)
{
	std::swap(diffFileInfo[idx1], diffFileInfo[idx2]);
	diffcode.swap(idx1, idx2);
	if (HasChildren())
	{
		for (ListEntry *p = children.IsSibling(children.Flink); p; p = children.IsSibling(p->Flink))
			static_cast<DIFFITEM *>(p)->Swap(idx1, idx2);
	}
}
