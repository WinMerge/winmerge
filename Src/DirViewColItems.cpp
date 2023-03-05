/** 
 * @file  DirViewColItems.cpp
 *
 * @brief Code for individual columns in the DirView
 *
 * @date  Created: 2003-08-19
 */

#include "pch.h"
#include "DirViewColItems.h"
#include <Poco/Timestamp.h>
#include <Shlwapi.h>
#include "UnicodeString.h"
#include "DiffItem.h"
#include "DiffContext.h"
#include "locality.h"
#include "paths.h"
#include "MergeApp.h"
#include "FileTransform.h"
#include "PropertySystem.h"
#include "DebugNew.h"

using Poco::Timestamp;

using std::swap;

namespace
{
const char *COLHDR_FILENAME     = N_("Filename");
const char *COLHDR_DIR          = NC_("DirView|ColumnHeader", "Folder");
const char *COLHDR_RESULT       = N_("Comparison result");
const char *COLHDR_LTIMEM       = N_("Left Date");
const char *COLHDR_RTIMEM       = N_("Right Date");
const char *COLHDR_MTIMEM       = N_("Middle Date");
const char *COLHDR_EXTENSION    = N_("Extension");
const char *COLHDR_LSIZE        = N_("Left Size");
const char *COLHDR_RSIZE        = N_("Right Size");
const char *COLHDR_MSIZE        = N_("Middle Size");
const char *COLHDR_RSIZE_SHORT  = N_("Right Size (Short)");
const char *COLHDR_LSIZE_SHORT  = N_("Left Size (Short)");
const char *COLHDR_MSIZE_SHORT  = N_("Middle Size (Short)");
const char *COLHDR_LTIMEC       = N_("Left Creation Time");
const char *COLHDR_RTIMEC       = N_("Right Creation Time");
const char *COLHDR_MTIMEC       = N_("Middle Creation Time");
const char *COLHDR_NEWER        = N_("Newer File");
const char *COLHDR_LVERSION     = N_("Left File Version");
const char *COLHDR_RVERSION     = N_("Right File Version");
const char *COLHDR_MVERSION     = N_("Middle File Version");
const char *COLHDR_RESULT_ABBR  = N_("Short Result");
const char *COLHDR_LATTRIBUTES  = N_("Left Attributes");
const char *COLHDR_RATTRIBUTES  = N_("Right Attributes");
const char *COLHDR_MATTRIBUTES  = N_("Middle Attributes");
const char *COLHDR_LEOL_TYPE    = N_("Left EOL");
const char *COLHDR_MEOL_TYPE    = N_("Middle EOL");
const char *COLHDR_REOL_TYPE    = N_("Right EOL");
const char *COLHDR_LENCODING    = N_("Left Encoding");
const char *COLHDR_RENCODING    = N_("Right Encoding");
const char *COLHDR_MENCODING    = N_("Middle Encoding");
const char *COLHDR_NIDIFFS      = N_("Ignored Diff");
const char *COLHDR_NSDIFFS      = N_("Differences");
const char *COLHDR_BINARY       = NC_("DirView|ColumnHeader", "Binary");
const char *COLHDR_UNPACKER     = N_("Unpacker");
const char *COLHDR_PREDIFFER    = N_("Prediffer");
#ifdef SHOW_DIFFITEM_DEBUG_INFO
const char *COLHDR_DEBUG_DIFFCODE    = N_("[Debug]diffcode");
const char *COLHDR_DEBUG_CUSTOMFLAGS = N_("[Debug]customFlags");
const char *COLHDR_DEBUG_THIS        = N_("[Debug]this");
const char *COLHDR_DEBUG_PARENT      = N_("[Debug]parent");
const char *COLHDR_DEBUG_CHILDREN    = N_("[Debug]children");
const char *COLHDR_DEBUG_FLINK       = N_("[Debug]Flink");
const char *COLHDR_DEBUG_BLINK       = N_("[Debug]Blink");
#endif // SHOW_DIFFITEM_DEBUG_INFO

const char *COLDESC_FILENAME    = N_("Filename or folder name.");
const char *COLDESC_DIR         = N_("Subfolder name when subfolders are included.");
const char *COLDESC_RESULT      = N_("Comparison result, long form.");
const char *COLDESC_LTIMEM      = N_("Left side modification date.");
const char *COLDESC_RTIMEM      = N_("Right side modification date.");
const char *COLDESC_MTIMEM      = N_("Middle side modification date.");
const char *COLDESC_EXTENSION   = N_("File's extension.");
const char *COLDESC_LSIZE       = N_("Left file size in bytes.");
const char *COLDESC_RSIZE       = N_("Right file size in bytes.");
const char *COLDESC_MSIZE       = N_("Middle file size in bytes.");
const char *COLDESC_LSIZE_SHORT = N_("Left file size abbreviated.");
const char *COLDESC_RSIZE_SHORT = N_("Right file size abbreviated.");
const char *COLDESC_MSIZE_SHORT = N_("Middle file size abbreviated.");
const char *COLDESC_LTIMEC      = N_("Left side creation time.");
const char *COLDESC_RTIMEC      = N_("Right side creation time.");
const char *COLDESC_MTIMEC      = N_("Middle side creation time.");
const char *COLDESC_NEWER       = N_("Tells which side has newer modification date.");
const char *COLDESC_LVERSION    = N_("Left side file version, only for some file types.");
const char *COLDESC_RVERSION    = N_("Right side file version, only for some file types.");
const char *COLDESC_MVERSION    = N_("Middle side file version, only for some file types.");
const char *COLDESC_RESULT_ABBR = N_("Short comparison result.");
const char *COLDESC_LATTRIBUTES = N_("Left side attributes.");
const char *COLDESC_RATTRIBUTES = N_("Right side attributes.");
const char *COLDESC_MATTRIBUTES = N_("Middle side attributes.");
const char *COLDESC_LEOL_TYPE   = N_("Left side file EOL type.");
const char *COLDESC_REOL_TYPE   = N_("Right side file EOL type.");
const char *COLDESC_MEOL_TYPE   = N_("Middle side file EOL type.");
const char *COLDESC_LENCODING   = N_("Left side encoding.");
const char *COLDESC_RENCODING   = N_("Right side encoding.");
const char *COLDESC_MENCODING   = N_("Middle side encoding.");
const char *COLDESC_NIDIFFS     = N_("Number of ignored differences in file. These differences are ignored by WinMerge and cannot be merged.");
const char *COLDESC_NSDIFFS     = N_("Number of differences in file. This number does not include ignored differences.");
const char *COLDESC_BINARY      = N_("Shows an asterisk (*) if the file is binary.");
const char *COLDESC_UNPACKER    = N_("Unpacker plugin name or pipeline.");
const char *COLDESC_PREDIFFER   = N_("Prediffer plugin name or pipeline.");
#ifdef SHOW_DIFFITEM_DEBUG_INFO
const char *COLDESC_DEBUG_DIFFCODE    = N_("Compare result");
const char *COLDESC_DEBUG_CUSTOMFLAGS = N_("ViewCustomFlags flags");
const char *COLDESC_DEBUG_THIS        = N_("This item");
const char *COLDESC_DEBUG_PARENT      = N_("Parent of current item");
const char *COLDESC_DEBUG_CHILDREN    = N_("Link to first child of this item");
const char *COLDESC_DEBUG_FLINK       = N_("Forward \"sibling\" link");
const char *COLDESC_DEBUG_BLINK       = N_("Backward \"sibling\" link");
#endif // SHOW_DIFFITEM_DEBUG_INFO
}

/**
 * @brief Function to compare two int64_t's for a sort
 */
static int cmp64(int64_t i1, int64_t i2)
{
	if (i1==i2) return 0;
	return i1>i2 ? 1 : -1;
}

/**
 * @brief Function to compare two uint64_t's for a sort
 */
static int cmpu64(uint64_t i1, uint64_t i2)
{
	if (i1==i2) return 0;
	return i1>i2 ? 1 : -1;
}
/**
 * @brief Function to compare two diffcodes for a sort
 * @todo How shall we order diff statuses?
 */
static int cmpdiffcode(unsigned diffcode1, unsigned diffcode2)
{
	bool same1 = (diffcode1 & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SAME;
	bool same2 = (diffcode2 & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SAME;
	if (same1 && !same2)
		return 1;
	if (!same1 && same2)
		return -1;
	return diffcode1 - diffcode2;
}
/**
 * @brief Formats a size as a short string.
 *
 * MakeShortSize(500) = "500b"
 * MakeShortSize(1024) = "1Kb"
 * MakeShortSize(12000) = "1.7Kb"
 * MakeShortSize(200000) = "195Kb"
 * @param [in] size File's size to convert.
 * @return Size string with localized suffix.
 * @note Localized suffix strings are read from resource.
 * @todo Can't handle > terabyte filesizes.
 */
static String MakeShortSize(int64_t size)
{
	tchar_t buffer[48];
	if (size < 1024)
		return strutils::format(_T("%d B"), static_cast<int>(size));
	else
		StrFormatByteSize64(size, buffer, static_cast<unsigned>(std::size(buffer)));
	return buffer;
}

/**
 * @name Functions to format content of each type of column.
 * These functions all receive two parameters, a pointer to CDiffContext.
 * which contains general compare information. And a void pointer whose type
 * depends on column to format. Function to call for each column, and
 * parameter for the function are defined in static DirColInfo f_cols table.
 */
/* @{ */
/**
 * @brief Format Filename column data.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
template<class Type>
static Type ColFileNameGet(const CDiffContext * pCtxt, const void *p, int) //sfilename
{
	assert(pCtxt != nullptr && p != nullptr);

	int nDirs = pCtxt->GetCompareDirs();

	const DIFFITEM* pDiffItem = static_cast<const DIFFITEM*>(p);
	const DiffFileInfo* pDiffFileInfo = pDiffItem->diffFileInfo;

	bool bExist[3] = {};
	for (int i = 0; i < nDirs; i++)
		bExist[i] = (pDiffItem->diffcode.exists(i) && (!pDiffFileInfo[i].filename.get().empty()));

	bool bIsSameName = true;
	int index = -1;
	for (int i = 0; i < nDirs; i++)
	{
		if (bExist[i])
		{
			if (index == -1)
				index = i;
			else if (pDiffFileInfo[i].filename != pDiffFileInfo[index].filename)
			{
				bIsSameName = false;
				break;
			}
		}
	}

	if (bIsSameName)
	{
		if (index == -1)
			index = 0;
		return pDiffFileInfo[index].filename;
	}
	else
	{
		if (nDirs < 3)
			return static_cast<Type>(pDiffFileInfo[0].filename.get() + _T("|") + pDiffFileInfo[1].filename.get());
		else
		{
			String none = _("<None>");
			return static_cast<Type>((bExist[0] ? pDiffFileInfo[0].filename.get() : none) + _T("|")
				+ (bExist[1] ? pDiffFileInfo[1].filename.get() : none) + _T("|")
				+ (bExist[2] ? pDiffFileInfo[2].filename.get() : none));
		}
	}
}

/**
 * @brief Format Extension column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColExtGet(const CDiffContext *, const void *p, int) //sfilename
{
	const DIFFITEM &di = *static_cast<const DIFFITEM*>(p);
	// We don't show extension for folder names
	if (di.diffcode.isDirectory())
		return _T("");
	const String &r = di.diffFileInfo[0].filename;
	String s = paths::FindExtension(r);
	return s.c_str() + tc::tcsspn(s.c_str(), _T("."));
}

/**
 * @brief Format Folder column data.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColPathGet(const CDiffContext * pCtxt, const void *p, int)
{
	assert(pCtxt != nullptr && p != nullptr);

	int nDirs = pCtxt->GetCompareDirs();

	const DIFFITEM &di = *static_cast<const DIFFITEM*>(p);

	if (nDirs < 3)
	{
		String s = di.diffFileInfo[1].path;
		const String& t = di.diffFileInfo[0].path;
		
		// If we have unique path, just print the existing path name
		if (s.length() == 0 || t.length() == 0)
		{
			if (s.length() == 0)
				return t;
			else
				return s;
		}

		size_t i = 0, j = 0;
		do
		{
			const tchar_t* pi = tc::tcschr(s.c_str() + i, '\\');
			const tchar_t* pj = tc::tcschr(t.c_str() + j, '\\');
			size_t i_ahead = (pi != nullptr ? pi - s.c_str() : std::string::npos);
			size_t j_ahead = (pj != nullptr ? pj - t.c_str() : std::string::npos);
			size_t length_s = ((i_ahead != std::string::npos ? i_ahead : s.length()) - i);
			size_t length_t = ((j_ahead != std::string::npos ? j_ahead : t.length()) - j);
			if (length_s != length_t ||
				memcmp(s.c_str() + i, t.c_str() + j, length_s) != 0)
			{
				String u(t.c_str() + j, length_t + 1);
				u[length_t] = '|';
				s.insert(i, u);
				i_ahead += u.length();
			}
			i = i_ahead + 1;
			j = j_ahead + 1;
		} while (i && j);
		if (s.empty())
			s = _T(".");
		return s;
	}
	else
	{
		// If we have unique path, just print the existing path name
		const DiffFileInfo* pDiffFileInfo = di.diffFileInfo;
		if (pDiffFileInfo[0].path == pDiffFileInfo[1].path && pDiffFileInfo[0].path == pDiffFileInfo[2].path)
			return pDiffFileInfo[0].path;

		String s;
		const std::vector<const DIFFITEM*> ancestors = di.GetAncestors();
		size_t depth = ancestors.size();
		for (int i = 0; i < depth; i++)
		{
			if (i > 0)
				s += _T("\\");
			s += ColFileNameGet<String>(pCtxt, ancestors[i], 0);
		}
		return s;
	}
}

/**
 * @brief Format Result column data.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColStatusGet(const CDiffContext *pCtxt, const void *p, int)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM*>(p);
	int nDirs = pCtxt->GetCompareDirs();
	// Note that order of items does matter. We must check for
	// skipped items before unique items, for example, so that
	// skipped unique items are labeled as skipped, not unique.
	String s;
	bool bAddCompareFlags3WayString = false;
	if (di.diffcode.isResultError())
	{
		s = _("Unable to compare files");
	}
	else if (di.diffcode.isResultAbort())
	{
		s = _("Item aborted");
	}
	else if (di.diffcode.isResultFiltered())
	{
		if (di.diffcode.isDirectory())
			s = _("Folder skipped");
		else
			s = _("File skipped");
	}
	else if (di.diffcode.isSideFirstOnly())
	{
		s = strutils::format_string1(_("Left only: %1"),
				di.getFilepath(0, pCtxt->GetNormalizedLeft()));
	}
	else if (di.diffcode.isSideSecondOnly())
	{
		if (nDirs < 3)
		{
			s = strutils::format_string1(_("Right only: %1"),
					di.getFilepath(1, pCtxt->GetNormalizedRight()));
		}
		else
		{
			s = strutils::format_string1(_("Middle only: %1"),
					di.getFilepath(1, pCtxt->GetNormalizedMiddle()));
		}
	}
	else if (di.diffcode.isSideThirdOnly())
	{
		s = strutils::format_string1(_("Right only: %1"),
				di.getFilepath(2, pCtxt->GetNormalizedRight()));
	}
	else if (nDirs > 2 && !di.diffcode.existsFirst())
	{
		s = strutils::format_string1(_("Does not exist in %1"),
				pCtxt->GetNormalizedLeft());
		bAddCompareFlags3WayString = true;
	}
	else if (nDirs > 2 && !di.diffcode.existsSecond())
	{
		s = strutils::format_string1(_("Does not exist in %1"),
				pCtxt->GetNormalizedMiddle());
		bAddCompareFlags3WayString = true;
	}
	else if (nDirs > 2 && !di.diffcode.existsThird())
	{
		s = strutils::format_string1(_("Does not exist in %1"),
				pCtxt->GetNormalizedRight());
		bAddCompareFlags3WayString = true;
	}
	else if (di.diffcode.isResultSame())
	{
		if (di.diffcode.isText())
			s = _("Text files are identical");
		else if (di.diffcode.isBin())
			s = _("Binary files are identical");
		else if (di.diffcode.isImage())
			s = _("Image files are identical");
		else
			s = _("Identical");
	}
	else if (di.diffcode.isResultDiff()) // diff
	{
		if (di.diffcode.isText())
			s = _("Text files are different");
		else if (di.diffcode.isBin())
			s = _("Binary files are different");
		else if (di.diffcode.isImage())
			s = _("Image files are different");
		else if (di.diffcode.isDirectory())
			s = _("Folders are different");
		else
			s = _("Files are different");
		if (nDirs > 2)
			bAddCompareFlags3WayString = true;
	}
	if (bAddCompareFlags3WayString)
	{
		switch (di.diffcode.diffcode & DIFFCODE::COMPAREFLAGS3WAY)
		{
		case DIFFCODE::DIFF1STONLY: s += _(" (Middle and right are identical)"); break;
		case DIFFCODE::DIFF2NDONLY: s += _(" (Left and right are identical)"); break;
		case DIFFCODE::DIFF3RDONLY: s += _(" (Left and middle are identical)"); break;
		}
	}
	return s;
}

/**
 * @brief Format Date column data.
 * @param [in] p Pointer to integer (seconds since 1.1.1970).
 * @return String to show in the column.
 */
static String ColTimeGet(const CDiffContext *, const void *p, int)
{
	const int64_t r = *static_cast<const int64_t*>(p) / Timestamp::resolution();
	if (r)
		return locality::TimeString(&r);
	else
		return _T("");
}

/**
 * @brief Format Sizw column data.
 * @param [in] p Pointer to integer containing size in bytes.
 * @return String to show in the column.
 */
static String ColSizeGet(const CDiffContext *, const void *p, int)
{
	const int64_t &r = *static_cast<const int64_t*>(p);
	String s;
	if (r != -1)
	{
		s = locality::NumToLocaleStr(r);
	}
	return s;
}

/**
 * @brief Format Folder column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColSizeShortGet(const CDiffContext *, const void *p, int)
{
	const int64_t &r = *static_cast<const int64_t*>(p);
	String s;
	if (r != -1)
	{
		s = MakeShortSize(r);
	}
	return s;
}

/**
 * @brief Format Difference cout column data.
 * @param [in] p Pointer to integer having count of differences.
 * @return String to show in the column.
 */
static String ColDiffsGet(const CDiffContext *, const void *p, int)
{
	const int &r = *static_cast<const int*>(p);
	String s;
	if (r == CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE)
	{ // QuickCompare, unknown
		s = _T("*");
	}
	else if (r == CDiffContext::DIFFS_UNKNOWN)
	{ // Unique item
		s.clear();
	}
	else
	{
		s = locality::NumToLocaleStr(r);
	}
	return s;
}

/**
 * @brief Format Newer/Older column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColNewerGet(const CDiffContext *pCtxt, const void *p, int)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	if (pCtxt->GetCompareDirs() < 3)
	{
		if (di.diffcode.isSideFirstOnly())
		{
			return _T("<--");
		}
		if (di.diffcode.isSideSecondOnly())
		{
			return _T("-->");
		}
		if (di.diffFileInfo[0].mtime != 0 && di.diffFileInfo[1].mtime != 0)
		{
			if (di.diffFileInfo[0].mtime > di.diffFileInfo[1].mtime)
			{
				return _T("<-");
			}
			if (di.diffFileInfo[0].mtime < di.diffFileInfo[1].mtime)
			{
				return _T("->");
			}
			return _T("==");
		}
		return _T("***");
	}
	else
	{
		String res;
		int sortno[3] = {0, 1, 2};
		Timestamp sorttime[3] = {di.diffFileInfo[0].mtime, di.diffFileInfo[1].mtime, di.diffFileInfo[2].mtime};
		for (int i = 0; i < 3; i++)
		{
			for (int j = i; j < 3; j++)
			{
				if (sorttime[i] < sorttime[j])
				{
					swap(sorttime[i], sorttime[j]);
					swap(sortno[i], sortno[j]);
				}
			}
		}
		res = _T("LMR")[sortno[0]];
		res += sorttime[0] == sorttime[1] ? _T("==") : _T("<-");
		res += _T("LMR")[sortno[1]];
		res += sorttime[1] == sorttime[2] ? _T("==") : _T("<-");
		res += _T("LMR")[sortno[2]];
		return res;
	}
}

/**
 * @brief Format Version info to string.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] pdi Pointer to DIFFITEM.
 * @param [in] bLeft Is the item left-size item?
 * @return String proper to show in the GUI.
 */
static String GetVersion(const CDiffContext * pCtxt, const DIFFITEM *pdi, int nIndex)
{
	DIFFITEM &di = const_cast<DIFFITEM &>(*pdi);
	DiffFileInfo & dfi = di.diffFileInfo[nIndex];
	if (dfi.version.IsCleared())
	{
		pCtxt->UpdateVersion(di, nIndex);
	}
	return dfi.version.GetFileVersionString();
}

static uint64_t GetVersionQWORD(const CDiffContext * pCtxt, const DIFFITEM *pdi, int nIndex)
{
	DIFFITEM &di = const_cast<DIFFITEM &>(*pdi);
	DiffFileInfo & dfi = di.diffFileInfo[nIndex];
	if (dfi.version.IsCleared())
	{
		pCtxt->UpdateVersion(di, nIndex);
	}
	return dfi.version.GetFileVersionQWORD();
}

/**
 * @brief Format Version column data
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColVersionGet(const CDiffContext * pCtxt, const void *p, int opt)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetVersion(pCtxt, &di, opt);
}

/**
 * @brief Format Short Result column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColStatusAbbrGet(const CDiffContext *pCtxt, const void *p, int opt)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	const char *id = 0;
	int nDirs = pCtxt->GetCompareDirs();

	// Note that order of items does matter. We must check for
	// skipped items before unique items, for example, so that
	// skipped unique items are labeled as skipped, not unique.
	if (di.diffcode.isResultError())
	{
		id = N_("Error");
	}
	else if (di.diffcode.isResultAbort())
	{
		id = N_("Item aborted");
	}
	else if (di.diffcode.isResultFiltered())
	{
		if (di.diffcode.isDirectory())
			id = N_("Folder skipped");
		else
			id = N_("File skipped");
	}
	else if (di.diffcode.isSideFirstOnly())
	{
		id = N_("Left Only");
	}
	else if (di.diffcode.isSideSecondOnly())
	{
		if (nDirs < 3)
			id = N_("Right Only");
		else
			id = N_("Middle Only");
	}
	else if (di.diffcode.isSideThirdOnly())
	{
		id = N_("Right Only");
	}
	else if (nDirs > 2 && !di.diffcode.existsFirst())
	{
		id = N_("No item in left");
	}
	else if (nDirs > 2 && !di.diffcode.existsSecond())
	{
		id = N_("No item in middle");
	}
	else if (nDirs > 2 && !di.diffcode.existsThird())
	{
		id = N_("No item in right");
	}
	else if (di.diffcode.isResultSame())
	{
		id = N_("Identical");
	}
	else if (di.diffcode.isResultDiff())
	{
		id = N_("Different");
	}

	return id ? tr(id) : _T("");
}

/**
 * @brief Format Binary column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColBinGet(const CDiffContext *, const void *p, int)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);

	if (di.diffcode.isBin())
		return _T("*");
	else
		return _T("");
}

/**
 * @brief Format File Attributes column data.
 * @param [in] p Pointer to file flags class.
 * @return String to show in the column.
 */
static String ColAttrGet(const CDiffContext *, const void *p, int)
{
	const FileFlags &r = *static_cast<const FileFlags *>(p);
	return r.ToString();
}

/**
 * @brief Format File Encoding column data.
 * @param [in] p Pointer to file information.
 * @return String to show in the column.
 */
static String ColEncodingGet(const CDiffContext *, const void *p, int)
{
	const DiffFileInfo &r = *static_cast<const DiffFileInfo *>(p);
	return r.encoding.GetName();
}

/**
 * @brief Format EOL type to string.
 * @param [in] p Pointer to DIFFITEM.
 * @param [in] bLeft Are we formatting left-side file's data?
 * @return EOL type as as string.
 */
static String GetEOLType(const CDiffContext *, const void *p, int index)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	const DiffFileInfo & dfi = di.diffFileInfo[index];
	const FileTextStats &stats = dfi.m_textStats;

	if (stats.ncrlfs == 0 && stats.ncrs == 0 && stats.nlfs == 0)
	{
		return String();
	}
	if (di.diffcode.isBin())
	{
		return tr("EOL Type", "Binary");
	}

	char *id = 0;
	if (stats.ncrlfs > 0 && stats.ncrs == 0 && stats.nlfs == 0)
	{
		id = N_("Win");
	}
	else if (stats.ncrlfs == 0 && stats.ncrs > 0 && stats.nlfs == 0)
	{
		id = N_("Mac");
	}
	else if (stats.ncrlfs == 0 && stats.ncrs == 0 && stats.nlfs > 0)
	{
		id = N_("Unix");
	}
	else
	{
		return strutils::format(_T("%s:%d/%d/%d"),
			_("Mixed"),
			stats.ncrlfs, stats.ncrs, stats.nlfs);
	}
	
	return tr(id);
}

/**
 * @brief Format EOL type column data
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColEOLTypeGet(const CDiffContext * pCtxt, const void *p, int opt)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetEOLType(pCtxt, &di, opt);
}

static String ColPluginPipelineGet(const CDiffContext* pCtxt, const void *p, int opt)
{
	const DIFFITEM& di = *static_cast<const DIFFITEM*>(p);
	if (di.diffcode.isDirectory())
		return _T("");
	PackingInfo* pInfoUnpacker = nullptr;
	PrediffingInfo * pInfoPrediffer = nullptr;
	String filteredFilenames = pCtxt->GetFilteredFilenames(di);
	const_cast<CDiffContext *>(pCtxt)->FetchPluginInfos(filteredFilenames, &pInfoUnpacker, &pInfoPrediffer);
	if (opt != 0)
		return pInfoUnpacker ? pInfoUnpacker->GetPluginPipeline() : _T("");
	else
		return pInfoPrediffer ? pInfoPrediffer->GetPluginPipeline() : _T("");
}

#ifdef SHOW_DIFFITEM_DEBUG_INFO
/**
 * @brief Format "[Debug]diffcode" column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColDebugDiffCodeGet(const CDiffContext *, const void* p, int)
{
	const DIFFITEM& di = *static_cast<const DIFFITEM*>(p);
	unsigned int diffcode = di.diffcode.diffcode;

	String s = strutils::format(_T("0x%08x"), diffcode);

	std::vector<String> flags;
	if (diffcode & DIFFCODE::FIRST)
		flags.push_back(_T("FIRST"));
	if (diffcode & DIFFCODE::SECOND)
		flags.push_back(_T("SECOND"));
	if (diffcode & DIFFCODE::THIRD)
		flags.push_back(_T("THIRD"));
	if (diffcode & DIFFCODE::THREEWAY)
		flags.push_back(_T("THREEWAY"));
	if (diffcode & DIFFCODE::NEEDSCAN)
		flags.push_back(_T("NEEDSCAN"));
	if (diffcode & DIFFCODE::SKIPPED)
		flags.push_back(_T("SKIPPED"));
	switch (diffcode & DIFFCODE::COMPAREFLAGS3WAY)
	{
	case DIFFCODE::DIFF1STONLY: flags.push_back(_T("DIFF1STONLY")); break;
	case DIFFCODE::DIFF2NDONLY: flags.push_back(_T("DIFF2NDONLY")); break;
	case DIFFCODE::DIFF3RDONLY: flags.push_back(_T("DIFF3RDONLY")); break;
	}
	switch (diffcode & DIFFCODE::COMPAREFLAGS)
	{
	case DIFFCODE::DIFF: flags.push_back(_T("DIFF")); break;
	case DIFFCODE::SAME: flags.push_back(_T("SAME")); break;
	case DIFFCODE::CMPERR: flags.push_back(_T("CMPERR")); break;
	case DIFFCODE::CMPABORT: flags.push_back(_T("CMPABORT")); break;
	}
	if (diffcode & DIFFCODE::FILE)
		flags.push_back(_T("FILE"));
	if (diffcode & DIFFCODE::DIR)
		flags.push_back(_T("DIR"));
	if (diffcode & DIFFCODE::TEXT)
		flags.push_back(_T("TEXT"));
	if (diffcode & DIFFCODE::BIN)
		flags.push_back(_T("BIN"));
	if (diffcode & DIFFCODE::BINSIDE1)
		flags.push_back(_T("BINSIDE1"));
	if (diffcode & DIFFCODE::BINSIDE2)
		flags.push_back(_T("BINSIDE2"));
	if (diffcode & DIFFCODE::BINSIDE3)
		flags.push_back(_T("BINSIDE3"));
	if (diffcode & DIFFCODE::IMAGE)
		flags.push_back(_T("IMAGE"));

	if (!flags.empty())
	{
		s += _T(" (");
		for (size_t i = 0; i < flags.size(); i++)
		{
			s += flags[i];
			if (i < flags.size() - 1)
				s += _T(" ");
		}
		s += _T(")");
	}

	return s;
}
#endif // SHOW_DIFFITEM_DEBUG_INFO

#ifdef SHOW_DIFFITEM_DEBUG_INFO
/**
 * @brief Format "[Debug]customFlags" column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColDebugCustomFlagsGet(const CDiffContext *, const void* p, int)
{
	const DIFFITEM& di = *static_cast<const DIFFITEM*>(p);
	unsigned customFlags = di.customFlags;

	String s = strutils::format(_T("0x%08x"), customFlags);

	std::vector<String> flags;
	if (customFlags & ViewCustomFlags::EXPANDED)
		flags.push_back(_T("EXPANDED"));
	if (customFlags & ViewCustomFlags::HIDDEN)
		flags.push_back(_T("HIDDEN"));
	if (customFlags & ViewCustomFlags::VISIBLE)
		flags.push_back(_T("VISIBLE"));

	if (!flags.empty())
	{
		s += _T(" (");
		for (size_t i = 0; i < flags.size(); i++)
		{
			s += flags[i];
			if (i < flags.size() - 1)
				s += _T(" ");
		}
		s += _T(")");
	}

	return s;
}
#endif // SHOW_DIFFITEM_DEBUG_INFO

#ifdef SHOW_DIFFITEM_DEBUG_INFO
/**
 * @brief Format "[Debug]this", "[Debug]parent", "[Debug]children", "[Debug]Flink" and "[Debug]Blink" column data.
 * @param [in] p Pointer to DIFFITEM.
 * @param [in] opt Type of data to show (0:"[Debug]this", 1:"[Debug]parent", 2:"[Debug]children", 3:"[Debug]Flink", 4:"[Debug]Blink")
 * @return String to show in the column.
 */
static String ColDebugNodeGet(const CDiffContext *, const void* p, int opt)
{
	const DIFFITEM& di = *static_cast<const DIFFITEM*>(p);

	const DIFFITEM* pdi = nullptr;
	switch (opt)
	{
	case 0: pdi = &di; break;
	case 1: pdi = di.GetParentLink(); break;
	case 2: pdi = di.GetFirstChild(); break;
	case 3: pdi = di.GetFwdSiblingLink(); break;
	case 4: pdi = di.GetBackwardSiblingLink(); break;
	}

	return strutils::format(_T("%p"), pdi);
}
#endif // SHOW_DIFFITEM_DEBUG_INFO

static String ColPropertyGet(const CDiffContext *pCtxt, const void *p, int opt)
{
	const DiffFileInfo &dfi = *static_cast<const DiffFileInfo *>(p);
	PropertyValues* pprops = dfi.m_pAdditionalProperties.get();
	return (pprops != nullptr && opt < pprops->GetSize()) ? pCtxt->m_pPropertySystem->FormatPropertyValue(*pprops, opt) : _T("");
}

static const DuplicateInfo *GetDuplicateInfo(const CDiffContext* pCtxt, const DiffFileInfo& dfi, int index)
{
	PropertyValues* pprops = dfi.m_pAdditionalProperties.get();
	if (!pprops || index >= pprops->GetSize() || !pprops->IsHashValue(index) || pCtxt->m_duplicateValues.empty())
		return nullptr;
	auto it = pCtxt->m_duplicateValues[index].find(pprops->GetHashValue(index));
	if (it == pCtxt->m_duplicateValues[index].end())
		return nullptr;
	return &(it->second);
}

static String ColPropertyDuplicateCountGet(const CDiffContext *pCtxt, const void *p, int opt)
{
	const int index = opt & 0xffff;
	const int pane = opt >> 16;
	const DiffFileInfo &dfi = *static_cast<const DiffFileInfo *>(p);
	const DuplicateInfo *info = GetDuplicateInfo(pCtxt, dfi, index);
	return (!info || info->count[pane] <= 1) ? _T("") :
		strutils::format(_("%s: %d"), strutils::format(_("Group%d"), info->groupid), info->count[pane] - 1);
}

static String ColAllPropertyGet(const CDiffContext *pCtxt, const void *p, int opt)
{
	const DIFFITEM& di = *static_cast<const DIFFITEM*>(p);
	bool equal = true;
	PropertyValues* pFirstProps = di.diffFileInfo[0].m_pAdditionalProperties.get();
	for (int i = 1; i < pCtxt->GetCompareDirs(); ++i)
	{
		PropertyValues* pprops = di.diffFileInfo[i].m_pAdditionalProperties.get();
		if (pFirstProps && pprops)
		{
			if (PropertyValues::CompareValues(*pFirstProps, *pprops, opt) != 0)
				equal = false;
		}
	}
	if (equal)
		return pFirstProps ? pCtxt->m_pPropertySystem->FormatPropertyValue(*pFirstProps, opt) : _T("");

	std::vector<String> values;
	for (int i = 0; i < pCtxt->GetCompareDirs(); ++i)
	{
		PropertyValues* pprops = di.diffFileInfo[i].m_pAdditionalProperties.get();
		if (pCtxt->GetCompareDirs() == 3 || di.diffcode.exists(i))
			values.push_back(pprops ? pCtxt->m_pPropertySystem->FormatPropertyValue(*pprops, opt) : _T(""));
	}
	return strutils::join(values.begin(), values.end(), _T("|"));
}

static String ColPropertyDiffGetEx(const CDiffContext *pCtxt, const void *p, int opt, bool addNumDiff, bool& numeric, int64_t *pnumdiff)
{
	const DIFFITEM& di = *static_cast<const DIFFITEM*>(p);
	if (di.diffcode.isDirectory())
		return _T("");
	if (di.diffcode.isResultError())
		return _("Error");
	if (di.diffcode.isResultAbort())
		return _("Item aborted");
	if (di.diffcode.isResultFiltered())
		return _("File skipped");
	PropertyValues* pFirstProps = di.diffFileInfo[0].m_pAdditionalProperties.get();
	if (!pFirstProps)
		return _T("");
	int64_t diff = 0;
	bool equal = true;
	numeric = false;
	const int nDirs = pCtxt->GetCompareDirs();
	for (int i = 1; i < nDirs; ++i)
	{
		PropertyValues* pprops = di.diffFileInfo[i].m_pAdditionalProperties.get();
		if (pFirstProps && pprops)
		{
			diff = PropertyValues::DiffValues(*pFirstProps, *pprops, opt, numeric);
			if (diff != 0)
				equal = false;
			if (pnumdiff)
				*pnumdiff = diff;
		}
	}
	String result;
	if (!di.diffcode.existAll())
	{
		bool allempty = true;
		for (int i = 0; i < nDirs; ++i)
		{
			if (di.diffcode.exists(i))
			{
				PropertyValues* pprops = di.diffFileInfo[i].m_pAdditionalProperties.get();
				if (pprops && !pprops->IsEmptyValue(opt))
					allempty = false;
			}
		}
		if (!allempty)
		{
			if (di.diffcode.isSideFirstOnly())
				result = _("Left Only");
			else if (di.diffcode.isSideSecondOnly())
			{
				if (nDirs < 3)
					result = _("Right Only");
				else
					result = _("Middle Only");
			}
			else if (di.diffcode.isSideThirdOnly())
				result = _("Right Only");
			else if (nDirs > 2 && !di.diffcode.existsFirst())
				result = _("No item in left");
			else if (nDirs > 2 && !di.diffcode.existsSecond())
				result = _("No item in middle");
			else if (nDirs > 2 && !di.diffcode.existsThird())
				result = _("No item in right");
		}
	}
	else
	{
		result = !equal ? _("Different") :
			(!pFirstProps->IsEmptyValue(opt) ? _("Identical") : _T(""));
	}
	if (nDirs == 2 && numeric && addNumDiff)
		result += strutils::format(_T(" (%+ld)"), diff);
	return result;
}

static String ColPropertyDiffGet(const CDiffContext* pCtxt, const void* p, int opt)
{
	bool numeric = false;
	return ColPropertyDiffGetEx(pCtxt, p, opt, true, numeric, nullptr);
}

static String ColPropertyMoveGet(const CDiffContext *pCtxt, const void *p, int opt)
{
	const DIFFITEM& di = *static_cast<const DIFFITEM*>(p);
	if (di.diffcode.isDirectory())
		return _T("");
	std::vector<String> list;
	const int nDirs = pCtxt->GetCompareDirs();
	for (int i = 0; i < nDirs; ++i)
	{
		if (di.diffcode.exists(i))
		{
			PropertyValues* pprops = di.diffFileInfo[i].m_pAdditionalProperties.get();
			if (pprops && !pCtxt->m_duplicateValues.empty())
			{
				auto it = pCtxt->m_duplicateValues[opt].find(pprops->GetHashValue(opt));
				if (it != pCtxt->m_duplicateValues[opt].end())
				{
					if (it->second.groupid != 0 && it->second.nonpaired)
					{
						int count = 0;
						for (int j = 0; j < nDirs; ++j)
						{
							if (it->second.count[j] > 0)
								++count;
						}
						if (count > 1)
							list.push_back(strutils::format(_("Group%d"), it->second.groupid));
					}
				}
			}
		}
	}
	return strutils::join(list.begin(), list.end(), _T("|"));
}

/**
 * @}
 */

/**
 * @name Functions to sort each type of column info.
 * These functions are used to sort information in folder compare GUI. Each
 * column info (type) has its own function to compare the data. Each
 * function receives three parameters:
 * - pointer to compare context
 * - two parameters for data to compare (type varies)
 * Return value is -1, 0, or 1, where 0 means datas are identical.
 */
/* @{ */
/**
 * @brief Compare file names.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM having first name to compare.
 * @param [in] q Pointer to DIFFITEM having second name to compare.
 * @return Compare result.
 */
static int ColFileNameSort(const CDiffContext *pCtxt, const void *p, const void *q, int)
{
	const DIFFITEM &ldi = *static_cast<const DIFFITEM *>(p);
	const DIFFITEM &rdi = *static_cast<const DIFFITEM *>(q);
	if (ldi.diffcode.isDirectory() && !rdi.diffcode.isDirectory())
		return -1;
	if (!ldi.diffcode.isDirectory() && rdi.diffcode.isDirectory())
		return 1;
	return strutils::compare_logical(ColFileNameGet<boost::flyweight<String>>(pCtxt, p, 0), ColFileNameGet<boost::flyweight<String>>(pCtxt, q, 0));
}

/**
 * @brief Compare file name extensions.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p First file name extension to compare.
 * @param [in] q Second file name extension to compare.
 * @return Compare result.
 */
static int ColExtSort(const CDiffContext *pCtxt, const void *p, const void *q, int)
{
	const DIFFITEM &ldi = *static_cast<const DIFFITEM *>(p);
	const DIFFITEM &rdi = *static_cast<const DIFFITEM *>(q);
	if (ldi.diffcode.isDirectory() && !rdi.diffcode.isDirectory())
		return -1;
	if (!ldi.diffcode.isDirectory() && rdi.diffcode.isDirectory())
		return 1;
	return strutils::compare_logical(ColExtGet(pCtxt, p, 0), ColExtGet(pCtxt, q, 0));
}

/**
 * @brief Compare folder names.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM having first folder name to compare.
 * @param [in] q Pointer to DIFFITEM having second folder name to compare.
 * @return Compare result.
 */
static int ColPathSort(const CDiffContext *pCtxt, const void *p, const void *q, int)
{
	return strutils::compare_logical(ColPathGet(pCtxt, p, 0), ColPathGet(pCtxt, q, 0));
}

/**
 * @brief Compare compare results.
 * @param [in] p Pointer to DIFFITEM having first result to compare.
 * @param [in] q Pointer to DIFFITEM having second result to compare.
 * @return Compare result.
 */
static int ColStatusSort(const CDiffContext *, const void *p, const void *q, int)
{
	const DIFFITEM &ldi = *static_cast<const DIFFITEM *>(p);
	const DIFFITEM &rdi = *static_cast<const DIFFITEM *>(q);
	return cmpdiffcode(rdi.diffcode.diffcode, ldi.diffcode.diffcode);
}

/**
 * @brief Compare file times.
 * @param [in] p First time to compare.
 * @param [in] q Second time to compare.
 * @return Compare result.
 */
static int ColTimeSort(const CDiffContext *, const void *p, const void *q, int)
{
	const int64_t &r = *static_cast<const int64_t*>(p);
	const int64_t &s = *static_cast<const int64_t*>(q);
	return cmp64(r, s);
}

/**
 * @brief Compare file sizes.
 * @param [in] p First size to compare.
 * @param [in] q Second size to compare.
 * @return Compare result.
 */
static int ColSizeSort(const CDiffContext *, const void *p, const void *q, int)
{
	const int64_t &r = *static_cast<const int64_t*>(p);
	const int64_t &s = *static_cast<const int64_t*>(q);
	return cmp64(r, s);
}

/**
 * @brief Compare difference counts.
 * @param [in] p First count to compare.
 * @param [in] q Second count to compare.
 * @return Compare result.
 */
static int ColDiffsSort(const CDiffContext *, const void *p, const void *q, int)
{
	const int &r = *static_cast<const int*>(p);
	const int &s = *static_cast<const int*>(q);
	return r - s;
}

/**
 * @brief Compare newer/older statuses.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM having first status to compare.
 * @param [in] q Pointer to DIFFITEM having second status to compare.
 * @return Compare result.
 */
static int ColNewerSort(const CDiffContext *pCtxt, const void *p, const void *q, int)
{
	return ColNewerGet(pCtxt, p, 0).compare(ColNewerGet(pCtxt, q, 0));
}

/**
 * @brief Compare file versions.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM having first version to compare.
 * @param [in] q Pointer to DIFFITEM having second version to compare.
 * @return Compare result.
 */
static int ColVersionSort(const CDiffContext *pCtxt, const void *p, const void *q, int opt)
{
	return cmpu64(GetVersionQWORD(pCtxt, reinterpret_cast<const DIFFITEM *>(p), opt), GetVersionQWORD(pCtxt, reinterpret_cast<const DIFFITEM *>(q), opt));
}

/**
 * @brief Compare binary statuses.
 * This function returns a comparison of binary results.
 * @param [in] p Pointer to DIFFITEM having first status to compare.
 * @param [in] q Pointer to DIFFITEM having second status to compare.
 * @return Compare result:
 * - if both items are text files or binary files: 0
 * - if left is text and right is binary: -1
 * - if left is binary and right is text: 1
 */
static int ColBinSort(const CDiffContext *, const void *p, const void *q, int)
{
	const DIFFITEM &ldi = *static_cast<const DIFFITEM *>(p);
	const DIFFITEM &rdi = *static_cast<const DIFFITEM *>(q);
	const bool i = ldi.diffcode.isBin();
	const bool j = rdi.diffcode.isBin();

	if (!i && !j)
		return 0;
	else if (i && !j)
		return 1;
	else if (!i && j)
		return -1;
	else
		return 0;
}

/**
 * @brief Compare file flags.
 * @param [in] p Pointer to first flag structure to compare.
 * @param [in] q Pointer to second flag structure to compare.
 * @return Compare result.
 */
static int ColAttrSort(const CDiffContext *, const void *p, const void *q, int)
{
	const FileFlags &r = *static_cast<const FileFlags *>(p);
	const FileFlags &s = *static_cast<const FileFlags *>(q);
	if (r.attributes == s.attributes)
		return 0;
	return r.attributes < s.attributes ? -1 : 1;
}

/**
 * @brief Compare file encodings.
 * @param [in] p Pointer to first structure to compare.
 * @param [in] q Pointer to second structure to compare.
 * @return Compare result.
 */
static int ColEncodingSort(const CDiffContext *, const void *p, const void *q, int)
{
	const DiffFileInfo &r = *static_cast<const DiffFileInfo *>(p);
	const DiffFileInfo &s = *static_cast<const DiffFileInfo *>(q);
	return FileTextEncoding::Collate(r.encoding, s.encoding);
}

/**
 * @brief Compare property values
 * @param [in] p Pointer to first structure to compare.
 * @param [in] q Pointer to second structure to compare.
 * @return Compare result.
 */
static int ColPropertySort(const CDiffContext *, const void *p, const void *q, int opt)
{
	const DiffFileInfo &r = *static_cast<const DiffFileInfo *>(p);
	const DiffFileInfo &s = *static_cast<const DiffFileInfo *>(q);
	if (!r.m_pAdditionalProperties && s.m_pAdditionalProperties)
		return -1;
	if (r.m_pAdditionalProperties && !s.m_pAdditionalProperties)
		return 1;
	if (!r.m_pAdditionalProperties && !s.m_pAdditionalProperties)
		return 0;
	return PropertyValues::CompareValues(*r.m_pAdditionalProperties, *s.m_pAdditionalProperties, opt);
}

/**
 * @brief Compare duplicate count
 * @param [in] p Pointer to first structure to compare.
 * @param [in] q Pointer to second structure to compare.
 * @return Compare result.
 */
static int ColPropertyDuplicateCountSort(const CDiffContext* pCtxt, const void* p, const void* q, int opt)
{
	const int index = opt & 0xffff;
	const int pane = opt >> 16;
	const DiffFileInfo& r = *static_cast<const DiffFileInfo*>(p);
	const DiffFileInfo& s = *static_cast<const DiffFileInfo*>(q);
	const DuplicateInfo* rinfo = GetDuplicateInfo(pCtxt, r, index);
	const DuplicateInfo* sinfo = GetDuplicateInfo(pCtxt, s, index);
	if (!rinfo && !sinfo)
		return 0;
	if (!rinfo && sinfo)
		return -1;
	if (rinfo && !sinfo)
		return 1;
	if (rinfo->count[pane] <= 1 && sinfo->count[pane] <= 1)
		return 0;
	if (rinfo->count[pane] <= 1 && sinfo->count[pane] > 1)
		return -1;
	if (rinfo->count[pane] > 1 && sinfo->count[pane] <= 1)
		return 1;
	if (rinfo->count[pane] == sinfo->count[pane])
	{
		if (rinfo->groupid == sinfo->groupid)
			return 0;
		return rinfo->groupid < sinfo->groupid ? -1 : 1;
	}
	return rinfo->count[pane] < sinfo->count[pane] ? -1 : 1;
}

/**
 * @brief Compare all property values
 * @param [in] p Pointer to first structure to compare.
 * @param [in] q Pointer to second structure to compare.
 * @return Compare result.
 */
static int ColAllPropertySort(const CDiffContext *pCtxt, const void *p, const void *q, int opt)
{
	const DIFFITEM &r = *static_cast<const DIFFITEM *>(p);
	const DIFFITEM &s = *static_cast<const DIFFITEM *>(q);
	for (int i = 0; i < pCtxt->GetCompareDirs(); ++i)
	{
		if (r.diffcode.exists(i))
		{
			for (int j = 0; j < pCtxt->GetCompareDirs(); ++j)
			{
				if (s.diffcode.exists(j))
				{
					if (!r.diffFileInfo[i].m_pAdditionalProperties && s.diffFileInfo[j].m_pAdditionalProperties)
						return -1;
					if (r.diffFileInfo[i].m_pAdditionalProperties && !s.diffFileInfo[j].m_pAdditionalProperties)
						return 1;
					if (!r.diffFileInfo[i].m_pAdditionalProperties && !s.diffFileInfo[j].m_pAdditionalProperties)
						return 0;
					int result = PropertyValues::CompareValues(*r.diffFileInfo[i].m_pAdditionalProperties, *s.diffFileInfo[j].m_pAdditionalProperties, opt);
					if (result != 0)
						return result;
				}
			}
		}
	}
	return 0;
}

/**
 * @brief Compare Hash(Diff) column values
 * @param [in] p Pointer to first structure to compare.
 * @param [in] q Pointer to second structure to compare.
 * @return Compare result.
 */
static int ColPropertyDiffSort(const CDiffContext *pCtxt, const void *p, const void *q, int opt)
{
	int64_t rnumdiff = 0;
	int64_t snumdiff = 0;
	bool rnumeric = false;
	bool snumeric = false;
	String r2 = ColPropertyDiffGetEx(pCtxt, p, opt, false, rnumeric, &rnumdiff);
	String s2 = ColPropertyDiffGetEx(pCtxt, q, opt, false, snumeric, &snumdiff);
	int result = strutils::compare_logical(r2, s2);
	if (pCtxt->GetCompareDirs() == 2 && result == 0 && rnumeric && snumeric)
	{
		if (rnumdiff == snumdiff)
			return 0;
		return rnumdiff < snumdiff ? -1 : 1;
	}
	return result;
}

/**
 * @brief Compare Hash(Move) column values
 * @param [in] p Pointer to first structure to compare.
 * @param [in] q Pointer to second structure to compare.
 * @return Compare result.
 */
static int ColPropertyMoveSort(const CDiffContext *pCtxt, const void *p, const void *q, int opt)
{
	String r2 = ColPropertyMoveGet(pCtxt, p, opt);
	String s2 = ColPropertyMoveGet(pCtxt, q, opt);
	return strutils::compare_logical(r2, s2);
}

/* @} */

#undef FIELD_OFFSET	// incorrect for Win32 as defined in WinNT.h
#define FIELD_OFFSET(type, field)    ((size_t)(LONG_PTR)&(((type *)nullptr)->field))

/**
 * @brief All existing folder compare columns.
 *
 * This table has information for all folder compare columns. Fields are
 * (in this order):
 *  - internal name
 *  - name resource ID: column's name shown in header
 *  - description resource ID: columns description text
 *  - custom function for getting column data
 *  - custom function for sorting column data
 *  - parameter for custom functions: DIFFITEM (if `nullptr`) or one of its fields
 *  - default column order number, -1 if not shown by default
 *  - ascending (`true`) or descending (`false`) default sort order
 *  - alignment of column contents: numbers are usually right-aligned
 */
static DirColInfo f_cols[] =
{
	{ _T("Name"), nullptr, COLHDR_FILENAME, COLDESC_FILENAME, &ColFileNameGet<String>, &ColFileNameSort, 0, 0, true, DirColInfo::ALIGN_LEFT },
	{ _T("Path"), "DirView|ColumnHeader", COLHDR_DIR, COLDESC_DIR, &ColPathGet, &ColPathSort, 0, 1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Status"), nullptr, COLHDR_RESULT, COLDESC_RESULT, &ColStatusGet, &ColStatusSort, 0, 2, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lmtime"), nullptr, COLHDR_LTIMEM, COLDESC_LTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].mtime), 3, false, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Rmtime"), nullptr, COLHDR_RTIMEM, COLDESC_RTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].mtime), 4, false, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Lctime"), nullptr, COLHDR_LTIMEC, COLDESC_LTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].ctime), -1, false, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Rctime"), nullptr, COLHDR_RTIMEC, COLDESC_RTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].ctime), -1, false, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Ext"), nullptr, COLHDR_EXTENSION, COLDESC_EXTENSION, &ColExtGet, &ColExtSort, 0, 5, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lsize"), nullptr, COLHDR_LSIZE, COLDESC_LSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].size), -1, false, DirColInfo::ALIGN_RIGHT, 0 },
	{ _T("Rsize"), nullptr, COLHDR_RSIZE, COLDESC_RSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].size), -1, false, DirColInfo::ALIGN_RIGHT, 1 },
	{ _T("LsizeShort"), nullptr, COLHDR_LSIZE_SHORT, COLDESC_LSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].size), -1, false, DirColInfo::ALIGN_RIGHT, 0 },
	{ _T("RsizeShort"), nullptr, COLHDR_RSIZE_SHORT, COLDESC_RSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].size), -1, false, DirColInfo::ALIGN_RIGHT, 1 },
	{ _T("Newer"), nullptr, COLHDR_NEWER, COLDESC_NEWER, &ColNewerGet, &ColNewerSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lversion"), nullptr, COLHDR_LVERSION, COLDESC_LVERSION, &ColVersionGet, &ColVersionSort, 0, -1, true, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Rversion"), nullptr, COLHDR_RVERSION, COLDESC_RVERSION, &ColVersionGet, &ColVersionSort, 0, -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("StatusAbbr"), nullptr, COLHDR_RESULT_ABBR, COLDESC_RESULT_ABBR, &ColStatusAbbrGet, &ColStatusSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Binary"), "DirView|ColumnHeader", COLHDR_BINARY, COLDESC_BINARY, &ColBinGet, &ColBinSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lattr"), nullptr, COLHDR_LATTRIBUTES, COLDESC_LATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].flags), -1, true, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Rattr"), nullptr, COLHDR_RATTRIBUTES, COLDESC_RATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].flags), -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Lencoding"), nullptr, COLHDR_LENCODING, COLDESC_LENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0]), -1, true, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Rencoding"), nullptr, COLHDR_RENCODING, COLDESC_RENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1]), -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Snsdiffs"), nullptr, COLHDR_NSDIFFS, COLDESC_NSDIFFS, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, nsdiffs), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Snidiffs"), nullptr, COLHDR_NIDIFFS, COLDESC_NIDIFFS, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, nidiffs), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Leoltype"), nullptr, COLHDR_LEOL_TYPE, COLDESC_LEOL_TYPE, &ColEOLTypeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Reoltype"), nullptr, COLHDR_REOL_TYPE, COLDESC_REOL_TYPE, &ColEOLTypeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Unpacker"), nullptr, COLHDR_UNPACKER, COLDESC_UNPACKER, &ColPluginPipelineGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Prediffer"), nullptr, COLHDR_PREDIFFER, COLDESC_PREDIFFER, &ColPluginPipelineGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 0 },
#ifdef SHOW_DIFFITEM_DEBUG_INFO
	{ _T("diffcode"), nullptr, COLHDR_DEBUG_DIFFCODE, COLDESC_DEBUG_DIFFCODE, &ColDebugDiffCodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("customFlags"), nullptr, COLHDR_DEBUG_CUSTOMFLAGS, COLDESC_DEBUG_CUSTOMFLAGS, &ColDebugCustomFlagsGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("this"), nullptr, COLHDR_DEBUG_THIS, COLDESC_DEBUG_THIS, &ColDebugNodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("parent"), nullptr, COLHDR_DEBUG_PARENT, COLDESC_DEBUG_PARENT, &ColDebugNodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("children"), nullptr, COLHDR_DEBUG_CHILDREN, COLDESC_DEBUG_CHILDREN, &ColDebugNodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 2 },
	{ _T("Flink"), nullptr, COLHDR_DEBUG_FLINK, COLDESC_DEBUG_FLINK, &ColDebugNodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 3 },
	{ _T("Blink"), nullptr, COLHDR_DEBUG_BLINK, COLDESC_DEBUG_BLINK, &ColDebugNodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 4 },
#endif // SHOW_DIFFITEM_DEBUG_INFO
};
static DirColInfo f_cols3[] =
{
	{ _T("Name"), nullptr, COLHDR_FILENAME, COLDESC_FILENAME, &ColFileNameGet<String>, &ColFileNameSort, 0, 0, true, DirColInfo::ALIGN_LEFT },
	{ _T("Path"), "DirView|ColumnHeader", COLHDR_DIR, COLDESC_DIR, &ColPathGet, &ColPathSort, 0, 1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Status"), nullptr, COLHDR_RESULT, COLDESC_RESULT, &ColStatusGet, &ColStatusSort, 0, 2, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lmtime"), nullptr, COLHDR_LTIMEM, COLDESC_LTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].mtime), 3, false, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Mmtime"), nullptr, COLHDR_MTIMEM, COLDESC_MTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].mtime), 4, false, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Rmtime"), nullptr, COLHDR_RTIMEM, COLDESC_RTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2].mtime), 5, false, DirColInfo::ALIGN_LEFT, 2 },
	{ _T("Lctime"), nullptr, COLHDR_LTIMEC, COLDESC_LTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].ctime), -1, false, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Mctime"), nullptr, COLHDR_MTIMEC, COLDESC_MTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].ctime), -1, false, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Rctime"), nullptr, COLHDR_RTIMEC, COLDESC_RTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2].ctime), -1, false, DirColInfo::ALIGN_LEFT, 2 },
	{ _T("Ext"), nullptr, COLHDR_EXTENSION, COLDESC_EXTENSION, &ColExtGet, &ColExtSort, 0, 6, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lsize"), nullptr, COLHDR_LSIZE, COLDESC_LSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].size), -1, false, DirColInfo::ALIGN_RIGHT, 0 },
	{ _T("Msize"), nullptr, COLHDR_MSIZE, COLDESC_MSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].size), -1, false, DirColInfo::ALIGN_RIGHT, 1 },
	{ _T("Rsize"), nullptr, COLHDR_RSIZE, COLDESC_RSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2].size), -1, false, DirColInfo::ALIGN_RIGHT, 2 },
	{ _T("LsizeShort"), nullptr, COLHDR_LSIZE_SHORT, COLDESC_LSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].size), -1, false, DirColInfo::ALIGN_RIGHT, 0 },
	{ _T("MsizeShort"), nullptr, COLHDR_MSIZE_SHORT, COLDESC_MSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].size), -1, false, DirColInfo::ALIGN_RIGHT, 1 },
	{ _T("RsizeShort"), nullptr, COLHDR_RSIZE_SHORT, COLDESC_RSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2].size), -1, false, DirColInfo::ALIGN_RIGHT, 2 },
	{ _T("Newer"), nullptr, COLHDR_NEWER, COLDESC_NEWER, &ColNewerGet, &ColNewerSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lversion"), nullptr, COLHDR_LVERSION, COLDESC_LVERSION, &ColVersionGet, &ColVersionSort, 0, -1, true, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Mversion"), nullptr, COLHDR_MVERSION, COLDESC_MVERSION, &ColVersionGet, &ColVersionSort, 0, -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Rversion"), nullptr, COLHDR_RVERSION, COLDESC_RVERSION, &ColVersionGet, &ColVersionSort, 0, -1, true, DirColInfo::ALIGN_LEFT, 2 },
	{ _T("StatusAbbr"), nullptr, COLHDR_RESULT_ABBR, COLDESC_RESULT_ABBR, &ColStatusAbbrGet, &ColStatusSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Binary"), "DirView|ColumnHeader", COLHDR_BINARY, COLDESC_BINARY, &ColBinGet, &ColBinSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lattr"), nullptr, COLHDR_LATTRIBUTES, COLDESC_LATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].flags), -1, true, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Mattr"), nullptr, COLHDR_MATTRIBUTES, COLDESC_MATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].flags), -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Rattr"), nullptr, COLHDR_RATTRIBUTES, COLDESC_RATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2].flags), -1, true, DirColInfo::ALIGN_LEFT, 2 },
	{ _T("Lencoding"), nullptr, COLHDR_LENCODING, COLDESC_LENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0]), -1, true, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Mencoding"), nullptr, COLHDR_MENCODING, COLDESC_MENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1]), -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Rencoding"), nullptr, COLHDR_RENCODING, COLDESC_RENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2]), -1, true, DirColInfo::ALIGN_LEFT, 2 },
	{ _T("Snsdiffs"), nullptr, COLHDR_NSDIFFS, COLDESC_NSDIFFS, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, nsdiffs), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Snidiffs"), nullptr, COLHDR_NIDIFFS, COLDESC_NIDIFFS, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, nidiffs), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Leoltype"), nullptr, COLHDR_LEOL_TYPE, COLDESC_LEOL_TYPE, &ColEOLTypeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("Meoltype"), nullptr, COLHDR_MEOL_TYPE, COLDESC_MEOL_TYPE, &ColEOLTypeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Reoltype"), nullptr, COLHDR_REOL_TYPE, COLDESC_REOL_TYPE, &ColEOLTypeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 2 },
	{ _T("Unpacker"), nullptr, COLHDR_UNPACKER, COLDESC_UNPACKER, &ColPluginPipelineGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("Prediffer"), nullptr, COLHDR_PREDIFFER, COLDESC_PREDIFFER, &ColPluginPipelineGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 0 },
#ifdef SHOW_DIFFITEM_DEBUG_INFO
	{ _T("diffcode"), nullptr, COLHDR_DEBUG_DIFFCODE, COLDESC_DEBUG_DIFFCODE, &ColDebugDiffCodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("customFlags"), nullptr, COLHDR_DEBUG_CUSTOMFLAGS, COLDESC_DEBUG_CUSTOMFLAGS, &ColDebugCustomFlagsGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("this"), nullptr, COLHDR_DEBUG_THIS, COLDESC_DEBUG_THIS, &ColDebugNodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 0 },
	{ _T("parent"), nullptr, COLHDR_DEBUG_PARENT, COLDESC_DEBUG_PARENT, &ColDebugNodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 1 },
	{ _T("children"), nullptr, COLHDR_DEBUG_CHILDREN, COLDESC_DEBUG_CHILDREN, &ColDebugNodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 2 },
	{ _T("Flink"), nullptr, COLHDR_DEBUG_FLINK, COLDESC_DEBUG_FLINK, &ColDebugNodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 3 },
	{ _T("Blink"), nullptr, COLHDR_DEBUG_BLINK, COLDESC_DEBUG_BLINK, &ColDebugNodeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT, 4 },
#endif // SHOW_DIFFITEM_DEBUG_INFO
};

String DirColInfo::GetDisplayName() const
{
	if (idName)
		return tr(idNameContext, idName);
	PropertySystem ps({ regName + 1 });
	std::vector<String> names;
	ps.GetDisplayNames(names);
	String name = names[0];
	if (regName[0] != 'A')
	{
		name += _T(" (");
		name +=
			(regName[0] == 'L') ? _("Left") :
			(regName[0] == 'R') ? _("Right") :
			(regName[0] == 'M') ? _("Middle") :
			(regName[0] == 'D') ? _("Diff") :
			(regName[0] == 'l') ? _("Left Duplicate Count") :
			(regName[0] == 'r') ? _("Right Duplicate Count") :
			(regName[0] == 'm') ? _("Middle Duplicate Count") :
			(regName[0] == 'N') ? _("Move") : _("");
		name += ')';
	}
	return name;
}

String DirColInfo::GetDescription() const
{
	if (idDesc)
		return tr(idDesc);
	return GetDisplayName();
}

DirViewColItems::DirViewColItems(int nDirs, const std::vector<String>& additionalPropertyNames) :
	m_nDirs(nDirs), m_dispcols(-1)
{
	m_numcols = static_cast<int>(nDirs < 3 ? std::size(f_cols) : std::size(f_cols3));
	DirColInfo *pcol = nDirs < 3 ? f_cols : f_cols3;
	for (size_t i = 0; i < m_numcols; ++i)
		m_cols.push_back(pcol[i]);
	ResetColumnOrdering();
	PropertySystem ps(additionalPropertyNames);
	for (const auto& propertyName : ps.GetCanonicalNames())
		AddAdditionalPropertyName(propertyName);
}

void
DirViewColItems::AddAdditionalPropertyName(const String& propertyName)
{
	const int nfixedcols = static_cast<int>(m_nDirs < 3 ? std::size(f_cols) : std::size(f_cols3));
	int fixedColPhysicalIndexMax = 0;
	for (int i = 0; i < nfixedcols; ++i)
		fixedColPhysicalIndexMax = (std::max)(fixedColPhysicalIndexMax, m_cols[i].physicalIndex);
	int pane = 0;
	String cList = (m_nDirs < 3) ? String(_T("ADLR")) : String(_T("ADLMR"));
	if (propertyName.substr(0, 5) == _T("Hash."))
		cList += (m_nDirs < 3) ? String(_T("Nlr")) : String(_T("lmr"));
	for (auto c : cList)
	{
		m_cols.emplace_back(DirColInfo{});
		m_strpool.push_back(c + propertyName);
		auto& col = m_cols.back();
		col.regName = m_strpool.back().c_str();
		col.opt = static_cast<int>(m_additionalPropertyNames.size());
		if (c == 'A')
		{
			col.physicalIndex = fixedColPhysicalIndexMax + 1 + col.opt;
			col.offset = 0;
			col.getfnc = ColAllPropertyGet;
			col.sortfnc = ColAllPropertySort;
		}
		else if (c == 'D')
		{
			col.physicalIndex = -1;
			col.offset = 0;
			col.getfnc = ColPropertyDiffGet;
			col.sortfnc = ColPropertyDiffSort;
		}
		else if (c == 'N')
		{
			col.physicalIndex = -1;
			col.offset = 0;
			col.getfnc = ColPropertyMoveGet;
			col.sortfnc = ColPropertyMoveSort;
		}
		else
		{
			col.physicalIndex = -1;
			col.offset = FIELD_OFFSET(DIFFITEM, diffFileInfo[pane]);
			if (c == 'L' || c == 'M' || c == 'R')
			{
				col.getfnc = ColPropertyGet;
				col.sortfnc = ColPropertySort;
			}
			else
			{
				col.opt |= pane << 16;
				col.getfnc = ColPropertyDuplicateCountGet;
				col.sortfnc = ColPropertyDuplicateCountSort;
			}
			pane = (pane + 1) % m_nDirs;
		}
		m_invcolorder.push_back(-1);
		if (c == 'A')
			m_invcolorder[m_dispcols] = m_numcols;
		m_colorder.push_back(c == 'A' ? m_dispcols++ : -1);
		++m_numcols;
	}
	m_additionalPropertyNames.push_back(propertyName);
}

void
DirViewColItems::RemoveAdditionalPropertyName(const String& propertyName)
{
	const int nfixedcols = static_cast<int>(m_nDirs < 3 ? std::size(f_cols) : std::size(f_cols3));
	std::vector<int> deletedPhysicalIndexes;
	std::vector<int> deletedLogicalIndexes;
	int deletedOpt = -1;
	for (int i = static_cast<int>(m_cols.size()) - 1; i >= nfixedcols; --i)
	{
		if (m_cols[i].regName + 1 == propertyName)
		{
			deletedLogicalIndexes.push_back(static_cast<int>(i));
			if (m_colorder[i] >= 0)
			{
				deletedPhysicalIndexes.push_back(m_colorder[i]);
				--m_dispcols;
			}
			deletedOpt = m_cols[i].opt;
			m_cols.erase(m_cols.begin() + i);
			--m_numcols;
		}
	}
	for (int i = static_cast<int>(m_invcolorder.size()) - 1; i >= 0; --i)
	{
		auto it = std::find(deletedLogicalIndexes.begin(), deletedLogicalIndexes.end(), m_invcolorder[i]);
		if (it != deletedLogicalIndexes.end())
			m_invcolorder.erase(m_invcolorder.begin() + i);
		else
		{
			int logicalIndex = m_invcolorder[i];
			for (auto deletedLogicalIndex : deletedLogicalIndexes)
			{
				if (deletedLogicalIndex < logicalIndex)
					--m_invcolorder[i];
			}
		}
	}
	for (int i = static_cast<int>(m_colorder.size()) - 1; i >= 0; --i)
	{
		auto it = std::find(deletedLogicalIndexes.begin(), deletedLogicalIndexes.end(), i);
		if (it != deletedLogicalIndexes.end())
			m_colorder.erase(m_colorder.begin() + i);
		else
		{
			int physicalIndex = m_colorder[i];
			for (auto deletedPhysicalIndex : deletedPhysicalIndexes)
			{
				if (deletedPhysicalIndex < physicalIndex)
					--m_colorder[i];
			}
		}
	}
	for (int i = static_cast<int>(m_cols.size()) - 1; i >= nfixedcols; --i)
	{
		if (deletedOpt < m_cols[i].opt)
			--m_cols[i].opt;
	}
	for (size_t i = 0; i < m_additionalPropertyNames.size(); ++i)
	{
		if (m_additionalPropertyNames[i] == propertyName)
			m_additionalPropertyNames.erase(m_additionalPropertyNames.begin() + i);
	}
	m_colorder.resize(m_numcols);
	m_invcolorder.resize(m_numcols);
	if (m_dispcols <= 0)
		ResetColumnOrdering();
}

void
DirViewColItems::SetAdditionalPropertyNames(const std::vector<String>& additionalPropertyNames)
{
	for (int i = static_cast<int>(m_additionalPropertyNames.size()) - 1; i >= 0; i--)
	{
		auto it = std::find(additionalPropertyNames.begin(), additionalPropertyNames.end(), m_additionalPropertyNames[i]);
		if (it == additionalPropertyNames.end())
			RemoveAdditionalPropertyName(m_additionalPropertyNames[i]);
	}
	for (const auto& propertyName : additionalPropertyNames)
	{
		auto it = std::find(m_additionalPropertyNames.begin(), m_additionalPropertyNames.end(), propertyName);
		if (it == m_additionalPropertyNames.end())
			AddAdditionalPropertyName(propertyName);
	}
}

/**
 * @brief Registry base value name for saving/loading info for this column
 */
String
DirViewColItems::GetColRegValueNameBase(int col) const
{
	assert(col>=0 && col<m_numcols);
	return strutils::format(_T("WDirHdr_%s"), m_cols[col].regName);
}

/**
 * @brief Get default physical order for specified logical column
 */
int
DirViewColItems::GetColDefaultOrder(int col) const
{
	assert(col>=0 && col<m_numcols);
	return m_cols[col].physicalIndex;
}

/**
 * @brief Return the info about the specified physical column
 */
const DirColInfo *
DirViewColItems::GetDirColInfo(int col) const
{
	if (col < 0 || col >= m_numcols)
	{
		assert(false); // fix caller, should not ask for nonexistent columns
		return nullptr;
	}
	return &m_cols[col];
}

/**
 * @brief Check if specified physical column has specified resource id name
 */
bool
DirViewColItems::IsColById(int col, const char *idname) const
{
	if (col < 0 || col >= m_numcols)
	{
		assert(false); // fix caller, should not ask for nonexistent columns
		return false;
	}
	return m_cols[col].idName == idname;
}

/**
 * @brief Is specified physical column the name column?
 */
bool
DirViewColItems::IsColName(int col) const
{
	return IsColById(col, COLHDR_FILENAME);
}
/**
 * @brief Is specified physical column the left modification time column?
 */
bool
DirViewColItems::IsColLmTime(int col) const
{
	return IsColById(col, COLHDR_LTIMEM);
}
/**
 * @brief Is specified physical column the middle modification time column?
 */
bool
DirViewColItems::IsColMmTime(int col) const
{
	return IsColById(col, COLHDR_MTIMEM);
}
/**
 * @brief Is specified physical column the right modification time column?
 */
bool
DirViewColItems::IsColRmTime(int col) const
{
	return IsColById(col, COLHDR_RTIMEM);
}
/**
 * @brief Is specified physical column the full status (result) column?
 */
bool
DirViewColItems::IsColStatus(int col) const
{
	return IsColById(col, COLHDR_RESULT);
}
/**
 * @brief Is specified physical column the full status (result) column?
 */
bool
DirViewColItems::IsColStatusAbbr(int col) const
{
	return IsColById(col, COLHDR_RESULT_ABBR);
}

/**
 * @brief return whether column normally sorts ascending (dates do not)
 */
bool
DirViewColItems::IsDefaultSortAscending(int col) const
{
	const DirColInfo * pColInfo = GetDirColInfo(col);
	if (pColInfo == nullptr)
	{
		assert(false); // fix caller, should not ask for nonexistent columns
		return false;
	}
	return pColInfo->defSortUp;
}

/**
 * @brief Return display name of column
 */
String
DirViewColItems::GetColDisplayName(int col) const
{
	const DirColInfo * colinfo = GetDirColInfo(col);
	return colinfo ? colinfo->GetDisplayName() : _T("");
}

/**
 * @brief Return description of column
 */
String
DirViewColItems::GetColDescription(int col) const
{
	const DirColInfo * colinfo = GetDirColInfo(col);
	return colinfo ? colinfo->GetDescription() : _T("");
}

/**
 * @brief Get text for specified column.
 * This function retrieves the text for the specified colum. Text is
 * retrieved by using column-specific handler functions.
 * @param [in] pCtxt Compare context.
 * @param [in] col Column number.
 * @param [in] di Difference data.
 * @return Text for the specified column.
 */
String
DirViewColItems::ColGetTextToDisplay(const CDiffContext *pCtxt, int col,
		const DIFFITEM &di) const
{
	// Custom properties have custom get functions
	const DirColInfo * pColInfo = GetDirColInfo(col);
	if (pColInfo == nullptr)
	{
		assert(false); // fix caller, should not ask for nonexistent columns
		return _T("???");
	}
	ColGetFncPtrType fnc = pColInfo->getfnc;
	size_t offset = pColInfo->offset;
	String s = (*fnc)(pCtxt, reinterpret_cast<const char *>(&di) + offset, pColInfo->opt);

	// Add '*' to newer time field
	if (IsColLmTime(col) || IsColMmTime(col) || IsColRmTime(col))
	{
		if (m_nDirs < 3)
		{
			if (di.diffFileInfo[0].mtime != 0 || di.diffFileInfo[1].mtime != 0)
			{
				if
					(
						IsColLmTime(col) && di.diffFileInfo[0].mtime > di.diffFileInfo[1].mtime // Left modification time
						|| IsColRmTime(col) && di.diffFileInfo[0].mtime < di.diffFileInfo[1].mtime // Right modification time
						)
				{
					s.insert(0, _T("* "));
				}
				else
				{
					s.insert(0, _T("  "));  // Looks best with a fixed-font, but not too bad otherwise
				}
				// GreyMerlin (14 Nov 2009) - the flagging of Date needs to be done with
				//		something not involving extra characters.  Perhaps <red> for oldest, 
				//		<green> for newest.  Note (20 March 2017): the introduction of 3-Way
				//		Merge and the yellow difference highlighting adds to the design
				//		difficulty of any changes.  So maybe this "* "/"  " scheme is good enough.

			}
		}
		else
		{
			if (di.diffFileInfo[0].mtime != 0 || di.diffFileInfo[1].mtime != 0 || di.diffFileInfo[2].mtime != 0)
			{
				if
					(
						IsColLmTime(col) && di.diffFileInfo[0].mtime > di.diffFileInfo[1].mtime && di.diffFileInfo[0].mtime > di.diffFileInfo[2].mtime // Left modification time
						|| IsColMmTime(col) && di.diffFileInfo[1].mtime > di.diffFileInfo[0].mtime && di.diffFileInfo[1].mtime > di.diffFileInfo[2].mtime // Middle modification time
						|| IsColRmTime(col) && di.diffFileInfo[2].mtime > di.diffFileInfo[0].mtime && di.diffFileInfo[2].mtime > di.diffFileInfo[1].mtime // Right modification time
						)
				{
					s.insert(0, _T("* "));
				}
				else
				{
					s.insert(0, _T("  "));  // Looks best with a fixed-font, but not too bad otherwise
				}
				// GreyMerlin (14 Nov 2009) - See note above.

			}
		}
	}

	return s;
}


/**
 * @brief Sort two items on specified column.
 * This function determines order of two items in specified column. Order
 * is determined by column-specific functions.
 * @param [in] pCtxt Compare context.
 * @param [in] col Column number to sort.
 * @param [in] ldi Left difference item data.
 * @param [in] rdi Right difference item data.
 * @return Order of items.
 */
int
DirViewColItems::ColSort(const CDiffContext *pCtxt, int col, const DIFFITEM &ldi,
		const DIFFITEM &rdi, bool bTreeMode) const
{
	// Custom properties have custom sort functions
	const DirColInfo * pColInfo = GetDirColInfo(col);
	if (pColInfo == nullptr)
	{
		assert(false); // fix caller, should not ask for nonexistent columns
		return 0;
	}
	size_t offset = pColInfo->offset;
	int opt = pColInfo->opt;
	const void * arg1;
	const void * arg2;
	if (bTreeMode)
	{
		int lLevel = ldi.GetDepth();
		int rLevel = rdi.GetDepth();
		const DIFFITEM *lcur = &ldi, *rcur = &rdi;
		if (lLevel < rLevel)
		{
			for (; lLevel != rLevel; rLevel--)
				rcur = rcur->GetParentLink();
		}
		else if (rLevel < lLevel)
		{
			for (; lLevel != rLevel; lLevel--)
				lcur = lcur->GetParentLink();
		}
		while (lcur->GetParentLink() != rcur->GetParentLink())
		{
			lcur = lcur->GetParentLink();
			rcur = rcur->GetParentLink();
		}
		arg1 = reinterpret_cast<const char *>(lcur) + offset;
		arg2 = reinterpret_cast<const char *>(rcur) + offset;
	}
	else
	{
		arg1 = reinterpret_cast<const char *>(&ldi) + offset;
		arg2 = reinterpret_cast<const char *>(&rdi) + offset;
	}
	if (ColSortFncPtrType fnc = pColInfo->sortfnc)
	{
		return (*fnc)(pCtxt, arg1, arg2, opt);
	}
	if (ColGetFncPtrType fnc = pColInfo->getfnc)
	{
		String p = (*fnc)(pCtxt, arg1, opt);
		String q = (*fnc)(pCtxt, arg2, opt);
		return strutils::compare_logical(p, q);
	}
	return 0;
}

void DirViewColItems::SetColumnOrdering(const int colorder[])
{
	m_dispcols = 0;
	for (int i = 0; i < m_numcols; ++i)
	{
		m_colorder[i] = colorder[i];
		int phy = m_colorder[i];
		if (phy>=0)
		{
			++m_dispcols;
			m_invcolorder[phy] = i;
		}
	}
}

/**
 * @brief Set column ordering to default initial order
 */
void DirViewColItems::ResetColumnOrdering()
{
	ClearColumnOrders();
	m_dispcols = 0;
	for (int i=0; i<m_numcols; ++i)
	{
		int phy = GetColDefaultOrder(i);
		m_colorder[i] = phy;
		if (phy>=0)
		{
			m_invcolorder[phy] = i;
			++m_dispcols;
		}
	}
}

/**
 * @brief Reset all current column ordering information
 */
void DirViewColItems::ClearColumnOrders()
{
	m_colorder.resize(m_numcols);
	m_invcolorder.resize(m_numcols);
	for (int i=0; i<m_numcols; ++i)
	{
		m_colorder[i] = -1;
		m_invcolorder[i] = -1;
	}
}

/**
 * @brief Remove any windows reordering of columns (params are physical columns)
 */
void DirViewColItems::MoveColumn(int psrc, int pdest)
{
	// actually moved column
	m_colorder[m_invcolorder[psrc]] = pdest;
	// shift all other affected columns
	int dir = psrc > pdest ? +1 : -1;
	int i=0;
	for (i=pdest; i!=psrc; i += dir)
	{
		m_colorder[m_invcolorder[i]] = i+dir;
	}
	// fix inverse mapping
	for (i=0; i<m_numcols; ++i)
	{
		if (m_colorder[i] >= 0)
			m_invcolorder[m_colorder[i]] = i;
	}
}

/**
 * @brief Resets column widths to defaults.
 */
String DirViewColItems::ResetColumnWidths(int defcolwidth)
{
	String result;
	for (int i = 0; i < m_numcols; i++)
	{
		if (!result.empty()) result += ' ';
		result += strutils::to_str(defcolwidth);
	}
	return result;
}

/**
 * @brief Load column orders from registry
 */
void DirViewColItems::LoadColumnOrders(const String& colOrders)
{
	ClearColumnOrders();
	m_dispcols = 0;
	std::basic_istringstream<tchar_t> ss(colOrders);

	// Load column orders
	// Break out if one is missing
	// Break out & mark failure (m_dispcols == -1) if one is invalid
	int i=0;
	for (i=0; i<m_numcols; ++i)
	{
		int ord = -1;
		ss >> ord;
		if (ord<-1 || ord >= m_numcols)
			break;
		m_colorder[i] = ord;
		if (ord>=0)
		{
			++m_dispcols;
			if (m_invcolorder[ord] != -1)
			{
				m_dispcols = -1;
				break;
			}
			m_invcolorder[ord] = i;
		}
	}
	// Check that a contiguous range was set
	for (i=0; i<m_dispcols; ++i)
	{
		if (m_invcolorder[i] < 0)
		{
			m_dispcols = -1;
			break;
		}
	}
	// Must have at least one column
	if (m_dispcols<=1)
	{
		ResetColumnOrdering();
	}
}

/// store current column orders into registry
String DirViewColItems::SaveColumnOrders()
{
	assert(static_cast<int>(m_colorder.size()) == m_numcols);
	assert(static_cast<int>(m_invcolorder.size()) == m_numcols);
	return strutils::join<String (*)(int)>(m_colorder.begin(), m_colorder.end(), _T(" "), strutils::to_str);
}
