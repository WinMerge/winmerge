/** 
 * @file  FolderCmp.cpp
 *
 * @brief Implementation file for FolderCmp
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "stdafx.h"
#include "DiffUtils.h"
#include "ByteCompare.h"
#include "LogFile.h"
#include "Merge.h"
#include "paths.h"
#include "FilterList.h"
#include "DiffContext.h"
#include "DiffWrapper.h"
#include "FileTransform.h"
#include "diff.h"
#include "IAbortable.h"
#include "FolderCmp.h"
#include "ByteComparator.h"

using namespace CompareEngines;

static void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, CString & left, CString & right);
static bool Unpack(CString & filepathTransformed,
	const CString & filteredFilenames, PackingInfo * infoUnpacker);

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
	CString filteredFilenames = plugCtxt->origFileName1 + "|" + plugCtxt->origFileName2;

	// Get existing or new plugin infos
	pCtxt->FetchPluginInfos(filteredFilenames, &plugCtxt->infoUnpacker,
			&plugCtxt->infoPrediffer);

	// plugin may alter filepaths to temp copies (which we delete before returning in all cases)
	plugCtxt->filepathUnpacked1 = plugCtxt->origFileName1;
	plugCtxt->filepathUnpacked2 = plugCtxt->origFileName2;

	//DiffFileData diffdata; //(filepathTransformed1, filepathTransformed2);
	// Invoke unpacking plugins
	if (!Unpack(plugCtxt->filepathUnpacked1, filteredFilenames, plugCtxt->infoUnpacker))
	{
		errStr = _T("Unpack Error Side 1");
		return false;
	}

	// we use the same plugins for both files, so they must be defined before second file
	ASSERT(plugCtxt->infoUnpacker->bToBeScanned == FALSE);

	if (!Unpack(plugCtxt->filepathUnpacked2, filteredFilenames, plugCtxt->infoUnpacker))
	{
		errStr = _T("Unpack Error Side 2");
		return false;
	}

	// As we keep handles open on unpacked files, Transform() may not delete them.
	// Unpacked files will be deleted at end of this function.
	plugCtxt->filepathTransformed1 = plugCtxt->filepathUnpacked1;
	plugCtxt->filepathTransformed2 = plugCtxt->filepathUnpacked2;
	m_diffFileData.SetDisplayFilepaths(plugCtxt->origFileName1,
			plugCtxt->origFileName2); // store true names for diff utils patch file
	if (!m_diffFileData.OpenFiles(plugCtxt->filepathTransformed1,
			plugCtxt->filepathTransformed2))
	{
		errStr = _T("OpenFiles Error (before tranform)");
		return false;
	}

	// Invoke prediff'ing plugins
	if (!m_diffFileData.Filepath_Transform(m_diffFileData.m_FileLocation[0],
			plugCtxt->filepathUnpacked1, plugCtxt->filepathTransformed1,
			filteredFilenames, plugCtxt->infoPrediffer,
			m_diffFileData.m_inf[0].desc))
	{
		errStr = _T("Transform Error Side 1");
		return false;
	}

	// we use the same plugins for both files, so they must be defined before second file
	ASSERT(plugCtxt->infoPrediffer->bToBeScanned == FALSE);

	if (!m_diffFileData.Filepath_Transform(m_diffFileData.m_FileLocation[1],
			plugCtxt->filepathUnpacked2, plugCtxt->filepathTransformed2,
			filteredFilenames, plugCtxt->infoPrediffer,
			m_diffFileData.m_inf[1].desc))
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
		if (!m_diffFileData.OpenFiles(plugCtxt->filepathTransformed1,
				plugCtxt->filepathTransformed2))
		{
			errStr = _T("OpenFiles Error (after tranform)");
			return false;
		}
	}
	return true;
}

void FolderCmp::CleanupAfterPlugins(PluginsContext *plugCtxt)
{
	m_diffFileData.Reset();
	// delete the temp files after comparison
	if (plugCtxt->filepathTransformed1 != plugCtxt->filepathUnpacked1)
		VERIFY(::DeleteFile(plugCtxt->filepathTransformed1) ||
				GetLog()->DeleteFileFailed(plugCtxt->filepathTransformed1));
	if (plugCtxt->filepathTransformed2 != plugCtxt->filepathUnpacked2)
		VERIFY(::DeleteFile(plugCtxt->filepathTransformed2) ||
				GetLog()->DeleteFileFailed(plugCtxt->filepathTransformed2));
	if (plugCtxt->filepathUnpacked1 != plugCtxt->origFileName1)
		VERIFY(::DeleteFile(plugCtxt->filepathUnpacked1) ||
				GetLog()->DeleteFileFailed(plugCtxt->filepathUnpacked1));
	if (plugCtxt->filepathUnpacked2 != plugCtxt->origFileName2)
		VERIFY(::DeleteFile(plugCtxt->filepathUnpacked2) ||
				GetLog()->DeleteFileFailed(plugCtxt->filepathUnpacked2));
}

/**
 * @brief Prepare files (run plugins) & compare them, and return diffcode.
 * This is function to compare two files in folder compare. It is not used in
 * file compare.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in, out] di Compared files with associated data.
 * @return Compare result code.
 */
int FolderCmp::prepAndCompareTwoFiles(CDiffContext * pCtxt, DIFFITEM &di)
{
	PluginsContext plugCtxt;
	int nCompMethod = pCtxt->m_nCompMethod;
	m_pCtx = pCtxt;

	// Reset text stats
	m_diffFileData.m_textStats0.clear();
	m_diffFileData.m_textStats1.clear();

	int code = DIFFCODE::FILE | DIFFCODE::CMPERR;

	// Run plugins
	if (pCtxt->m_nCompMethod == CMP_CONTENT ||
		pCtxt->m_nCompMethod == CMP_QUICK_CONTENT)
	{
		GetComparePaths(pCtxt, di, plugCtxt.origFileName1, plugCtxt.origFileName2);
		CString errStr;
		bool pluginsOk = RunPlugins(pCtxt, &plugCtxt, errStr);
		if (!pluginsOk)
		{
			di.errorDesc = errStr;
			CleanupAfterPlugins(&plugCtxt);
			return code;
		}
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

		if (!DIFFCODE::isResultError(code) && pCtxt->m_bGuessEncoding)
		{
			// entire file is in memory in the diffutils buffers
			// inside the diff context, so may as well use in-memory copy
			m_diffFileData.GuessEncoding_from_buffer_in_DiffContext(0, pCtxt);
			m_diffFileData.GuessEncoding_from_buffer_in_DiffContext(1, pCtxt);
		}
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
			m_pByteCompare->SetPaths(m_diffFileData.m_FileLocation[0].filepath,
				m_diffFileData.m_FileLocation[1].filepath);
			m_pByteCompare->SetFileData(2, m_diffFileData.m_inf);

			// Close any descriptors open for diffutils
			m_diffFileData.Reset();
			// use our own byte-by-byte compare
			code = m_pByteCompare->CompareFiles();

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

		if (!DIFFCODE::isResultError(code) && pCtxt->m_bGuessEncoding)
		{
			m_diffFileData.GuessEncoding_from_FileLocation(m_diffFileData.m_FileLocation[0]);
			m_diffFileData.GuessEncoding_from_FileLocation(m_diffFileData.m_FileLocation[1]);
		}
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
				code = DIFFCODE::TEXT | DIFFCODE::SAME;
			else
				code = DIFFCODE::TEXT | DIFFCODE::DIFF;
		}
		else
		{
			// Filetimes for item(s) could not be read. So we have to
			// set error status, unless we have DATE_SIZE -compare
			// when we have still hope for size compare..
			if (pCtxt->m_nCompMethod == CMP_DATE_SIZE)
				code = DIFFCODE::TEXT | DIFFCODE::SAME;
			else
				code = DIFFCODE::TEXT | DIFFCODE::CMPERR;
		}
		
		// This is actual CMP_DATE_SIZE method..
		// If file sizes differ mark them different
		if (pCtxt->m_nCompMethod == CMP_DATE_SIZE && di.diffcode.isResultSame())
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

	if (pCtxt->m_nCompMethod == CMP_CONTENT ||
		pCtxt->m_nCompMethod == CMP_QUICK_CONTENT)
	{
		CleanupAfterPlugins(&plugCtxt);
	}

	return code;
}


/**
 * @brief Get actual compared paths from DIFFITEM.
 * @note If item is unique, same path is returned for both.
 */
void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, CString & left, CString & right)
{
	static const TCHAR backslash[] = _T("\\");

	if (!di.diffcode.isSideRightOnly())
	{
		// Compare file to itself to detect encoding
		left = pCtxt->GetNormalizedLeft();
		if (!paths_EndsWithSlash(left))
			left += backslash;
		if (!di.sLeftSubdir.IsEmpty())
			left += di.sLeftSubdir + backslash;
		left += di.sLeftFilename;
		if (di.diffcode.isSideLeftOnly())
			right = left;
	}
	if (!di.diffcode.isSideLeftOnly())
	{
		// Compare file to itself to detect encoding
		right = pCtxt->GetNormalizedRight();
		if (!paths_EndsWithSlash(right))
			right += backslash;
		if (!di.sRightSubdir.IsEmpty())
			right += di.sRightSubdir + backslash;
		right += di.sRightFilename;
		if (di.diffcode.isSideRightOnly())
			left = right;
	}
}

/**
 * @brief Invoke appropriate plugins for unpacking
 * return false if anything fails
 * caller has to DeleteFile filepathTransformed, if it differs from filepath
 */
static bool Unpack(CString & filepathTransformed,
	const CString & filteredFilenames, PackingInfo * infoUnpacker)
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
