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
const char *COLHDR_NIDIFFS      = N_("Ignored Diff.");
const char *COLHDR_NSDIFFS      = N_("Differences");
const char *COLHDR_BINARY       = NC_("DirView|ColumnHeader", "Binary");

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
const char *COLDESC_LVERSION    = N_("Left side file version, only for some filetypes.");
const char *COLDESC_RVERSION    = N_("Right side file version, only for some filetypes.");
const char *COLDESC_MVERSION    = N_("Middle side file version, only for some filetypes.");
const char *COLDESC_RESULT_ABBR = N_("Short comparison result.");
const char *COLDESC_LATTRIBUTES = N_("Left side attributes.");
const char *COLDESC_RATTRIBUTES = N_("Right side attributes.");
const char *COLDESC_MATTRIBUTES = N_("Middle side attributes.");
const char *COLDESC_LEOL_TYPE   = N_("Left side file EOL type");
const char *COLDESC_REOL_TYPE   = N_("Right side file EOL type");
const char *COLDESC_MEOL_TYPE   = N_("Middle side file EOL type");
const char *COLDESC_LENCODING   = N_("Left side encoding.");
const char *COLDESC_RENCODING   = N_("Right side encoding.");
const char *COLDESC_MENCODING   = N_("Middle side encoding.");
const char *COLDESC_NIDIFFS     = N_("Number of ignored differences in file. These differences are ignored by WinMerge and cannot be merged.");
const char *COLDESC_NSDIFFS     = N_("Number of differences in file. This number does not include ignored differences.");
const char *COLDESC_BINARY      = N_("Shows an asterisk (*) if the file is binary.");
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
 * @brief Convert int64_t to int sign
 */
static int sign64(int64_t val)
{
  if (val>0) return 1;
  if (val<0) return -1;
  return 0;
}
/**
 * @brief Function to compare two diffcodes for a sort
 * @todo How shall we order diff statuses?
 */
static int cmpdiffcode(unsigned diffcode1, unsigned diffcode2)
{
	// Lower priority of the same items (FIXME:)
	if (((diffcode1 & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SAME) && ((diffcode2 & DIFFCODE::COMPAREFLAGS) != DIFFCODE::SAME))
		return -1;
	if (((diffcode1 & DIFFCODE::COMPAREFLAGS) != DIFFCODE::SAME) && ((diffcode2 & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SAME))
		return 1;
	if ((diffcode1 & DIFFCODE::DIR) && !(diffcode2 & DIFFCODE::DIR))
		return 1;
	if (!(diffcode1 & DIFFCODE::DIR) && (diffcode2 & DIFFCODE::DIR))
		return -1;
	return diffcode1-diffcode2;	
}
/**
 * @brief Function to compare two doubles for a sort
 */
static int cmpfloat(double v1, double v2)
{
	if (v1>v2)
		return 1;
	if (v1<v2)
		return -1;
	return 0;
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
	TCHAR buffer[48];
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
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
template<class Type>
static Type ColFileNameGet(const CDiffContext *, const void *p) //sfilename
{
	const boost::flyweight<String> &lfilename = static_cast<const DIFFITEM*>(p)->diffFileInfo[0].filename;
	const boost::flyweight<String> &rfilename = static_cast<const DIFFITEM*>(p)->diffFileInfo[1].filename;
	if (lfilename.get().empty())
		return rfilename;
	else if (rfilename.get().empty() || lfilename == rfilename)
		return lfilename;
	else
		return static_cast<Type>(lfilename.get() + _T("|") + rfilename.get());
}

/**
 * @brief Format Extension column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColExtGet(const CDiffContext *, const void *p) //sfilename
{
	const DIFFITEM &di = *static_cast<const DIFFITEM*>(p);
	// We don't show extension for folder names
	if (di.diffcode.isDirectory())
		return _T("");
	const String &r = di.diffFileInfo[0].filename;
	String s = paths::FindExtension(r);
	return s.c_str() + _tcsspn(s.c_str(), _T("."));
}

/**
 * @brief Format Folder column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColPathGet(const CDiffContext *, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM*>(p);
	String s = di.diffFileInfo[1].path;
	const String &t = di.diffFileInfo[0].path;

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
		const TCHAR *pi = _tcschr(s.c_str() + i, '\\');
		const TCHAR *pj = _tcschr(t.c_str() + j, '\\');
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

/**
 * @brief Format Result column data.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColStatusGet(const CDiffContext *pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM*>(p);
	int nDirs = pCtxt->GetCompareDirs();
	// Note that order of items does matter. We must check for
	// skipped items before unique items, for example, so that
	// skipped unique items are labeled as skipped, not unique.
	String s;
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
	}
	else if (nDirs > 2 && !di.diffcode.existsSecond())
	{
		s = strutils::format_string1(_("Does not exist in %1"),
				pCtxt->GetNormalizedMiddle());
	}
	else if (nDirs > 2 && !di.diffcode.existsThird())
	{
		s = strutils::format_string1(_("Does not exist in %1"),
				pCtxt->GetNormalizedRight());
	}
	else if (di.diffcode.isResultSame())
	{
		if (di.diffcode.isText())
			s = _("Text files are identical");
		else if (di.diffcode.isBin())
			s = _("Binary files are identical");
		else
			s = _("Identical");
	}
	else if (di.diffcode.isResultDiff()) // diff
	{
		if (di.diffcode.isText())
			s = _("Text files are different");
		else if (di.diffcode.isBin())
			s = _("Binary files are different");
		else if (di.diffcode.isDirectory())
			s = _("Folders are different");
		else
			s = _("Files are different");
		if (nDirs > 2)
		{
			switch (di.diffcode.diffcode & DIFFCODE::COMPAREFLAGS3WAY)
			{
			case DIFFCODE::DIFF1STONLY: s += _("(Middle and right are identical)"); break;
			case DIFFCODE::DIFF2NDONLY: s += _("(Left and right are identical)"); break;
			case DIFFCODE::DIFF3RDONLY: s += _("(Left and middle are identical)"); break;
			}
		}
	}
	return s;
}

/**
 * @brief Format Date column data.
 * @param [in] p Pointer to integer (seconds since 1.1.1970).
 * @return String to show in the column.
 */
static String ColTimeGet(const CDiffContext *, const void *p)
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
static String ColSizeGet(const CDiffContext *, const void *p)
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
static String ColSizeShortGet(const CDiffContext *, const void *p)
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
static String ColDiffsGet(const CDiffContext *, const void *p)
{
	const int &r = *static_cast<const int*>(p);
	String s;
	if (r == CDiffContext::DIFFS_UNKNOWN_QUICKCOMPARE)
	{ // QuickCompare, unknown
		s = _T("*");
	}
	else if (r == CDiffContext::DIFFS_UNKNOWN)
	{ // Unique item
		s = _T("");
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
static String ColNewerGet(const CDiffContext *pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	if (pCtxt->GetCompareDirs() < 3)
	{
		if (di.diffcode.isSideFirstOnly())
		{
			return _T("<*<");
		}
		if (di.diffcode.isSideSecondOnly())
		{
			return _T(">*>");
		}
		if (di.diffFileInfo[0].mtime != 0 && di.diffFileInfo[1].mtime != 0)
		{
			if (di.diffFileInfo[0].mtime > di.diffFileInfo[1].mtime)
			{
				return _T("<<");
			}
			if (di.diffFileInfo[0].mtime < di.diffFileInfo[1].mtime)
			{
				return _T(">>");
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
		res += sorttime[0] == sorttime[1] ? _T("==") : _T("<<");
		res += _T("LMR")[sortno[1]];
		res += sorttime[1] == sorttime[2] ? _T("==") : _T("<<");
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
 * @brief Format Version column data (for left-side).
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColLversionGet(const CDiffContext * pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetVersion(pCtxt, &di, 0);
}

/**
 * @brief Format Version column data (for middle-side).
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColMversionGet(const CDiffContext * pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetVersion(pCtxt, &di, 1);
}

/**
 * @brief Format Version column data (for right-side).
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColRversionGet(const CDiffContext * pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetVersion(pCtxt, &di, pCtxt->GetCompareDirs() < 3 ? 1 : 2);
}

/**
 * @brief Format Short Result column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColStatusAbbrGet(const CDiffContext *pCtxt, const void *p)
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
static String ColBinGet(const CDiffContext *, const void *p)
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
static String ColAttrGet(const CDiffContext *, const void *p)
{
	const FileFlags &r = *static_cast<const FileFlags *>(p);
	return r.ToString();
}

/**
 * @brief Format File Encoding column data.
 * @param [in] p Pointer to file information.
 * @return String to show in the column.
 */
static String ColEncodingGet(const CDiffContext *, const void *p)
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
 * @brief Format EOL type column data (for left-side file).
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColLEOLTypeGet(const CDiffContext * pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetEOLType(pCtxt, &di, 0);
}

/**
 * @brief Format EOL type column data (for right-side file).
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColMEOLTypeGet(const CDiffContext * pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetEOLType(pCtxt, &di, 1);
}

static String ColREOLTypeGet(const CDiffContext * pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetEOLType(pCtxt, &di, pCtxt->GetCompareDirs() < 3 ? 1 : 2);
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
static int ColFileNameSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	const DIFFITEM &ldi = *static_cast<const DIFFITEM *>(p);
	const DIFFITEM &rdi = *static_cast<const DIFFITEM *>(q);
	if (ldi.diffcode.isDirectory() && !rdi.diffcode.isDirectory())
		return -1;
	if (!ldi.diffcode.isDirectory() && rdi.diffcode.isDirectory())
		return 1;
	return strutils::compare_nocase(ColFileNameGet<boost::flyweight<String> >(pCtxt, p), ColFileNameGet<boost::flyweight<String> >(pCtxt, q));
}

/**
 * @brief Compare file name extensions.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p First file name extension to compare.
 * @param [in] q Second file name extension to compare.
 * @return Compare result.
 */
static int ColExtSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	const DIFFITEM &ldi = *static_cast<const DIFFITEM *>(p);
	const DIFFITEM &rdi = *static_cast<const DIFFITEM *>(q);
	if (ldi.diffcode.isDirectory() && !rdi.diffcode.isDirectory())
		return -1;
	if (!ldi.diffcode.isDirectory() && rdi.diffcode.isDirectory())
		return 1;
	return strutils::compare_nocase(ColExtGet(pCtxt, p), ColExtGet(pCtxt, q));
}

/**
 * @brief Compare folder names.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM having first folder name to compare.
 * @param [in] q Pointer to DIFFITEM having second folder name to compare.
 * @return Compare result.
 */
static int ColPathSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	return strutils::compare_nocase(ColPathGet(pCtxt, p), ColPathGet(pCtxt, q));
}

/**
 * @brief Compare compare results.
 * @param [in] p Pointer to DIFFITEM having first result to compare.
 * @param [in] q Pointer to DIFFITEM having second result to compare.
 * @return Compare result.
 */
static int ColStatusSort(const CDiffContext *, const void *p, const void *q)
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
static int ColTimeSort(const CDiffContext *, const void *p, const void *q)
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
static int ColSizeSort(const CDiffContext *, const void *p, const void *q)
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
static int ColDiffsSort(const CDiffContext *, const void *p, const void *q)
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
static int ColNewerSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	return ColNewerGet(pCtxt, p).compare(ColNewerGet(pCtxt, q));
}

/**
 * @brief Compare left-side file versions.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM having first version to compare.
 * @param [in] q Pointer to DIFFITEM having second version to compare.
 * @return Compare result.
 */
static int ColLversionSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	return cmpu64(GetVersionQWORD(pCtxt, reinterpret_cast<const DIFFITEM *>(p), 0), GetVersionQWORD(pCtxt, reinterpret_cast<const DIFFITEM *>(q), 0));
}

/**
 * @brief Compare middle-side file versions.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM having first version to compare.
 * @param [in] q Pointer to DIFFITEM having second version to compare.
 * @return Compare result.
 */
static int ColMversionSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	return cmpu64(GetVersionQWORD(pCtxt, reinterpret_cast<const DIFFITEM *>(p), 1), GetVersionQWORD(pCtxt, reinterpret_cast<const DIFFITEM *>(q), 1));
}

/**
 * @brief Compare right-side file versions.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM having first version to compare.
 * @param [in] q Pointer to DIFFITEM having second version to compare.
 * @return Compare result.
 */
static int ColRversionSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	const int i = pCtxt->GetCompareDirs() < 3 ? 1 : 2;
	return cmpu64(GetVersionQWORD(pCtxt, reinterpret_cast<const DIFFITEM *>(p), i), GetVersionQWORD(pCtxt, reinterpret_cast<const DIFFITEM *>(q), i));
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
static int ColBinSort(const CDiffContext *, const void *p, const void *q)
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
static int ColAttrSort(const CDiffContext *, const void *p, const void *q)
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
static int ColEncodingSort(const CDiffContext *, const void *p, const void *q)
{
	const DiffFileInfo &r = *static_cast<const DiffFileInfo *>(p);
	const DiffFileInfo &s = *static_cast<const DiffFileInfo *>(q);
	return FileTextEncoding::Collate(r.encoding, s.encoding);
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
	{ _T("Lmtime"), nullptr, COLHDR_LTIMEM, COLDESC_LTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].mtime), 3, false, DirColInfo::ALIGN_LEFT },
	{ _T("Rmtime"), nullptr, COLHDR_RTIMEM, COLDESC_RTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].mtime), 4, false, DirColInfo::ALIGN_LEFT },
	{ _T("Lctime"), nullptr, COLHDR_LTIMEC, COLDESC_LTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].ctime), -1, false, DirColInfo::ALIGN_LEFT },
	{ _T("Rctime"), nullptr, COLHDR_RTIMEC, COLDESC_RTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].ctime), -1, false, DirColInfo::ALIGN_LEFT },
	{ _T("Ext"), nullptr, COLHDR_EXTENSION, COLDESC_EXTENSION, &ColExtGet, &ColExtSort, 0, 5, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lsize"), nullptr, COLHDR_LSIZE, COLDESC_LSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].size), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Rsize"), nullptr, COLHDR_RSIZE, COLDESC_RSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].size), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("LsizeShort"), nullptr, COLHDR_LSIZE_SHORT, COLDESC_LSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].size), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("RsizeShort"), nullptr, COLHDR_RSIZE_SHORT, COLDESC_RSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].size), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Newer"), nullptr, COLHDR_NEWER, COLDESC_NEWER, &ColNewerGet, &ColNewerSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lversion"), nullptr, COLHDR_LVERSION, COLDESC_LVERSION, &ColLversionGet, &ColLversionSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Rversion"), nullptr, COLHDR_RVERSION, COLDESC_RVERSION, &ColRversionGet, &ColRversionSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("StatusAbbr"), nullptr, COLHDR_RESULT_ABBR, COLDESC_RESULT_ABBR, &ColStatusAbbrGet, &ColStatusSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Binary"), "DirView|ColumnHeader", COLHDR_BINARY, COLDESC_BINARY, &ColBinGet, &ColBinSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lattr"), nullptr, COLHDR_LATTRIBUTES, COLDESC_LATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].flags), -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Rattr"), nullptr, COLHDR_RATTRIBUTES, COLDESC_RATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].flags), -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lencoding"), nullptr, COLHDR_LENCODING, COLDESC_LENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0]), -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Rencoding"), nullptr, COLHDR_RENCODING, COLDESC_RENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1]), -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Snsdiffs"), nullptr, COLHDR_NSDIFFS, COLDESC_NSDIFFS, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, nsdiffs), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Snidiffs"), nullptr, COLHDR_NIDIFFS, COLDESC_NIDIFFS, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, nidiffs), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Leoltype"), nullptr, COLHDR_LEOL_TYPE, COLDESC_LEOL_TYPE, &ColLEOLTypeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Reoltype"), nullptr, COLHDR_REOL_TYPE, COLDESC_REOL_TYPE, &ColREOLTypeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT },
};
static DirColInfo f_cols3[] =
{
	{ _T("Name"), nullptr, COLHDR_FILENAME, COLDESC_FILENAME, &ColFileNameGet<String>, &ColFileNameSort, 0, 0, true, DirColInfo::ALIGN_LEFT },
	{ _T("Path"), "DirView|ColumnHeader", COLHDR_DIR, COLDESC_DIR, &ColPathGet, &ColPathSort, 0, 1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Status"), nullptr, COLHDR_RESULT, COLDESC_RESULT, &ColStatusGet, &ColStatusSort, 0, 2, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lmtime"), nullptr, COLHDR_LTIMEM, COLDESC_LTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].mtime), 3, false, DirColInfo::ALIGN_LEFT },
	{ _T("Mmtime"), nullptr, COLHDR_MTIMEM, COLDESC_MTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].mtime), 4, false, DirColInfo::ALIGN_LEFT },
	{ _T("Rmtime"), nullptr, COLHDR_RTIMEM, COLDESC_RTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2].mtime), 5, false, DirColInfo::ALIGN_LEFT },
	{ _T("Lctime"), nullptr, COLHDR_LTIMEC, COLDESC_LTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].ctime), -1, false, DirColInfo::ALIGN_LEFT },
	{ _T("Mctime"), nullptr, COLHDR_MTIMEC, COLDESC_MTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].ctime), -1, false, DirColInfo::ALIGN_LEFT },
	{ _T("Rctime"), nullptr, COLHDR_RTIMEC, COLDESC_RTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2].ctime), -1, false, DirColInfo::ALIGN_LEFT },
	{ _T("Ext"), nullptr, COLHDR_EXTENSION, COLDESC_EXTENSION, &ColExtGet, &ColExtSort, 0, 6, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lsize"), nullptr, COLHDR_LSIZE, COLDESC_LSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].size), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Msize"), nullptr, COLHDR_MSIZE, COLDESC_MSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].size), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Rsize"), nullptr, COLHDR_RSIZE, COLDESC_RSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2].size), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("LsizeShort"), nullptr, COLHDR_LSIZE_SHORT, COLDESC_LSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].size), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("MsizeShort"), nullptr, COLHDR_MSIZE_SHORT, COLDESC_MSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].size), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("RsizeShort"), nullptr, COLHDR_RSIZE_SHORT, COLDESC_RSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2].size), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Newer"), nullptr, COLHDR_NEWER, COLDESC_NEWER, &ColNewerGet, &ColNewerSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lversion"), nullptr, COLHDR_LVERSION, COLDESC_LVERSION, &ColLversionGet, &ColLversionSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Mversion"), nullptr, COLHDR_MVERSION, COLDESC_MVERSION, &ColMversionGet, &ColMversionSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Rversion"), nullptr, COLHDR_RVERSION, COLDESC_RVERSION, &ColRversionGet, &ColRversionSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("StatusAbbr"), nullptr, COLHDR_RESULT_ABBR, COLDESC_RESULT_ABBR, &ColStatusAbbrGet, &ColStatusSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Binary"), "DirView|ColumnHeader", COLHDR_BINARY, COLDESC_BINARY, &ColBinGet, &ColBinSort, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lattr"), nullptr, COLHDR_LATTRIBUTES, COLDESC_LATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0].flags), -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Mattr"), nullptr, COLHDR_MATTRIBUTES, COLDESC_MATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1].flags), -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Rattr"), nullptr, COLHDR_RATTRIBUTES, COLDESC_RATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2].flags), -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Lencoding"), nullptr, COLHDR_LENCODING, COLDESC_LENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[0]), -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Mencoding"), nullptr, COLHDR_MENCODING, COLDESC_MENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[1]), -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Rencoding"), nullptr, COLHDR_RENCODING, COLDESC_RENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, diffFileInfo[2]), -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Snsdiffs"), nullptr, COLHDR_NSDIFFS, COLDESC_NSDIFFS, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, nsdiffs), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Snidiffs"), nullptr, COLHDR_NIDIFFS, COLDESC_NIDIFFS, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, nidiffs), -1, false, DirColInfo::ALIGN_RIGHT },
	{ _T("Leoltype"), nullptr, COLHDR_LEOL_TYPE, COLDESC_LEOL_TYPE, &ColLEOLTypeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Meoltype"), nullptr, COLHDR_MEOL_TYPE, COLDESC_MEOL_TYPE, &ColMEOLTypeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT },
	{ _T("Reoltype"), nullptr, COLHDR_REOL_TYPE, COLDESC_REOL_TYPE, &ColREOLTypeGet, 0, 0, -1, true, DirColInfo::ALIGN_LEFT },
};

/**
 * @brief Count of all known columns
 */
const int g_ncols = static_cast<int>(std::size(f_cols));
const int g_ncols3 = static_cast<int>(std::size(f_cols3));

/**
 * @brief Registry base value name for saving/loading info for this column
 */
String
DirViewColItems::GetColRegValueNameBase(int col) const
{
	if (m_nDirs < 3)
	{
		assert(col>=0 && col<static_cast<int>(std::size(f_cols)));
		return strutils::format(_T("WDirHdr_%s"), f_cols[col].regName);
	}
	else
	{
		assert(col>=0 && col<static_cast<int>(std::size(f_cols3)));
		return strutils::format(_T("WDirHdr_%s"), f_cols3[col].regName);
	}
}

/**
 * @brief Get default physical order for specified logical column
 */
int
DirViewColItems::GetColDefaultOrder(int col) const
{
	if (m_nDirs < 3)
	{
		assert(col>=0 && col<static_cast<int>(std::size(f_cols)));
		return f_cols[col].physicalIndex;
	}
	else
	{
		assert(col>=0 && col<static_cast<int>(std::size(f_cols3)));
		return f_cols3[col].physicalIndex;
	}
}

/**
 * @brief Return the info about the specified physical column
 */
const DirColInfo *
DirViewColItems::GetDirColInfo(int col) const
{
	if (m_nDirs < 3)
	{
		if (col < 0 || col >= static_cast<int>(std::size(f_cols)))
		{
			assert(false); // fix caller, should not ask for nonexistent columns
			return nullptr;
		}
		return &f_cols[col];
	}
	else
	{
		if (col < 0 || col >= static_cast<int>(std::size(f_cols3)))
		{
			assert(false); // fix caller, should not ask for nonexistent columns
			return nullptr;
		}
		return &f_cols3[col];
	}
}

/**
 * @brief Check if specified physical column has specified resource id name
 */
bool
DirViewColItems::IsColById(int col, const char *idname) const
{
	int nDirs = m_nDirs;
	if (nDirs < 3)
	{
		if (col < 0 || col >= static_cast<int>(std::size(f_cols)))
		{
			assert(false); // fix caller, should not ask for nonexistent columns
			return false;
		}
		return f_cols[col].idName == idname;
	}
	else
	{
		if (col < 0 || col >= sizeof(f_cols3)/sizeof(f_cols3[0]))
		{
			assert(false); // fix caller, should not ask for nonexistent columns
			return false;
		}
		return f_cols3[col].idName == idname;
	}
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
	return tr(colinfo->idNameContext, colinfo->idName);
}

/**
 * @brief Return description of column
 */
String
DirViewColItems::GetColDescription(int col) const
{
	const DirColInfo * colinfo = GetDirColInfo(col);
	return tr(colinfo->idDesc);
}

/**
 * @brief Return total number of known columns
 */
int
DirViewColItems::GetColCount() const
{
	if (m_nDirs < 3)
		return g_ncols;
	else
		return g_ncols3;
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
	String s = (*fnc)(pCtxt, reinterpret_cast<const char *>(&di) + offset);

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
		return (*fnc)(pCtxt, arg1, arg2);
	}
	if (ColGetFncPtrType fnc = pColInfo->getfnc)
	{
		String p = (*fnc)(pCtxt, arg1);
		String q = (*fnc)(pCtxt, arg2);
		return strutils::compare_nocase(p, q);
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
void DirViewColItems::LoadColumnOrders(String colorders)
{
	assert(m_numcols == -1);
	m_numcols = GetColCount();
	ClearColumnOrders();
	m_dispcols = 0;
	std::basic_istringstream<TCHAR> ss(colorders);

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
