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

/**
 * @brief Copy constructor.
 */
DIFFITEM::DIFFITEM(const DIFFITEM &di)
{
	left = di.left;
	right = di.right;
	diffcode = di.diffcode;
	if (di.sfilename.GetLength())
		sfilename = di.sfilename;
	if (di.sSubdir.GetLength())
		sSubdir = di.sSubdir;
	nsdiffs = di.nsdiffs;
	ndiffs = di.ndiffs;
}

/** @brief Return path to left file, including all but file name */
CString DIFFITEM::getLeftFilepath(CString sLeftRoot) const
{
	CString sPath;
	if (!isSideRight())
	{
		sPath = sLeftRoot;
		if (sSubdir.GetLength())
		{
			sPath = paths_ConcatPath(sPath, sSubdir);
		}
	}
	return sPath;
}

/** @brief Return path to right file, including all but file name */
CString DIFFITEM::getRightFilepath(CString sRightRoot) const
{
	CString sPath;
	if (!isSideLeft())
	{
		sPath = sRightRoot;
		if (sSubdir.GetLength())
		{
			sPath = paths_ConcatPath(sPath, sSubdir);
		}
	}
	return sPath;
}
