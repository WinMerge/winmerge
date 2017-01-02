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
	filter_item(const std::string &filter, int reOpts) : filterAsString(filter), regexp(filter, reOpts) {}
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
	
	void AddRegExp(const std::string& regularExpression);
	void RemoveAllFilters();
	bool HasRegExps() const;
	bool Match(const std::string& string, int codepage = ucr::CP_UTF_8);
	const char * GetLastMatchExpression() const;

private:
	std::vector <filter_item_ptr> m_list;
	const std::string *m_lastMatchExpression;

};
