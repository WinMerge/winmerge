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
//#include "coretools.h"
//#include "dllver.h"
#include "DirViewColItems.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Return time displayed appropriately, as string
 */
static CString
TimeString(const time_t * tim)
{
	if (!tim) return _T("---");
	// _tcsftime does not respect user date customizations from
	// Regional Options/Configuration Regional; COleDateTime::Format does so.
	COleDateTime odt = *tim;
	return odt.Format();
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
	CString s;
	switch (di.code)
	{
	case FILE_DIFF:
		VERIFY(s.LoadString(IDS_FILES_ARE_DIFFERENT));
		break;
	case FILE_BINDIFF:
		VERIFY(s.LoadString(IDS_BIN_FILES_DIFF));
		break;
	case FILE_BINSAME:
		VERIFY(s.LoadString(IDS_BIN_FILES_SAME));
		break;
	case FILE_LUNIQUE:
	case FILE_LDIRUNIQUE:
		AfxFormatString1(s, IDS_ONLY_IN_FMT, di.getLeftFilepath());
		break;
	case FILE_RUNIQUE:
	case FILE_RDIRUNIQUE:
		AfxFormatString1(s, IDS_ONLY_IN_FMT, di.getRightFilepath());
		break;
	case FILE_SAME:
		VERIFY(s.LoadString(IDS_IDENTICAL));
		break;
	case FILE_SKIP:
		VERIFY(s.LoadString(IDS_FILE_SKIPPED));
		break;
	case FILE_DIRSKIP:
		VERIFY(s.LoadString(IDS_DIR_SKIPPED));
		break;
	default: // error
		VERIFY(s.LoadString(IDS_CANT_COMPARE_FILES));
		break;
	}
	return s;
}
static CString ColLmtimeGet(const DIFFITEM & di)
{
	if (di.left.mtime)
		return TimeString(&di.left.mtime);
	else
		return _T("");
}
static CString ColRmtimeGet(const DIFFITEM & di)
{
	if (di.right.mtime)
		return TimeString(&di.right.mtime);
	else
		return _T("");
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
	return s;
}
static CString ColRsizeGet(const DIFFITEM & di)
{
	CString s;
	s.Format(_T("%I64d"), di.right.size);
	return s;
}
static CString ColNewerGet(const DIFFITEM & di)
{
	if (di.left.mtime && di.right.mtime)
	{
		if (di.left.mtime > di.right.mtime)
			return _T("<<<");
		if (di.left.mtime < di.right.mtime)
			return _T(">>>");
		return _T("===");
	}
	else if (di.left.mtime)
	{
		return _T("<*<");
	}
	else if (di.right.mtime)
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
	switch (di.code)
	{
	case FILE_DIFF: id = IDS_FILES_ARE_DIFFERENT; break;
	case FILE_BINDIFF: id = IDS_BIN_FILES_DIFF; break;
	case FILE_BINSAME: id = IDS_BIN_FILES_SAME; break;
	case FILE_LUNIQUE:
	case FILE_LDIRUNIQUE:
		id = IDS_LEFTONLY; break;
	case FILE_RUNIQUE:
	case FILE_RDIRUNIQUE:
		id = IDS_RIGHTONLY; break;
	case FILE_SAME: id = IDS_IDENTICAL; break;
	case FILE_SKIP: id = IDS_FILE_SKIPPED; break;
	case FILE_DIRSKIP: id = IDS_DIR_SKIPPED; break;
	default: id = IDS_CANT_COMPARE_FILES;
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
	return rdi.code-ldi.code;
}
static int ColLmtimeSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return rdi.left.mtime-ldi.left.mtime;
}
static int ColRmtimeSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return rdi.right.mtime-ldi.right.mtime;
}
static int ColLctimeSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return rdi.left.ctime-ldi.left.ctime;
}
static int ColRctimeSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return rdi.right.ctime-ldi.right.ctime;
}
static int ColExtSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return ldi.sext.CompareNoCase(rdi.sext);
}
static int ColLsizeSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return rdi.left.size - ldi.left.size;
}
static int ColRsizeSort(const DIFFITEM & ldi, const DIFFITEM &rdi)
{
	return rdi.right.size - ldi.right.size;
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
/**
 * @}
 */

/**
 * @brief All existing columns
 */
DirColInfo g_cols[] =
{
	{ _T("Name"), IDS_COLHDR_FILENAME, -1, &ColNameGet, &ColNameSort, 0, true }
	, { _T("Path"), IDS_COLHDR_DIR, -1, &ColPathGet, &ColPathSort, 1, true }
	, { _T("Status"), IDS_COLHDR_RESULT, -1, &ColStatusGet, &ColStatusSort, 2, true}
	, { _T("Lmtime"), IDS_COLHDR_LTIMEM, -1, &ColLmtimeGet, &ColLmtimeSort, 3, false }
	, { _T("Rmtime"), IDS_COLHDR_RTIMEM, -1, &ColRmtimeGet, &ColRmtimeSort, 4, false }
	, { _T("Lctime"), IDS_COLHDR_LTIMEC, -1, &ColLctimeGet, &ColLctimeSort, -1, false }
	, { _T("Rctime"), IDS_COLHDR_RTIMEC, -1, &ColRctimeGet, &ColRctimeSort, -1, false }
	, { _T("Ext"), IDS_COLHDR_EXTENSION, -1, &ColExtGet, &ColExtSort, 5, true }
	, { _T("Lsize"), IDS_COLHDR_LSIZE, -1, &ColLsizeGet, &ColLsizeSort, -1, true }
	, { _T("Rsize"), IDS_COLHDR_RSIZE, -1, &ColRsizeGet, &ColRsizeSort, -1, true }
	, { _T("Newer"), IDS_COLHDR_NEWER, -1, &ColNewerGet, &ColNewerSort, -1, true }
	, { _T("Lversion"), IDS_COLHDR_LVERSION, -1, &ColLverGet, &ColLverSort, -1, true }
	, { _T("Rversion"), IDS_COLHDR_RVERSION, -1, &ColRverGet, &ColRverSort, -1, true }
	, { _T("StatusAbbr"), IDS_COLHDR_RESULT_ABBR, -1, &ColStatusAbbrGet, &ColStatusSort, -1, true }
	, { _T("Lattr"), IDS_COLHDR_LATTRIBUTES, -1, &ColLattrGet, &ColLattrSort, -1, true }
	, { _T("Rattr"), IDS_COLHDR_RATTRIBUTES, -1, &ColRattrGet, &ColRattrSort, -1, true }
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
