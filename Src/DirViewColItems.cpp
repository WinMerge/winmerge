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
 * @{ Functions to display each type of column info
 */
static CString ColNameGet(const void *p) //sfilename
{
	const CString &r = *static_cast<const CString*>(p);
	return r;
}
static CString ColPathGet(const void *p)
{
	const CString &r = *static_cast<const CString*>(p);
	if (r.IsEmpty())
		return _T(".");
	else
		return r;
}
static CString ColStatusGet(const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM*>(p);
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
static CString ColTimeGet(const void *p)
{
	const __int64 &r = *static_cast<const __int64*>(p);
	if (r)
		return TimeString(&r);
	else
		return _T("");
}
static CString ColSizeGet(const void *p)
{
	const __int64 &r = *static_cast<const __int64*>(p);
	CString s;
	s.Format(_T("%I64d"), r);
	return locality::GetLocaleStr(s);
}
static CString ColDiffsGet(const void *p)
{
	const int &r = *static_cast<const int*>(p);
	CString s;
	if (r != -1)
	{
		s.Format(_T("%ld"), r);
		s = locality::GetLocaleStr(s);
	}
	return s;
}
static CString ColNewerGet(const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
	if (di.isSideLeft())
	{
		return _T("<*<");
	}
	if (di.isSideRight())
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
static CString ColStatusAbbrGet(const void *p)
{
	const DIFFITEM &di = *static_cast<const DIFFITEM *>(p);
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
static CString ColAttrGet(const void *p)
{
	const FileFlags &r = *static_cast<const FileFlags *>(p);
	return r.toString();
}
static CString ColEncodingGet(const void *p)
{
	const DiffFileInfo &r = *static_cast<const DiffFileInfo *>(p);
	return EncodingString(r.unicoding, r.codepage);
}
/**
 * @}
 */

/**
 * @{ Functions to sort each type of column info
 */
static int ColNameSort(const void *p, const void *q)
{
	const CString &r = *static_cast<const CString*>(p);
	const CString &s = *static_cast<const CString*>(q);
	return r.CompareNoCase(s);
}

static int ColStatusSort(const void *p, const void *q)
{
	const DIFFITEM &ldi = *static_cast<const DIFFITEM *>(p);
	const DIFFITEM &rdi = *static_cast<const DIFFITEM *>(q);
	return cmpdiffcode(rdi.diffcode, ldi.diffcode);
}
static int ColTimeSort(const void *p, const void *q)
{
	const __int64 &r = *static_cast<const __int64*>(p);
	const __int64 &s = *static_cast<const __int64*>(q);
	return cmp64(r, s);
}
static int ColSizeSort(const void *p, const void *q)
{
	const __int64 &r = *static_cast<const __int64*>(p);
	const __int64 &s = *static_cast<const __int64*>(q);
	return cmp64(r, s);
}
static int ColDiffsSort(const void *p, const void *q)
{
	const int &r = *static_cast<const int*>(p);
	const int &s = *static_cast<const int*>(q);
	return r - s;
}
static int ColNewerSort(const void *p, const void *q)
{
	return ColNewerGet(p).Compare(ColNewerGet(q));
}
static int ColAttrSort(const void *p, const void *q)
{
	const FileFlags &r = *static_cast<const FileFlags *>(p);
	const FileFlags &s = *static_cast<const FileFlags *>(q);
	return r.toString().Compare(s.toString());
}
static int ColEncodingSort(const void *p, const void *q)
{
	const DiffFileInfo &r = *static_cast<const DiffFileInfo *>(p);
	const DiffFileInfo &s = *static_cast<const DiffFileInfo *>(q);
	__int64 n = cmp64(r.unicoding, s.unicoding);
	if (n) return n;
	n = cmp64(r.codepage, s.codepage);
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
	{ _T("Name"), IDS_COLHDR_FILENAME, -1, &ColNameGet, &ColNameSort, FIELD_OFFSET(DIFFITEM, sfilename), 0, true },
	{ _T("Path"), IDS_COLHDR_DIR, -1, &ColPathGet, &ColNameSort, FIELD_OFFSET(DIFFITEM, sSubdir), 1, true },
	{ _T("Status"), IDS_COLHDR_RESULT, -1, &ColStatusGet, &ColStatusSort, 0, 2, true},
	{ _T("Lmtime"), IDS_COLHDR_LTIMEM, -1, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, left.mtime), 3, false },
	{ _T("Rmtime"), IDS_COLHDR_RTIMEM, -1, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, right.mtime), 4, false },
	{ _T("Lctime"), IDS_COLHDR_LTIMEC, -1, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, left.ctime), -1, false },
	{ _T("Rctime"), IDS_COLHDR_RTIMEC, -1, &ColTimeGet, &ColTimeSort, FIELD_OFFSET(DIFFITEM, right.ctime), -1, false },
	{ _T("Ext"), IDS_COLHDR_EXTENSION, -1, &ColNameGet, &ColNameSort, FIELD_OFFSET(DIFFITEM, sext), 5, true },
	{ _T("Lsize"), IDS_COLHDR_LSIZE, -1, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, left.size), -1, false },
	{ _T("Rsize"), IDS_COLHDR_RSIZE, -1, &ColSizeGet, &ColSizeSort, FIELD_OFFSET(DIFFITEM, right.size), -1, false },
	{ _T("Newer"), IDS_COLHDR_NEWER, -1, &ColNewerGet, &ColNewerSort, 0, -1, true },
	{ _T("Lversion"), IDS_COLHDR_LVERSION, -1, &ColNameGet, &ColNameSort, FIELD_OFFSET(DIFFITEM, left.version), -1, true },
	{ _T("Rversion"), IDS_COLHDR_RVERSION, -1, &ColNameGet, &ColNameSort, FIELD_OFFSET(DIFFITEM, right.version), -1, true },
	{ _T("StatusAbbr"), IDS_COLHDR_RESULT_ABBR, -1, &ColStatusAbbrGet, &ColStatusSort, 0, -1, true },
	{ _T("Lattr"), IDS_COLHDR_LATTRIBUTES, -1, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, left.flags), -1, true },
	{ _T("Rattr"), IDS_COLHDR_RATTRIBUTES, -1, &ColAttrGet, &ColAttrSort, FIELD_OFFSET(DIFFITEM, right.flags), -1, true },
	{ _T("Lencoding"), IDS_COLHDR_LENCODING, -1, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, left), -1, true },
	{ _T("Rencoding"), IDS_COLHDR_RENCODING, -1, &ColEncodingGet, &ColEncodingSort, FIELD_OFFSET(DIFFITEM, right), -1, true },
	{ _T("Sndiffs"), IDS_COLHDR_NDIFFS, -1, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, ndiffs), -1, false },
	{ _T("Snsdiffs"), IDS_COLHDR_NSDIFFS, -1, ColDiffsGet, ColDiffsSort, FIELD_OFFSET(DIFFITEM, nsdiffs), -1, false },
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
