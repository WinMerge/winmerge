/**
 * @file  FilterList.h
 *
 * @brief Declaration file for FilterList.
 */
#pragma once

#include <vector>
#include <memory>
#include <Poco/RegularExpression.h>
#include "unicoder.h"

struct FileFilter;

/**
 * @brief Container for one filtering rule / compiled expression.
 * This structure holds compiled regular expression and a original expression
 * as a string. We need the original expression string in case we want to
 * know which regular expression did match.
 */
struct filter_item
{
	std::string filterAsString; /** Original regular expression string */
	Poco::RegularExpression regexp; /**< Compiled regular expression */
	int _reOpts; /**< Options to set to Poco::RegularExpression */
	filter_item(const std::string &filter, int reOpts) : filterAsString(filter), regexp(filter, reOpts), _reOpts(reOpts) {}
	filter_item(const filter_item* item) : filterAsString(item->filterAsString), regexp(item->filterAsString, item->_reOpts), _reOpts(item->_reOpts) {}
};

typedef std::shared_ptr<filter_item> filter_item_ptr;

/**
 * @brief Regular expression list.
 * This class holds a list of regular expressions for matching strings.
 * The class also provides simple function for matching and remembers the
 * last matched expression.
 */
class FilterList
{
public:
	FilterList();
	~FilterList();
	
	void AddRegExp(const std::string& regularExpression, bool exclude = false, bool throwIfInvalid = false);
	void SetFileFilter(bool dir, std::shared_ptr<FileFilter> fileFilter) { m_isDirFilter = dir; m_fileFilter = fileFilter; }
	void RemoveAllFilters();
	bool HasRegExps() const;
	bool Match(const std::string& string, int codepage = ucr::CP_UTF_8);
	void CloneFrom(const FilterList* filterList);

private:
	std::vector <filter_item_ptr> m_list;
	std::vector <filter_item_ptr> m_listExclude;
	std::shared_ptr<FileFilter> m_fileFilter; /**< Pointer to FileFilter object, if any. */
	bool m_isDirFilter; /**< True if this is a directory filter. */
};

/** 
 * @brief Removes all expressions from the list.
 */
inline void FilterList::RemoveAllFilters()
{
	m_list.clear();
	m_listExclude.clear();
}

/** 
 * @brief Returns if list has any expressions.
 * @return true if list contains one or more expressions.
 */
inline bool FilterList::HasRegExps() const
{
	return !m_list.empty() || !m_listExclude.empty();
}
