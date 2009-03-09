/** 
 * @file  DirViewColItems.cpp
 *
 * @brief Code for individual columns in the DirView
 *
 * @date  Created: 2003-08-19
 */
// ID line follows -- this is updated by SVN
// $Id$


#include "stdafx.h"
#include <shlwapi.h>
#include "UnicodeString.h"
#include "Merge.h"
#include "DirView.h"
#include "DirDoc.h"
#include "MainFrm.h"
#include "resource.h"
#include "DirViewColItems.h"
#include "locality.h"
#include "unicoder.h"
#include "coretools.h"

// shlwapi.h prior to VC6SP6 might lack definition of StrIsIntlEqual
#ifdef UNICODE
#define StrIsIntlEqual StrIsIntlEqualW
#else
#define StrIsIntlEqual StrIsIntlEqualA
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @name Constants for short sizes.
 */
/* @{ */
static const UINT KILO = 1024;
static const UINT MEGA = 1024 * KILO;
static const UINT GIGA = 1024 * MEGA;
static const __int64 TERA = 1024 * (__int64) GIGA;
/**
 * @}
 */

/**
 * @brief Function to compare two __int64s for a sort
 */
static int cmp64(__int64 i1, __int64 i2)
{
	if (i1==i2) return 0;
	return i1>i2 ? 1 : -1;
}
/**
 * @brief Convert __int64 to int sign
 */
static int sign64(__int64 val)
{
  if (val>0) return 1;
  if (val<0) return -1;
  return 0;
}
/**
 * @brief Function to compare two diffcodes for a sort
 * @todo How shall we order diff statuses?
 */
static UINT cmpdiffcode(UINT diffcode1, UINT diffcode2)
{
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
static String MakeShortSize(__int64 size)
{
#pragma warning(disable:4244) // warning C4244: '=' : conversion from '__int64' to 'double', possible loss of data
	double fsize = size;
#pragma warning(default:4244) // warning C4244: '=' : conversion from '__int64' to 'double', possible loss of data
	double number = 0;
	int ndigits = 0;
	String suffix;

	if (size < KILO)
	{
		number = fsize;
		suffix = theApp.LoadString(IDS_SUFFIX_BYTE);
	}
	else if (size < MEGA)
	{
		number = fsize / KILO;
		suffix = theApp.LoadString(IDS_SUFFIX_KILO);
		if (size < KILO * 10)
		{
			ndigits = 2;
		}
		else if (size < KILO * 100)
		{
			ndigits = 1;
		}
	}
	else if (size < GIGA)
	{
		number = fsize / (MEGA);
		suffix = theApp.LoadString(IDS_SUFFIX_MEGA);
		if (size < MEGA * 10)
		{
			ndigits = 2;
		}
		else if (size < MEGA * 100)
		{
			ndigits = 1;
		}
	}
	else if (size < (__int64)TERA)
	{
		number = fsize / ((__int64)GIGA);
		suffix = theApp.LoadString(IDS_SUFFIX_GIGA);
		if (size < (__int64)GIGA * 10)
		{
			ndigits = 2;
		}
		else if (size < (__int64)GIGA * 100)
		{
			ndigits = 1;
		}
	}
	else
	{
		// overflow (?) -- show ">TB"
		String s(_T(">"));
		suffix = theApp.LoadString(IDS_SUFFIX_TERA);
		s += suffix;
		return s;
	}

	TCHAR buffer[48];
	_sntprintf(buffer, countof(buffer), _T("%lf"), number);
	return locality::GetLocaleStr(buffer, ndigits) + suffix;
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
static String ColFileNameGet(const CDiffContext *, const void *p) //sfilename
{
	const DIFFITEM &di = *static_cast<const DIFFITEM*>(p);
	return
	(
		di.left.filename.empty() ? di.right.filename :
		di.right.filename.empty() ? di.left.filename :
		di.left.filename == di.right.filename ? di.left.filename :
		di.left.filename + _T("|") + di.right.filename
	);
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
	const String &r = di.left.filename;
	LPCTSTR s = PathFindExtension(r.c_str());
	return s + _tcsspn(s, _T("."));
}

/**
 * @brief Format Folder column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColPathGet(const CDiffContext *, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM*>(p);
	String s = di.right.path;
	const String &t = di.left.path;

	// If we have unique path, just print the existing path name
	if (s.length() == 0 || t.length() == 0)
	{
		if (s.length() == 0)
			return t;
		else
			return s;
	}

	int i = 0, j = 0;
	do
	{
		int i_ahead = s.find('\\', i);
		int j_ahead = t.find('\\', j);
		int length_s = (i_ahead != std::string.npos ? i_ahead : s.length()) - i;
		int length_t = (j_ahead != std::string.npos ? j_ahead : t.length()) - j;
		if (length_s != length_t ||
			!StrIsIntlEqual(FALSE, s.c_str() + i, t.c_str() + j, length_s))
		{
			String u(t.c_str() + j, length_t + 1);
			u[length_t] = '|';
			s.insert(i, u.c_str());
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
	// Note that order of items does matter. We must check for
	// skipped items before unique items, for example, so that
	// skipped unique items are labeled as skipped, not unique.
	String s;
	if (di.diffcode.isResultError())
	{
		s = theApp.LoadString(IDS_CANT_COMPARE_FILES);
	}
	else if (di.diffcode.isResultAbort())
	{
		s = theApp.LoadString(IDS_ABORTED_ITEM);
	}
	else if (di.diffcode.isResultFiltered())
	{
		if (di.diffcode.isDirectory())
			s = theApp.LoadString(IDS_DIR_SKIPPED);
		else
			s = theApp.LoadString(IDS_FILE_SKIPPED);
	}
	else if (di.diffcode.isSideLeftOnly())
	{
		s = theApp.LoadString(IDS_LEFT_ONLY_IN_FMT);
		string_replace(s, _T("%1"), di.getLeftFilepath(pCtxt->GetNormalizedLeft()).c_str());
	}
	else if (di.diffcode.isSideRightOnly())
	{
		s = theApp.LoadString(IDS_RIGHT_ONLY_IN_FMT);
		string_replace(s, _T("%1"), di.getRightFilepath(pCtxt->GetNormalizedRight()).c_str());
	}
	else if (di.diffcode.isResultSame())
	{
		if (di.diffcode.isText())
			s = theApp.LoadString(IDS_TEXT_FILES_SAME);
		else if (di.diffcode.isBin())
			s = theApp.LoadString(IDS_BIN_FILES_SAME);
		else
			s = theApp.LoadString(IDS_IDENTICAL);
	}
	else if (di.diffcode.isResultDiff()) // diff
	{
		if (di.diffcode.isText())
			s = theApp.LoadString(IDS_TEXT_FILES_DIFF);
		else if (di.diffcode.isBin())
			s = theApp.LoadString(IDS_BIN_FILES_DIFF);
		else if (di.diffcode.isDirectory())
			s = theApp.LoadString(IDS_FOLDERS_ARE_DIFFERENT);
		else
			s = theApp.LoadString(IDS_FILES_ARE_DIFFERENT);
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
	const __int64 &r = *static_cast<const __int64*>(p);
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
	const __int64 &r = *static_cast<const __int64*>(p);
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
	const __int64 &r = *static_cast<const __int64*>(p);
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
static String ColNewerGet(const CDiffContext *, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	if (di.diffcode.isSideLeftOnly())
	{
		return _T("<*<");
	}
	if (di.diffcode.isSideRightOnly())
	{
		return _T(">*>");
	}
	if (di.left.mtime && di.right.mtime)
	{
		if (di.left.mtime > di.right.mtime)
		{
			return _T("<<");
		}
		if (di.left.mtime < di.right.mtime)
		{
			return _T(">>");
		}
		return _T("==");
	}
	return _T("***");
}

/**
 * @brief Format Version info to string.
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] pdi Pointer to DIFFITEM.
 * @param [in] bLeft Is the item left-size item?
 * @return String proper to show in the GUI.
 */
static String GetVersion(const CDiffContext * pCtxt, const DIFFITEM * pdi, BOOL bLeft)
{
	DIFFITEM & di = const_cast<DIFFITEM &>(*pdi);
	DiffFileInfo & dfi = bLeft ? di.left : di.right;
	if (!dfi.bVersionChecked)
	{
		pCtxt->UpdateVersion(di, bLeft);
	}
	return dfi.version.GetFileVersionString();
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
	return GetVersion(pCtxt, &di, TRUE);
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
	return GetVersion(pCtxt, &di, FALSE);
}

/**
 * @brief Format Short Result column data.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColStatusAbbrGet(const CDiffContext *, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	int id = 0;

	// Note that order of items does matter. We must check for
	// skipped items before unique items, for example, so that
	// skipped unique items are labeled as skipped, not unique.
	if (di.diffcode.isResultError())
	{
		id = IDS_CMPRES_ERROR;
	}
	else if (di.diffcode.isResultAbort())
	{
		id = IDS_ABORTED_ITEM;
	}
	else if (di.diffcode.isResultFiltered())
	{
		if (di.diffcode.isDirectory())
			id = IDS_DIR_SKIPPED;
		else
			id = IDS_FILE_SKIPPED;
	}
	else if (di.diffcode.isSideLeftOnly())
	{
		id = IDS_LEFTONLY;
	}
	else if (di.diffcode.isSideRightOnly())
	{
		id = IDS_RIGHTONLY;
	}
	else if (di.diffcode.isResultSame())
	{
		id = IDS_IDENTICAL;
	}
	else if (di.diffcode.isResultDiff())
	{
		id = IDS_DIFFERENT;
	}

	return id ? theApp.LoadString(id) : _T("");
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
	const DiffFileFlags &r = *static_cast<const DiffFileFlags *>(p);
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
static String GetEOLType(const CDiffContext *, const void *p, BOOL bLeft)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	const DiffFileInfo & dfi = bLeft ? di.left : di.right;
	const FileTextStats &stats = dfi.m_textStats;

	if (stats.ncrlfs == 0 && stats.ncrs == 0 && stats.nlfs == 0)
	{
		return String();
	}
	if (di.diffcode.isBin())
	{
		int id = IDS_EOL_BIN;
		return theApp.LoadString(id);
	}

	int id = 0;
	if (stats.ncrlfs > 0 && stats.ncrs == 0 && stats.nlfs == 0)
	{
		id = IDS_EOL_DOS;
	}
	else if (stats.ncrlfs == 0 && stats.ncrs > 0 && stats.nlfs == 0)
	{
		id = IDS_EOL_MAC;
	}
	else if (stats.ncrlfs == 0 && stats.ncrs == 0 && stats.nlfs > 0)
	{
		id = IDS_EOL_UNIX;
	}
	else
	{
		String s = theApp.LoadString(IDS_EOL_MIXED);
		TCHAR strstats[40];
		_sntprintf(strstats, countof(strstats), _T(":%d/%d/%d"), stats.ncrlfs, stats.ncrs, stats.nlfs);
		s += strstats;
		return s;
	}
	
	return theApp.LoadString(id);
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
	return GetEOLType(pCtxt, &di, TRUE);
}

/**
 * @brief Format EOL type column data (for right-side file).
 * @param [in] pCtxt Pointer to compare context.
 * @param [in] p Pointer to DIFFITEM.
 * @return String to show in the column.
 */
static String ColREOLTypeGet(const CDiffContext * pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetEOLType(pCtxt, &di, FALSE);
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
	return lstrcmpi(ColFileNameGet(pCtxt, p).c_str(), ColFileNameGet(pCtxt, q).c_str());
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
	const String &r = *static_cast<const String*>(p);
	const String &s = *static_cast<const String*>(q);
	return lstrcmpi(PathFindExtension(r.c_str()), PathFindExtension(s.c_str()));
	//return ColExtGet(pCtxt, p).CompareNoCase(ColExtGet(pCtxt, q));
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
	return lstrcmpi(ColPathGet(pCtxt, p).c_str(), ColPathGet(pCtxt, q).c_str());
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
	const __int64 &r = *static_cast<const __int64*>(p);
	const __int64 &s = *static_cast<const __int64*>(q);
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
	const __int64 &r = *static_cast<const __int64*>(p);
	const __int64 &s = *static_cast<const __int64*>(q);
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
	return ColLversionGet(pCtxt, p).compare(ColLversionGet(pCtxt, q));
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
	return ColRversionGet(pCtxt, p).compare(ColRversionGet(pCtxt, q));
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
	const DiffFileFlags &r = *static_cast<const DiffFileFlags *>(p);
	const DiffFileFlags &s = *static_cast<const DiffFileFlags *>(q);
	return r.ToString().compare(s.ToString());
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
 *  - parameter for custom functions: DIFFITEM (if NULL) or one of its fields
 *  - default column order number, -1 if not shown by default
 *  - ascending (TRUE) or descending (FALSE) default sort order
 *  - alignment of column contents: numbers are usually right-aligned
 */
static DirColInfo f_cols[] =
{
	{ _T("Name"), IDS_COLHDR_FILENAME, IDS_COLDESC_FILENAME, &ColFileNameGet, &ColFileNameSort, 0, 0, true, LVCFMT_LEFT },
	{ _T("Path"), IDS_COLHDR_DIR, IDS_COLDESC_DIR, &ColPathGet, &ColPathSort, 0, 1, true, LVCFMT_LEFT },
	{ _T("Status"), IDS_COLHDR_RESULT, IDS_COLDESC_RESULT, &ColStatusGet, &ColStatusSort, 0, 2, true, LVCFMT_LEFT },
	{ _T("Lmtime"), IDS_COLHDR_LTIMEM, IDS_COLDESC_LTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, left.mtime), 3, false, LVCFMT_LEFT },
	{ _T("Rmtime"), IDS_COLHDR_RTIMEM, IDS_COLDESC_RTIMEM, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, right.mtime), 4, false, LVCFMT_LEFT },
	{ _T("Lctime"), IDS_COLHDR_LTIMEC, IDS_COLDESC_LTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, left.ctime), -1, false, LVCFMT_LEFT },
	{ _T("Rctime"), IDS_COLHDR_RTIMEC, IDS_COLDESC_RTIMEC, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, right.ctime), -1, false, LVCFMT_LEFT },
	{ _T("Ext"), IDS_COLHDR_EXTENSION, IDS_COLDESC_EXTENSION, &ColExtGet, &ColExtSort, 0, 5, true, LVCFMT_LEFT },
	{ _T("Lsize"), IDS_COLHDR_LSIZE, IDS_COLDESC_LSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, left.size), -1, false, LVCFMT_RIGHT },
	{ _T("Rsize"), IDS_COLHDR_RSIZE, IDS_COLDESC_RSIZE, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, right.size), -1, false, LVCFMT_RIGHT },
	{ _T("LsizeShort"), IDS_COLHDR_LSIZE_SHORT, IDS_COLDESC_LSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, left.size), -1, false, LVCFMT_RIGHT },
	{ _T("RsizeShort"), IDS_COLHDR_RSIZE_SHORT, IDS_COLDESC_RSIZE_SHORT, &ColSizeShortGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, right.size), -1, false, LVCFMT_RIGHT },
	{ _T("Newer"), IDS_COLHDR_NEWER, IDS_COLDESC_NEWER, &ColNewerGet, &ColNewerSort, 0, -1, true, LVCFMT_LEFT },
	{ _T("Lversion"), IDS_COLHDR_LVERSION, IDS_COLDESC_LVERSION, &ColLversionGet, &ColLversionSort, 0, -1, true, LVCFMT_LEFT },
	{ _T("Rversion"), IDS_COLHDR_RVERSION, IDS_COLDESC_RVERSION, &ColRversionGet, &ColRversionSort, 0, -1, true, LVCFMT_LEFT },
	{ _T("StatusAbbr"), IDS_COLHDR_RESULT_ABBR, IDS_COLDESC_RESULT_ABBR, &ColStatusAbbrGet, &ColStatusSort, 0, -1, true, LVCFMT_LEFT },
	{ _T("Binary"), IDS_COLHDR_BINARY, IDS_COLDESC_BINARY, &ColBinGet, &ColBinSort, 0, -1, true, LVCFMT_LEFT },
	{ _T("Lattr"), IDS_COLHDR_LATTRIBUTES, IDS_COLDESC_LATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, left.flags), -1, true, LVCFMT_LEFT },
	{ _T("Rattr"), IDS_COLHDR_RATTRIBUTES, IDS_COLDESC_RATTRIBUTES, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, right.flags), -1, true, LVCFMT_LEFT },
	{ _T("Lencoding"), IDS_COLHDR_LENCODING, IDS_COLDESC_LENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, left), -1, true, LVCFMT_LEFT },
	{ _T("Rencoding"), IDS_COLHDR_RENCODING, IDS_COLDESC_RENCODING, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, right), -1, true, LVCFMT_LEFT },
	{ _T("Snsdiffs"), IDS_COLHDR_NSDIFFS, IDS_COLDESC_NSDIFFS, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, nsdiffs), -1, false, LVCFMT_RIGHT },
	{ _T("Snidiffs"), IDS_COLHDR_NIDIFFS, IDS_COLDESC_NIDIFFS, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, nidiffs), -1, false, LVCFMT_RIGHT },
	{ _T("Leoltype"), IDS_COLHDR_LEOL_TYPE, IDS_COLDESC_LEOL_TYPE, &ColLEOLTypeGet, 0, 0, -1, true, LVCFMT_LEFT },
	{ _T("Reoltype"), IDS_COLHDR_REOL_TYPE, IDS_COLDESC_REOL_TYPE, &ColREOLTypeGet, 0, 0, -1, true, LVCFMT_LEFT },
};

/**
 * @brief Count of all known columns
 */
int g_ncols = countof(f_cols);

/**
 * @brief Registry base value name for saving/loading info for this column
 */
CString
CDirView::GetColRegValueNameBase(int col) const
{
	ASSERT(col>=0 && col<countof(f_cols));
	CString regName;
	regName.Format(_T("WDirHdr_%s"), f_cols[col].regName);
	return regName;
}

/**
 * @brief Get default physical order for specified logical column
 */
int
CDirView::GetColDefaultOrder(int col) const
{
	ASSERT(col>=0 && col<countof(f_cols));
	return f_cols[col].physicalIndex;
}

/**
 * @brief Return the info about the specified physical column
 */
const DirColInfo *
CDirView::DirViewColItems_GetDirColInfo(int col) const
{
	if (col < 0 || col >= countof(f_cols))
	{
		ASSERT(0); // fix caller, should not ask for nonexistent columns
		return 0;
	}
	return &f_cols[col];
}

/**
 * @brief Check if specified physical column has specified resource id name
 */
static bool
IsColById(int col, int id)
{
	if (col < 0 || col >= countof(f_cols))
	{
		ASSERT(0); // fix caller, should not ask for nonexistent columns
		return false;
	}
	return f_cols[col].idName == id;
}

/**
 * @brief Is specified physical column the name column?
 */
bool
CDirView::IsColName(int col) const
{
	return IsColById(col, IDS_COLHDR_FILENAME);
}
/**
 * @brief Is specified physical column the left modification time column?
 */
bool
CDirView::IsColLmTime(int col) const
{
	return IsColById(col, IDS_COLHDR_LTIMEM);
}
/**
 * @brief Is specified physical column the right modification time column?
 */
bool
CDirView::IsColRmTime(int col) const
{
	return IsColById(col, IDS_COLHDR_RTIMEM);
}
/**
 * @brief Is specified physical column the full status (result) column?
 */
bool
CDirView::IsColStatus(int col) const
{
	return IsColById(col, IDS_COLHDR_RESULT);
}
/**
 * @brief Is specified physical column the full status (result) column?
 */
bool
CDirView::IsColStatusAbbr(int col) const
{
	return IsColById(col, IDS_COLHDR_RESULT_ABBR);
}
