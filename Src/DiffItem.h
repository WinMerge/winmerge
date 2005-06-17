/**
 *  @file DiffItem.h
 *
 *  @brief Declaration of DIFFITEM
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef _DIFF_ITEM_H_
#define _DIFF_ITEM_H_

#ifndef _DIFF_FILE_INFO_H_INCLUDED
#include "DiffFileInfo.h"
#endif

/**
 * @brief values for DIFFITEM.code
 */
struct DIFFCODE
{
	enum
	{
		// We use extra bits so that no valid values are 0
		// and each set of flags is in a different hex digit
		// to make debugging easier
		// These can always be packed down in the future
		TEXTFLAG=0x3, TEXT=0x1, BIN=0x2,
		DIRFLAG=0x30, FILE=0x10, DIR=0x20,
		SIDEFLAG=0x300, LEFT=0x100, RIGHT=0x200, BOTH=0x300,
		COMPAREFLAGS=0x7000, NOCMP=0x0000, SAME=0x1000, DIFF=0x2000, CMPERR=0x4000,
		FILTERFLAGS=0x30000, INCLUDED=0x10000, SKIPPED=0x20000,
		SCANFLAGS=0x100000, NEEDSCAN=0x100000,
	};

	int diffcode;

	DIFFCODE() : diffcode(0) { }
	DIFFCODE(int diffcode) : diffcode(diffcode) { }

	// file/directory
	bool isDirectory() const { return ((diffcode & DIFFCODE::DIRFLAG) == DIFFCODE::DIR); }
	// left/right
	bool isSideLeft() const { return ((diffcode & DIFFCODE::SIDEFLAG) == DIFFCODE::LEFT); }
	bool isSideRight() const { return ((diffcode & DIFFCODE::SIDEFLAG) == DIFFCODE::RIGHT); }
	// result filters
	bool isResultError() const { return ((diffcode & DIFFCODE::COMPAREFLAGS) == DIFFCODE::CMPERR); }
	bool isResultSame() const { return ((diffcode & DIFFCODE::COMPAREFLAGS) == DIFFCODE::SAME); }
	bool isResultDiff() const { return (!isResultSame() && !isResultFiltered() && !isResultError() &&
			!isSideLeft() && !isSideRight()); }
	bool isResultFiltered() const { return ((diffcode & DIFFCODE::FILTERFLAGS) == DIFFCODE::SKIPPED); }
	// type
	bool isBin() const { return ((diffcode & DIFFCODE::TEXTFLAG) == DIFFCODE::BIN); }
	// rescan
	bool isScanNeeded() const { return ((diffcode & DIFFCODE::SCANFLAGS) == DIFFCODE::NEEDSCAN); }

};

/**
 * @brief information about one diff (including files on both sides)
 *
 * Bitmask can be seen as a 4 dimensional space; that is, there are four
 * different attributes, and each entry picks one of each attribute
 * independently.
 *
 * One dimension is how the compare went: same or different or
 * skipped or error.
 *
 * One dimension is file mode: text or binary (text is only if
 * both sides were text)
 *
 * One dimension is existence: both sides, left only, or right only
 *
 * One dimension is type: directory, or file
 *
 * @note times in fileinfo's are seconds since January 1, 1970.
 * See Dirscan.cpp/fentry and Dirscan.cpp/LoadFiles()
 */
struct DIFFITEM : DIFFCODE
{
	DiffFileInfo left; /**< Fileinfo for left file */
	DiffFileInfo right; /**< Fileinfo for right file */
	CString sfilename; /**< Filename (without path!) */
	CString sSubdir; //*< Common subdirectory from root of comparison */
	int	nsdiffs; /**< Amount of non-ignored differences */
	int ndiffs; /**< Total amount of differences */

	DIFFITEM() : ndiffs(-1), nsdiffs(-1),
		sfilename(_T("")), sSubdir(_T("")) { }

	CString getLeftFilepath(CString sLeftRoot) const;
	CString getRightFilepath(CString sRightRoot) const;
};

#endif // _DIFF_ITEM_H_