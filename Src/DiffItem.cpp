/**
 *  @file DiffItem.cpp
 *
 *  @brief Implementation of DIFFITEM
 */ 
// ID line follows -- this is updated by SVN
// $Id: DiffItem.cpp 7063 2009-12-27 15:28:16Z kimmov $

#include <windows.h>
#include "DiffItem.h"
#include "paths.h"

DIFFITEM DIFFITEM::emptyitem;

/** @brief DIFFITEM's destructor */
DIFFITEM::~DIFFITEM()
{
	while (children.IsSibling(children.Flink))
	{
		DIFFITEM *p = (DIFFITEM *)children.Flink;
		p->RemoveSelf();
		delete p;
	}
}

/** @brief Return path to left/right file, including all but file name */
String DIFFITEM::getFilepath(int nIndex, const String &sRoot) const
{
	String sPath;
	if (diffcode.isExists(nIndex))
	{
		sPath = paths_ConcatPath(sRoot, diffFileInfo[nIndex].path);
	}
	return sPath;
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
	DIFFITEM *p = (DIFFITEM *)children.IsSibling(children.Flink);
	return p ? true : false;
}

