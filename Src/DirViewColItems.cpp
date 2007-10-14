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
#include "Merge.h"
#include "DirView.h"
#include "DirDoc.h"
#include "MainFrm.h"
#include "resource.h"
#include "DirViewColItems.h"
#include "locality.h"
#include "unicoder.h"
#include "coretools.h"
#include "FolderCmp.h"

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
static int cmpdiffcode(int diffcode1, int diffcode2)
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
	_stprintf(buffer, _T("%lf"), number);
	return locality::GetLocaleStr(buffer, ndigits) + suffix;
}

/**
 * @name Functions to display each type of column info.
 */
/* @{ */
static String ColFileNameGet(const CDiffContext *, const void *p) //sfilename
{
	const DIFFITEM &di = *static_cast<const DIFFITEM*>(p);
	return
	(
		di.sLeftFilename.empty() ? di.sRightFilename :
		di.sRightFilename.empty() ? di.sLeftFilename :
		di.sLeftFilename == di.sRightFilename ? di.sLeftFilename :
		di.sLeftFilename + _T("|") + di.sRightFilename
	);
}
/*static CString ColNameGet(const CDiffContext *, const void *p) //sfilename
{
	const CString &r = *static_cast<const CString*>(p);
	return r;
}*/
static String ColExtGet(const CDiffContext *, const void *p) //sfilename
{
	const String &r = *static_cast<const String*>(p);
	LPCTSTR s = PathFindExtension(r.c_str());
	return s + _tcsspn(s, _T("."));
}
static String ColPathGet(const CDiffContext *, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM*>(p);
	String s = di.sRightSubdir;
	const String &t = di.sLeftSubdir;
	int i = 0, j = 0;
	do
	{
		int i_ahead = s.find('\\', i);
		int j_ahead = t.find('\\', j);
		int length_s = (i_ahead != -1 ? i_ahead : s.length()) - i;
		int length_t = (j_ahead != -1 ? j_ahead : t.length()) - j;
		if (length_s != length_t ||
			!StrIsIntlEqual(FALSE, s.c_str() + i, t.c_str() + j, length_s))
		{
			String u(t.c_str() + j, length_t + 1);
			u[length_t] = '|';
			//u.SetAt(length_t, '|');
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
		if (di.diffcode.isBin())
			s = theApp.LoadString(IDS_BIN_FILES_SAME);
		else
			s = theApp.LoadString(IDS_IDENTICAL);
	}
	else // diff
	{
		if (di.diffcode.isBin())
			s = theApp.LoadString(IDS_BIN_FILES_DIFF);
		else
			s = theApp.LoadString(IDS_FILES_ARE_DIFFERENT);
	}
	return s;
}
static String ColTimeGet(const CDiffContext *, const void *p)
{
	const __int64 &r = *static_cast<const __int64*>(p);
	if (r)
		return TimeString(&r);
	else
		return _T("");
}
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
static String ColLversionGet(const CDiffContext * pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetVersion(pCtxt, &di, TRUE);
}
static String ColRversionGet(const CDiffContext * pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetVersion(pCtxt, &di, FALSE);
}
static String ColStatusAbbrGet(const CDiffContext *, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	int id;

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
	else // diff
	{
		id = IDS_DIFFERENT;
	}

	return theApp.LoadString(id);
}
static String ColBinGet(const CDiffContext *, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);

	if (di.diffcode.isBin())
		return _T("*");
	else
		return _T("");
}
static String ColAttrGet(const CDiffContext *, const void *p)
{
	const DiffFileFlags &r = *static_cast<const DiffFileFlags *>(p);
	return r.toString();
}
static String ColEncodingGet(const CDiffContext *, const void *p)
{
	const DiffFileInfo &r = *static_cast<const DiffFileInfo *>(p);
	return r.encoding.GetName();
}
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
		_stprintf(strstats, _T(":%d/%d/%d"), stats.ncrlfs, stats.ncrs, stats.nlfs);
		s += strstats;
		return s;
	}
	
	return theApp.LoadString(id);
}
static String ColLEOLTypeGet(const CDiffContext * pCtxt, const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	return GetEOLType(pCtxt, &di, TRUE);
}
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
 */
/* @{ */ 
static int ColFileNameSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	const DIFFITEM &ldi = *static_cast<const DIFFITEM *>(p);
	const DIFFITEM &rdi = *static_cast<const DIFFITEM *>(q);
	if (ldi.diffcode.isDirectory() && !rdi.diffcode.isDirectory())
		return -1;
	if (!ldi.diffcode.isDirectory() && rdi.diffcode.isDirectory())
		return 1;
	return lstrcmpi(ColFileNameGet(pCtxt, p).c_str(), ColFileNameGet(pCtxt, q).c_str());
	//return ldi.sLeftFilename.CompareNoCase(rdi.sLeftFilename);
}
/*static int ColNameSort(const CDiffContext *, const void *p, const void *q)
{
	const CString &r = *static_cast<const CString*>(p);
	const CString &s = *static_cast<const CString*>(q);
	return r.CompareNoCase(s);
}*/
static int ColExtSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	const String &r = *static_cast<const String*>(p);
	const String &s = *static_cast<const String*>(q);
	return lstrcmpi(PathFindExtension(r.c_str()), PathFindExtension(s.c_str()));
	//return ColExtGet(pCtxt, p).CompareNoCase(ColExtGet(pCtxt, q));
}
static int ColPathSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	return lstrcmpi(ColPathGet(pCtxt, p).c_str(), ColPathGet(pCtxt, q).c_str());
}
static int ColStatusSort(const CDiffContext *, const void *p, const void *q)
{
	const DIFFITEM &ldi = *static_cast<const DIFFITEM *>(p);
	const DIFFITEM &rdi = *static_cast<const DIFFITEM *>(q);
	return cmpdiffcode(rdi.diffcode.diffcode, ldi.diffcode.diffcode);
}
static int ColTimeSort(const CDiffContext *, const void *p, const void *q)
{
	const __int64 &r = *static_cast<const __int64*>(p);
	const __int64 &s = *static_cast<const __int64*>(q);
	return cmp64(r, s);
}
static int ColSizeSort(const CDiffContext *, const void *p, const void *q)
{
	const __int64 &r = *static_cast<const __int64*>(p);
	const __int64 &s = *static_cast<const __int64*>(q);
	return cmp64(r, s);
}
static int ColDiffsSort(const CDiffContext *, const void *p, const void *q)
{
	const int &r = *static_cast<const int*>(p);
	const int &s = *static_cast<const int*>(q);
	return r - s;
}
static int ColNewerSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	return ColNewerGet(pCtxt, p).compare(ColNewerGet(pCtxt, q));
}
static int ColLversionSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	return ColLversionGet(pCtxt, p).compare(ColLversionGet(pCtxt, q));
}
static int ColRversionSort(const CDiffContext *pCtxt, const void *p, const void *q)
{
	return ColRversionGet(pCtxt, p).compare(ColRversionGet(pCtxt, q));
}
static int ColBinSort(const CDiffContext *, const void *p, const void *q)
{
	const DIFFITEM &ldi = *static_cast<const DIFFITEM *>(p);
	const DIFFITEM &rdi = *static_cast<const DIFFITEM *>(q);
	int i = ldi.diffcode.isBin();
	int j = rdi.diffcode.isBin();

	return i - j;
}
static int ColAttrSort(const CDiffContext *, const void *p, const void *q)
{
	const DiffFileFlags &r = *static_cast<const DiffFileFlags *>(p);
	const DiffFileFlags &s = *static_cast<const DiffFileFlags *>(q);
	return r.toString().compare(s.toString());
}
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
	{ _T("Ext"), IDS_COLHDR_EXTENSION, IDS_COLDESC_EXTENSION, &ColExtGet, &ColExtSort, FIELD_OFFSET(DIFFITEM, sLeftFilename), 5, true, LVCFMT_LEFT },
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
	if (col < 0 || col >= sizeof(f_cols)/sizeof(f_cols[0]))
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
	if (col < 0 || col >= sizeof(f_cols)/sizeof(f_cols[0]))
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
