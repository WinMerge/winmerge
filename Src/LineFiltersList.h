/** 
 * @file  LineFiltersList.h
 *
 * @brief Declaration file for LineFiltersList class
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _LINEFILTERS_LIST_H_
#define _LINEFILTERS_LIST_H_

#include <vector>

class COptionsMgr;

/**
 @brief Structure for one line filter.
 */
struct LineFilterItem
{
	BOOL enabled; /**< Is filter enabled? */
	String filterStr; /**< Filter string */
	LineFilterItem() : enabled(FALSE) { }
};

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
	String GetAsString() const;
	const LineFilterItem & GetAt(size_t ind) const;
	void CloneFrom(const LineFiltersList *list);
	bool Compare(const LineFiltersList *list) const;

	void Initialize(COptionsMgr *pOptionsMgr);
	void SaveFilters();

	void Import(LPCTSTR filters);

private:
	std::vector<LineFilterItem*> m_items; /**< List for linefilter items */
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};

#endif // _LINEFILTERS_LIST_H_
