/** 
 * @file  FolderCmp.cpp
 *
 * @brief Implementation file for FolderCmp
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"
#include "DiffUtils.h"
#include "ByteCompare.h"
#include "LogFile.h"
#include "Merge.h"
#include "paths.h"
#include "FilterList.h"
#include "DiffContext.h"
#include "DiffWrapper.h"
#include "FileTransform.h"
#include "DIFF.H"
#include "IAbortable.h"
#include "FolderCmp.h"
#include "ByteComparator.h"
#include "codepage_detect.h"

using CompareEngines::ByteCompare;

static void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, String & left, String & right);
static bool Unpack(String & filepathTransformed,
	LPCTSTR filteredFilenames, PackingInfo * infoUnpacker);

FolderCmp::FolderCmp()
: m_pDiffUtilsEngine(NULL)
, m_pByteCompare(NULL)
, m_ndiffs(CDiffContext::DIFFS_UNKNOWN)
, m_ntrivialdiffs(CDiffContext::DIFFS_UNKNOWN)
{
}

FolderCmp::~FolderCmp()
{
	delete m_pDiffUtilsEngine;
	delete m_pByteCompare;
}

bool FolderCmp::RunPlugins(CDiffContext * pCtxt, PluginsContext * plugCtxt, CString &errStr)
{
	// For user chosen plugins, define bAutomaticUnpacker as false and use the chosen infoHandler
	// but how can we receive the infoHandler ? DirScan actually only 
	// returns info, but can not use file dependent information.

	// Transformation happens here
	// text used for automatic mode : plugin filter must match it
	String filteredFilenames(plugCtxt->origFileName1);
	filteredFilenames += '|';
	filteredFilenames += plugCtxt->origFileName2;

	// Get existing or new plugin infos
	pCtxt->FetchPluginInfos(filteredFilenames.c_str(), &plugCtxt->infoUnpacker,
			&plugCtxt->infoPrediffer);

	// plugin may alter filepaths to temp copies (which we delete before returning in all cases)
	plugCtxt->filepathUnpacked1 = plugCtxt->origFileName1;
	plugCtxt->filepathUnpacked2 = plugCtxt->origFileName2;

	//DiffFileData diffdata; //(filepathTransformed1, filepathTransformed2);
	// Invoke unpacking plugins
	if (!Unpack(plugCtxt->filepathUnpacked1, filteredFilenames.c_str(), plugCtxt->infoUnpacker))
	{
		errStr = _T("Unpack Error Side 1");
		return false;
	}

	// we use the same plugins for both files, so they must be defined before second file
	ASSERT(plugCtxt->infoUnpacker->bToBeScanned == FALSE);

	if (!Unpack(plugCtxt->filepathUnpacked2, filteredFilenames.c_str(), plugCtxt->infoUnpacker))
	{
		errStr = _T("Unpack Error Side 2");
		return false;
	}

	// As we keep handles open on unpacked files, Transform() may not delete them.
	// Unpacked files will be deleted at end of this function.
	plugCtxt->filepathTransformed1 = plugCtxt->filepathUnpacked1;
	plugCtxt->filepathTransformed2 = plugCtxt->filepathUnpacked2;
	if (!m_diffFileData.OpenFiles(plugCtxt->filepathTransformed1.c_str(),
			plugCtxt->filepathTransformed2.c_str()))
	{
		errStr = _T("OpenFiles Error (before tranform)");
		return false;
	}

	// Invoke prediff'ing plugins
	if (!m_diffFileData.Filepath_Transform(m_diffFileData.m_FileLocation[0],
			plugCtxt->filepathUnpacked1, plugCtxt->filepathTransformed1,
			filteredFilenames.c_str(), plugCtxt->infoPrediffer))
	{
		errStr = _T("Transform Error Side 1");
		return false;
	}

	// we use the same plugins for both files, so they must be defined before second file
	ASSERT(plugCtxt->infoPrediffer->bToBeScanned == FALSE);

	if (!m_diffFileData.Filepath_Transform(m_diffFileData.m_FileLocation[1],
			plugCtxt->filepathUnpacked2, plugCtxt->filepathTransformed2,
			filteredFilenames.c_str(), plugCtxt->infoPrediffer))
	{
		errStr = _T("Transform Error Side 2");
		return false;
	}

	// If options are binary equivalent, we could check for filesize
	// difference here, and bail out if files are clearly different
	// But, then we don't know if file is ascii or binary, and this
	// affects behavior (also, we don't have an icon for unknown type)

	// Actually compare the files
	// diffutils_compare_files is a fairly thin front-end to diffutils
	if (plugCtxt->filepathTransformed1 != plugCtxt->filepathUnpacked1 ||
			plugCtxt->filepathTransformed2 != plugCtxt->filepathUnpacked2)
	{
		if (!m_diffFileData.OpenFiles(plugCtxt->filepathTransformed1.c_str(),
				plugCtxt->filepathTransformed2.c_str()))
		{
			errStr = _T("OpenFiles Error (after tranform)");
			return false;
		}
	}
	return true;
}

void FolderCmp::CleanupAfterPlugins(PluginsContext *plugCtxt)
{
	// delete the temp files after comparison
	if (plugCtxt->filepathTransformed1 != plugCtxt->filepathUnpacked1)
		VERIFY(::DeleteFile(plugCtxt->filepathTransformed1.c_str()) ||
				GetLog()->DeleteFileFailed(plugCtxt->filepathTransformed1.c_str()));
	if (plugCtxt->filepathTransformed2 != plugCtxt->filepathUnpacked2)
		VERIFY(::DeleteFile(plugCtxt->filepathTransformed2.c_str()) ||
				GetLog()->DeleteFileFailed(plugCtxt->filepathTransformed2.c_str()));
	if (plugCtxt->filepathUnpacked1 != plugCtxt->origFileName1)
		VERIFY(::DeleteFile(plugCtxt->filepathUnpacked1.c_str()) ||
				GetLog()->DeleteFileFailed(plugCtxt->filepathUnpacked1.c_str()));
	if (plugCtxt->filepathUnpacked2 != plugCtxt->origFileName2)
		VERIFY(::DeleteFile(plugCtxt->filepathUnpacked2.c_str()) ||
				GetLog()->DeleteFileFailed(plugCtxt->filepathUnpacked2.c_str()));
}

/**
 * @brief Prepare files (run plugins) & compare them, and return diffcode.
 * This is function to compare two files in folder compare. It is not used in
 * file compare.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in, out] di Compared files with associated data.
 * @return Compare result code.
 */
UINT FolderCmp::prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di)
{
	PluginsContext plugCtxt;
	int nCompMethod = pCtxt->m_nCompMethod;
	const int origCompMethod = pCtxt->m_nCompMethod;
	m_pCtx = pCtxt;

	// Reset text stats
	m_diffFileData.m_textStats0.clear();
	m_diffFileData.m_textStats1.clear();

	UINT code = DIFFCODE::FILE | DIFFCODE::CMPERR;

	if (pCtxt->m_nCompMethod == CMP_CONTENT ||
		pCtxt->m_nCompMethod == CMP_QUICK_CONTENT)
	{
		GetComparePaths(pCtxt, di, plugCtxt.origFileName1, plugCtxt.origFileName2);
		m_diffFileData.SetDisplayFilepaths(plugCtxt.origFileName1.c_str(),
				plugCtxt.origFileName2.c_str()); // store true names for diff utils patch file
	
		// Run plugins
		if (pCtxt->m_bPluginsEnabled)
		{
			CString errStr;
			bool pluginsOk = RunPlugins(pCtxt, &plugCtxt, errStr);
			if (!pluginsOk)
			{
				di.errorDesc = errStr;
				CleanupAfterPlugins(&plugCtxt);
				return code;
			}
		}
		else
		{
			if (!m_diffFileData.OpenFiles(plugCtxt.origFileName1.c_str(),
					plugCtxt.origFileName2.c_str()))
			{
				di.errorDesc = _T("Error opening compared files");
				return false;
			}
		}

		GuessCodepageEncoding(m_diffFileData.m_FileLocation[0].filepath.c_str(), 
				&m_diffFileData.m_FileLocation[0].encoding, pCtxt->m_bGuessEncoding);
		GuessCodepageEncoding(m_diffFileData.m_FileLocation[1].filepath.c_str(), 
				&m_diffFileData.m_FileLocation[1].encoding, pCtxt->m_bGuessEncoding);
	}


	// If either file is larger than limit compare files by quick contents
	// This allows us to (faster) compare big binary files
	if (pCtxt->m_nCompMethod == CMP_CONTENT && 
		(di.left.size > pCtxt->m_nQuickCompareLimit ||
		di.right.size > pCtxt->m_nQuickCompareLimit))
	{
		nCompMethod = CMP_QUICK_CONTENT;
	}

	if (nCompMethod == CMP_CONTENT)
	{
		if (m_pDiffUtilsEngine == NULL)
			m_pDiffUtilsEngine = new CompareEngines::DiffUtils();
		m_pDiffUtilsEngine->SetCodepage(
			m_diffFileData.m_FileLocation[0].encoding.m_unicoding ? 
				CP_UTF8 : m_diffFileData.m_FileLocation[0].encoding.m_codepage);
		bool success = m_pDiffUtilsEngine->SetCompareOptions(
				*m_pCtx->GetCompareOptions(CMP_CONTENT));
		if (success)
		{
			if (m_pCtx->m_pFilterList != NULL)
                m_pDiffUtilsEngine->SetFilterList(m_pCtx->m_pFilterList);
			else
				m_pDiffUtilsEngine->ClearFilterList();
			m_pDiffUtilsEngine->SetFileData(2, m_diffFileData.m_inf);
			code = m_pDiffUtilsEngine->diffutils_compare_files();
			m_pDiffUtilsEngine->GetDiffCounts(m_ndiffs, m_ntrivialdiffs);
			m_pDiffUtilsEngine->GetTextStats(0, &m_diffFileData.m_textStats0);
			m_pDiffUtilsEngine->GetTextStats(1, &m_diffFileData.m_textStats1);
		}
		else
			code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;

		// If unique item, it was being compared to itself to determine encoding
		// and the #diffs is invalid
		if (di.diffcode.isSideRightOnly() || di.diffcode.isSideLeftOnly())
		{
			m_ndiffs = CDiffContext::DIFFS_UNKNOWN;
			m_ntrivialdiffs = CDiffContext::DIFFS_UNKNOWN;
		}
		if (DIFFCODE::isResultError(code))
			di.errorDesc = _T("DiffUtils Error");
	}
	else if (nCompMethod == CMP_QUICK_CONTENT)
	{
		if (m_pByteCompare == NULL)
			m_pByteCompare = new ByteCompare();
		bool success = m_pByteCompare->SetCompareOptions(
			*m_pCtx->GetCompareOptions(CMP_QUICK_CONTENT));

		if (success)
		{
			m_pByteCompare->SetAdditionalOptions(pCtxt->m_bStopAfterFirstDiff);
			m_pByteCompare->SetAbortable(pCtxt->GetAbortable());
			m_pByteCompare->SetFileData(2, m_diffFileData.m_inf);

			// use our own byte-by-byte compare
			code = m_pByteCompare->CompareFiles(m_diffFileData.m_FileLocation);

			m_pByteCompare->GetTextStats(0, &m_diffFileData.m_textStats0);
			m_pByteCompare->GetTextStats(1, &m_diffFileData.m_textStats1);
		}
		else
			code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;

		// Quick contents doesn't know about diff counts
		// Set to special value to indicate invalid
		m_ndiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
		m_ntrivialdiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
		di.left.m_textStats = m_diffFileData.m_textStats0;
		di.right.m_textStats = m_diffFileData.m_textStats1;
	}
	else if (pCtxt->m_nCompMethod == CMP_DATE ||
		pCtxt->m_nCompMethod == CMP_DATE_SIZE)
	{
		// Compare by modified date
		// Check that we have both filetimes
		if (di.left.mtime != 0 && di.right.mtime != 0)
		{
			__int64 nTimeDiff = di.left.mtime - di.right.mtime;
			// Remove sign
			nTimeDiff = (nTimeDiff > 0 ? nTimeDiff : -nTimeDiff);
			if (pCtxt->m_bIgnoreSmallTimeDiff)
			{
				// If option to ignore small timediffs (couple of seconds)
				// is set, decrease absolute difference by allowed diff
				nTimeDiff -= SmallTimeDiff;
			}
			if (nTimeDiff <= 0)
				code = DIFFCODE::SAME;
			else
				code = DIFFCODE::DIFF;
		}
		else
		{
			// Filetimes for item(s) could not be read. So we have to
			// set error status, unless we have DATE_SIZE -compare
			// when we have still hope for size compare..
			if (pCtxt->m_nCompMethod == CMP_DATE_SIZE)
				code = DIFFCODE::SAME;
			else
				code = DIFFCODE::CMPERR;
		}
		
		// This is actual CMP_DATE_SIZE method..
		// If file sizes differ mark them different
		if (pCtxt->m_nCompMethod == CMP_DATE_SIZE)
		{
			if (di.left.size != di.right.size)
			{
				code &= ~DIFFCODE::SAME;
				code = DIFFCODE::DIFF;
			}
		}
	}
	else if (pCtxt->m_nCompMethod == CMP_SIZE)
	{
		// Compare by size
		if (di.left.size == di.right.size)
			code = DIFFCODE::SAME;
		else
			code = DIFFCODE::DIFF;
	}
	else
	{
		// Print error since we should have handled by date compare earlier
		_RPTF0(_CRT_ERROR, "Invalid compare type, DiffFileData can't handle it");
		di.errorDesc = _T("Bad compare type");
	}

	// Reset the compare method to original (could have been changed)
	m_pCtx->GetCompareOptions(origCompMethod);
	m_diffFileData.Reset();
	if (pCtxt->m_bPluginsEnabled && (pCtxt->m_nCompMethod == CMP_CONTENT ||
		pCtxt->m_nCompMethod == CMP_QUICK_CONTENT))
	{
		CleanupAfterPlugins(&plugCtxt);
	}

	return code;
}


/**
 * @brief Get actual compared paths from DIFFITEM.
 * @note If item is unique, same path is returned for both.
 */
void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, String & left, String & right)
{
	static const TCHAR backslash[] = _T("\\");

	if (!di.diffcode.isSideRightOnly())
	{
		// Compare file to itself to detect encoding
		left = pCtxt->GetNormalizedLeft();
		if (!paths_EndsWithSlash(left.c_str()))
			left += backslash;
		if (!di.left.path.empty())
			left += di.left.path + backslash;
		left += di.left.filename;
		if (di.diffcode.isSideLeftOnly())
			right = left;
	}
	if (!di.diffcode.isSideLeftOnly())
	{
		// Compare file to itself to detect encoding
		right = pCtxt->GetNormalizedRight();
		if (!paths_EndsWithSlash(right.c_str()))
			right += backslash;
		if (!di.right.path.empty())
			right += di.right.path + backslash;
		right += di.right.filename;
		if (di.diffcode.isSideRightOnly())
			left = right;
	}
}

/**
 * @brief Invoke appropriate plugins for unpacking
 * return false if anything fails
 * caller has to DeleteFile filepathTransformed, if it differs from filepath
 */
static bool Unpack(String & filepathTransformed,
	LPCTSTR filteredFilenames, PackingInfo * infoUnpacker)
{
	// first step : unpack (plugins)
	if (infoUnpacker->bToBeScanned)
	{
		if (!FileTransform_Unpacking(filepathTransformed, filteredFilenames, infoUnpacker, &infoUnpacker->subcode))
			return false;
	}
	else
	{
		if (!FileTransform_Unpacking(filepathTransformed, infoUnpacker, &infoUnpacker->subcode))
			return false;
	}
	return true;
}
