/**
 *  @file DiffItem.cpp
 *
 *  @brief Implementation of DIFFITEM
 */ 
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "DiffItem.h"
#include "paths.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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

/** @brief Return path to left file, including all but file name */
String DIFFITEM::GetLeftFilepath(const String &sLeftRoot) const
{
	String sPath;
	if (!diffcode.isSideRightOnly())
	{
		sPath = paths_ConcatPath(sLeftRoot, left.path);
	}
	return sPath;
}

/** @brief Return path to right file, including all but file name */
String DIFFITEM::GetRightFilepath(const String &sRightRoot) const
{
	String sPath;
	if (!diffcode.isSideLeftOnly())
	{
		sPath = paths_ConcatPath(sRightRoot, right.path);
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

