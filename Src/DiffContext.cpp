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

#include "pch.h"
#include "DiffContext.h"
#include <Poco/ScopedLock.h>
#include "CompareOptions.h"
#include "VersionInfo.h"
#include "paths.h"
#include "codepage_detect.h"
#include "DiffItemList.h"
#include "IAbortable.h"
#include "DiffWrapper.h"
#include "DebugNew.h"

using Poco::FastMutex;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * @brief Construct CDiffContext.
 *
 * @param [in] pszLeft Initial left-side path.
 * @param [in] pszRight Initial right-side path.
 * @param [in] compareMethod Main compare method for this compare.
 */
CDiffContext::CDiffContext(const PathContext & paths, int compareMethod)
: m_piFilterGlobal(nullptr)
, m_piPluginInfos(nullptr)
, m_nCompMethod(compareMethod)
, m_bIgnoreSmallTimeDiff(false)
, m_pCompareStats(nullptr)
, m_piAbortable(nullptr)
, m_bStopAfterFirstDiff(false)
, m_pFilterList(nullptr)
, m_pContentCompareOptions(nullptr)
, m_pQuickCompareOptions(nullptr)
, m_pOptions(nullptr)
, m_bPluginsEnabled(false)
, m_bRecursive(false)
, m_bWalkUniques(true)
, m_bIgnoreReparsePoints(false)
, m_bIgnoreCodepage(false)
, m_iGuessEncodingType(0)
, m_nQuickCompareLimit(0)
, m_nBinaryCompareLimit(0)
, m_pFilterCommentsManager(nullptr)
{
	int index;
	for (index = 0; index < paths.GetSize(); index++)
		m_paths.SetPath(index, paths[index]);
}

/**
 * @brief Destructor.
 */
CDiffContext::~CDiffContext()
{
}

/**
 * @brief Update info in item in result list from disk.
 * This function updates result list item's file information from actual
 * file in the disk. This updates info like date, size and attributes.
 * @param [in] diffpos DIFFITEM to update.
 * @param [in] nIndex index to update 
 */
void CDiffContext::UpdateStatusFromDisk(DIFFITEM *diffpos, int nIndex)
{
	DIFFITEM &di = GetDiffRefAt(diffpos);
	di.diffFileInfo[nIndex].ClearPartial();
	if (di.diffcode.exists(nIndex))
		UpdateInfoFromDiskHalf(di, nIndex);
}

/**
 * @brief Update file information from disk for DIFFITEM.
 * This function updates DIFFITEM's file information from actual file in
 * the disk. This updates info like date, size and attributes.
 * @param [in, out] di DIFFITEM to update.
 * @param [in] nIndex index to update
 * @return true if file exists
 */
bool CDiffContext::UpdateInfoFromDiskHalf(DIFFITEM &di, int nIndex)
{
	String filepath = paths::ConcatPath(paths::ConcatPath(m_paths[nIndex], di.diffFileInfo[nIndex].path), di.diffFileInfo[nIndex].filename);
	DiffFileInfo & dfi = di.diffFileInfo[nIndex];
	if (!dfi.Update(filepath))
		return false;
	UpdateVersion(di, nIndex);
	dfi.encoding = GuessCodepageEncoding(filepath, m_iGuessEncodingType);
	return true;
}

/**
 * @brief Determine if file is one to have a version information.
 * This function determines if the given file has a version information
 * attached into it in resource. This is done by comparing file extension to
 * list of known filename extensions usually to have a version information.
 * @param [in] ext Extension to check.
 * @return true if extension has version info, false otherwise.
 */
static bool CheckFileForVersion(const String& ext)
{
	String lower_ext = strutils::makelower(ext);
	if (lower_ext == _T(".exe") || lower_ext == _T(".dll") || lower_ext == _T(".sys") ||
	    lower_ext == _T(".drv") || lower_ext == _T(".ocx") || lower_ext == _T(".cpl") ||
	    lower_ext == _T(".scr") || lower_ext == _T(".lang"))
	{
		return true;
	}
	return false;
}

/**
 * @brief Load file version from disk.
 * Update fileversion for given item and side from disk. Note that versions
 * are read from only some filetypes. See CheckFileForVersion() function
 * for list of files to check versions.
 * @param [in,out] di DIFFITEM to update.
 * @param [in] bLeft If true left-side file is updated, right-side otherwise.
 */
void CDiffContext::UpdateVersion(DIFFITEM &di, int nIndex) const
{
	DiffFileInfo & dfi = di.diffFileInfo[nIndex];
	// Check only binary files
	dfi.version.SetFileVersionNone();

	if (di.diffcode.isDirectory())
		return;
	
	String spath;
	if (!di.diffcode.exists(nIndex))
		return;
	String ext = paths::FindExtension(di.diffFileInfo[nIndex].filename);
	if (!CheckFileForVersion(ext))
		return;
	spath = di.getFilepath(nIndex, GetNormalizedPath(nIndex));
	spath = paths::ConcatPath(spath, di.diffFileInfo[nIndex].filename);
	
	// Get version info if it exists
	CVersionInfo ver(spath.c_str());
	unsigned verMS = 0;
	unsigned verLS = 0;
	if (ver.GetFixedFileVersion(verMS, verLS))
		dfi.version.SetFileVersion(verMS, verLS);
}

/**
 * @brief Create compare-method specific compare options class.
 * This function creates a compare options class that is specific for
 * main compare method. Compare options class is initialized from
 * given set of options.
 * @param [in] compareMethod Selected compare method.
 * @param [in] options Initial set of compare options.
 * @return true if creation succeeds.
 */
bool CDiffContext::CreateCompareOptions(int compareMethod, const DIFFOPTIONS & options)
{
	m_pContentCompareOptions.reset();
	m_pQuickCompareOptions.reset();
	m_pOptions.reset(new DIFFOPTIONS);
	if (m_pOptions != nullptr)
		std::memcpy(m_pOptions.get(), &options, sizeof(DIFFOPTIONS));
	else
		return false;

	m_nCompMethod = compareMethod;
	if (GetCompareOptions(m_nCompMethod) == nullptr)
	{
		// For Date and Date+Size compare `nullptr` is ok since they don't have actual
		// compare options.
		if (m_nCompMethod == CMP_DATE || m_nCompMethod == CMP_DATE_SIZE ||
			m_nCompMethod == CMP_SIZE)
		{
			return true;
		}
		else
			return false;
	}
	return true;
}

/**
 * @brief Get compare-type specific compare options.
 * This function returns per-compare method options. The compare options
 * returned are converted from general options to match options for specific
 * comapare type. Not all compare options in general set are available for
 * some other compare type. And some options can have different values.
 * @param [in] compareMethod Compare method used.
 * @return Compare options class.
 */
CompareOptions * CDiffContext::GetCompareOptions(int compareMethod)
{
	FastMutex::ScopedLock lock(m_mutex);
	CompareOptions *pCompareOptions = nullptr;

	// Otherwise we have to create new options
	switch (compareMethod)
	{
	case CMP_CONTENT:
		if (m_pContentCompareOptions != nullptr)
			return m_pContentCompareOptions.get();
		m_pContentCompareOptions.reset(pCompareOptions = new DiffutilsOptions());
		break;

	case CMP_QUICK_CONTENT:
		if (m_pQuickCompareOptions != nullptr)
			return m_pQuickCompareOptions.get();
		m_pQuickCompareOptions.reset(pCompareOptions = new QuickCompareOptions());
		break;
	}


	if (pCompareOptions != nullptr)
		pCompareOptions->SetFromDiffOptions(*m_pOptions);

	return pCompareOptions;
}

/** @brief Forward call to retrieve plugin info (winds up in DirDoc) */
void CDiffContext::FetchPluginInfos(const String& filteredFilenames,
		PackingInfo ** infoUnpacker, PrediffingInfo ** infoPrediffer)
{
	assert(m_piPluginInfos != nullptr);
	m_piPluginInfos->FetchPluginInfos(filteredFilenames, infoUnpacker, infoPrediffer);
}

/**
 * @brief Check if user has requested aborting the compare.
 * @return true if user has requested abort, false otherwise.
 */
bool CDiffContext::ShouldAbort() const
{
	return m_piAbortable!=nullptr && m_piAbortable->ShouldAbort();
}
