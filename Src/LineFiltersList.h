/** 
 * @file  LineFiltersList.h
 *
 * @brief Declaration file for LineFiltersList class
 */
// ID line follows -- this is updated by SVN
// $Id: SyntaxColors.h 3126 2006-03-04 02:36:46Z elsapo $

#ifndef _LINEFILTERS_LIST_H_
#define _LINEFILTERS_LIST_H_

#include "stdafx.h"

class COptionsMgr;

/**
 @brief Structure for one line filter.
 */
struct LineFilterItem
{
	BOOL enabled; /**< Is filter enabled? */
	TCHAR * filterStr; /**< Filter string */
	LineFilterItem() : enabled(FALSE), filterStr(NULL) { }
};

typedef CList<LineFilterItem, const LineFilterItem&> LineFilterItems;

/**
 @brief List of line filters.
 */
class LineFiltersList
{
public:
	LineFiltersList();
	~LineFiltersList();

	void AddFilter(LPCTSTR filter, BOOL enabled);
	int GetCount() const;
	void Empty();
	CString GetAsString() const;
	LineFilterItem GetAt(int ind);
	void CloneFrom(LineFiltersList *list);

	void Initialize(COptionsMgr *pOptionsMgr);
	void SaveFilters();

private:
	LineFilterItems m_items; /**< List for linefilter items */
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};

#endif // _LINEFILTERS_LIST_H_

