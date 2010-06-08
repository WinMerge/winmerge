/** 
 * @file  FolderCmp.cpp
 *
 * @brief Implementation file for FolderCmp
 */
// ID line follows -- this is updated by SVN
// $Id: FolderCmp.cpp 7024 2009-10-22 18:26:45Z kimmov $

#include "StdAfx.h"
#include <assert.h>
#include "diff.h"
#include "DiffUtils.h"
#include "ByteCompare.h"
#include "LogFile.h"
#include "paths.h"
#include "FilterList.h"
#include "DiffContext.h"
#include "DiffList.h"
#include "DiffWrapper.h"
#include "FileTransform.h"
#include "IAbortable.h"
#include "FolderCmp.h"
#include "ByteComparator.h"
#include "codepage_detect.h"
#include "unicoder.h"
#include "TimeSizeCompare.h"
#include "Merge.h"

using CompareEngines::ByteCompare;
using CompareEngines::TimeSizeCompare;

static void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, PathContext & files);
static bool Unpack(String & filepathTransformed,
	LPCTSTR filteredFilenames, PackingInfo * infoUnpacker);

FolderCmp::FolderCmp()
: m_pDiffUtilsEngine(NULL)
, m_pByteCompare(NULL)
, m_pTimeSizeCompare(NULL)
, m_ndiffs(CDiffContext::DIFFS_UNKNOWN)
, m_ntrivialdiffs(CDiffContext::DIFFS_UNKNOWN)
, m_codepage(-1)
, m_pCtx(NULL)
{
}

FolderCmp::~FolderCmp()
{
	delete m_pDiffUtilsEngine;
	delete m_pByteCompare;
	delete m_pTimeSizeCompare;
}

bool FolderCmp::RunPlugins(CDiffContext * pCtxt, PluginsContext * plugCtxt, String &errStr)
{
	// FIXME:
	return true;
}

void FolderCmp::CleanupAfterPlugins(PluginsContext *plugCtxt)
{
}

/**
 * @brief Prepare files (run plugins) & compare them, and return diffcode.
 * This is function to compare two files in folder compare. It is not used in
 * file compare.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in, out] di Compared files with associated data.
 * @return Compare result code.
 */
int FolderCmp::prepAndCompareFiles(CDiffContext * pCtxt, DIFFITEM &di)
{
	int nIndex;
	int nDirs = pCtxt->GetCompareDirs();
	int nCompMethod = pCtxt->GetCompareMethod();
	PathContext files;
	GetComparePaths(pCtxt, di, files);
	m_pCtx = pCtxt;

	// Reset text stats
	for (nIndex = 0; nIndex < nDirs; nIndex++)
		m_diffFileData.m_textStats[nIndex].clear();

	UINT code = DIFFCODE::FILE | DIFFCODE::CMPERR;

	struct change *script = NULL;
	struct change *script10 = NULL;
	struct change *script12 = NULL;
	struct change *script02 = NULL;
	FolderCmp diffdata10, diffdata12, diffdata02;
	String filepathUnpacked[3];
	String filepathTransformed[3];

	if (nCompMethod == CMP_CONTENT ||
		nCompMethod == CMP_QUICK_CONTENT)
	{
		// For user chosen plugins, define bAutomaticUnpacker as false and use the chosen infoHandler
		// but how can we receive the infoHandler ? DirScan actually only 
		// returns info, but can not use file dependent information.

		// Transformation happens here
		// text used for automatic mode : plugin filter must match it
		String filteredFilenames = files[0] + _T("|") + files[1];
		if (nDirs == 3)
		{
			filteredFilenames += _T("|");
			filteredFilenames += files[2];
		}

		PackingInfo * infoUnpacker=0;
		PrediffingInfo * infoPrediffer=0;

		// Get existing or new plugin infos
		pCtxt->FetchPluginInfos(filteredFilenames.c_str(), &infoUnpacker,
				&infoPrediffer);

		FileTextEncoding encoding[3];
		BOOL bForceUTF8 = FALSE;

		for (nIndex = 0; nIndex < nDirs; nIndex++)
		{
		// plugin may alter filepaths to temp copies (which we delete before returning in all cases)
			filepathUnpacked[nIndex] = files[nIndex];

			//DiffFileData diffdata; //(filepathTransformed1, filepathTransformed2);
			// Invoke unpacking plugins
			if (lstrcmpi(filepathUnpacked[nIndex].c_str(), _T("\\\\.\\NUL")) != 0)
			{
				if (!Unpack(filepathUnpacked[nIndex], filteredFilenames.c_str(), infoUnpacker))
				{
					LPCTSTR pszErrorDesc[] = {_T("Unpack Error Side 1"), _T("Unpack Error Side 2"), _T("Unpack Error Side 3")};
					di.errorDesc = pszErrorDesc[nIndex];
					goto exitPrepAndCompare;
				}

				// we use the same plugins for both files, so they must be defined before second file
				ASSERT(infoUnpacker->bToBeScanned == FALSE);
			}

			// As we keep handles open on unpacked files, Transform() may not delete them.
			// Unpacked files will be deleted at end of this function.
			filepathTransformed[nIndex] = filepathUnpacked[nIndex];

			GuessCodepageEncoding(filepathTransformed[nIndex].c_str(), &encoding[nIndex], pCtxt->m_iGuessEncodingType);
			m_diffFileData.m_FileLocation[nIndex].encoding = encoding[nIndex];
		}

		for (nIndex = 1; nIndex < nDirs; nIndex++)
		{
			if (encoding[0].m_unicoding != encoding[nIndex].m_unicoding || encoding[0].m_codepage != encoding[nIndex].m_codepage)
				bForceUTF8 = TRUE;
		}
		m_codepage = bForceUTF8 ? CP_UTF8 : (encoding[0].m_unicoding ? CP_UTF8 : encoding[0].m_codepage);
		for (nIndex = 0; nIndex < nDirs; nIndex++)
		{
		// Invoke prediff'ing plugins
			if (!m_diffFileData.Filepath_Transform(bForceUTF8, encoding[nIndex], filepathUnpacked[nIndex], filepathTransformed[nIndex], filteredFilenames.c_str(), infoPrediffer))
			{
				LPCTSTR pszErrorDesc[] = {_T("Transform Error Side 1"), _T("Unpack Error Side 2"), _T("Unpack Error Side 3")};
				di.errorDesc = pszErrorDesc[nIndex];
				goto exitPrepAndCompare;
			}
		}

		// If options are binary equivalent, we could check for filesize
		// difference here, and bail out if files are clearly different
		// But, then we don't know if file is ascii or binary, and this
		// affects behavior (also, we don't have an icon for unknown type)

		// Actually compare the files
		// diffutils_compare_files is a fairly thin front-end to diffutils

		if (files.GetSize() == 2)
		{
			m_diffFileData.SetDisplayFilepaths(files[0].c_str(), files[1].c_str()); // store true names for diff utils patch file
			// This opens & fstats both files (if it succeeds)
			if (!m_diffFileData.OpenFiles(filepathTransformed[0].c_str(), filepathTransformed[1].c_str()))
			{
				di.errorDesc = _T("OpenFiles error");
				goto exitPrepAndCompare;
			}
		}
		else
		{
			diffdata10.m_diffFileData.SetDisplayFilepaths(files[1].c_str(), files[0].c_str()); // store true names for diff utils patch file
			diffdata02.m_diffFileData.SetDisplayFilepaths(files[0].c_str(), files[2].c_str()); // store true names for diff utils patch file
			diffdata12.m_diffFileData.SetDisplayFilepaths(files[1].c_str(), files[2].c_str()); // store true names for diff utils patch file

			if (!diffdata10.m_diffFileData.OpenFiles(filepathTransformed[1].c_str(), filepathTransformed[0].c_str()))
			{
				di.errorDesc = _T("OpenFiles(10) error");
				goto exitPrepAndCompare;
			}


			if (!diffdata12.m_diffFileData.OpenFiles(filepathTransformed[1].c_str(), filepathTransformed[2].c_str()))
			{
				di.errorDesc = _T("OpenFiles(12) error");
				goto exitPrepAndCompare;
			}

			if (!diffdata02.m_diffFileData.OpenFiles(filepathTransformed[0].c_str(), filepathTransformed[2].c_str()))
			{
				di.errorDesc = _T("OpenFiles(02) error");
				goto exitPrepAndCompare;
			}

		}
	}

	// If either file is larger than limit compare files by quick contents
	// This allows us to (faster) compare big binary files
	if (nCompMethod == CMP_CONTENT && 
		(di.diffFileInfo[0].size > pCtxt->m_nQuickCompareLimit ||
		di.diffFileInfo[1].size > pCtxt->m_nQuickCompareLimit))
	{
		nCompMethod = CMP_QUICK_CONTENT;
	}

	if (nCompMethod == CMP_CONTENT)
	{
		if (files.GetSize() == 2)
		{
			if (m_pDiffUtilsEngine == NULL)
				m_pDiffUtilsEngine = new CompareEngines::DiffUtils();
			m_pDiffUtilsEngine->SetCodepage(m_codepage);
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
				m_pDiffUtilsEngine->GetTextStats(0, &m_diffFileData.m_textStats[0]);
				m_pDiffUtilsEngine->GetTextStats(1, &m_diffFileData.m_textStats[1]);
			}
			else
				code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;

			// If unique item, it was being compared to itself to determine encoding
			// and the #diffs is invalid
			if (di.diffcode.isSideSecondOnly() || di.diffcode.isSideFirstOnly())
			{
				m_ndiffs = CDiffContext::DIFFS_UNKNOWN;
				m_ntrivialdiffs = CDiffContext::DIFFS_UNKNOWN;
			}
			if (DIFFCODE::isResultError(code))
				di.errorDesc = _T("DiffUtils Error");
		}
		else
		{
			if (m_pDiffUtilsEngine == NULL)
				m_pDiffUtilsEngine = new CompareEngines::DiffUtils();
			m_pDiffUtilsEngine->SetCodepage(m_codepage);
			bool success = m_pByteCompare->SetCompareOptions(
				*m_pCtx->GetCompareOptions(CMP_QUICK_CONTENT));
			if (success)
			{
				if (m_pCtx->m_pFilterList != NULL)
					m_pDiffUtilsEngine->SetFilterList(m_pCtx->m_pFilterList);
				else
					m_pDiffUtilsEngine->ClearFilterList();

				BOOL bRet;
				int bin_flag = 0, bin_flag10 = 0, bin_flag12 = 0, bin_flag02 = 0;

				m_pDiffUtilsEngine->SetFileData(2, diffdata10.m_diffFileData.m_inf);
				bRet = m_pDiffUtilsEngine->Diff2Files(&script10, 0, &bin_flag10, FALSE, NULL);
				m_pDiffUtilsEngine->SetFileData(2, diffdata12.m_diffFileData.m_inf);
				bRet = m_pDiffUtilsEngine->Diff2Files(&script12, 0, &bin_flag12, FALSE, NULL);
				m_pDiffUtilsEngine->SetFileData(2, diffdata02.m_diffFileData.m_inf);
				bRet = m_pDiffUtilsEngine->Diff2Files(&script02, 0, &bin_flag02, FALSE, NULL);
				code = DIFFCODE::FILE;

				CDiffWrapper dw;
				DiffList diffList;
				DIFFSTATUS status;

				diffList.Clear();
				dw.SetCreateDiffList(&diffList);
				dw.LoadWinMergeDiffsFromDiffUtilsScript3(
					script10, script12, script02,
					diffdata10.m_diffFileData.m_inf, diffdata12.m_diffFileData.m_inf, diffdata02.m_diffFileData.m_inf);
				m_ndiffs = diffList.GetSignificantDiffs(); 
				m_ntrivialdiffs = diffList.GetSize() - m_ndiffs;
				
				if (m_ndiffs > 0)
					code |= DIFFCODE::DIFF;
				else
					code |= DIFFCODE::SAME;
				if (bin_flag10 || bin_flag12 || bin_flag02)
					code |= DIFFCODE::BIN;
				else
					code |= DIFFCODE::TEXT;

				// If unique item, it was being compared to itself to determine encoding
				// and the #diffs is invalid
				if (di.diffcode.isSideFirstOnly() || di.diffcode.isSideSecondOnly() || di.diffcode.isSideThirdOnly())
				{
					m_ndiffs = CDiffContext::DIFFS_UNKNOWN;
					m_ntrivialdiffs = CDiffContext::DIFFS_UNKNOWN;
				}
				if (DIFFCODE::isResultError(code))
					di.errorDesc = _T("DiffUtils Error");

				dw.FreeDiffUtilsScript3(script10, script12, script02);
			}
			else
				code = DIFFCODE::FILE | DIFFCODE::CMPERR;
		}

	}
	else if (nCompMethod == CMP_QUICK_CONTENT)
	{
		// use our own byte-by-byte compare
		if (files.GetSize() == 2)
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
	
				m_pByteCompare->GetTextStats(0, &m_diffFileData.m_textStats[0]);
				m_pByteCompare->GetTextStats(1, &m_diffFileData.m_textStats[1]);
			}
			else
				code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;
	
			// Quick contents doesn't know about diff counts
			// Set to special value to indicate invalid
			m_ndiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
			m_ntrivialdiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
			di.diffFileInfo[0].m_textStats = m_diffFileData.m_textStats[0];
			di.diffFileInfo[1].m_textStats = m_diffFileData.m_textStats[1];	
		}
		else
		{
			int code10, code12, code02;
			if (m_pByteCompare == NULL)
				m_pByteCompare = new ByteCompare();
			bool success = m_pByteCompare->SetCompareOptions(
				*m_pCtx->GetCompareOptions(CMP_QUICK_CONTENT));
	
			if (success)
			{
				/* “r’† */
				m_pByteCompare->SetAdditionalOptions(pCtxt->m_bStopAfterFirstDiff);
				m_pByteCompare->SetAbortable(pCtxt->GetAbortable());

				// 10
				m_pByteCompare->SetFileData(2, diffdata10.m_diffFileData.m_inf);
	
				// use our own byte-by-byte compare
				code10 = m_pByteCompare->CompareFiles(diffdata10.m_diffFileData.m_FileLocation);
	
				m_pByteCompare->GetTextStats(0, &diffdata10.m_diffFileData.m_textStats[0]);
				m_pByteCompare->GetTextStats(1, &diffdata10.m_diffFileData.m_textStats[1]);

				// 12
				m_pByteCompare->SetFileData(2, diffdata12.m_diffFileData.m_inf);
	
				// use our own byte-by-byte compare
				code12 = m_pByteCompare->CompareFiles(diffdata12.m_diffFileData.m_FileLocation);
	
				m_pByteCompare->GetTextStats(0, &diffdata12.m_diffFileData.m_textStats[0]);
				m_pByteCompare->GetTextStats(1, &diffdata12.m_diffFileData.m_textStats[1]);

				// 02
				m_pByteCompare->SetFileData(2, diffdata02.m_diffFileData.m_inf);
	
				// use our own byte-by-byte compare
				code02 = m_pByteCompare->CompareFiles(diffdata02.m_diffFileData.m_FileLocation);
	
				m_pByteCompare->GetTextStats(0, &diffdata02.m_diffFileData.m_textStats[0]);
				m_pByteCompare->GetTextStats(1, &diffdata02.m_diffFileData.m_textStats[1]);

				code = DIFFCODE::FILE;
				if (DIFFCODE::isResultError(code10) || DIFFCODE::isResultError(code12) || DIFFCODE::isResultError(code02))
					code |= DIFFCODE::CMPERR;
				if (code10 & DIFFCODE::DIFF || code12 & DIFFCODE::DIFF || code02 & DIFFCODE::DIFF)
					code |= DIFFCODE::DIFF;
				else
					code |= DIFFCODE::SAME;
				if (code10 & DIFFCODE::BIN || code12 & DIFFCODE::BIN || code02 & DIFFCODE::BIN)
					code |= DIFFCODE::BIN;
				else
					code |= DIFFCODE::TEXT;
			}
			else
				code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;
	
			// Quick contents doesn't know about diff counts
			// Set to special value to indicate invalid
			m_ndiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
			m_ntrivialdiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
			// FIXME:
			di.diffFileInfo[0].m_textStats = diffdata10.m_diffFileData.m_textStats[1];
			di.diffFileInfo[1].m_textStats = diffdata10.m_diffFileData.m_textStats[0];	
			di.diffFileInfo[2].m_textStats = diffdata02.m_diffFileData.m_textStats[1];	
		}

	}
	else if (nCompMethod == CMP_DATE || nCompMethod == CMP_DATE_SIZE || nCompMethod == CMP_SIZE)
	{
		if (m_pTimeSizeCompare == NULL)
			m_pTimeSizeCompare = new TimeSizeCompare();

		m_pTimeSizeCompare->SetAdditionalOptions(!!pCtxt->m_bIgnoreSmallTimeDiff);
		code = m_pTimeSizeCompare->CompareFiles(nCompMethod, di);

	}
	else
	{
		// Print error since we should have handled by date compare earlier
		_RPTF0(_CRT_ERROR, "Invalid compare type, DiffFileData can't handle it");
		di.errorDesc = _T("Bad compare type");
		goto exitPrepAndCompare;
	}


exitPrepAndCompare:
	if (pCtxt->m_bPluginsEnabled && (nCompMethod == CMP_CONTENT ||
		nCompMethod == CMP_QUICK_CONTENT))
	{
		m_diffFileData.Reset();
		diffdata10.m_diffFileData.Reset();
		diffdata12.m_diffFileData.Reset();
		diffdata02.m_diffFileData.Reset();
		
		// delete the temp files after comparison
		if (filepathTransformed[0] != filepathUnpacked[0])
			::DeleteFile(filepathTransformed[0].c_str()) || GetLog()->DeleteFileFailed(filepathTransformed[0].c_str());
		if (filepathTransformed[1] != filepathUnpacked[1])
			::DeleteFile(filepathTransformed[1].c_str()) || GetLog()->DeleteFileFailed(filepathTransformed[1].c_str());
		if (nDirs > 2 && filepathTransformed[2] != filepathUnpacked[2])
			::DeleteFile(filepathTransformed[2].c_str()) || GetLog()->DeleteFileFailed(filepathTransformed[2].c_str());
		if (filepathUnpacked[0] != files[0])
			::DeleteFile(filepathUnpacked[0].c_str()) || GetLog()->DeleteFileFailed(filepathUnpacked[0].c_str());
		if (filepathUnpacked[1] != files[1])
			::DeleteFile(filepathUnpacked[1].c_str()) || GetLog()->DeleteFileFailed(filepathUnpacked[1].c_str());
		if (nDirs > 2 && filepathUnpacked[2] != files[2])
			::DeleteFile(filepathUnpacked[2].c_str()) || GetLog()->DeleteFileFailed(filepathUnpacked[2].c_str());
	}
	return code;
}

/**
 * @brief Get actual compared paths from DIFFITEM.
 * @param [in] pCtx Pointer to compare context.
 * @param [in] di DiffItem from which the paths are created.
 * @param [out] left Gets the left compare path.
 * @param [out] right Gets the right compare path.
 * @note If item is unique, same path is returned for both.
 */
void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, PathContext & files)
{
	static const TCHAR backslash[] = _T("\\");
	int nDirs = pCtxt->GetCompareDirs();

	files.SetSize(nDirs);

	for (int nIndex = 0; nIndex < nDirs; nIndex++)
	{
		if (di.diffcode.isExists(nIndex))
		{
			// Compare file to itself to detect encoding
			String path = pCtxt->GetNormalizedPath(nIndex);
			if (!di.diffFileInfo[nIndex].path.empty())
				path = paths_ConcatPath(path, di.diffFileInfo[nIndex].path);
			path = paths_ConcatPath(path, di.diffFileInfo[nIndex].filename);
			files.SetPath(nIndex, path.c_str());
		}
		else
		{
			files.SetPath(nIndex, _T("NUL"));
		}
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
