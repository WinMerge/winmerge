/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file DiffContext.cpp
 *
 *  @brief Implementation of CDiffContext
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "merge.h"
#include "CompareStats.h"
#include "version.h"
#include "DiffContext.h"
#include "paths.h"
#include "coretools.h"
#include "codepage_detect.h"
#include "DiffItemList.h"
#include <shlwapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * @brief Construct CDiffContext.
 *
 * @param [in] pszLeft Initial left-side path.
 * @param [in] pszRight Initial right-side path.
 */
CDiffContext::CDiffContext(LPCTSTR pszLeft /*=NULL*/, LPCTSTR pszRight /*=NULL*/)
: m_bRecurse(FALSE)
, m_piFilterGlobal(NULL)
, m_piPluginInfos(NULL)
, m_msgUpdateStatus(0)
, m_hDirFrame(NULL)
, m_nCompMethod(-1)
, m_bIgnoreSmallTimeDiff(FALSE)
{
	m_paths.SetLeft(pszLeft);
	m_paths.SetRight(pszRight);
	m_pList = &m_dirlist;
}

/**
 * @brief Construct copy of existing CDiffContext.
 *
 * @param [in] pszLeft Initial left-side path.
 * @param [in] pszRight Initial right-side path.
 * @param [in] src Existing CDiffContext whose data is copied.
 */
CDiffContext::CDiffContext(LPCTSTR pszLeft, LPCTSTR pszRight, CDiffContext& src)
{
	// This is used somehow in recursive comparisons
	// I think that it is only used during rescan to copy into temporaries
	// which modify the original (because pointers are copied below)
	// and then the temporary goes away
	// so the temporary never exists while the user is interacting with the GUI

	m_bRecurse=src.m_bRecurse;
	m_paths.SetLeft(pszLeft);
	m_paths.SetRight(pszRight);
	m_pList = src.m_pList;
	m_piFilterGlobal = src.m_piFilterGlobal;
	m_msgUpdateStatus = src.m_msgUpdateStatus;
	m_hDirFrame = src.m_hDirFrame;
	m_nCompMethod = src.m_nCompMethod;
	m_bIgnoreSmallTimeDiff = src.m_bIgnoreSmallTimeDiff;
}

/**
 * @brief Fetch & return the fixed file version as a dotted string
 */
static CString GetFixedFileVersion(const CString & path)
{
	CVersionInfo ver(path);
	return ver.GetFixedFileVersion();
}

/**
 * @brief Add new diffitem to CDiffContext array
 */
void CDiffContext::AddDiff(const DIFFITEM & di)
{
	DiffItemList::AddDiff(di);
	m_pCompareStats->AddItem(di.diffcode);
}

/**
 * @brief Update info in list (specified by position) from disk
 * @param [in] diffpos Difference to update
 * @param [in] bLeft Update left-side item
 * @param [in] bRight Update right-side item
 */
void CDiffContext::UpdateStatusFromDisk(POSITION diffpos, BOOL bLeft, BOOL bRight)
{
	DIFFITEM &di = m_pList->GetAt(diffpos);
	if (bLeft)
	{
		di.left.Clear();
		if (!di.isSideRight())
			UpdateInfoFromDiskHalf(di, di.left);
	}
	if (bRight)
	{
		di.right.Clear();
		if (!di.isSideLeft())
			UpdateInfoFromDiskHalf(di, di.right);
	}
}

/**
 * @brief Update information from disk (for one side)
 */
void CDiffContext::UpdateInfoFromDiskHalf(DIFFITEM & di, DiffFileInfo & dfi)
{
	UpdateVersion(di, dfi);
	ASSERT(&dfi == &di.left || &dfi == &di.right);
	CString spath = &dfi == &di.left ? di.getLeftFilepath(GetNormalizedLeft()) :
			di.getRightFilepath(GetNormalizedRight());
	CString filepath = paths_ConcatPath(spath, di.sfilename);
	dfi.Update(filepath);
	GuessCodepageEncoding(filepath, &dfi.unicoding, &dfi.codepage, m_bGuessEncoding);
}

/**
 * @brief Return if this extension is one we expect to have a file version
 */
static bool CheckFileForVersion(LPCTSTR ext)
{
	if (!lstrcmpi(ext, _T(".EXE")) || !lstrcmpi(ext, _T(".DLL")) || !lstrcmpi(ext, _T(".SYS")) ||
	    !lstrcmpi(ext, _T(".DRV")) || !lstrcmpi(ext, _T(".OCX")) || !lstrcmpi(ext, _T(".CPL")) ||
	    !lstrcmpi(ext, _T(".SCR")) || !lstrcmpi(ext, _T(".LANG")))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief Load file versions from disk
 */
void CDiffContext::UpdateVersion(DIFFITEM & di, DiffFileInfo & dfi) const
{
	// Check only binary files
	LPCTSTR ext = PathFindExtension(di.sfilename);
	dfi.version = _T("");
	dfi.bVersionChecked = true;

	if (di.isDirectory())
		return;
	
	if (!CheckFileForVersion(ext))
		return;

	CString spath;
	if (&dfi == &di.left)
	{
		if (di.isSideRight())
			return;
		spath = di.getLeftFilepath(GetNormalizedLeft());
	}
	else
	{
		ASSERT(&dfi == &di.right);
		if (di.isSideLeft())
			return;
		spath = di.getRightFilepath(GetNormalizedRight());
	}
	CString filepath = paths_ConcatPath(spath, di.sfilename);
	dfi.version = GetFixedFileVersion(filepath);
}

/** @brief Forward call to retrieve plugin info (winds up in DirDoc) */
void CDiffContext::FetchPluginInfos(const CString& filteredFilenames, PackingInfo ** infoUnpacker, PrediffingInfo ** infoPrediffer)
{
	ASSERT(m_piPluginInfos);
	m_piPluginInfos->FetchPluginInfos(filteredFilenames, infoUnpacker, infoPrediffer);
}
