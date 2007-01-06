/**
 *  @file DiffItem.cpp
 *
 *  @brief Implementation of DIFFITEM
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "DiffItem.h"
#include "paths.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/** @brief Make and return a diffitem with no data */
DIFFITEM DIFFITEM::MakeEmptyDiffItem() // static
{
	DIFFITEM di;
	di.empty = true;
	return di;
}

/** @brief Return path to left file, including all but file name */
CString DIFFITEM::getLeftFilepath(const CString &sLeftRoot) const
{
	CString sPath;
	if (!isSideRightOnly())
	{
		sPath = paths_ConcatPath(sLeftRoot, sLeftSubdir);
	}
	return sPath;
}

/** @brief Return path to right file, including all but file name */
CString DIFFITEM::getRightFilepath(const CString &sRightRoot) const
{
	CString sPath;
	if (!isSideLeftOnly())
	{
		sPath = paths_ConcatPath(sRightRoot, sRightSubdir);
	}
	return sPath;
}
