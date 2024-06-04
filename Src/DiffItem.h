/**
 *  @file DiffItem.h
 *
 *  @brief Declaration of DIFFITEM
 */
#pragma once

#include "DiffFileInfo.h"

// Uncomment this to show debug information in the folder comparison window.
// We don't use _DEBUG since the mapping of the setting (OPT_DIRVIEW_COLUMN_ORDERS or OPT_DIRVIEW3_COLUMN_ORDERS) shifts if this feature is enabled.
//#define SHOW_DIFFITEM_DEBUG_INFO

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
	enum : unsigned long
	{
		// We use extra bits so that no valid values are 0
		// and each set of flags is in a different hex digit
		// to make debugging easier
		// These can always be packed down in the future
		TEXTFLAGS=0x3FU, TEXT=0x1U, BIN=0x2U, BINSIDE1=0x4U, BINSIDE2=0x8U, BINSIDE3=0x10U, IMAGE=0x20U,
		TYPEFLAGS=0xC0U, FILE=0x40U, DIR=0x80U,
		COMPAREFLAGS=0x7000U, NOCMP=0x0000U, DIFF=0x1000U, SAME=0x2000U, CMPERR=0x3000U, CMPABORT=0x4000U,
		COMPAREFLAGS3WAY=0x18000U, DIFFALL=0x0000U, DIFF1STONLY=0x8000U, DIFF2NDONLY=0x10000U, DIFF3RDONLY=0x18000U,
		FILTERFLAGS=0x20000U, INCLUDED=0x00000U, SKIPPED=0x20000U,
		SCANFLAGS=0x100000U, NEEDSCAN=0x100000U,
		THREEWAYFLAGS=0x200000U, THREEWAY=0x200000U,
		SIDEFLAGS=0x70000000U, FIRST=0x10000000U, SECOND=0x20000000U, THIRD=0x40000000U, BOTH=0x30000000U, ALL=0x70000000U,
	};

	unsigned diffcode;

	explicit DIFFCODE(unsigned diffcode = 0) : diffcode(diffcode) { }

protected:
	/// Worker function, to check one area (mask) of code for a particular value (result)
	static bool Check(unsigned code, unsigned mask, unsigned result) { return ((code & mask) == result); }
	/// Convenience function to check the part of the code for comparison results
	static bool CheckCompare(unsigned code, unsigned result) { return Check(code, DIFFCODE::COMPAREFLAGS, result); }
	/// Convenience function to check the part of the code for filter status
	static bool CheckFilter(unsigned code, unsigned result) { return Check(code, DIFFCODE::FILTERFLAGS, result); }
	/// Convenience function to check the part of the code for side status (eg, left-only)
	static bool CheckSide(unsigned code, unsigned result) { return Check(code, DIFFCODE::SIDEFLAGS, result); }

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
	bool isMissingFirstOnly() const { return CheckSide(diffcode, DIFFCODE::SECOND | DIFFCODE::THIRD); }
	bool isMissingSecondOnly() const { return CheckSide(diffcode, DIFFCODE::FIRST | DIFFCODE::THIRD); }
	bool isMissingThirdOnly() const { return CheckSide(diffcode, DIFFCODE::FIRST | DIFFCODE::SECOND); }
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
		return ((diffcode & DIFFCODE::THREEWAY) ? DIFFCODE::ALL : DIFFCODE::BOTH) == (diffcode & DIFFCODE::ALL);
	}

	// compare result
	bool isResultNone() const { return CheckCompare(diffcode, 0); }
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
	bool isImage() const { return (diffcode & DIFFCODE::IMAGE) != 0; }
	// rescan
	bool isScanNeeded() const { return ((diffcode & DIFFCODE::SCANFLAGS) == DIFFCODE::NEEDSCAN); }

	void swap(int idx1, int idx2);
};

enum ViewCustomFlags
{
	// No valid values are 0
	INVALID_CODE = 0,
	VISIBILITY = 0x3U, VISIBLE = 0x1U, HIDDEN = 0x2U, EXPANDED = 0x4U
};

/**
 * @brief information about one file/folder item.
 * This class holds information about one compared "item" in the folder comparison tree.
 * The item can be a file item or folder item.  The item can have data from
 * both (or all three) compare sides (file/folder exists in all sides) or from
 * fewer sides (file/folder is missing in one/two sides).
 *
 * The basic structure consists of linkage to the `parent` folder and possible "sibling"
 * file/folder items that share the identical `parent` folder item.  Folder items also
 * have a `children` link indicating the head of another DIFFITEM list of contained 
 * files/folders.  
 *
 * This class is for backend differences processing, representing physical
 * files and folders. This class is not for GUI data like selection or
 * visibility statuses. So do not include any GUI-dependent data here. 
 */

// See http://web.eecs.utk.edu/~bvz/teaching/cs140Fa09/notes/Dllists for a discussion of
// "doubly linked lists".  The "sibling" linkage used here is basically a doubly linked
// list (similar to what is described in that article), with the notes ...
//		A. Items may be deleted anywhere within the tree (e.g. via "Refresh Selected (ctrl-F5)").
//			This justifies the usage of a double link list.
//		B. Lists are only traversed in the forward direction (using `Flink`).  Using `nullptr`  
//			to stop this traversal is both obvious and convenient.
//		C. Items are only inserted at the end, so storing the end's link (for a future insertion) 
//			into the head's `Blink` serves the function of the "sentinel node" without consuming 
//			extra space.
//		D. A "sentinel" structure is not necessary because ...
//			D1. The head of a sibling list can always be found directly from the parent folder 
//				item by `this->children`.  From an arbitrary sibling item, the head is at
//				`this->parent->children` .
//			D2. Similarily, the end of a sibling list is found (from the parent folder item) at
//				`this->children->Blink` or (from a sibling item) at `this->parent->children->Blink`.
//		E. The "root" item for a DIFFITEM tree is `m_pRoot`, declared in and managed by the
//			DiffItemList class.  There is one "root" for each active folder comparison 
//			(i.e. each folder comparison window in the GUI).  The "root" item exists to anchor the 
//			tree, as well as to guarantee that the `parent` and `children` linkage is correct at the 
//			top folder comparison level.
//
//
// Sometimes a picture is worth many words...
//
// |------P1-------| 
// | "parent item" |  
// | parent -------|---> P0
// | Blink & Flink |
// | children -----|->\
// |---------------|  |
//                    |
//                    |  /--------------------------------------------------------------------------->\
//                    |  |                                                                            |
//                    |  |   |---P1.C0-----|           |---P1.S1-----|           |---P1.S2-----|      |
//                    |  |   | data values |           | data values |           | data values |      | 
//                    |  |   |-------------|           |-------------|           |-------------|      |    
//                    |  \<--|--Blink      |<----------|--Blink      |<----------|--Blink      |<-----/
//                    |      |  Flink -----|---------->|  Flink -----|---------->|  Flink -----|--------> `nullptr`
//                    \----->|  children---|->\        |  children---|->\        |  children---|->\
//            P1<------------|--parent     |  |   P1<--|--parent     |  |   P1<--|--parent     |  |
//                           |-------------|  |        |-------------|  |        |-------------|  | 
//                                            |                         |                         |  
//                                            \--> P1.C0.C0             \--> P1.S1.C0             \--> P1.S2.C0
//                                            \--> `nullptr`            \--> `nullptr`            \--> `nullptr`
//											

class DIFFITEM 
{
//**** DIFFITEM data values

public:
	DiffFileInfo diffFileInfo[3];	/**< Fileinfo for left/middle/right file. */
	int	nsdiffs;					/**< Amount of non-ignored differences */
	int nidiffs;					/**< Amount of ignored differences */
									// Note: Keep `diffFileInfo[]`, `nsdiffs` and `nidiffs`
									//		 near front of class for small offsets.
									// (see `DirColInfo` arrays in `DirViewColItems.cpp`) *>
	DIFFCODE diffcode;				/**< Compare result */
	unsigned customFlags;			/**< ViewCustomFlags flags */

	String getFilepath(int nIndex, const String &sRoot) const;
	String getItemRelativePath() const;

	void Swap(int idx1, int idx2);
	void ClearAllAdditionalProperties();

//**** Child, Parent, Sibling linkage
private:							// Don't allow direct external manipulation of link values
	DIFFITEM *parent;				/**< Parent of current item */
	DIFFITEM *children;				/**< Link to first child of this item */
	DIFFITEM *Flink;				/**< Forward "sibling" link.  The forward linkage ends with
										 a `nullptr` in the final (newest) item. */
	DIFFITEM *Blink;				/**< Backward "sibling" link.  The backward linkage is circular,
										 with the first (oldest) item (pointed to by `this->parent->children`)
										 pointing to the last (newest) item. This is for easy insertion. */
	void AppendSibling(DIFFITEM *p);

public:
	void DelinkFromSiblings();
	void AddChildToParent(DIFFITEM *p);
	void RemoveChildren();
	int GetDepth() const;
	bool IsAncestor(const DIFFITEM *pdi) const;
	std::vector<const DIFFITEM*> GetAncestors() const;
	inline DIFFITEM *GetFwdSiblingLink() const { return Flink; }
	inline DIFFITEM *GetFirstChild() const { return children; }
	inline DIFFITEM *GetParentLink() const { return parent; }
#ifdef SHOW_DIFFITEM_DEBUG_INFO
	inline DIFFITEM *GetBackwardSiblingLink() const { return Blink; }
#endif // SHOW_DIFFITEM_DEBUG_INFO

	/** @brief Return whether the current DIFFITEM has children */
	inline bool HasChildren() const { return (children != nullptr); }
	/** @brief Return whether the current DIFFITEM has children */
	inline bool HasParent() const { return (parent != nullptr); }

//**** The `emptyitem` and its access procedures
private:
	static DIFFITEM emptyitem;		/**< Singleton to represent a DIFFITEM that doesn't have any data */

public:
	static DIFFITEM *GetEmptyItem();
	inline bool isEmpty() const { return this == GetEmptyItem(); /* to invoke "emptiness" checking */ }

//**** CTOR, DTOR
public:
	DIFFITEM() : parent(nullptr), children(nullptr), Flink(nullptr), Blink(nullptr), 
					nidiffs(-1), nsdiffs(-1), customFlags(ViewCustomFlags::INVALID_CODE) 
					// `DiffFileInfo` and `DIFFCODE` have their own initializers. 
					{}
	~DIFFITEM();

};
