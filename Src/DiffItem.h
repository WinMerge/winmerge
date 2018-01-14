/**
 *  @file DiffItem.h
 *
 *  @brief Declaration of DIFFITEM
 */
#pragma once

#include "ListEntry.h"
#include "DiffFileInfo.h"

/**
 * @brief Bitfield values for binary file sides.
 * These values are used as bitfield values when determining which file(s)
 * are binary files. There is no "both" -value since in bitfield "both" is
 * when side1- and side2- bits are set (BINFILE_SIDE1 | BINFILE_SIDE2).
 */
enum BINFILE_SIDE
{
	BINFILE_NONE = 0, /**< No binary files detected. */
	BINFILE_SIDE1, /**< First file was detected as binary file. */
	BINFILE_SIDE2, /**< Second file was detected as binart file. */
};

/**
 * @brief Status of one item comparison, stored as bitfields
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
 */
struct DIFFCODE
{
	/**
	 * @brief values for DIFFITEM.diffcode
	 */
	enum
	{
		// We use extra bits so that no valid values are 0
		// and each set of flags is in a different hex digit
		// to make debugging easier
		// These can always be packed down in the future
		TEXTFLAGS=0x1F, TEXT=0x1, BIN=0x2, BINSIDE1=0x4, BINSIDE2=0x8, BINSIDE3=0x10,
		TYPEFLAGS=0x60, FILE=0x20, DIR=0x40,
		SIDEFLAGS=0x700, FIRST=0x100, SECOND=0x200, THIRD=0x400, BOTH=0x300, ALL=0x700,
		COMPAREFLAGS=0x7000, NOCMP=0x0000, SAME=0x1000, DIFF=0x2000, CMPERR=0x3000, CMPABORT=0x4000,
		COMPAREFLAGS3WAY=0x18000, DIFFALL=0x0000, DIFF1STONLY=0x8000, DIFF2NDONLY=0x10000, DIFF3RDONLY=0x18000,
		FILTERFLAGS=0x20000, INCLUDED=0x00000, SKIPPED=0x20000,
		SCANFLAGS=0x100000, NEEDSCAN=0x100000,
		THREEWAYFLAGS=0x200000, THREEWAY=0x200000,
	};

	unsigned diffcode;

	explicit DIFFCODE(unsigned diffcode = 0) : diffcode(diffcode) { }

protected:
	/// Worker function, to check one area (mask) of code for a particular value (result)
	static bool Check(unsigned code, int mask, int result) { return ((code & mask) == result); }
	/// Convenience function to check the part of the code for comparison results
	static bool CheckCompare(unsigned code, int result) { return Check(code, DIFFCODE::COMPAREFLAGS, result); }
	/// Convenience function to check the part of the code for filter status
	static bool CheckFilter(unsigned code, int result) { return Check(code, DIFFCODE::FILTERFLAGS, result); }
	/// Convenience function to check the part of the code for side status (eg, left-only)
	static bool CheckSide(unsigned code, int result) { return Check(code, DIFFCODE::SIDEFLAGS, result); }

	/// Worker function to set the area indicated by mask to specified result
	void Set(int mask, unsigned result) { diffcode &= (~mask); diffcode |= result; }
	/// Convenience function to set the side status, eg, SetSide(DIFFCODE::LEFT)
	void SetSide(unsigned result) { Set(DIFFCODE::SIDEFLAGS, result); }
public:

	// file/directory
	bool isDirectory() const { return Check(diffcode, DIFFCODE::TYPEFLAGS, DIFFCODE::DIR); }
	// left/right
	bool isSideFirstOnly() const { return CheckSide(diffcode, DIFFCODE::FIRST); }
	bool isSideSecondOnly() const { return CheckSide(diffcode, DIFFCODE::SECOND); }
	bool isSideThirdOnly() const { return CheckSide(diffcode, DIFFCODE::THIRD); }
	bool isSideBoth() const { return CheckSide(diffcode, DIFFCODE::BOTH); }
	bool isSideAll() const { return CheckSide(diffcode, DIFFCODE::ALL); }
	void setSideNone() { SetSide(0); }
	void setSideFlag(int nIndex)
	{
		switch (nIndex)
		{
		case 0: SetSide(diffcode | DIFFCODE::FIRST); return;
		case 1: SetSide(diffcode | DIFFCODE::SECOND); return;
		case 2: SetSide(diffcode | DIFFCODE::THIRD); return;
		}
	}
	void unsetSideFlag(int nIndex)
	{
		switch (nIndex)
		{
		case 0: SetSide(diffcode & ~DIFFCODE::FIRST); return;
		case 1: SetSide(diffcode & ~DIFFCODE::SECOND); return;
		case 2: SetSide(diffcode & ~DIFFCODE::THIRD); return;
		}
	}
	bool isSideOnly(int nIndex) const
	{
		switch (nIndex)
		{
		case 0: return isSideFirstOnly();
		case 1: return isSideSecondOnly();
		case 2: return isSideThirdOnly();
		default: return 0;
		}
	}
	bool existsFirst() const { return !!(diffcode & DIFFCODE::FIRST); }
	bool existsSecond() const { return !!(diffcode & DIFFCODE::SECOND); }
	bool existsThird() const { return !!(diffcode & DIFFCODE::THIRD); }
	bool exists(int nIndex) const
	{
		switch (nIndex)
		{
		case 0: return !!(diffcode & DIFFCODE::FIRST);
		case 1: return !!(diffcode & DIFFCODE::SECOND);
		case 2: return !!(diffcode & DIFFCODE::THIRD);
		default: return 0;
		}
	}
	bool existAll() const
	{
		if ((diffcode & DIFFCODE::THREEWAY) == 0)
			return (existsFirst() && existsSecond());
		else
			return (existsFirst() && existsSecond() && existsThird());
	}

	// compare result
	bool isResultSame() const { return CheckCompare(diffcode, DIFFCODE::SAME); }
	bool isResultDiff() const { return (CheckCompare(diffcode, DIFFCODE::DIFF) && !isResultFiltered() &&
			existAll()); }
	static bool isResultError(unsigned code) { return CheckCompare(code, DIFFCODE::CMPERR); }
	bool isResultError() const { return isResultError(diffcode); }
	static bool isResultAbort(unsigned code) { return CheckCompare(code, DIFFCODE::CMPABORT); }
	bool isResultAbort() const { return isResultAbort(diffcode); }
	// filter status
	bool isResultFiltered() const { return CheckFilter(diffcode, DIFFCODE::SKIPPED); }
	// type
	bool isText() const { return Check(diffcode, DIFFCODE::TEXTFLAGS, DIFFCODE::TEXT); }
	bool isBin() const { return (diffcode & DIFFCODE::BIN) != 0; }
	// rescan
	bool isScanNeeded() const { return ((diffcode & DIFFCODE::SCANFLAGS) == DIFFCODE::NEEDSCAN); }

	void swap(int idx1, int idx2)
	{
		bool e[3] = { false, false, false };
		for (int i = 0; i < 3; ++i)
			e[i] = exists(i);
		std::swap(e[idx1], e[idx2]);
		setSideNone();
		for (int i = 0; i < 3; ++i)
			if (e[i]) setSideFlag(i);
	}
};

/**
 * @brief information about one file/folder item.
 * This class holds information about one compared item in the folder compare.
 * The item can be a file item or folder item. The item can have data from
 * both compare sides (file/folder exists in both sides) or just from one
 * side (file/folder exists only in other side).
 *
 * This class is for backend differences processing, presenting physical
 * files and folders. This class is not for GUI data like selection or
 * visibility statuses. So do not include any GUI-dependent data here. 
 */
struct DIFFITEM : ListEntry
{
	DIFFITEM *parent; /**< Parent of current item */
	ListEntry children; /**< Head of doubly linked list for chldren */

	DiffFileInfo diffFileInfo[3]; /**< Fileinfo for left/middle/right file */
	int	nsdiffs; /**< Amount of non-ignored differences */
	int nidiffs; /**< Amount of ignored differences */
	unsigned customFlags1; /**< Custom flags set 1 */
	DIFFCODE diffcode; /**< Compare result */

	static DIFFITEM emptyitem; /**< singleton to represent a diffitem that doesn't have any data */

	DIFFITEM() : parent(NULL), nidiffs(-1), nsdiffs(-1), customFlags1(0) { }
	~DIFFITEM();

	bool isEmpty() const { return this == &emptyitem; }
	String getFilepath(int nIndex, const String &sRoot) const;
	int GetDepth() const;
	bool IsAncestor(const DIFFITEM *pdi) const;
	bool HasChildren() const;
	void RemoveChildren();
	void Swap(int idx1, int idx2);
};
