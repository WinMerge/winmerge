/** 
 * @file  DirViewColItems.h
 *
 * @brief Declaration file for DirColInfo
 *
 * @date  Created: 2003-08-19
 */
// RCS ID line follows -- this is updated by CVS
// $Id: DirViewColItems.h 4619 2007-10-14 08:50:20Z jtuc $

#ifndef DirViewColItems_h
#define DirViewColItems_h

#include "UnicodeString.h"

class CDiffContext;

// DirViewColItems typedefs
typedef String (*ColGetFncPtrType)(const CDiffContext *, const void *);
typedef int (*ColSortFncPtrType)(const CDiffContext *, const void *, const void *);


/**
 * @brief Information about one column of dirview list info
 */
struct DirColInfo
{
	enum ColAlign
	{
		ALIGN_LEFT = 0,   // LVCFMT_LEFT
		ALIGN_RIGHT = 1,  // LVCFMT_RIGHT
		ALIGN_CENTER = 2  // LVCFMT_CENTER
	};
	const TCHAR *regName; /**< Internal name used for registry entries etc */
	// localized string resources
	const char *idName; /**< Displayed name, ID of string resource */
	const char *idDesc; /**< Description, ID of string resource */
	ColGetFncPtrType getfnc; /**< Handler giving display string */
	ColSortFncPtrType sortfnc; /**< Handler for sorting this column */
	size_t offset;
	int physicalIndex; /**< Current physical index, -1 if not displayed */
	bool defSortUp; /**< Does column start with ascending sort (most do) */
	int alignment; /**< Column alignment */
};

extern const int g_ncols;
extern const int g_ncols3;

class DirViewColItems
{
public:
	DirViewColItems(int nDirs): m_nDirs(nDirs) {}
	String GetColRegValueNameBase(int col) const;
	int GetColDefaultOrder(int col) const;
	const DirColInfo * GetDirColInfo(int col) const;
	bool IsColById(int col, const char *idname) const;
	bool IsColName(int col) const;
	bool IsColLmTime(int col) const;
	bool IsColMmTime(int col) const;
	bool IsColRmTime(int col) const;
	bool IsColStatus(int col) const;
	bool IsColStatusAbbr(int col) const;
private:
	int m_nDirs;
};

#endif // DirViewColItems_h
