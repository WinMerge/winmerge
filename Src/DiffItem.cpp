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

/** @brief Return path to left file, including all but file name */
String DIFFITEM::getLeftFilepath(const String &sLeftRoot) const
{
	String sPath;
	if (!diffcode.isSideRightOnly())
	{
		sPath = paths_ConcatPath(sLeftRoot, left.path);
	}
	return sPath;
}

/** @brief Return path to right file, including all but file name */
String DIFFITEM::getRightFilepath(const String &sRightRoot) const
{
	String sPath;
	if (!diffcode.isSideLeftOnly())
	{
		sPath = paths_ConcatPath(sRightRoot, right.path);
	}
	return sPath;
}
