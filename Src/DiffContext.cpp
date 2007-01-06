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
#include <shlwapi.h>
#include "Merge.h"
#include "CompareStats.h"
#include "version.h"
#include "DiffContext.h"
#include "paths.h"
#include "coretools.h"
#include "codepage_detect.h"
#include "DiffItemList.h"
#include "IAbortable.h"

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
: m_piFilterGlobal(NULL)
, m_piPluginInfos(NULL)
, m_hDirFrame(NULL)
, m_nCompMethod(-1)
, m_bIgnoreSmallTimeDiff(FALSE)
, m_pCompareStats(NULL)
, m_pList(&m_dirlist)
, m_piAbortable(NULL)
, m_bStopAfterFirstDiff(FALSE)
{
	m_paths.SetLeft(pszLeft);
	m_paths.SetRight(pszRight);
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

	m_paths.SetLeft(pszLeft);
	m_paths.SetRight(pszRight);
	m_pList = src.m_pList;
	m_piFilterGlobal = src.m_piFilterGlobal;
	m_hDirFrame = src.m_hDirFrame;
	m_nCompMethod = src.m_nCompMethod;
	m_bIgnoreSmallTimeDiff = src.m_bIgnoreSmallTimeDiff;
	m_bStopAfterFirstDiff = src.m_bStopAfterFirstDiff;
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
 * @brief Update info in DIFFITEM from disk.
 * @param [in] diffpos DIFFITEM to update.
 * @param [in] bLeft Update left-side info.
 * @param [in] bRight Update right-side info.
 */
void CDiffContext::UpdateStatusFromDisk(POSITION diffpos, BOOL bLeft, BOOL bRight)
{
	DIFFITEM &di = m_pList->GetAt(diffpos);
	if (bLeft)
	{
		di.left.Clear();
		if (!di.isSideRightOnly())
			UpdateInfoFromDiskHalf(di, TRUE);
	}
	if (bRight)
	{
		di.right.Clear();
		if (!di.isSideLeftOnly())
			UpdateInfoFromDiskHalf(di, FALSE);
	}
}

/**
 * @brief Update information from disk for selected side.
 * @param [in, out] di DIFFITEM to update (selected side, see bLeft param).
 * @param [in] bLeft If TRUE left side information is updated,
 *  right side otherwise.
 * @return TRUE if file exists
 */
BOOL CDiffContext::UpdateInfoFromDiskHalf(DIFFITEM & di, BOOL bLeft)
{
	CString filepath
	(
		bLeft == TRUE
	?	paths_ConcatPath(di.getLeftFilepath(GetNormalizedLeft()), di.sLeftFilename)
	:	paths_ConcatPath(di.getRightFilepath(GetNormalizedRight()), di.sRightFilename)
	);
	DiffFileInfo & dfi = bLeft ? di.left : di.right;
	if (!dfi.Update(filepath))
		return FALSE;
	UpdateVersion(di, bLeft);
	GuessCodepageEncoding(filepath, &dfi.encoding, m_bGuessEncoding);
	return TRUE;
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
 * @brief Load file version from disk.
 * Update fileversion for given item and side from disk. Note that versions
 * are read from only some filetypes. See CheckFileForVersion() function
 * for list of files to check versions.
 * @param [in,out] di DIFFITEM to update.
 * @param [in] bLeft If TRUE left-side file is updated, right-side otherwise.
 */
void CDiffContext::UpdateVersion(DIFFITEM & di, BOOL bLeft) const
{
	DiffFileInfo & dfi = bLeft ? di.left : di.right;
	// Check only binary files
	dfi.version = _T("");
	dfi.bVersionChecked = true;

	if (di.isDirectory())
		return;
	
	CString spath;
	if (bLeft)
	{
		if (di.isSideRightOnly())
			return;
		LPCTSTR ext = PathFindExtension(di.sLeftFilename);
		if (!CheckFileForVersion(ext))
			return;
		spath = di.getLeftFilepath(GetNormalizedLeft());
		spath = paths_ConcatPath(spath, di.sLeftFilename);
	}
	else
	{
		if (di.isSideLeftOnly())
			return;
		LPCTSTR ext = PathFindExtension(di.sRightFilename);
		if (!CheckFileForVersion(ext))
			return;
		spath = di.getRightFilepath(GetNormalizedRight());
		spath = paths_ConcatPath(spath, di.sRightFilename);
	}
	dfi.version = GetFixedFileVersion(spath);
}

/** @brief Forward call to retrieve plugin info (winds up in DirDoc) */
void CDiffContext::FetchPluginInfos(const CString& filteredFilenames, PackingInfo ** infoUnpacker, PrediffingInfo ** infoPrediffer)
{
	ASSERT(m_piPluginInfos);
	m_piPluginInfos->FetchPluginInfos(filteredFilenames, infoUnpacker, infoPrediffer);
}

/** @brief Return true only if user has requested abort */
bool CDiffContext::ShouldAbort() const
{
	return m_piAbortable && m_piAbortable->ShouldAbort();
}
