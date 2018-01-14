/** 
 * @file  FolderCmp.cpp
 *
 * @brief Implementation file for FolderCmp
 */

#include "diff.h"
#include "FolderCmp.h"
#include <cassert>
#include "DiffUtils.h"
#include "ByteCompare.h"
#include "paths.h"
#include "FilterList.h"
#include "DiffContext.h"
#include "DiffList.h"
#include "DiffWrapper.h"
#include "FileTransform.h"
#include "codepage_detect.h"
#include "BinaryCompare.h"
#include "TimeSizeCompare.h"
#include "TFile.h"

using CompareEngines::ByteCompare;
using CompareEngines::BinaryCompare;
using CompareEngines::TimeSizeCompare;

static void GetComparePaths(CDiffContext * pCtxt, const DIFFITEM &di, PathContext & files);

FolderCmp::FolderCmp()
: m_pDiffUtilsEngine(nullptr)
, m_pByteCompare(nullptr)
, m_pBinaryCompare(nullptr)
, m_pTimeSizeCompare(nullptr)
, m_ndiffs(CDiffContext::DIFFS_UNKNOWN)
, m_ntrivialdiffs(CDiffContext::DIFFS_UNKNOWN)
{
}

FolderCmp::~FolderCmp()
{
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
	int nCompMethod = pCtxt->GetCompareMethod();

	unsigned code = DIFFCODE::FILE | DIFFCODE::CMPERR;

	if (nCompMethod == CMP_CONTENT ||
		nCompMethod == CMP_QUICK_CONTENT)
	{
		int nDirs = pCtxt->GetCompareDirs();

		// Reset text stats
		for (nIndex = 0; nIndex < nDirs; nIndex++)
			m_diffFileData.m_textStats[nIndex].clear();

		PathContext files;
		GetComparePaths(pCtxt, di, files);
		struct change *script = NULL;
		struct change *script10 = NULL;
		struct change *script12 = NULL;
		struct change *script02 = NULL;
		FolderCmp diffdata10, diffdata12, diffdata02;
		String filepathUnpacked[3];
		String filepathTransformed[3];
		int codepage = 0;

		// For user chosen plugins, define bAutomaticUnpacker as false and use the chosen infoHandler
		// but how can we receive the infoHandler ? DirScan actually only 
		// returns info, but can not use file dependent information.

		// Transformation happens here
		// text used for automatic mode : plugin filter must match it
		String filteredFilenames = strutils::join(files.begin(), files.end(), _T("|"));

		PackingInfo * infoUnpacker=0;
		PrediffingInfo * infoPrediffer=0;

		// Get existing or new plugin infos
		if (pCtxt->m_piPluginInfos)
			pCtxt->FetchPluginInfos(filteredFilenames, &infoUnpacker,
					&infoPrediffer);

		FileTextEncoding encoding[3];
		bool bForceUTF8 = pCtxt->GetCompareOptions(nCompMethod)->m_bIgnoreCase;

		for (nIndex = 0; nIndex < nDirs; nIndex++)
		{
		// plugin may alter filepaths to temp copies (which we delete before returning in all cases)
			filepathUnpacked[nIndex] = files[nIndex];

			//DiffFileData diffdata; //(filepathTransformed1, filepathTransformed2);
			// Invoke unpacking plugins
			if (infoUnpacker && strutils::compare_nocase(filepathUnpacked[nIndex], _T("NUL")) != 0)
			{
				if (!FileTransform::Unpacking(infoUnpacker, filepathUnpacked[nIndex], filteredFilenames))
					goto exitPrepAndCompare;

				// we use the same plugins for both files, so they must be defined before second file
				assert(infoUnpacker->bToBeScanned == false);
			}

			// As we keep handles open on unpacked files, Transform() may not delete them.
			// Unpacked files will be deleted at end of this function.
			filepathTransformed[nIndex] = filepathUnpacked[nIndex];

			encoding[nIndex] = GuessCodepageEncoding(filepathTransformed[nIndex], pCtxt->m_iGuessEncodingType);
			m_diffFileData.m_FileLocation[nIndex].encoding = encoding[nIndex];
		}

		if (!std::equal(encoding + 1, encoding + nDirs, encoding))
			bForceUTF8 = true;
		codepage = bForceUTF8 ? CP_UTF8 : (encoding[0].m_unicoding ? CP_UTF8 : encoding[0].m_codepage);
		for (nIndex = 0; nIndex < nDirs; nIndex++)
		{
		// Invoke prediff'ing plugins
			if (infoPrediffer && !m_diffFileData.Filepath_Transform(bForceUTF8, encoding[nIndex], filepathUnpacked[nIndex], filepathTransformed[nIndex], filteredFilenames, infoPrediffer))
				goto exitPrepAndCompare;
		}

		// If options are binary equivalent, we could check for filesize
		// difference here, and bail out if files are clearly different
		// But, then we don't know if file is ascii or binary, and this
		// affects behavior (also, we don't have an icon for unknown type)

		// Actually compare the files
		// diffutils_compare_files is a fairly thin front-end to diffutils

		if (files.GetSize() == 2)
		{
			m_diffFileData.SetDisplayFilepaths(files[0], files[1]); // store true names for diff utils patch file
			// This opens & fstats both files (if it succeeds)
			if (!m_diffFileData.OpenFiles(filepathTransformed[0], filepathTransformed[1]))
				goto exitPrepAndCompare;
		}
		else
		{
			diffdata10.m_diffFileData.SetDisplayFilepaths(files[1], files[0]); // store true names for diff utils patch file
			diffdata12.m_diffFileData.SetDisplayFilepaths(files[1], files[2]); // store true names for diff utils patch file
			diffdata02.m_diffFileData.SetDisplayFilepaths(files[0], files[2]); // store true names for diff utils patch file

			if (!diffdata10.m_diffFileData.OpenFiles(filepathTransformed[1], filepathTransformed[0]))
				goto exitPrepAndCompare;

			if (!diffdata12.m_diffFileData.OpenFiles(filepathTransformed[1], filepathTransformed[2]))
				goto exitPrepAndCompare;

			if (!diffdata02.m_diffFileData.OpenFiles(filepathTransformed[0], filepathTransformed[2]))
				goto exitPrepAndCompare;
		}

		// If either file is larger than limit compare files by quick contents
		// This allows us to (faster) compare big binary files
		if (nCompMethod == CMP_CONTENT && 
			(di.diffFileInfo[0].size > pCtxt->m_nQuickCompareLimit ||
			di.diffFileInfo[1].size > pCtxt->m_nQuickCompareLimit || 
			(nDirs > 2 && di.diffFileInfo[2].size > pCtxt->m_nQuickCompareLimit)))
		{
			nCompMethod = CMP_QUICK_CONTENT;
		}

		if (nCompMethod == CMP_CONTENT)
		{
			if (files.GetSize() == 2)
			{
				if (m_pDiffUtilsEngine == NULL)
					m_pDiffUtilsEngine.reset(new CompareEngines::DiffUtils());
				m_pDiffUtilsEngine->SetCodepage(codepage);
				bool success = m_pDiffUtilsEngine->SetCompareOptions(
						*pCtxt->GetCompareOptions(CMP_CONTENT));
				if (success)
				{
					if (pCtxt->m_pFilterList != NULL)
						m_pDiffUtilsEngine->SetFilterList(pCtxt->m_pFilterList.get());
					else
						m_pDiffUtilsEngine->ClearFilterList();
					m_pDiffUtilsEngine->SetFilterCommentsManager(pCtxt->m_pFilterCommentsManager);
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
			}
			else
			{
				if (m_pDiffUtilsEngine == NULL)
					m_pDiffUtilsEngine.reset(new CompareEngines::DiffUtils());
				m_pDiffUtilsEngine->SetCodepage(codepage);
				bool success = m_pDiffUtilsEngine->SetCompareOptions(
						*pCtxt->GetCompareOptions(CMP_CONTENT));
				if (success)
				{
					if (pCtxt->m_pFilterList != NULL)
						m_pDiffUtilsEngine->SetFilterList(pCtxt->m_pFilterList.get());
					else
						m_pDiffUtilsEngine->ClearFilterList();
					m_pDiffUtilsEngine->SetFilterCommentsManager(pCtxt->m_pFilterCommentsManager);

					bool bRet;
					int bin_flag10 = 0, bin_flag12 = 0, bin_flag02 = 0;

					m_pDiffUtilsEngine->SetFileData(2, diffdata10.m_diffFileData.m_inf);
					bRet = m_pDiffUtilsEngine->Diff2Files(&script10, 0, &bin_flag10, false, NULL);
					m_pDiffUtilsEngine->GetTextStats(0, &m_diffFileData.m_textStats[1]);
					m_pDiffUtilsEngine->GetTextStats(1, &m_diffFileData.m_textStats[0]);

					m_pDiffUtilsEngine->SetFileData(2, diffdata12.m_diffFileData.m_inf);
					bRet = m_pDiffUtilsEngine->Diff2Files(&script12, 0, &bin_flag12, false, NULL);
					m_pDiffUtilsEngine->GetTextStats(0, &m_diffFileData.m_textStats[1]);
					m_pDiffUtilsEngine->GetTextStats(1, &m_diffFileData.m_textStats[2]);

					m_pDiffUtilsEngine->SetFileData(2, diffdata02.m_diffFileData.m_inf);
					bRet = m_pDiffUtilsEngine->Diff2Files(&script02, 0, &bin_flag02, false, NULL);
					m_pDiffUtilsEngine->GetTextStats(0, &m_diffFileData.m_textStats[0]);
					m_pDiffUtilsEngine->GetTextStats(1, &m_diffFileData.m_textStats[2]);

					code = DIFFCODE::FILE;

					CDiffWrapper dw;
					DiffList diffList;
					DIFFSTATUS status;

					diffList.Clear();
					dw.SetCreateDiffList(&diffList);
					dw.LoadWinMergeDiffsFromDiffUtilsScript3(
						script10, script12,
						diffdata10.m_diffFileData.m_inf, diffdata12.m_diffFileData.m_inf);
					m_ndiffs = diffList.GetSignificantDiffs(); 
					m_ntrivialdiffs = diffList.GetSize() - m_ndiffs;
				
					if (m_ndiffs > 0 || bin_flag10 < 0 || bin_flag12 < 0)
						code |= DIFFCODE::DIFF;
					else
						code |= DIFFCODE::SAME;
					if (bin_flag10 || bin_flag12)
						code |= DIFFCODE::BIN;
					else
						code |= DIFFCODE::TEXT;

					if ((code & DIFFCODE::COMPAREFLAGS) == DIFFCODE::DIFF)
					{
						if ((code & DIFFCODE::TEXTFLAGS) == DIFFCODE::TEXT)
						{
							if (!script12)
								code |= DIFFCODE::DIFF1STONLY;
							else if (!script02)
								code |= DIFFCODE::DIFF2NDONLY;
							else if (!script10)
								code |= DIFFCODE::DIFF3RDONLY;
						}
						else
						{
							if (bin_flag12 > 0)
								code |= DIFFCODE::DIFF1STONLY;
							else if (bin_flag02 > 0)
								code |= DIFFCODE::DIFF2NDONLY;
							else if (bin_flag10 > 0)
								code |= DIFFCODE::DIFF3RDONLY;
						}
					}

					// If unique item, it was being compared to itself to determine encoding
					// and the #diffs is invalid
					if (di.diffcode.isSideFirstOnly() || di.diffcode.isSideSecondOnly() || di.diffcode.isSideThirdOnly())
					{
						m_ndiffs = CDiffContext::DIFFS_UNKNOWN;
						m_ntrivialdiffs = CDiffContext::DIFFS_UNKNOWN;
					}

					dw.FreeDiffUtilsScript(script10);
					dw.FreeDiffUtilsScript(script12);
					dw.FreeDiffUtilsScript(script02);
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
					m_pByteCompare.reset(new ByteCompare());
				bool success = m_pByteCompare->SetCompareOptions(
					*pCtxt->GetCompareOptions(CMP_QUICK_CONTENT));
	
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
			}
			else
			{
				if (m_pByteCompare == NULL)
					m_pByteCompare.reset(new ByteCompare());
				bool success = m_pByteCompare->SetCompareOptions(
					*pCtxt->GetCompareOptions(CMP_QUICK_CONTENT));
	
				if (success)
				{
					/* “r’† */
					m_pByteCompare->SetAdditionalOptions(pCtxt->m_bStopAfterFirstDiff);
					m_pByteCompare->SetAbortable(pCtxt->GetAbortable());

					// 10
					m_pByteCompare->SetFileData(2, diffdata10.m_diffFileData.m_inf);

					// use our own byte-by-byte compare
					int code10 = m_pByteCompare->CompareFiles(diffdata10.m_diffFileData.m_FileLocation);

					m_pByteCompare->GetTextStats(0, &m_diffFileData.m_textStats[1]);
					m_pByteCompare->GetTextStats(1, &m_diffFileData.m_textStats[0]);

					// 12
					m_pByteCompare->SetFileData(2, diffdata12.m_diffFileData.m_inf);

					// use our own byte-by-byte compare
					int code12 = m_pByteCompare->CompareFiles(diffdata12.m_diffFileData.m_FileLocation);

					m_pByteCompare->GetTextStats(0, &m_diffFileData.m_textStats[1]);
					m_pByteCompare->GetTextStats(1, &m_diffFileData.m_textStats[2]);

					// 02
					m_pByteCompare->SetFileData(2, diffdata02.m_diffFileData.m_inf);

					// use our own byte-by-byte compare
					int code02 = m_pByteCompare->CompareFiles(diffdata02.m_diffFileData.m_FileLocation);

					m_pByteCompare->GetTextStats(0, &m_diffFileData.m_textStats[0]);
					m_pByteCompare->GetTextStats(1, &m_diffFileData.m_textStats[2]);

					code = DIFFCODE::FILE;
					if (DIFFCODE::isResultError(code10) || DIFFCODE::isResultError(code12) || DIFFCODE::isResultError(code02))
						code |= DIFFCODE::CMPERR;
					if ((code10 & DIFFCODE::COMPAREFLAGS) == DIFFCODE::DIFF || (code12 & DIFFCODE::COMPAREFLAGS) == DIFFCODE::DIFF)
						code |= DIFFCODE::DIFF;
					else
						code |= DIFFCODE::SAME;
					if ((code10 & DIFFCODE::TEXTFLAGS) == DIFFCODE::TEXT && 
					    (code12 & DIFFCODE::TEXTFLAGS) == DIFFCODE::TEXT)
						code |= DIFFCODE::TEXT;
					else
						code |= DIFFCODE::BIN;
					if ((code10 & DIFFCODE::TEXTFLAGS) == (DIFFCODE::BIN | DIFFCODE::BINSIDE1))
						code |= DIFFCODE::BINSIDE2;
					if ((code10 & DIFFCODE::TEXTFLAGS) == (DIFFCODE::BIN | DIFFCODE::BINSIDE2))
						code |= DIFFCODE::BINSIDE1;
					if ((code12 & DIFFCODE::TEXTFLAGS) == (DIFFCODE::BIN | DIFFCODE::BINSIDE2))
						code |= DIFFCODE::BINSIDE3;
					if ((code & DIFFCODE::COMPAREFLAGS) == DIFFCODE::DIFF)
					{
						if ((code12 & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SAME)
							code |= DIFFCODE::DIFF1STONLY;
						else if ((code02 & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SAME)
							code |= DIFFCODE::DIFF2NDONLY;
						else if ((code10 & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SAME)
							code |= DIFFCODE::DIFF3RDONLY;
					}
				}
				else
					code = DIFFCODE::FILE | DIFFCODE::TEXT | DIFFCODE::CMPERR;
	
				// Quick contents doesn't know about diff counts
				// Set to special value to indicate invalid
				m_ndiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
				m_ntrivialdiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
			}
		}
exitPrepAndCompare:
		m_diffFileData.Reset();
		diffdata10.m_diffFileData.Reset();
		diffdata12.m_diffFileData.Reset();
		diffdata02.m_diffFileData.Reset();
		
		// delete the temp files after comparison
		if (filepathTransformed[0] != filepathUnpacked[0])
			try { TFile(filepathTransformed[0]).remove(); } catch (...) { LogErrorString(strutils::format(_T("DeleteFile(%s) failed"), filepathTransformed[0].c_str())); }
		if (filepathTransformed[1] != filepathUnpacked[1])
			try { TFile(filepathTransformed[1]).remove(); } catch (...) { LogErrorString(strutils::format(_T("DeleteFile(%s) failed"), filepathTransformed[1].c_str())); }
		if (nDirs > 2 && filepathTransformed[2] != filepathUnpacked[2])
			try { TFile(filepathTransformed[2]).remove(); } catch (...) { LogErrorString(strutils::format(_T("DeleteFile(%s) failed"), filepathTransformed[2].c_str())); }
		if (filepathUnpacked[0] != files[0])
			try { TFile(filepathUnpacked[0]).remove(); } catch (...) { LogErrorString(strutils::format(_T("DeleteFile(%s) failed"), filepathUnpacked[0].c_str())); }
		if (filepathUnpacked[1] != files[1])
			try { TFile(filepathUnpacked[1]).remove(); } catch (...) { LogErrorString(strutils::format(_T("DeleteFile(%s) failed"), filepathUnpacked[1].c_str())); }
		if (nDirs > 2 && filepathUnpacked[2] != files[2])
			try { TFile(filepathUnpacked[2]).remove(); } catch (...) { LogErrorString(strutils::format(_T("DeleteFile(%s) failed"), filepathUnpacked[2].c_str())); }

		if (!pCtxt->m_bIgnoreCodepage && 
		    (code & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SAME &&
		    !std::equal(encoding + 1, encoding + nDirs, encoding))
			code = (code & ~DIFFCODE::COMPAREFLAGS) | DIFFCODE::DIFF;
	}
	else if (nCompMethod == CMP_BINARY_CONTENT)
	{
		if (m_pBinaryCompare == NULL)
			m_pBinaryCompare.reset(new BinaryCompare());

		PathContext files;
		GetComparePaths(pCtxt, di, files);
		code = m_pBinaryCompare->CompareFiles(files, di);
	}
	else if (nCompMethod == CMP_DATE || nCompMethod == CMP_DATE_SIZE || nCompMethod == CMP_SIZE)
	{
		if (m_pTimeSizeCompare == NULL)
			m_pTimeSizeCompare.reset(new TimeSizeCompare());

		m_pTimeSizeCompare->SetAdditionalOptions(!!pCtxt->m_bIgnoreSmallTimeDiff);
		code = m_pTimeSizeCompare->CompareFiles(nCompMethod, pCtxt->GetCompareDirs(), di);
	}
	else
	{
		// Print error since we should have handled by date compare earlier
		throw "Invalid compare type, DiffFileData can't handle it";
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
	int nDirs = pCtxt->GetCompareDirs();

	files.SetSize(nDirs);

	for (int nIndex = 0; nIndex < nDirs; nIndex++)
	{
		if (di.diffcode.exists(nIndex))
		{
			files.SetPath(nIndex,
				paths::ConcatPath(pCtxt->GetPath(nIndex), di.diffFileInfo[nIndex].GetFile()), false);
		}
		else
		{
			files.SetPath(nIndex, _T("NUL"), false);
		}
	}
}
