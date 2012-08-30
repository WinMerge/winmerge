/**
 * @file  FilterList.h
 *
 * @brief Declaration file for FilterList.
 */
// ID line follows -- this is updated by SVN
// $Id$

#ifndef _FILTERLIST_H_
#define _FILTERLIST_H_

#include <vector>
#include <string>
#include <Poco/RegularExpression.h>

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

/**
 * @brief Regular expression list.
 * This class holds a list of regular expressions for matching strings.
 * The class also provides simple function for matching and remembers the
 * last matched expression.
 */
class FilterList
{
public:
	enum EncodingType
	{
		ENC_ANSI,
		ENC_UTF8,
	};
	
	FilterList();
	~FilterList();
	
	void AddRegExp(const std::string& regularExpression, EncodingType encoding);
	void RemoveAllFilters();
	bool HasRegExps();
	bool Match(const std::string& string, int codepage = CP_UTF8);
	const char * GetLastMatchExpression();

private:
	std::vector <filter_item *> m_list;
	const std::string *m_lastMatchExpression;

};


#endif // _FILTERLIST_H_
