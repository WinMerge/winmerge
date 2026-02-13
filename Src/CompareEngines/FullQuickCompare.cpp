/**
 * @file  FullQuickCompare.cpp
 *
 * @brief Implementation file for FullQuickCompare
 */

#include "pch.h"
#include "FullQuickCompare.h"
#include "DiffContext.h"
#include "DiffItem.h"
#include "PathContext.h"
#include "DiffFileData.h"
#include "DiffWrapper.h"
#include "Wrap_DiffUtils.h"
#include "ByteCompare.h"
#include "paths.h"
#include "codepage_detect.h"
#include "Logger.h"
#include "TFile.h"

namespace CompareEngines
{

FullQuickCompare::FullQuickCompare(CDiffContext& ctxt, int nCompMethod)
	: m_pCtxt(&ctxt), m_nCompMethod(nCompMethod)
{
}

FullQuickCompare::~FullQuickCompare() = default;

/**
 * @brief Compare two specified files using full content comparison
 * @param [in,out] di Diffitem info. Results are written to di.diffcode.
 * @return DIFFCODE
 */
int FullQuickCompare::CompareFiles(DIFFITEM& di) const
{
	DiffFileData m_diffFileData;

	int nIndex;
	int nCompMethod = m_nCompMethod;
	int nDirs = m_pCtxt->GetCompareDirs();

	unsigned code = DIFFCODE::FILE | DIFFCODE::CMPERR;
	// Reset text stats
	for (nIndex = 0; nIndex < nDirs; nIndex++)
	{
		m_diffFileData.m_textStats[nIndex].clear();
		m_diffFileData.m_FileLocation[nIndex].encoding.Clear();
	}
	int m_ndiffs = CDiffContext::DIFFS_UNKNOWN;
	int m_ntrivialdiffs = CDiffContext::DIFFS_UNKNOWN;

	PathContext tFiles;
	m_pCtxt->GetComparePaths(di, tFiles);
	struct change *script10 = nullptr;
	struct change *script12 = nullptr;
	struct change *script02 = nullptr;
	DiffFileData diffdata10, diffdata12, diffdata02;
	String filepathUnpacked[3];
	String filepathTransformed[3];
	int codepage = 0;

	// For user chosen plugins, define bAutomaticUnpacker as false and use the chosen infoHandler
	// but how can we receive the infoHandler ? DirScan actually only 
	// returns info, but can not use file dependent information.

	// Transformation happens here
	// text used for automatic mode : plugin filter must match it
	String filteredFilenames = CDiffContext::GetFilteredFilenames(tFiles);

	PackingInfo * infoUnpacker = nullptr;
	PrediffingInfo * infoPrediffer = nullptr;

	// Get existing or new plugin infos
	if (m_pCtxt->m_piPluginInfos != nullptr)
		m_pCtxt->FetchPluginInfos(filteredFilenames, &infoUnpacker,
				&infoPrediffer);

	FileTextEncoding encoding[3];
	bool bForceUTF8 = m_pCtxt->GetCompareOptions(nCompMethod)->m_bIgnoreCase;

	for (nIndex = 0; nIndex < nDirs; nIndex++)
	{
	// plugin may alter filepaths to temp copies (which we delete before returning in all cases)
		filepathUnpacked[nIndex] = tFiles[nIndex];

		//DiffFileData diffdata; //(filepathTransformed1, filepathTransformed2);
		// Invoke unpacking plugins
		if (infoUnpacker && !paths::IsNullDeviceName(filepathUnpacked[nIndex]))
		{
			if (!infoUnpacker->Unpacking(nIndex, nullptr, filepathUnpacked[nIndex], filteredFilenames, { tFiles[nIndex] }))
				goto exitPrepAndCompare;
		}

		// As we keep handles open on unpacked files, Transform() may not delete them.
		// Unpacked files will be deleted at end of this function.
		filepathTransformed[nIndex] = filepathUnpacked[nIndex];

		encoding[nIndex] = codepage_detect::Guess(filepathTransformed[nIndex], m_pCtxt->m_iGuessEncodingType);
		m_diffFileData.m_FileLocation[nIndex].encoding = encoding[nIndex];
	}

	if (!std::equal(encoding + 1, encoding + nDirs, encoding))
		bForceUTF8 = true;
	codepage = bForceUTF8 ? ucr::CP_UTF_8 : (encoding[0].m_unicoding ? ucr::CP_UTF_8 : encoding[0].m_codepage);
	for (nIndex = 0; nIndex < nDirs; nIndex++)
	{
	// Invoke prediff'ing plugins
		if (infoPrediffer && !m_diffFileData.Filepath_Transform(nIndex, bForceUTF8, encoding[nIndex], filepathUnpacked[nIndex], filepathTransformed[nIndex], filteredFilenames, *infoPrediffer))
			goto exitPrepAndCompare;
	}

	// If options are binary equivalent, we could check for filesize
	// difference here, and bail out if files are clearly different
	// But, then we don't know if file is ascii or binary, and this
	// affects behavior (also, we don't have an icon for unknown type)

	// Actually compare the files
	// `diffutils_compare_files()` is a fairly thin front-end to GNU diffutils

	if (tFiles.GetSize() == 2)
	{
		m_diffFileData.SetDisplayFilepaths(tFiles[0], tFiles[1]); // store true names for diff utils patch file
		// This opens & fstats both files (if it succeeds)
		if (!m_diffFileData.OpenFiles(filepathTransformed[0], filepathTransformed[1]))
			goto exitPrepAndCompare;
	}
	else
	{
		diffdata10.SetDisplayFilepaths(tFiles[1], tFiles[0]); // store true names for diff utils patch file
		diffdata12.SetDisplayFilepaths(tFiles[1], tFiles[2]); // store true names for diff utils patch file
		diffdata02.SetDisplayFilepaths(tFiles[0], tFiles[2]); // store true names for diff utils patch file

		if (!diffdata10.OpenFiles(filepathTransformed[1], filepathTransformed[0]))
			goto exitPrepAndCompare;

		if (!diffdata12.OpenFiles(filepathTransformed[1], filepathTransformed[2]))
			goto exitPrepAndCompare;

		if (!diffdata02.OpenFiles(filepathTransformed[0], filepathTransformed[2]))
			goto exitPrepAndCompare;
	}

	if (nCompMethod == CMP_CONTENT)
	{
		if (m_pDiffUtilsEngine == nullptr)
		{
			m_pDiffUtilsEngine.reset(new DiffUtils());
			m_pDiffUtilsEngine->SetCodepage(codepage);
			m_pDiffUtilsEngine->SetCompareOptions(*m_pCtxt->GetCompareOptions(CMP_CONTENT));
			if (m_pCtxt->m_pFilterList != nullptr)
				m_pDiffUtilsEngine->SetFilterList(m_pCtxt->m_pFilterList);
			else
				m_pDiffUtilsEngine->ClearFilterList();
			if (m_pCtxt->m_pSubstitutionList != nullptr)
				m_pDiffUtilsEngine->SetSubstitutionList(m_pCtxt->m_pSubstitutionList);
			else
				m_pDiffUtilsEngine->ClearSubstitutionList();
		}
		if (tFiles.GetSize() == 2)
		{
			code = m_pDiffUtilsEngine->CompareFiles(&m_diffFileData);
			m_pDiffUtilsEngine->GetDiffCounts(di.nsdiffs, di.nidiffs);

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
			bool bRet;
			int bin_flag10 = 0, bin_flag12 = 0, bin_flag02 = 0;

			bRet = m_pDiffUtilsEngine->Diff2Files(&script10, &diffdata10, &bin_flag10, nullptr);
			bRet = m_pDiffUtilsEngine->Diff2Files(&script12, &diffdata12, &bin_flag12, nullptr);
			bRet = m_pDiffUtilsEngine->Diff2Files(&script02, &diffdata02, &bin_flag02, nullptr);
			m_diffFileData.m_textStats[0] = diffdata10.m_textStats[1];
			m_diffFileData.m_textStats[1] = diffdata12.m_textStats[0];
			m_diffFileData.m_textStats[2] = diffdata02.m_textStats[1];

			code = DIFFCODE::FILE;

			String Ext = tFiles[0];
			size_t PosOfDot = Ext.rfind('.');
			if (PosOfDot != String::npos)
				Ext.erase(0, PosOfDot + 1);

			CDiffWrapper dw;
			DiffList diffList;
			DIFFSTATUS status;

			diffList.Clear();
			dw.SetCompareFiles(tFiles);
			dw.SetOptions(m_pCtxt->GetOptions(), true);
			dw.SetFilterList(m_pCtxt->m_pFilterList);
			dw.SetSubstitutionList(m_pCtxt->m_pSubstitutionList);
			dw.SetFilterCommentsSourceDef(Ext);
			dw.SetCreateDiffList(&diffList);
			dw.LoadWinMergeDiffsFromDiffUtilsScript3(
				script10, script12, script02,
				diffdata10.m_inf, diffdata12.m_inf, diffdata02.m_inf);
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
					if (CDiffWrapper::IsIdenticalOrIgnorable(script12))
						code |= DIFFCODE::DIFF1STONLY;
					else if (CDiffWrapper::IsIdenticalOrIgnorable(script02))
						code |= DIFFCODE::DIFF2NDONLY;
					else if (CDiffWrapper::IsIdenticalOrIgnorable(script10))
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

	}
	else if (nCompMethod == CMP_QUICK_CONTENT)
	{
		// use our own byte-by-byte compare
		if (m_pByteCompare == nullptr)
		{
			m_pByteCompare.reset(new ByteCompare());
			m_pByteCompare->SetCompareOptions(*m_pCtxt->GetCompareOptions(CMP_QUICK_CONTENT));

			m_pByteCompare->SetAdditionalOptions(m_pCtxt->m_bStopAfterFirstDiff);
			m_pByteCompare->SetAbortable(m_pCtxt->GetAbortable());
		}
		if (tFiles.GetSize() == 2)
		{
			// use our own byte-by-byte compare
			code = m_pByteCompare->CompareFiles(&m_diffFileData);

			// Quick contents doesn't know about diff counts
			// Set to special value to indicate invalid
			m_ndiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
			m_ntrivialdiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
		}
		else
		{
			// use our own byte-by-byte compare
			// 10
			int code10 = m_pByteCompare->CompareFiles(&diffdata10);
			// 12
			int code12 = m_pByteCompare->CompareFiles(&diffdata12);
			// 02
			int code02 = m_pByteCompare->CompareFiles(&diffdata02);

			m_diffFileData.m_textStats[0] = diffdata10.m_textStats[1];
			m_diffFileData.m_textStats[1] = diffdata12.m_textStats[0];
			m_diffFileData.m_textStats[2] = diffdata02.m_textStats[1];

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

			// Quick contents doesn't know about diff counts
			// Set to special value to indicate invalid
			m_ndiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
			m_ntrivialdiffs = CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE;
		}
	}
exitPrepAndCompare:
	const int errnoSaved = errno;

	m_diffFileData.Reset();
	diffdata10.Reset();
	diffdata12.Reset();
	diffdata02.Reset();
	
	// delete the temp files after comparison
	if (filepathTransformed[0] != filepathUnpacked[0] && !filepathTransformed[0].empty())
		try { TFile(filepathTransformed[0]).remove(); } catch (...) { RootLogger::Error(strutils::format(_T("DeleteFile(%s) failed"), filepathTransformed[0])); }
	if (filepathTransformed[1] != filepathUnpacked[1] && !filepathTransformed[1].empty())
		try { TFile(filepathTransformed[1]).remove(); } catch (...) { RootLogger::Error(strutils::format(_T("DeleteFile(%s) failed"), filepathTransformed[1])); }
	if (nDirs > 2 && filepathTransformed[2] != filepathUnpacked[2] && !filepathTransformed[2].empty())
		try { TFile(filepathTransformed[2]).remove(); } catch (...) { RootLogger::Error(strutils::format(_T("DeleteFile(%s) failed"), filepathTransformed[2])); }
	if (filepathUnpacked[0] != tFiles[0] && !filepathUnpacked[0].empty())
		try { TFile(filepathUnpacked[0]).remove(); } catch (...) { RootLogger::Error(strutils::format(_T("DeleteFile(%s) failed"), filepathUnpacked[0])); }
	if (filepathUnpacked[1] != tFiles[1] && !filepathUnpacked[1].empty())
		try { TFile(filepathUnpacked[1]).remove(); } catch (...) { RootLogger::Error(strutils::format(_T("DeleteFile(%s) failed"), filepathUnpacked[1])); }
	if (nDirs > 2 && filepathUnpacked[2] != tFiles[2] && !filepathUnpacked[2].empty())
		try { TFile(filepathUnpacked[2]).remove(); } catch (...) { RootLogger::Error(strutils::format(_T("DeleteFile(%s) failed"), filepathUnpacked[2])); }

	// When comparing empty file and nonexistent file, `DIFFCODE::SAME` flag is set to the variable `code`, so change the flag to `DIFFCODE::DIFF`
	// Also when disabling ignore codepage option and the encodings of files are not equal, change the flag to `DIFFCODE::DIFF even if  `DIFFCODE::SAME` flag is set to the variable `code`
	if (!di.diffcode.existAll() || (!m_pCtxt->m_bIgnoreCodepage && !std::equal(encoding + 1, encoding + nDirs, encoding)))
		code = (code & ~DIFFCODE::COMPAREFLAGS) | DIFFCODE::DIFF;

	for (int i = 0; i < nDirs; ++i)
	{
		// Set text statistics
		if (di.diffcode.exists(i))
		{
			di.diffFileInfo[i].m_textStats = m_diffFileData.m_textStats[i];
			di.diffFileInfo[i].encoding = m_diffFileData.m_FileLocation[i].encoding;
		}
	}

	errno = errnoSaved;

	di.diffcode.diffcode &= ~(DIFFCODE::TEXTFLAGS | DIFFCODE::TYPEFLAGS | DIFFCODE::COMPAREFLAGS | DIFFCODE::COMPAREFLAGS3WAY);
	di.diffcode.diffcode |= code;
	di.nsdiffs = m_ndiffs;
	di.nidiffs = m_ntrivialdiffs;
	return code;

}

} // namespace CompareEngines
