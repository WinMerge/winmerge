/** 
 * @file  DirViewColItems.cpp
 *
 * @brief Code for individual columns in the DirView
 *
 * @date  Created: 2003-08-19
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


#include "stdafx.h"
#include "Merge.h"
#include "DirView.h"
#include "DirDoc.h"
#include "MainFrm.h"
#include "resource.h"
#include "DirViewColItems.h"
#include "locality.h"
#include "unicoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Return time displayed appropriately, as string
 */
static CString
TimeString(const __int64 * tim)
{
	if (!tim) return _T("---");
	// _tcsftime does not respect user date customizations from
	// Regional Options/Configuration Regional; COleDateTime::Format does so.
#if _MSC_VER < 1300
		// MSVC6
	COleDateTime odt = (time_t)*tim;
#else
		// MSVC7 (VC.NET)
	COleDateTime odt = *tim;
#endif
	return odt.Format();
}
/**
 * @brief Return string representation of encoding, eg "UCS-2LE", or "1252"
 */
static CString EncodingString(int unicoding, int codepage)
{
	if (unicoding == ucr::UCS2LE)
		return _T("UCS-2LE");
	if (unicoding == ucr::UCS2BE)
		return _T("UCS-2BE");
	if (unicoding == ucr::UTF8)
		return _T("UTF-8");
	CString str;
	LPTSTR s = str.GetBuffer(32);
	_sntprintf(s, 32, _T("%d"), codepage);
	str.ReleaseBuffer();
	return str;
}
/**
 * @brief Function to compare two __int64s for a sort
 */
static int cmp64(__int64 i1, __int64 i2)
{
	if (i1==i2) return 0;
	return i1>i2 ? 1 : -1;
}
/**
 * @brief Function to compare two diffcodes for a sort
 */
static int cmpdiffcode(int diffcode1, int diffcode2)
{
	// TODO: How shall we order these ?
	return diffcode1-diffcode2;	
}
/**
 * @brief Function to compare two COleDateTimes for a sort
 */
static int cmptime(COleDateTime t1, COleDateTime t2)
{
	if (int cmp = t2.GetStatus() - t1.GetStatus())
		return cmp;
	if (t1.GetStatus() == COleDateTime::valid)
	{
		if (t1>t2)
			return 1;
		if (t1<t2)
			return -1;
	}
	return 0;
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
;
}

/**
 * @{ Functions to display each type of column info
 */
static CString ColNameGet(const DIFFITEM & di)
{
	return di.sfilename;
}
static CString ColPathGet(const DIFFITEM & di)
{
	if (di.sSubdir.IsEmpty())
		return _T(".");
	else
		return di.sSubdir;
}
static CString ColStatusGet(const DIFFITEM & di)
{
	// Note that order of items does matter. We must check for
	// skipped items before unique items, for example, so that
	// skipped unique items are labeled as skipped, not unique.
	CString s;
	if (di.isResultError())
	{
		VERIFY(s.LoadString(IDS_CANT_COMPARE_FILES));
	}
	else if (di.isResultSkipped())
	{
		if (di.isDirectory())
			VERIFY(s.LoadString(IDS_DIR_SKIPPED));
		else
			VERIFY(s.LoadString(IDS_FILE_SKIPPED));
	}
	else if (di.isSideLeft())
	{
		AfxFormatString1(s, IDS_ONLY_IN_FMT, di.getLeftFilepath());
	}
	else if (di.isSideRight())
	{
		AfxFormatString1(s, IDS_ONLY_IN_FMT, di.getRightFilepath());
	}
	else if (di.isResultSame())
	{
		if (di.isBin())
			VERIFY(s.LoadString(IDS_BIN_FILES_SAME));
		else
			VERIFY(s.LoadString(IDS_IDENTICAL));
	}
	else // diff
	{
		if (di.isBin())
			VERIFY(s.LoadString(IDS_BIN_FILES_DIFF));
		else
			VERIFY(s.LoadString(IDS_FILES_ARE_DIFFERENT));
	}
	return s;
}
static CString ColLctimeGet(const DIFFITEM & di)
{
	if (di.left.ctime)
		return TimeString(&di.left.ctime);
	else
		return _T("");
}
static CString ColRctimeGet(const DIFFITEM & di)
{
	if (di.right.ctime)
		return TimeString(&di.right.ctime);
	else
		return _T("");
}
static CString ColExtGet(const DIFFITEM & di)
{
	return di.sext;
}
static CString ColLsizeGet(const DIFFITEM & di)
{
	CString s;
	s.Format(_T("%I64d"), di.left.size);
	return locality::GetLocaleStr(s);
}
static CString ColRsizeGet(const DIFFITEM & di)
{
	CString s;
	s.Format(_T("%I64d"), di.right.size);
	return locality::GetLocaleStr(s);
}

static CString ColNewerGet(const DIFFITEM & di)
{
	const varprop::Property * prop = di.shprops.GetProperty(_T("Snewer"));
	if (prop && prop->value.isString())
	{
		CString newer = prop->value.getString();
		if (newer == _T("L"))
			return _T("<<<");
		else if (newer == _T("R"))
			return _T(">>");
		else
			return _T("==");
	}
	else if (di.isSideLeft())
	{
		return _T("<*<");
	}
	else if (di.isSideRight())
	{
		return _T(">*>");
	}
	else
	{
		return _T("***");
	}
}
static CString ColLverGet(const DIFFITEM & di)
{
	return di.left.version;
}
static CString ColRverGet(const DIFFITEM & di)
{
	return di.right.version;
}
static CString ColStatusAbbrGet(const DIFFITEM & di)
{
	int id;

	// Note that order of items does matter. We must check for
	// skipped items before unique items, for example, so that
	// skipped unique items are labeled as skipped, not unique.
	if (di.isResultError())
	{
		id = IDS_CANT_COMPARE_FILES;
	}
	else if (di.isResultSkipped())
	{
		if (di.isDirectory())
			id = IDS_DIR_SKIPPED;
		else
			id = IDS_FILE_SKIPPED;
	}
	else if (di.isSideLeft())
	{
		id = IDS_LEFTONLY;
	}
	else if (di.isSideRight())
	{
		id = IDS_RIGHTONLY;
	}
	else if (di.isResultSame())
	{
		if (di.isBin())
			id = IDS_BIN_FILES_SAME;
		else
			id = IDS_IDENTICAL;
	}
	else // diff
	{
		if (di.isBin())
			id = IDS_BIN_FILES_DIFF;
		else
			id = IDS_FILES_ARE_DIFFERENT;
	}

	CString s;
	VERIFY(s.LoadString(id));
	return s;
}
static CString ColLattrGet(const DIFFITEM & di)
{
	return di.left.flags.toString();
}
static CString ColRattrGet(const DIFFITEM & di)
{
	return di.right.flags.toString();
}
static CString ColLencodingGet(const DIFFITEM & di)
{
	return EncodingString(di.left.unicoding, di.left.codepage);
}
static CString ColRencodingGet(const DIFFITEM & di)
{
	return EncodingString(di.right.unicoding, di.right.codepage);
}
/**
 * @}
 */

/**
 * @{ Functions to sort each type of column info
 */
static int ColNameSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return ldi.sfilename.CompareNoCase(rdi.sfilename);
}
static int ColPathSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return ldi.sSubdir.CompareNoCase(rdi.sSubdir);
}
static int ColStatusSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return cmpdiffcode(rdi.diffcode, ldi.diffcode);
}
static int ColExtSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return ldi.sext.CompareNoCase(rdi.sext);
}
static int ColLsizeSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return cmp64(rdi.left.size, ldi.left.size);
}
static int ColRsizeSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return cmp64(rdi.right.size, ldi.right.size);
}
static int ColNewerSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return ColNewerGet(ldi).Compare(ColNewerGet(rdi));
}
static int ColLverSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return ldi.left.version.Compare(rdi.left.version);
}
static int ColRverSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return ldi.right.version.Compare(rdi.right.version);
}
static int ColLattrSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return ldi.left.flags.toString().Compare(rdi.left.flags.toString());
}
static int ColRattrSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return ldi.right.flags.toString().Compare(rdi.right.flags.toString());
}
static int ColLencodingSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	__int64 n = cmp64(ldi.left.unicoding, rdi.left.unicoding);
	if (n) return n;
	n = cmp64(ldi.left.codepage, rdi.left.codepage);
	return n;
}
static int ColRencodingSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	__int64 n = cmp64(ldi.right.unicoding, rdi.right.unicoding);
	if (n) return n;
	n = cmp64(ldi.right.codepage, rdi.right.codepage);
	return n;
}
/**
 * @}
 */

/**
 * @brief All existing columns
 *
 * Column internal name, followed by resource ID for localized name
 *  then resource ID for localized description (all -1 currently)
 *  then custom get & custom sort functions (NULL for generic properties)
 *  then default order (or -1 if not shown by default), then whether to start ascending
 */
DirColInfo g_cols[] =
{
	{ _T("Name"), IDS_COLHDR_FILENAME, -1, &ColNameGet, &ColNameSort, 0, true }
	, { _T("Path"), IDS_COLHDR_DIR, -1, &ColPathGet, &ColPathSort, 1, true }
	, { _T("Status"), IDS_COLHDR_RESULT, -1, &ColStatusGet, &ColStatusSort, 2, true}
	, { _T("Lmtime"), IDS_COLHDR_LTIMEM, -1, NULL, NULL, 3, false }
	, { _T("Rmtime"), IDS_COLHDR_RTIMEM, -1, NULL, NULL, 4, false }
	, { _T("Lctime"), IDS_COLHDR_LTIMEC, -1, NULL, NULL, -1, false }
	, { _T("Rctime"), IDS_COLHDR_RTIMEC, -1, NULL, NULL, -1, false }
	, { _T("Ext"), IDS_COLHDR_EXTENSION, -1, &ColExtGet, &ColExtSort, 5, true }
	, { _T("Lsize"), IDS_COLHDR_LSIZE, -1, &ColLsizeGet, &ColLsizeSort, -1, true }
	, { _T("Rsize"), IDS_COLHDR_RSIZE, -1, &ColRsizeGet, &ColRsizeSort, -1, true }
	, { _T("Newer"), IDS_COLHDR_NEWER, -1, &ColNewerGet, &ColNewerSort, -1, true }
	, { _T("Lversion"), IDS_COLHDR_LVERSION, -1, &ColLverGet, &ColLverSort, -1, true }
	, { _T("Rversion"), IDS_COLHDR_RVERSION, -1, &ColRverGet, &ColRverSort, -1, true }
	, { _T("StatusAbbr"), IDS_COLHDR_RESULT_ABBR, -1, &ColStatusAbbrGet, &ColStatusSort, -1, true }
	, { _T("Lattr"), IDS_COLHDR_LATTRIBUTES, -1, &ColLattrGet, &ColLattrSort, -1, true }
	, { _T("Rattr"), IDS_COLHDR_RATTRIBUTES, -1, &ColRattrGet, &ColRattrSort, -1, true }
	, { _T("Lencoding"), IDS_COLHDR_LENCODING, -1, &ColLencodingGet, &ColLencodingSort, -1, true }
	, { _T("Rencoding"), IDS_COLHDR_RENCODING, -1, &ColRencodingGet, &ColRencodingSort, -1, true }
	, { _T("Sndiffs"), IDS_COLHDR_NDIFFS, -1, NULL, NULL, -1, false }
	, { _T("Snsdiffs"), IDS_COLHDR_NSDIFFS, -1, NULL, NULL, -1, false }
};

/**
 * @brief Count of all known columns
 */
int g_ncols = countof(g_cols);

/**
 * @brief Registry base value name for saving/loading info for this column
 */
CString CDirView::GetColRegValueNameBase(int col) const
{
	ASSERT(col>=0 && col<countof(g_cols));
	CString regName;
	regName.Format(_T("WDirHdr_%s"), g_cols[col].regName);
	return regName;
}

/**
 * @brief Get default physical order for specified logical column
 */
int CDirView::GetColDefaultOrder(int col) const
{
	ASSERT(col>=0 && col<countof(g_cols));
	return g_cols[col].physicalIndex;
}

/**
 * @brief Format and display a generic property
 */
CString CDirView::GetColItemDisplay(const varprop::VariantValue & var)
{
	if (var.isString())
	{
		return var.getString();
	}
	if (var.isInt())
	{
		CString str;
		str.Format(_T("%d"), var.getInt());
		return locality::GetLocaleStr(str);
	}
	if (var.isTime())
	{
		if (var.getTime().GetStatus() != COleDateTime::valid)
			return _T("");
		return var.getTime().Format();
	}
	if (var.isFloat())
	{
		CString str;
		str.Format(_T("%lf"), var.getFloat());
		return str;
	}
	return _T("?");
}

/**
 * @brief Compare two generic property values (used in dirview sort)
 */
int CDirView::GenericSortItem(const varprop::VariantValue * lvar, const varprop::VariantValue * rvar)
{
	// If not the same type, simply compare types (this really shouldn't happen anyway)
	if (!lvar)
		return rvar ? 1 : 0;
	if (!rvar)
		return -1;
	if (lvar->getType() > rvar->getType())
		return 1;
	else if (lvar->getType() < rvar->getType())
		return -1;
	// Ok, they're the same type, so we can compare their values
	if (lvar->isString())
		return lvar->getString().Compare(rvar->getString());
	if (lvar->isInt())
		return cmp64(lvar->getInt(), rvar->getInt());
	if (lvar->isTime())
		return cmptime(lvar->getTime(), rvar->getTime());
	if (lvar->isFloat())
		return cmpfloat(lvar->getFloat(), rvar->getFloat());
	ASSERT(0);
	return 0;
}


